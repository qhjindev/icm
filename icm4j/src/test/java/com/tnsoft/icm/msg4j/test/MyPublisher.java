package com.tnsoft.icm.msg4j.test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.tnsoft.icm.msg4j.Publisher;

public class MyPublisher {

	private static final String MSG_SERVER = "172.16.10.20";
	private static final int MSG_PORT = 10001; //?
	private static final String TOPIC_ID_1 = "topic1";
	private static final String TOPIC_ID_2 = "topic2";
	
	public static void main(String[] args) {
		Publisher publisher = new Publisher(MSG_SERVER, MSG_PORT);
		try {
			// open the socket channel and connect the MsgServer
			publisher.openAndConnect();
			// publish the topics
			publisher.publish(TOPIC_ID_1);
			publisher.publish(TOPIC_ID_2);
			// send events
			Event event = generateEvent(TOPIC_ID_1);
			publisher.event(TOPIC_ID_1, event);
			event = generateEvent(TOPIC_ID_2);
			publisher.event(TOPIC_ID_2, event);
			// un-publish the topics
//			publisher.unpublish(TOPIC_ID_1);
//			publisher.unpublish(TOPIC_ID_2);
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				publisher.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	private static Event generateEvent(String topicId) {
		Event e = new Event();
		e.setTopic(topicId);
		e.setItem1((short) 1);
		e.setItem2("2");
		Device device = new Device();
		device.setA("device:a");
		device.setA("device:b");
		e.setDevice(device);
		List<Port> ports = new ArrayList<Port>();
		Port port = new Port();
		port.setI(10086);
		port.setStr("101.102.103.104");
		ports.add(port);
		e.setPorts(ports);
		return e;
	}
}
