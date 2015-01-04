package com.tnsoft.icm.icm4j.test.simple4c;

import com.tnsoft.icm.icm4j.protocol.annotation.Name;
import com.tnsoft.icm.icm4j.protocol.extension.Out;

// For using with C/C++ server, you must set the name (operation) by the @Name (@Operation) !
@Name("MyHello")
public interface MyHello {
	
//////////////////////////////////////////////////////////////////////////////
// You can also write the method like this:
//	
//	@com.tnsoft.icm.icm4j.protocol.annotation.Operation("sayHello")
//	String this_is_the_same_as_sayHello(String msg, short u, Out<Long> out);
//
// also see: com.tnsoft.icm.icm4j.test.simple4c.MyH
//
//////////////////////////////////////////////////////////////////////////////

	String sayHello(String msg, short u, Out<Long> out);
}
