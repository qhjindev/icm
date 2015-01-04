package com.tnsoft.icm.icm4j.test;

import com.tnsoft.icm.icm4j.TCPNioServer;
import com.tnsoft.icm.icm4j.TCPICMServer;

public class TestNioServer {

	public static final int PORT = 12345;
	
	public static void main(String[] args) throws Exception {
		TCPICMServer tcpServer = new TCPNioServer(PORT);
		tcpServer.getServiceDispatcher().addService(TestService.class, new TestServiceImpl());
		tcpServer.start();
		System.out.println("-- input any key to shutdown the server:");
		System.in.read();
		tcpServer.shutdown();
	}
}
