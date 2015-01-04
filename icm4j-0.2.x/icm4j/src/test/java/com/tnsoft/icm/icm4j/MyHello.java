package com.tnsoft.icm.icm4j;

import java.util.List;
import java.util.Map;

//import com.tnsoft.icm.icm4j.annotation.Name;

//@Name
public interface MyHello {

	String sayHello1(List<String> req1);
	
	String sayHello1I(List<Integer> req1i);
	
	String sayHello1R(List<Req3> req1r);
	
	String sayHello2(Map<Integer, String> req2);
	
	String sayHello3(Req3 req3);
}
