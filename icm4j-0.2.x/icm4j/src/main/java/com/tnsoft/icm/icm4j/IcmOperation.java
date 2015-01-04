package com.tnsoft.icm.icm4j;

import java.lang.reflect.Method;

public class IcmOperation {

	private final Method method;
	private final Object obj;
	private final boolean async;

	public IcmOperation(Method method, Object obj, boolean async) {
		this.method = method;
		this.obj = obj;
		this.async = async;
	}

	public Method getMethod() {
		return method;
	}

	public Object getObj() {
		return obj;
	}

	public boolean isAsync() {
		return async;
	}

	@Override
	public String toString() {
		StringBuilder builder = new StringBuilder().append(IcmOperation.class.toString()).append(" ");
		builder.append(obj.getClass().getName()).append('#').append(method.getName()).append('(');
		Class<?>[] types = method.getParameterTypes();
		for (int i = 0; i < types.length; i++) {
			builder.append(types[i].getName());
			if (i < types.length - 1) {
				builder.append(", ");
			}
		}
		builder.append(").");
		return builder.toString();
	}

}
