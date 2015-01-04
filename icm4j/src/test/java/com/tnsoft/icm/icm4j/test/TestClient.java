package com.tnsoft.icm.icm4j.test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import com.tnsoft.icm.icm4j.ServiceFactory;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.util.PoolProviderFactory;

public class TestClient {

	private static TestService service;

	public static void main(String[] args) throws Exception {
		// get the service factory
		final ServiceFactory factory = ServiceFactory.getInstance();
		factory.setPoolProvider(PoolProviderFactory.newDequeChannelPoolProvider(3));
		service = factory.getService(TestService.class, "localhost", TestNioServer.PORT);
		// test
		invokeService();
		// finish
		factory.stop();
		// thread count
//		int count = 5;
//		final boolean[] b = new boolean[count];
//		List<Thread> list = new ArrayList<Thread>();
//		for (int i = 0; i < count; i++) {
//			final int index = i;
//			list.add(new Thread(new Runnable() {
//				public void run() {
//					try {
//						Thread.sleep(index / 3 * 100);
//					} catch (InterruptedException e) {
//						e.printStackTrace();
//					}
//					invokeService();
//					synchronized (b) {
//						b[index] = true;
//						for (int j = 0; j < b.length; j++) {
//							if (!b[j]) {
//								return;
//							}
//						}
//						// all finished
//						factory.stop();
//					}
//				}
//			}));
//		}
//		for (Thread thread : list) {
//			thread.start();
//		}
	}

	private static void invokeService() {
		// invoke remote methods
		System.out.println("result: " + service.getLength("5"));
		System.out.println("==================================================");
		System.out.println("result: " + service.generateID("test", 20));
		System.out.println("==================================================");
		System.out.println("send \"test\" but no result");
		service.noResultJustPrint("test");
		System.out.println("==================================================");
		System.out.println("result: " + service.sum(new int[] { 1, 2, 3, 4, 5 }));
		System.out.println("==================================================");
		System.out.println("result: " + service.division(1234.56f, 9.4f));
		System.out.println("==================================================");
		System.out.println("result: " + Arrays.toString(service.split("a,b,c,d,e", ",")));
		System.out.println("==================================================");
		System.out.println("result: "
				+ Arrays.toString(service.sort(new int[] { 5, 9, 2, 7, 1, 87, 234, 78, 123, 79, 234, 78, 34, 8, 3 })));
		System.out.println("==================================================");
		Map<String, Integer> map = new LinkedHashMap<String, Integer>();
		map.put("a", 1);
		map.put("b", 2);
		map.put("c", 3);
		System.out.println("result: " + service.callback(map));
		System.out.println("===================================================");
		List<Integer> il = Arrays.asList(1, 2, 3, 4, 5);
		System.out.println("result: " + service.convert(il));
		System.out.println("===================================================");
		Out<Long> out = new Out<Long>();
		service.testOut_1(1234, out);
		System.out.println("out: " + out.get());
		System.out.println("===================================================");
		Out<String> helloWorld = new Out<String>();
		System.out.println("result: " + service.testOut_2(1234, helloWorld));
		System.out.println("out: " + helloWorld.get());
		System.out.println("===================================================");
		Struct struct = new Struct();
		struct.setName("test");
		struct.setAge(20);
		List<String> telephones = new ArrayList<String>();
		telephones.add("1234567890");
		struct.setTelephones(telephones);
		struct = service.testStruct(struct);
		System.out.println("result: " + struct);
		System.out.println("\tname: " + struct.getName());
		System.out.println("\tage: " + struct.getAge());
		System.out.println("\ttelephones: " + struct.getTelephones());
	}

}
