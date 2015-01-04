/*
 * com.tnsoft.util.logging.LogFactory.java		1.00	2011-7-22
 * CREATE		M.J.Fung
 * MODULE ID    logging
 *
 * Copyright (c) 2011 Transoft, Inc.
 * All rights resvered.
 *
 * This software is the confidential and proprietary information of
 * Transoft, Inc. ("confidential Information"). You shall not disclose
 * such Confidential Information and shall use it only in accordance
 * with the terms of the license agreement you entered into with Transoft.
 *
 * Alteration Records:
 * Create file at:	2011-7-22   下午04:07:51
 *
 */
package com.tnsoft.util.logging;

import java.text.MessageFormat;

/**
 * <p>
 * A factory to create {@link Log} instance.
 * <p>
 * 
 * @author M.J.Fung
 * 
 */
public class LogFactory {

	public static Log getFormatterLog(String name) {
		return new FormatterLog(name);
	}

	public static Log getFormatterLog(Class<?> clazz) {
		return new FormatterLog(clazz);
	}

	public static Log getMessageFormatLog(String name) {
		return new MessageFormatLog(name);
	}

	public static Log getMessageFormatLog(Class<?> clazz) {
		return new MessageFormatLog(clazz);
	}

	private static class FormatterLog extends AbstractLog {

		private static MessageFormatter formatter = new MessageFormatter() {

			public String format(String message, Object... args) {
				return String.format(message, args);
			}
		};

		public FormatterLog(String name) {
			super(name);
		}

		public FormatterLog(Class<?> clazz) {
			super(clazz);
		}

		protected MessageFormatter getMessageFormatter() {
			return formatter;
		}

	}

	private static class MessageFormatLog extends AbstractLog {

		private static MessageFormatter messager = new MessageFormatter() {

			public String format(String message, Object... args) {
				return MessageFormat.format(message, args);
			}
		};

		public MessageFormatLog(String name) {
			super(name);
		}

		public MessageFormatLog(Class<?> clazz) {
			super(clazz);
		}

		protected MessageFormatter getMessageFormatter() {
			return messager;
		}

	}

}
