package com.tnsoft.icm.icm4j.protocol;

public class ResponseHead {

	public static final int OK = 0;
	public static final int UNKNOWN_ERROR = -1;
	public static final int RESULT_IS_NULL = -200;

	private int id;
	private int replyStatus;

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public int getReplyStatus() {
		return replyStatus;
	}

	public void setReplyStatus(int replyStatus) {
		this.replyStatus = replyStatus;
	}

}
