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
 * @OSF_COPYRIGHT@
 */
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)acct.h	1.5  com/inc/sys,3.1,8943 9/21/89 18:47:28 */
/* acct.h	5.2 - 87/01/09 - 18:20:16 */
/*
 * HISTORY
 * $Log: acct.h,v $
 * Revision 1.4  1994/11/18  20:39:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:23:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:20  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:05  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:38:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:30  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/12/16  18:34:14  roy
 * 	91/11/26  15:33:44  sp
 * 	Upgrade to 1.0.3
 *
 * Revision 2.2  91/08/31  14:02:01  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:41:27  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:48:41  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:36:31  gm]
 * 
 * Revision 1.6  90/06/22  20:52:20  devrcs
 * 	Changed uid, gid, and tty fields to shorts for now to keep structure
 * 	size from changing as uid_t, gid_t, and dev_t get widened.
 * 	[90/06/06  13:04:31  ers]
 * 
 * 	Added system V fields.
 * 	[90/06/10  17:24:30  ers]
 * 
 * Revision 1.5  90/05/13  20:33:29  devrcs
 * 	Rearranged fields for better compatibility with BSD acct file format.
 * 	[90/05/07  15:45:19  ers]
 * 
 * Revision 1.4	 90/03/13  20:48:59  mbrown
 * 	AIX merge first cut.
 * 	[90/02/09  16:01:20  tom]
 * 
 * Revision 1.3	 90/01/02  20:15:37  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */

#ifndef _SYS_ACCT_H_
#define _SYS_ACCT_H_

#include "sys/types.h"
#include "sys/param.h"
/*
 * Accounting structures
 */

typedef ushort comp_t;		/* "floating point" */
		/* 13-bit fraction, 3-bit exponent  */

struct	acct
{
	char	ac_comm[8];		/* Accounting command name */
	comp_t	ac_io;			/* number of chars read/written */
	comp_t	ac_utime;		/* Accounting user time */
	comp_t	ac_stime;		/* Accounting system time */
	comp_t	ac_etime;		/* Accounting elapsed time */
	time_t	ac_btime;		/* Beginning time */
	uid_t	ac_uid;			/* Accounting user ID */
	gid_t	ac_gid;			/* Accounting group ID */
	short	ac_mem;			/* average memory usage */
	comp_t	ac_rw;			/* blocks read or written */
	dev_t	ac_tty;			/* control typewriter */
	char	ac_flag;		/* Accounting flag */
	char	ac_stat;		/* Exit status */
};

#define AFORK	0001		/* has executed fork, but no exec */
#define ASU	0002		/* used super-user privileges */
#define ACOMPAT 0004		/* used compatibility mode */
#define ACORE	0010		/* dumped core */
#define AXSIG	0020		/* killed by a signal */

/*
 * 1/AHZ is the granularity of the data encoded in the various
 * comp_t fields.  This is not necessarily equal to hz.
 */
#define AHZ 64

#ifdef	_KERNEL
extern struct	acct	acctbuf;
extern struct	vnode	*acctp;
#endif	/* _KERNEL */


#endif	/* _SYS_ACCT_H_ */
