package com.tnsoft.icm.icm4j.msg.protocol;

import com.tnsoft.icm.icm4j.protocol.IcmProtocol;

public interface MsgProtocol extends IcmProtocol {

	public static final byte TYPE_SUBSCRIBE = 0;
	public static final byte TYPE_UNSUBSCRIBE = 1;
	public static final byte TYPE_PUBLISH = 2;
	public static final byte TYPE_UNPUBLISH = 3;
	public static final byte TYPE_EVENT = 4;
	public static final byte TYPE_UNKNOW = 5;
	public static final byte TYPE_OK = 6;
}
