package com.tnsoft.icm.icm4j;

import java.lang.reflect.Proxy;

import com.tnsoft.icm.util.BufferReader;
import com.tnsoft.icm.util.BufferWriter;
import com.tnsoft.icm.util.PoolProvider;
import com.tnsoft.icm.util.PoolProviderFactory;
import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

@Deprecated
public class ServiceFactory {

	private static final Log log = LogFactory.getFormatterLog(ServiceFactory.class);

	private static final ServiceFactory instance = newInstance();

	public static ServiceFactory getInstance() {
		return instance;
	}
	
	public static ServiceFactory newInstance() {
		return new ServiceFactory();
	}
	
	public static ServiceFactory newInstance(BufferReader reader, BufferWriter writer) {
		return new ServiceFactory(reader, writer);
	}

	private TCPServiceHander tcpServiceHander;
	private PoolProvider poolProvider;

	private ServiceFactory() {
		tcpServiceHander = new TCPServiceHander();
		setPoolProvider(PoolProviderFactory.newSimpleChannelPoolProvider());
	}
	
	private ServiceFactory(BufferReader reader, BufferWriter writer) {
		this();
		tcpServiceHander.setReader(reader);
		tcpServiceHander.setWriter(writer);
	}

	@SuppressWarnings("unchecked")
	public <T> T getService(Class<T> clazz, String ip, int port) {
		T service = (T) Proxy.newProxyInstance(clazz.getClassLoader(), new Class<?>[] { clazz }, tcpServiceHander);
		tcpServiceHander.putAddress(service, clazz, ip, port);
		log.debug("Create a new Proxy for %s at remote server [%s:%d].", clazz.getName(), ip, port);
		return service;
	}

	public void setPoolProvider(PoolProvider poolProvider) {
		this.poolProvider = poolProvider;
		tcpServiceHander.setPoolProvider(poolProvider);
	}

	public synchronized void stop() {
		poolProvider.stopAllPools();
		poolProvider = null;
	}

}
