package com.tnsoft.icm.icm4j;

import java.nio.ByteOrder;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.LinkedBlockingQueue;

import com.tnsoft.icm.icm4j.io.Buffer;

public class CachedBufferProvider implements BufferProvider {

	private static final int DEFAULT_CAPACITY = 128;
	private static final int MAX_CACHED_CAPACITY = 4 * DEFAULT_CAPACITY;
	
	private static class InstanceHolder {
		private static final ConcurrentMap<ByteOrder, CachedBufferProvider> instanceMap = new ConcurrentHashMap<ByteOrder, CachedBufferProvider>();		
	}
	
	public static CachedBufferProvider getInstance(ByteOrder byteOrder) {
		CachedBufferProvider cachedBufferProvider = InstanceHolder.instanceMap.get(byteOrder);
		if (cachedBufferProvider == null) {
			CachedBufferProvider new_provider = new CachedBufferProvider(20, byteOrder);
			cachedBufferProvider = InstanceHolder.instanceMap.putIfAbsent(byteOrder, new_provider);
			if (cachedBufferProvider == null) {
				cachedBufferProvider = new_provider;
			}
		}
		return cachedBufferProvider;
	}

	private final BlockingQueue<Buffer> cache;
	private final ByteOrder byteOrder;

	public CachedBufferProvider(int cacheSize, ByteOrder byteOrder) {
		cache = new LinkedBlockingQueue<Buffer>(cacheSize);
		this.byteOrder = byteOrder;
	}

	public Buffer getBuffer() {
		Buffer buffer = cache.poll();
		if (buffer == null) {
			buffer = Buffer.allocate(DEFAULT_CAPACITY, byteOrder);
		}
		return buffer;
	}

	public void destroy(Buffer buffer) {
		if (buffer.capacity() <= MAX_CACHED_CAPACITY) {
			buffer.clear();
			cache.offer(buffer);
		}
	}

}
