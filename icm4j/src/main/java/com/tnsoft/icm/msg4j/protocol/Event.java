package com.tnsoft.icm.msg4j.protocol;

import java.nio.ByteBuffer;

import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;

public final class Event extends Msg {

	public Event(String topicId, Object event) {
		messageHead.setType(TYPE_EVENT);
		setTopicId(topicId);
		setEvent(event);
	}

	public Event(ByteBuffer buffer) {
		setBuffer(buffer);
	}

	private Object event;

	public Object getEvent() {
		return event;
	}

	public void setEvent(Object event) {
		this.event = event;
	}

	public void readEvent(Class<?> eventType, BufferReader reader) {
		setEvent(reader.readObject(buffer, eventType));
	}

	@Override
	public void writeBuffer(BufferWriter writer, int bufferSize) {
		allocateBuffer(bufferSize);
		writeHeadAndTopicId(writer);
		// write event
		writer.writeObject(buffer, event, event.getClass());
		finishWrite(writer);
	}

}
