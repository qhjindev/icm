package com.tnsoft.icm.io;

public class WriteClosedWriterException extends RuntimeException {

	private static final long serialVersionUID = -5835506125868554160L;

	public WriteClosedWriterException() {
		super();
	}
	
	public WriteClosedWriterException(String message) {
		super(message);
	}
	
	public WriteClosedWriterException(Throwable cause) {
		super(cause);
	}
	
	public WriteClosedWriterException(String message, Throwable cause) {
		super(message, cause);
	}
}
