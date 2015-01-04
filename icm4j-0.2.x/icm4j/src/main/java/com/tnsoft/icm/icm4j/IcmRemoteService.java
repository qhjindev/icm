package com.tnsoft.icm.icm4j;


public class IcmRemoteService extends IcmService implements ConnectionMode {

	private static final long DEFAULT_TIMEOUT = 60 * 60 * 1000L;

	private final String hostname;
	private int connectionMode = MODE_SHORT_CONNECTION;
	private int connectionCount = 0;
	private IcmRemoteProxy remoteProxy;
	private long timeout = DEFAULT_TIMEOUT;

	public IcmRemoteService(String hostname, int port) {
		this(hostname, port, TransferProtocol.TCP);
	}

	public IcmRemoteService(String hostname, int port, TransferProtocol protocol) {
		super(port, protocol);
		this.hostname = hostname;
	}

	public String getHostname() {
		return hostname;
	}

	public int getConnectionMode() {
		return connectionMode;
	}

	public void setConnectionMode(int connectionMode) {
		this.connectionMode = connectionMode;
	}

	public boolean isLongConnectionMode() {
		return (connectionMode & MODE_LONG_CONNECTION) != 0;
	}
	
	public int getConnectionCount() {
		return connectionCount;
	}
	
	public void setConnectionCount(int connectionCount) {
		this.connectionCount = connectionCount;
	}

	public synchronized void setRemoteProxy(IcmRemoteProxy remoteProxy) {
		if (remoteProxy == null)
			throw new NullPointerException();
		this.remoteProxy = remoteProxy;
		if (!remoteProxy.contains(this)) {
			remoteProxy.addRemoteService(this);
		}
	}

	public IcmRemoteProxy getRemoteProxy() {
		return remoteProxy;
	}

	public synchronized <T> T getService(Class<T> interfaceClass) {
		if (remoteProxy != null) {
			return remoteProxy.getService(this, interfaceClass);
		}
		return null;
	}

	public long getTimeout() {
		return timeout;
	}

	public void setTimeout(long timeout) {
		if (timeout < 1) {
			timeout = DEFAULT_TIMEOUT;
		}
		this.timeout = timeout;
	}

}
