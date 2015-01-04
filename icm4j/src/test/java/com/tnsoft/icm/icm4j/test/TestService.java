package com.tnsoft.icm.icm4j.test;

import java.util.List;
import java.util.Map;

import com.tnsoft.icm.icm4j.protocol.annotation.Name;
import com.tnsoft.icm.icm4j.protocol.annotation.Operation;
import com.tnsoft.icm.icm4j.protocol.extension.Out;

@Name("TestService")
public interface TestService {

	int getLength(String value);
	
	@Operation("getId")
	String generateID(String name, int age);
	
	void noResultJustPrint(String value);
	
	int sum(int[] values);
	
	double division(float a, float b);
	
	String[] split(String src, String regex);
	
	int[] sort(int[] src);
	
	Map<String, Integer> callback(Map<String, Integer> map); 
	
	List<String> convert(List<Integer> list);
	
	void testOut_1(int n, Out<Long> nx2);
	
	Long testOut_2(int n, Out<String> helloWorld);
	
	Struct testStruct(Struct struct);
}
