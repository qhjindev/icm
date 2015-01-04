/*
 * com.tnsoft.util.logging.AbstractLog.java		1.00	2011-7-22
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
 * Create file at:	2011-7-22   下午03:14:39
 *
 */
package com.tnsoft.util.logging;

import org.apache.commons.logging.LogFactory;

/**
 * <p>
 * The abstract implementation of {@link Log} for generic operations.
 * </p>
 * 
 * @author M.J.Fung
 * 
 */
public abstract class AbstractLog implements Log {

	private org.apache.commons.logging.Log log;

	public AbstractLog(String name) {
		log = LogFactory.getLog(name);
	}

	public AbstractLog(Class<?> clazz) {
		log = LogFactory.getLog(clazz);
	}

	protected abstract MessageFormatter getMessageFormatter();

	public void trace(String message, Object... args) {
		if (log.isTraceEnabled()) {
			log.trace(getMessageFormatter().format(message, args));
		}
	}

	public void trace(String message, Throwable throwable, Object... args) {
		if (log.isTraceEnabled()) {
			log.trace(getMessageFormatter().format(message, args), throwable);
		}
	}

	public void trace(Object object) {
		if (log.isTraceEnabled()) {
			log.trace(object);
		}
	}

	public void debug(String message, Object... args) {
		if (log.isDebugEnabled()) {
			log.debug(getMessageFormatter().format(message, args));
		}
	}

	public void debug(String message, Throwable throwable, Object... args) {
		if (log.isDebugEnabled()) {
			log.debug(getMessageFormatter().format(message, args), throwable);
		}
	}

	public void debug(Object object) {
		if (log.isDebugEnabled()) {
			log.debug(object);
		}
	}

	public void info(String message, Object... args) {
		if (log.isInfoEnabled()) {
			log.info(getMessageFormatter().format(message, args));
		}
	}

	public void info(String message, Throwable throwable, Object... args) {
		if (log.isInfoEnabled()) {
			log.info(getMessageFormatter().format(message, args), throwable);
		}
	}

	public void info(Object object) {
		if (log.isInfoEnabled()) {
			log.info(object);
		}
	}

	public void warn(String message, Object... args) {
		if (log.isWarnEnabled()) {
			log.warn(getMessageFormatter().format(message, args));
		}
	}

	public void warn(String message, Throwable throwable, Object... args) {
		if (log.isWarnEnabled()) {
			log.warn(getMessageFormatter().format(message, args), throwable);
		}
	}

	public void warn(Object object) {
		if (log.isWarnEnabled()) {
			log.warn(object);
		}
	}

	public void error(String message, Object... args) {
		if (log.isErrorEnabled()) {
			log.error(getMessageFormatter().format(message, args));
		}
	}

	public void error(String message, Throwable throwable, Object... args) {
		if (log.isErrorEnabled()) {
			log.error(getMessageFormatter().format(message, args), throwable);
		}
	}

	public void error(Object object) {
		if (log.isErrorEnabled()) {
			log.error(object);
		}
	}

	public void fatal(String message, Object... args) {
		if (log.isFatalEnabled()) {
			log.fatal(getMessageFormatter().format(message, args));
		}
	}

	public void fatal(String message, Throwable throwable, Object... args) {
		if (log.isFatalEnabled()) {
			log.fatal(getMessageFormatter().format(message, args), throwable);
		}
	}

	public void fatal(Object object) {
		if (log.isFatalEnabled()) {
			log.fatal(object);
		}
	}

}
