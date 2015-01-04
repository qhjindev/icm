package com.tnsoft.icm.icm4j;

import java.nio.charset.Charset;

import com.tnsoft.icm.icm4j.io.Buffer;
import com.tnsoft.icm.icm4j.io.BufferReader;

public interface BufferReaderProvider {

	BufferReader getReader(Buffer buffer, Charset charset);
}

class DefaultBufferReaderProvider implements BufferReaderProvider {

	public BufferReader getReader(Buffer buffer, Charset charset) {
		return new BufferReader(buffer, charset);
	}
	
}