package com.tnsoft.icm.icm4j;

public class ICMConnectionException extends RuntimeException {

	private static final long serialVersionUID = 7253025035584623111L;

	public ICMConnectionException() {
		super();
	}

	public ICMConnectionException(String message) {
		super(message);
	}

	public ICMConnectionException(Throwable e) {
		super(e);
	}

	public ICMConnectionException(String message, Throwable e) {
		super(message, e);
	}
}
