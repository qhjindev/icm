package com.tnsoft.icm.icm4j;

import java.util.List;

public class Req3 {

	private int svn;
	private List<String> request1;

	public int getSvn() {
		return svn;
	}

	public void setSvn(int svn) {
		this.svn = svn;
	}

	public List<String> getRequest1() {
		return request1;
	}

	public void setRequest1(List<String> request1) {
		this.request1 = request1;
	}

	@Override
	public String toString() {
		StringBuilder builder = new StringBuilder().append('{');
		builder.append('"').append("svn").append('"').append(':').append(svn).append(',');
		builder.append('"').append("request1").append('"').append(':').append('[');
		for (String req : request1) {
			builder.append('"').append(req).append('"').append(',');
		}
		builder.deleteCharAt(builder.length() - 1).append(']');
		return builder.append('}').toString();
	}

}
