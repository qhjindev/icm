package com.tnsoft.icm.io;

public class WriteFlipedBufferException extends RuntimeException {

	private static final long serialVersionUID = -1407472679837719291L;

	public WriteFlipedBufferException() {}

	public WriteFlipedBufferException(String message) {
		super(message);
	}

	public WriteFlipedBufferException(Throwable cause) {
		super(cause);
	}

	public WriteFlipedBufferException(String message, Throwable cause) {
		super(message, cause);
	}
}
