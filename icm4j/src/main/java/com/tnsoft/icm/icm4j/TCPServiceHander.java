package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

import com.tnsoft.icm.icm4j.protocol.MessageHead;
import com.tnsoft.icm.icm4j.protocol.Request;
import com.tnsoft.icm.icm4j.protocol.RequestHead;
import com.tnsoft.icm.icm4j.protocol.Response;
import com.tnsoft.icm.icm4j.protocol.ResponseHead;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.util.BufferFactory;
import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;
import com.tnsoft.icm.util.ChannelPool;
import com.tnsoft.icm.util.PoolProvider;
import com.tnsoft.icm.util.ReaderWriterFactory;
import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

@Deprecated
public class TCPServiceHander implements InvocationHandler {

	private static final Log log = LogFactory.getFormatterLog(TCPServiceHander.class);

	private BufferWriter writer = ReaderWriterFactory.getCBufferWriter();
	private BufferReader reader = ReaderWriterFactory.getCBufferReader();
	private PoolProvider poolProvider;

	private Map<Class<?>, List<ProxyHolder>> endpointMapper = new ConcurrentHashMap<Class<?>, List<ProxyHolder>>();

	public PoolProvider getPoolProvider() {
		return poolProvider;
	}

	public void setPoolProvider(PoolProvider poolProvider) {
		this.poolProvider = poolProvider;
	}

	public void setWriter(BufferWriter writer) {
		this.writer = writer;
	}

	public void setReader(BufferReader reader) {
		this.reader = reader;
	}

	public void putAddress(Object proxy, Class<?> interfaceClass, String ip, int port) {
		ProxyHolder proxyHolder = new ProxyHolder(proxy, ip, port);
		List<ProxyHolder> list = endpointMapper.get(interfaceClass);
		if (list == null) {
			list = new CopyOnWriteArrayList<TCPServiceHander.ProxyHolder>();
			endpointMapper.put(interfaceClass, list);
		}
		list.add(proxyHolder);
	}

	public int channelRead(SocketChannel socketChannel, ByteBuffer buffer) throws IOException {
		int readCount = 0;
		for (int n = 0; readCount < buffer.capacity(); readCount += n) {
			n = socketChannel.read(buffer);
			if (n < 0) {
				// the channel has been closed by remote client
				return n;
			}
		}
		return readCount;
	}

	@SuppressWarnings({ "unchecked", "rawtypes" })
	public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
		// the method is not declared by interface but by java.lang.Object
		if (Object.class == method.getDeclaringClass()) {
			return method.invoke(proxy, args);
		}
		// request
		Request request = new Request(method, args);
		request.writeBuffer(writer);
		// send message
		ProxyHolder proxyHolder = findTheEndpoint(proxy, method);
		if (proxyHolder != null) {
			SocketChannel socketChannel = null;
			ChannelPool channelPool = poolProvider.getPool(proxyHolder.getHostname(), proxyHolder.getPort());
			try {
				socketChannel = channelPool.get();
				socketChannel.write(request.getBuffer());
				// get response
				switch (request.getHead().getResponseFlag()) {
				case RequestHead.RESPONSE: // has response
					ByteBuffer buffer = BufferFactory.getInstance().getBuffer(MessageHead.MESSAGE_HEAD_SIZE);
					int readCount = channelRead(socketChannel, buffer);
					if (readCount < 0) {
						// close the channel
						socketChannel.close();
						log.error("The remote host at [%s:%d] has closed the channel!", proxyHolder.getHostname(),
								proxyHolder.getPort());
						throw new RuntimeException("The remote host at [" + proxyHolder.getHostname() + ":"
								+ proxyHolder.getPort() + "] has closed the channel!");

					}
					buffer.flip();
					int messageSize = reader.readInt(buffer);
					byte messageType = reader.readByte(buffer);
					if (messageType == MessageHead.RESPONSE) {
						buffer = BufferFactory.getInstance().getBuffer(messageSize);
						// writer.writeInt(buffer, messageSize);
						// writer.writeByte(buffer, messageType);
						readCount = channelRead(socketChannel, buffer);
						if (readCount < 0) {
							// the channel has been closed by remote client
							// cancel the selection key and close the channel
							socketChannel.close();
							log.error("The remote host at [%s:%d] has closed the channel!", proxyHolder.getHostname(),
									proxyHolder.getPort());
							throw new RuntimeException("The remote host at [" + proxyHolder.getHostname() + ":"
									+ proxyHolder.getPort() + "] has closed the channel!");

						}
						buffer.flip();
						// response head
						Response response = new Response(buffer);
						response.getMessageHead().setSize(messageSize);
						response.getMessageHead().setType(messageType);
						response.readHead(reader);
						if (response.getHead().getReplyStatus() == ResponseHead.OK
								|| response.getHead().getReplyStatus() == ResponseHead.RESULT_IS_NULL) {
							response.readBody(reader, method);
							// out parameters
							Class<?>[] outTypes = method.getParameterTypes();
							for (int i = 0; i < outTypes.length; i++) {
								if (outTypes[i].isAssignableFrom(Out.class)) {
									((Out) args[i]).set(response.getBody().getOutArguments().remove(0));
								}
							}
							return response.getBody().getResult();
						}
					}
					break;
				case RequestHead.NO_RESPONSE:
					// no response, do nothing
					return null;
				default:
					throw new RuntimeException("Unknown response flag value: [" + request.getHead().getResponseFlag()
							+ "]!");
				}
			} catch (IOException e) {
				log.error(e);
				throw new ICMConnectionException(e);
			} finally {
				channelPool.back(socketChannel);
			}
		} else {
			// Can't find a ProxyHolder? It can't be happened.
		}
		return null;
	}

	private ProxyHolder findTheEndpoint(Object proxy, Method method) {
		List<ProxyHolder> list = endpointMapper.get(method.getDeclaringClass());
		if (list != null) {
			for (ProxyHolder proxyHolder : list) {
				if (proxyHolder.getProxy() == proxy) {
					return proxyHolder;
				}
			}
		}
		return null;
	}

	private class ProxyHolder {

		private Object proxy;
		private String hostname;
		private int port;
		private SocketAddress endpoint;

		public ProxyHolder(Object proxy, String hostname, int port) {
			this.proxy = proxy;
			this.hostname = hostname;
			this.port = port;
			this.endpoint = new InetSocketAddress(hostname, port);
		}

		public Object getProxy() {
			return proxy;
		}

		@SuppressWarnings("unused")
		public SocketAddress getEndpoint() {
			return endpoint;
		}

		public String getHostname() {
			return hostname;
		}

		public int getPort() {
			return port;
		}

	}
}
