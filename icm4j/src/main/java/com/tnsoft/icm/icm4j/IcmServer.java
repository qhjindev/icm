package com.tnsoft.icm.icm4j;

import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.icm4j.protocol.IcmProtocol;
import com.tnsoft.icm.icm4j.protocol.IcmRequest;
import com.tnsoft.icm.icm4j.protocol.IcmResponse;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.io.BufferReader;
import com.tnsoft.icm.io.BufferWriter;
import com.tnsoft.icm.io.DataBuffer;

public abstract class IcmServer implements IcmProtocol {

	protected final Log log = LogFactory.getLog(getClass());

	private ServiceDispatcher serviceDispatcher;
	protected ReaderWriterProvider readerWriterProvider;
	
	public void setReaderWriterProvider(ReaderWriterProvider readerWriterProvider) {
		this.readerWriterProvider = readerWriterProvider;
	}

	public synchronized ServiceDispatcher getServiceDispatcher() {
		if (serviceDispatcher == null) {
			serviceDispatcher = new ServiceDispatcher();
		}
		return serviceDispatcher;
	}

	public synchronized void setServiceDispatcher(ServiceDispatcher serviceDispatcher) {
		this.serviceDispatcher = serviceDispatcher;
	}

	@SuppressWarnings("rawtypes")
	protected OperatingBean readRequest(DataBuffer buffer, IcmRequest req) {
		BufferReader reader = readerWriterProvider.getReader(buffer);
		// request head
		req.setId(reader.readInt());
		req.setResponseFlag(reader.readByte());
		req.setName(reader.readString());
		req.setCategory(reader.readString());
		req.setOperation(reader.readString());
		// request body    	
		OperatingBean ob = serviceDispatcher.getOperatingBean(req.getName(), req.getOperation());
		if (ob == null) {
			throw new RuntimeException("Cannot find the service with name=\""+req.getName()+"\" & operation=\""+req.getOperation()+"\".");
		}
		Class<?>[] types = ob.getParameterTypes();
		Type[] gts = ob.getGenericParameterTypes();
		Object[] args = new Object[types.length];
		for (int i = 0; i < args.length; i++) {
			if (types[i].isAssignableFrom(Out.class)) {
				args[i] = new Out();
			} else if (types[i].isAssignableFrom(Set.class)) {
				args[i] = reader.readSet((ParameterizedType) gts[i]);
			} else if (types[i].isAssignableFrom(List.class)) {
				args[i] = reader.readList((ParameterizedType) gts[i]);
			} else if (types[i].isAssignableFrom(Map.class)) {
				args[i] = reader.readMap((ParameterizedType) gts[i]);
			} else {
				args[i] = reader.readObject(types[i]);
			}
		}
		req.setArguments(args);
		reader.close();
		return ob;
	}

	protected IcmResponse process(OperatingBean ob, IcmRequest req) {
		int error = 0;
		Object result = null;
		try {
			result = ob.doOperation(req.getArguments());
		} catch (Exception e) {
			error++;
			if (log.isErrorEnabled()) {
				log.error(e.getMessage(), e);
			}
		}
		if (req.getResponseFlag() == HAS_RESPONSE) {
			IcmResponse resp = new IcmResponse();
			resp.setId(req.getId());
			if (error > 0) {
				resp.setReplyStatus(RESPONSE_UNKNOWN_ERROR);
			} else {
				resp.setReplyStatus(RESPONSE_OK);
				Type[] gpts = ob.getGenericParameterTypes();
				List<Type> opts = new ArrayList<Type>();
				Class<?>[] types = ob.getParameterTypes();
				List<Object> outs = new LinkedList<Object>();
				for (int i = 0; i < types.length; i++) {
					if (types[i].isAssignableFrom(Out.class)) {
						outs.add(((Out<?>) req.getArguments()[i]).get());
						opts.add(gpts[i]);
					}
				}
				resp.setOutArguments(outs);
				resp.setResult(result);
				resp.setGenericReturnType(ob.getGenericReturnType());
			}
			return resp;
		}
		return null;
	}

	@SuppressWarnings("rawtypes")
	protected void writeResponse(DataBuffer buffer, IcmResponse resp) {
		BufferWriter writer = readerWriterProvider.getWriter(buffer);
		// head
		writer.writeInt(0);
		writer.writeByte(TYPE_RESPONSE);
		// response
		writer.writeInt(resp.getId());
		writer.writeInt(resp.getReplyStatus());
		int i = 0;
		for (Object out : resp.getOutArguments()) {
			if (out instanceof Collection) {
				Class ot = (Class) ((ParameterizedType) ((ParameterizedType)resp.getOutArgumentTypes().get(i)).getActualTypeArguments()[0]).getActualTypeArguments()[0];
				writer.writeCollection((Collection<?>) out, ot);
			} else if (out instanceof Map) {
				writer.writeMap((Map<?, ?>) out);
			} else {
				writer.writeObject(out);
			}
			i++;
		}
		if (resp.getResult() != null) {
			Object result = resp.getResult();
			if (result instanceof Collection<?>) {
				Class type = (Class) ((ParameterizedType)resp.getGenericReturnType()).getActualTypeArguments()[0];
				writer.writeCollection((Collection<?>) result, type);
			} else if(result instanceof Map<?, ?>) {
				writer.writeMap((Map<?, ?>) result);
			} else {
				writer.writeObject(resp.getResult());
			}
		}
		int p = buffer.position();
		buffer.position(0);
		writer.writeInt(p - MESSAGE_HEAD_SIZE);
		buffer.position(p);
		writer.close();
	}
}
