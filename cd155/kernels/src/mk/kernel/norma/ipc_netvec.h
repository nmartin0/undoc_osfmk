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
 * $Log: ipc_netvec.h,v $
 * Revision 1.6  1994/11/18  20:55:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/22  02:20:36  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:50:37  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:45:51  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:51:13  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_netvec.h,v 
 * Revision 1.1  1992/09/30  02:34:06  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.1  92/01/21  21:52:10  jsb
 * 	Added file/author/date comment.
 * 	[92/01/21  19:44:00  jsb]
 * 
 * 	First checkin. Contents moved here from norma/ipc_net.h.
 * 	[92/01/10  20:51:02  jsb]
 * 
 */ 
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1992 Carnegie Mellon University
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
 *	File:	norma/ipc_netvec.h
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definition for scatter/gather vector.
 */

struct netvec {
	unsigned long	addr;
	unsigned long	size;
};
