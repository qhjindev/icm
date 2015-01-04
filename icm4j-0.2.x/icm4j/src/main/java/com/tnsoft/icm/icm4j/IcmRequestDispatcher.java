package com.tnsoft.icm.icm4j;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.tnsoft.icm.icm4j.protocol.RequestHead;

public class IcmRequestDispatcher {

	private final Map<String, IcmOperation> operationMap = new ConcurrentHashMap<String, IcmOperation>();

	private String key(String name, String category, String operation) {
		return name + "_" + category + "#" + operation;
	}

	private String key(RequestHead requestHead) {
		return key(requestHead.getName(), requestHead.getCategory(), requestHead.getOperation());
	}

	public IcmOperation dispatch(RequestHead requestHead) {
		String key = key(requestHead);
		return operationMap.get(key);
	}

	public void addOperation(String name, String category, String operation, IcmOperation icmOperation) {
		operationMap.put(key(name, category, operation), icmOperation);
	}
}
