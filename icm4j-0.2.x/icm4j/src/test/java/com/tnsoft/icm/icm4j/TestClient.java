package com.tnsoft.icm.icm4j;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.tnsoft.icm.icm4j.protocol.Out;

public class TestClient implements Runnable {

	public static void main(String[] args) {
		IcmTcpClient client = new IcmTcpClient("localhost", 12345);
		client.setConnectionMode(IcmTcpClient.MODE_LONG_CONNECTION);
//		client.setTimeout(1000);
//		client.setConnectionCount(3);
		Sample sample = client.getService(Sample.class);
		TestClient testClient = new TestClient(sample);
		ExecutorService executorService = Executors.newCachedThreadPool();
		for (int i = 0; i < 100; i++) {			
			executorService.execute(testClient);
		}
		try {
			System.in.read();
		} catch (IOException e) {
			e.printStackTrace();
		}
		executorService.shutdown();
//		try {
//			testClient.run();
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
		client.shutdown();
	}
	
	private Sample sample;
	
	public TestClient(Sample sample) {
		this.sample = sample;
	}

	public void run() {
		System.out.println(sample.ok());
		System.out.println("--------------------------------");
		Out<Double> out = new Out<Double>();
		System.out.println(sample.test(10086, true, out));
		System.out.println("out: " + out.get());
		System.out.println("================================");
		System.out.println("*******************************************");
		Out<Integer> oi = new Out<Integer>();
		Map<Integer, String> map = new HashMap<Integer, String>();
		map.put(1, "1");
		map.put(2, "2");
		map.put(3, "3");
		map.put(4, "4");
		System.out.println(sample.map(map, oi));
		System.out.println("*******************************************");
	}
}
