package com.tnsoft.icm.icm4j;

import java.lang.reflect.GenericArrayType;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.icm4j.protocol.IcmProtocol;
import com.tnsoft.icm.icm4j.protocol.IcmRequest;
import com.tnsoft.icm.icm4j.protocol.IcmResponse;
import com.tnsoft.icm.icm4j.protocol.annotation.Name;
import com.tnsoft.icm.icm4j.protocol.annotation.Operation;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.io.BufferReader;
import com.tnsoft.icm.io.BufferWriter;
import com.tnsoft.icm.io.DataBuffer;

public abstract class IcmClient implements IcmProtocol {

	protected final Log log = LogFactory.getLog(getClass());

	private static final Map<Method, IcmRequest> request_templates = new ConcurrentHashMap<Method, IcmRequest>();

	private static IcmRequest getTemplatedRequest(Method method) {
		// template
		IcmRequest t_req = request_templates.get(method);
		if (t_req == null) {
			t_req = new IcmRequest(false);
			Class<?> clazz = method.getDeclaringClass();
			if (method.getReturnType() != void.class) {
				t_req.setResponseFlag(HAS_RESPONSE);
			} else {
				Class<?>[] types = method.getParameterTypes();
				boolean has_response = false;
				for (Class<?> type : types) {
					if (type.isAssignableFrom(Out.class)) {
						has_response = true;;
						break;
					}
				}
				t_req.setResponseFlag(has_response ? HAS_RESPONSE : NO_RESPONSE);
			}
			Name name = clazz.getAnnotation(Name.class);
			if (name == null) {
				t_req.setName(clazz.getName());
			} else {
				t_req.setName("".equals(name.value()) ? clazz.getSimpleName() : name.value());
			}
			t_req.setCategory("");
			Operation operation = method.getAnnotation(Operation.class);
			if (operation == null || "".equals(operation.value())) {
				t_req.setOperation(method.getName());
			} else {
				t_req.setOperation(operation.value());
			}
			request_templates.put(method, t_req);
		}
		// request
		IcmRequest req = new IcmRequest();
		req.setParameterTypes(method.getParameterTypes());
		req.setGenericParameterTypes(method.getGenericParameterTypes());
		req.setResponseFlag(t_req.getResponseFlag());
		req.setName(t_req.getName());
		req.setCategory(t_req.getCategory());
		req.setOperation(t_req.getOperation());
		return req;
	}

	protected ReaderWriterProvider readerWriterProvider;

	public void setReaderWriterProvider(ReaderWriterProvider readerWriterProvider) {
		this.readerWriterProvider = readerWriterProvider;
	}

	protected IcmRequest generateRequest(Method method, Object[] args) {
		IcmRequest req = getTemplatedRequest(method);
		req.setArguments(args);
		return req;
	}

	@SuppressWarnings("rawtypes")
	protected void writeRequest(DataBuffer buffer, IcmRequest req) {
		BufferWriter writer = readerWriterProvider.getWriter(buffer);
		// message head
		writer.writeInt(0);
		writer.writeByte(TYPE_REQUEST);
		// request head
		writer.writeInt(req.getId());
		writer.writeByte(req.getResponseFlag());
		writer.writeString(req.getName());
		writer.writeString(req.getCategory());
		writer.writeString(req.getOperation());
		// request args
		if (req.getArguments() != null) {
			Object[] args = req.getArguments();
			for (int i = 0; i < args.length; i++) {
				Object arg = args[i];
				if (arg instanceof Out<?>) {
					continue;
				}
				if (arg instanceof Collection<?>) {
					Class type = (Class) ((ParameterizedType)req.getGenericParameterTypes()[i]).getActualTypeArguments()[0];
					writer.writeCollection((Collection<?>) arg, type);
				} else if (arg instanceof Map<?,?>) {
					writer.writeMap((Map<?, ?>) arg);
				} else {
					writer.writeObject(arg);
				}
			}
		}
		// replace size
		int p = buffer.position();
		buffer.position(0);
		writer.writeInt(p - MESSAGE_HEAD_SIZE);
		buffer.position(p);
		writer.close();
	}

	@SuppressWarnings({ "unchecked", "rawtypes" })
	protected int readResponse(DataBuffer buffer, IcmResponse resp, Method method, Object[] args) {
		BufferReader reader = readerWriterProvider.getReader(buffer);
		resp.setId(reader.readInt());
		resp.setReplyStatus(reader.readInt());
		switch (resp.getReplyStatus()) {
		case RESPONSE_OK:
			Class<?>[] types = method.getParameterTypes();
			Type[] gts = method.getGenericParameterTypes();
			for (int i = 0; i < types.length; i++) {
				if (types[i].isAssignableFrom(Out.class)) {
					Object out = null;
					Type type = ((ParameterizedType) gts[i]).getActualTypeArguments()[0];
					if (type instanceof GenericArrayType) {
						out = reader.readArray((Class<?>) ((GenericArrayType) type).getGenericComponentType());
					} else if (type instanceof ParameterizedType) {
						String typeString = type.toString();
						if (typeString.startsWith("java.util.List")) {
							out = reader.readList((ParameterizedType) type);
						} else if (typeString.startsWith("java.util.Map")) {
							out = reader.readMap((ParameterizedType) type);
						} else if (typeString.startsWith("java.util.Set")) {
							out = reader.readSet((ParameterizedType) type);
						}
					} else {
						out = reader.readObject((Class<?>) type);
					}
					((Out) args[i]).set(out);
				}
			}
			Class<?> rt = method.getReturnType();
			if (rt != void.class) {
				if (log.isDebugEnabled()) {
					log.debug("return type: " + rt);
				}
				if (rt.isAssignableFrom(List.class)) {
					resp.setResult(reader.readList((ParameterizedType) method.getGenericReturnType()));
				} else if (rt.isAssignableFrom(Set.class)) {
					resp.setResult(reader.readSet((ParameterizedType) method.getGenericReturnType()));
				} else if (rt.isAssignableFrom(Map.class)) {
					resp.setResult(reader.readMap((ParameterizedType) method.getGenericReturnType()));
				} else {
					resp.setResult(reader.readObject(rt));
				}
			}
			break;
		default:
		case RESPONSE_UNKNOWN_ERROR:
			break;
		}
		reader.close();
		return resp.getReplyStatus();
	}

}
