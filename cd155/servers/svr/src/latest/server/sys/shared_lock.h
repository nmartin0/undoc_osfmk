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
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: shared_lock.h,v $
 * Revision 1.4  1994/11/18  20:41:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:27:19  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:36:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:40  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:12  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/12/16  18:04:21  roy
 * 	91/12/11  15:12:21  condict
 * 	Add a condition to shared lock, for sleeping when busy (fixes bug).
 *
 * Revision 2.2  91/08/31  14:11:39  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:08:49  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.4  90/11/05  16:56:40  rpd
 * 	Add spin_lock_t.
 * 	[90/10/31            rwd]
 * 
 * Revision 2.3  90/06/19  23:13:51  rpd
 * 	Minor cleanup.
 * 	[90/06/09            rpd]
 * 
 * Revision 2.2  90/05/29  20:24:34  rwd
 * 	Created from sys/ushared.h
 * 	[90/04/30            rwd]
 * 
 */

#ifndef	_SYS_SHARED_LOCK_H_
#define	_SYS_SHARED_LOCK_H_

typedef struct shared_lock {
	spin_lock_t lock;
	struct condition lock_is_free;	/* Sleep on this, waiting for lock */
	int timed_out;		/* Sleep timed out, something wrong */
	int who;		/* Which process holds the lock */
	int need_wakeup;	/* Indicates lock wanted by server thread */
} shared_lock_t;

#endif	_SYS_SHARED_LOCK_H_
