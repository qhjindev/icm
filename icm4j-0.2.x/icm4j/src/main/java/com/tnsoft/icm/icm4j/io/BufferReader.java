package com.tnsoft.icm.icm4j.io;

import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.GenericArrayType;
import java.lang.reflect.Modifier;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.icm4j.IcmException;

public class BufferReader extends BufferIO {

	private static final Log log = LogFactory.getLog(BufferReader.class);

	private static final Map<Class<?>, SimpleReader<?>> simpleReaderMap = new HashMap<Class<?>, SimpleReader<?>>();

	protected static SimpleReader<?> getSimpleReader(Class<?> type) {
		return simpleReaderMap.get(type);
	}
	
	static {
		Class<?>[] rcs = BufferReader.class.getDeclaredClasses();
		for (Class<?> clazz : rcs) {
			if (!Modifier.isAbstract(clazz.getModifiers())) {
				if (SimpleReader.class.isAssignableFrom(clazz)) {
					try {
						@SuppressWarnings("rawtypes")
						SimpleReader reader = (SimpleReader) clazz.newInstance();
						simpleReaderMap.put(reader.getType(), reader);
					} catch (InstantiationException e) {
						log.error(e, e);
						throw new IcmException(e);
					} catch (IllegalAccessException e) {
						log.error(e, e);
						throw new IcmException(e);
					}
				}
			}
		}
	}

	protected static abstract class SimpleReader<T> {

		public abstract Class<?> getType();

		public abstract T read(BufferReader bufferReader);

	}

	protected static class ByteReader extends SimpleReader<Byte> {

		@Override
		public Class<?> getType() {
			return byte.class;
		}

		@Override
		public Byte read(BufferReader bufferReader) {
			return bufferReader.readByte();
		}
	}

	protected static class ShortReader extends SimpleReader<Short> {

		@Override
		public Class<?> getType() {
			return short.class;
		}

		@Override
		public Short read(BufferReader bufferReader) {
			return bufferReader.readShort();
		}

	}

	protected static class IntegerReader extends SimpleReader<Integer> {

		@Override
		public Class<?> getType() {
			return int.class;
		}

		@Override
		public Integer read(BufferReader bufferReader) {
			return bufferReader.readInt();
		}

	}

	protected static class LongReader extends SimpleReader<Long> {

		@Override
		public Class<?> getType() {
			return long.class;
		}

		@Override
		public Long read(BufferReader bufferReader) {
			return bufferReader.readLong();
		}

	}

	protected static class BooleanReader extends SimpleReader<Boolean> {

		@Override
		public Class<?> getType() {
			return boolean.class;
		}

		@Override
		public Boolean read(BufferReader bufferReader) {
			return bufferReader.readBoolean();
		}

	}

	protected static class CharacterReader extends SimpleReader<Character> {

		@Override
		public Class<?> getType() {
			return char.class;
		}

		@Override
		public Character read(BufferReader bufferReader) {
			return bufferReader.readChar();
		}

	}

	protected static class FloatReader extends SimpleReader<Float> {

		@Override
		public Class<?> getType() {
			return float.class;
		}

		@Override
		public Float read(BufferReader bufferReader) {
			return bufferReader.readFloat();
		}

	}

	protected static class DoubleReader extends SimpleReader<Double> {

		@Override
		public Class<?> getType() {
			return double.class;
		}

		@Override
		public Double read(BufferReader bufferReader) {
			return bufferReader.readDouble();
		}

	}

	protected static class StringReader extends SimpleReader<String> {

		@Override
		public Class<?> getType() {
			return String.class;
		}

		@Override
		public String read(BufferReader bufferReader) {
			return bufferReader.readString();
		}

	}

	protected static class DateReader extends SimpleReader<Date> {

		@Override
		public Class<?> getType() {
			return Date.class;
		}

		@Override
		public Date read(BufferReader bufferReader) {
			return new Date(bufferReader.readLong());
		}

	}

	protected static class ByteObjectReader extends ByteReader {
		@Override
		public Class<?> getType() {
			return Byte.class;
		}
	}

	protected static class ShortObjectReader extends ShortReader {
		@Override
		public Class<?> getType() {
			return Short.class;
		}
	}

	protected static class IntegerObjectReader extends IntegerReader {
		@Override
		public Class<?> getType() {
			return Integer.class;
		}
	}

	protected static class LongObjectReader extends LongReader {
		@Override
		public Class<?> getType() {
			return Long.class;
		}
	}

	protected static class BooleanObjectReader extends BooleanReader {
		@Override
		public Class<?> getType() {
			return Boolean.class;
		}
	}

	protected static class CharacterObjectReader extends CharacterReader {
		@Override
		public Class<?> getType() {
			return Character.class;
		}
	}

	protected static class FloatObjectReader extends FloatReader {
		@Override
		public Class<?> getType() {
			return Float.class;
		}
	}

	protected static class DoubleObjectReader extends DoubleReader {
		@Override
		public Class<?> getType() {
			return Double.class;
		}
	}

	public BufferReader(Buffer buffer, Charset charset) {
		super(buffer, charset);
	}

	public byte readByte() {
		return buffer.get();
	}

	public short readShort() {
		return buffer.getShort();
	}

	public int readInt() {
		return buffer.getInt();
	}

	public long readLong() {
		return buffer.getLong();
	}

	public boolean readBoolean() {
		byte flag = buffer.get();
		return flag == 1;
	}

	public char readChar() {
		return buffer.getChar();
	}

	public float readFloat() {
		return buffer.getFloat();
	}

	public double readDouble() {
		return buffer.getDouble();
	}

	public String readString() {
		int length = readInt();
		if (length == 1) {
			readByte();
			return "";
		}
		byte[] data = new byte[length - 1];
		buffer.get(data);
		buffer.get();
		return new String(data, charset);
	}

	public List<?> readList(Type type) {
		int length = readInt();
		Type ct = ((ParameterizedType) type).getActualTypeArguments()[0];
		List<Object> list = new ArrayList<Object>(length);
		for (int i = 0; i < length; i++) {
			list.add(readObject(ct));
		}
		return list;
	}

	public Set<?> readSet(Type type) {
		int length = readInt();
		Type ct = ((ParameterizedType) type).getActualTypeArguments()[0];
		Set<Object> set = new LinkedHashSet<Object>(length);
		for (int i = 0; i < length; i++) {
			set.add(readObject(ct));
		}
		return set;
	}

	public Map<?, ?> readMap(Type type) {
		int length = readInt();
		Type key_type = ((ParameterizedType) type).getActualTypeArguments()[0];
		Type value_type = ((ParameterizedType) type).getActualTypeArguments()[1];
		Map<Object, Object> map = new LinkedHashMap<Object, Object>(length);
		for (int i = 0; i < length; i++) {
			Object key = readObject(key_type);
			Object value = readObject(value_type);
			map.put(key, value);
		}
		return map;
	}

	@SuppressWarnings("rawtypes")
	public Object readArray(Type type) {
		int length = readInt();
		if (type instanceof Class) {
			Class<?> ct = ((Class<?>) type).getComponentType();
			Object array = Array.newInstance(ct, length);
			for (int i = 0; i < length; i++) {
				Array.set(array, i, readObject(ct));
			}
			return array;
		} else {
			ParameterizedType ct = (ParameterizedType) ((GenericArrayType) type).getGenericComponentType();
			Type rawType = ct.getRawType();
			if (rawType == List.class) {
				List[] lists = new List[length];
				for (int i = 0; i < length; i++) {
					lists[i] = readList(ct);
				}
				return lists;
			} else if (rawType == Set.class) {
				Set[] sets = new Set[length];
				for (int i = 0; i < length; i++) {
					sets[i] = readSet(ct);
				}
				return sets;
			} else if (rawType == Map.class) {
				Map[] maps = new Map[length];
				for (int i = 0; i < length; i++) {
					maps[i] = readMap(ct);
				}
				return maps;
			}
			return null;
		}
	}

	public Object readObject(Type type) {
		if (type instanceof ParameterizedType) {
			Type rawType = ((ParameterizedType) type).getRawType();
			if (rawType == List.class) {
				return readList(type);
			} else if (rawType == Map.class) {
				return readMap(type);
			} else if (rawType == Set.class) {
				return readSet(type);
			}
			throw new RuntimeException("Unsupported type: " + type);
		} else if (type instanceof GenericArrayType) {
			return readArray(type);
		} else { // if (type instanceof Class<?>)
			Class<?> clazz = (Class<?>) type;
			if (clazz.isArray()) {
				return readArray(type);
			} else {
				return readSimpleOrPojo(clazz);
			}
		}
	}

	private Object readSimpleOrPojo(Class<?> clazz) {
		@SuppressWarnings("rawtypes")
		SimpleReader reader = getSimpleReader(clazz);
		if (reader != null) {
			return reader.read(this);
		} else {
			return readPojo(clazz);
		}
	}

	private Object readPojo(Class<?> clazz) {
		PojoInfo info = getPojoInfo(clazz);
		try {
			Constructor<?> constructor = clazz.getDeclaredConstructor();
			constructor.setAccessible(true);
			Object obj = constructor.newInstance();
			Field[] fields = info.getProperties();
			for (Field field : fields) {
				field.set(obj, readObject(field.getGenericType()));
			}
			return obj;
		} catch (Exception e) {
			log.error(e, e);
			throw new RuntimeException(e);
		}
	}

	@Override
	public void close() {
		buffer = null;
	}

	@Override
	public boolean isClosed() {
		return buffer == null;
	}

}
