package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingDeque;

import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public class PooledSocketProvider implements SocketProvider {

	@SuppressWarnings("unused")
	private static final Log log = LogFactory.getFormatterLog(PooledSocketProvider.class);

	private final ConcurrentHashMap<String, SimpleSocketPool> pools = new ConcurrentHashMap<String, SimpleSocketPool>();
	private final Object pools_lock = new Object();
	private final int maxInPool;
	private boolean alive;

	public PooledSocketProvider() {
		this(3);
	}

	public PooledSocketProvider(int maxInPool) {
		this.alive = true;
		this.maxInPool = maxInPool;
	}

	private String generateKey(String hostname, int port) {
		return hostname + ':' + port;
	}

	private SimpleSocketPool getPool(String hostname, int port) {
		String key = generateKey(hostname, port);
		SimpleSocketPool pool = pools.get(key);
		if (pool == null) {
			synchronized (pools_lock) {
				pool = pools.get(key);
				if (pool == null) {
					pool = new SimpleSocketPool(hostname, port, maxInPool);
				}
				pools.put(key, pool);
			}
		}
		return pool;
	}

	public Socket get(String hostname, int port) throws UnknownHostException, IOException {
		Socket socket = null;
		if (alive) {
			SimpleSocketPool pool = getPool(hostname, port);
			socket = pool.get();
		}
		return socket;
	}

	public void back(Socket socket) {
		if (alive) {
			if (socket != null) {
				SimpleSocketPool pool = getPool(socket.getInetAddress().getHostName(), socket.getPort());
				pool.back(socket);
			}
		}
	}

	public synchronized void shutdown() {
		if (alive) {
			alive = false;
			for (SimpleSocketPool pool : pools.values()) {
				pool.shutdown();
			}
			pools.clear();
		}
	}

}

class SimpleSocketPool {

	private static final Log log = LogFactory.getFormatterLog(SimpleSocketPool.class);

	private LinkedBlockingDeque<Socket> deque;
	private boolean alive;
	private String hostname;
	private int port;

	public SimpleSocketPool(String hostname, int port, int max) {
		this.hostname = hostname;
		this.port = port;
		alive = true;
		deque = new LinkedBlockingDeque<Socket>(max);
	}

	public Socket get() throws UnknownHostException, IOException {
		Socket socket = null;
		if (alive) {
			socket = deque.pollLast();
			if (socket == null) {
				socket = new Socket(hostname, port);
			}
		}
		return socket;
	}

	public void back(Socket socket) {
		if (alive) {
			if (!deque.offerLast(socket)) {
				close(socket);
			}
		}
	}

	public synchronized void shutdown() {
		if (alive) {
			alive = false;
			Socket socket = null;
			while ((socket = deque.poll()) != null) {
				close(socket);
			}
			deque = null;
		}
	}

	private void close(Socket socket) {
		try {
			socket.close();
		} catch (IOException e) {
			log.error(e.getMessage(), e);
		}
	}

}
