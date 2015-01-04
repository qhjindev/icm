package com.tnsoft.icm.icm4j;

import java.nio.ByteOrder;

import com.tnsoft.icm.io.DataBuffer;

public abstract class DataBufferProvider {

	protected ByteOrder byteOrder;
	
	public void setByteOrder(ByteOrder byteOrder) {
		this.byteOrder = byteOrder;
	}

	public abstract DataBuffer getBuffer();

	public void back(DataBuffer buffer) {}
}
