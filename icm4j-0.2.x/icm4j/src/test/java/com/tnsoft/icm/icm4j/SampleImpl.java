package com.tnsoft.icm.icm4j;

import java.util.Map;
import java.util.Map.Entry;
import java.util.Random;

import com.tnsoft.icm.icm4j.annotation.Async;
import com.tnsoft.icm.icm4j.protocol.Out;

@Async
public class SampleImpl implements Sample {

	public String test(int i, boolean b, Out<Double> out) {
		out.set(new Random().nextDouble());
		System.out.println("out: " + out.get());
		return "int: " + i + ", boolean: " + b;
	}

	public String ok() {
		return "ok";
	}

	public String map(Map<Integer, String> map, Out<Integer> out) {
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {}
		StringBuilder builder = new StringBuilder();
		int sam = 0;
		for (Entry<Integer, String> entry : map.entrySet()) {
			builder.append(entry.getValue()).append("+");
			sam += entry.getKey();
		}
		builder.deleteCharAt(builder.lastIndexOf("+"));
		builder.append("=").append(sam);
		out.set(sam);
		return builder.toString();
	}

}
