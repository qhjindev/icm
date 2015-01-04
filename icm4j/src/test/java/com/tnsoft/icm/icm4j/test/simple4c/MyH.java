package com.tnsoft.icm.icm4j.test.simple4c;

import com.tnsoft.icm.icm4j.protocol.annotation.Name;
import com.tnsoft.icm.icm4j.protocol.annotation.Operation;
import com.tnsoft.icm.icm4j.protocol.extension.Out;

@Name("MyHello")
public interface MyH {

	@Operation("sayHello")
	String hello(String message, short u, Out<Long> out);
}
