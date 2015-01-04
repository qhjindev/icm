package com.tnsoft.icm.sample.msg;

import java.io.IOException;

import com.tnsoft.icm.msg4j.TvmEventAction;
import com.tnsoft.icm.msg4j.TvmEventActionMap;
import com.tnsoft.icm.msg4j.TvmSubscriber;
import com.tnsoft.icm.sample.SampleCase;
import com.tnsoft.icm.sample.msg.model.Request3;
import com.tnsoft.icm.sample.msg.model.Request4;

public class SubscriberClient implements SampleCase {
	
	public static void main(String[] args) throws Exception {
		TvmSubscriber subscriber = new TvmSubscriber(HOST, SUBSCRIBER_PORT);
		TvmEventActionMap eventActionMap = new TvmEventActionMap();
		TvmEventAction<Request3> action_1 = new Action4Request3();
		TvmEventAction<Request4> action_2 = new Action4Request4();
		eventActionMap.put(action_1);
		eventActionMap.put(action_2);
		subscriber.start();
		try {
			subscriber.subscribe(JAVA_TOPIC, eventActionMap);
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Input any key to stop the Subscriber...");
		System.in.read();
		subscriber.stop();
	}

}

