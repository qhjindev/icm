package com.tnsoft.icm.sample.msg;

import java.text.SimpleDateFormat;
import java.util.Date;

import com.tnsoft.icm.msg4j.TvmEventAction;
import com.tnsoft.icm.sample.msg.model.Request4;

public class Action4Request4 extends TvmEventAction<Request4> {
	
	private static final SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

	static String getDate() {
		return format.format(new Date());
	}

	@Override
	public void execute(String topicId, Request4 evt) {
		System.out.println("---------------------------------------------------------------------");
		System.out.println(getDate());
		System.out.println("topic: " + topicId);
		System.out.println("Request3: ");
		System.out.println("  seq: " + evt.getSeq());
		System.out.println("  nodeId: " + evt.getNodeId());
		System.out.println("  instanceId: " + evt.getInstanceId());
		System.out.println("  result: " + evt.getResult());
		System.out.println("  state: " + evt.getState());
		System.out.println("---------------------------------------------------------------------");
	}
}
