package com.tnsoft.icm.msg4j;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;
import com.tnsoft.icm.util.ReaderWriterFactory;
import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public abstract class MsgClient {

	protected final Log log = LogFactory.getFormatterLog(getClass());

	protected String hostname;
	protected int port;
	protected SocketChannel socketChannel;
	protected BufferWriter writer = ReaderWriterFactory.getCBufferWriter();
	protected BufferReader reader = ReaderWriterFactory.getCBufferReader();

	public MsgClient(String hostname, int port) {
		this.hostname = hostname;
		this.port = port;
	}

	protected void channelRead(SocketChannel socketChannel, ByteBuffer buffer) throws IOException {
		int readCount = 0;
		for (int n = 0; readCount < buffer.capacity(); readCount += n) {
			n = socketChannel.read(buffer);
			if (n < 0) {
				// the channel has been closed by remote client, close the channel
				socketChannel.close();
				log.error("The remote host has closed the channel %s !", socketChannel.toString());
				throw new IOException("The remote host has closed the channel " + socketChannel.toString() + " !");
			}
		}
	}

	public String getHostname() {
		return hostname;
	}

	public int getPort() {
		return port;
	}

	public void setWriter(BufferWriter writer) {
		this.writer = writer;
	}

	public void setReader(BufferReader reader) {
		this.reader = reader;
	}

}
