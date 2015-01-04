package com.tnsoft.icm.sample.icm.service;

import com.tnsoft.icm.icm4j.protocol.annotation.Name;
import com.tnsoft.icm.icm4j.protocol.extension.Out;

@Name // Adding this annotation means the name of this service is the SimpleName of the Interface.
      // You can also use the annotation like this: @Name("MyHello")
public interface MyHello {
	
	String sayHello (String msg, short u, Out<Long> v);
}
