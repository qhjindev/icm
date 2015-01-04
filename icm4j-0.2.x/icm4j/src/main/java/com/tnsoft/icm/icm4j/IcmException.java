package com.tnsoft.icm.icm4j;

public class IcmException extends RuntimeException {

	private static final long serialVersionUID = -1281979231307814475L;

	public IcmException() {
		super();
	}
	
	public IcmException(String message) {
		super(message);
	}
	
	public IcmException(Throwable cause) {
		super(cause);
	}

	public IcmException(String message, Throwable cause) {
		super(message, cause);
	}

}
