package com.tnsoft.icm.icm4j;

public class OperatingException extends RuntimeException {

	private static final long serialVersionUID = -4647877955395076536L;

	public OperatingException() {
		super();
	}

	public OperatingException(String message) {
		super(message);
	}

	public OperatingException(Throwable e) {
		super(e);
	}

	public OperatingException(String message, Throwable e) {
		super(message, e);
	}
}
