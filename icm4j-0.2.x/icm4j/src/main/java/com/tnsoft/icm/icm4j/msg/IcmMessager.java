package com.tnsoft.icm.icm4j.msg;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.icm4j.BufferProvider;
import com.tnsoft.icm.icm4j.BufferReaderProvider;
import com.tnsoft.icm.icm4j.BufferWriterProvider;
import com.tnsoft.icm.icm4j.CachedBufferProvider;
import com.tnsoft.icm.icm4j.CachedBufferReaderProvider;
import com.tnsoft.icm.icm4j.CachedBufferWriterProvider;
import com.tnsoft.icm.icm4j.ChannelContext;
import com.tnsoft.icm.icm4j.IcmContext;
import com.tnsoft.icm.icm4j.annotation.Name;
import com.tnsoft.icm.icm4j.io.Buffer;
import com.tnsoft.icm.icm4j.io.BufferReader;
import com.tnsoft.icm.icm4j.io.BufferWriter;
import com.tnsoft.icm.icm4j.msg.protocol.MsgProtocol;
import com.tnsoft.icm.icm4j.msg.protocol.Topic;

public class IcmMessager implements MsgProtocol {

	private static final Log log = LogFactory.getLog(IcmMessager.class);

	public static final int PUBLISHER = 1;
	public static final int SUBSCRIBER = 2;

	private Selector selector;
	private Map<Integer, ChannelContext> channelContextMap;
	private Thread selectorThread;
	private ExecutorService dispatcherExecutor;
	private ExecutorService eventExecutor;
	private int roles;
	private boolean running;
	private String hostname;
	private int publishPort;
	private int subscribePort;
	private ByteOrder byteOrder;
	private Charset charset;
	private BufferProvider bufferProvider;
	private BufferReaderProvider bufferReaderProvider;
	private BufferWriterProvider bufferWriterProvider;
	private final EventDispatcher eventDispatcher = new EventDispatcher();
	private ConcurrentMap<Topic, Boolean> publishedTopics = new ConcurrentHashMap<Topic, Boolean>();
	private ConcurrentMap<Topic, Boolean> subscribedTopics = new ConcurrentHashMap<Topic, Boolean>();

	public void setRoles(int roles) {
		if (!running) {
			this.roles = roles;
		}
	}

	public int getRoles() {
		return roles;
	}

	public boolean isPublisher() {
		return (roles & PUBLISHER) != 0;
	}

	public boolean isSubscriber() {
		return (roles & SUBSCRIBER) != 0;
	}

	public boolean isRunning() {
		return running;
	}

	public void setHostname(String hostname) {
		if (!running) {
			this.hostname = hostname;
		}
	}

	public void setSubscribePort(int subscribePort) {
		if (!running && isSubscriber()) {
			this.subscribePort = subscribePort;
		}
	}

	public void setPublishPort(int publishPort) {
		if (!running && isPublisher()) {
			this.publishPort = publishPort;
		}
	}

	public void setByteOrder(ByteOrder byteOrder) {
		if (!running) {
			this.byteOrder = byteOrder;
		}
	}

	public void setCharset(Charset charset) {
		if (!running) {
			this.charset = charset;
		}
	}

	public void setBufferProvider(BufferProvider bufferProvider) {
		if (!running) {
			this.bufferProvider = bufferProvider;
		}
	}

	public void setBufferReaderProvider(BufferReaderProvider bufferReaderProvider) {
		if (!running) {
			this.bufferReaderProvider = bufferReaderProvider;
		}
	}

	public void setBufferWriterProvider(BufferWriterProvider bufferWriterProvider) {
		if (!running) {
			this.bufferWriterProvider = bufferWriterProvider;
		}
	}

	public EventDispatcher getEventDispatcher() {
		return eventDispatcher;
	}

	public void setEventExecutor(ExecutorService eventExecutor) {
		if (!running) {
			this.eventExecutor = eventExecutor;
		}
	}

	public synchronized void startup() {
		if (!running) {
			running = true;
			init();
			selectorThread = new Thread(new SelecorTask(), "selector thread");
			try {
				selector = Selector.open();
				channelContextMap = new HashMap<Integer, ChannelContext>(2);
				if (isPublisher()) {
					SocketChannel channel = newSocketChannel(hostname, publishPort, false);
					ChannelContext channelContext = new ChannelContext();
					channelContext.setHostname(hostname);
					channelContext.setRemotePort(publishPort);
					channelContext.setContextQueue(new LinkedBlockingQueue<IcmContext>());
					channel.register(selector, SelectionKey.OP_READ, channelContext);
					channelContextMap.put(publishPort, channelContext);
				}
				if (isSubscriber()) {
					if (publishPort != subscribePort) {
						SocketChannel channel = newSocketChannel(hostname, subscribePort, false);
						ChannelContext channelContext = new ChannelContext();
						channelContext.setHostname(hostname);
						channelContext.setRemotePort(subscribePort);
						channel.register(selector, SelectionKey.OP_READ, channelContext);
						channelContextMap.put(subscribePort, channelContext);
					}
				}
			} catch (IOException e) {
				throw new MsgException(e);
			}
			dispatcherExecutor = Executors.newSingleThreadExecutor();
			selectorThread.start();
		}
	}

	private SocketChannel newSocketChannel(String hostname, int port, boolean blocking) throws IOException {
		SocketChannel channel = SocketChannel.open(new InetSocketAddress(hostname, port));
		channel.configureBlocking(blocking);
		channel.finishConnect();
		return channel;
	}

	private void init() {
		if (getRoles() == 0) {
			throw new MsgException("The Messager must has a role!");
		}
		if (byteOrder == null) {
			setByteOrder(ByteOrder.BIG_ENDIAN);
		}
		if (charset == null) {
			setCharset(Charset.forName("UTF-8"));
		}
		if (bufferProvider == null) {
			setBufferProvider(new CachedBufferProvider(10, byteOrder));
		}
		if (bufferReaderProvider == null) {
			setBufferReaderProvider(new CachedBufferReaderProvider(10));
		}
		if (bufferWriterProvider == null) {
			setBufferWriterProvider(new CachedBufferWriterProvider(10));
		}
		if (eventExecutor == null) {
			setEventExecutor(Executors.newCachedThreadPool());
		}
	}

	public synchronized void shutdown() {
		if (running) {
			running = false;
			selector.wakeup();
		}
	}

	public void publish(Topic topic) {
		if (running && isPublisher()) {
			if (publishedTopics.putIfAbsent(topic, false) == null) {
				Buffer buffer = bufferProvider.getBuffer();
				BufferWriter writer = bufferWriterProvider.getWriter(buffer, charset);
				writer.writeInt(0);
				writer.writeByte(TYPE_PUBLISH);
				writer.writeObject(topic, Topic.class);
				int p = buffer.position();
				buffer.position(0);
				writer.writeInt(p - MESSAGE_HEAD_SIZE);
				buffer.position(p);
				writer.close();
				buffer.flip();
				IcmContext context = new IcmContext();
				context.setBuffer(buffer);
				ChannelContext channelContext = channelContextMap.get(publishPort);
				channelContext.getContextQueue().offer(context);
				SocketChannel channel = (SocketChannel) channelContext.getChannel();
				if (channel != null) {
					SelectionKey key = channel.keyFor(selector);
					key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
					selector.wakeup();
				}
			}
		}
	}

	private boolean isPublished(Topic topic) {
		Boolean is_published = publishedTopics.get(topic);
		return is_published != null && is_published;
	}

	public void publish(Topic topic, Object event) {
		if (running && isPublisher() && isPublished(topic)) {
			Buffer buffer = bufferProvider.getBuffer();
			BufferWriter writer = bufferWriterProvider.getWriter(buffer, charset);
			writer.writeInt(0);
			writer.writeByte(TYPE_EVENT);
			writer.writeObject(topic, Topic.class);
			Class<?> clazz = event.getClass();
			String name = null;
			Name aname = clazz.getAnnotation(Name.class);
			if (aname != null && aname.value().length() > 0) {
				name = aname.value();
			} else {
				name = clazz.getSimpleName();
			}
			writer.writeString(name);
			writer.writeObject(event, clazz);
			int p = buffer.position();
			buffer.position(0);
			writer.writeInt(p - MESSAGE_HEAD_SIZE);
			buffer.position(p);
			writer.close();
			buffer.flip();
			IcmContext context = new IcmContext();
			context.setBuffer(buffer);
			ChannelContext channelContext = channelContextMap.get(publishPort);
			channelContext.getContextQueue().offer(context);
			SocketChannel channel = (SocketChannel) channelContext.getChannel();
			if (channel != null) {
				SelectionKey key = channel.keyFor(selector);
				key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
				selector.wakeup();
			}
		}
	}

	public void subscribe(Topic topic, EventAction<?> action) {
		if (running && isSubscriber()) {
			getEventDispatcher().putAction(topic, action);
			if (subscribedTopics.putIfAbsent(topic, false) == null) {
				Buffer buffer = bufferProvider.getBuffer();
				BufferWriter writer = bufferWriterProvider.getWriter(buffer, charset);
				writer.writeInt(0);
				writer.writeByte(TYPE_SUBSCRIBE);
				writer.writeObject(topic, Topic.class);
				int p = buffer.position();
				buffer.position(0);
				writer.writeInt(p - MESSAGE_HEAD_SIZE);
				buffer.position(p);
				writer.close();
				buffer.flip();
				IcmContext context = new IcmContext();
				context.setBuffer(buffer);
				ChannelContext channelContext = channelContextMap.get(subscribePort);
				channelContext.getContextQueue().offer(context);
				SocketChannel channel = (SocketChannel) channelContext.getChannel();
				if (channel != null) {
					SelectionKey key = channel.keyFor(selector);
					key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
					selector.wakeup();
				}
			}
		}
	}

	public void unpublish(Topic topic) {
		if (running && isPublisher()) {
			publishedTopics.remove(topic);
		}
	}

	public void unsubscribe(Topic topic) {
		if (running && isSubscriber()) {
			subscribedTopics.remove(topic);
		}
	}

	private class SelecorTask implements Runnable {

		private ByteBuffer byteBuffer = ByteBuffer.allocate(4 * 1024);
		private final long timeout = 30 * 1000;
		private final Map<Integer, ChannelContext> toRecoverChannelContextMap = new HashMap<Integer, ChannelContext>(2);
		
		private void recoverConnections() {
			Iterator<Entry<Integer, ChannelContext>> entryIterator = toRecoverChannelContextMap.entrySet().iterator();
			while (entryIterator.hasNext()) {
				Entry<Integer, ChannelContext> entry = entryIterator.next();
				int port = entry.getKey();
				ChannelContext channelContext = entry.getValue();
				try {
					SocketChannel channel = newSocketChannel(hostname, port, false);
					BlockingQueue<IcmContext> queue = channelContext.getContextQueue();
					channelContext.setContextQueue(new LinkedBlockingQueue<IcmContext>());
					if (port == publishPort) {
						for (Entry<Topic, Boolean> pe : publishedTopics.entrySet()) {
							resetTopic(channelContext, pe, TYPE_PUBLISH);
						}
					}
					if (port == subscribePort) {
						for (Entry<Topic, Boolean> se : subscribedTopics.entrySet()) {
							resetTopic(channelContext, se, TYPE_SUBSCRIBE);
						}
					}
					channelContext.getContextQueue().addAll(queue);
					channel.register(selector, SelectionKey.OP_WRITE | SelectionKey.OP_READ, channelContext);
					entryIterator.remove();
				} catch (IOException e) {
					log.error("Recover connection to " + hostname + ":" + port + " failed!", e);
				}
			}
		}

		private void resetTopic(ChannelContext channelContext, Entry<Topic, Boolean> pe, byte type) {
			if (pe.getValue()) {
				Buffer buffer = bufferProvider.getBuffer();
				BufferWriter writer = bufferWriterProvider.getWriter(buffer, charset);
				writer.writeInt(0);
				writer.writeByte(type);
				writer.writeObject(pe.getKey(), Topic.class);
				int p = buffer.position();
				buffer.position(0);
				writer.writeInt(p - MESSAGE_HEAD_SIZE);
				buffer.position(p);
				writer.close();
				buffer.flip();
				IcmContext context = new IcmContext();
				context.setBuffer(buffer);
				channelContext.getContextQueue().offer(context);
			}
		}

		public void run() {
			while (running) {
				try {
					recoverConnections();
					if (selector.select(timeout) > 0) {
						Set<SelectionKey> keySet = selector.selectedKeys();
						for (Iterator<SelectionKey> iterator = keySet.iterator(); iterator.hasNext();) {
							SelectionKey key = iterator.next();
							iterator.remove();
							if (key.isValid() && key.isReadable()) {
								read(key);
							}
							if (key.isValid() && key.isWritable()) {
								write(key);
							}
						}
					}
				} catch (IOException e) {
					log.error(e, e);
				}
			}
			try {
				selector.close();
			} catch (IOException e) {
				log.error(e, e);
			}
			dispatcherExecutor.shutdown();
			eventExecutor.shutdown();
		}

		private void read(SelectionKey key) {
			SocketChannel channel = (SocketChannel) key.channel();
			ChannelContext channelContext = (ChannelContext) key.attachment();
			IcmContext context = channelContext.getResponseContext();
			try {
				if (context == null) {
					byteBuffer.clear();
					byteBuffer.limit(MESSAGE_HEAD_SIZE);
					while (byteBuffer.hasRemaining()) {
						if (channel.read(byteBuffer) == -1) {
							throw new IOException("The remote host has closed the connection!");
						}
					}
					byteBuffer.flip();
					Buffer buffer = bufferProvider.getBuffer();
					buffer.put(byteBuffer);
					buffer.flip();
					BufferReader reader = bufferReaderProvider.getReader(buffer, charset);
					int size = reader.readInt();
					byte type = reader.readByte();
					reader.close();
					context = new IcmContext();
					channelContext.setResponseContext(context);
					context.setLength(size);
					context.setCurrent(0);
					context.setType(type);
					context.setBuffer(buffer.clear());
				}
				int length = context.getLength();
				int current = context.getCurrent();
				int remaining = length - current;
				if (remaining < byteBuffer.capacity()) {
					byteBuffer.limit(remaining);
				}
				int readed = channel.read(byteBuffer);
				byteBuffer.flip();
				Buffer buffer = context.getBuffer();
				buffer.put(byteBuffer);
				current += readed;
				context.setCurrent(current);
				if (current == length) { // end
					channelContext.removeResponseContext();
					dispatcherExecutor.execute(new ResponseTask(context));
				}
			} catch (IOException e) {
				try {
					channel.close();
				} catch (IOException e1) {}
				key.cancel();
				log.error(e, e);
				channelContext.removeResponseContext();
				channelContext.removeRequestContext();
				channelContext.removeChannel();
				toRecoverChannelContextMap.put(channelContext.getRemotePort(), channelContext);
			}
		}

		private void write(SelectionKey key) {
			SocketChannel channel = (SocketChannel) key.channel();
			ChannelContext channelContext = (ChannelContext) key.attachment();
			IcmContext context = channelContext.getRequestContext();
			BlockingQueue<IcmContext> queue = channelContext.getContextQueue();
			if (context == null) {
				if (queue.isEmpty()) {
					key.interestOps(SelectionKey.OP_READ);
					if (queue.isEmpty()) {
						return;
					} else {
						key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
					}
				}
				context = queue.peek();
				context.getBuffer().mark();
			}
			byteBuffer.clear();
			Buffer buffer = context.getBuffer();
			buffer.get(byteBuffer);
			byteBuffer.flip();
			try {
				while (byteBuffer.hasRemaining()) {
					channel.write(byteBuffer);
				}
				if (!buffer.hasRemaining()) { // end
					bufferProvider.destroy(buffer);
					queue.remove();
					if (queue.isEmpty()) {
						key.interestOps(SelectionKey.OP_READ);
						if (!queue.isEmpty()) {
							key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
						}
					}
				}
			} catch (IOException e) {
				try {
					channel.close();
				} catch (IOException e1) {}
				key.cancel();
				log.error(e, e);
				channelContext.removeResponseContext();
				channelContext.removeRequestContext();
				channelContext.removeChannel();
				toRecoverChannelContextMap.put(channelContext.getRemotePort(), channelContext);
			}
		}
	}

	private class ResponseTask implements Runnable {

		private IcmContext context;

		public ResponseTask(IcmContext context) {
			this.context = context;
		}

		public void run() {
			Buffer buffer = context.getBuffer();
			BufferReader reader = bufferReaderProvider.getReader(buffer, charset);
			Topic topic = null;
			switch (context.getType()) {
			case TYPE_EVENT:
				topic = (Topic) reader.readObject(Topic.class);
				try {
					eventDispatcher.dispatch(topic, buffer);
				} catch (UnsupportedTopicException e) {
					log.error(e);
				}
				break;
			case TYPE_OK:
				topic = (Topic) reader.readObject(Topic.class);
				if (log.isDebugEnabled()) {
					log.debug("Topic [" + topic.getTopicId() + "] OK.");
				}
				if (isPublisher()) {
					publishedTopics.replace(topic, true);
				}
				if (isSubscriber()) {
					subscribedTopics.replace(topic, true);
				}
				break;
			case TYPE_UNKNOW:
				log.warn("Unknown type !");
				break;
			default:
				log.warn("Unsupported type [" + context.getType() + "] !");
				break;
			}
			reader.close();
			bufferProvider.destroy(buffer);
		}
	}

	public class EventDispatcher {

		private ConcurrentMap<Topic, ConcurrentMap<String, EventAction<?>>> topicEventActions = new ConcurrentHashMap<Topic, ConcurrentMap<String, EventAction<?>>>();

		public void putAction(Topic topic, EventAction<?> action) {
			ConcurrentMap<String, EventAction<?>> topicMap = topicEventActions.get(topic);
			if (topicMap == null) {
				topicMap = topicEventActions.putIfAbsent(topic, new ConcurrentHashMap<String, EventAction<?>>());
			}
			topicMap.put(action.getName(), action);
		}

		@SuppressWarnings("rawtypes")
		public void dispatch(final Topic topic, final Buffer eventBuffer) throws UnsupportedTopicException {
			ConcurrentMap<String, EventAction<?>> topicMap = topicEventActions.get(topic);
			if (topicMap == null) {
				throw new UnsupportedTopicException("TopicId=" + topic.getTopicId() + "");
			}
			BufferReader reader = bufferReaderProvider.getReader(eventBuffer, charset);
			String name = reader.readString();
			final EventAction action = topicMap.get(name);
			if (action != null) {
				final Object event = reader.readObject(action.getEventType());
				eventExecutor.execute(new Runnable() {
					@SuppressWarnings("unchecked")
					public void run() {
						action.execute(topic, event);
					}
				});
			}
			reader.close();
		}
	}

}
