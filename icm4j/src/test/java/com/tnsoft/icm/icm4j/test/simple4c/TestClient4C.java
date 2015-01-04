package com.tnsoft.icm.icm4j.test.simple4c;

import com.tnsoft.icm.icm4j.ServiceFactory;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.util.PoolProviderFactory;

public class TestClient4C {

	private static final String ip = "172.16.10.101";
	private static final int port = 3000;
	
	public static void main(String[] args) {
		// get the service factory
		ServiceFactory factory = ServiceFactory.getInstance();
		factory.setPoolProvider(PoolProviderFactory.newDequeChannelPoolProvider());
		
		MyHello myHello = factory.getService(MyHello.class, ip, port);
		Out<Long> lo = new Out<Long>();
		String result = myHello.sayHello("Hello from icm4j!", (short) 12, lo);
		System.out.println("out: " + lo.get());
		System.out.println("result: " + result);
		System.out.println();
		System.out.println("------------------------------------------------");
		System.out.println();
		MyH myH = factory.getService(MyH.class, ip, port);
		result = myH.hello("Another hello from icm4j!", (short) 36, lo);
		System.out.println("out: " + lo.get());
		System.out.println("result: " + result);

		factory.stop();
	}

}
