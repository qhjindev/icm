package com.tnsoft.icm.icm4j;

import java.nio.ByteOrder;
import java.nio.charset.Charset;

public class IcmService {

	protected final int port;
	protected final TransferProtocol protocol;
	protected ByteOrder byteOrder = ByteOrder.BIG_ENDIAN;
	protected Charset charset = Charset.forName("UTF-8");
	protected BufferProvider bufferProvider = CachedBufferProvider.getInstance(byteOrder);
	protected BufferReaderProvider bufferReaderProvider = CachedBufferReaderProvider.getInstance();
	protected BufferWriterProvider bufferWriterProvider = CachedBufferWriterProvider.getInstance();

	public IcmService(int port) {
		this(port, TransferProtocol.TCP);
	}
	
	public IcmService(int port, TransferProtocol protocol) {
		this.port = port;
		this.protocol = protocol;
	}

	public int getPort() {
		return port;
	}
	
	public TransferProtocol getProtocol() {
		return protocol;
	}

	public ByteOrder getByteOrder() {
		return byteOrder;
	}

	public void setByteOrder(ByteOrder byteOrder) {
		if (byteOrder == null)
			throw new NullPointerException();
		this.byteOrder = byteOrder;
	}

	public Charset getCharset() {
		return charset;
	}

	public void setCharset(Charset charset) {
		if (charset == null)
			throw new NullPointerException();
		this.charset = charset;
	}

	public BufferProvider getBufferProvider() {
		return bufferProvider;
	}

	public void setBufferProvider(BufferProvider bufferProvider) {
		if (bufferProvider == null)
			throw new NullPointerException();
		this.bufferProvider = bufferProvider;
	}

	public BufferReaderProvider getBufferReaderProvider() {
		return bufferReaderProvider;
	}

	public void setBufferReaderProvider(BufferReaderProvider bufferReaderProvider) {
		if (bufferReaderProvider == null)
			throw new NullPointerException();
		this.bufferReaderProvider = bufferReaderProvider;
	}

	public BufferWriterProvider getBufferWriterProvider() {
		return bufferWriterProvider;
	}

	public void setBufferWriterProvider(BufferWriterProvider bufferWriterProvider) {
		if (bufferWriterProvider == null)
			throw new NullPointerException();
		this.bufferWriterProvider = bufferWriterProvider;
	}

}
