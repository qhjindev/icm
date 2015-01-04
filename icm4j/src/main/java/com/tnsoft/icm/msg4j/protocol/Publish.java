package com.tnsoft.icm.msg4j.protocol;

import java.nio.ByteBuffer;

import com.tnsoft.icm.util.BufferWriter;

public final class Publish extends Msg {

	public Publish(String topicId) {
		this(true, topicId);
	}

	public Publish(boolean publish, String topicId) {
		messageHead.setType(publish ? TYPE_PUBLISH : TYPE_UNPUBLISH);
		setTopicId(topicId);
	}

	public Publish(ByteBuffer buffer) {
		setBuffer(buffer);
	}

	@Override
	public void writeBuffer(BufferWriter writer, int bufferSize) {
		allocateBuffer(bufferSize);
		writeHeadAndTopicId(writer);
		finishWrite(writer);
	}

}
