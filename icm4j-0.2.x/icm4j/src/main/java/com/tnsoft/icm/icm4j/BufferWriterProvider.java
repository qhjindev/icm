package com.tnsoft.icm.icm4j;

import java.nio.charset.Charset;

import com.tnsoft.icm.icm4j.io.Buffer;
import com.tnsoft.icm.icm4j.io.BufferWriter;

public interface BufferWriterProvider {

	BufferWriter getWriter(Buffer buffer, Charset charset);
}

class DefaultBufferWriterProvider implements BufferWriterProvider {

	public BufferWriter getWriter(Buffer buffer, Charset charset) {
		return new BufferWriter(buffer, charset);
	}

}
