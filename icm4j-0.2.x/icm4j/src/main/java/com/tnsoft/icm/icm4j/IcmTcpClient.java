package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Proxy;
import java.lang.reflect.Type;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.rmi.RemoteException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
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

public class IcmTcpClient implements IcmProtocol, ConnectionMode {

	private static final Log log = LogFactory.getLog(IcmTcpClient.class);

	private static final int BUFFER_SIZE = 4 * 1024;
	private static final long DEFAULT_TIMEOUT = 60 * 60 * 1000L;
	
	private final String hostname;
	private final int port;
	private int connectionCount = 1;
	private int connectionMode = MODE_SHORT_CONNECTION;
	private boolean running;
	private Selector selector;
	private Thread selectorThread;
	private Thread queueThread;
	private BlockingQueue<IcmContext> requestQueue;
	private Map<Integer, IcmContext> responseWaitedMap;
	private BlockingQueue<SocketChannel> channelQueue;
	private final AtomicInteger identity = new AtomicInteger(0);
	private ByteOrder byteOrder;
	private Charset charset;
	private BufferProvider bufferProvider;
	private BufferReaderProvider bufferReaderProvider;
	private BufferWriterProvider bufferWriterProvider;
	private long timeout = -1;
	private final ByteBuffer byteBuffer = ByteBuffer.allocate(BUFFER_SIZE);
	private final IcmInvocationHandler icmInvocationHandler = new IcmInvocationHandler();
	private final Map<Method, RequestHead> requestHeadMap = new HashMap<Method, RequestHead>();

	public IcmTcpClient(String hostname, int port) {
		this.hostname = hostname;
		this.port = port;
	}

	public String getRemoteHostname() {
		return hostname;
	}

	public int getRemotePort() {
		return port;
	}

	public int getConnectionCount() {
		return connectionCount;
	}

	public void setConnectionCount(int connectionCount) {
		this.connectionCount = connectionCount;
	}

	public int getConnectionMode() {
		return connectionMode;
	}

	public synchronized void setConnectionMode(int connectionMode) {
		if (!running) {
			this.connectionMode = connectionMode;
		}
	}

	public boolean isShortConnectionMode() {
		return (connectionMode & MODE_SHORT_CONNECTION) == MODE_SHORT_CONNECTION;
	}

	public boolean isLongConnectionMode() {
		return (connectionMode & MODE_LONG_CONNECTION) == MODE_LONG_CONNECTION;
	}

	public boolean isRunning() {
		return running;
	}

	public void setByteOrder(ByteOrder byteOrder) {
		this.byteOrder = byteOrder;
	}

	public void setCharset(Charset charset) {
		this.charset = charset;
	}

	public void setBufferProvider(BufferProvider bufferProvider) {
		this.bufferProvider = bufferProvider;
	}

	public void setBufferReaderProvider(BufferReaderProvider bufferReaderProvider) {
		this.bufferReaderProvider = bufferReaderProvider;
	}

	public void setBufferWriterProvider(BufferWriterProvider bufferWriterProvider) {
		this.bufferWriterProvider = bufferWriterProvider;
	}
	
	public void setTimeout(long timeout) {
		this.timeout = timeout;
	}

	public synchronized void startup() {
		if (!running) {
			running = true;
			init();
			if (isShortConnectionMode()) {
				if (log.isInfoEnabled()) {
					log.info("Connection Mode: Short Connection.");
				}
			}
			if (isLongConnectionMode()) {
				if (log.isInfoEnabled()) {
					log.info("Connection Mode: Long Connection.");
				}
				try {
					selector = Selector.open();
				} catch (IOException e) {
					throw new IcmException(e);
				}
				selectorThread = new Thread(new SelectorTask(), "selector thread");
				queueThread = new Thread(new QueueTask(), "queue thread");
				requestQueue = new LinkedBlockingQueue<IcmContext>();
				responseWaitedMap = new ConcurrentHashMap<Integer, IcmContext>();
				if (connectionCount < 1) {
					connectionCount = 1;
				}
				if (log.isInfoEnabled()) {
					log.info("Long Connection Count: " + connectionCount);
				}
				channelQueue = new LinkedBlockingQueue<SocketChannel>(connectionCount);
				for (int i = 0; i < connectionCount; i++) {
					newConnection();
				}
				selectorThread.start();
				queueThread.start();
			}
		}
	}

	private void init() {
		if (byteOrder == null) {
			byteOrder = ByteOrder.BIG_ENDIAN;
			if (log.isInfoEnabled()) {
				log.info("Use default byte order: " + byteOrder + ".");
			}
		}
		if (charset == null) {
			charset = Charset.forName("UTF-8");
			if (log.isInfoEnabled()) {
				log.info("Use default charset: " + charset + ".");
			}
		}
		if (bufferProvider == null) {
			bufferProvider = CachedBufferProvider.getInstance(byteOrder);
			if (log.isInfoEnabled()) {
				log.info("Use default bufferProvider: CachedBufferProvider.");
			}
		}
		if (bufferReaderProvider == null) {
			bufferReaderProvider = CachedBufferReaderProvider.getInstance();
			if (log.isInfoEnabled()) {
				log.info("Use default bufferReaderProvider: CachedBufferReaderProvider.");
			}
		}
		if (bufferWriterProvider == null) {
			bufferWriterProvider = CachedBufferWriterProvider.getInstance();
			if (log.isInfoEnabled()) {
				log.info("Use default bufferWriterProvider: CachedBufferWriterProvider.");
			}
		}
		if (timeout < 0) {
			timeout = DEFAULT_TIMEOUT;
		}
	}

	private void newConnection() {
		ChannelContext channelContext = new ChannelContext();
		channelContext.setContextQueue(new LinkedBlockingQueue<IcmContext>());
		newConnection(channelContext);
	}
	
	private void newConnection(ChannelContext channelContext) {
		try {
			SocketChannel channel = newConnectedChannel(false);
			BlockingQueue<IcmContext> queue = channelContext.getContextQueue();
			if (queue.isEmpty()) {
				channel.register(selector, SelectionKey.OP_READ, channelContext);
			} else {
				channel.register(selector, SelectionKey.OP_READ | SelectionKey.OP_WRITE, channelContext);
			}
			channelQueue.offer(channel);
		} catch (IOException e) {
			throw new IcmException(e);
		}
	}

	private SocketChannel newConnectedChannel(boolean blocking) throws IOException {
		SocketChannel channel = SocketChannel.open(new InetSocketAddress(hostname, port));
		channel.configureBlocking(blocking);
		channel.finishConnect();
		return channel;
	}

	public synchronized void shutdown() {
		if (running) {
			running = false;
			if (isLongConnectionMode()) {
				requestQueue.offer(new IcmContext());
				selector.wakeup();
			}
		}
	}

	@SuppressWarnings("unchecked")
	public synchronized <T> T getService(Class<T> interfaceClass) {
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
				icmInvocationHandler);
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
		
		public void run() {
			while (running) {
				try {
					if (selector.select() > 0) {
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
			IcmContext context = channelContext.getResponseContext();
			try {
				if (context == null) {
					context = initContext(channel, channelContext);
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
					IcmContext resContext = responseWaitedMap.remove(id);
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
				key.cancel();
				channelContext.removeResponseContext();
				log.error(e, e);
				newConnection(channelContext);
			}
		}

		private IcmContext initContext(SocketChannel channel, ChannelContext channelContext) throws IOException {
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
				throw new IOException("The message type is not a request(type=0)! (type=" + context.getType()
						+ ")");
			}
			return context;
		}

		private void write(SelectionKey key) {
			SocketChannel channel = (SocketChannel) key.channel();
			ChannelContext channelContext = (ChannelContext) key.attachment();
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
				key.cancel();
				log.error(e, e);
				channelQueue.remove(channel);
				channelContext.removeRequestContext();
				buffer.reset();
				queue.offer(context);
				newConnection(channelContext);				
			}
		}
	}

	private class QueueTask implements Runnable {

		public void run() {
			while (running) {
				try {
					IcmContext context = requestQueue.take();
					if (context.containsKey(IcmContext.KEY_BUFFER)) {
						SocketChannel channel = channelQueue.take();
						SelectionKey key = channel.keyFor(selector);
						ChannelContext channelContext = (ChannelContext) key.attachment();
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

		@SuppressWarnings({ "unchecked", "rawtypes" })
		public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
			if (method.getDeclaringClass() == Object.class) {
				throw new UnsupportedOperationException();
			}
			RequestHead requestHead = generateRequestHead(method);
			if (requestHead == null) {
				throw new IcmException("Cannot generate the request head for method [" + method + "].");
			}
			Buffer buffer = bufferProvider.getBuffer();
			BufferWriter writer = bufferWriterProvider.getWriter(buffer, charset);
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
			if (isLongConnectionMode()) {
				if (requestHead.getResponseFlag() == NO_RESPONSE) {
					requestQueue.offer(context);
					return null;
				} else {
					Object lock = new Object();
					context.setResponseLock(lock);
					responseWaitedMap.put(requestHead.getId(), context);
					requestQueue.offer(context);
					synchronized (lock) {
						try {
							lock.wait(timeout);
						} catch (InterruptedException e) {}
					}
				}
			} else if (isShortConnectionMode()) {
				ByteBuffer byteBuffer = ByteBuffer.allocate(BUFFER_SIZE);
				SocketChannel channel = null;
				try {
					channel = newConnectedChannel(true);
					while (buffer.hasRemaining()) {
						byteBuffer.clear();
						buffer.get(byteBuffer);
						byteBuffer.flip();
						channel.write(byteBuffer);
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
					BufferReader reader = bufferReaderProvider.getReader(buffer, charset);
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
						ResponseHead responseHead = (ResponseHead) reader.readObject(ResponseHead.class);
						context.setResponseHead(responseHead);
					} else {
						throw new IcmRemoteException();
					}
				} catch (IOException e) {
					throw new IcmException(e);
				} finally {
					if (channel != null) {
						try {
							channel.close();
						} catch (IOException e) {}
					}
				}
			}
			ResponseHead responseHead = context.getResponseHead();
			if (responseHead == null) {
				throw new RemoteException("Timeout when invoke remote method [" + method + "]!");
			}
			buffer = context.getBuffer();
			BufferReader reader = bufferReaderProvider.getReader(buffer, charset);
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
