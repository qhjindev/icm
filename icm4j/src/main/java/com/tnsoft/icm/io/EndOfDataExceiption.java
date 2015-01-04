package com.tnsoft.icm.io;

public class EndOfDataExceiption extends RuntimeException {

	private static final long serialVersionUID = 6953368540451976032L;

	public EndOfDataExceiption() {
		super();
	}
	
	public EndOfDataExceiption(String message) {
		super(message);
	}
	
	public EndOfDataExceiption(Throwable cause) {
		super(cause);
	}
	
	public EndOfDataExceiption(String message, Throwable cause) {
		super(message, cause);
	}
}
