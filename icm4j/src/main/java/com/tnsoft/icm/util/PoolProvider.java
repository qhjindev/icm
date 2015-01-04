package com.tnsoft.icm.util;


public interface PoolProvider {

	ChannelPool getPool(String hostname, int port);
	
	void stopAllPools();

}
