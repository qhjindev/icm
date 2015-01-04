package com.tnsoft.icm.sample.msg;

import java.io.IOException;

import com.tnsoft.icm.msg4j.TvmPublisher;
import com.tnsoft.icm.sample.SampleCase;
import com.tnsoft.icm.sample.msg.model.Request1;
import com.tnsoft.icm.sample.msg.model.Request2;

public class PublisherClient implements SampleCase {

	public static void main(String[] args) {
		TvmPublisher publisher = new TvmPublisher(HOST, PUBLISHER_PORT);
		try {
			publisher.openAndConnect();
			System.out.println("publish topic: " + CPP_TOPIC);
			publisher.publish(CPP_TOPIC);
			
			Request1 req1 = generateRequest1();
			System.out.println("Send event Request1.");
			publisher.event(CPP_TOPIC, req1);
			
			Request2 req2 = generateRequest2();
			System.out.println("Send event Request2.");
			publisher.event(CPP_TOPIC, req2);
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
