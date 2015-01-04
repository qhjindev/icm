package com.tnsoft.icm.util;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class BufferFactory {

	private static final BufferFactory instance = new BufferFactory();

	public static BufferFactory getInstance() {
		return instance;
	}
	
	private ByteOrder order;
	
	private BufferFactory() {
		this(ByteOrder.BIG_ENDIAN);
	}
	
	private BufferFactory(ByteOrder order) {
		this.order = order;
	}
	
	public ByteBuffer getBuffer(int size) {
		ByteBuffer buffer = ByteBuffer.allocate(size);
		if (!order.equals(buffer.order())) {
			buffer.order(order);
		}
		return buffer;
	}
}
