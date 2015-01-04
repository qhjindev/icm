package com.tnsoft.icm.icm4j;

import com.tnsoft.icm.io.BufferReader;
import com.tnsoft.icm.io.BufferWriter;
import com.tnsoft.icm.io.DataBuffer;

public interface ReaderWriterProvider {

	BufferWriter getWriter(DataBuffer buffer);

	BufferReader getReader(DataBuffer buffer);
}

class CppCachedReaderWriterProvider implements ReaderWriterProvider {

	public BufferWriter getWriter(DataBuffer buffer) {
		return BufferWriter.getCachedCppWriter(buffer);
	}

	public BufferReader getReader(DataBuffer buffer) {
		return BufferReader.getCachedCppReader(buffer);
	}

}
