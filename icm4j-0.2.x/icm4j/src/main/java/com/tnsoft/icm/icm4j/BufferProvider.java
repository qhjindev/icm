package com.tnsoft.icm.icm4j;

import java.nio.ByteOrder;

import com.tnsoft.icm.icm4j.io.Buffer;

public interface BufferProvider {

	Buffer getBuffer();

	void destroy(Buffer buffer);
}

class DefaultBufferProvider implements BufferProvider {

	private final ByteOrder byteOrder;

	public DefaultBufferProvider(ByteOrder byteOrder) {
		this.byteOrder = byteOrder;
	}

	public Buffer getBuffer() {
		return Buffer.allocate(128, byteOrder);
	}

	public void destroy(Buffer buffer) {}
}