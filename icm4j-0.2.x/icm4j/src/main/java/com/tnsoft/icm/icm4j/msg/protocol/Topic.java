package com.tnsoft.icm.icm4j.msg.protocol;

public class Topic {

	private String topicId;

	public String getTopicId() {
		return topicId;
	}

	public void setTopicId(String topicId) {
		this.topicId = topicId;
	}

	@Override
	public int hashCode() {
		return topicId.hashCode();
	}

	@Override
	public boolean equals(Object obj) {
		if (obj != null && obj instanceof Topic) {
			topicId.equals(((Topic) obj).topicId);
		}
		return super.equals(obj);
	}
	
	@Override
	public String toString() {
		return super.toString() + ", topicId: " + topicId;
	}
}