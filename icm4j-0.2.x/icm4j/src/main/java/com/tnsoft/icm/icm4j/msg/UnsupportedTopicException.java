package com.tnsoft.icm.icm4j.msg;

public class UnsupportedTopicException extends Exception {

	private static final long serialVersionUID = -7166479288080624356L;

	public UnsupportedTopicException() {
		super();
	}
	
	public UnsupportedTopicException(String message) {
		super(message);
	}
	
	public UnsupportedTopicException(Throwable cause) {
		super(cause);
	}
	
	public UnsupportedTopicException(String message, Throwable cause) {
		super(message, cause);
	}

	
}
