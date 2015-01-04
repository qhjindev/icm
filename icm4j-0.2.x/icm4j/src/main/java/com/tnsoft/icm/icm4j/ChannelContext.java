package com.tnsoft.icm.icm4j;

import java.nio.channels.SelectableChannel;
import java.nio.charset.Charset;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentMap;

public class ChannelContext {

	private SelectableChannel channel;
	private IcmContext requestContext;
	private IcmContext responseContext;
	private BlockingQueue<IcmContext> contextQueue;
	private BufferProvider bufferProvider;
	private BufferReaderProvider bufferReaderProvider;
	private BufferWriterProvider bufferWriterProvider;
	private Charset charset;
	private String hostname;
	private int localPort;
	private int remotePort;
	private ConcurrentMap<Integer, IcmContext> responseWaitedContextMap;

	public IcmContext getRequestContext() {
		return requestContext;
	}

	public void setRequestContext(IcmContext requestContext) {
		this.requestContext = requestContext;
	}

	public void removeRequestContext() {
		setRequestContext(null);
	}

	public IcmContext getResponseContext() {
		return responseContext;
	}

	public void setResponseContext(IcmContext responseContext) {
		this.responseContext = responseContext;
	}

	public void removeResponseContext() {
		setResponseContext(null);
	}

	public BlockingQueue<IcmContext> getContextQueue() {
		return contextQueue;
	}

	public void setContextQueue(BlockingQueue<IcmContext> contextQueue) {
		this.contextQueue = contextQueue;
	}

	public BufferProvider getBufferProvider() {
		return bufferProvider;
	}

	public void setBufferProvider(BufferProvider bufferProvider) {
		this.bufferProvider = bufferProvider;
	}

	public BufferReaderProvider getBufferReaderProvider() {
		return bufferReaderProvider;
	}

	public void setBufferReaderProvider(BufferReaderProvider bufferReaderProvider) {
		this.bufferReaderProvider = bufferReaderProvider;
	}

	public BufferWriterProvider getBufferWriterProvider() {
		return bufferWriterProvider;
	}

	public void setBufferWriterProvider(BufferWriterProvider bufferWriterProvider) {
		this.bufferWriterProvider = bufferWriterProvider;
	}

	public Charset getCharset() {
		return charset;
	}

	public void setCharset(Charset charset) {
		this.charset = charset;
	}

	public String getHostname() {
		return hostname;
	}

	public void setHostname(String hostname) {
		this.hostname = hostname;
	}

	public int getLocalPort() {
		return localPort;
	}

	public void setLocalPort(int localPort) {
		this.localPort = localPort;
	}

	public int getRemotePort() {
		return remotePort;
	}

	public void setRemotePort(int remotePort) {
		this.remotePort = remotePort;
	}

	public SelectableChannel getChannel() {
		return channel;
	}

	public void setChannel(SelectableChannel channel) {
		this.channel = channel;
	}

	public void removeChannel() {
		setChannel(null);
	}

	public void clear() {
		removeChannel();
		removeRequestContext();
		removeResponseContext();
	}

	public ConcurrentMap<Integer, IcmContext> getResponseWaitedContextMap() {
		return responseWaitedContextMap;
	}

	public void setResponseWaitedContextMap(ConcurrentMap<Integer, IcmContext> responseWaitedContextMap) {
		this.responseWaitedContextMap = responseWaitedContextMap;
	}

}
