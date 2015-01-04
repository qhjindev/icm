package com.tnsoft.icm.icm4j.protocol;

public class RequestHead {

	public static final byte NO_RESPONSE = 0;
	public static final byte RESPONSE = 1;

	private int id;
	private byte responseFlag;
	private String name;
	private String category;
	private String operation;

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public byte getResponseFlag() {
		return responseFlag;
	}

	public void setResponseFlag(byte responseFlag) {
		this.responseFlag = responseFlag;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getCategory() {
		return category;
	}

	public void setCategory(String category) {
		this.category = category;
	}

	public String getOperation() {
		return operation;
	}

	public void setOperation(String operation) {
		this.operation = operation;
	}

}
