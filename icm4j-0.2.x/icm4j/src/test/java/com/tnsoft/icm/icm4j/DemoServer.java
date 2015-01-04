package com.tnsoft.icm.icm4j;

import java.io.IOException;

public class DemoServer {

	private static final int PORT = 3000;
	
	public static void main(String[] args) {
		server();
	}

	private static void server() {
		IcmTcpServer server = new IcmTcpServer(PORT);
		server.addService(MyHello.class, new MyHelloImpl());
		server.startup();
		try {
			System.in.read();
		} catch (IOException e) {
			e.printStackTrace();
		}
		server.shutdown();
	}
}
