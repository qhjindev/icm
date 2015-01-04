package com.tnsoft.icm.io;

import java.lang.reflect.Array;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingDeque;

import com.tnsoft.icm.util.StructBeanInfo;
import com.tnsoft.icm.util.StructBeanInfo.Property;

public abstract class BufferReader extends BufferIO {

	public static BufferReader getDefaultReader(DataBuffer dataBuffer) {
		return getDefaultReader(dataBuffer, DEFAULT_CHARSET);
	}

	public static BufferReader getDefaultReader(DataBuffer dataBuffer, Charset charset) {
		return new DefaultBufferReader(dataBuffer, charset);
	}
	
	public static BufferReader getCppReader(DataBuffer dataBuffer) {
		return getCppReader(dataBuffer, DEFAULT_CHARSET);
	}
	
	public static BufferReader getCppReader(DataBuffer dataBuffer, Charset charset) {
		return new CppBufferReader(dataBuffer, charset);
	}
	
	public static BufferReader getCachedDefaultReader(DataBuffer dataBuffer) {
		return getCachedDefaultReader(dataBuffer, DEFAULT_CHARSET);
	}
	
	public static BufferReader getCachedDefaultReader(DataBuffer dataBuffer, Charset charset) {
		return CachedDefaultBufferReader.getReader(dataBuffer, charset);
	}
	
	public static BufferReader getCachedCppReader(DataBuffer dataBuffer) {
		return getCachedCppReader(dataBuffer, DEFAULT_CHARSET);
	}
	
	public static BufferReader getCachedCppReader(DataBuffer dataBuffer, Charset charset) {
		return CachedCppBufferReader.getReader(dataBuffer, DEFAULT_CHARSET);
	}

	public BufferReader(DataBuffer dataBuffer, Charset charset) {
		this.dataBuffer = dataBuffer;
		this.charset = charset;
	}

	public abstract Set<?> readSet(ParameterizedType type);

	public abstract List<?> readList(ParameterizedType type);

	public abstract Map<?, ?> readMap(ParameterizedType type);

	public abstract Object readObject(Class<?> type);

	public abstract Object readArray(Class<?> componentType);

	public abstract String readString();

	public void close() {
		dataBuffer = null;
		closed = true;
	}

	public byte readByte() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		return dataBuffer.get();
	}

	public short readShort() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		return dataBuffer.getShort();
	}

	public int readInt() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		return dataBuffer.getInt();
	}

	public long readLong() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		return dataBuffer.getLong();
	}

	public char readChar() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		return dataBuffer.getChar();
	}

	public boolean readBoolean() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		return readByte() == 0 ? false : true;
	}

	public float readFloat() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		return dataBuffer.getFloat();
	}

	public double readDouble() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		return dataBuffer.getDouble();
	}

}

class DefaultBufferReader extends BufferReader {

	public DefaultBufferReader(DataBuffer dataBuffer, Charset charset) {
		super(dataBuffer, charset);
	}

	@Override
	public Set<?> readSet(ParameterizedType type) {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		int size = readShort();
		if (size < 0) {
			return null;
		}
		Class<?> elementType = (Class<?>) type.getActualTypeArguments()[0];
		Set<Object> set = new LinkedHashSet<Object>();
		for (int i = 0; i < size; i++) {
			set.add(readObject(elementType));
		}
		return set;
	}

	@Override
	public List<?> readList(ParameterizedType type) {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		Class<?> elementType = (Class<?>) type.getActualTypeArguments()[0];
		long size = 0;
		if (elementType.isPrimitive() || elementType == Byte.class || elementType == Short.class|| elementType == Integer.class || elementType == Long.class|| elementType == Character.class || elementType == Boolean.class|| elementType == Float.class || elementType == Double.class|| elementType == Boolean.class) {
			size = readLong();
		} else if (elementType == String.class) {
			size = readInt();
		} else {			
			size = readShort();
		}
		if (size < 0) {
			return null;
		}
		List<Object> list = new ArrayList<Object>();
		for (int i = 0; i < size; i++) {
			list.add(readObject(elementType));
		}
		return list;
	}

	@Override
	@SuppressWarnings({ "rawtypes", "unchecked" })
	public Map<?, ?> readMap(ParameterizedType type) {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		int size = readShort();
		if (size < 0) {
			return null;
		}
		Type[] types = type.getActualTypeArguments();
		Class<?> keyType = (Class<?>) types[0];
		Class<?> valueType = (Class<?>) types[1];
		Map map = new LinkedHashMap(size);
		for (int i = 0; i < size; i++) {
			Object key = readObject(keyType);
			Object value = readObject(valueType);
			map.put(key, value);
		}
		return map;
	}

	@Override
	public Object readObject(Class<?> type) {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		if (type.isArray()) {
			return readArray(type.getComponentType());
		} else if (byte.class == type || Byte.class == type) {
			return readByte();
		} else if (short.class == type || Short.class == type) {
			return readShort();
		} else if (int.class == type || Integer.class == type) {
			return readInt();
		} else if (long.class == type || Long.class == type) {
			return readLong();
		} else if (boolean.class == type || Boolean.class == type) {
			return readBoolean();
		} else if (float.class == type || Float.class == type) {
			return readFloat();
		} else if (double.class == type || Double.class == type) {
			return readDouble();
		} else if (char.class == type || Character.class == type) {
			return readChar();
		} else if (String.class == type) {
			return readString();
		} else if (type.isEnum()) {
			String stringvalue = readString();
			Object[] es = type.getEnumConstants();
			for (int i = 0; i < es.length; i++) {
				if (stringvalue.equals(es[i].toString())) {
					return es[i];
				}
			}
			return null;
		} else {
			StructBeanInfo sbi = getStructBeanInfo(type);
			Object object = sbi.newInstance();
			Property[] properties = sbi.getProperties();
			for (Property property : properties) {
				Class<?> pt = property.getType();
				if (pt.isAssignableFrom(Set.class)) {
					property.setValue(object, readSet((ParameterizedType) property.getGenericType()));
				} else if (pt.isAssignableFrom(List.class)) {
					property.setValue(object, readList((ParameterizedType) property.getGenericType()));
				} else if (pt.isAssignableFrom(Map.class)) {
					property.setValue(object, readMap((ParameterizedType) property.getGenericType()));
				} else {
					property.setValue(object, readObject(pt));
				}
			}
			return object;
		}
	}

	@Override
	public Object readArray(Class<?> componentType) {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		int length = (int) readLong();
		if (length < 0) {
			return null;
		}
		Object result = Array.newInstance(componentType, length);
		for (int i = 0; i < length; i++) {
			Array.set(result, i, readObject(componentType));
		}
		return result;
	}

	@Override
	public String readString() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		int length = readInt();
		if (length < 0) {
			return null;
		} else {
			byte[] b = new byte[length];
			dataBuffer.get(b);
			return new String(b, DEFAULT_CHARSET);
		}
	}

}

class CppBufferReader extends DefaultBufferReader {

	public CppBufferReader(DataBuffer dataBuffer, Charset charset) {
		super(dataBuffer, charset);
	}

	@Override
	public String readString() {
		if (isClosed()) {
			throw new ReadClosedReaderException();
		}
		int length = readInt();
		if (length < 0) {
			return null;
		} else {
			byte[] b = new byte[length];
			dataBuffer.get(b);
			return new String(b, 0, length - 1, DEFAULT_CHARSET);
		}
	}
}

class CachedDefaultBufferReader extends DefaultBufferReader {

	private static final int MAX = 5;

	private static final ConcurrentHashMap<Charset, LinkedBlockingDeque<CachedDefaultBufferReader>> cache = new ConcurrentHashMap<Charset, LinkedBlockingDeque<CachedDefaultBufferReader>>();

	private static LinkedBlockingDeque<CachedDefaultBufferReader> getDeque(Charset charset) {
		LinkedBlockingDeque<CachedDefaultBufferReader> deque = cache.get(charset);
		if (deque == null) {
			deque = new LinkedBlockingDeque<CachedDefaultBufferReader>(MAX);
			cache.put(charset, deque);
		}
		return deque;
	}

	public static CachedDefaultBufferReader getReader(DataBuffer dataBuffer, Charset charset) {
		LinkedBlockingDeque<CachedDefaultBufferReader> deque = getDeque(charset);
		CachedDefaultBufferReader reader = deque.pollFirst();
		if (reader == null) {
			reader = new CachedDefaultBufferReader(dataBuffer, charset);
		} else {
			reader.dataBuffer = dataBuffer;
			reader.closed = false;
		}
		return reader;
	}

	private static void back(CachedDefaultBufferReader reader) {
		LinkedBlockingDeque<CachedDefaultBufferReader> deque = getDeque(reader.getCharset());
		deque.offerFirst(reader);
	}

	private CachedDefaultBufferReader(DataBuffer dataBuffer, Charset charset) {
		super(dataBuffer, charset);
	}

	@Override
	public void close() {
		super.close();
		back(this);
	}
}

class CachedCppBufferReader extends CppBufferReader {

	private static final int MAX = 5;

	private static final ConcurrentHashMap<Charset, LinkedBlockingDeque<CachedCppBufferReader>> cache = new ConcurrentHashMap<Charset, LinkedBlockingDeque<CachedCppBufferReader>>();

	private static LinkedBlockingDeque<CachedCppBufferReader> getDeque(Charset charset) {
		LinkedBlockingDeque<CachedCppBufferReader> deque = cache.get(charset);
		if (deque == null) {
			deque = new LinkedBlockingDeque<CachedCppBufferReader>(MAX);
			cache.put(charset, deque);
		}
		return deque;
	}

	public static CachedCppBufferReader getReader(DataBuffer dataBuffer, Charset charset) {
		LinkedBlockingDeque<CachedCppBufferReader> deque = getDeque(charset);
		CachedCppBufferReader reader = deque.pollFirst();
		if (reader == null) {
			reader = new CachedCppBufferReader(dataBuffer, charset);
		} else {
			reader.dataBuffer = dataBuffer;
			reader.closed = false;
		}
		return reader;
	}

	private static void back(CachedCppBufferReader reader) {
		LinkedBlockingDeque<CachedCppBufferReader> deque = getDeque(reader.getCharset());
		deque.offerFirst(reader);
	}

	private CachedCppBufferReader(DataBuffer dataBuffer, Charset charset) {
		super(dataBuffer, charset);
	}

	@Override
	public void close() {
		super.close();
		back(this);
	}

}
