package com.tnsoft.icm.io;

import java.lang.reflect.Array;
import java.lang.reflect.ParameterizedType;
import java.nio.charset.Charset;
import java.util.Collection;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingDeque;

import com.tnsoft.icm.util.StructBeanInfo;
import com.tnsoft.icm.util.StructBeanInfo.Property;

public abstract class BufferWriter extends BufferIO {

	public static BufferWriter getDefaultWriter(DataBuffer dataBuffer) {
		return new DefaultBufferWriter(dataBuffer, DEFAULT_CHARSET);
	}

	public static BufferWriter getDefaultWriter(DataBuffer dataBuffer, Charset charset) {
		return new DefaultBufferWriter(dataBuffer, charset);
	}

	public static BufferWriter getCppWriter(DataBuffer dataBuffer) {
		return new CppBufferWriter(dataBuffer, DEFAULT_CHARSET);
	}

	public static BufferWriter getCppWriter(DataBuffer dataBuffer, Charset charset) {
		return new CppBufferWriter(dataBuffer, charset);
	}

	public static BufferWriter getCachedDefaultWriter(DataBuffer dataBuffer) {
		return getCachedDefaultWriter(dataBuffer, DEFAULT_CHARSET);
	}

	public static BufferWriter getCachedDefaultWriter(DataBuffer dataBuffer, Charset charset) {
		return CachedDefaultBufferWriter.getWriter(dataBuffer, charset);
	}

	public static BufferWriter getCachedCppWriter(DataBuffer dataBuffer) {
		return getCachedCppWriter(dataBuffer, DEFAULT_CHARSET);
	}

	public static BufferWriter getCachedCppWriter(DataBuffer dataBuffer, Charset charset) {
		return CachedCppBufferWriter.getWriter(dataBuffer, charset);
	}

	public BufferWriter(DataBuffer dataBuffer, Charset charset) {
		this.dataBuffer = dataBuffer;
		this.charset = charset;
	}

	public void close() {
		closed = true;
		dataBuffer.flip();
		dataBuffer = null;
	}

	public void writeByte(byte value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		dataBuffer.put(value);
	}

	public void writeShort(short value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		dataBuffer.putShort(value);
	}

	public void writeInt(int value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		dataBuffer.putInt(value);
	}

	public void writeLong(long value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		dataBuffer.putLong(value);
	}

	public void writeChar(char value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		dataBuffer.putInt(value);
	}

	public void writeBoolean(boolean value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		dataBuffer.put((byte) (value ? 1 : 0));
	}

	public void writeFloat(float value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		dataBuffer.putFloat(value);
	}

	public void writeDouble(double value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		dataBuffer.putDouble(value);
	}

	public abstract void writeString(String value);

	public abstract void writeCollection(Collection<?> collection, Class type);

	public abstract void writeMap(Map<?, ?> map);

	public abstract void writeObject(Object value);

	public abstract void writeArray(Object array);
}

class DefaultBufferWriter extends BufferWriter {

	public DefaultBufferWriter(DataBuffer dataBuffer, Charset charset) {
		super(dataBuffer, charset);
	}

	@Override
	public void writeString(String value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		if (value == null) {
			writeInt(-1);
		} else {
			byte[] b = value.getBytes(charset);
			writeInt(b.length);
			dataBuffer.put(b);
		}
	}

	@Override
	public void writeCollection(Collection<?> set, Class type) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		if (set == null) {
			if (type.isPrimitive() || type == Byte.class || type == Short.class|| type == Integer.class || type == Long.class|| type == Character.class || type == Boolean.class|| type == Float.class || type == Double.class|| type == Boolean.class) {
				writeLong(0);
			} else if (type == String.class) {
				writeInt(0);
			} else {
				writeShort((short) 0);
			}
		} else {
			int length = set.size();
			if (type.isPrimitive() || type == Byte.class || type == Short.class|| type == Integer.class || type == Long.class|| type == Character.class || type == Boolean.class|| type == Float.class || type == Double.class|| type == Boolean.class) {
				writeLong(length);
			} else if (type == String.class) {
				writeInt(length);
			} else {
				writeShort((short) length);
			}
			for (Object object : set) {
				writeObject(object);
			}
		}
	}

	@Override
	public void writeMap(Map<?, ?> map) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		if (map == null) {
			writeShort((short) -1);
		} else {
			int length = map.size();
			writeShort((short) length);
			for (Entry<?, ?> entry : map.entrySet()) {
				writeObject(entry.getKey());
				writeObject(entry.getValue());
			}
		}
	}

	@Override
	public void writeArray(Object array) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		if (array == null) {
			writeLong(-1);
		} else {
			int length = Array.getLength(array);
			writeLong(length);
			for (int i = 0; i < length; i++) {
				Object value = Array.get(array, i);
				writeObject(value);
			}
		}
	}

	@Override
	public void writeObject(Object value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		if (value == null) {
			throw new NullPointerException("The object to be writed cannot be null!");
		}
		Class<?> type = value.getClass();
		if (type.isArray()) {
			writeArray(value);
		} else if (byte.class == type || Byte.class == type) {
			writeByte((Byte) value);
		} else if (short.class == type || Short.class == type) {
			writeShort((Short) value);
		} else if (int.class == type || Integer.class == type) {
			writeInt((Integer) value);
		} else if (long.class == type || Long.class == type) {
			writeLong((Long) value);
		} else if (boolean.class == type || Boolean.class == type) {
			writeBoolean((Boolean) value);
		} else if (float.class == type || Float.class == type) {
			writeFloat((Float) value);
		} else if (double.class == type || Double.class == type) {
			writeDouble((Double) value);
		} else if (char.class == type || Character.class == type) {
			writeChar((Character) value);
		} else if (String.class == type) {
			writeString((String) value);
		} else if (type.isEnum()) {
			writeString(value.toString());
		} else {
			StructBeanInfo sbi = getStructBeanInfo(type);
			Property[] properties = sbi.getProperties();
			for (Property property : properties) {
				Class<?> pt = property.getType();
				Object pv = property.getValue(value);
				if (pv instanceof Collection<?>) {
					Class pvt = (Class) ((ParameterizedType) property.getGenericType()).getActualTypeArguments()[0];
					writeCollection((Collection<?>) pv, pvt);
				} else if (pt.isAssignableFrom(Map.class)) {
					writeMap((Map<?, ?>) pv);
				} else {
					writeObject(pv);
				}
			}
		}
	}

}

class CppBufferWriter extends DefaultBufferWriter {

	public CppBufferWriter(DataBuffer dataBuffer, Charset charset) {
		super(dataBuffer, charset);
	}

	@Override
	public void writeString(String value) {
		if (closed) {
			throw new WriteClosedWriterException();
		}
		if (value == null) {
			writeInt(-1);
		} else {
			byte[] b = value.getBytes(charset);
			writeInt(b.length + 1);
			dataBuffer.put(b);
			dataBuffer.put((byte) 0);
		}
	}

}

class CachedDefaultBufferWriter extends DefaultBufferWriter {

	private static final int MAX = 5;

	private static final ConcurrentHashMap<Charset, LinkedBlockingDeque<CachedDefaultBufferWriter>> cache = new ConcurrentHashMap<Charset, LinkedBlockingDeque<CachedDefaultBufferWriter>>();

	private static LinkedBlockingDeque<CachedDefaultBufferWriter> getDeque(Charset charset) {
		LinkedBlockingDeque<CachedDefaultBufferWriter> deque = cache.get(charset);
		if (deque == null) {
			deque = new LinkedBlockingDeque<CachedDefaultBufferWriter>(MAX);
			cache.put(charset, deque);
		}
		return deque;
	}

	public static CachedDefaultBufferWriter getWriter(DataBuffer dataBuffer, Charset charset) {
		LinkedBlockingDeque<CachedDefaultBufferWriter> deque = getDeque(charset);
		CachedDefaultBufferWriter writer = deque.pollFirst();
		if (writer == null) {
			writer = new CachedDefaultBufferWriter(dataBuffer, charset);
		} else {
			writer.dataBuffer = dataBuffer;
			writer.closed = false;
		}
		return writer;
	}

	private static void back(CachedDefaultBufferWriter writer) {
		LinkedBlockingDeque<CachedDefaultBufferWriter> deque = getDeque(writer.getCharset());
		deque.offerFirst(writer);
	}

	private CachedDefaultBufferWriter(DataBuffer dataBuffer, Charset charset) {
		super(dataBuffer, charset);
	}

	@Override
	public void close() {
		super.close();
		back(this);
	}

}

class CachedCppBufferWriter extends CppBufferWriter {
	private static final int MAX = 5;

	private static final ConcurrentHashMap<Charset, LinkedBlockingDeque<CachedCppBufferWriter>> cache = new ConcurrentHashMap<Charset, LinkedBlockingDeque<CachedCppBufferWriter>>();

	private static LinkedBlockingDeque<CachedCppBufferWriter> getDeque(Charset charset) {
		LinkedBlockingDeque<CachedCppBufferWriter> deque = cache.get(charset);
		if (deque == null) {
			deque = new LinkedBlockingDeque<CachedCppBufferWriter>(MAX);
			cache.put(charset, deque);
		}
		return deque;
	}

	public static CachedCppBufferWriter getWriter(DataBuffer dataBuffer, Charset charset) {
		LinkedBlockingDeque<CachedCppBufferWriter> deque = getDeque(charset);
		CachedCppBufferWriter writer = deque.pollFirst();
		if (writer == null) {
			writer = new CachedCppBufferWriter(dataBuffer, charset);
		} else {
			writer.dataBuffer = dataBuffer;
			writer.closed = false;
		}
		return writer;
	}

	private static void back(CachedCppBufferWriter writer) {
		LinkedBlockingDeque<CachedCppBufferWriter> deque = getDeque(writer.getCharset());
		deque.offerFirst(writer);
	}

	private CachedCppBufferWriter(DataBuffer dataBuffer, Charset charset) {
		super(dataBuffer, charset);
	}

	@Override
	public void close() {
		super.close();
		back(this);
	}
}
