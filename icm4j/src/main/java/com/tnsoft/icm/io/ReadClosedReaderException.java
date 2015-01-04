package com.tnsoft.icm.io;

public class ReadClosedReaderException extends RuntimeException {

	private static final long serialVersionUID = -6288690942830240781L;

	public ReadClosedReaderException() {
		super();
	}
	
	public ReadClosedReaderException(String message) {
		super(message);
	}
	
	public ReadClosedReaderException(Throwable cause) {
		super(cause);
	}
	
	public ReadClosedReaderException(String message, Throwable cause) {
		super(message, cause);
	}
}
