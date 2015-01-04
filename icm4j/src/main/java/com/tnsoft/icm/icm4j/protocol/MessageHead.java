package com.tnsoft.icm.icm4j.protocol;

public class MessageHead {

	public static final int MESSAGE_HEAD_SIZE = 5;

	public static final byte REQUEST = 0;
	public static final byte RESPONSE = 1;

	private int size;
	private byte type;

	public int getSize() {
		return size;
	}

	public void setSize(int size) {
		this.size = size;
	}

	public byte getType() {
		return type;
	}

	public void setType(byte type) {
		this.type = type;
	}

}
