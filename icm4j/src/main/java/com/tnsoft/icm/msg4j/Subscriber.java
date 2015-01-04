package com.tnsoft.icm.msg4j;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import com.tnsoft.icm.icm4j.protocol.MessageHead;
import com.tnsoft.icm.msg4j.protocol.Event;
import com.tnsoft.icm.msg4j.protocol.Msg;
import com.tnsoft.icm.msg4j.protocol.MsgResponse;
import com.tnsoft.icm.msg4j.protocol.Subscribe;
import com.tnsoft.icm.util.BufferFactory;

public final class Subscriber extends MsgClient implements Runnable {

	private Map<String, EventAction<?>> actionMap = new ConcurrentHashMap<String, EventAction<?>>();
	private Set<String> subscribedTopicIdSet = new HashSet<String>();
	private final ReadWriteLock lock = new ReentrantReadWriteLock();
	private final Lock rl = lock.readLock();
	private final Lock wl = lock.writeLock();
	private boolean running;
	private Object stop_lock = new Object();
	private Selector selector;
	private Thread selectorThread;

	public Subscriber(String hostname, int port) {
		super(hostname, port);
	}
	
	public Subscriber(String hostname) {
		this(hostname, 10002);
	}
	
	private boolean addTopicId(String topicId) {
		wl.lock();
		try {
			return subscribedTopicIdSet.add(topicId);
		} finally {
			wl.unlock();
		}
	}

	private boolean removeTopicId(String topicId) {
		wl.lock();
		try {
			return subscribedTopicIdSet.remove(topicId);
		} finally {
			wl.unlock();
		}
	}

	private boolean containsTopicId(String topicId) {
		rl.lock();
		try {
			return subscribedTopicIdSet.contains(topicId);
		} finally {
			rl.unlock();
		}
	}

	private void clearTopicIds() {
		wl.lock();
		try {
			subscribedTopicIdSet.clear();
		} finally {
			wl.unlock();
		}
	}

	public boolean isRunning() {
		return running;
	}

	private void openAndConnect() throws IOException {
		selector = Selector.open();
		socketChannel = SocketChannel.open();
		socketChannel.configureBlocking(false);
		socketChannel.register(selector, SelectionKey.OP_READ);
		while (!socketChannel.isConnected()) {
			if (socketChannel.isConnectionPending()) {
				socketChannel.finishConnect();
			} else {
				socketChannel.connect(new InetSocketAddress(hostname, port));
			}
		}
	}

	public synchronized void start() {
		if (!isRunning()) {
			log.info("Starting the subscriber...");
			this.running = true;
			try {
				openAndConnect();
				selectorThread = new Thread(this);
				selectorThread.start();
				log.info("Starting the subscriber succeed.");
			} catch (IOException e) {
				log.error(e);
				throw new RuntimeException(e);
			}
		}
	}

	public synchronized void stop() {
		if (isRunning()) {
			log.info("Stopping the subscriber...");
			synchronized (stop_lock) {
				this.running = false;
				try {
					log.debug("Closing the socket channel %s...", socketChannel.toString());
					socketChannel.close();
					stop_lock.wait();
					log.debug("Closing the selector...");
					selector.close();
				} catch (IOException e) {
					log.error(e);
				} catch (InterruptedException e) {
					log.error(e);
				}
			}
			log.info("The subscriber stoped.");
		}
	}

	public void run() {
		while (running) {
			try {
				selector.select();
				for (Iterator<SelectionKey> iter = selector.selectedKeys().iterator(); iter.hasNext();) {
					SelectionKey key = iter.next();
					iter.remove();
					if (key.isValid()) {
						if (key.isReadable()) {
							read(key);
						}
					}
				}
			} catch (IOException e) {
				log.error(e);
				try {
					if (running) {
						if (socketChannel.isOpen()) {
							log.debug("Closing the channel and try to reconnect the Server...");
							socketChannel.close();
						}
						log.debug("Closing the selector...");
						selector.close();
						log.info("Trying to reconnect once...");
						openAndConnect();
						log.info("Reconecting succeed, subscribing the subscribed topics");
						clearTopicIds();
						for (Iterator<Entry<String, EventAction<?>>> iterator = actionMap.entrySet().iterator(); iterator
								.hasNext();) {
							Entry<String, EventAction<?>> entry = iterator.next();
							Subscribe subscribe = new Subscribe(true, entry.getKey());
							subscribe.writeBuffer(writer);
							socketChannel.write(subscribe.getBuffer());
						}
						log.info("Reconnecting all finished.");
					}
				} catch (IOException e1) {
					log.error(e1);
					running = false;
					return;
				}
			}
		}
		synchronized (stop_lock) {
			stop_lock.notifyAll();
		}
	}

	@SuppressWarnings({ "unchecked", "rawtypes" })
	private void read(SelectionKey key) throws IOException {
		ByteBuffer buffer = BufferFactory.getInstance().getBuffer(MessageHead.MESSAGE_HEAD_SIZE);
		channelRead(socketChannel, buffer);
		buffer.flip();
		int size = reader.readInt(buffer);
		int type = reader.readByte(buffer);
		buffer = BufferFactory.getInstance().getBuffer(size);
		channelRead(socketChannel, buffer);
		buffer.flip();
		EventAction action = null;
		switch (type) {
		case Msg.TYPE_OK:
			MsgResponse response = new MsgResponse(buffer);
			response.readTopicId(reader);
			if (addTopicId(response.getTopicId())) {
				// is subscribing
				action = actionMap.get(response.getTopicId());
				log.info("Subscribing topic with ID [%s] succeed!", response.getTopicId());
			} else {
				// is canceling subscribing
				removeTopicId(response.getTopicId());
				action = actionMap.remove(response.getTopicId());
				log.info("Unsubscribing topic with ID [%d] succeed!", response.getTopicId());
			}
			if (action == null) {
				// this line probability can't be reached
				log.error("The event with topicId [%d] has not an EventAction to do the specific logic!",
						response.getTopicId());
			} else {
				synchronized (action) {
					action.notify();
				}
			}
			break;
		case Msg.TYPE_EVENT:
			Event event = new Event(buffer);
			event.readTopicId(reader);
			if (containsTopicId(event.getTopicId())) {
				action = actionMap.get(event.getTopicId());
				if (action == null) {
					// this line probability can't be reached
					log.error(
							"The event with topicId [%d] has been subscribed but has not an EventAction to do the specific logic!",
							event.getTopicId());
				} else {
					synchronized (action) {
						event.readEvent(action.getEventClass(), reader);
						action.execute(event.getTopicId(), event.getEvent());
					}
				}
			} else {
				log.warn("The event with topicId [%d] hasn't been subscribed or subscribed failed!", event.getTopicId());
			}
			break;
		case Msg.TYPE_UNKNOW:
		default:
			log.error("Get an unknown response type.");
			break;
		}
	}

	public boolean subscribe(String topicId, EventAction<?> action) throws IOException {
		if (containsTopicId(topicId)) {
			return true;
		}
		log.info("Subscribe a topic with ID [%s].", topicId);
		actionMap.put(topicId, action);
		synchronized (action) {
			Subscribe subscribe = new Subscribe(topicId);
			subscribe.writeBuffer(writer);
			socketChannel.write(subscribe.getBuffer());
			try {
				action.wait();
			} catch (InterruptedException e) {
				log.error(e);
			}
		}
		return containsTopicId(topicId);
	}

	public boolean unsubscribe(String topicId) throws IOException {
		if (!containsTopicId(topicId)) {
			return true;
		}
		log.info("Unsubscribe a topic with ID [%s].", topicId);
		Subscribe subscribe = new Subscribe(false, topicId);
		subscribe.writeBuffer(writer);
		EventAction<?> action = actionMap.get(topicId);
		synchronized (action) {
			socketChannel.write(subscribe.getBuffer());
			try {
				action.wait();
			} catch (InterruptedException e) {
				log.error(e);
			}
		}
		return !containsTopicId(topicId);
	}

}
