package com.tnsoft.icm.icm4j;

import java.lang.annotation.Annotation;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Type;

import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public class OperatingBean {

	private static final Log log = LogFactory.getFormatterLog(OperatingBean.class);

	private Object serviceInstance;
	private Method operationMethod;
	private boolean async;

	public OperatingBean(Object serviceInstance, Method operationMethod) {
		this.serviceInstance = serviceInstance;
		this.operationMethod = operationMethod;
	}

	public Object getServiceInstance() {
		return serviceInstance;
	}

	public Method getOperationMethod() {
		return operationMethod;
	}

	public Object doOperation(Object[] args) {
		try {
			return this.operationMethod.invoke(serviceInstance, args);
		} catch (IllegalArgumentException e) {
			log.error(e);
			throw new OperatingException(e);
		} catch (IllegalAccessException e) {
			log.error(e);
			throw new OperatingException(e);
		} catch (InvocationTargetException e) {
			log.error(e);
			throw new OperatingException(e);
		}
	}

	public Class<?>[] getParameterTypes() {
		return operationMethod.getParameterTypes();
	}

	public Type[] getGenericParameterTypes() {
		return operationMethod.getGenericParameterTypes();
	}

	public <T extends Annotation> T getAnnotation(Class<T> annotationClass) {
		return operationMethod.getAnnotation(annotationClass);
	}

	public Class<?> getReturnType() {
		return operationMethod.getReturnType();
	}
	
	public Type getGenericReturnType() {
		return operationMethod.getGenericReturnType();
	}

	public boolean hasResult() {
		return getReturnType() != void.class;
	}

	public boolean isAsync() {
		return async;
	}

	public void setAsync(boolean async) {
		this.async = async;
	}

}
