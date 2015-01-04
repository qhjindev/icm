package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.ClosedChannelException;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

import com.tnsoft.icm.icm4j.protocol.IcmRequest;
import com.tnsoft.icm.icm4j.protocol.IcmResponse;
import com.tnsoft.icm.io.BufferReader;
import com.tnsoft.icm.io.DataBuffer;

public class IcmTcpServer extends IcmServer implements Runnable {

	private static final AtomicInteger IDENTITY = new AtomicInteger(1);
	private int port;
	private boolean running = false;
	private Thread serverThread;
	private ExecutorService executorService;
	private ExecutorService sychronizedExecutor = Executors.newSingleThreadScheduledExecutor();
	private DataBufferProvider dataBufferProvider;
	private Selector selector;
	private ServerSocketChannel serverSocketChannel;
	private ByteBuffer buffer = ByteBuffer.allocate(8 * 1024);
	private ByteBuffer headBuffer = ByteBuffer.allocate(MESSAGE_HEAD_SIZE);
	private ByteOrder byteOrder;

	public IcmTcpServer(int port) {
		this(port, null);
	}

	public IcmTcpServer(int port, ExecutorService executorService) {
		this.port = port;
		this.executorService = executorService;
	}

	public ByteOrder getByteOrder() {
		return byteOrder;
	}

	public void setByteOrder(ByteOrder byteOrder) {
		this.byteOrder = byteOrder;
	}

	public void setDataBufferProvider(DataBufferProvider dataBufferProvider) {
		this.dataBufferProvider = dataBufferProvider;
	}

	public boolean isRunning() {
		return running;
	}

	public synchronized void startup() {
		if (isRunning()) {
			throw new IllegalStateException("The " + serverThread.getName() + " has been started!");
		} else {
			running = true;
			if (executorService == null) {
				executorService = Executors.newCachedThreadPool();
			}
			if (byteOrder == null) {
				byteOrder = ByteOrder.BIG_ENDIAN;
			}
			buffer.order(byteOrder);
			headBuffer.order(byteOrder);
			if (readerWriterProvider == null) {
				setReaderWriterProvider(new CppCachedReaderWriterProvider());
			}
			if (dataBufferProvider == null) {
				setDataBufferProvider(new CachedDataBufferProvider(byteOrder));
			}
			try {
				selector = Selector.open();
				serverSocketChannel = ServerSocketChannel.open();
				serverSocketChannel.configureBlocking(false);
				InetSocketAddress address = new InetSocketAddress(port);
				serverSocketChannel.socket().bind(address);
				serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
			} catch (IOException e) {
				if (log.isErrorEnabled()) {
					log.error(e.getMessage(), e);
				}
				throw new ICMConnectionException(e);
			}
			serverThread = new Thread(this, "IcmTcpServer-" + IDENTITY.getAndIncrement());
			serverThread.start();
			if (log.isInfoEnabled()) {
				log.info("The " + serverThread.getName() + " started at port [" + port + "].");
			}
		}
	}

	public synchronized void shutdown() {
		if (isRunning()) {
			if (log.isInfoEnabled()) {
				log.info(String.format("Shutting down the %s.", serverThread.getName()));
			}
			running = false;
			executorService.shutdown();
			sychronizedExecutor.shutdown();
			selector.wakeup();
		}
	}

	public void run() {
		while (isRunning()) {
			try {
				selector.select();
				Iterator<SelectionKey> keys = selector.selectedKeys().iterator();
				while (keys.hasNext()) {
					SelectionKey key = keys.next();
					keys.remove();
					try {
						if (key.isValid() && key.isAcceptable()) {
							accept(key);
						}
						if (key.isValid() && key.isReadable()) {
							read(key);
						}
						if (key.isValid() && key.isWritable()) {
							write(key);
						}
					} catch (Throwable e) {
						key.cancel();
						if (log.isErrorEnabled()) {
							log.error(e, e);
						}
					}
				}
			} catch (IOException e) {
				if (log.isErrorEnabled()) {
					log.error(e, e);
				}
			}
		}
		try {
			selector.close();
		} catch (IOException e) {
			if (log.isErrorEnabled()) {
				log.error(e, e);
			}
		}
		if (log.isInfoEnabled()) {
			log.info(String.format("The %s stoped.", serverThread.getName()));
		}
	}

	private void accept(final SelectionKey key) {
		ServerSocketChannel serverSocketChannel = (ServerSocketChannel) key.channel();
		try {
			SocketChannel socketChannel = serverSocketChannel.accept();
			socketChannel.configureBlocking(false);
			socketChannel.register(selector, SelectionKey.OP_READ);
		} catch (IOException e) {
			key.cancel();
			if (log.isErrorEnabled()) {
				log.error(e, e);
			}
		}
	}

	private void read(final SelectionKey key) {
		final SocketChannel socketChannel = (SocketChannel) key.channel();
		RequestHolder rh = (RequestHolder) key.attachment();
		boolean finished = false;
		DataBuffer dataBuffer = null;
		try {
			if (rh == null) {
				readHead(socketChannel);
				dataBuffer = dataBufferProvider.getBuffer();
				dataBuffer.put(headBuffer);
				dataBuffer.flip();				
				BufferReader reader = readerWriterProvider.getReader(dataBuffer);
				int size = reader.readInt();
				byte type = reader.readByte();
				reader.close();
				dataBuffer.clear();
				switch (type) {
				case TYPE_REQUEST:
					int r = readBuffer(socketChannel, dataBuffer);
					if (r == size) {
						finished = true;
					} else {
						rh = new RequestHolder(size, dataBuffer);
						socketChannel.register(selector, SelectionKey.OP_READ, rh);
					}
					break;
				default:
					throw new IllegalArgumentException(String.format("Unknown message type [%d] for ICM!", type));
				}
			} else {
				dataBuffer = rh.getBuffer();
				int size = rh.getLength();
				readBuffer(socketChannel, dataBuffer);
				if (dataBuffer.position() == size) {
					finished = true;
				}
			}
			if (finished) {
				dataBuffer.flip();				
				IcmRequest req = new IcmRequest(false);
				OperatingBean ob = readRequest(dataBuffer, req);
				if (ob.isAsync()) {
					executorService.execute(new ProcessOperation(req, dataBuffer, ob, socketChannel, key));
				} else {
					sychronizedExecutor.execute(new ProcessOperation(req, dataBuffer, ob, socketChannel, key));
//					IcmResponse resp = process(ob, req);
//					if (req.getResponseFlag() == HAS_RESPONSE) {
//						dataBuffer.clear();
//						writeResponse(dataBuffer, resp);
//						socketChannel.register(selector, SelectionKey.OP_WRITE, dataBuffer);
//					} else {
//						socketChannel.register(selector, SelectionKey.OP_READ);
//					}
				}
			}
		} catch (IOException e) {
			key.cancel();
			if (log.isInfoEnabled()) {
				log.info(e);
			}
		}
	}

	private class ProcessOperation implements Runnable {

		final IcmRequest req;
		final DataBuffer dataBuffer;
		final OperatingBean ob;
		final SocketChannel socketChannel;
		final SelectionKey key;

		public ProcessOperation(IcmRequest req, DataBuffer dataBuffer, OperatingBean ob, SocketChannel socketChannel,
				SelectionKey key) {
			this.req = req;
			this.dataBuffer = dataBuffer;
			this.ob = ob;
			this.socketChannel = socketChannel;
			this.key = key;
		}

		public void run() {
			IcmResponse resp = process(ob, req);
			if (req.getResponseFlag() == HAS_RESPONSE) {
				dataBuffer.clear();
				writeResponse(dataBuffer, resp);
				try {
					socketChannel.register(selector, SelectionKey.OP_WRITE, dataBuffer);
				} catch (ClosedChannelException e) {
					key.cancel();
					if (log.isErrorEnabled()) {
						log.error(e, e);
					}
				}
			}
			selector.wakeup();
		}
	}

	private int readBuffer(final SocketChannel socketChannel, DataBuffer dataBuffer) throws IOException {
		buffer.clear();
		int r = socketChannel.read(buffer);
		if (r < 0) {
			throw new IOException("The SocketChannel has been closed!");
		}
		buffer.flip();
		dataBuffer.put(buffer);
		return r;
	}

	private void readHead(final SocketChannel socketChannel) throws IOException {
		headBuffer.clear();
		for (int i = 0; i < MESSAGE_HEAD_SIZE;) {
			int r = socketChannel.read(headBuffer);
			if (r < 0) {
				throw new IOException("The SocketChannel has been closed!");
			}
			i += r;
		}
		headBuffer.flip();
	}

	private void write(final SelectionKey key) {
		final SocketChannel socketChannel = (SocketChannel) key.channel();
		DataBuffer dataBuffer = (DataBuffer) key.attachment();
		buffer.clear();
		dataBuffer.get(buffer);
		buffer.flip();
		try {
			socketChannel.write(buffer);
			if (dataBuffer.remaining() == 0) {
				socketChannel.register(selector, SelectionKey.OP_READ);
			} else {
				socketChannel.register(selector, SelectionKey.OP_WRITE, dataBuffer);
			}
		} catch (IOException e) {
			key.cancel();
			if (log.isErrorEnabled()) {
				log.error(e, e);
			}
		}
	}

	private class RequestHolder {
		private int length;
		private DataBuffer buffer;

		public RequestHolder() {}

		public RequestHolder(int length, DataBuffer buffer) {
			this();
			setLength(length);
			setBuffer(buffer);
		}

		public int getLength() {
			return length;
		}

		public void setLength(int length) {
			this.length = length;
		}

		public DataBuffer getBuffer() {
			return buffer;
		}

		public void setBuffer(DataBuffer buffer) {
			this.buffer = buffer;
		}

	}

}
