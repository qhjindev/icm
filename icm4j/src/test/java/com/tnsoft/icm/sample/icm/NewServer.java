package com.tnsoft.icm.sample.icm;

import java.nio.ByteOrder;

import com.tnsoft.icm.icm4j.IcmTcpServer;
import com.tnsoft.icm.sample.SampleCase;
import com.tnsoft.icm.sample.icm.service.MyHello;
import com.tnsoft.icm.sample.icm.service.MyHelloImpl;

public class NewServer implements SampleCase {

	public static void main(String[] args) throws Throwable {
		MyHello myHello = new MyHelloImpl();
		IcmTcpServer server = new IcmTcpServer(RPC_PORT);
		server.setByteOrder(ByteOrder.BIG_ENDIAN);
		server.getServiceDispatcher().addService(MyHello.class, myHello);
		server.startup();
		System.in.read();
		server.shutdown();
	}

}
