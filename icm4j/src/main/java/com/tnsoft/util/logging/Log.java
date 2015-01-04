/*
 * com.tnsoft.util.logging.Log.java		1.00	2011-7-22
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
 * Create file at:	2011-7-22   下午03:07:10
 *
 */
package com.tnsoft.util.logging;

/**
 * <p>
 * A logger object used to log messages.
 * </P>
 * 
 * @author M.J.Fung
 * 
 */
public interface Log {

	void trace(String message, Object... args);

	void trace(String message, Throwable throwable, Object... args);

	void trace(Object object);

	void debug(String message, Object... args);

	void debug(String message, Throwable throwable, Object... args);
	
	void debug(Object object);

	void info(String message, Object... args);

	void info(String message, Throwable throwable, Object... args);
	
	void info(Object object);

	void warn(String message, Object... args);
	
	void warn(String message, Throwable throwable, Object... args);

	void warn(Object object);

	void error(String message, Object... args);
	
	void error(String message, Throwable throwable, Object... args);

	void error(Object object);

	void fatal(String message, Object... args);
	
	void fatal(String message, Throwable throwable, Object... args);

	void fatal(Object object);
}
