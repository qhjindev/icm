package com.tnsoft.icm.icm4j.io;

import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Buffer {

	public static final Buffer allocate(int initialCapacity) {
		return new Buffer(initialCapacity, ByteOrder.BIG_ENDIAN);
	}

	public static final Buffer allocate(int initialCapacity, ByteOrder order) {
		return new Buffer(initialCapacity, order);
	}

	private byte[] data;
	private int limit;
	private int position;
	private int mark;
	private ByteOrder order;

	private Buffer(int initialCapacity, ByteOrder order) {
		data = new byte[initialCapacity];
		setOrder(order);
		clear();
	}

	private Buffer enablePut(int length) {
		int newDataLength = data.length;
		while (!hasRemaining(length)) {
			newDataLength <<= 2;
			limit = newDataLength;
		}
		byte[] newData = new byte[newDataLength];
		System.arraycopy(data, 0, newData, 0, data.length);
		data = newData;
		return this;
	}

	private int next() {
		return data[position++] & 0xff;
	}

	public Buffer mark() {
		mark = position;
		return this;
	}

	public Buffer reset() {
		if (mark < 0) {
			throw new IllegalArgumentException();
		}
		position = mark;
		return this;
	}

	public final int position() {
		return position;
	}

	public final Buffer position(int newPosition) {
		if (newPosition > limit || newPosition < 0) {
			throw new IllegalArgumentException();
		}
		this.position = newPosition;
		return this;
	}

	public final Buffer rewind() {
		position = 0;
		mark = -1;
		return this;
	}

	public final int limit() {
		return limit;
	}

	public final Buffer limit(int newLimit) {
		if ((newLimit > capacity()) || (newLimit < 0)) {
			throw new IllegalArgumentException();
		}
		limit = newLimit;
		if (position > limit) {
			position = limit;
		}
		if (mark > limit) {
			mark = -1;
		}
		return this;
	}

	public final int capacity() {
		return data.length;
	}

	public final Buffer flip() {
		limit = position;
		position = 0;
		mark = -1;
		return this;
	}

	public final Buffer clear() {
		limit = capacity();
		position = 0;
		mark = -1;
		return this;
	}

	public final int remaining() {
		return limit - position;
	}

	public final boolean hasRemaining(int length) {
		return remaining() >= length;
	}

	public final boolean hasRemaining() {
		return position < limit;
	}

	public final ByteOrder getOrder() {
		return order;
	}

	public final Buffer setOrder(ByteOrder order) {
		this.order = order;
		return this;
	}

	public final Buffer put(byte b) {
		enablePut(1);
		data[position++] = b;
		return this;
	}

	public final Buffer put(int b) {
		return put((byte) b);
	}

	public final Buffer put(byte[] src) {
		return put(src, 0, src.length);
	}

	public final Buffer put(byte[] src, int offset, int length) {
		if (length < 0) {
			throw new IllegalArgumentException();
		}
		if (length > src.length) {
			length = src.length;
		}
		enablePut(length);
		System.arraycopy(src, offset, this.data, position, length);
		position += length;
		return this;
	}

	public final Buffer put(ByteBuffer buffer) {
		return put(buffer, buffer.remaining());
	}

	public final Buffer put(ByteBuffer buffer, int length) {
		if (length < 0) {
			throw new IllegalArgumentException();
		}
		if (length > buffer.remaining()) {
			length = buffer.remaining();
		}
		enablePut(length);
		buffer.get(data, position, length);
		position += length;
		return this;
	}

	public final byte get() {
		if (position >= limit) {
			throw new BufferUnderflowException();
		}
		return data[position++];
	}

	public final Buffer get(byte[] dst) {
		return get(dst, 0, dst.length);
	}

	public final Buffer get(byte[] dst, int offset, int length) {
		if (length < 0 || offset < 0 || offset >= dst.length) {
			throw new IllegalArgumentException();
		}
		int d_len = dst.length - offset;
		if (length > d_len) {
			length = d_len;
		}
		if (length > remaining()) {
			length = remaining();
		}
		System.arraycopy(data, position, dst, offset, length);
		position += length;
		return this;
	}

	public final Buffer get(ByteBuffer buffer) {
		return get(buffer, buffer.remaining());
	}

	public final Buffer get(ByteBuffer buffer, int length) {
		if (length < 0) {
			throw new IllegalArgumentException();
		}
		int d_len = buffer.remaining();
		if (length > d_len) {
			length = d_len;
		}
		if (length > remaining()) {
			length = remaining();
		}
		buffer.put(data, position, length);
		position += length;
		return this;
	}

	public final short getShort() {
		if (remaining() < 2) {
			throw new BufferUnderflowException();
		}
		int b1, b2;
		if (order == ByteOrder.BIG_ENDIAN) {
			b1 = next();
			b2 = next();
		} else {
			b2 = next();
			b1 = next();
		}
		return (short) ((b1 << 8) + b2);
	}

	public final Buffer putShort(short value) {
		enablePut(2);
		if (order == ByteOrder.BIG_ENDIAN) {
			data[position++] = (byte) ((value >>> 8) & 0xff);
			data[position++] = (byte) (value & 0xff);
		} else {
			data[position++] = (byte) (value & 0xff);
			data[position++] = (byte) ((value >>> 8) & 0xff);
		}
		return this;
	}

	public final int getInt() {
		if (remaining() < 4) {
			throw new BufferUnderflowException();
		}
		int b1, b2, b3, b4;
		if (order == ByteOrder.BIG_ENDIAN) {
			b1 = next();
			b2 = next();
			b3 = next();
			b4 = next();
		} else {
			b4 = next();
			b3 = next();
			b2 = next();
			b1 = next();
		}
		return ((b1 << 24) + (b2 << 16) + (b3 << 8) + b4);
	}

	public final Buffer putInt(int value) {
		enablePut(4);
		if (order == ByteOrder.BIG_ENDIAN) {
			data[position++] = (byte) ((value >>> 24) & 0xff);
			data[position++] = (byte) ((value >>> 16) & 0xff);
			data[position++] = (byte) ((value >>> 8) & 0xff);
			data[position++] = (byte) (value & 0xff);
		} else {
			data[position++] = (byte) (value & 0xff);
			data[position++] = (byte) ((value >>> 8) & 0xff);
			data[position++] = (byte) ((value >>> 16) & 0xff);
			data[position++] = (byte) ((value >>> 24) & 0xff);
		}
		return this;
	}

	public final long getLong() {
		long b1, b2, b3, b4;
		int b5, b6, b7, b8;
		if (order == ByteOrder.BIG_ENDIAN) {
			b1 = next();
			b2 = next();
			b3 = next();
			b4 = next();
			b5 = next();
			b6 = next();
			b7 = next();
			b8 = next();
		} else {
			b8 = next();
			b7 = next();
			b6 = next();
			b5 = next();
			b4 = next();
			b3 = next();
			b2 = next();
			b1 = next();
		}
		return (b1 << 56) + (b2 << 48) + (b3 << 40) + (b4 << 32) + (b5 << 24) + (b6 << 16) + (b7 << 8) + b8;
	}

	public final Buffer putLong(long value) {
		enablePut(8);
		if (order == ByteOrder.BIG_ENDIAN) {
			data[position++] = (byte) ((value >>> 56) & 0xff);
			data[position++] = (byte) ((value >>> 48) & 0xff);
			data[position++] = (byte) ((value >>> 40) & 0xff);
			data[position++] = (byte) ((value >>> 32) & 0xff);
			data[position++] = (byte) ((value >>> 24) & 0xff);
			data[position++] = (byte) ((value >>> 16) & 0xff);
			data[position++] = (byte) ((value >>> 8) & 0xff);
			data[position++] = (byte) (value & 0xff);
		} else {
			data[position++] = (byte) (value & 0xff);
			data[position++] = (byte) ((value >>> 8) & 0xff);
			data[position++] = (byte) ((value >>> 16) & 0xff);
			data[position++] = (byte) ((value >>> 24) & 0xff);
			data[position++] = (byte) ((value >>> 32) & 0xff);
			data[position++] = (byte) ((value >>> 40) & 0xff);
			data[position++] = (byte) ((value >>> 48) & 0xff);
			data[position++] = (byte) ((value >>> 56) & 0xff);
		}
		return this;
	}

	public final float getFloat() {
		return Float.intBitsToFloat(getInt());
	}

	public final Buffer putFloat(float value) {
		return putInt(Float.floatToIntBits(value));
	}

	public final double getDouble() {
		return Double.longBitsToDouble(getLong());
	}

	public final Buffer putDouble(double value) {
		return putLong(Double.doubleToLongBits(value));
	}

	public final char getChar() {
		return (char) getShort();
	}

	public final Buffer putChar(char value) {
		return putShort((short) value);
	}

}
