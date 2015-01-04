package com.tnsoft.icm.msg4j;

public interface EventAction<E> {

	/**
	 * Get the class of the Event Content.
	 * 
	 * @return
	 */
	Class<E> getEventClass();

	/**
	 * Implement by the super class.
	 * <p>
	 * When an event for the topic accepting, the {@link Subscriber} will invoke this method to do the specific logic.
	 * </p>
	 * 
	 * @param topicId
	 * @param event
	 */
	void execute(String topicId, E event);

}
