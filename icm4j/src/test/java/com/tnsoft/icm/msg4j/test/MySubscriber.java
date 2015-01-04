package com.tnsoft.icm.msg4j.test;

import java.io.IOException;

import com.tnsoft.icm.msg4j.EventAction;
import com.tnsoft.icm.msg4j.Subscriber;

public class MySubscriber {

	private static final String MSG_SERVER = "172.16.10.20";
	private static final int MSG_PORT = 10002; //?
	private static final String TOPIC_ID_1 = "topic1";
	private static final String TOPIC_ID_2 = "topic2";

	public static void main(String[] args) throws Exception {
		MyAction action = new MyAction();
		Subscriber subscriber = new Subscriber(MSG_SERVER, MSG_PORT);
		subscriber.start();
		try {
			subscriber.subscribe(TOPIC_ID_1, action);
			subscriber.subscribe(TOPIC_ID_2, action);
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Input any key to stop the Subscriber...");
		System.in.read();
		subscriber.stop();
	}

	private static class MyAction implements EventAction<Event> {
		public Class<Event> getEventClass() {
			return Event.class;
		}

		public void execute(String topicId, Event event) {
			System.out.println("--------------------------------------------------------------------------");
			System.out.println("TopicID: " + topicId);
			System.out.println("\ttopic:" + event.getTopic());
			System.out.println("\titem1: " + event.getItem1());
			System.out.println("\titem2: " + event.getItem2());
			System.out.println("\tdevice: d=" + event.getDevice().getD() + ", a=" + event.getDevice().getA());
			for (int i = 0; i < event.getPorts().size(); i++) {
				Port p = event.getPorts().get(i);
				System.out.println("\tport_" + i + ": i=" + p.getI() + ", str=" + p.getStr());
			}
			System.out.println("--------------------------------------------------------------------------");
		}
	}
}
