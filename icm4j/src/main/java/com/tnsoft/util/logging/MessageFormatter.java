/*
 * com.tnsoft.util.logging.MessageFormatter.java		1.00	2011-7-22
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
 * Create file at:	2011-7-22   下午03:51:16
 *
 */
package com.tnsoft.util.logging;

/**
 * <p>
 * Provide methods to format message string.
 * </p>
 * 
 * @author M.J.Fung
 * 
 */
public interface MessageFormatter {

	String format(String message, Object... args);

}
