package com.tnsoft.icm.util;

import java.lang.reflect.ParameterizedType;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Map;
import java.util.Set;

public interface BufferReader {
	
	Set<?> readSet(ByteBuffer buffer, ParameterizedType type);
	
	List<?> readList(ByteBuffer buffer, ParameterizedType type);
	
	Map<?, ?> readMap(ByteBuffer buffer, ParameterizedType type);
	
	Object readObject(ByteBuffer buffer, Class<?> type);
	
	Object readArray(ByteBuffer buffer, Class<?> componentType);

	byte readByte(ByteBuffer buffer);
	
	short readShort(ByteBuffer buffer);
	
	int readInt(ByteBuffer buffer);
	
	long readLong(ByteBuffer buffer);
	
	char readChar(ByteBuffer buffer);
	
	boolean readBoolean(ByteBuffer buffer);
	
	float readFloat(ByteBuffer buffer);
	
	double readDouble(ByteBuffer buffer);
	
	String readString(ByteBuffer buffer);
}
