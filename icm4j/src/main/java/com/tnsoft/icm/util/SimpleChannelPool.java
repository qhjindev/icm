package com.tnsoft.icm.util;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.SocketChannel;

import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public class SimpleChannelPool implements ChannelPool {
	
	private static final Log log = LogFactory.getFormatterLog(SimpleChannelPool.class);

	protected String hostname;
	protected int port;
	
	public SimpleChannelPool(String hostname, int port) {
		this.hostname = hostname;
		this.port = port;
	}
	
	public SocketChannel get() throws IOException {
		SocketChannel channel = SocketChannel.open();
		channel.connect(new InetSocketAddress(hostname, port));
		channel.finishConnect();
		return channel;
	}

	public void back(SocketChannel channel) {
		if (channel != null) {
			try {
				channel.close();
			} catch (IOException e) {
				log.error(e);
				throw new RuntimeException(e);
			}
		}
	}

	public void stopPool() {}

}
