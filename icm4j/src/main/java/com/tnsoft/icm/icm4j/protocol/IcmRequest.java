package com.tnsoft.icm.icm4j.protocol;

import java.lang.reflect.Type;

@SuppressWarnings("rawtypes")
public final class IcmRequest extends IcmMessage {

	private byte responseFlag;
	private String name;
	private String category;
	private String operation;
	private Object[] arguments;
	private Type[] genericParameterTypes;
	private Class[] parameterTypes;

	public Type[] getGenericParameterTypes() {
		return genericParameterTypes;
	}

	public void setGenericParameterTypes(Type[] genericParameterTypes) {
		this.genericParameterTypes = genericParameterTypes;
	}

	public Class[] getParameterTypes() {
		return parameterTypes;
	}

	public void setParameterTypes(Class[] parameterTypes) {
		this.parameterTypes = parameterTypes;
	}

	public IcmRequest() {
		this(true);
	}

	public IcmRequest(boolean generateId) {
		if (generateId) {
			setId(generateId());
		}
	}

	public byte getResponseFlag() {
		return responseFlag;
	}

	public void setResponseFlag(byte responseFlag) {
		this.responseFlag = responseFlag;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getCategory() {
		return category;
	}

	public void setCategory(String category) {
		this.category = category;
	}

	public String getOperation() {
		return operation;
	}

	public void setOperation(String operation) {
		this.operation = operation;
	}

	public Object[] getArguments() {
		return arguments;
	}

	public void setArguments(Object[] arguments) {
		this.arguments = arguments;
	}

}
