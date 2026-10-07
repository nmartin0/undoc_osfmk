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
 * $Log: uipc_syscalls.c,v $
 * Revision 1.11  1995/02/01  21:30:51  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:28:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/01/13  17:52:16  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.8  1993/09/01  01:35:07  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.7  1993/07/14  17:51:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  18:51:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:08:18  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:26:06  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.2.2.1  1993/02/16  20:03:20  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/02/03  00:22:06  nandy
 * 02/02/93 drop from LOCUS
 *   This fixes the getsockname panic of freeing a free mbuf
 *   Fixed by Bernie Keany
 *
 * Revision 1.3  1992/11/30  22:17:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  18:22:09  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/06  00:07:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.23  93/08/22  23:54:28  mjl
 * [LCCbug #0370] Make AF_UNIX virtual sockets use "more vanilla" fileops.
 * Revision 2.21  92/10/27  17:30:26  bhk
 * Restructured getsockname adn get peername to use the sogetaddr function.
 * Virtualized the sogetaddr function.
 * 
 * Revision 2.22  93/02/02  12:12:50  bhk
 * This fixes the getsockname panic of freeing a free mbuf
 * Fixed by Bernie Keany
 *
 * Revision 2.21  92/10/27  17:30:26  bhk
 * Restructured getsockname adn get peername to use the sogetaddr function.
 * Virtualized the sogetaddr function.
 * 
 * Revision 2.20  92/08/17  12:52:16  mjl
 * Got rid of extraneous arg to VSOP_SLEEP().  Use of uth->uu_opn_options is
 * now obsolete.
 * 
 * Revision 2.19  92/08/08  01:37:07  jdh
 * added extra field to VSOP_SLEEP()
 * XXX don't need -- remove extra field -- jdh
 * 
 * Revision 2.18  92/07/26  17:21:04  bhk
 * Added TNC support for getpeername()
 * 
 * Revision 2.17  92/06/26  18:00:44  mjl
 * Now that protocol number is used to select among various sets of AF_UNIX
 * virtual socket ops, forbid the user from specifying any protocol number
 * but 0.  Only syscalls can specify non-zero protocol numbers for AF_UNIX.
 * 
 * Revision 2.16  92/06/15  17:31:46  mjl
 * In socket creation syscalls, pass file struct address to low level code
 * via the uthread area.  This required reordering some syscalls to allocate
 * the file struct *before* calling SOCREATE().
 * 
 * Revision 2.15  92/05/06  13:09:55  bhk
 * added VSOP_SLEEP virtual socket operation
 * 
 * Revision 2.14  92/04/22  16:57:10  bhk
 * Added support for virtual getsockname
 * call
 * 
 * Revision 2.13  92/04/20  17:25:31  bhk
 * Added virtual socket operations for soconnect, sosend, soreceive, soshutdown,
 * sosetopt and sogetopt
 * 
 * Revision 2.12  92/04/14  10:14:55  roman
 * Revert to SOCREATE()s; add calls to SOSETFP() and use VSOP_CONNECT2() in place
 * 	of soconnect2().
 * 
 * Revision 2.11  92/04/06  19:09:40  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.7  92/04/05  16:51:02  pjg
 * 	Renamed SOCREATE to VSOP_CREATE (pjg).
 * 
 * 	Allow non-TNC build without Virtual Sockets (chrisp)
 * 
 * 	Add support for vsockets for TNC. (roman)
 * 
 * Revision 2.6  92/03/09  14:35:28  durriya
 * 	Revision 3.3  92/01/15  16:54:45  jose
 * 	Changed mispelled UNI_COMPAT to SER_COMPAT
 * 	(allows parallelized pipes & sockets) [PhB].
 * 
 * Revision 2.5  91/12/17  13:53:57  roy
 * 	91/10/30  17:42:22  bernadat
 * 	Horrible hack for AFS 3.0 binary compatibility with MACH 2.5.
 * 
 * Revision 2.4  91/10/04  14:54:44  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:52:46  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	System calls with file descriptors as the first uarg were changed to 
 * 	have a file pointer as the first argument, and no file descriptor
 * 	in the uarg. Use new interface to getf(). New interface to getf()
 * 	means changes to interface to getsock(). Change interface between
 * 	the emulator and the accept(), socket(), socketpair(), and pipe()
 * 	system calls to pass back file structure pointers as explicit
 * 	output parameters. SIGPIPE signal on EPIPE error now done in emulator.
 * 
 * Revision 2.2  91/08/31  13:24:57  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:32:11  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.15  90/10/31  13:50:25  devrcs
 * 	Delete double sockaddr conversion in accept(), done by sodequeue.
 * 	[90/10/25  07:58:06  tmt]
 * 
 * 	Modify accept() syscall to use sodequeue, must do so with new so_dqlen.
 * 	[90/10/10  12:03:04  tmt]
 * 
 * Revision 1.14  90/10/07  13:20:59  devrcs
 * 	Cleanup u_file_state and fix Unix build.
 * 	[90/10/01  16:55:42  tmt]
 * 
 * 	Move MSG_COMPAT to sys/socket.h.
 * 	[90/10/01  09:44:24  tmt]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:03:59  gm]
 * 
 * 	Hide MACH file array changes from Unix build.
 * 	[90/09/29  18:07:12  tmt]
 * 
 * 	Set SP_PIPE and SP_WATOMIC attributes on pipe _before_ soconnect2.
 * 	[90/09/27  09:37:48  tmt]
 * 
 * 	Move SS_WATOMIC to so_special as SP_WATOMIC, add SP_PIPE.
 * 	[90/09/20  18:45:44  tmt]
 * 
 * Revision 1.13  90/09/13  11:42:51  devrcs
 * 	Remove issig() glue in send/receive now sosleep() handles it.
 * 	Add PCATCH to sosleep priority.
 * 	[90/08/28  11:53:11  tmt]
 * 
 * 	Add sockaddr converters for COMPAT_43 (just rearranges code).
 * 	[90/08/24  12:46:26  tmt]
 * 
 * Revision 1.12  90/08/24  11:19:35  devrcs
 * 	Re-merge and incorporate Reno COMPAT_43.
 * 	Remove "notyet" around file funnels.
 * 	[90/08/19  19:34:39  tmt]
 * 
 * 	Changes for new system call interface.
 * 	Changes for u_file_state and getf.
 * 	[90/08/17  17:40:17  nags]
 * 
 * 	just a few new comments for MP changes
 * 	[90/08/14  15:19:41  hosking]
 * 
 * Revision 1.11  90/07/27  08:45:18  devrcs
 * 	Update to BSD Reno release. Major restructuring, but mostly equivalent.
 * 	Pass timeout to sosleep. Streamline syscall error return.
 * 	[90/07/20  12:45:40  tmt]
 * 
 * Revision 1.10  90/07/17  11:20:19  devrcs
 * 	Include security header files.
 * 	[90/07/10  21:53:19  seiden]
 * 
 * 	Uniprocessor compatibility using DOMAIN_FUNNEL().
 * 	[90/07/03  18:44:58  tmt]
 * 
 * Revision 1.8  90/06/22  20:07:58  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:55:01  seiden]
 * 
 * 	Call issig() before checking sigmasks, since sosleep
 * 	cannot (does not) do so, so we would find a zero mask.
 * 	[90/06/12  18:03:46  tmt]
 * 
 * 	Fix broken locking sequence when rejecting secure connection.
 * 	Rearrange code and #ifdefs slightly.
 * 	[90/06/11  16:36:59  tmt]
 * 
 * 	nags merge
 * 	[90/06/12  21:18:28  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:41:44  seiden]
 * 
 * 	Take unix_master around signals. Restore COMPAT_43 endian check.
 * 	Set SS_PIPE bits on pipe sockets.
 * 	[90/06/06  14:14:32  tmt]
 * 
 * Revision 1.7  90/04/27  18:53:50  devrcs
 * 	Modify callers of sosleep, sosbwait, sosblock because these routines
 * 	no longer longjmp on interrupted sleeps. Finally get COMPAT_43 right.
 * 	[90/04/20  12:21:05  tmt]
 * 
 * Revision 1.6  90/04/14  00:30:37  devrcs
 * 	Fix bug in getsock() due to use of GETF().
 * 	Close locking hole in accept and add assertion to check.
 * 	Use soconnect2() in pipe() to remove dependency.
 * 	[90/04/05  10:06:19  tmt]
 * 
 * Revision 1.5  90/03/27  13:15:28  gm
 * 	Filesystem parallelization changes [noemi]
 * 	Remove a setjmp because sosleep now helps out.
 * 	Add comments near questionable code - TBD.
 * 	[90/03/14  10:49:11  tmt]
 * 
 * Revision 1.4  90/01/18  08:42:43  gm
 * 	Cast args to usrreq in {sock,peer}addr.
 * 	[90/01/08  15:39:58  tmt]
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
 * Revision 1.3  90/01/03  11:52:28  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:28:18  gm]
 * 
 * Revision 1.2  89/12/26  09:24:24  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/* @(#)uipc_syscalls.c	2.1 16:10:53 4/20/90 SecureWare */
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
 * Copyright (c) 1982, 1986, 1989, 1990 Regents of the University of California.
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
 *	Base:	%W% (Berkeley) %G%
 *	Merged: uipc_syscalls.c	7.20 (Berkeley) 6/30/90
 */

#include <mach_afs_30.h>
#include "net/net_globals.h"
#if	MACH
#include <sys/secdefines.h>
#endif

#include "sys/param.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/file.h"

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

#if	MACH
#include "kern/parallel.h"

#if	SEC_ARCH
#include <sys/security.h>
#endif
#endif

LOCK_ASSERTL_DECL

#if	defined(COMPAT_43) && defined(OSF1_SERVER)
#include <machine/endian.h>
#endif

#if	defined(COMPAT_43) && !defined(BYTE_ORDER)
/*#error*/ BYTE_ORDER not defined - necessary for COMPAT_43 networking
#endif

/*
 * System call interface to the socket abstraction.
 */

#ifdef TNC
extern	struct fileops vsocketops;
#endif
extern	struct fileops	socketops;

/* ARGSUSED */
socket(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	register struct args {
		int		domain;
		int		type;
		int		protocol;
		struct	file 	**fpp;
	} *uap = (struct args *) args;
	struct socket *so;
	register struct file *fp;
	int error;
#ifdef	TNC
	struct uthread *uth = current_thread();
#endif

#ifdef	TNC
	/*
	 *  Only the server may use non-zero AF_UNIX protocols.
	 *  If the user specifies one, pretend it's not available.
	 */
	if (uap->domain == AF_UNIX && uap->type == SOCK_STREAM)
		if (uap->protocol != 0) {
			return (EPROTONOSUPPORT);
		} else {
			uap->protocol = VOS_SOCK;
		}
#endif
	if (error = falloc(uap->fpp))
		return (error);
	fp = *(uap->fpp);
	FP_LOCK(fp);
	fp->f_flag = FREAD|FWRITE;
	fp->f_type = DTYPE_SOCKET;
#ifdef TNC
	fp->f_ops = (uap->domain == AF_UNIX ? &socketops : &vsocketops);
	uth->uu_opn_filep = (mach_port_t)fp;
#else
	fp->f_ops = &socketops;
#endif
#if	SER_COMPAT
	fp->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(fp);
	if (error = SOCREATE(uap->domain, &so, uap->type, uap->protocol)) {
		fdealloc(fp);
	} else {
		FP_LOCK(fp);
		fp->f_data = (caddr_t)so;
		ASSERT(fp->f_count == 1);
		FP_UNLOCK(fp);
	}
	return (error);
}

/* ARGSUSED */
bind(fp, args, retval)
	register struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	name;
		int	namelen;
	} *uap = (struct args *) args;
	struct mbuf *nam;
	int error;

	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	error = sockargs(&nam, uap->name, uap->namelen, MT_SONAME);
	if (error == 0) {
#if	SEC_BASE
		/*
		 * Save the bound address for auditing.
		 * MP note: This is pendable.
		 */
		audstub_pathname(mtod(nam, caddr_t), nam->m_len);
#endif
		error = VSOP_BIND((struct socket *)fp->f_data, nam);
		m_freem(nam);
	}
	FP_UNREF(fp);
	return (error);
}

/* ARGSUSED */
listen(fp, args, retval)
	register struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		int	backlog;
	} *uap = (struct args *) args;
	int error;

	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	error = VSOP_LISTEN((struct socket *)fp->f_data, uap->backlog);
	FP_UNREF(fp);
	return (error);
}

#ifdef COMPAT_43
accept(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	return (accept1(fp, args, retval, 0));
}

oaccept(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	return (accept1(fp, args, retval, 1));
}
#else /* COMPAT_43 */

#define accept1(p, args, retval, compat_43)	accept(p, args, retval) 
#endif

/* ARGSUSED */
accept1(fp, args, retval, compat_43)
	register struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t		name;
		int		*anamelen;
		struct file	**fpp;
	} *uap = (struct args *) args;
	struct file *nfp = NULL;
	struct mbuf *nam = 0;
	int namelen, error;
	register struct socket *so;
	struct socket *aso;
#ifdef	TNC
	struct uthread *uth = current_thread();
#endif
	DOMAIN_FUNNEL_DECL(f)

	if (uap->name && (error = copyin((caddr_t)uap->anamelen,
	    (caddr_t)&namelen, sizeof (namelen))))
		return (error);
	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	so = (struct socket *)fp->f_data;
again:
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	if ((so->so_options & SO_ACCEPTCONN) == 0) {
		error = EINVAL;
		goto out;
	}
	if ((so->so_state & SS_NBIO) && so->so_qlen == 0) {
		error = EWOULDBLOCK;
		goto out;
	}
	while (so->so_qlen == 0 && so->so_error == 0) {
		if (so->so_state & SS_CANTRCVMORE) {
			so->so_error = ECONNABORTED;
			break;
		}
		if (error = VSOP_SLEEP(so, (caddr_t)&so->so_timeo,
		    (PZERO+1) | PCATCH, 0))
			goto out;
	}
	if (so->so_error) {
		error = so->so_error;
		so->so_error = 0;
		goto out;
	}
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);

	if (nfp == NULL) {
		if (error = falloc(uap->fpp))
			goto out2;
		nfp = *(uap->fpp);
#ifdef	TNC
		uth->uu_opn_filep = (mach_port_t)nfp;
#endif
	}

	/*
	 * We used to do various things BEFORE dequeuing the new socket
	 * from the head, but it creates several race conditions. It is
	 * necessary to get hold of the new socket first. The only thing
	 * that will behave differently is a failure of falloc. Previously
	 * the socket would not be lost.
	 */
	if (error = VSOP_DEQUEUE(so, &aso, &nam, compat_43)) {
		/* We may have lost the connection when we unlocked "so". */
		if (error == ENOTCONN)
			goto again;
		fdealloc(nfp);
		goto out2;
	}

#if	SEC_ARCH
	/*
	 * Perform an access check between the prototype socket and
	 * the new one.  The completely obvious choice of SP_IOCTLACC is
	 * to force a MAC-only equality check.  We should probably
	 * define a new access type with equivalent semantics and
	 * more mnemonic value. The audit hook is used to store
	 * the new socket tags (assuming the new socket and the
	 * prototype will be at the same levels.
	 *
	 * It is not necessary to lock the sockets simply to peek at
	 * the (constant) so_tag's.
	 */
	audstub_levels(aso->so_tag);
	if (SP_ACCESS(so->so_tag, aso->so_tag, SP_IOCTLACC, NULL))
		error = u.u_error;	/* Sigh */

	if (error) {
		(void) soabort(aso);
		(void) VSOP_CLOSE(aso);
		goto out2;
	}
#endif	/* SEC_ARCH */

	FP_LOCK(nfp);
	ASSERT(nfp->f_count == 1 && nfp->f_type == DTYPE_RESERVED);
	nfp->f_type = DTYPE_SOCKET;
	nfp->f_flag = FREAD|FWRITE;
#ifdef TNC
	nfp->f_ops = (sodomain(so)->dom_family == AF_UNIX
		      ? &socketops
		      : &vsocketops);
#else
	nfp->f_ops = &socketops;
#endif
	nfp->f_data = (caddr_t)aso;
#if	SER_COMPAT
	nfp->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(nfp);
#if	SEC_BASE
	/*
	 * Save the client address for auditing
	 * MP note: This is pendable.
	 */
	audstub_pathname(mtod(nam, caddr_t), nam->m_len);
#endif
	if (uap->name) {
		if (namelen > nam->m_len)
			namelen = nam->m_len;
		/* SHOULD COPY OUT A CHAIN HERE */
		if ((error = copyout(mtod(nam, caddr_t), (caddr_t)uap->name,
		    (u_int)namelen)) == 0)
			error = copyout((caddr_t)&namelen,
			    (caddr_t)uap->anamelen, sizeof (*uap->anamelen));
	}
	m_freem(nam);
	FP_UNREF(fp);
	return (error);

out:
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
out2:
	FP_UNREF(fp);
	if (nam)
		m_freem(nam);
	return (error);
}

/* ARGSUSED */
connect(fp, args, retval)
	register struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	name;
		int	namelen;
	} *uap = (struct args *) args;
	register struct socket *so;
	struct mbuf *nam;
	int error;
	DOMAIN_FUNNEL_DECL(f)

	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	so = (struct socket *)fp->f_data;
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		FP_UNREF(fp);
		return (EALREADY);
	}
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);	/* XXX suboptimal? */
	error = sockargs(&nam, uap->name, uap->namelen, MT_SONAME);
	if (error) {
		FP_UNREF(fp);
		return (error);
	}
#if	SEC_BASE
	/*
	 * Save the target address for auditing.
	 * MP note: This is pendable.
	 */
	audstub_pathname(mtod(nam, caddr_t), nam->m_len);
#endif
	error = VSOP_CONNECT(so, nam);
	DOMAIN_FUNNEL(sodomain(so), f);
	SOCKET_LOCK(so);
	if (error)
		goto bad;
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
		SOCKET_UNLOCK(so);
		DOMAIN_UNFUNNEL(f);
		FP_UNREF(fp);
		m_freem(nam);
		return (EINPROGRESS);
	}
	while ((so->so_state & SS_ISCONNECTING) && so->so_error == 0)
		if (error = VSOP_SLEEP(so, (caddr_t)&so->so_timeo,
		    (PZERO+1) | PCATCH, 0))
			goto bad;
	if (error == 0) {
		error = so->so_error;
		so->so_error = 0;
	}
bad:
	so->so_state &= ~SS_ISCONNECTING;
	SOCKET_UNLOCK(so);
	DOMAIN_UNFUNNEL(f);
	FP_UNREF(fp);
	m_freem(nam);
	if (error == ERESTART)
		error = EINTR;
	return (error);
}

/* ARGSUSED */
socketpair(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	register struct args {
		int		domain;
		int		type;
		int		protocol;
		struct file 	**fpp1;
		struct file 	**fpp2;
	} *uap = (struct args *) args;
	struct file *fp1, *fp2;
	struct socket *so1, *so2;
	int fd, error, sv[2];
#ifdef	TNC
	struct uthread *uth = current_thread();
#endif

#ifdef	TNC
	/*
	 *  Only the server may use non-zero AF_UNIX protocols.
	 *  If the user specifies one, pretend it's not available.
	 */
	if (uap->domain == AF_UNIX && uap->type == SOCK_STREAM)
		if (uap->protocol != 0) {
			return (EPROTONOSUPPORT);
		} else {
			uap->protocol = VOS_PIPE;
		}
#endif
	if (error = falloc(uap->fpp1))
		return (error);
	fp1 = *(uap->fpp1);
	sv[0] = fd;
	FP_LOCK(fp1);
	fp1->f_flag = FREAD|FWRITE;
	fp1->f_type = DTYPE_SOCKET;
#ifdef TNC
	fp1->f_ops = (uap->domain == AF_UNIX ? &socketops : &vsocketops);
	uth->uu_opn_filep = (mach_port_t)fp1;
#else
	fp1->f_ops = &socketops;
#endif
#if	SER_COMPAT
	fp1->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(fp1);
	if (error = SOCREATE(uap->domain, &so1, uap->type, uap->protocol))
		goto free1;
	FP_LOCK(fp1);
	fp1->f_data = (caddr_t)so1;
	FP_UNLOCK(fp1);

	if (error = falloc(uap->fpp2))
		goto free2;
	fp2 = *(uap->fpp2);
	FP_LOCK(fp2);
	fp2->f_flag = FREAD|FWRITE;
	fp2->f_type = DTYPE_SOCKET;
#ifdef TNC
	fp2->f_ops = (uap->domain == AF_UNIX ? &socketops : &vsocketops);
	uth->uu_opn_filep = (mach_port_t)fp2;
#else
	fp2->f_ops = &socketops;
#endif
#if	SER_COMPAT
	fp2->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(fp2);
	sv[1] = fd;
	if (error = SOCREATE(uap->domain, &so2, uap->type, uap->protocol))
		goto free3;
	FP_LOCK(fp2);
	fp2->f_data = (caddr_t)so2;
	FP_UNLOCK(fp2);

	if (error = VSOP_CONNECT2(so1, so2))
		goto free4;
	if (uap->type == SOCK_DGRAM) {
		/*
		 * Datagram socket connection is asymmetric.
		 */
		 if (error = VSOP_CONNECT2(so2, so1))
			goto free4;
	}
	return (error);
free4:
	(void)VSOP_CLOSE(so2);
free3:
	fdealloc(fp2);
free2:
	(void)VSOP_CLOSE(so1);
free1:
	fdealloc(fp1);
	return (error);
}

/* ARGSUSED */
sendto(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	buf;
		int	len;
		int	flags;
		caddr_t	to;
		int	tolen;
	} *uap = (struct args *) args;
	struct msghdr msg;
	struct iovec aiov;

	msg.msg_name = uap->to;
	msg.msg_namelen = uap->tolen;
	msg.msg_iov = &aiov;
	msg.msg_iovlen = 1;
	msg.msg_control = 0;
#ifdef COMPAT_43
	msg.msg_flags = 0;
#endif
	aiov.iov_base = uap->buf;
	aiov.iov_len = uap->len;
	return (sendit(fp, &msg, uap->flags, retval));
}

#ifdef COMPAT_43
/* ARGSUSED */
osend(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	buf;
		int	len;
		int	flags;
	} *uap = (struct args *) args;
	struct msghdr msg;
	struct iovec aiov;

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_iov = &aiov;
	msg.msg_iovlen = 1;
	aiov.iov_base = uap->buf;
	aiov.iov_len = uap->len;
	msg.msg_control = 0;
	msg.msg_flags = 0;
	return (sendit(fp, &msg, uap->flags, retval));
}

/* ARGSUSED */
osendmsg(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	msg;
		int	flags;
	} *uap = (struct args *) args;
	struct msghdr msg;
	struct iovec aiov[MSG_MAXIOVLEN];	/* XXX use malloc */
	int error;

	if (error = copyin(uap->msg, (caddr_t)&msg, sizeof(struct omsghdr)))
		return (error);
	if ((u_int)msg.msg_iovlen >= sizeof (aiov) / sizeof (aiov[0]))
		return (EMSGSIZE);
	if (error = copyin((caddr_t)msg.msg_iov, (caddr_t)aiov,
	    (unsigned)(msg.msg_iovlen * sizeof (struct iovec))))
		return (error);
	msg.msg_flags = MSG_COMPAT;
	msg.msg_iov = aiov;
	return (sendit(fp, &msg, uap->flags, retval));
}
#endif

/* ARGSUSED */
sendmsg(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	msg;
		int	flags;
	} *uap = (struct args *) args;
	struct msghdr msg;
	struct iovec aiov[MSG_MAXIOVLEN];	/* XXX use malloc */
	int error;

	if (error = copyin(uap->msg, (caddr_t)&msg, sizeof (msg)))
		return (error);
	if ((u_int)msg.msg_iovlen >= sizeof (aiov) / sizeof (aiov[0]))
		return (EMSGSIZE);
	if (msg.msg_iovlen &&
	    (error = copyin((caddr_t)msg.msg_iov, (caddr_t)aiov,
	    (unsigned)(msg.msg_iovlen * sizeof (struct iovec)))))
		return (error);
	msg.msg_iov = aiov;
#ifdef COMPAT_43
	msg.msg_flags = 0;
#endif
	return (sendit(fp, &msg, uap->flags, retval));
}

sendit(fp, mp, flags, retsize)
	register struct file * fp;
	register struct msghdr *mp;
	int flags, *retsize;
{
	struct uio auio;
	register struct iovec *iov;
	register int i;
	struct mbuf *to, *control;
	int len, error;
	
	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
#if	SEC_BASE
	audstub_rdwr1(UIO_WRITE, s);
#endif
	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_WRITE;
	auio.uio_offset = 0;			/* XXX */
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	to = control = 0;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if (iov->iov_len < 0 || (auio.uio_resid += iov->iov_len) < 0) {
			error = EINVAL;
			goto bad;
		}
	}
	if (mp->msg_name) {
		if (error = sockargs(&to, mp->msg_name, mp->msg_namelen,
		    MT_SONAME))
			goto bad;
	}
	if (mp->msg_control) {
		if (mp->msg_controllen < sizeof(struct cmsghdr)
#ifdef COMPAT_43
		    && mp->msg_flags != MSG_COMPAT
#endif
		) {
			error = EINVAL;
			goto bad;
		}
		if (error = sockargs(&control, mp->msg_control,
		    mp->msg_controllen, MT_CONTROL))
			goto bad;
#ifdef COMPAT_43
		if (mp->msg_flags == MSG_COMPAT) {
			register struct cmsghdr *cm;

			M_PREPEND(control, sizeof(*cm), M_WAIT);
			if (control == 0) {
				error = ENOBUFS;
				goto bad;
			} else {
				cm = mtod(control, struct cmsghdr *);
				cm->cmsg_len = control->m_len;
				cm->cmsg_level = SOL_SOCKET;
				cm->cmsg_type = SCM_RIGHTS;
			}
		}
#endif
	}
#if	SEC_ARCH
	if (error = sec_internalize_rights(&control))
		goto bad;
#endif
	len = auio.uio_resid;
	if (error = VSOP_SEND((struct socket *)fp->f_data, to, &auio,
	    (struct mbuf *)0, control, flags)) {
		switch (error) {
		case ERESTART: case EWOULDBLOCK: case EINTR:
			/* See comment at sosend() */
			if (auio.uio_resid != len)
				error = 0;
			break;
		}
	}
	if (error == 0)
		*retsize = len - auio.uio_resid;
bad:
	FP_UNREF(fp);
	if (to)
		m_freem(to);
	return (error);
}

#ifdef COMPAT_43
orecvfrom(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct args {
		caddr_t	buf;
		int	len;
		int	flags;
		caddr_t	from;
		int	*fromlenaddr;
	} *uap = (struct args *) args;

	uap->flags |= MSG_COMPAT;
	return (recvfrom(fp, args, retval));
}
#endif

/* ARGSUSED */
recvfrom(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	buf;
		int	len;
		int	flags;
		caddr_t	from;
		int	*fromlenaddr;
	} *uap = (struct args *) args;
	struct msghdr msg;
	struct iovec aiov;
	int error;

	if (uap->fromlenaddr) {
		if (error = copyin((caddr_t)uap->fromlenaddr,
		    (caddr_t)&msg.msg_namelen, sizeof (msg.msg_namelen)))
			return (error);
	} else
		msg.msg_namelen = 0;
	msg.msg_name = uap->from;
	msg.msg_iov = &aiov;
	msg.msg_iovlen = 1;
	aiov.iov_base = uap->buf;
	aiov.iov_len = uap->len;
	msg.msg_control = 0;
	msg.msg_flags = uap->flags;
	return (recvit(fp, &msg, (caddr_t)uap->fromlenaddr, retval));
}

#ifdef COMPAT_43
/* ARGSUSED */
orecv(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	buf;
		int	len;
		int	flags;
	} *uap = (struct args *) args;
	struct msghdr msg;
	struct iovec aiov;

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	msg.msg_iov = &aiov;
	msg.msg_iovlen = 1;
	aiov.iov_base = uap->buf;
	aiov.iov_len = uap->len;
	msg.msg_control = 0;
	msg.msg_flags = uap->flags;
	return (recvit(fp, &msg, (caddr_t)0, retval));
}

/*
 * Old recvmsg.  This code takes advantage of the fact that the old msghdr
 * overlays the new one, missing only the flags, and with the (old) access
 * rights where the control fields are now.
 */
/* ARGSUSED */
orecvmsg(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		struct	omsghdr *msg;
		int	flags;
	} *uap = (struct args *) args;
	struct msghdr msg;
	struct iovec aiov[MSG_MAXIOVLEN];	/* XXX use malloc */
	int error;

	if (error = copyin((caddr_t)uap->msg, (caddr_t)&msg,
	    sizeof (struct omsghdr)))
		return (error);
	if ((u_int)msg.msg_iovlen >= sizeof (aiov) / sizeof (aiov[0]))
		return (EMSGSIZE);
	msg.msg_flags = uap->flags | MSG_COMPAT;
	if (error = copyin((caddr_t)msg.msg_iov, (caddr_t)aiov,
	    (unsigned)(msg.msg_iovlen * sizeof (struct iovec))))
		return (error);
	msg.msg_iov = aiov;
	error = recvit(fp, &msg, (caddr_t)&uap->msg->msg_namelen, retval);

	if (msg.msg_controllen && error == 0)
		error = copyout((caddr_t)&msg.msg_controllen,
		    (caddr_t)&uap->msg->msg_accrightslen, sizeof (int));
	return (error);
}
#endif

/* ARGSUSED */
recvmsg(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		struct	msghdr *msg;
		int	flags;
	} *uap = (struct args *) args;
	struct msghdr msg;
	struct iovec aiov[MSG_MAXIOVLEN], *uiov;	/* XXX use malloc */
	register int error;

	if (error = copyin((caddr_t)uap->msg, (caddr_t)&msg, sizeof (msg)))
		return (error);
	if ((u_int)msg.msg_iovlen >= sizeof (aiov) / sizeof (aiov[0]))
		return (EMSGSIZE);
#ifdef COMPAT_43
	msg.msg_flags = uap->flags &~ MSG_COMPAT;
#else
	msg.msg_flags = uap->flags;
#endif
	uiov = msg.msg_iov;
	msg.msg_iov = aiov;
	if (error = copyin((caddr_t)uiov, (caddr_t)aiov,
	    (unsigned)(msg.msg_iovlen * sizeof (struct iovec))))
		return (error);
	if ((error = recvit(fp, &msg, (caddr_t)0, retval)) == 0) {
		msg.msg_iov = uiov;
		error = copyout((caddr_t)&msg, (caddr_t)uap->msg, sizeof(msg));
	}
	return (error);
}

recvit(fp, mp, namelenp, retsize)
	struct file * fp;
	register struct msghdr *mp;
	caddr_t namelenp;
	int *retsize;
{
	struct uio auio;
	register struct iovec *iov;
	register int i;
	int len, error;
	struct mbuf *from = 0, *control = 0;
	
	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
#if	SEC_BASE
	audstub_rdwr1(UIO_READ, s);
#endif
	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_offset = 0;			/* XXX */
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if (iov->iov_len < 0 || (auio.uio_resid += iov->iov_len) < 0) {
			FP_UNREF(fp);
			return (EINVAL);
		}
	}
	len = auio.uio_resid;
	if (error = VSOP_RECEIVE((struct socket *)fp->f_data, &from, &auio,
	    (struct mbuf **)0, &control, &mp->msg_flags)) {
		switch (error) {
		case ERESTART: case EWOULDBLOCK: case EINTR:
			/* See comment at soreceive() */
			if (auio.uio_resid != len)
				error = 0;
			break;
		}
	}
	if (error)
		goto out;
	*retsize = len - auio.uio_resid;
	if (mp->msg_name) {
		len = mp->msg_namelen;
		if (len <= 0 || from == 0)
			len = 0;
		else {
#ifdef COMPAT_43
			if (mp->msg_flags & MSG_COMPAT)
				mtod(from, struct osockaddr *)->sa_family =
				    mtod(from, struct sockaddr *)->sa_family;
#endif
			if (len > from->m_len)
				len = from->m_len;
			/* else if len < from->m_len ??? */
			if (error = copyout(mtod(from, caddr_t),
			    (caddr_t)mp->msg_name, (unsigned)len))
				goto out;
		}
		mp->msg_namelen = len;
		if (namelenp &&
		    (error = copyout((caddr_t)&len, namelenp, sizeof (int)))) {
#ifdef COMPAT_43
			if (mp->msg_flags & MSG_COMPAT)
				error = 0;	/* old recvfrom didn't check */
			else
#endif
			goto out;
		}
	}
	if (mp->msg_control) {
#if	SEC_ARCH
		sec_externalize_rights(&control, (struct socket *) fp->f_data);
#endif
#ifdef	COMPAT_43
		/*
		 * We assume that old recvmsg calls won't receive access
		 * rights and other control info, esp. as control info
		 * is always optional and those options didn't exist in 4.3.
		 * If we receive rights, trim the cmsghdr; anything else
		 * is tossed.
		 */
		if (control && mp->msg_flags & MSG_COMPAT) {
			if (mtod(control, struct cmsghdr *)->cmsg_level !=
			    SOL_SOCKET ||
			    mtod(control, struct cmsghdr *)->cmsg_type !=
			    SCM_RIGHTS) {
				mp->msg_controllen = 0;
				goto out;
			}
			control->m_len -= sizeof (struct cmsghdr);
			control->m_data += sizeof (struct cmsghdr);
		}
#endif
		len = mp->msg_controllen;
		if (len <= 0 || control == 0)
			len = 0;
		else {
			if (len >= control->m_len)
				len = control->m_len;
			else
				mp->msg_flags |= MSG_CTRUNC;
			error = copyout((caddr_t)mtod(control, caddr_t),
			    (caddr_t)mp->msg_control, (unsigned)len);
		}
		mp->msg_controllen = len;
	}
out:
	FP_UNREF(fp);
	if (from)
		m_freem(from);
	if (control)
		m_freem(control);
	return (error);
}

/* ARGSUSED */
shutdown(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		int	how;
	} *uap = (struct args *) args;
	int error;

	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	/*
	 * Decrementing fp's reference count can result in a call to
	 * soclose when racing another thread calling close(uap->s).
	 * Defer possibility until after calling soshutdown.
	 */
	error = VSOP_SHUTDOWN((struct socket *)fp->f_data, uap->how);
	FP_UNREF(fp);
	return (error);
}

/* ARGSUSED */
setsockopt(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		int	level;
		int	name;
		caddr_t	val;
		int	valsize;
	} *uap = (struct args *) args;
	struct mbuf *m = NULL;
	int error;

	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	if (uap->valsize > MLEN) {
		FP_UNREF(fp);
		return (EINVAL);
	}
	if (uap->val) {
		m = m_get(M_WAIT, MT_SOOPTS);
		if (m == NULL) {
			FP_UNREF(fp);
			return (ENOBUFS);
		}
		if (error = copyin(uap->val, mtod(m, caddr_t),
		    (u_int)uap->valsize)) {
			(void) m_free(m);
			FP_UNREF(fp);
			return (error);
		}
		m->m_len = uap->valsize;
	}
	error = VSOP_SETOPT((struct socket *)fp->f_data, uap->level, uap->name, m);
	FP_UNREF(fp);
	return (error);
}

/* ARGSUSED */
getsockopt(fp, args, retval)
	struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		int	level;
		int	name;
		caddr_t	val;
		int	*avalsize;
	} *uap = (struct args *) args;
	struct mbuf *m = NULL;
	int valsize, error;

	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	if (uap->val) {
		if (error = copyin((caddr_t)uap->avalsize, (caddr_t)&valsize,
		    sizeof (valsize))) {
			FP_UNREF(fp);
			return (error);
		}
	} else
		valsize = 0;
	if ((error = VSOP_GETOPT((struct socket *)fp->f_data, uap->level,
	    uap->name, &m)) == 0 && uap->val && valsize && m != NULL) {
		if (valsize > m->m_len)
			valsize = m->m_len;
		error = copyout(mtod(m, caddr_t), uap->val, (u_int)valsize);
		if (error == 0)
			error = copyout((caddr_t)&valsize,
			    (caddr_t)uap->avalsize, sizeof (valsize));
	}
	FP_UNREF(fp);
	if (m)
		m_freem(m);
	return (error);
}


/* ARGSUSED */
pipe(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		struct file	**fpp1;
		struct file	**fpp2;
	} *uap = (struct args *) args;
	struct file *rf, *wf;
	struct socket *rso, *wso;
	int error;
#ifdef	TNC
	struct uthread *uth = current_thread();
#define PROTO	VOS_PIPE
#else
#define PROTO	0
#endif

	if (error = falloc(uap->fpp1))
		return (error);
	rf = *(uap->fpp1);
	FP_LOCK(rf);
	rf->f_flag = FREAD;
	rf->f_type = DTYPE_SOCKET;
	rf->f_ops = &socketops;
#ifdef TNC
	uth->uu_opn_filep = (mach_port_t)rf;
#endif
#if	SER_COMPAT
	rf->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(rf);
	if (error = SOCREATE(AF_UNIX, &rso, SOCK_STREAM, PROTO))
		goto free1;
	FP_LOCK(rf);
	rf->f_data = (caddr_t)rso;
	FP_UNLOCK(rf);

	if (error = falloc(uap->fpp2))
		goto free2;
	wf = *(uap->fpp2);
	FP_LOCK(wf);
	wf->f_flag = FWRITE;
	wf->f_type = DTYPE_SOCKET;
	wf->f_ops = &socketops;
#ifdef TNC
	uth->uu_opn_filep = (mach_port_t)wf;
#endif
#if	SER_COMPAT
	wf->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(wf);
	if (error = SOCREATE(AF_UNIX, &wso, SOCK_STREAM, PROTO))
		goto free3;
	FP_LOCK(wf);
	wf->f_data = (caddr_t)wso;
	FP_UNLOCK(wf);

	/* Need write atomicity and pipe times behavior */
	wso->so_special |= (SP_WATOMIC|SP_PIPE);
	rso->so_special |= SP_PIPE;

	if (error = VSOP_CONNECT2(wso, rso))
		goto free4;
	wso->so_state |= SS_CANTRCVMORE;
	rso->so_state |= SS_CANTSENDMORE;
	return (0);
free4:
	(void)VSOP_CLOSE(wso);
free3:
	fdealloc(wf);
free2:
	(void)VSOP_CLOSE(rso);
free1:
	fdealloc(rf);
	return (error);
#undef PROTO
}

/*
 * Get socket name.
 */
#ifdef COMPAT_43
getsockname(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	return (getsockname1(fp, args, retval, 0));
}

ogetsockname(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	return (getsockname1(fp, args, retval, 1));
}
#else /* COMPAT_43 */

#define getsockname1(p, args, retval, compat_43) getsockname(p, args, retval)
#endif

/* ARGSUSED */
getsockname1(fp, args, retval, compat_43)
	register struct file *fp;
	void *args;	
	int *retval;
{
	register struct args {
		caddr_t	asa;
		int	*alen;
	} *uap = (struct args *) args;
	register struct socket *so;
	struct mbuf *m;
	int len, error;

	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	if (error = copyin((caddr_t)uap->alen, (caddr_t)&len, sizeof (len))) {
		FP_UNREF(fp);
		return (error);
	}
	so = (struct socket *)fp->f_data;
#ifdef COMPAT_43
	error = VSOP_GETADDR(so, &m, FALSE, compat_43);
#else	
	error = VSOP_GETADDR(so, &m, FALSE, TRUE);
#endif
	if(error)
		goto bad;
	if (len > m->m_len)
		len = m->m_len;
	error = copyout(mtod(m, caddr_t), (caddr_t)uap->asa, (u_int)len);
	if (error == 0)
		error = copyout((caddr_t)&len, (caddr_t)uap->alen,
		    sizeof (len));
	m_freem(m);
bad:
	FP_UNREF(fp);
	return (error);
}

/*
 * Get name of peer for connected socket.
 */
#ifdef COMPAT_43
getpeername(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
#if	MACH_AFS_30
	int rc;

	rc = getpeername1(fp, args, retval, 0);
	if (rc)
		rc = afs_check_nosys(rc);
	return(rc);
#else	/* MACH_AFS_30 */
	return (getpeername1(fp, args, retval, 0));
#endif	/* MACH_AFS_30 */
}

ogetpeername(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	return (getpeername1(fp, args, retval, 1));
}
#else /* COMPAT_43 */

#define getpeername1(fp, args, retval, compat_43) getpeername(fp, args, retval)
#endif

/* ARGSUSED */
getpeername1(fp, args, retval, compat_43)
	struct file *fp;
	void *args;	
	int *retval;
{
	struct args {
		caddr_t	asa;
		int	*alen;
	} *uap = (struct args *) args;
	register struct socket *so;
	struct mbuf *m;
	int len, error;

	fp = getsock(fp, &error);
	if (fp == 0)
		return (error);
	if (error = copyin((caddr_t)uap->alen, (caddr_t)&len, sizeof (len)))
		goto bad;
	so = (struct socket *)fp->f_data;
#ifdef COMPAT_43
	error = VSOP_GETADDR(so, &m, TRUE, compat_43);
#else
	error = VSOP_GETADDR(so, &m, TRUE, TRUE);
#endif
	if (error)
		goto bad;
	if (len > m->m_len)
		len = m->m_len;
	if (error = copyout(mtod(m, caddr_t), (caddr_t)uap->asa, (u_int)len))
		goto bad;
	error = copyout((caddr_t)&len, (caddr_t)uap->alen, sizeof (len));
bad:
	FP_UNREF(fp);
	if(m)
		m_freem(m);
	return (error);
}

void
sockaddr_new(m)
	struct mbuf *m;
{
	if (m->m_type == MT_SONAME) {
		register struct sockaddr *sa = mtod(m, struct sockaddr *);
#if	defined(COMPAT_43) && BYTE_ORDER != BIG_ENDIAN
		if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
			sa->sa_family = sa->sa_len;
#endif
		sa->sa_len = m->m_len;
	}
}

void
sockaddr_old(m)
	struct mbuf *m;
{
#ifdef	COMPAT_43
	if (m->m_type == MT_SONAME) {
		mtod(m, struct osockaddr *)->sa_family =
		    mtod(m, struct sockaddr *)->sa_family;
	}
#endif
}

sockargs(mp, buf, buflen, type)
	struct mbuf **mp;
	caddr_t buf;
	int buflen, type;
{
	register struct mbuf *m;
	int error;

	if ((u_int)buflen > MLEN) {
#ifdef COMPAT_43
		if (type == MT_SONAME && (u_int)buflen <= 112)
			buflen = MLEN;		/* unix domain compat. hack */
		else
#endif
		return (EINVAL);
	}
	m = m_getclr(M_WAIT, type);
	if (m == NULL)
		return (ENOBUFS);
	m->m_len = buflen;
	if (error = copyin(buf, mtod(m, caddr_t), (u_int)buflen))
		(void) m_free(m);
	else
		sockaddr_new(*mp = m);
	return (error);
}

/*
 * Get socket from file struct.
 * Return an unlocked, referenced file structure.  The calling
 * routine must delete the reference when it is done with the
 * file structure. Note doesn't use GETF because of return value.
 */
struct file *
getsock(fp, errp)
	struct file * fp;
	int  *errp;
{
#if	MACH
	if (fp == U_FD_RESERVED)
		fp = NULL;
#endif
	if (fp == NULL) {
		*errp = EBADF;
	} else if (fp->f_type != DTYPE_SOCKET) {
		fp = NULL;
		*errp = ENOTSOCK;
	} else
		FP_REF(fp);
	return (fp);
}
