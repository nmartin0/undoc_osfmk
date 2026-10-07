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
 * HISTORY
 * $Log: unpcb.h,v $
 * Revision 1.4  1994/11/18  20:42:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:30:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:22  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:43:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:03  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/08/08  01:42:34  jdh
 * added extern for fake node number prototype -- jdh
 *
 * Revision 2.4  92/01/14  10:52:15  roy
 * 	Unix domain socket support now under OSF1_ADFS.
 * 
 * Revision 2.3  92/01/09  16:15:27  roy
 * 	Unix domain socket support (OFS1_ADFS). (loverso)
 * 
 * Revision 2.2  91/08/31  14:16:20  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:47:16  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10  90/10/07  14:56:20  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:48:34  gm]
 * 
 * 	Add access, modify, change times to unpcb for Posix/AES
 * 	compliance on pipe operations. Sigh.
 * 	[90/09/20  18:40:13  tmt]
 * 
 * Revision 1.9  90/07/27  09:08:31  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  15:52:01  tmt]
 * 
 * Revision 1.8  90/06/22  20:54:41  devrcs
 * 	unp_rcnt and unp_wcnt are no longer needed, since the fifo code
 * 	now uses the vnode readers/writers counts.
 * 	[90/06/17  17:32:13  ers]
 * 
 * Revision 1.7  90/04/27  19:27:51  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:09:26  tmt]
 * 
 * Revision 1.6  90/04/14  00:34:18  devrcs
 * 	Move data declarations. KERNEL => _KERNEL
 * 	[90/04/09  15:47:17  tmt]
 * 
 * Revision 1.5  90/02/23  00:33:13  devrcs
 * 	Added rcnt and wcnt fields to struct unpcb to keep track
 * 	of number of readers and writers on named pipes (FIFOs).
 * 	[90/02/18  14:05:05  ers]
 * 
 * Revision 1.4  90/01/18  08:49:12  gm
 * 	Make file re-entrant.
 * 	[90/01/08  15:43:59  tmt]
 * 
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/03  12:44:18  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:42:07  gm]
 * 
 * Revision 1.2  89/12/26  10:43:06  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	unpcb.h	7.4 (Berkeley) 5/9/89
 *	Merged: unpcb.h	7.6 (Berkeley) 6/28/90
 */

#ifndef	_SYS_UNPCB_H_
#define _SYS_UNPCB_H_

/*
 * Protocol control block for an active
 * instance of a UNIX internal protocol.
 *
 * A socket may be associated with a vnode in the
 * file system.  If so, the unp_vnode pointer holds
 * a reference count to this vnode, which should be vrele'd
 * when the socket goes away.
 *
 * A socket may be connected to another socket, in which
 * case the control block of the socket to which it is connected
 * is given by unp_conn.
 *
 * A socket may be referenced by a number of sockets (e.g. several
 * sockets may be connected to a datagram socket.)  These sockets
 * are in a linked list starting with unp_refs, linked through
 * unp_nextref and null-terminated.  Note that a socket may be referenced
 * by a number of other sockets and may also reference a socket (not
 * necessarily one which is referencing it).  This generates
 * the need for unp_refs and unp_nextref to be separate fields.
 *
 * Stream sockets keep copies of receive sockbuf sb_cc and sb_mbcnt
 * so that changes in the sockbuf may be computed to modify
 * back pressure on the sender accordingly.
 */
struct	unpcb {
	struct	socket *unp_socket;	/* pointer back to socket */
#ifndef OSF1_ADFS
	struct  vnode *unp_vnode;       /* if associated with file */
#else
	mach_port_t unp_vnodeport;      /* if associated with file */
#endif
	ino_t	unp_vno;		/* fake vnode number */
	struct	unpcb *unp_conn;	/* control block of connected socket */
	struct	unpcb *unp_refs;	/* referencing socket linked list */
	struct 	unpcb *unp_nextref;	/* link in unp_refs list */
	struct	mbuf *unp_addr;		/* bound address of socket */
	int	unp_cc;			/* copy of rcv.sb_cc */
	int	unp_mbcnt;		/* copy of rcv.sb_mbcnt */
	time_t	unp_atime;		/* pipe access time for stat */
	time_t	unp_mtime;		/* pipe modify time for stat */
	time_t	unp_ctime;		/* pipe change time for stat */
};

#define	sotounpcb(so)	((struct unpcb *)((so)->so_pcb))

#ifdef	_KERNEL
extern ino_t	unp_vno;	/* prototype for fake vnode numbers */
#if	NETSYNC_LOCK
extern simple_lock_data_t	global_unpconn_lock, unp_misc_lock;
#define UNPCONN_LOCKINIT()	simple_lock_init(&global_unpconn_lock)
#define UNPCONN_LOCK()		simple_lock(&global_unpconn_lock)
#define UNPCONN_UNLOCK()	simple_unlock(&global_unpconn_lock)
#define UNPMISC_LOCKINIT()	simple_lock_init(&unp_misc_lock)
#define UNPMISC_LOCK()		simple_lock(&unp_misc_lock)
#define UNPMISC_UNLOCK()	simple_unlock(&unp_misc_lock)
#else
#define UNPCONN_LOCKINIT()
#define UNPCONN_LOCK()
#define UNPCONN_UNLOCK()
#define UNPMISC_LOCKINIT()
#define UNPMISC_LOCK()
#define UNPMISC_UNLOCK()
#endif
#endif
#endif
