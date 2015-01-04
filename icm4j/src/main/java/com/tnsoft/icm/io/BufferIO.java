package com.tnsoft.icm.io;

import java.nio.charset.Charset;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.tnsoft.icm.util.StructBeanInfo;

public abstract class BufferIO {

	private static final Map<Class<?>, StructBeanInfo> structMap = new ConcurrentHashMap<Class<?>, StructBeanInfo>();
	private static final Object map_lock = new Object();

	protected static StructBeanInfo getStructBeanInfo(Class<?> type) {
		StructBeanInfo sbi = structMap.get(type);
		if (sbi == null) {
			synchronized (map_lock) {
				sbi = structMap.get(type);
				if (sbi == null) {
					sbi = new StructBeanInfo(type);
					structMap.put(type, sbi);
				}
			}
		}
		return sbi;
	}

	protected static final Charset DEFAULT_CHARSET = Charset.forName("UTF-8");

	protected Log log = LogFactory.getLog(getClass());
	protected DataBuffer dataBuffer;
	protected Charset charset;
	protected boolean closed = false;
	
	public Charset getCharset() {
		return charset;
	}
	
	public boolean isClosed() {
		return closed;
	}
}
