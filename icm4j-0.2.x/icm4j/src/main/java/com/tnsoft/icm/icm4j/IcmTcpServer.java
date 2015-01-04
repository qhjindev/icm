package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.icm4j.annotation.Async;
import com.tnsoft.icm.icm4j.annotation.Name;
import com.tnsoft.icm.icm4j.annotation.Operation;
import com.tnsoft.icm.icm4j.annotation.Sync;
import com.tnsoft.icm.icm4j.io.Buffer;
import com.tnsoft.icm.icm4j.io.BufferReader;
import com.tnsoft.icm.icm4j.io.BufferWriter;
import com.tnsoft.icm.icm4j.protocol.IcmProtocol;
import com.tnsoft.icm.icm4j.protocol.Out;
import com.tnsoft.icm.icm4j.protocol.RequestHead;
import com.tnsoft.icm.icm4j.protocol.ResponseHead;

public class IcmTcpServer implements IcmProtocol {

	private static final Log log = LogFactory.getLog(IcmTcpServer.class);
	private static final int BUFFER_SIZE = 4 * 1024;

	private Selector selector;
	private ServerSocketChannel serverSocketChannel;
	private final int port;
	private ByteOrder byteOrder;
	private Charset charset;
	private boolean running;
	private BufferProvider bufferProvider;
	private BufferReaderProvider bufferReaderProvider;
	private BufferWriterProvider bufferWriterProvider;
	private final ByteBuffer byteBuffer = ByteBuffer.allocate(BUFFER_SIZE);
	private final IcmRequestDispatcher requestDispatcher = new IcmRequestDispatcher();
	private final Thread selectorThread = new Thread(new SelectorTask(), "selector thread");
	private final ExecutorService requestExecutor = Executors.newSingleThreadExecutor();
	private ExecutorService threadPoolExecutor;

	public IcmTcpServer(int port) {
		this.port = port;
	}

	public void addService(Class<?> interfaceClass, Object service) {
		if (!running) {
			boolean all_async = service.getClass().isAnnotationPresent(Async.class);
			String name = null;
			Name aname = interfaceClass.getAnnotation(Name.class);
			if (aname == null || aname.value() == "") {
				name = interfaceClass.getSimpleName();
			} else {
				name = aname.value();
			}
			String category = ""; // TODO ?
			for (Method method : interfaceClass.getMethods()) {
				String operation = null;
				Operation aoperation = method.getAnnotation(Operation.class);
				if (aoperation == null || aoperation.value() == "") {
					operation = method.getName();
				} else {
					operation = aoperation.value();
				}
				boolean async = false;
				Method realMethod = getRealMethod(method, service);
				if (all_async) {
					async = !realMethod.isAnnotationPresent(Sync.class);
				} else {
					async = realMethod.isAnnotationPresent(Async.class);
				}
				IcmOperation icmOperation = new IcmOperation(method, service, async);
				requestDispatcher.addOperation(name, category, operation, icmOperation);
			}
		}
	}

	private Method getRealMethod(Method method, Object service) {
		try {
			return service.getClass().getMethod(method.getName(), method.getParameterTypes());
		} catch (NoSuchMethodException e) {
			throw new IllegalArgumentException("object [" + service + "] is not an instance of declaring class ["
					+ method.getDeclaringClass() + "]");
		}
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

	public void setThreadPoolExecutor(ExecutorService threadPoolExecutor) {
		this.threadPoolExecutor = threadPoolExecutor;
	}

	public synchronized void startup() {
		if (!running) {
			running = true;
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
				bufferProvider = new CachedBufferProvider(10, byteOrder);
				if (log.isInfoEnabled()) {
					log.info("Use default bufferProvider: CachedBufferProvider.");
				}
			}
			if (bufferReaderProvider == null) {
				bufferReaderProvider = new CachedBufferReaderProvider(10);
				if (log.isInfoEnabled()) {
					log.info("Use default bufferReaderProvider: CachedBufferReaderProvider.");
				}
			}
			if (bufferWriterProvider == null) {
				bufferWriterProvider = new CachedBufferWriterProvider(10);
				if (log.isInfoEnabled()) {
					log.info("Use default bufferWriterProvider: CachedBufferWriterProvider.");
				}
			}
			if (threadPoolExecutor == null) {
				threadPoolExecutor = Executors.newCachedThreadPool();
				if (log.isInfoEnabled()) {
					log.info("Use default threadPoolExecutor: CachedThreadPool.");
				}
			}
			try {
				selector = Selector.open();
				serverSocketChannel = ServerSocketChannel.open();
				serverSocketChannel.configureBlocking(false);
				serverSocketChannel.socket().bind(new InetSocketAddress(port));
				if (log.isInfoEnabled()) {
					log.info("Listening the port: " + port);
				}
				serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
				selectorThread.start();
			} catch (IOException e) {
				throw new IcmException(e);
			}
		}
	}

	public synchronized void shutdown() {
		if (running) {
			running = false;
			threadPoolExecutor.shutdown();
			requestExecutor.shutdown();
			selector.wakeup();
		}
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
							if (key.isValid() && key.isAcceptable()) {
								accept(key);
							}
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
					log.info("The ICM-TCP-Server is now closed.");
				}
			} catch (IOException e) {
				log.error(e, e);
			}
		}

		private void accept(SelectionKey key) {
			ServerSocketChannel channel = (ServerSocketChannel) key.channel();
			try {
				SocketChannel socketChannel = channel.accept();
				socketChannel.configureBlocking(false);
				ChannelContext context = new ChannelContext();
				context.setContextQueue(new LinkedBlockingQueue<IcmContext>());
				socketChannel.register(selector, SelectionKey.OP_READ, context);
			} catch (IOException e) {
				try {
					channel.close();
				} catch (IOException e1) {}
				key.cancel();
				log.error(e, e);
			}
		}

		private void read(SelectionKey key) {
			SocketChannel channel = (SocketChannel) key.channel();
			ChannelContext channelContext = (ChannelContext) key.attachment();
			IcmContext context = channelContext.getRequestContext();
			try {
				if (context == null) {
					byteBuffer.clear();
					byteBuffer.limit(MESSAGE_HEAD_SIZE);
					while (byteBuffer.hasRemaining()) {
						if (channel.read(byteBuffer) == -1) {
							throw new IOException("The remote host at " + channel.socket().getRemoteSocketAddress()
									+ " has closed the connection!");
						}
					}
					byteBuffer.flip();
					context = new IcmContext();
					context.setChannelContext(channelContext);
					channelContext.setRequestContext(context);
					Buffer buffer = bufferProvider.getBuffer();
					context.setBuffer(buffer);
					buffer.put(byteBuffer);
					buffer.flip();
					BufferReader reader = bufferReaderProvider.getReader(buffer, charset);
					context.setLength(reader.readInt());
					context.setType(reader.readByte());
					context.setCurrent(0);
					reader.close();
					buffer.clear();
					if (context.getType() != TYPE_REQUEST) {
						throw new IOException("The message type is not a request(type=0)! (type=" + context.getType()
								+ ")");
					}
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
					context.setSelectableChannel(channel);
					channelContext.removeRequestContext();
					requestExecutor.execute(new RequestTask(context));
				}
			} catch (IOException e) {
				try {
					channel.close();
				} catch (IOException e1) {}
				key.cancel();
				channelContext.clear();
				log.warn(e, e);
			}
		}

		private void write(SelectionKey key) {
			SocketChannel channel = (SocketChannel) key.channel();
			ChannelContext channelContext = (ChannelContext) key.attachment();
			IcmContext context = channelContext.getResponseContext();
			BlockingQueue<IcmContext> responseQueue = channelContext.getContextQueue();
			try {
				if (context == null) {
					if (responseQueue.isEmpty()) {
						key.interestOps(SelectionKey.OP_READ);
						if (!responseQueue.isEmpty()) {
							key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
						}
						return;
					} else {
						context = responseQueue.poll();
						channelContext.setResponseContext(context);
					}
				}
				Buffer buffer = context.getBuffer();
				byteBuffer.clear();
				buffer.get(byteBuffer);
				byteBuffer.flip();
				channel.write(byteBuffer);
				if (!buffer.hasRemaining()) {
					context.clear();
					bufferProvider.destroy(buffer);
					channelContext.removeResponseContext();
					if (responseQueue.isEmpty()) {
						key.interestOps(SelectionKey.OP_READ);
						if (!responseQueue.isEmpty()) {
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
			}
		}
	}

	private class RequestTask implements Runnable {

		private final IcmContext context;

		public RequestTask(IcmContext context) {
			this.context = context;
		}

		public void run() {
			final Buffer buffer = context.getBuffer();
			final BufferReader reader = bufferReaderProvider.getReader(buffer, charset);
			final RequestHead requestHead = (RequestHead) reader.readObject(RequestHead.class);
			final IcmOperation icmOperation = requestDispatcher.dispatch(requestHead);
			if (icmOperation != null) {
				Runnable runnable = new Runnable() {
					public void run() {
						Type[] types = icmOperation.getMethod().getGenericParameterTypes();
						Class<?>[] classes = icmOperation.getMethod().getParameterTypes();
						Object[] args = new Object[types.length];
						List<Integer> outIndexList = new ArrayList<Integer>();
						for (int i = 0; i < args.length; i++) {
							if (Out.class != classes[i]) {
								args[i] = reader.readObject(types[i]);
							} else {
								args[i] = new Out<Object>();
								outIndexList.add(i);
							}
						}
						reader.close();
						buffer.clear();
						try {
							Object result = icmOperation.getMethod().invoke(icmOperation.getObj(), args);
							if (requestHead.getResponseFlag() == HAS_RESPONSE) {
								BufferWriter writer = bufferWriterProvider.getWriter(buffer, charset);
								writer.writeInt(0);
								writer.writeByte(TYPE_RESPONSE);
								ResponseHead responseHead = new ResponseHead();
								responseHead.setId(requestHead.getId());
								responseHead.setReplyStatus(RESPONSE_OK);
								writer.writeObject(responseHead, ResponseHead.class);
								for (Integer i : outIndexList) {
									@SuppressWarnings("unchecked")
									Out<Object> outArg = (Out<Object>) args[i];
									writer.writeObject(outArg.get(),
											((ParameterizedType) types[i]).getActualTypeArguments()[0]);
								}
								if (icmOperation.getMethod().getReturnType() != void.class) {
									writer.writeObject(result, icmOperation.getMethod().getGenericReturnType());
								}
								int size = buffer.position();
								buffer.position(0);
								writer.writeInt(size - MESSAGE_HEAD_SIZE);
								buffer.position(size);
								writer.close();
								buffer.flip();
								SelectionKey key = context.getSelectableChannel().keyFor(selector);
								if (key != null) {
									ChannelContext channelContext = context.getChannelContext();
									channelContext.getContextQueue().offer(context);
									key.interestOps(SelectionKey.OP_READ | SelectionKey.OP_WRITE);
									selector.wakeup();
								}
							}
						} catch (Exception e) {
							log.error(e, e);
							// TODO send error response ?
						}
					}
				};
				if (icmOperation.isAsync()) {
					if (log.isDebugEnabled()) {
						log.debug("Async operation: " + icmOperation);
					}
					threadPoolExecutor.execute(runnable);
				} else {
					runnable.run();
				}
			} else {
				// TODO can not find the operation !
				// send error response ?
			}
		}

	}

}
