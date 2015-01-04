package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Proxy;
import java.lang.reflect.Type;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.rmi.RemoteException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.icm4j.annotation.Name;
import com.tnsoft.icm.icm4j.annotation.Operation;
import com.tnsoft.icm.icm4j.io.Buffer;
import com.tnsoft.icm.icm4j.io.BufferReader;
import com.tnsoft.icm.icm4j.io.BufferWriter;
import com.tnsoft.icm.icm4j.protocol.IcmProtocol;
import com.tnsoft.icm.icm4j.protocol.Out;
import com.tnsoft.icm.icm4j.protocol.RequestHead;
import com.tnsoft.icm.icm4j.protocol.ResponseHead;

public class IcmMutiClient implements IcmProtocol, IcmRemoteProxy {

	private static final Log log = LogFactory.getLog(IcmMutiClient.class);

	private static final int BUFFER_SIZE = 4 * 1024;

	private Set<IcmRemoteService> remoteServices = new HashSet<IcmRemoteService>();
	private Selector selector;
	private boolean running;
	private Thread selectorThread;
	private Thread queueThread;
	private BlockingQueue<IcmContext> requestQueue;
	private final AtomicInteger identity = new AtomicInteger(0);
	private final ByteBuffer byteBuffer = ByteBuffer.allocate(BUFFER_SIZE);
	private final Map<String, BlockingQueue<SocketChannel>> channelMap = new HashMap<String, BlockingQueue<SocketChannel>>();
	private final Map<Method, RequestHead> requestHeadMap = new HashMap<Method, RequestHead>();

	public boolean isRunning() {
		return running;
	}

	public synchronized void addRemoteService(IcmRemoteService remoteService) {
		if (isRunning()) {
			throw new IcmException("You can't add a new IcmRemoteService when the client is running!");
		}
		if (remoteServices.add(remoteService)) {
			if (remoteService.getRemoteProxy() == null) {
				remoteService.setRemoteProxy(this);
			}
		}
	}

	public synchronized void startup() {
		if (!running) {
			running = true;
			selectorThread = new Thread(new SelectorTask(), "selector thread");
			queueThread = new Thread(new QueueTask(), "queue thread");
			requestQueue = new LinkedBlockingQueue<IcmContext>();
			try {
				selector = Selector.open();
			} catch (IOException e) {
				throw new IcmException(e);
			}
			for (IcmRemoteService remoteService : remoteServices) {
				if (remoteService.isLongConnectionMode()) {
					int count = remoteService.getConnectionCount();
					if (count <= 0) {
						count = 1;
					}
					BlockingQueue<SocketChannel> scq = new LinkedBlockingQueue<SocketChannel>(count);
					channelMap.put(generateKey(remoteService.getHostname(), remoteService.getPort()), scq);
					newConnection(remoteService);
				}
			}
			selectorThread.start();
			queueThread.start();
		}
	}

	public synchronized void shutdown() {
		if (running) {
			running = false;
			requestQueue.offer(new IcmContext());
			selector.wakeup();
		}
	}

	@SuppressWarnings("unchecked")
	public synchronized <T> T getService(IcmRemoteService remoteService, Class<T> interfaceClass) {
		if (!running) {
			startup();
		}
		String name = null;
		Name aname = interfaceClass.getAnnotation(Name.class);
		if (aname == null || aname.value() == "") {
			name = interfaceClass.getSimpleName();
		} else {
			name = aname.value();
		}
		String category = ""; // TODO ?
		Method[] methods = interfaceClass.getMethods();
		for (Method method : methods) {
			String operation = null;
			Operation aoperation = method.getAnnotation(Operation.class);
			if (aoperation == null || aoperation.value() == "") {
				operation = method.getName();
			} else {
				operation = aoperation.value();
			}
			RequestHead requestHead = new RequestHead();
			requestHead.setName(name);
			requestHead.setCategory(category);
			requestHead.setOperation(operation);
			boolean hasResponse = false;
			for (Class<?> type : method.getParameterTypes()) {
				if (type == Out.class) {
					hasResponse = true;
					break;
				}
			}
			if (!hasResponse && method.getReturnType() != void.class) {
				hasResponse = true;
			}
			requestHead.setResponseFlag(hasResponse ? HAS_RESPONSE : NO_RESPONSE);
			requestHeadMap.put(method, requestHead);
		}
		return (T) Proxy.newProxyInstance(interfaceClass.getClassLoader(), new Class[] { interfaceClass },
				new IcmInvocationHandler(remoteService));
	}

	public boolean contains(IcmRemoteService remoteService) {
		return remoteServices.contains(remoteService);
	}

	private String generateKey(String hostname, int port) {
		return hostname + ":" + port;
	}

	private SocketChannel newConnection(String hostname, int port, boolean blocking) throws IOException {
		SocketChannel socketChannel = SocketChannel.open(new InetSocketAddress(hostname, port));
		socketChannel.configureBlocking(blocking);
		socketChannel.finishConnect();
		return socketChannel;
	}

	private void newConnection(String hostname, int port, ChannelContext channelContext) {
		BlockingQueue<SocketChannel> queue = channelMap.get(generateKey(hostname, port));
		if (queue != null) {
			try {
				SocketChannel socketChannel = newConnection(hostname, port, false);
				socketChannel.register(selector, SelectionKey.OP_READ, channelContext);
				queue.offer(socketChannel);
			} catch (IOException e) {
				log.error(e, e);
			}
		}
	}

	private void newConnection(IcmRemoteService remoteService) {
		ChannelContext channelContext = new ChannelContext();
		channelContext.setContextQueue(new LinkedBlockingQueue<IcmContext>());
		channelContext.setBufferProvider(remoteService.getBufferProvider());
		channelContext.setBufferReaderProvider(remoteService.getBufferReaderProvider());
		channelContext.setBufferWriterProvider(remoteService.getBufferWriterProvider());
		channelContext.setCharset(remoteService.getCharset());
		channelContext.setHostname(remoteService.getHostname());
		channelContext.setRemotePort(remoteService.getPort());
		channelContext.setResponseWaitedContextMap(new ConcurrentHashMap<Integer, IcmContext>());
		newConnection(remoteService.getHostname(), remoteService.getPort(), channelContext);
	}
	
	private RequestHead generateRequestHead(Method method) {
		RequestHead source = requestHeadMap.get(method);
		if (source == null) {
			return null;
		}
		RequestHead requestHead = new RequestHead();
		requestHead.setId(identity.getAndIncrement());
		requestHead.setResponseFlag(source.getResponseFlag());
		requestHead.setName(source.getName());
		requestHead.setCategory(source.getCategory());
		requestHead.setOperation(source.getOperation());
		return requestHead;
	}


	private class SelectorTask implements Runnable {
		
		private final long timeout = 30 * 1000;
		private final Set<ChannelContext> toRecoverChannelContextSet = new HashSet<ChannelContext>();
		
		private void recoverConnections() {
			Iterator<ChannelContext> iterator = toRecoverChannelContextSet.iterator();
			while (iterator.hasNext()) {
				ChannelContext channelContext = iterator.next();
				String hostname = channelContext.getHostname();
				int port = channelContext.getRemotePort();
				BlockingQueue<SocketChannel> queue = channelMap.get(generateKey(hostname, port));
				if (queue != null) {
					try {
						SocketChannel socketChannel = newConnection(hostname, port, false);
						socketChannel.register(selector, SelectionKey.OP_READ, channelContext);
						queue.offer(socketChannel);
						iterator.remove();
					} catch (IOException e) {
						log.error(e, e);
					}
				}
			}
		}
		
		public void run() {
			while (running) {
				try {
					recoverConnections();
					if (selector.select(timeout) > 0) {
						Set<SelectionKey> keys = selector.selectedKeys();
						for (Iterator<SelectionKey> iterator = keys.iterator(); iterator.hasNext();) {
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
				for (SelectionKey key : selector.keys()) {
					try {
						key.channel().close();
					} catch (IOException e1) {}
					key.cancel();
				}
				selector.close();
				if (log.isInfoEnabled()) {
					log.info("The ICM-TCP-Client is now closed.");
				}
			} catch (IOException e) {
				log.error(e, e);
			}
		}

		private void read(SelectionKey key) {
			SocketChannel channel = (SocketChannel) key.channel();
			ChannelContext channelContext = (ChannelContext) key.attachment();
			BufferProvider bufferProvider = channelContext.getBufferProvider();
			Charset charset = channelContext.getCharset();
			BufferReaderProvider bufferReaderProvider = channelContext.getBufferReaderProvider();
			IcmContext context = channelContext.getResponseContext();
			try {
				if (context == null) {
					context = initContext(channel, channelContext, bufferProvider, bufferReaderProvider, charset);
				}
				int length = context.getLength();
				int current = context.getCurrent();
				Buffer buffer = context.getBuffer();
				byteBuffer.clear();
				int len = length - current;
				if (len < byteBuffer.capacity()) {
					byteBuffer.limit(len);
				}
				int readed = channel.read(byteBuffer);
				byteBuffer.flip();
				buffer.put(byteBuffer);
				current += readed;
				context.setCurrent(current);
				if (current == length) { // finished
					buffer.flip();
					channelContext.removeResponseContext();
					BufferReader reader = bufferReaderProvider.getReader(buffer, charset);
					ResponseHead responseHead = (ResponseHead) reader.readObject(ResponseHead.class);
					reader.close();
					int id = responseHead.getId();
					IcmContext resContext = channelContext.getResponseWaitedContextMap().remove(id);
					if (resContext != null) {
						resContext.setBuffer(buffer);
						resContext.setResponseHead(responseHead);
						Object responseLock = resContext.getResponseLock();
						synchronized (responseLock) {
							responseLock.notify();
						}
					}
				}
			} catch (IOException e) {
				try {
					channel.close();
				} catch (IOException e1) {}
				key.cancel();
				log.error(e, e);
				String hostname = channelContext.getHostname();
				int port = channelContext.getRemotePort();
				channelMap.get(generateKey(hostname, port)).remove(channel);
				IcmContext requestContext = channelContext.getRequestContext();
				if (requestContext != null) {
					requestContext.getBuffer().reset();
					channelContext.getContextQueue().add(requestContext);
				}
				channelContext.removeRequestContext();
				channelContext.removeResponseContext();
				for (Entry<Integer, IcmContext> entry : channelContext.getResponseWaitedContextMap().entrySet()) {
					Object responseLock = entry.getValue().getResponseLock();
					synchronized (responseLock) {
						responseLock.notify();
					}
				}
				toRecoverChannelContextSet.add(channelContext);
			}
		}

		private IcmContext initContext(SocketChannel channel, ChannelContext channelContext,
				BufferProvider bufferProvider, BufferReaderProvider bufferReaderProvider, Charset charset)
				throws IOException {
			byteBuffer.clear();
			byteBuffer.limit(MESSAGE_HEAD_SIZE);
			while (byteBuffer.hasRemaining()) {
				if (channel.read(byteBuffer) == -1) {
					throw new IOException("The remote host at " + channel.socket().getRemoteSocketAddress()
							+ " has closed the connection!");
				}
			}
			byteBuffer.flip();
			IcmContext context = new IcmContext();
			channelContext.setResponseContext(context);
			Buffer buffer = bufferProvider.getBuffer();
			context.setBuffer(buffer);
			buffer.put(byteBuffer);
			buffer.flip();
			BufferReader reader = bufferReaderProvider.getReader(buffer, charset);
			int length = reader.readInt();
			byte type = reader.readByte();
			reader.close();
			if (type == TYPE_RESPONSE) {
				context.setLength(length);
				context.setCurrent(0);
				buffer.clear();
			} else {
				throw new IOException("The message type is not a request(type=0)! (type=" + context.getType() + ")");
			}
			return context;
		}

		private void write(SelectionKey key) {
			SocketChannel channel = (SocketChannel) key.channel();
			ChannelContext channelContext = (ChannelContext) key.attachment();
			BufferProvider bufferProvider = channelContext.getBufferProvider();
			IcmContext context = channelContext.getRequestContext();
			BlockingQueue<IcmContext> queue = channelContext.getContextQueue();
			if (context == null) {
				context = queue.poll();
				if (context == null) {
					key.interestOps(SelectionKey.OP_READ);
					if (!queue.isEmpty()) {
						key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
					}
					return;
				}
			}
			byteBuffer.clear();
			Buffer buffer = context.getBuffer().mark();
			buffer.get(byteBuffer);
			byteBuffer.flip();
			try {
				channel.write(byteBuffer);
				if (!buffer.hasRemaining()) {
					bufferProvider.destroy(buffer);
					context.remove(IcmContext.KEY_BUFFER);
					channelContext.removeRequestContext();
					if (queue.isEmpty()) {
						key.interestOps(SelectionKey.OP_READ);
					}
					if (!queue.isEmpty()) {
						key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
					}
				}
			} catch (IOException e) {
				try {
					channel.close();
				} catch (IOException e1) {}
				key.cancel();
				log.error(e, e);
				String hostname = channelContext.getHostname();
				int port = channelContext.getRemotePort();
				channelMap.get(generateKey(hostname, port)).remove(channel);
				channelContext.removeRequestContext();
				channelContext.removeResponseContext();
				for (Entry<Integer, IcmContext> entry : channelContext.getResponseWaitedContextMap().entrySet()) {
					Object responseLock = entry.getValue().getResponseLock();
					synchronized (responseLock) {
						responseLock.notify();
					}
				}
				buffer.reset();
				queue.offer(context);
				toRecoverChannelContextSet.add(channelContext);
			}
		}
	}

	private class QueueTask implements Runnable {

		public void run() {
			while (running) {
				try {
					IcmContext context = requestQueue.take();
					if (context.isEmpty()) {
						continue;
					}
					String hostname = context.getHostname();
					int port = context.getPort();
					BlockingQueue<SocketChannel> channelQueue = channelMap.get(generateKey(hostname, port));
					if (channelQueue != null && context.containsKey(IcmContext.KEY_BUFFER)) {
						SocketChannel channel = channelQueue.take();
						SelectionKey key = channel.keyFor(selector);
						ChannelContext channelContext = (ChannelContext) key.attachment();
						channelContext.getResponseWaitedContextMap().put(context.getId(), context);
						BlockingQueue<IcmContext> queue = channelContext.getContextQueue();
						queue.offer(context);
						channelQueue.offer(channel);
						key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
						selector.wakeup();
					}
				} catch (InterruptedException e) {
					Thread.currentThread().interrupt();
				}
			}
		}
	}

	private class IcmInvocationHandler implements InvocationHandler {

		private final IcmRemoteService remoteService;

		private IcmInvocationHandler(IcmRemoteService remoteService) {
			this.remoteService = remoteService;
		}

		@SuppressWarnings({ "unchecked", "rawtypes" })
		public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
			if (method.getDeclaringClass() == Object.class) {
				throw new UnsupportedOperationException();
			}
			RequestHead requestHead = generateRequestHead(method);
			if (requestHead == null) {
				throw new IcmException("Cannot generate the request head for method [" + method + "].");
			}
			Buffer buffer = remoteService.getBufferProvider().getBuffer();
			BufferWriter writer = remoteService.getBufferWriterProvider().getWriter(buffer, remoteService.getCharset());
			writer.writeInt(0); // message size
			writer.writeByte(TYPE_REQUEST);
			writer.writeObject(requestHead, RequestHead.class);
			Type[] types = method.getGenericParameterTypes();
			Class<?>[] classes = method.getParameterTypes();
			if (args != null) { // has parameter
				for (int i = 0; i < args.length; i++) {
					if (classes[i] != Out.class) {
						writer.writeObject(args[i], types[i]);
					}
				}
			}
			int position = buffer.position();
			buffer.position(0);
			writer.writeInt(position - MESSAGE_HEAD_SIZE);
			buffer.position(position);
			writer.close();
			buffer.flip();
			IcmContext context = new IcmContext();
			context.setBuffer(buffer);
			context.setHostname(remoteService.getHostname());
			context.setPort(remoteService.getPort());
			if (remoteService.isLongConnectionMode()) {
				if (requestHead.getResponseFlag() == NO_RESPONSE) {
					requestQueue.offer(context);
					return null;
				} else {
					Object lock = new Object();
					context.setResponseLock(lock);
					context.setId(requestHead.getId());
					requestQueue.offer(context);
					synchronized (lock) {
						try {
							lock.wait(remoteService.getTimeout());
						} catch (InterruptedException e) {}
					}
				}
			} else {
				ByteBuffer byteBuffer = ByteBuffer.allocate(BUFFER_SIZE);
				SocketChannel channel = null;
				try {
					if (log.isDebugEnabled()) {
						log.debug("shot connection mode");
						log.debug("connect remote server " + remoteService.getHostname() + ":" + remoteService.getPort());
					}
					channel = newConnection(remoteService.getHostname(), remoteService.getPort(), true);
					if (log.isDebugEnabled()) {
						log.debug("connecting finished, send message");
					}
					while (buffer.hasRemaining()) {
						byteBuffer.clear();
						buffer.get(byteBuffer);
						byteBuffer.flip();
						channel.write(byteBuffer);
					}
					if (log.isDebugEnabled()) {
						log.debug("send finished, reading result");
					}
					byteBuffer.clear();
					byteBuffer.limit(MESSAGE_HEAD_SIZE);
					while (byteBuffer.hasRemaining()) {
						channel.read(byteBuffer);
					}
					byteBuffer.flip();
					buffer.clear();
					buffer.put(byteBuffer);
					buffer.flip();
					BufferReader reader = remoteService.getBufferReaderProvider().getReader(buffer, remoteService.getCharset());
					int length = reader.readInt();
					byte type = reader.readByte();
					buffer.clear();
					if (TYPE_RESPONSE == type) {
						if (length <= byteBuffer.capacity()) {
							byteBuffer.clear();
							byteBuffer.limit(length);
							while (byteBuffer.hasRemaining()) {
								channel.read(byteBuffer);
							}
							byteBuffer.flip();
							buffer.put(byteBuffer);
						} else {
							int readed = 0;
							while (readed < length) {
								byteBuffer.clear();
								int remaining = length - readed;
								if (remaining < byteBuffer.capacity()) {
									byteBuffer.clear();
									byteBuffer.limit(remaining);
								}
								readed += channel.read(byteBuffer);
								byteBuffer.flip();
								buffer.put(byteBuffer);
							}
						}
						buffer.flip();
						if (log.isDebugEnabled()) {
							log.debug("read finished");
						}
						ResponseHead responseHead = (ResponseHead) reader.readObject(ResponseHead.class);
						context.setResponseHead(responseHead);
					} else {
						throw new IcmRemoteException();
					}
				} catch (IOException e) {
					throw new IcmException(e);
				} finally {
					if (log.isDebugEnabled()) {
						log.debug("close the connection");
					}
					if (channel != null) {
						try {
							channel.close();
						} catch (IOException e) {}
					}
				}
			}
			ResponseHead responseHead = context.getResponseHead();
			if (responseHead == null) {
				throw new RemoteException("Timeout or connection refused when invoke remote method [" + method + "]!");
			}
			buffer = context.getBuffer();
			BufferReader reader = remoteService.getBufferReaderProvider().getReader(buffer, remoteService.getCharset());
			if (responseHead.getReplyStatus() == RESPONSE_OK) {
				if (args != null) {
					for (int i = 0; i < args.length; i++) {
						if (classes[i] == Out.class) {
							((Out) args[i]).set(reader.readObject(((ParameterizedType) types[i])
									.getActualTypeArguments()[0]));
						}
					}
				}
				if (method.getReturnType() != void.class) {
					Object result = reader.readObject(method.getGenericReturnType());
					reader.close();
					return result;
				}
				return null;
			} else {
				throw new IcmRemoteException();
			}
		}

	}

}
