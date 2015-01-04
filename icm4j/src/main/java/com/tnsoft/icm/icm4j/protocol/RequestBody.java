package com.tnsoft.icm.icm4j.protocol;

import java.lang.reflect.Type;

public class RequestBody {

	private Class<?>[] types;
	private Object[] arguments;
	private Type[] genericTypes;

	public Class<?>[] getTypes() {
		return types;
	}

	public void setTypes(Class<?>[] types) {
		this.types = types;
	}

	public Object[] getArguments() {
		return arguments;
	}

	public void setArguments(Object[] arguments) {
		this.arguments = arguments;
	}

	public Type[] getGenericTypes() {
		return genericTypes;
	}

	public void setGenericTypes(Type[] genericTypes) {
		this.genericTypes = genericTypes;
	}

}
