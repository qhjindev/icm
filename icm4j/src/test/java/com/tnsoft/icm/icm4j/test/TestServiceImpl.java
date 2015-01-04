package com.tnsoft.icm.icm4j.test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

import com.tnsoft.icm.icm4j.protocol.annotation.Async;
import com.tnsoft.icm.icm4j.protocol.extension.Out;

public class TestServiceImpl implements TestService {

	public int getLength(String value) {
		return value.length();
	}

	public String generateID(String name, int age) {
		return name + "-" + age + "-" + System.nanoTime();
	}

	public void noResultJustPrint(String value) {
		System.out.println("-- get the message: \"" + value + "\" from client");
	}

	public int sum(int[] values) {
		int sum = 0;
		for (int value : values) {
			sum += value;
		}
		return sum;
	}

	public double division(float a, float b) {
		return a / b;
	}

	public String[] split(String src, String regex) {
		return src.split(regex);
	}

	@Async
	public int[] sort(int[] src) {
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		Arrays.sort(src);
		return src;
	}

	public Map<String, Integer> callback(Map<String, Integer> map) {
		return map;
	}

	public List<String> convert(List<Integer> list) {
		List<String> s = new ArrayList<String>();
		for (Integer i : list) {
			s.add(String.valueOf(i));
		}
		return s;
	}

	public void testOut_1(int n, Out<Long> nx2) {
		long r = n *2;
		nx2.set(r);
	}

	public Long testOut_2(int n, Out<String> helloWorld) {
		long r = n * 2;
		helloWorld.set("Hello world!");
		return r;
	}

	public Struct testStruct(Struct struct) {
		return struct;
	}

}
