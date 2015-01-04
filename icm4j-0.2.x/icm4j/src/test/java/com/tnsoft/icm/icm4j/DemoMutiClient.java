package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class DemoMutiClient {

	private static final int PORT_1 = 3001;
	private static final int PORT_2 = 3002;
//	private static final String HOST = "localhost";
	private static final String HOST = "172.16.10.23";
	private static final int PORT = 3000;

	public static void main(String[] args) {
		client();
	}

	private static void client() {
		IcmMutiClient client = new IcmMutiClient();

//		IcmRemoteService remoteService = new IcmRemoteService(HOST, PORT_1);
//		remoteService.setConnectionMode(ConnectionMode.MODE_LONG_CONNECTION);
//		// remoteService.setConnectionCount(1);
//		client.addRemoteService(remoteService);
//
//		IcmRemoteService remoteService2 = new IcmRemoteService(HOST, PORT_2);
//		remoteService2.setConnectionMode(ConnectionMode.MODE_LONG_CONNECTION);
//		// remoteService2.setConnectionCount(1);
//		client.addRemoteService(remoteService2);
		
		IcmRemoteService remoteService = new IcmRemoteService(HOST, PORT);
		remoteService.setConnectionMode(ConnectionMode.MODE_LONG_CONNECTION);
		client.addRemoteService(remoteService);

		MyHello myHello = remoteService.getService(MyHello.class);
//		MyHello myHello2 = remoteService2.getService(MyHello.class);
		
		ExecutorService executorService = Executors.newFixedThreadPool(20);
		
		System.out.println("ready!");
		try {
			System.in.read();
		} catch (IOException e) {}
		System.out.println("go!");
		
		for (int i = 0; i < 200; i++) {
			executorService.execute(new ClientInvoker(myHello));
		}
//		executorService.execute(new ClientInvoker(myHello));
//		executorService.execute(new ClientInvoker(myHello2));

		try {
			System.in.read();
		} catch (IOException e) {}
		executorService.shutdown();
		client.shutdown();
	}

	static class ClientInvoker implements Runnable {
		private MyHello myHello;

		public ClientInvoker(MyHello myHello) {
			this.myHello = myHello;
		}

		public void run() {
			String result = myHello.sayHello1(Arrays.asList("a", "b", "c", "d", "e", "f", "g"));
			System.out.println("-- sayHello1");
			System.out.println("-- " + result);
			System.out.println("-------------------------------------------------");
			result = myHello.sayHello1I(Arrays.asList(1, 2, 3, 4, 5, 6, 7, 8, 9, 0));
			System.out.println("-- sayHello1I");
			System.out.println("-- " + result);
			System.out.println("-------------------------------------------------");
			List<Req3> list = new ArrayList<Req3>();
			for (int i = 1; i <= 5; i++) {
				Req3 req = new Req3();
				req.setSvn(i);
				req.setRequest1(Arrays.asList(i + "-a", i + "-b", i + "-c", i + "-d"));
				list.add(req);
			}
			result = myHello.sayHello1R(list);
			System.out.println("-- sayHello1I");
			System.out.println("-- " + result);
			System.out.println("-------------------------------------------------");
			Map<Integer, String> map = new LinkedHashMap<Integer, String>();
			map.put(1, "a");
			map.put(2, "b");
			map.put(3, "c");
			result = myHello.sayHello2(map);
			System.out.println("-- sayHello2");
			System.out.println("-- " + result);
			System.out.println("-------------------------------------------------");
			Req3 req3 = new Req3();
			req3.setSvn(100);
			req3.setRequest1(Arrays.asList("aa", "bb", "cc", "dd"));
			myHello.sayHello3(req3);
			System.out.println("-- sayHello3");
			System.out.println("-- " + result);
			System.out.println("=================================================");
		}
	}
}
