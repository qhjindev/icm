package com.tnsoft.icm.icm4j;

public class IcmRemoteException extends IcmException {

	private static final long serialVersionUID = -3193987809401331080L;

	public IcmRemoteException() {
		super();
	}

	public IcmRemoteException(String message) {
		super(message);
	}

	public IcmRemoteException(Throwable cause) {
		super(cause);
	}

	public IcmRemoteException(String message, Throwable cause) {
		super(message, cause);
	}
}
