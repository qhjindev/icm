package com.tnsoft.icm.msg4j.protocol;

import java.nio.ByteBuffer;

import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;

public final class TvmEvent extends Msg {

	public TvmEvent(String topicId, Object event) {
		messageHead.setType(TYPE_EVENT);
		setTopicId(topicId);
		setEvent(event);
	}

	public TvmEvent(ByteBuffer buffer) {
		setBuffer(buffer);
	}

	private String type;
	private Object event;

	public String getType() {
		return type;
	}

	public Object getEvent() {
		return event;
	}

	public void setEvent(Object event) {
		this.event = event;
		type = event.getClass().getSimpleName();
	}

	public void readType(BufferReader reader) {
		type = reader.readString(buffer);
	}

	@Override
	public void writeBuffer(BufferWriter writer, int bufferSize) {
		allocateBuffer(bufferSize);
		writeHeadAndTopicId(writer);
		// write event
		writer.writeString(buffer, type);
		writer.writeObject(buffer, event, event.getClass());
		finishWrite(writer);
	}

}
