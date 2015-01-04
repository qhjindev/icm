package com.tnsoft.icm.util;

import java.util.Collection;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class PoolProviderFactory {

	public static PoolProvider newSimpleChannelPoolProvider() {
		return new SimpleChannelPoolProvider();
	}
	
	public static PoolProvider newDequeChannelPoolProvider(int maxWait) {
		return new DequeChannelPoolProvider(maxWait);
	}
	
	public static PoolProvider newDequeChannelPoolProvider() {
		return new DequeChannelPoolProvider(3);
	} 
	
	private static abstract class PoolProviderImpl implements PoolProvider {

		protected static Map<String, ChannelPool> poolMap = new ConcurrentHashMap<String, ChannelPool>();
		
		protected static String generatePoolKey(String hostname, int port) {
			return hostname + ':' + port;
		}

		private Object lock = new Object();

		protected abstract ChannelPool createNewPool(String hostname, int port);

		public ChannelPool getPool(String hostname, int port) {
			String key = generatePoolKey(hostname, port);
			ChannelPool pool = poolMap.get(key);
			if (pool == null) {
				synchronized (lock) {
					pool = poolMap.get(key);
					if (pool == null) {
						pool = createNewPool(hostname, port);
						poolMap.put(key, pool);
					}
				}
			}
			return pool;
		}

		public synchronized void stopAllPools() {
			Collection<ChannelPool> channelPools = poolMap.values();
			for (Iterator<ChannelPool> iterator = channelPools.iterator(); iterator.hasNext();) {
				ChannelPool channelPool = iterator.next();
				channelPool.stopPool();
			}
			poolMap.clear();
		}
	}

	public static class SimpleChannelPoolProvider extends PoolProviderImpl {

		@Override
		protected ChannelPool createNewPool(String hostname, int port) {
			return new SimpleChannelPool(hostname, port);
		}

	}

	public static class DequeChannelPoolProvider extends PoolProviderImpl {

		private int maxWait;

		public int getMaxWait() {
			return maxWait;
		}

		public void setMaxWait(int maxWait) {
			this.maxWait = maxWait;
		}
		
		public DequeChannelPoolProvider(int maxWait) {
			setMaxWait(maxWait);
		}

		@Override
		protected ChannelPool createNewPool(String hostname, int port) {
			return new DequeChannelPool(hostname, port, maxWait);
		}

	}
}
