package com.tnsoft.icm.icm4j.msg;

import com.tnsoft.icm.icm4j.IcmException;

public class MsgException extends IcmException {

	private static final long serialVersionUID = 5842597128608339655L;

	public MsgException() {
		super();
	}
	
	public MsgException(String message) {
		super(message);
	}
	
	public MsgException(Throwable cause) {
		super(cause);
	}

	public MsgException(String message, Throwable cause) {
		super(message, cause);
	}

}
