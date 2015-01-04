package com.tnsoft.icm.icm4j;

import java.io.IOException;

public class Test {

	public static void main(String[] args) {
		server();		
	}
	
	private static void server() {
		IcmTcpServer server = new IcmTcpServer(12345);
		server.addService(Sample.class, new SampleImpl());
		server.startup();
		try {
			System.in.read();
		} catch (IOException e) {
			e.printStackTrace();
		}
		server.shutdown();		
	}
}
