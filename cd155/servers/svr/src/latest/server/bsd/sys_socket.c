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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: sys_socket.c,v $
 * Revision 1.6  1995/02/01  21:29:10  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:28:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/09/01  01:34:45  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 2.5  93/08/22  23:52:52  mjl
 * [LCCbug #0370] Use VSOP_*() virtual socket macros here.  AF_UNIX domain
 * virtual sockets will use socketops fileops; the vsocketops fileops are
 * specific to true networking address families.
 * 
 * Revision 1.3  1993/07/14  17:49:41  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:50:17  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:17:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:07:16  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:08:35  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/05/18  12:30:16  roy
 * 	Take in OSF/1 1.1 soo_select complete.  sbselqueue is no longer used.
 * 	Only 1 selq is used per socket.   (loverso).
 *
 * Revision 2.3  92/05/12  00:06:58  loverso
 * 	Changes ala OSF/1 1.1 to only call select_enqueue() once, and to
 * 	call select_dequeue() faster.
 * 
 * Revision 2.2  91/08/31  13:23:29  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:55:32  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.14  90/10/31  13:49:49  devrcs
 * 	Rearrange soo_write so it builds with SEC_ARCH on.
 * 	[90/10/10  12:00:38  tmt]
 * 
 * 	Separate NDELAY and NONBLOCK.
 * 	[90/10/12  10:16:53  jvs]
 * 
 * 	fix recent regression that made B1SECURITY
 * 	version of this code not compile
 * 	[90/10/11  22:21:01  hosking]
 * 
 * Revision 1.13  90/10/07  13:19:35  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:51:54  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:00:33  gm]
 * 
 * 	Pass nonblock flag to sosend/soreceive from soo_write/soo_read.
 * 	Don't set SS_NBIO on pipes, use above instead.
 * 	[90/09/29  17:09:41  tmt]
 * 
 * Revision 1.12  90/09/13  11:42:26  devrcs
 * 	Remove issig() glue, now sosleep handles it.
 * 	[90/08/28  11:49:12  tmt]
 * 
 * Revision 1.11  90/08/24  11:19:01  devrcs
 * 	Must call issig() unconditionally if sosleep indicates EINTR.
 * 	[90/08/20  10:10:04  tmt]
 * 
 * 	Make soo_read/write do issig without longjmp.
 * 	[90/08/20  07:14:34  gmf]
 * 
 * 	(It would be desirable for the networking to use tsleep)
 * 	[90/08/20  03:38:00  gmf]
 * 
 * Revision 1.10  90/07/27  08:44:45  devrcs
 * 	Update to BSD Reno release.
 * 	Modify soo_read and soo_write to conform.
 * 	[90/07/20  12:44:16  tmt]
 * 
 * Revision 1.9  90/07/05  23:08:03  devrcs
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	[90/07/03  18:39:41  tmt]
 * 
 * Revision 1.8  90/06/22  20:07:14  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:54:10  seiden]
 * 
 * 	Call issig() before longjmp'ing to rwuio, since sosleep
 * 	cannot (does not) do so. Pipe reads (e.g.) were not
 * 	restarting because of this bug.
 * 	[90/06/12  18:01:41  tmt]
 * 
 * 	Rearrange sec_sobufcount w/locks to avoid panic.
 * 	[90/06/11  16:32:49  tmt]
 * 
 * 	Use IOCGROUP macro. Remove unneded #includes. Take FP_LOCK in close.
 * 	[90/06/09  17:57:27  tmt]
 * 
 * 	nags merge
 * 	[90/06/12  21:16:56  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:41:01  seiden]
 * 
 * 	Remove MMAX_MP tokens and replace with PARALLEL_SELECT.
 * 	Restore old code in select, add unix_master around signals.
 * 	[90/06/06  14:06:58  tmt]
 * 
 * Revision 1.7  90/04/27  18:53:03  devrcs
 * 	Check error return of sosend/soreceive, they no longer longjmp.
 * 	[90/04/20  12:10:22  tmt]
 * 
 * Revision 1.6  90/04/14  00:30:03  devrcs
 * 	Add Robert Coren's poll interface, with #ifdef for !MACH.
 * 	De-lint and rearrange two things too.
 * 	[90/04/10  13:31:58  tmt]
 * 
 * Revision 1.5  90/03/27  13:14:56  gm
 * 	Filesystem parallelization changes [noemi]
 * 
 * Revision 1.4  90/01/18  08:42:11  gm
 * 	Do sopriv before ioctl to refresh SS_PRIV bit
 * 	[89/01/08  15:39:05  tmt]
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
 * Revision 1.3  90/01/03  11:51:00  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:26:55  gm]
 * 
 * Revision 1.2  89/12/26  09:21:51  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)sys_socket.c	2.1 16:10:28 4/20/90 SecureWare, Inc. */
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
 * Copyright (c) 1982, 1986, 1990 Regents of the University of California.
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
 *	Base:	sys_socket.c	7.5 (Berkeley) 5/9/89
 *	Merged: sys_socket.c	7.8 (Berkeley) 6/28/90
 */

#include "net/net_globals.h"
#if	MACH
#include <sys/secdefines.h>
#endif

#include "sys/param.h"
#include "sys/systm.h"
#include "sys/ioctl.h"
#include "sys/user.h"
#include "sys/file.h"
#include "sys/uio.h"
#include "sys/stat.h"

#include "sys/mbuf.h"
#include "sys/socket.h"
#ifdef TNC
#include "vsocket/vsocket.h"
#else
#include "sys/so_defs.h"
#endif
#include "sys/socketvar.h"
#include "sys/domain.h"
#include "sys/protosw.h"

LOCK_ASSERTL_DECL

CONST struct	fileops socketops =
    { soo_read, soo_write, soo_ioctl, soo_select, soo_close };

/* ARGSUSED */
soo_read(fp, uio, cred)
	struct file *fp;
	struct uio *uio;
	struct ucred *cred;
{
	int flags = 0;

	BM(FP_LOCK(fp));
	if (fp->f_flag & (FNDELAY|FNONBLOCK))
		flags = MSG_NONBLOCK;
	BM(FP_UNLOCK(fp));

	return VSOP_RECEIVE((struct socket *)fp->f_data, (struct mbuf **)0,
		uio, (struct mbuf **)0, (struct mbuf **)0, &flags);
}

/* ARGSUSED */
soo_write(fp, uio, cred)
	struct file *fp;
	struct uio *uio;
	struct ucred *cred;
{
#if	SEC_ARCH
	struct mbuf *control = 0;
	int error;
#endif
	int flags = 0;

	BM(FP_LOCK(fp));
	if (fp->f_flag & (FNDELAY|FNONBLOCK))
		flags = MSG_NONBLOCK;
	BM(FP_UNLOCK(fp));

#if	!SEC_ARCH
	return VSOP_SEND((struct socket *)fp->f_data, (struct mbuf *)0,
		uio, (struct mbuf *)0, (struct mbuf *)0, flags);
#else	/* SEC_ARCH */
	if (error = sec_internalize_rights(&control))
		return error;
	return VSOP_SEND((struct socket *)fp->f_data, (struct mbuf *)0,
		uio, (struct mbuf *)0, control, flags);
#endif
}

soo_ioctl(fp, cmd, data)
	struct file *fp;
	int cmd;
	register caddr_t data;
{
	register struct socket *so = (struct socket *)fp->f_data;
	int error = 0;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	switch (cmd) {

	case FIONBIO:
		if (so->so_special & SP_PIPE)	/* Handled by soo_read/write */
			break;
		if (*(int *)data)
			so->so_state |= SS_NBIO;
		else
			so->so_state &= ~SS_NBIO;
		break;

	case FIOASYNC:
		SOCKBUF_LOCK(&so->so_rcv);
		SOCKBUF_LOCK(&so->so_snd);
		if (*(int *)data) {
			so->so_state |= SS_ASYNC;
			so->so_rcv.sb_flags |= SB_ASYNC;
			so->so_snd.sb_flags |= SB_ASYNC;
		} else {
			so->so_state &= ~SS_ASYNC;
			so->so_rcv.sb_flags &= ~SB_ASYNC;
			so->so_snd.sb_flags &= ~SB_ASYNC;
		}
		SOCKBUF_UNLOCK(&so->so_snd);
		SOCKBUF_UNLOCK(&so->so_rcv);
		break;

	case FIONREAD:
#if	SEC_ARCH
		*(int *)data = sec_sobufcount(&so->so_rcv, so);
#else
		SOCKBUF_LOCK(&so->so_rcv);
		*(int *)data = so->so_rcv.sb_cc;
		SOCKBUF_UNLOCK(&so->so_rcv);
#endif
		break;

	case SIOCSPGRP:
		so->so_pgid = *(int *)data;
		break;

	case SIOCGPGRP:
		*(int *)data = so->so_pgid;
		break;

	case SIOCATMARK:
		*(int *)data = (so->so_state&SS_RCVATMARK) != 0;
		break;

	default:
		/*
		 * Interface/routing/protocol specific ioctls:
		 * interface and routing ioctls should have a
		 * different entry since a socket's unnecessary
		 * However, socket SS_PRIV bit serves as auth.
		 */
		sopriv(so);
		if (IOCGROUP(cmd) == 'i')
			error = ifioctl(so, cmd, data);
		else if (IOCGROUP(cmd) == 'r')
			error = rtioctl(so, cmd, data);
		else
			error = ((*so->so_proto->pr_usrreq)(so, PRU_CONTROL, 
		    (struct mbuf *)cmd, (struct mbuf *)data, (struct mbuf *)0));
		break;
	}
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

#if	MACH

#include "sys/poll.h"

/*
 * It is safe to not hold the SOCK_BUF lock for select_enqueue(),
 * as we hold the socket lock, and sowakeup() will hold that, too.
 */
soo_select(fp, events, revents, scanning)
/* Note: must not set revents when scanning == 0 */
	struct file *fp;
	short *events, *revents;
	int scanning;
{
	register struct socket *so = (struct socket *)fp->f_data;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	if (scanning) {
#ifdef notdef
/* from 1.1; shouldn't add this 'cause don't know what havoc might result */
		if (so->so_error)
			*revents |= POLLERR;
		else if (so->so_state & SS_CANTRCVMORE)
			*revents |= POLLHUP;
#endif
		if (*events & (POLLNORM|POLLPRI)) {
			SOCKBUF_LOCK(&so->so_rcv);
			if (*events & POLLNORM) {
				if (soreadable(so))
					*revents |= POLLNORM;
				else if (*revents == 0)
					so->so_rcv.sb_flags |= SB_SEL;
			}
			if (*events & POLLPRI) {
				if (so->so_oobmark ||
				    (so->so_state & SS_RCVATMARK))
					*revents |= POLLPRI;
				else if (*revents == 0)
					so->so_rcv.sb_flags |= SB_SEL;
			}
			SOCKBUF_UNLOCK(&so->so_rcv);
		}
		if (*events & POLLOUT) {
			SOCKBUF_LOCK(&so->so_snd);
			if (sowriteable(so))
				*revents |= POLLOUT;
			else if (*revents == 0)
				so->so_snd.sb_flags |= SB_SEL;
			SOCKBUF_UNLOCK(&so->so_snd);
		}
		if (*revents == 0)
			select_enqueue(&so->so_rcv.sb_selq);
	} else
		select_dequeue(&so->so_rcv.sb_selq);

	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (0);
}

#else	/* UNIX || MACH pre-above */

/* This certainly doesn't work! */

#if	PARALLEL_SELECT
soo_select(fp, which, scanning)
#else
soo_select(fp, which)
#endif
	struct file *fp;
	int which;
{
	register struct socket *so = (struct socket *)fp->f_data;
	int ret = 0;
	DOMAIN_FUNNEL_DECL(f)

	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);

	switch (which) {

	case FREAD:
		SOCKBUF_LOCK(&so->so_rcv);
#if	PARALLEL_SELECT
		if (scanning) {
#endif
			if (soreadable(so))
				ret = 1;
			else
				sbselqueue(&so->so_rcv);
#if	PARALLEL_SELECT
		} else
			sbseldequeue(&so->so_rcv);
#endif
		SOCKBUF_UNLOCK(&so->so_rcv);
		break;

	case FWRITE:
		SOCKBUF_LOCK(&so->so_snd);
#if	PARALLEL_SELECT
		if (scanning) {
#endif
			if (sowriteable(so))
				ret = 1;
			else
				sbselqueue(&so->so_snd);
#if	PARALLEL_SELECT
		} else
			sbseldequeue(&so->so_snd);
#endif
		SOCKBUF_UNLOCK(&so->so_snd);
		break;

	case 0:
#if	PARALLEL_SELECT
		if (scanning) {
#endif
			if (so->so_oobmark ||
			    (so->so_state & SS_RCVATMARK))
				ret = 1;
			else {
				SOCKBUF_LOCK(&so->so_rcv);
				sbselqueue(&so->so_rcv);
				SOCKBUF_UNLOCK(&so->so_rcv);
			}
#if	PARALLEL_SELECT
		} else {
			SOCKBUF_LOCK(&so->so_rcv);
			sbseldequeue(&so->so_rcv);
			SOCKBUF_UNLOCK(&so->so_rcv);
		}
#endif
		break;
	}
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (ret);
}
#endif	/* UNIX */

soo_stat(so, ub)
	register struct socket *so;
	register struct stat *ub;
{
	int error = 0;
	DOMAIN_FUNNEL_DECL(f)

	bzero((caddr_t)ub, sizeof (*ub));
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	error = ((*so->so_proto->pr_usrreq)(so, PRU_SENSE,
	    (struct mbuf *)ub, (struct mbuf *)0, 
	    (struct mbuf *)0));
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	return (error);
}

soo_close(fp)
	struct file *fp;
{
	int error = 0;

	/*
	 * We don't need to lock fp here because soo_close is only
	 * called on the last close of the socket.
	 */
	if (fp->f_data)
		error = VSOP_CLOSE((struct socket *)fp->f_data);
	FP_LOCK(fp);
	fp->f_data = 0;
	FP_UNLOCK(fp);
	return (error);
}
