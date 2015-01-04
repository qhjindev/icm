package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.channels.SocketChannel;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public interface SocketChannelProvider {

	SocketChannel get(String hostname, int port) throws IOException;
	
	void back(SocketChannel channel);
	
	void shutdown();
}

class SimpleSocketChannelProvider implements SocketChannelProvider {

	private static final Log log = LogFactory.getLog(SimpleSocketProvider.class);

	private boolean alive = true;
	
	public SocketChannel get(String hostname, int port) throws IOException {
		if (alive) {
			return SocketChannel.open(new InetSocketAddress(hostname, port));
		} else {
			return null;
		}
	}

	public void back(SocketChannel channel) {
		if (channel != null) {
			try {
				channel.close();
			} catch (IOException e) {
				if (log.isErrorEnabled()) {
					log.error(e, e);
				}
			}
		}
	}

	public void shutdown() {
		if (alive) {
			alive = false;
		}
	}
	
}