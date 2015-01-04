package com.tnsoft.icm.msg4j;

import java.util.HashMap;
import java.util.Map;

public class TvmEventActionMap {

	private Map<String, TvmEventAction<?>> eventMap = new HashMap<String, TvmEventAction<?>>();

	public void put(TvmEventAction<?> action) {
		eventMap.put(action.getTypeClass().getSimpleName(), action);
	}

	public TvmEventAction<?> get(String type) {
		return eventMap.get(type);
	}
	
	public TvmEventAction<?> remove(String type) {
		return eventMap.remove(type);
	}
	
	public void clear() {
		eventMap.clear();
	}

	public Map<String, TvmEventAction<?>> getEventMap() {
		return eventMap;
	}

	public void setEventMap(Map<String, TvmEventAction<?>> eventMap) {
		this.eventMap = eventMap;
	}

}
