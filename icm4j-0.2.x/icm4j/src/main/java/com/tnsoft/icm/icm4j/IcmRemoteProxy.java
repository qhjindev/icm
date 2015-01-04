package com.tnsoft.icm.icm4j;

public interface IcmRemoteProxy {

	void addRemoteService(IcmRemoteService remoteService);
	
	boolean contains(IcmRemoteService remoteService);
	
	boolean isRunning();
	
	<T> T getService(IcmRemoteService remoteService, Class<T> interfaceClass);
}
