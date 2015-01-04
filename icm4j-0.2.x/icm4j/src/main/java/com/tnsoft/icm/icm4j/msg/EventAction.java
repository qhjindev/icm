package com.tnsoft.icm.icm4j.msg;

import java.lang.reflect.ParameterizedType;

import com.tnsoft.icm.icm4j.annotation.Name;
import com.tnsoft.icm.icm4j.msg.protocol.Topic;

public abstract class EventAction<T> {
	
	private Class<?> clazz;
	private Class<?> eventType;
	private String name;

	public EventAction() {
		clazz = getClass();
		eventType = (Class<?>)((ParameterizedType) clazz.getGenericSuperclass()).getActualTypeArguments()[0];
		Name aname = clazz.getAnnotation(Name.class);
		if (aname != null) {
			name = aname.value();
		} else {
			name = getEventType().getSimpleName();
		}
	}

	public String getName() {
		return name;
	}

	public Class<?> getEventType() {
		return eventType;
	}

	public abstract void execute(Topic topic, T event);
}
