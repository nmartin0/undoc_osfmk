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
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * HISTORY
 * $Log: xpr.h,v $
 * Revision 1.5  1994/11/18  20:34:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:25:38  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:24:16  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:24:15  dleslie
 * First R1_0 release
 *
 * Revision 2.4.9.2  92/04/30  11:51:27  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.4.9.1  92/02/18  18:50:37  jeffreyh
 * 	Changed time_stamp type to unsigned
 * 	[91/12/06            bernadat]
 * 
 * 	Support for the Corollary MP,
 * 	time_stamp can be implemented by a slave cpu
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.4  91/05/14  16:19:01  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:15:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:39:08  mrt]
 * 
 * Revision 2.2  90/05/03  15:38:37  dbg
 * 	First checkin.
 * 
 * Revision 1.3  89/02/26  12:35:32  gm0w
 * 	Changes for cleanup.
 * 
 */
 
/*
 *	File:	xpr.h
 *
 *	Machine dependent module for the XPR tracing facility.
 */

#include <platforms.h>
#if	AT386 && NCPUS > 1
extern	unsigned time_stamp;
#define	XPR_TIMESTAMP	(int) time_stamp
#else	AT386 && NCPUS > 1
#define XPR_TIMESTAMP	(0)
#endif	AT386 && NCPUS > 1
