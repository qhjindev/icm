package com.tnsoft.icm.msg4j.protocol;

import com.tnsoft.icm.icm4j.protocol.Message;
import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;

public abstract class Msg extends Message {

	public static final byte TYPE_SUBSCRIBE = 0;
	public static final byte TYPE_UNSUBSCRIBE = 1;
	public static final byte TYPE_PUBLISH = 2;
	public static final byte TYPE_UNPUBLISH = 3;
	public static final byte TYPE_EVENT = 4;
	public static final byte TYPE_UNKNOW = 5;
	public static final byte TYPE_OK = 6;

	protected String topicId;

	public String getTopicId() {
		return topicId;
	}

	public void setTopicId(String topicId) {
		this.topicId = topicId;
	}
	
	public void readTopicId(BufferReader reader) {
		setTopicId(reader.readString(buffer));
	}

	protected void writeHeadAndTopicId(BufferWriter writer) {
		// message head
		writer.writeInt(buffer, 0);
		writer.writeByte(buffer, messageHead.getType());
		// topicId
		writer.writeString(buffer, topicId);
	}

}
