package com.tnsoft.icm.icm4j;

import java.nio.ByteOrder;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import com.tnsoft.icm.io.DataBuffer;

public class CachedDataBufferProvider extends DataBufferProvider {

	private static final int DEFAULT_CAPACITY = 10;
	private final BlockingQueue<DataBuffer> cache;

	public CachedDataBufferProvider(ByteOrder byteOrder) {
		this(DEFAULT_CAPACITY, byteOrder);
	}

	public CachedDataBufferProvider(int capacity, ByteOrder byteOrder) {
		cache = new LinkedBlockingQueue<DataBuffer>();
		setByteOrder(byteOrder);
	}

	public DataBuffer getBuffer() {
		DataBuffer buffer = cache.poll();
		return buffer == null ? new DataBuffer(byteOrder) : buffer;
	}

	@Override
	public void back(DataBuffer buffer) {
		buffer.clear();
		cache.offer(buffer);
	}

}
