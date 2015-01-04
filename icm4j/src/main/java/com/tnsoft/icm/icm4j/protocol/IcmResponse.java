package com.tnsoft.icm.icm4j.protocol;

import java.lang.reflect.Type;
import java.util.List;

public class IcmResponse extends IcmMessage {

	private int replyStatus;

	private List<Object> outArguments;
	private List<Type> outArgumentTypes;
	private Object result;
	private Type genericReturnType;

	public int getReplyStatus() {
		return replyStatus;
	}

	public void setReplyStatus(int replyStatus) {
		this.replyStatus = replyStatus;
	}

	public List<Object> getOutArguments() {
		return outArguments;
	}

	public void setOutArguments(List<Object> outArguments) {
		this.outArguments = outArguments;
	}

	public Object getResult() {
		return result;
	}

	public void setResult(Object result) {
		this.result = result;
	}

	public List<Type> getOutArgumentTypes() {
		return outArgumentTypes;
	}

	public void setOutArgumentTypes(List<Type> outArgumentTypes) {
		this.outArgumentTypes = outArgumentTypes;
	}

	public Type getGenericReturnType() {
		return genericReturnType;
	}

	public void setGenericReturnType(Type genericReturnType) {
		this.genericReturnType = genericReturnType;
	}

}
