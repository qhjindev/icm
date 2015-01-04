package com.tnsoft.icm.icm4j;

import java.nio.charset.Charset;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.LinkedBlockingQueue;

import com.tnsoft.icm.icm4j.io.Buffer;
import com.tnsoft.icm.icm4j.io.BufferWriter;

public class CachedBufferWriterProvider extends DefaultBufferWriterProvider {

	private static final class InstanceHolder {
		private static final CachedBufferWriterProvider instance = new CachedBufferWriterProvider(20);
	}
	
	public static CachedBufferWriterProvider getInstance() {
		return InstanceHolder.instance;
	}
	
	private final ConcurrentMap<Charset, BlockingQueue<CachedWriter>> cacheMap = new ConcurrentHashMap<Charset, BlockingQueue<CachedWriter>>();
	private final int cacheSize;

	public CachedBufferWriterProvider(int cacheSize) {
		this.cacheSize = cacheSize;
	}

	private BlockingQueue<CachedWriter> getCache(Charset charset) {
		BlockingQueue<CachedWriter> cache = cacheMap.get(charset);
		if (cache == null) {
			BlockingQueue<CachedWriter> new_cache = new LinkedBlockingQueue<CachedWriter>(cacheSize);
			cache = cacheMap.putIfAbsent(charset, new_cache);
			if (cache == null) {
				cache = new_cache;
			}
		}
		return cache;
	}

	private void cache(CachedWriter writer, Charset charset) {
		BlockingQueue<CachedWriter> cache = getCache(charset);
		cache.offer(writer);
	}

	@Override
	public BufferWriter getWriter(Buffer buffer, Charset charset) {
		CachedWriter writer = getCache(charset).poll();
		if (writer == null) {
			writer = new CachedWriter(buffer, charset);
		} else {
			writer.setBuffer(buffer);
		}
		return writer;
	}

	private class CachedWriter extends BufferWriter {

		public CachedWriter(Buffer buffer, Charset charset) {
			super(buffer, charset);
		}

		private void setBuffer(Buffer buffer) {
			this.buffer = buffer;
		}

		public void close() {
			super.close();
			cache(this, charset);
		}
	}

}
