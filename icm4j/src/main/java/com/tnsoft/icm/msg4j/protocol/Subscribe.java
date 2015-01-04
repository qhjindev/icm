package com.tnsoft.icm.msg4j.protocol;

import java.nio.ByteBuffer;

import com.tnsoft.icm.util.BufferWriter;

public final class Subscribe extends Msg {

	public Subscribe(String topicId) {
		this(true, topicId);
	}

	public Subscribe(boolean subscribe, String topicId) {
		messageHead.setSize(0);
		messageHead.setType(subscribe ? TYPE_SUBSCRIBE : TYPE_UNSUBSCRIBE);
		setTopicId(topicId);
	}
	
	public Subscribe(ByteBuffer buffer) {
		setBuffer(buffer);
	}

	@Override
	public void writeBuffer(BufferWriter writer, int bufferSize) {
		allocateBuffer(bufferSize);
		writeHeadAndTopicId(writer);
		finishWrite(writer);
	}

}
