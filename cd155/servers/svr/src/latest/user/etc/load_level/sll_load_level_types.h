/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
/*
 * Copyright (c) 1993, Intel Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: sll_load_level_types.h,v $
 * Revision 1.5  1994/11/19  03:08:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/11/18  20:53:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/05/27  13:11:33  stefan
 * Moved NO_TIMEOUT to load_leveld.c (cosmetic).
 *
 *  Reviewer: yazz
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: developer testing
 *  Module(s): sll_load_level_types.h
 *
 * Revision 1.2  1993/05/21  20:34:32  stefan
 * Removed MIN macro because it is no longer needed.
 *
 * Revision 1.1  1993/05/18  13:42:08  stefan
 * Forgot this file when I integrated static load leveling support.
 *
 */


/*
 * External variables.
 */
extern char	*duplicate;
extern double	min_overload;
extern double	min_underload;
extern double	static_min_load_delta;
extern double	fast_node_timeout;
extern int	static_load_leveling;

/*
 * External functions.
 */
extern int	enable_remote_process_creation();
extern void	hand_fast_node_to_os();
extern void	fast_node();

