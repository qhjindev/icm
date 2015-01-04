package com.tnsoft.icm.sample.icm.service;

import com.tnsoft.icm.icm4j.protocol.annotation.Async;
import com.tnsoft.icm.icm4j.protocol.extension.Out;

public class MyHelloImpl implements MyHello {

	@Async // Adding this annotation means this method will be executed asynchronously.
	public String sayHello(String msg, short u, Out<Long> v) {
		System.out.println("----------------------------------------------------");
		System.out.printf("msg=%s, u=%d\n", msg, u);
		System.out.println("----------------------------------------------------");
		v.set(System.currentTimeMillis());
		return "You too.";
	}

}
