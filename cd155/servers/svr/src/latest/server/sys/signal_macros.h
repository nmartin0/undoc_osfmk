/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: signal_macros.h,v $
 * Revision 1.4  1994/11/18  20:41:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:27:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:51  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:25  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:24  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/12/16  18:23:05  roy
 * 	Remove VICE conditionals.
 *
 * Revision 2.2  91/08/31  14:12:21  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:09:20  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.3  90/03/14  21:29:36  rwd
 * 	Added VICE check for SRMT to mask stopsigmask.
 * 	[90/02/28            rwd]
 * 
 * Revision 2.2  89/10/17  11:26:34  rwd
 * 	Change HAVE_SIGNALS to check for signals in p_sig.
 * 	Add EXITING check.
 * 	[89/09/22            dbg]
 * 	Added history message.
 * 	[89/09/21            dbg]
 * 
 */
/*
 * Macros to check and lock signal handling.
 */

/*
 * Check whether a kernel thread should quit.  Conditions are
 * process exiting or signal being delivered to a user thread.
 */
#define	HAVE_SIGNALS(p)	\
	( (p)->p_sig & ~ \
	    ( (p)->p_sigmask \
	      | ( ((p)->p_flag&STRC) ? 0 : (p)->p_sigignore) \
	    ) \
	)

#define	EXITING(p)	(((p)->p_flag & SWEXIT) != 0)
