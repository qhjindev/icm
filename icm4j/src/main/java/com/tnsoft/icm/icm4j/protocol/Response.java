package com.tnsoft.icm.icm4j.protocol;

import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;

public final class Response extends Message {

	private ResponseHead head = new ResponseHead();
	private ResponseBody body = new ResponseBody();

	private boolean hasResponse = false;

	public void hasResponse(boolean hasResponse) {
		this.hasResponse = hasResponse;
	}

	public Response(int id) {
		messageHead.setType(MessageHead.RESPONSE);
		head.setId(id);
	}

	public Response(ByteBuffer buffer) {
		setBuffer(buffer);
	}

	public Response(int id, int bufferSize) {
		this(id);
		super.bufferSize = bufferSize;
	}

	public Response setOutParameters(Method method, List<Object> outs) {
		// head
		head.setReplyStatus(ResponseHead.OK);
		// body
		body.getOutArguments().addAll(outs);
		Class<?>[] types = method.getParameterTypes();
		Type[] gts = method.getGenericParameterTypes();
		for (int i = 0; i < types.length; i++) {
			if (types[i].isAssignableFrom(Out.class)) {
				Class<?> clazz = (Class<?>) ((ParameterizedType) gts[i]).getActualTypeArguments()[0];
				body.getOutParameterTypes().add(clazz);
			}
		}
		return this;
	}

	public Response setResult(Object result, Method method) {
		// head
		if (result == null) {
			head.setReplyStatus(ResponseHead.RESULT_IS_NULL);
		} else {
			head.setReplyStatus(ResponseHead.OK);
		}
		// body
		body.setResult(result);
		body.setResultType(method.getReturnType());
		body.setGenericType(method.getGenericReturnType());
		return this;
	}

	public Response setReplyStatus(int replyStatus) {
		head.setReplyStatus(replyStatus);
		this.body = null;
		return this;
	}

	public void readHead(BufferReader reader) {
		// message head
		// readMessageHead(reader);
		// read head
		head.setId(reader.readInt(buffer));
		head.setReplyStatus(reader.readInt(buffer));
	}

	public void readBody(BufferReader reader, Method method) {
		if (head.getReplyStatus() == ResponseHead.OK) {
			// read out parameters
			Class<?> type = method.getReturnType();
			Type gt = method.getGenericReturnType();
			body.setResultType(type);
			body.setGenericType(gt);
			Class<?>[] types = method.getParameterTypes();
			Type[] gts = method.getGenericParameterTypes();
			for (int i = 0; i < types.length; i++) {
				if (types[i].isAssignableFrom(Out.class)) {
					Class<?> clazz = (Class<?>) ((ParameterizedType) gts[i]).getActualTypeArguments()[0];
					body.getOutParameterTypes().add(clazz);
					body.getOutArguments().add(reader.readObject(buffer, clazz));
				}
			}
			// read result
			if (type == void.class) {
				return;
			} else if (type.isAssignableFrom(Set.class)) {
				body.setResult(reader.readSet(buffer, (ParameterizedType) gt));
			} else if (type.isAssignableFrom(List.class)) {
				body.setResult(reader.readList(buffer, (ParameterizedType) gt));
			} else if (type.isAssignableFrom(Map.class)) {
				body.setResult(reader.readMap(buffer, (ParameterizedType) gt));
			} else {
				body.setResult(reader.readObject(buffer, type));
			}
		} else if (head.getReplyStatus() == ResponseHead.RESULT_IS_NULL) {
			Class<?> type = method.getReturnType();
			Type gt = method.getGenericReturnType();
			body.setResultType(type);
			body.setGenericType(gt);
			body.setResult(null);
		}
	}

	@Override
	public void writeBuffer(BufferWriter writer, int bufferSize) {
		allocateBuffer(bufferSize);
		// message head
		writer.writeInt(buffer, 0);
		writer.writeByte(buffer, messageHead.getType());
		// head
		writer.writeInt(buffer, head.getId());
		writer.writeInt(buffer, head.getReplyStatus());
		// body
		if (body != null) {
			// write out parameters
			int size = body.getOutArguments().size();
			for (int i = 0; i < size; i++) {
				writer.writeObject(buffer, body.getOutArguments().get(i), body.getOutParameterTypes().get(i));
			}
			if (hasResponse && head.getReplyStatus() != ResponseHead.RESULT_IS_NULL) {
				// write result
				if (body.getResultType().isAssignableFrom(Set.class)) {
					writer.writeSet(buffer, (Set<?>) body.getResult(), (ParameterizedType) body.getGenericType());
				} else if (body.getResultType().isAssignableFrom(List.class)) {
					writer.writeList(buffer, (List<?>) body.getResult(), (ParameterizedType) body.getGenericType());
				} else if (body.getResultType().isAssignableFrom(Map.class)) {
					writer.writeMap(buffer, (Map<?, ?>) body.getResult(), (ParameterizedType) body.getGenericType());
				} else {
					writer.writeObject(buffer, body.getResult(), body.getResultType());
				}
			}
		}
		// return to set the message size in message head
		int p = buffer.position();
		buffer.position(0);
		writer.writeInt(buffer, p - MessageHead.MESSAGE_HEAD_SIZE);
		buffer.position(p);
		buffer.flip();
		messageHead.setSize(buffer.limit());
	}

	public ResponseHead getHead() {
		return head;
	}

	public void setHead(ResponseHead head) {
		this.head = head;
	}

	public ResponseBody getBody() {
		return body;
	}

	public void setBody(ResponseBody body) {
		this.body = body;
	}

}
