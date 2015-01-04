package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.channels.SocketChannel;

import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public interface SocketProvider {

	Socket get(String hostname, int port) throws UnknownHostException, IOException;

	void back(Socket socket);

	void shutdown();
}

class SimpleSocketProvider implements SocketProvider {

	private static final Log log = LogFactory.getFormatterLog(SimpleSocketProvider.class);

	private boolean alive = true;

	public Socket get(String hostname, int port) throws UnknownHostException, IOException {
		if (alive) {
			return SocketChannel.open(new InetSocketAddress(hostname, port)).socket();
//			return new Socket(hostname, port);
		} else {
			return null;
		}
	}

	public void back(Socket socket) {
		if (socket != null) {
			try {
//				socket.getChannel().close();
				socket.close();
			} catch (IOException e) {
				log.error(e.getMessage(), e);
			}
		}
	}

	public synchronized void shutdown() {
		if (alive) {
			alive = false;
		}
	}

}
