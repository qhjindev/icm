package com.tnsoft.icm.icm4j;

import java.util.Map;

import com.tnsoft.icm.icm4j.protocol.Out;

public interface Sample {

	String test(int i, boolean b, Out<Double> out);
	
	String ok();
	
	String map(Map<Integer, String> map, Out<Integer> out);
}
