package com.tnsoft.icm.util;

import java.lang.reflect.Array;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import com.tnsoft.icm.util.StructBeanInfo.Property;
import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public class ReaderWriterFactory {

	private static final Log log = LogFactory.getFormatterLog(ReaderWriterFactory.class);

	// readers
	private static final BufferReader DEFAULT_READER = new BufferReaderImpl();
	private static final BufferReader C_CPP_READER = new CBufferReader();

	// writers
	private static final BufferWriter DEFAULT_WRITER = new BufferWriterImpl();
	private static final BufferWriter C_CPP_WRITER = new CBufferWriter();

	// for user struct
	private static final Map<Class<?>, StructBeanInfo> structMap = new ConcurrentHashMap<Class<?>, StructBeanInfo>();
	private static final Object map_lock = new Object();

	private static StructBeanInfo getStructBeanInfo(Class<?> type) {
		StructBeanInfo sbi = structMap.get(type);
		if (sbi == null) {
			synchronized (map_lock) {
				sbi = structMap.get(type);
				if (sbi == null) {
					sbi = new StructBeanInfo(type);
					structMap.put(type, sbi);
				}
			}
		}
		return sbi;
	}

	public static BufferReader getDefaultBufferReader() {
		return DEFAULT_READER;
	}

	public static BufferReader getCBufferReader() {
		return C_CPP_READER;
	}

	public static BufferWriter getDefaultBufferWriter() {
		return DEFAULT_WRITER;
	}

	public static BufferWriter getCBufferWriter() {
		return C_CPP_WRITER;
	}

	private static final Charset DEFAULT_CHARSET = Charset.forName("UTF-8");

	private static class BufferReaderImpl implements BufferReader {

		protected int readArrayLength(ByteBuffer buffer) {
			return (int) readLong(buffer);
		}

		public Set<?> readSet(ByteBuffer buffer, ParameterizedType type) {
			Class<?> elementType = (Class<?>) type.getActualTypeArguments()[0];
			Set<Object> set = new LinkedHashSet<Object>();
			int size = readShort(buffer);
			for (int i = 0; i < size; i++) {
				set.add(readObject(buffer, elementType));
			}
			return set;
		}

		public List<?> readList(ByteBuffer buffer, ParameterizedType type) {
			Class<?> elementType = (Class<?>) type.getActualTypeArguments()[0];
			List<Object> list = new ArrayList<Object>();
			int size = readShort(buffer);
			for (int i = 0; i < size; i++) {
				list.add(readObject(buffer, elementType));
			}
			return list;
		}

		@SuppressWarnings({ "rawtypes", "unchecked" })
		public Map readMap(ByteBuffer buffer, ParameterizedType type) {
			Type[] types = type.getActualTypeArguments();
			Class<?> keyType = (Class<?>) types[0];
			Class<?> valueType = (Class<?>) types[1];
			int size = readShort(buffer);
			Map map = new LinkedHashMap(size);
			for (int i = 0; i < size; i++) {
				Object key = readObject(buffer, keyType);
				Object value = readObject(buffer, valueType);
				map.put(key, value);
			}
			return map;
		}

		public Object readObject(ByteBuffer buffer, Class<?> type) {
			if (type.isArray()) {
				return readArray(buffer, type.getComponentType());
			} else if (byte.class == type || Byte.class == type) {
				return readByte(buffer);
			} else if (short.class == type || Short.class == type) {
				return readShort(buffer);
			} else if (int.class == type || Integer.class == type) {
				return readInt(buffer);
			} else if (long.class == type || Long.class == type) {
				return readLong(buffer);
			} else if (boolean.class == type || Boolean.class == type) {
				return readBoolean(buffer);
			} else if (float.class == type || Float.class == type) {
				return readFloat(buffer);
			} else if (double.class == type || Double.class == type) {
				return readDouble(buffer);
			} else if (char.class == type || Character.class == type) {
				return readChar(buffer);
			} else if (String.class == type) {
				return readString(buffer);
			} else if (type.isEnum()) {
				String stringvalue = readString(buffer);
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
						property.setValue(object, readSet(buffer, (ParameterizedType) property.getGenericType()));
					} else if (pt.isAssignableFrom(List.class)) {
						property.setValue(object, readList(buffer, (ParameterizedType) property.getGenericType()));
					} else if (pt.isAssignableFrom(Map.class)) {
						property.setValue(object, readMap(buffer, (ParameterizedType) property.getGenericType()));
					} else {
						property.setValue(object, readObject(buffer, pt));
					}
				}
				return object;
			}
		}

		public Object readArray(ByteBuffer buffer, Class<?> componentType) {
			int length = readArrayLength(buffer);
			Object result = Array.newInstance(componentType, length);
			for (int i = 0; i < length; i++) {
				Array.set(result, i, readObject(buffer, componentType));
			}
			return result;
		}

		public byte readByte(ByteBuffer buffer) {
			return buffer.get();
		}

		public short readShort(ByteBuffer buffer) {
			return buffer.getShort();
		}

		public int readInt(ByteBuffer buffer) {
			return buffer.getInt();
		}

		public long readLong(ByteBuffer buffer) {
			return buffer.getLong();
		}

		public char readChar(ByteBuffer buffer) {
			return buffer.getChar();
		}

		public boolean readBoolean(ByteBuffer buffer) {
			byte b = readByte(buffer);
			return b == 0 ? false : true;
		}

		public float readFloat(ByteBuffer buffer) {
			return buffer.getFloat();
		}

		public double readDouble(ByteBuffer buffer) {
			return buffer.getDouble();
		}

		public String readString(ByteBuffer buffer) {
			int length = readInt(buffer);
			if (length == -1) {
				return null;
			} else {
				byte[] b = new byte[length];
				buffer.get(b);
				return new String(b, DEFAULT_CHARSET);
			}
		}

	}

	private static class CBufferReader extends BufferReaderImpl {
		@Override
		public String readString(ByteBuffer buffer) {
			int length = readInt(buffer);
			byte[] b = new byte[length - 1];
			buffer.get(b);
			buffer.get();
			return new String(b, DEFAULT_CHARSET);
		}
	}

	private static class BufferWriterImpl implements BufferWriter {

		public void writeSet(ByteBuffer buffer, Set<?> set, ParameterizedType type) {
			if (set == null) {
				writeShort(buffer, (short) 0);
			} else {
				int length = set.size();
				Class<?> elementType = (Class<?>) type.getActualTypeArguments()[0];
				writeShort(buffer, (short) length);
				for (Object object : set) {
					writeObject(buffer, object, elementType);
				}
			}
		}

		public void writeList(ByteBuffer buffer, List<?> list, ParameterizedType type) {
			if (list == null) {
				writeShort(buffer, (short) 0);
			} else {
				Class<?> elementType = (Class<?>) type.getActualTypeArguments()[0];
				int length = list.size();
				writeShort(buffer, (short) length);
				for (Object object : list) {
					writeObject(buffer, object, elementType);
				}
			}
		}

		@SuppressWarnings("rawtypes")
		public void writeMap(ByteBuffer buffer, Map map, ParameterizedType type) {
			if (map == null) {
				writeShort(buffer, (short) 0);
			} else {
				Type[] types = type.getActualTypeArguments();
				Class<?> keyType = (Class<?>) types[0];
				Class<?> valueType = (Class<?>) types[1];
				int length = map.size();
				// write size;
				writeShort(buffer, (short) length);
				for (Iterator<?> iterator = map.entrySet().iterator(); iterator.hasNext();) {
					// write entry
					Entry entry = (Entry) iterator.next();
					writeObject(buffer, entry.getKey(), keyType);
					writeObject(buffer, entry.getValue(), valueType);
				}
			}
		}

		public void writeObject(ByteBuffer buffer, Object value, Class<?> type) {
			if (type.isArray()) {
				writeArray(buffer, value, type.getComponentType());
			} else if (byte.class == type || Byte.class == type) {
				writeByte(buffer, (Byte) value);
			} else if (short.class == type || Short.class == type) {
				writeShort(buffer, (Short) value);
			} else if (int.class == type || Integer.class == type) {
				writeInt(buffer, (Integer) value);
			} else if (long.class == type || Long.class == type) {
				writeLong(buffer, (Long) value);
			} else if (boolean.class == type || Boolean.class == type) {
				writeBoolean(buffer, (Boolean) value);
			} else if (float.class == type || Float.class == type) {
				writeFloat(buffer, (Float) value);
			} else if (double.class == type || Double.class == type) {
				writeDouble(buffer, (Double) value);
			} else if (char.class == type || Character.class == type) {
				writeChar(buffer, (Character) value);
			} else if (String.class == type) {
				writeString(buffer, (String) value);
			} else if (type.isEnum()) {
				writeString(buffer, value.toString());
			} else {
				if (value == null) {
					try {
						value = type.newInstance();
					} catch (InstantiationException e) {
						log.error(e);
					} catch (IllegalAccessException e) {
						log.error(e);
					}
				}
				StructBeanInfo sbi = getStructBeanInfo(type);
				Property[] properties = sbi.getProperties();
				for (Property property : properties) {
					Class<?> pt = property.getType();
					if (pt.isAssignableFrom(Set.class)) {
						writeSet(buffer, (Set<?>) property.getValue(value),
								(ParameterizedType) property.getGenericType());
					} else if (pt.isAssignableFrom(List.class)) {
						writeList(buffer, (List<?>) property.getValue(value),
								(ParameterizedType) property.getGenericType());
					} else if (pt.isAssignableFrom(Map.class)) {
						writeMap(buffer, (Map<?, ?>) property.getValue(value),
								(ParameterizedType) property.getGenericType());
					} else {
						writeObject(buffer, property.getValue(value), pt);
					}
				}
			}
		}

		protected void writeArrayLength(ByteBuffer buffer, int length) {
			writeLong(buffer, length);
		}

		public void writeArray(ByteBuffer buffer, Object array, Class<?> componentType) {
			if (array == null) {
				writeLong(buffer, 0);
			} else if (componentType.isPrimitive()) {
				if (byte.class == componentType) {
					byte[] values = (byte[]) array;
					writeArrayLength(buffer, values.length);
					for (byte value : values) {
						writeByte(buffer, value);
					}
				} else if (short.class == componentType) {
					short[] values = (short[]) array;
					writeArrayLength(buffer, values.length);
					for (short value : values) {
						writeShort(buffer, value);
					}
				} else if (int.class == componentType) {
					int[] values = (int[]) array;
					writeArrayLength(buffer, values.length);
					for (int value : values) {
						writeInt(buffer, value);
					}
				} else if (long.class == componentType) {
					long[] values = (long[]) array;
					writeArrayLength(buffer, values.length);
					for (long value : values) {
						writeLong(buffer, value);
					}
				} else if (char.class == componentType) {
					char[] values = (char[]) array;
					writeArrayLength(buffer, values.length);
					for (char value : values) {
						writeChar(buffer, value);
					}
				} else if (boolean.class == componentType) {
					boolean[] values = (boolean[]) array;
					writeArrayLength(buffer, values.length);
					for (boolean value : values) {
						writeBoolean(buffer, value);
					}
				} else if (float.class == componentType) {
					float[] values = (float[]) array;
					writeArrayLength(buffer, values.length);
					for (float value : values) {
						writeFloat(buffer, value);
					}
				} else if (double.class == componentType) {
					double[] values = (double[]) array;
					writeArrayLength(buffer, values.length);
					for (double value : values) {
						writeDouble(buffer, value);
					}
				}
			} else {
				Object[] values = (Object[]) array;
				writeArrayLength(buffer, values.length);
				for (Object value : values) {
					writeObject(buffer, value, componentType);
				}
			}
		}

		public void writeByte(ByteBuffer buffer, byte value) {
			buffer.put(value);
		}

		public void writeShort(ByteBuffer buffer, short value) {
			buffer.putShort(value);
		}

		public void writeInt(ByteBuffer buffer, int value) {
			buffer.putInt(value);
		}

		public void writeLong(ByteBuffer buffer, long value) {
			buffer.putLong(value);
		}

		public void writeChar(ByteBuffer buffer, char value) {
			buffer.putChar(value);
		}

		public void writeBoolean(ByteBuffer buffer, boolean value) {
			buffer.put((byte) (value ? 1 : 0));
		}

		public void writeFloat(ByteBuffer buffer, float value) {
			buffer.putFloat(value);
		}

		public void writeDouble(ByteBuffer buffer, double value) {
			buffer.putDouble(value);
		}

		public void writeString(ByteBuffer buffer, String value) {
			if (value != null) {
				writeInt(buffer, value.length());
				buffer.put(value.getBytes(DEFAULT_CHARSET));
			} else {
				writeInt(buffer, -1);
			}
		}

	}

	private static class CBufferWriter extends BufferWriterImpl {
		@Override
		public void writeString(ByteBuffer buffer, String value) {
			if (value != null) {
				writeInt(buffer, value.length() + 1);
				buffer.put(value.getBytes(DEFAULT_CHARSET));
			} else {
				buffer.putInt(1);
			}
			buffer.put((byte) 0);
		}
	}
}
