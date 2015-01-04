package com.tnsoft.icm.icm4j;

import java.io.IOException;
//import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class DemoClient implements Runnable {

	private static final int PORT = 3000;
//	private static final String HOST = "172.16.10.23";
	private static final String HOST = "localhost";

	public static void main(String[] args) {
		client();
	}

	private static void client() {
		IcmTcpClient client = new IcmTcpClient(HOST, PORT);
		client.setConnectionMode(IcmTcpClient.MODE_LONG_CONNECTION);
//		client.setConnectionCount(3);
//		client.setTimeout(1000);
//		client.setByteOrder(ByteOrder.BIG_ENDIAN);
//		client.startup();
		MyHello myHello = client.getService(MyHello.class);
		DemoClient demoClient = new DemoClient(myHello);
		ExecutorService executorService = Executors.newFixedThreadPool(20);
		for (int i = 0; i < 200; i++) {
			executorService.execute(demoClient);
		}
		try {
			System.in.read();
		} catch (IOException e) {
			e.printStackTrace();
		}
		executorService.shutdown();
		client.shutdown();
	}

	private MyHello myHello;
	
	private AtomicInteger identity = new AtomicInteger(1);
	
	public DemoClient(MyHello myHello) {
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
		System.out.println("-------------------------------------------------");
		System.out.println(identity.getAndIncrement());
		System.out.println("=================================================");
	}
}
