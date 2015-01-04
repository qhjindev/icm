package com.tnsoft.icm.sample.icm;

import com.tnsoft.icm.icm4j.TCPNioServer;
import com.tnsoft.icm.sample.SampleCase;
import com.tnsoft.icm.sample.icm.service.MyHello;
import com.tnsoft.icm.sample.icm.service.MyHelloImpl;

public class Server implements SampleCase {

	public static void main(String[] args) throws Throwable {
		MyHello myHello = new MyHelloImpl();
		TCPNioServer server = new TCPNioServer(RPC_PORT);
		server.getServiceDispatcher().addService(MyHello.class, myHello);
		server.start();
		System.out.println("-- input any key to shutdown the server:");
		System.in.read();
		server.shutdown();
	}
}
