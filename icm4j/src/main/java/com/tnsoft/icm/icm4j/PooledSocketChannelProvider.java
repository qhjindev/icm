package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.SocketChannel;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class PooledSocketChannelProvider implements SocketChannelProvider {

	private static final Log log = LogFactory.getLog(PooledSocketChannelProvider.class);

	private final ConcurrentHashMap<String, SimpleSocketChannelPool> pools = new ConcurrentHashMap<String, SimpleSocketChannelPool>();
	private final Object pools_lock = new Object();
	private final int maxInPool;
	private boolean alive;

	public PooledSocketChannelProvider() {
		this(3);
	}

	public PooledSocketChannelProvider(int maxInPool) {
		this.maxInPool = maxInPool;
		this.alive = true;
	}

	private String generateKey(String hostname, int port) {
		return hostname + ':' + port;
	}
	
	private SimpleSocketChannelPool getSocketChannelPool(String hostname, int port) {
		String key = generateKey(hostname, port);
		SimpleSocketChannelPool pool = pools.get(key);
		if (pool == null) {
			synchronized (pools_lock) {
				pool = pools.get(key);
				if (pool == null) {
					pool = new SimpleSocketChannelPool(hostname, port, maxInPool);
				}
				pools.put(key, pool);
			}
		}
		return pool;
	}

	public SocketChannel get(String hostname, int port) throws IOException {
		if (alive) {
			return getSocketChannelPool(hostname, port).get();
		}
		return null;
	}

	public void back(SocketChannel channel) {
		if (alive && channel != null) {
			getSocketChannelPool(channel.socket().getInetAddress().getHostName(), channel.socket().getPort()).back(channel);
		}
	}

	public void shutdown() {
		if (alive) {
			alive = false;
			for (SimpleSocketChannelPool pool : pools.values()) {
				pool.shutdown();
			}
			pools.clear();
		}
	}

	class SimpleSocketChannelPool {
		private LinkedBlockingQueue<SocketChannel> deque;
		private boolean alive;
		private String hostname;
		private int port;

		public SimpleSocketChannelPool(String hostname, int port, int max) {
			this.hostname = hostname;
			this.port = port;
			this.alive = true;
			this.deque = new LinkedBlockingQueue<SocketChannel>(max);
		}

		public SocketChannel get() throws IOException {
			SocketChannel sc = null;
			if (alive) {
				sc = deque.poll();
				if (sc == null) {
					sc = SocketChannel.open(new InetSocketAddress(hostname, port));
				}
			}
			return sc;
		}

		public void back(SocketChannel channel) {
			if (alive) {
				if (!deque.offer(channel)) {
					close(channel);
				}
			}
		}
		
		public synchronized void shutdown() {
			if (alive) {
				alive = false;
				SocketChannel sc = null;
				while ((sc = deque.poll()) != null) {
					close(sc);
				}
				deque = null;
			}
		}

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
}
