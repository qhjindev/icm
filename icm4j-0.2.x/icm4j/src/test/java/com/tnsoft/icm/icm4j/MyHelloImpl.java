package com.tnsoft.icm.icm4j;

import java.util.List;
import java.util.Map;

import com.tnsoft.icm.icm4j.annotation.Async;

@Async
public class MyHelloImpl implements MyHello {

	public String sayHello1(List<String> req) {
		System.out.println("-- sayHello1 --");
		StringBuilder builder = new StringBuilder().append('[');
		for (String r : req) {
			builder.append('"').append(r).append('"').append(',');
		}
		builder.deleteCharAt(builder.length() - 1).append(']');
		System.out.println("  " + builder.toString());
		return builder.toString();
	}

	public String sayHello1I(List<Integer> req) {
		System.out.println("-- sayHello1I --");
		String result = req.toString();
		System.out.println("   " + result);;
		return result;
	}

	public String sayHello1R(List<Req3> req) {
		System.out.println("-- sayHello1R --");
		String result = req.toString();
		System.out.println("   " + result);;
		return result;
	}

	public String sayHello2(Map<Integer, String> req) {
		System.out.println("-- sayHello2 --");
		String result = req.toString();
		System.out.println("   " + result);;
		return result;
	}

	public String sayHello3(Req3 req) {
		System.out.println("-- sayHello3 --");
		String result = req.toString();
		System.out.println("   " + result);;
		return result;
	}

}
