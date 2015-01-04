package com.tnsoft.icm.icm4j;

import java.lang.reflect.Method;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.tnsoft.icm.icm4j.protocol.annotation.Async;
import com.tnsoft.icm.icm4j.protocol.annotation.Name;
import com.tnsoft.icm.icm4j.protocol.annotation.Operation;
import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public class ServiceDispatcher {

	private static final Log log = LogFactory.getFormatterLog(ServiceDispatcher.class);

	private static final String MARK = "#";

	private Map<String, OperatingBean> operationMap = new ConcurrentHashMap<String, OperatingBean>();

	public String generateId(String name, String operation) {
		return name + MARK + operation;
	}

	public void addService(Class<?> serviceInterface, Object serviceInstance) {
		Name an = serviceInterface.getAnnotation(Name.class);
		String name;
		if (an != null) {
			name = an.value().length() > 0 ? an.value() : serviceInterface.getSimpleName();
		} else {
			name = serviceInterface.getName();
		}
		Method[] methods = serviceInterface.getMethods();
		for (Method method : methods) {
			Operation ao = method.getAnnotation(Operation.class);
			String methodName = method.getName();
			try {
				Method implMethod = serviceInstance.getClass().getMethod(methodName, method.getParameterTypes());
				String operation = ao != null && ao.value().length() > 0 ? ao.value() : methodName;
				OperatingBean ob = new OperatingBean(serviceInstance, method);
				Async aa = implMethod.getAnnotation(Async.class);
				if (aa != null) {
					ob.setAsync(aa.value());
				}
				operationMap.put(generateId(name, operation), ob);
			} catch (SecurityException e) {
				log.error(e);
				throw new OperatingException(e);
			} catch (NoSuchMethodException e) {
				log.error(e);
				throw new OperatingException(e);
			}
		}
	}

	public OperatingBean getOperatingBean(String name, String operation) {
		return operationMap.get(generateId(name, operation));
	}

}
