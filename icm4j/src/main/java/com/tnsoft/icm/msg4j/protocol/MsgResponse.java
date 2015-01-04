package com.tnsoft.icm.msg4j.protocol;

import java.nio.ByteBuffer;

import com.tnsoft.icm.util.BufferWriter;

public class MsgResponse extends Msg {

	public MsgResponse(String topicId) {
		this(TYPE_OK, topicId);
	}
	
	public MsgResponse(byte msgType, String topicId) {
		messageHead.setType(msgType);
		setTopicId(topicId);
	}
	
	public MsgResponse(ByteBuffer buffer) {
		setBuffer(buffer);
	}
	
	@Override
	public void writeBuffer(BufferWriter writer, int bufferSize) {
		allocateBuffer(bufferSize);
		writeHeadAndTopicId(writer);
		finishWrite(writer);
	}

}
