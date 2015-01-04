package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import com.tnsoft.icm.icm4j.protocol.MessageHead;
import com.tnsoft.icm.icm4j.protocol.Request;
import com.tnsoft.icm.icm4j.protocol.RequestHead;
import com.tnsoft.icm.icm4j.protocol.Response;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.util.BufferFactory;
import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;
import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

@Deprecated
public class TCPNioServer extends TCPICMServer implements Runnable {

	private static final Log log = LogFactory.getFormatterLog(TCPNioServer.class);

	private Thread serverThread;
	private Selector selector;
	private ServerSocketChannel serverSocketChannel;
	private ByteBuffer headBuffer = BufferFactory.getInstance().getBuffer(MessageHead.MESSAGE_HEAD_SIZE);
	private boolean running;
	private Object shutdown_lock = new Object();
	private ExecutorService executorService;

	public TCPNioServer(int port) {
		this(port, new ServiceDispatcher());
	}

	public TCPNioServer(int port, Charset charset) {
		this(port, charset, new ServiceDispatcher());
	}

	public TCPNioServer(int port, ServiceDispatcher serviceDispatcher) {
		super(port);
		setServiceDispatcher(serviceDispatcher);
	}

	public TCPNioServer(int port, Charset charset, ServiceDispatcher serviceDispatcher) {
		super(port, charset);
		setServiceDispatcher(serviceDispatcher);
	}

	public void setExecutorService(ExecutorService executorService) {
		this.executorService = executorService;
	}

	@Override
	public boolean isRunning() {
		return running;
	}

	@Override
	public void start() {
		if (executorService == null) {
			BlockingQueue<Runnable> workQueue = new LinkedBlockingDeque<Runnable>();
			executorService = new ThreadPoolExecutor(3, 8, 10, TimeUnit.SECONDS, workQueue);
		}
		try {
			selector = Selector.open();
			serverSocketChannel = ServerSocketChannel.open();
			serverSocketChannel.configureBlocking(false);
			InetSocketAddress address = new InetSocketAddress(port);
			serverSocketChannel.socket().bind(address);
			serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		running = true;
		serverThread = new Thread(this);
		serverThread.start();
		log.info("The TCP NIO ICM Server started at port [%d].", port);
	}

	@Override
	public void shutdown() {
		log.info("Stopping the TCP NIO ICM Servet at port [%d]...", port);
		log.debug("Stoping the thread pool...");
		executorService.shutdown();
		running = false;
		try {
			log.debug("Create a new Socket to wake up the server!");
			Socket socket = new Socket("localhost", port);
			socket.close();
			synchronized (shutdown_lock) {
				try {
					shutdown_lock.wait();
				} catch (InterruptedException e) {
					log.error(e);
				}
			}
			log.debug("Closing the server socket channel...");
			serverSocketChannel.close();
			log.debug("Closing the selector...");
			selector.close();
		} catch (UnknownHostException e) {
			log.error(e);
		} catch (IOException e) {
			log.error(e);
		}
		log.info("The TCP NIO ICM Server at port [%d] has stoped.", port);
	}

	public void run() {
		while (running) {
			try {
				selector.select();
				Iterator<SelectionKey> selectedKeys = selector.selectedKeys().iterator();
				while (selectedKeys.hasNext()) {
					SelectionKey key = selectedKeys.next();
					selectedKeys.remove();
					if (key.isValid()) {
						// Check what event is available and deal with it
						if (key.isAcceptable()) {
							accept(key);
						} else if (key.isReadable()) {
							read(key);
						} else if (key.isWritable()) {
							write(key);
						}
					}
				}
			} catch (IOException e) {
				log.error(e);
			}
		}
		synchronized (shutdown_lock) {
			shutdown_lock.notify();
		}
	}

	private void accept(SelectionKey key) {
		ServerSocketChannel serverSocketChannel = (ServerSocketChannel) key.channel();
		try {
			// Accept the connection and make it non-blocking
			SocketChannel socketChannel = serverSocketChannel.accept();
			socketChannel.configureBlocking(false);
			// Register the new SocketChannel with our Selector, indicating
			// we'd like to be notified when there's data waiting to be read
			socketChannel.register(selector, SelectionKey.OP_READ);
		} catch (IOException e) {
			log.error(e);
			key.cancel();
		}
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

	private void read(SelectionKey key) {
		SocketChannel socketChannel = (SocketChannel) key.channel();
		// Clear out our head buffer so it's ready for new data
		headBuffer.clear();
		try {
			int readCount = channelRead(socketChannel, headBuffer);
			if (readCount < 0) {
				// cancel the selection key and close the channel
				key.cancel();
				log.warn("A SocketChannel %s has closed!", socketChannel.toString());
				return;
			}
			headBuffer.flip();
			int messageSize = reader.readInt(headBuffer);
			byte type = reader.readByte(headBuffer);
			ByteBuffer buffer = BufferFactory.getInstance().getBuffer(messageSize);
			readCount = channelRead(socketChannel, buffer);
			if (readCount < 0) {
				// cancel the selection key and close the channel
				key.cancel();
				log.warn("A SocketChannel %s has closed!", socketChannel.toString());
				return;
			}
			buffer.flip();
			Request request = new Request(buffer);
			request.getMessageHead().setSize(messageSize);
			request.getMessageHead().setType(type);
			Response response = invokeService(request, reader, writer, key);
			if (request.getHead().getResponseFlag() == RequestHead.RESPONSE) {
				if (response != null) {
					socketChannel.register(selector, SelectionKey.OP_WRITE, response);
				} else {
					// the operation is an async operation, do nothing
				}
			} else {
				// no response, then wait for next request
			}
			// TODO other responseFlag type, can implement in future
		} catch (IOException e) {
			// The remote client forcibly closed the channel, cancel the selection key and close the channel.
			key.cancel();
			log.warn("It seems that the remote client %s closed the channel.", e, socketChannel.toString());
		}
	}

	public Response invokeService(final Request request, final BufferReader reader, final BufferWriter writer,
			final SelectionKey key) {
		request.readHead(reader);
		final OperatingBean ob = serviceDispatcher.getOperatingBean(request.getHead().getName(), request.getHead()
				.getOperation());
		Response response = null;
		if (ob != null) { // matcher an operation
			if (ob.isAsync()) {
				// is async method
				log.debug("The operation [%s#%s] is an async operation", request.getHead().getName(), request.getHead()
						.getOperation());
				Runnable invoker = new Runnable() {
					public void run() {
						log.debug("Doing async operation in [%s]", Thread.currentThread().toString());
						Response r = sendResponse(request, reader, writer, ob);
						if (r != null && (r.getBody().getOutArguments().size() > 0 || ob.getReturnType() != void.class)) {
							r.writeBuffer(writer);
							try {
								((SocketChannel) key.channel()).write(r.getBuffer());
							} catch (IOException e) {
								log.error(e);
								key.cancel();
							}
						}
					}
				};
				executorService.execute(invoker);
			} else {
				response = sendResponse(request, reader, writer, ob);
				if (response != null && (response.getBody().getOutArguments().size() > 0 || ob.getReturnType() != void.class)) {
					response.writeBuffer(writer);
				}
			}
			// the operation hasn't a result, return null
		} else {
			// can not matcher an operation
			log.warn("Can't matcher any operating with name [%s] and operation [%s] !", request.getHead().getName(),
					request.getHead().getOperation());
		}
		return response;
	}

	private Response sendResponse(final Request request, final BufferReader reader, final BufferWriter writer,
			final OperatingBean ob) {
		Response response = null;
		request.readBody(reader, ob.getOperationMethod());
		Object result = ob.doOperation(request.getBody().getArguments());
		if (request.getHead().getResponseFlag() == RequestHead.RESPONSE) {
			response = new Response(request.getHead().getId());
			List<Object> outs = new ArrayList<Object>();
			Class<?>[] types = request.getBody().getTypes();
			for (int i = 0; i < types.length; i++) {
				if (types[i].isAssignableFrom(Out.class)) {
					outs.add(((Out<?>) request.getBody().getArguments()[i]).get());
				}
			}
			response.setOutParameters(ob.getOperationMethod(), outs);
			if (ob.getReturnType() != void.class) {
				// the operation has result
				response.hasResponse(true);
				response.setResult(result, ob.getOperationMethod());
			}
		}
		return response;
	}

	private void write(SelectionKey key) {
		SocketChannel socketChannel = (SocketChannel) key.channel();
		Response response = (Response) key.attachment();
		ByteBuffer buffer = response.getBuffer();
		try {
			socketChannel.write(buffer);
			socketChannel.register(selector, SelectionKey.OP_READ, null);
		} catch (IOException e) {
			// The remote client forcibly closed the channel, cancel the selection key and close the channel.
			key.cancel();
			log.warn("It seems that the remote client %s closed the channel.", e, socketChannel.toString());
		}
	}

}
