package com.tnsoft.icm.icm4j.protocol;

import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.atomic.AtomicInteger;

import com.tnsoft.icm.icm4j.protocol.annotation.Name;
import com.tnsoft.icm.icm4j.protocol.annotation.Operation;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;

public final class Request extends Message {

	private static final AtomicInteger IDENTITY_FACTORY = new AtomicInteger(0);

	private RequestHead head = new RequestHead();
	private RequestBody body = new RequestBody();

	public Request() {
		messageHead.setType(MessageHead.REQUEST);
	}

	public Request(int bufferSize) {
		this();
		super.bufferSize = bufferSize;
	}

	public Request(ByteBuffer buffer) {
		this();
		setBuffer(buffer);
	}

	public Request(Method method, Object[] arguments) {
		this();
		fillRequest(method, arguments);
	}

	public Request(Method method, Object[] arguments, int bufferSize) {
		this(bufferSize);
		fillRequest(method, arguments);
	}

	private byte responseFlag(Method method) {
		boolean flag = false;
		Class<?>[] types = method.getParameterTypes();
		for (Class<?> type : types) {
			if (type.isAssignableFrom(Out.class)) {
				flag = true;
			}
		}
		flag = flag || method.getReturnType() != void.class;
		return flag ? RequestHead.RESPONSE : RequestHead.NO_RESPONSE;
	}

	/**
	 * Fill the Request.
	 * 
	 * @param method
	 */
	public Request fillRequest(Method method, Object[] arguments) {
		// head
		head.setId(IDENTITY_FACTORY.getAndIncrement());
		head.setResponseFlag(responseFlag(method));
		Class<?> clazz = method.getDeclaringClass();
		Name an = clazz.getAnnotation(Name.class);
		if (an != null) {
			String name = an.value().length() > 0 ? an.value() : clazz.getSimpleName();
			head.setName(name);
		} else {
			head.setName(clazz.getName());
		}
		head.setCategory(""); // TODO What is category ?
		Operation o = method.getAnnotation(Operation.class);
		if (o != null && o.value().length() > 0) {
			head.setOperation(o.value());
		} else {
			head.setOperation(method.getName());
		}
		// body
		body.setTypes(method.getParameterTypes());
		body.setArguments(arguments);
		body.setGenericTypes(method.getGenericParameterTypes());
		return this;
	}

	public void readHead(BufferReader reader) {
		// message head
		// readMessageHead(reader);
		// head
		head.setId(reader.readInt(buffer));
		head.setResponseFlag(reader.readByte(buffer));
		head.setName(reader.readString(buffer));
		head.setCategory(reader.readString(buffer));
		head.setOperation(reader.readString(buffer));
	}

	@SuppressWarnings("rawtypes")
	public void readBody(BufferReader reader, Method method) {
		Class<?>[] types = method.getParameterTypes();
		Object[] args = new Object[types.length];
		Type[] gts = method.getGenericParameterTypes();
		for (int i = 0; i < args.length; i++) {
			if (types[i].isAssignableFrom(Out.class)) {
				args[i] = new Out();
			} else if (types[i].isAssignableFrom(Set.class)) {
				args[i] = reader.readSet(buffer, (ParameterizedType) gts[i]);
			} else if (types[i].isAssignableFrom(List.class)) {
				args[i] = reader.readList(buffer, (ParameterizedType) gts[i]);
			} else if (types[i].isAssignableFrom(Map.class)) {
				args[i] = reader.readMap(buffer, (ParameterizedType) gts[i]);
			} else {
				args[i] = reader.readObject(buffer, types[i]);
			}
		}
		body.setTypes(types);
		body.setArguments(args);
		body.setGenericTypes(gts);
	}

	@Override
	public void writeBuffer(BufferWriter writer, int bufferSize) {
		allocateBuffer(bufferSize);
		// message head
		writer.writeInt(buffer, 0);
		writer.writeByte(buffer, messageHead.getType());
		// head
		writer.writeInt(buffer, head.getId());
		writer.writeByte(buffer, head.getResponseFlag());
		writer.writeString(buffer, head.getName());
		writer.writeString(buffer, head.getCategory());
		writer.writeString(buffer, head.getOperation());
		// body
		Class<?>[] types = body.getTypes();
		Object[] values = body.getArguments();
		Type[] gts = body.getGenericTypes();
		if (values != null) {
			for (int i = 0; i < values.length; i++) {
				if (types[i].isAssignableFrom(Out.class)) {
					continue;
				} else if (types[i].isAssignableFrom(Set.class)) {
					writer.writeSet(buffer, (Set<?>) values[i], (ParameterizedType) gts[i]);
				} else if (types[i].isAssignableFrom(List.class)) {
					writer.writeList(buffer, (List<?>) values[i], (ParameterizedType) gts[i]);
				} else if (types[i].isAssignableFrom(Map.class)) {
					writer.writeMap(buffer, (Map<?, ?>) values[i], (ParameterizedType) gts[i]);
				} else {
					writer.writeObject(buffer, values[i], types[i]);
				}
			}
		}
		// return to set the message size in message head
		int p = buffer.position();
		buffer.position(0);
		writer.writeInt(buffer, p - MessageHead.MESSAGE_HEAD_SIZE);
		buffer.position(p);
		// write end
		buffer.flip();
		// set message size
		messageHead.setSize(buffer.limit());
	}

	public RequestHead getHead() {
		return head;
	}

	public void setHead(RequestHead head) {
		this.head = head;
	}

	public RequestBody getBody() {
		return body;
	}

	public void setBody(RequestBody body) {
		this.body = body;
	}

}
