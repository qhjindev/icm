package com.tnsoft.icm.icm4j;

import java.nio.charset.Charset;

import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;
import com.tnsoft.icm.util.ReaderWriterFactory;


@Deprecated
public abstract class TCPICMServer {

	protected BufferReader reader = ReaderWriterFactory.getCBufferReader();
	protected BufferWriter writer = ReaderWriterFactory.getCBufferWriter();
	protected Charset charset = Charset.forName("UTF-8");
	protected int port;
	protected ServiceDispatcher serviceDispatcher;

	public TCPICMServer(int port) {
		this.port = port;
	}

	public TCPICMServer(int port, Charset charset) {
		this(port);
		setCharset(charset);
	}

	public void setCharset(Charset charset) {
		this.charset = charset;
	}

	public void setReader(BufferReader reader) {
		this.reader = reader;
	}

	public void setWriter(BufferWriter writer) {
		this.writer = writer;
	}

	public ServiceDispatcher getServiceDispatcher() {
		return serviceDispatcher;
	}

	public void setServiceDispatcher(ServiceDispatcher serviceDispatcher) {
		this.serviceDispatcher = serviceDispatcher;
	}

	public abstract void start();

	public abstract void shutdown();

	public abstract boolean isRunning();
}
