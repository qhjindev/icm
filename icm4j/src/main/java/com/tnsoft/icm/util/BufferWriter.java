package com.tnsoft.icm.util;

import java.lang.reflect.ParameterizedType;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Map;
import java.util.Set;

public interface BufferWriter {
	
	void writeSet(ByteBuffer buffer, Set<?> set, ParameterizedType type);
	
	void writeList(ByteBuffer buffer, List<?> list, ParameterizedType type);
	
	void writeMap(ByteBuffer buffer, Map<?, ?> map, ParameterizedType type);
	
	void writeObject(ByteBuffer buffer, Object value, Class<?> type);
	
	void writeArray(ByteBuffer buffer, Object array, Class<?> componentType);

	void writeByte(ByteBuffer buffer, byte value);
	
	void writeShort(ByteBuffer buffer, short value);
	
	void writeInt(ByteBuffer buffer, int value);
	
	void writeLong(ByteBuffer buffer, long value);
	
	void writeChar(ByteBuffer buffer, char value);
	
	void writeBoolean(ByteBuffer buffer, boolean value);
	
	void writeFloat(ByteBuffer buffer, float value);
	
	void writeDouble(ByteBuffer buffer, double value);
	
	void writeString(ByteBuffer buffer, String value);
}
