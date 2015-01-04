package com.tnsoft.icm.msg4j.test;

import java.util.List;

public class Event {

	private String topic;
	private short item1;
	private String item2;
	private Device device;
	private List<Port> ports;

	public String getTopic() {
		return topic;
	}

	public void setTopic(String topic) {
		this.topic = topic;
	}

	public short getItem1() {
		return item1;
	}

	public void setItem1(short item1) {
		this.item1 = item1;
	}

	public String getItem2() {
		return item2;
	}

	public void setItem2(String item2) {
		this.item2 = item2;
	}

	public Device getDevice() {
		return device;
	}

	public void setDevice(Device device) {
		this.device = device;
	}

	public List<Port> getPorts() {
		return ports;
	}

	public void setPorts(List<Port> ports) {
		this.ports = ports;
	}

}
