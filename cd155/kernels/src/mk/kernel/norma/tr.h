/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */

/*
 * SSD HISTORY
 * $Log: tr.h,v $
 * Revision 1.6  1994/11/18  20:56:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/22  02:21:11  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:00  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:21  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:51:46  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: tr.h,v 
 * Revision 1.1  1992/09/30  02:34:09  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.1  92/09/15  17:35:14  jeffreyh
 * 	Created.
 * 	[92/09/15  15:27:42  jeffreyh]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 *	File:		norma/tr.h
 *	Author:		Alan Langerman, Jeffrey Heller
 *	Date:		1992
 *
 *	Internal trace routines.  Like old-style XPRs but
 *	less formatting.
 */

#include <mach_assert.h>

#define	TRACE_BUFFER	MACH_ASSERT

/*
 *	Log events in a circular trace buffer for future debugging.
 *	Events are unsigned integers.  Each event has a descriptive
 *	message.
 *
 *	TR_DECL must be used at the beginning of a routine using
 *	one of the tr calls.  The macro should be passed the name
 *	of the function surrounded by quotation marks, e.g.,
 *		TR_DECL("netipc_recv_intr");
 *	and should be terminated with a semi-colon.  The TR_DECL
 *	must be the *last* declaration in the variable declaration
 *	list, or syntax errors will be introduced when TRACE_BUFFER
 *	is turned off.
 */

#if	TRACE_BUFFER
#define	TR_DECL(funcname)	char	*__ntr_func_name__ = funcname
#define	tr1(msg)							\
	tr(__ntr_func_name__, __FILE__, __LINE__, (msg),0,0,0,0)
#define	tr2(msg,tag1)							\
	tr(__ntr_func_name__, __FILE__, __LINE__, (msg),(tag1),0,0,0)
#define	tr3(msg,tag1,tag2)						\
	tr(__ntr_func_name__, __FILE__, __LINE__, (msg),(tag1),(tag2),0,0)
#define	tr4(msg,tag1,tag2,tag3)						\
	tr(__ntr_func_name__, __FILE__, __LINE__, (msg),(tag1),(tag2),(tag3),0)
#define	tr5(msg,tag1,tag2,tag3,tag4)					\
	tr(__ntr_func_name__, __FILE__, __LINE__, (msg),(tag1),(tag2),(tag3),(tag4))
#else
#define	TR_DECL(funcname)
#define tr1(msg)
#define tr2(msg, tag1)
#define tr3(msg, tag1, tag2)
#define tr4(msg, tag1, tag2, tag3)
#define tr5(msg, tag1, tag2, tag3, tag4)
#endif
