package com.tnsoft.icm.icm4j.test.simple4c;

import com.tnsoft.icm.icm4j.TCPICMServer;
import com.tnsoft.icm.icm4j.TCPNioServer;
import com.tnsoft.icm.icm4j.protocol.extension.Out;

public class TestServer4C {

public static final int PORT = 3000;
	
	public static void main(String[] args) throws Exception {
		TCPICMServer tcpServer = new TCPNioServer(PORT);
		tcpServer.getServiceDispatcher().addService(MyHello.class, new MyHelloImpl());
		tcpServer.start();
		System.out.println("-- input any key to shutdown the server:");
		System.in.read();
		tcpServer.shutdown();
	}
}

class MyHelloImpl implements MyHello {

	public String sayHello(String msg, short u, Out<Long> out) {
		out.set(System.currentTimeMillis());
		System.out.println(msg + "u:" + u);
		return msg + "u:" + u;
	}
	
}
