package com.tnsoft.icm.msg4j;

import java.lang.reflect.ParameterizedType;

public abstract class TvmEventAction<T> {
	
	@SuppressWarnings("unchecked")
	public TvmEventAction() {
		clazz = (Class<T>) ((ParameterizedType)getClass().getGenericSuperclass()).getActualTypeArguments()[0];
	}
	
	private Class<T> clazz;
	
	public Class<T> getTypeClass() {
		return clazz;
	}

	public abstract void execute(String topicId, T evt);

}
