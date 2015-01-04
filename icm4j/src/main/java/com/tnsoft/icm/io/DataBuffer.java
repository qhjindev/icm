package com.tnsoft.icm.io;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class DataBuffer {

	private static final int DEFAULT_SIZE = 256;
	private static final int LOAD_FACTOR = 2;

	private byte[] data;
	private int position;
	private int limit;
	private ByteOrder byteOrder;
	private boolean fliped;

	public DataBuffer(ByteOrder byteOrder) {
		this(DEFAULT_SIZE, byteOrder);
	}

	public DataBuffer(int init, ByteOrder byteOrder) {
		this.data = new byte[init];
		this.byteOrder = byteOrder;
		clear();
	}

	public ByteOrder getByteOrder() {
		return byteOrder;
	}

	public byte[] getData() {
		return data.clone();
	}

	public int position() {
		return this.position;
	}

	public void position(int position) {
		if (position < 0) {
			this.position = 0;
		} else if (position > capacity()) {
			this.position = capacity();
		} else {
			this.position = position;
		}
	}

	public boolean isFliped() {
		return fliped;
	}

	public int capacity() {
		return data.length;
	}

	public int limit() {
		return limit;
	}

	public int remaining() {
		return limit - position;
	}

	public boolean hasNext() {
		return remaining() > 0;
	}

	public DataBuffer flip() {
		limit = position;
		position = 0;
		fliped = true;
		return this;
	}

	public DataBuffer clear() {
		limit = capacity();
		position = 0;
		fliped = false;
		return this;
	}

	private void enableRemaining(int remaining) {
		while (remaining() < remaining) {
			byte[] n_data = new byte[data.length * LOAD_FACTOR];
			System.arraycopy(data, 0, n_data, 0, data.length);
			this.data = n_data;
			limit = capacity();
		}
	}

	private byte[] shortBuffer = new byte[2];

	private byte[] getData(short value) {
		if (ByteOrder.BIG_ENDIAN.equals(byteOrder)) {
			shortBuffer[0] = Bits.get(value, 1);
			shortBuffer[1] = Bits.get(value, 0);
		} else {
			shortBuffer[0] = Bits.get(value, 0);
			shortBuffer[1] = Bits.get(value, 1);
		}
		return shortBuffer;
	}

	private short getShortData() {
		boolean reverse = ByteOrder.BIG_ENDIAN.equals(byteOrder);
		return (short) Bits.get(shortBuffer, reverse);
	}

	private byte[] intBuffer = new byte[4];

	private byte[] getData(int value) {
		if (ByteOrder.BIG_ENDIAN.equals(byteOrder)) {
			intBuffer[0] = Bits.get(value, 3);
			intBuffer[1] = Bits.get(value, 2);
			intBuffer[2] = Bits.get(value, 1);
			intBuffer[3] = Bits.get(value, 0);
		} else {
			intBuffer[0] = Bits.get(value, 0);
			intBuffer[1] = Bits.get(value, 1);
			intBuffer[2] = Bits.get(value, 2);
			intBuffer[3] = Bits.get(value, 3);
		}
		return intBuffer;
	}

	private int getIntData() {
		boolean reverse = ByteOrder.BIG_ENDIAN.equals(byteOrder);
		return (int) Bits.get(intBuffer, reverse);
	}

	private byte[] longBuffer = new byte[8];

	private byte[] getData(long value) {
		if (ByteOrder.BIG_ENDIAN.equals(byteOrder)) {
			longBuffer[0] = Bits.get(value, 7);
			longBuffer[1] = Bits.get(value, 6);
			longBuffer[2] = Bits.get(value, 5);
			longBuffer[3] = Bits.get(value, 4);
			longBuffer[4] = Bits.get(value, 3);
			longBuffer[5] = Bits.get(value, 2);
			longBuffer[6] = Bits.get(value, 1);
			longBuffer[7] = Bits.get(value, 0);
		} else {
			longBuffer[0] = Bits.get(value, 0);
			longBuffer[1] = Bits.get(value, 1);
			longBuffer[2] = Bits.get(value, 2);
			longBuffer[3] = Bits.get(value, 3);
			longBuffer[4] = Bits.get(value, 4);
			longBuffer[5] = Bits.get(value, 5);
			longBuffer[6] = Bits.get(value, 6);
			longBuffer[7] = Bits.get(value, 7);
		}
		return longBuffer;
	}

	private long getLongData() {
		boolean reverse = ByteOrder.BIG_ENDIAN.equals(byteOrder);
		return Bits.get(longBuffer, reverse);
	}

	private byte[] getData(float value) {
		return getData(Float.floatToIntBits(value));
	}

	private byte[] getData(double value) {
		return getData(Double.doubleToLongBits(value));
	}

	private void putDate(byte b) {
		this.data[position++] = b;
	}

	public DataBuffer put(byte b) {
		if (fliped) {
			throw new WriteFlipedBufferException();
		}
		enableRemaining(1);
		putDate(b);
		return this;
	}

	public byte get() {
		if (!fliped) {
			throw new ReadUnflipedBufferException();
		}
		if (remaining() > 0) {
			return data[position++];
		} else {
			throw new EndOfDataExceiption();
		}
	}

	public DataBuffer put(byte[] src) {
		return put(src, 0, src.length);
	}

	public DataBuffer put(byte[] src, int offset, int length) {
		if (fliped) {
			throw new WriteFlipedBufferException();
		}
		enableRemaining(length);
		System.arraycopy(src, offset, data, position, length);
		position += length;
		return this;
	}

	public DataBuffer put(ByteBuffer buf) {
		return put(buf, buf.remaining());
	}

	public DataBuffer put(ByteBuffer buf, int length) {
		if (fliped) {
			throw new WriteFlipedBufferException();
		}
		enableRemaining(length);
		buf.get(data, position, length);
		position += length;
		return this;
	}

	public DataBuffer get(byte[] dest) {
		return get(dest, 0, dest.length);
	}

	public DataBuffer get(byte[] dest, int offset, int length) {
		if (!fliped) {
			throw new ReadUnflipedBufferException();
		}
		if (remaining() == 0) {
			throw new EndOfDataExceiption();
		}
		if (length > remaining()) {
			length = remaining();
		}
		System.arraycopy(data, position, dest, offset, length);
		position += length;
		return this;
	}

	public DataBuffer get(ByteBuffer buf) {
		return get(buf, buf.position(), buf.remaining());
	}

	public DataBuffer get(ByteBuffer buf, int offset, int length) {
		if (!fliped) {
			throw new ReadUnflipedBufferException();
		}
		if (remaining() == 0) {
			throw new EndOfDataExceiption();
		}
		if (length > remaining()) {
			length = remaining();
		}
		buf.put(data, position, length);
		position += length;
		return this;
	}

	public DataBuffer putChar(char c) {
		return putShort((short) c);
	}

	public char getChar() {
		return (char) getShort();
	}

	public DataBuffer putShort(short value) {
		if (fliped) {
			throw new WriteFlipedBufferException();
		}
		enableRemaining(2);
		put(getData(value));
		return this;
	}

	public short getShort() {
		if (!fliped) {
			throw new ReadUnflipedBufferException();
		}
		if (remaining() < 2) {
			throw new EndOfDataExceiption();
		}
		get(shortBuffer);
		return getShortData();
	}

	public DataBuffer putInt(int value) {
		if (fliped) {
			throw new WriteFlipedBufferException();
		}
		enableRemaining(4);
		put(getData(value));
		return this;
	}

	public int getInt() {
		if (!fliped) {
			throw new ReadUnflipedBufferException();
		}
		if (remaining() < 4) {
			throw new EndOfDataExceiption();
		}
		get(intBuffer);
		return getIntData();
	}

	public DataBuffer putLong(long value) {
		if (fliped) {
			throw new WriteFlipedBufferException();
		}
		enableRemaining(8);
		put(getData(value));
		return this;
	}

	public long getLong() {
		if (!fliped) {
			throw new ReadUnflipedBufferException();
		}
		if (remaining() < 8) {
			throw new EndOfDataExceiption();
		}
		get(longBuffer);
		return getLongData();
	}

	public DataBuffer putFloat(float value) {
		if (fliped) {
			throw new WriteFlipedBufferException();
		}
		enableRemaining(4);
		put(getData(value));
		return this;
	}

	public float getFloat() {
		if (!fliped) {
			throw new ReadUnflipedBufferException();
		}
		if (remaining() < 4) {
			throw new EndOfDataExceiption();
		}
		get(intBuffer);
		return Float.intBitsToFloat(getIntData());
	}

	public DataBuffer putDouble(double value) {
		if (fliped) {
			throw new WriteFlipedBufferException();
		}
		enableRemaining(8);
		put(getData(value));
		return this;
	}

	public double getDouble() {
		if (!fliped) {
			throw new ReadUnflipedBufferException();
		}
		if (remaining() < 8) {
			throw new EndOfDataExceiption();
		}
		get(longBuffer);
		return Double.longBitsToDouble(getLongData());
	}

	private static class Bits {
		static byte get(long v, int i) {
			return (byte) ((v >>> (i * 8)) & 0xFF);
		}

		static long get(byte[] bytes, boolean reverse) {
			long value = 0;
			for (int i = 0; i < bytes.length; i++) {
				value += ((bytes[i] & 0xffL) << ((reverse ? bytes.length - 1 - i : i) * 8));
			}
			return value;
		}
	}

}
