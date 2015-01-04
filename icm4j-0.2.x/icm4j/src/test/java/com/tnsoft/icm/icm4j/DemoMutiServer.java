package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.util.List;
import java.util.Map;

public class DemoMutiServer {

	private static final int PORT_1 = 3001;
	private static final int PORT_2 = 3002;

	public static void main(String[] args) {
		IcmMutiServer mutiServer = new IcmMutiServer();
		mutiServer.addService(PORT_1, MyHello.class, new MyHelloImpl());
		mutiServer.addService(PORT_2, MyHello.class, new MyHelloImpl() {
			@Override
			public String sayHello1(List<String> req) {
				return "-- override\n" + super.sayHello1(req);
			}

			@Override
			public String sayHello1I(List<Integer> req) {
				return "-- override\n" + super.sayHello1I(req);
			}

			@Override
			public String sayHello1R(List<Req3> req) {
				return "-- override\n" + super.sayHello1R(req);
			}

			@Override
			public String sayHello2(Map<Integer, String> req) {
				return "-- override\n" + super.sayHello2(req);
			}

			@Override
			public String sayHello3(Req3 req) {
				return "-- override\n" + super.sayHello3(req);
			}
		});
		mutiServer.startup();
		try {
			System.in.read();
		} catch (IOException e) {}
		mutiServer.shutdown();
	}

}
