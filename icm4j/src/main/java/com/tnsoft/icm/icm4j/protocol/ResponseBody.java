package com.tnsoft.icm.icm4j.protocol;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;

public class ResponseBody {

	private List<Class<?>> outParameterTypes = new ArrayList<Class<?>>();
	private List<Object> outArguments = new ArrayList<Object>();

	private Class<?> resultType;
	private Object result;
	private Type genericType;

	public List<Class<?>> getOutParameterTypes() {
		return outParameterTypes;
	}

	public List<Object> getOutArguments() {
		return outArguments;
	}

	public Class<?> getResultType() {
		return resultType;
	}

	public void setResultType(Class<?> resultType) {
		this.resultType = resultType;
	}

	public Object getResult() {
		return result;
	}

	public void setResult(Object result) {
		this.result = result;
	}

	public Type getGenericType() {
		return genericType;
	}

	public void setGenericType(Type genericType) {
		this.genericType = genericType;
	}

}
