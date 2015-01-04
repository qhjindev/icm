package com.tnsoft.icm.util;

import java.io.IOException;
import java.nio.channels.SocketChannel;
import java.util.Deque;
import java.util.concurrent.LinkedBlockingDeque;

import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public class DequeChannelPool extends SimpleChannelPool {

	private static final Log log = LogFactory.getFormatterLog(DequeChannelPool.class);

	private Deque<SocketChannel> channels;
	private boolean stoped;

	public DequeChannelPool(String hostname, int port) {
		this(hostname, port, 3);
	}
	
	public DequeChannelPool(String hostname, int port, int maxWait) {
		super(hostname, port);
		if (maxWait < 1) {
			throw new RuntimeException("Error input arguments: maxWait=" + maxWait);
		}
		channels = new LinkedBlockingDeque<SocketChannel>(maxWait);
		log.debug("Create a new DequeChannelPool with size [%d].", maxWait);
	}

	@Override
	public SocketChannel get() throws IOException {
		if (stoped) {
			return null;
		}
		SocketChannel channel = channels.pollLast();
		if (channel == null) {
			channel = super.get();
		}
		return channel;
	}
	
	@Override
	public void back(SocketChannel channel) {
		if (stoped) {
			super.back(channel);
			return;
		}
		if (!channels.offerLast(channel)) {
			super.back(channel);
		}
	}
	
	@Override
	public synchronized void stopPool() {
		stoped = true;
		channels.clear();
		channels = null;
		log.info("Stop A DequeChannelPool for endpoint [%s:%d]", hostname, port);
	}

}
