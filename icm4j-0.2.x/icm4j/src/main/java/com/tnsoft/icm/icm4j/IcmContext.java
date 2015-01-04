package com.tnsoft.icm.icm4j;

import java.net.SocketAddress;
import java.nio.channels.SelectableChannel;
import java.util.HashMap;

import com.tnsoft.icm.icm4j.io.Buffer;
import com.tnsoft.icm.icm4j.protocol.ResponseHead;

public class IcmContext extends HashMap<String, Object> {

	private static final long serialVersionUID = -3803319440858795994L;
	
	public static final String KEY_CURRENT = "current";
	public static final String KEY_TYPE = "type";
	public static final String KEY_LENGTH = "length";
	public static final String KEY_SOURCE_ADDRESS = "sourceAddress";
	public static final String KEY_SELECTABLE_CHANNEL = "selectableChannel";
	public static final String KEY_BUFFER = "buffer";
	public static final String KEY_CHANNEL_CONTEXT = "channelContext";
	public static final String KEY_RESPONSE_HEAD = "responseHead";
	public static final String KEY_RESPONSE_LOCK = "responseLock";
	public static final String KEY_HOSTNAME = "hostname";
	public static final String KEY_PORT = "port";
	public static final String KEY_ID = "id";

	public Buffer getBuffer() {
		return (Buffer) get(KEY_BUFFER);
	}

	public void setBuffer(Buffer buffer) {
		put(KEY_BUFFER, buffer);
	}

	public SelectableChannel getSelectableChannel() {
		return (SelectableChannel) get(KEY_SELECTABLE_CHANNEL);
	}

	public void setSelectableChannel(SelectableChannel selectableChannel) {
		put(KEY_SELECTABLE_CHANNEL, selectableChannel);
	}

	public SocketAddress getSourceAddress() {
		return (SocketAddress) get(KEY_SOURCE_ADDRESS);
	}

	public void setSourceAddress(SocketAddress sourceAddress) {
		put(KEY_SOURCE_ADDRESS, sourceAddress);
	}

	public Integer getLength() {
		return (Integer) get(KEY_LENGTH);
	}

	public void setLength(Integer length) {
		put(KEY_LENGTH, length);
	}

	public byte getType() {
		return (Byte) get(KEY_TYPE);
	}

	public void setType(Byte type) {
		put(KEY_TYPE, type);
	}

	public Integer getCurrent() {
		return (Integer) get(KEY_CURRENT);
	}

	public void setCurrent(Integer current) {
		put(KEY_CURRENT, current);
	}

	public ChannelContext getChannelContext() {
		return (ChannelContext) get(KEY_CHANNEL_CONTEXT);
	}
	
	public void setChannelContext(ChannelContext channelContext) {
		put(KEY_CHANNEL_CONTEXT, channelContext);
	}
	
	public ResponseHead getResponseHead() {
		return (ResponseHead) get(KEY_RESPONSE_HEAD);
	}
	
	public void setResponseHead(ResponseHead responseHead) {
		put(KEY_RESPONSE_HEAD, responseHead);
	}
	
	public Object getResponseLock() {
		return get(KEY_RESPONSE_LOCK);
	}
	
	public void setResponseLock(Object responseLock) {
		put(KEY_RESPONSE_LOCK, responseLock);
	}
	
	public String getHostname() {
		return (String) get(KEY_HOSTNAME);
	}
	
	public void setHostname(String hostname) {
		put(KEY_HOSTNAME, hostname);
	}
	
	public Integer getPort() {
		return (Integer) get(KEY_PORT);
	}
	
	public void setPort(int port) {
		put(KEY_PORT, port);
	}
	
	public Integer getId() {
		return (Integer) get(KEY_ID);
	}
	
	public void setId(int id) {
		put(KEY_ID, id);
	}

}
