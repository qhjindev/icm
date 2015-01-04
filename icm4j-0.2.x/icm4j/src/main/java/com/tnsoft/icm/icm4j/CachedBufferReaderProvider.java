package com.tnsoft.icm.icm4j;

import java.nio.charset.Charset;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.LinkedBlockingQueue;

import com.tnsoft.icm.icm4j.io.Buffer;
import com.tnsoft.icm.icm4j.io.BufferReader;

public class CachedBufferReaderProvider extends DefaultBufferReaderProvider {
	
	private static class InstanceHolder {
		private static final CachedBufferReaderProvider instance = new CachedBufferReaderProvider(20);
	}
	
	public static CachedBufferReaderProvider getInstance() {
		return InstanceHolder.instance;
	}

	private final ConcurrentMap<Charset, BlockingQueue<CachedReader>> cacheMap = new ConcurrentHashMap<Charset, BlockingQueue<CachedReader>>();
	private final int cacheSize;

	public CachedBufferReaderProvider(int cacheSize) {
		this.cacheSize = cacheSize;
	}

	private BlockingQueue<CachedReader> getCache(Charset charset) {
		BlockingQueue<CachedReader> cache = cacheMap.get(charset);
		if (cache == null) {
			BlockingQueue<CachedReader> new_cache = new LinkedBlockingQueue<CachedReader>(cacheSize); 
			cache = cacheMap.putIfAbsent(charset, new_cache);
			if (cache == null) {
				cache = new_cache;
			}
		}
		return cache;
	}

	private void cache(CachedReader reader, Charset charset) {
		BlockingQueue<CachedReader> cache = getCache(charset);
		cache.offer(reader);
	}

	@Override
	public BufferReader getReader(Buffer buffer, Charset charset) {
		CachedReader reader = getCache(charset).poll();
		if (reader == null) {
			reader = new CachedReader(buffer, charset);
		} else {
			reader.setBuffer(buffer);
		}
		return reader;
	}

	private class CachedReader extends BufferReader {

		public CachedReader(Buffer buffer, Charset charset) {
			super(buffer, charset);
		}

		private void setBuffer(Buffer buffer) {
			this.buffer = buffer;
		}

		@Override
		public void close() {
			super.close();
			cache(this, charset);
		}

	}
}
