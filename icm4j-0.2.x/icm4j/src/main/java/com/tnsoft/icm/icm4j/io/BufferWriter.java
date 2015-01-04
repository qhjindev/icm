package com.tnsoft.icm.icm4j.io;

import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.GenericArrayType;
import java.lang.reflect.Modifier;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.nio.charset.Charset;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.icm4j.IcmException;

public class BufferWriter extends BufferIO {

	private static final Log log = LogFactory.getLog(BufferWriter.class);

	private static final Map<Class<?>, SimpleWriter<?>> simpleWriterMap = new HashMap<Class<?>, SimpleWriter<?>>();

	protected static SimpleWriter<?> getSimpleWriter(Class<?> type) {
		return simpleWriterMap.get(type);
	}

	static {
		Class<?>[] dcs = BufferWriter.class.getDeclaredClasses();
		for (Class<?> clazz : dcs) {
			if (!Modifier.isAbstract(clazz.getModifiers())) {
				if (SimpleWriter.class.isAssignableFrom(clazz)) {
					try {
						SimpleWriter<?> simpleWriter = (SimpleWriter<?>) clazz.newInstance();
						simpleWriterMap.put(simpleWriter.getType(), simpleWriter);
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

	protected static abstract class SimpleWriter<T> {

		public abstract Class<?> getType();

		public abstract void write(BufferWriter bufferWriter, T value);

	}

	protected static class ByteWriter extends SimpleWriter<Byte> {

		@Override
		public void write(BufferWriter bufferWriter, Byte value) {
			bufferWriter.writeByte(value);
		}

		@Override
		public Class<?> getType() {
			return byte.class;
		}
	}

	protected static class ShortWriter extends SimpleWriter<Short> {

		@Override
		public Class<?> getType() {
			return short.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, Short value) {
			bufferWriter.writeShort(value);
		}

	}

	protected static class IntegerWriter extends SimpleWriter<Integer> {

		@Override
		public Class<?> getType() {
			return int.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, Integer value) {
			bufferWriter.writeInt(value);
		}

	}

	protected static class LongWriter extends SimpleWriter<Long> {

		@Override
		public Class<?> getType() {
			return long.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, Long value) {
			bufferWriter.writeLong(value);
		}

	}

	protected static class BooleanWriter extends SimpleWriter<Boolean> {

		@Override
		public Class<?> getType() {
			return boolean.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, Boolean value) {
			bufferWriter.writeBoolean(value);
		}

	}

	protected static class CharacterWriter extends SimpleWriter<Character> {

		@Override
		public Class<?> getType() {
			return char.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, Character value) {
			bufferWriter.writeChar(value);
		}

	}

	protected static class FloatWriter extends SimpleWriter<Float> {

		@Override
		public Class<?> getType() {
			return float.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, Float value) {
			bufferWriter.writeFloat(value);
		}

	}

	protected static class DoubleWriter extends SimpleWriter<Double> {

		@Override
		public Class<?> getType() {
			return double.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, Double value) {
			bufferWriter.writeDouble(value);
		}

	}

	protected static class StringWriter extends SimpleWriter<String> {

		@Override
		public Class<?> getType() {
			return String.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, String value) {
			bufferWriter.writeString(value);
		}

	}

	protected static class DateWriter extends SimpleWriter<Date> {

		@Override
		public Class<?> getType() {
			return Date.class;
		}

		@Override
		public void write(BufferWriter bufferWriter, Date value) {
			bufferWriter.writeLong(value.getTime());
		}

	}

	protected static class ByteObjectWriter extends ByteWriter {
		@Override
		public Class<?> getType() {
			return Byte.class;
		}
	}

	protected static class ShortObjectWriter extends ShortWriter {
		@Override
		public Class<?> getType() {
			return Short.class;
		}
	}

	protected static class IntegerObjectWriter extends IntegerWriter {
		@Override
		public Class<?> getType() {
			return Integer.class;
		}
	}

	protected static class LongObjectWriter extends LongWriter {
		@Override
		public Class<?> getType() {
			return Long.class;
		}
	}

	protected static class BooleanObjectWriter extends BooleanWriter {
		@Override
		public Class<?> getType() {
			return Boolean.class;
		}
	}

	protected static class CharacterObjectWriter extends CharacterWriter {
		@Override
		public Class<?> getType() {
			return Character.class;
		}
	}

	protected static class FloatObjectWriter extends FloatWriter {
		@Override
		public Class<?> getType() {
			return Float.class;
		}
	}

	protected static class DoubleObjectWriter extends DoubleWriter {
		@Override
		public Class<?> getType() {
			return Double.class;
		}
	}

	public BufferWriter(Buffer buffer, Charset charset) {
		super(buffer, charset);
	}

	public void writeByte(byte value) {
		buffer.put(value);
	}

	public void writeShort(short value) {
		buffer.putShort(value);
	}

	public void writeInt(int value) {
		buffer.putInt(value);
	}

	public void writeLong(long value) {
		buffer.putLong(value);
	}

	public void writeBoolean(boolean value) {
		if (value) {
			buffer.put(1);
		} else {
			buffer.put(0);
		}
	}

	public void writeChar(char value) {
		buffer.putChar(value);
	}

	public void writeFloat(float value) {
		buffer.putFloat(value);
	}

	public void writeDouble(double value) {
		buffer.putDouble(value);
	}

	public void writeString(String value) {
		if (value == null) {
			value = "";
		}
		writeInt(value.length() + 1);
		buffer.put(value.getBytes(charset));
		buffer.put(0);
	}

	public void writeCollection(Collection<?> collection, Type type) {
		if (collection == null) {
			writeInt(0);
			return;
		}
		Type etype = ((ParameterizedType) type).getActualTypeArguments()[0];
		writeInt(collection.size());
		for (Object object : collection) {
			writeObject(object, etype);
		}
	}

	public void writeMap(Map<?, ?> map, Type type) {
		if (map == null) {
			writeInt(0);
			return;
		}
		writeInt(map.size());
		Type[] entryTypes = ((ParameterizedType) type).getActualTypeArguments();
		for (Entry<?, ?> entry : map.entrySet()) {
			writeObject(entry.getKey(), entryTypes[0]);
			writeObject(entry.getValue(), entryTypes[1]);
		}
	}

	public void writeArray(Object array, Type type) {
		if (array == null) {
			writeInt(0);
			return;
		}
		int length = Array.getLength(array);
		writeInt(length);
		if (type instanceof Class) {
			Class<?> ct = ((Class<?>) type).getComponentType();
			if (ct.isPrimitive()) {
				writePrimitiveArray(array, Array.getLength(array), ct);
			} else {
				for (int i = 0; i < length; i++) {
					writeObject(Array.get(array, i), ct);
				}
			}
		} else { // if (type instanceof GenericArrayType)
			Type ct = ((GenericArrayType) type).getGenericComponentType();
			for (int i = 0; i < length; i++) {
				writeObject(Array.get(array, i), ct);
			}
		}
	}

	private void writePrimitiveArray(Object array, int length, Class<?> ct) {
		if (ct == int.class) {
			for (int i = 0; i < length; i++) {
				writeInt(Array.getInt(array, i));
			}
		} else if (ct == long.class) {
			for (int i = 0; i < length; i++) {
				writeLong(Array.getLong(array, i));
			}
		} else if (ct == byte.class) {
			for (int i = 0; i < length; i++) {
				writeByte(Array.getByte(array, i));
			}
		} else if (ct == boolean.class) {
			for (int i = 0; i < length; i++) {
				writeBoolean(Array.getBoolean(array, i));
			}
		} else if (ct == double.class) {
			for (int i = 0; i < length; i++) {
				writeDouble(Array.getDouble(array, i));
			}
		} else if (ct == short.class) {
			for (int i = 0; i < length; i++) {
				writeShort(Array.getShort(array, i));
			}
		} else if (ct == char.class) {
			for (int i = 0; i < length; i++) {
				writeChar(Array.getChar(array, i));
			}
		} else if (ct == float.class) {
			for (int i = 0; i < length; i++) {
				writeFloat(Array.getFloat(array, i));
			}
		}
	}

	public void writeObject(Object value, Type type) {
		if (type instanceof ParameterizedType) {
			Class<?> rawClass = (Class<?>) ((ParameterizedType) type).getRawType();
			if (Collection.class.isAssignableFrom(rawClass)) {
				writeCollection((Collection<?>) value, type);
			} else if (Map.class.isAssignableFrom(rawClass)) {
				writeMap((Map<?, ?>) value, type);
			}
		} else if (type instanceof GenericArrayType) {
			writeArray(value, type);
		} else { // if (type instanceof Class<?>)
			Class<?> clazz = (Class<?>) type;
			if (clazz.isArray()) {
				writeArray(value, type);
			} else {
				writeSimpleOrPojo(value, clazz);
			}
		}
		;
	}

	@SuppressWarnings({ "unchecked", "rawtypes" })
	private void writeSimpleOrPojo(Object value, Class<?> clazz) {
		SimpleWriter simpleWriter = getSimpleWriter(clazz);
		if (simpleWriter != null) {
			simpleWriter.write(this, value);
		} else {
			writePojo(value, clazz);
		}
	}

	private void writePojo(Object value, Class<?> clazz) {
		if (value == null) {
			throw new NullPointerException("the object cannot be null");
		}
		PojoInfo info = getPojoInfo(clazz);
		Field[] fields = info.getProperties();
		for (Field field : fields) {
			try {
				writeObject(field.get(value), field.getGenericType());
			} catch (IllegalAccessException e) {
				log.error(e, e);
				throw new RuntimeException(e);
			}
		}
	}

	public void close() {
		buffer = null;
	}

	@Override
	public boolean isClosed() {
		return buffer == null;
	}

}
