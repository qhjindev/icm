package com.tnsoft.icm.sample.msg;

import java.io.IOException;
import java.util.Scanner;

import com.tnsoft.icm.msg4j.TvmEventAction;
import com.tnsoft.icm.msg4j.TvmEventActionMap;
import com.tnsoft.icm.msg4j.TvmPublisher;
import com.tnsoft.icm.msg4j.TvmSubscriber;
import com.tnsoft.icm.sample.SampleCase;
import com.tnsoft.icm.sample.msg.model.Request1;
import com.tnsoft.icm.sample.msg.model.Request2;
import com.tnsoft.icm.sample.msg.model.Request3;
import com.tnsoft.icm.sample.msg.model.Request4;

public class PubAndSubClient implements SampleCase {
	
	public static void main(String[] args) {
		TvmSubscriber subscriber = new TvmSubscriber(HOST, SUBSCRIBER_PORT);
		TvmPublisher publisher = new TvmPublisher(HOST, PUBLISHER_PORT);
		TvmEventActionMap eventActionMap = new TvmEventActionMap();
		TvmEventAction<Request3> action_1 = new Action4Request3();
		TvmEventAction<Request4> action_2 = new Action4Request4();
		eventActionMap.put(action_1);
		eventActionMap.put(action_2);
		try {
			subscriber.start();
			subscriber.subscribe(JAVA_TOPIC, eventActionMap);
			System.out.println("Subscribe the topic: " + JAVA_TOPIC);

			publisher.openAndConnect();
			System.out.println("publish topic: " + CPP_TOPIC);
			publisher.publish(CPP_TOPIC);
			System.out.println("Usage:");
			System.out.println("     exit          - exit the program");
			System.out.println("     send          - send the events");
			Scanner scanner = new Scanner(System.in);
			for (String cmd = scanner.nextLine(); !"exit".equals(cmd); cmd = scanner.nextLine()) {
				if ("send".equals(cmd)) {
					System.out.println("Sending event Request1...");
					publisher.event(CPP_TOPIC, generateRequest1());
					System.out.println("Finished.");
					System.out.println();
					System.out.println("Sending event Request2...");
					publisher.event(CPP_TOPIC, generateRequest2());
					System.out.println("Finished.");
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(0);
		} finally {
			try {
				publisher.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
			subscriber.stop();
		}
	}
	
	private static Request2 generateRequest2() {
		Request2 request2 = new Request2();
		request2.setSeq(2);
		request2.setInstanceId("instance-2");
		request2.setNodeId("node-2");
		request2.setResult("r-2");
		request2.setState(2);
		return request2;
	}

	private static Request1 generateRequest1() {
		Request1 request1 = new Request1();
		request1.setSeq(1);
		request1.setInstanceId("instance-1");
		request1.setNodeId("node-1");
		request1.setCpu(1);
		request1.setMem(1024);
		return request1;
	}

}
