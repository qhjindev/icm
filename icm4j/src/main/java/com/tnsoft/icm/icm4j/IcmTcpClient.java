package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.SocketChannel;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.tnsoft.icm.icm4j.protocol.IcmRequest;
import com.tnsoft.icm.icm4j.protocol.IcmResponse;
import com.tnsoft.icm.io.BufferReader;
import com.tnsoft.icm.io.DataBuffer;

public class IcmTcpClient extends IcmClient {
	
	private static final int BUFFER_CAPACITY = 4 * 1024;
	
	private final Map<Object, EndPoint> endpoints = new ConcurrentHashMap<Object, EndPoint>();

	private ByteOrder byteOrder;
	// private SocketProvider socketProvider;
	private SocketChannelProvider socketChannelProvider;
	private IcmInvocationHandler icmInvocationHandler;
	private DataBufferProvider dataBufferProvider;

	public IcmTcpClient() {
		this(new CppCachedReaderWriterProvider());
	}

	public IcmTcpClient(ReaderWriterProvider readerWriterProvider) {
		setReaderWriterProvider(readerWriterProvider);
		icmInvocationHandler = new IcmInvocationHandler();
	}

	// public void setSocketProvider(SocketProvider socketProvider) {
	// this.socketProvider = socketProvider;
	// }

	public void setSocketChannelProvider(SocketChannelProvider socketChannelProvider) {
		this.socketChannelProvider = socketChannelProvider;
	}

	public void setByteOrder(ByteOrder byteOrder) {
		this.byteOrder = byteOrder;
	}

	public void setDataBufferProvider(DataBufferProvider dataBufferProvider) {
		this.dataBufferProvider = dataBufferProvider;
	}

	@SuppressWarnings("unchecked")
	public synchronized <T> T getService(Class<T> serviceInterface, String hostname, int port) {
		// if (socketProvider == null) {
		// if (log.isInfoEnabled()) {
		// log.info("The current socketProvider is null. Then using default provider: \"SimpleSocketProvider\".");
		// }
		// socketProvider = new SimpleSocketProvider();
		// }
		if (socketChannelProvider == null) {
			socketChannelProvider = new SimpleSocketChannelProvider();
		}
		if (byteOrder == null) {
			if (log.isInfoEnabled()) {
				log.info("The current byteOrder is null. Then using defaul byteOrder: \"BIG_ENDIAN\".");
			}
			byteOrder = ByteOrder.BIG_ENDIAN;
		}
		if (dataBufferProvider == null) {
			if (log.isInfoEnabled()) {
				log.info("The current dataBufferProvider is null. Then using the default: \"CachedDataBufferProvider\".");
			}
			dataBufferProvider = new CachedDataBufferProvider(byteOrder);
		}
		Object proxy = Proxy.newProxyInstance(serviceInterface.getClassLoader(), new Class[] { serviceInterface },
				icmInvocationHandler);
		endpoints.put(proxy, new EndPoint(hostname, port));
		return (T) proxy;
	}

	public void shutdown() {
		// socketProvider.shutdown();
		socketChannelProvider.shutdown();
	}

	// private SocketProvider getSocketProvider() {
	// return socketProvider;
	// }

	private SocketChannelProvider getSocketChannelProvider() {
		return socketChannelProvider;
	}

	private class IcmInvocationHandler implements InvocationHandler {

		public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
			Class<?> dc = method.getDeclaringClass();
			if (dc == Object.class) {
				if ("hashCode".equals(method.getName())) {
					return dc.hashCode();
				} else if ("equals".equals(method.getName())) {
					return proxy == args[0];
				}
			} else {
				EndPoint endpoint = endpoints.get(proxy);
				SocketChannel channel = null;
//				Socket socket = null;
				try {
					// TODO In future, use selector instead.
					channel = getSocketChannelProvider().get(endpoint.hostname, endpoint.port);
					// socket = getSocketProvider().get(endpoint.hostname, endpoint.port);
//					OutputStream out = socket.getOutputStream();
					IcmRequest request = generateRequest(method, args);
					ByteBuffer buf = ByteBuffer.allocate(BUFFER_CAPACITY);
					DataBuffer buffer = dataBufferProvider.getBuffer();
					writeRequest(buffer, request);
					for (; buffer.hasNext();) {
						buf.clear();
						buffer.get(buf);
						buf.flip();
						channel.write(buf);
						Thread.sleep(100);
//						out.write(buffer.get());
					}
//					out.flush();
					if (request.getResponseFlag() == HAS_RESPONSE) {
//						InputStream in = socket.getInputStream();
						ByteBuffer headBuf = ByteBuffer.allocate(MESSAGE_HEAD_SIZE);
						channel.read(headBuf);
						headBuf.flip();
//						byte[] head_buf = new byte[MESSAGE_HEAD_SIZE];
//						in.read(head_buf);
						buffer.clear();
//						buffer.put(head_buf);
						buffer.put(headBuf);
						buffer.flip();
						BufferReader reader = readerWriterProvider.getReader(buffer);
						int size = reader.readInt();
						byte type = reader.readByte();
						reader.close();
						buffer.clear();
//						for (int i = 0; i < size; i++) {
//							buffer.put((byte) in.read());
//						}
						for (int cur = 0; cur < size;) {
							buf.clear();
							int r = channel.read(buf);
							if (r < 0) {
								throw new IOException();
							}
							buf.flip();
							buffer.put(buf);
							cur += r;
						}
						buffer.flip();
						if (type == TYPE_RESPONSE) {
							IcmResponse resp = new IcmResponse();
							switch (readResponse(buffer, resp, method, args)) {
							case RESPONSE_OK:
								return resp.getResult();
							default:
							case RESPONSE_UNKNOWN_ERROR:
								// TODO
								break;
							}
						}
					}
				} catch (UnknownHostException e) {
//					close(socket);
					close(channel);
					if (log.isErrorEnabled()) {
						log.error(e, e);
					}
					throw new ICMConnectionException(e.getMessage(), e);
				} catch (IOException e) {
//					close(socket);
					close(channel);
					if (log.isErrorEnabled()) {
						log.error(e, e);
					}
					throw new ICMConnectionException(e.getMessage(), e);
				} finally {
					getSocketChannelProvider().back(channel);
					// getSocketProvider().back(socket);
				}
			}
			return null;
		}

//		private void close(Socket socket) {
//			if (socket != null && !socket.isClosed()) {
//				try {
//					socket.close();
//				} catch (IOException e) {
//					if (log.isErrorEnabled()) {
//						log.error(e, e);
//					}
//				}
//			}
//		}

		private void close(SocketChannel channel) {
			if (channel != null) {
				try {
					channel.close();
				} catch (IOException e) {
					if (log.isErrorEnabled()) {
						log.error(e, e);
					}
				}
			}
		}
	}

	private class EndPoint {
		private final String hostname;
		private final int port;

		public EndPoint(String hostname, int port) {
			this.hostname = hostname;
			this.port = port;
		}

	}
}
