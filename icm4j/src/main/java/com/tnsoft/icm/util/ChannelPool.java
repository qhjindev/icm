package com.tnsoft.icm.util;

import java.io.IOException;
import java.nio.channels.SocketChannel;

public interface ChannelPool {

	SocketChannel get() throws IOException;
	
	void back(SocketChannel channel);
	
	void stopPool();
}
