package com.tnsoft.icm.icm4j.protocol;

import java.nio.ByteBuffer;

import com.tnsoft.icm.util.BufferFactory;
import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;

public abstract class Message {

	protected int bufferSize = 1024;
	protected ByteBuffer buffer;

	protected MessageHead messageHead = new MessageHead();

	public MessageHead getMessageHead() {
		return messageHead;
	}

	public void setMessageHead(MessageHead messageHead) {
		this.messageHead = messageHead;
	}

	protected void readMessageHead(BufferReader reader) {
		messageHead.setSize(reader.readInt(buffer));
		messageHead.setType(reader.readByte(buffer));
	}

	public void writeBuffer(BufferWriter writer) {
		writeBuffer(writer, this.bufferSize);
	}

	public abstract void writeBuffer(BufferWriter writer, int bufferSize);

	/**
	 * Allocates a new byte buffer with the size given.
	 * 
	 * @param bufferSize
	 *            the size of the buffer
	 */
	protected void allocateBuffer(int bufferSize) {
		buffer = BufferFactory.getInstance().getBuffer(bufferSize);
	}

	/**
	 * Finish writing the buffer and flip it.
	 * 
	 * @param writer
	 *            the using {@link BufferWriter}
	 */
	protected void finishWrite(BufferWriter writer) {
		// return to set the message size in message head
		int p = buffer.position();
		buffer.position(0);
		writer.writeInt(buffer, p - MessageHead.MESSAGE_HEAD_SIZE);
		buffer.position(p);
		// write end
		buffer.flip();
		// set message size
		messageHead.setSize(buffer.limit());
	}

	public void writeBuffer(byte[] b) {
		allocateBuffer(b.length);
		buffer.put(b);
		buffer.flip();
	}

	public void setBuffer(ByteBuffer buffer) {
		this.buffer = buffer;
	}

	public byte[] readBuffer() {
		byte[] b = new byte[buffer.limit()];
		buffer.get(b);
		return b;
	}

	public ByteBuffer getBuffer() {
		return buffer;
	}

}
