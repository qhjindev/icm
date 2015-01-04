package com.tnsoft.icm.icm4j.io;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.icm4j.annotation.Ignore;

public abstract class BufferIO {

	private static final Log log = LogFactory.getLog(BufferIO.class);

	private static final ConcurrentMap<Class<?>, PojoInfo> pojoInfoMap = new ConcurrentHashMap<Class<?>, PojoInfo>();

	protected static PojoInfo getPojoInfo(Class<?> clazz) {
		if (log.isDebugEnabled()) {
			log.debug("get pojo for class: " + clazz);
		}
		PojoInfo info = pojoInfoMap.get(clazz);
		if (info == null) {
			if (log.isDebugEnabled()) {
				log.debug("init pojo for class: " + clazz);
			}
			info = new PojoInfo(clazz);
			List<Field> fields = new ArrayList<Field>();
			fields(clazz, fields);
			info.setProperties(fields.toArray(new Field[fields.size()]));
			pojoInfoMap.putIfAbsent(clazz, info);
		}
		return info;
	}

	private static void fields(Class<?> clazz, List<Field> fields) {
		Class<?> superClass = clazz.getSuperclass();
		if (superClass != Object.class) {
			fields(superClass, fields);
		}
		Field[] fs = clazz.getDeclaredFields();
		for (Field field : fs) {
			if (Modifier.isStatic(field.getModifiers())) {
				if (log.isDebugEnabled()) {
					log.debug("ignore [static] field: " + field);
				}
				continue;
			}
			if (Modifier.isFinal(field.getModifiers())) {
				if (log.isDebugEnabled()) {
					log.debug("ignore [final] field: " + field);
				}
				continue;
			}
			if (field.isAnnotationPresent(Ignore.class)) {
				if (log.isDebugEnabled()) {
					log.debug("ignore [@Ignore] field: " + field);
				}
				continue;
			}
			field.setAccessible(true);
			fields.add(field);
		}
	}

	protected static class PojoInfo {
		private Class<?> type;
		private Field[] properties;

		public PojoInfo(Class<?> type) {
			setType(type);
		}

		public Class<?> getType() {
			return type;
		}

		public void setType(Class<?> type) {
			this.type = type;
		}

		public Field[] getProperties() {
			return properties;
		}

		public void setProperties(Field[] properties) {
			this.properties = properties;
		}

	}

	protected Buffer buffer;
	protected final Charset charset;

	public BufferIO(Buffer buffer, Charset charset) {
		this.buffer = buffer;
		this.charset = charset;
	}

	public abstract void close();

	public abstract boolean isClosed();
}
