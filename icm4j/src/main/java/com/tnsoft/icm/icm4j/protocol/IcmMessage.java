package com.tnsoft.icm.icm4j.protocol;

import java.util.concurrent.atomic.AtomicInteger;

public abstract class IcmMessage {

	private static final AtomicInteger IDENTITY_FACTORY = new AtomicInteger(1);

	protected int id;

	protected int generateId() {
		return IDENTITY_FACTORY.getAndIncrement();
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

}
