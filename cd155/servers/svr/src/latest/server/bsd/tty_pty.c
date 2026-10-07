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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: tty_pty.c,v $
 * Revision 1.6  1995/02/01  21:29:45  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:28:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:50:02  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:50:47  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:06:58  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:36  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.11  1993/04/24  18:47:46  klh
 * 	Revision 2.10  93/01/08  14:28:38  durriya
 * 		add ndoe # as arg to ptsread, write, close, ptcread,write,close
 * 		ptcselect and ptyioctl                                (durriya)
 *
 * Revision 2.10  92/09/28  16:36:54  roman
 * Change calls to VPOP_SIGPGRP() with VPROC_TERMINAL_ACCESS to use the
 * 	new vproc op VPOP_TERMINAL_SIGPGRP().
 * 
 * Revision 2.9  92/06/05  13:56:34  klh
 * 	Revision 2.8  92/05/12  00:06:51  loverso
 * 		Changes ala OSF/1 1.1 to only call select_enqueue() once, and to
 * 		call select_dequeue() faster.
 * 
 * Revision 2.8  92/04/14  10:05:26  roman
 * Add #ifndef's for TNC around disallowing tty operations on non root fs nodes.
 * 
 * Revision 2.7  92/04/06  19:07:34  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.6  92/04/05  16:48:47  pjg
 * 	Don't allow tty code to execute outside the root FS (rabii).
 * 
 * 	Remove explicit use of the PPROC_CTTY_SETUP() macro which is now 
 * 	invoked with isctty(). (chrisp)
 * 
 * Revision 2.5  91/11/14  14:44:17  chrisp
 * Corrections to controlling terminal handling in the dummy process
 * environment of the fileserver.
 * 
 * Revision 2.4  91/10/04  14:53:58  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:49:27  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Change to use vproc operations when handling controlling tty.
 * 	Also, process group and session structures replaced by ids.
 * 
 * Revision 2.2  91/08/31  13:23:52  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/05  13:56:13  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.6.2  91/03/16  15:27:47  morris
 * 	Fix from gmf: Don't allow open of master is slave is busy.
 * 	[91/03/16  15:10:45  morris]
 * 
 * Revision 1.13  90/10/31  13:50:09  devrcs
 * 	interrupted ttysleep calls must unlock lock
 * 	[90/10/23  15:29:44  gmf]
 * 
 * 	Seperate NDELAY and NONBLOCK.
 * 	[90/10/12  10:17:13  jvs]
 * 
 * Revision 1.12  90/10/07  13:20:09  devrcs
 * 	Move TSPLTTY and friends to tty.h.
 * 	[90/09/28  20:17:06  brezak]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:02:12  gm]
 * 
 * Revision 1.11  90/08/24  11:19:25  devrcs
 * 	Eliminate dependencies on setjmp/longjmp.
 * 	Change TTY_SLEEP to use ttysleep, which can return interruptions
 * 	synchronously.
 * 	Upgraded to 4.3 reno code in some areas.
 * 	[90/08/20  01:07:07  gmf]
 * 
 * 	Changes for new system call interface (new pgsignal interface).
 * 	[90/08/17  17:40:06  nags]
 * 
 * 	Line discipline open routine now takes a third argument.
 * 	[90/08/12  12:16:14  ers]
 * 
 * Revision 1.10  90/08/09  13:15:42  devrcs
 * 	Delete ttyloc code.
 * 	[90/08/01  10:46:03  tmt]
 * 
 * 	Added support for Kanji Shift JIS discipline.
 * 	[90/07/26  13:38:32  morris]
 * 
 * Revision 1.9  90/07/17  11:20:07  devrcs
 * 	Use symbolic in SP_* calls.
 * 	[90/07/10  21:53:03  seiden]
 * 
 * Revision 1.8  90/06/22  20:07:28  devrcs
 * 	Removed the JUNK_ ioctls.
 * 	[90/06/14  23:00:59  havens]
 * 
 * 	nags merge
 * 
 * 	Condensed relevant ancient history (reverse chronology):
 * 	SecureWare changes.				seiden@osf.org
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	New select interface				coren@osf.org
 * 	Removed call to ttydetach			noemi@osf.org
 * 	Change to use pgrp structure.			coren@osf.org
 * 	Removed include of <sys/dir.h>			gmf@osf.org
 * 	Posix tty support.				morris@osf.org
 * 	Merged Mach 2.5 and Encore parallelization	alan@encore.com
 * 	Fixed ptyioctl for Suntools and X on Suns	mikeg@cmu.edu
 * 	Enclosed changes for Suntools under #ifdef sun	mikeg@cmu.edu
 * 	Added support for Sun's ioctls in ptyioctl	mikeg@cmu.edu
 * 	Clear tp->t_winsize in ptsopen			boykin@encore.com
 * 	Code clean-up, new TTY_SLEEP(), lint.		alan@encore.com
 * 	ptcselect dequeued elements from wrong queue.	alan@encore.com
 * 	Revised select scheme to remove race condition	alan@encore.com
 * 	Added tty locking, non-master-based tty sleeps.	alan@encore.com
 * 	Select routines check whether thread is waiting	dbg@cmu.edu
 * 	Adjusted to also accept new general TIOCSLOC 	mja@cmu.edu
 * 	Support for multiple threads.			avie@cmu.edu
 * 	cp TS_ONDELAY from control to slave in ptsopen	rvb@cmu.edu
 * 	Upgraded to 4.3.				avie@cmu.edu
 * 	Expanded NPTY to (64+16).			mja@cmu.edu
 * 	[90/06/12  21:17:21  gmf]
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
 * Copyright (c) 1982, 1986, 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)tty_pty.c	7.9 (Berkeley) 11/20/89
 */

/*
 * Pseudo-teletype Driver
 * (Actually two drivers, requiring two entries in 'cdevsw')
 */

#include <pty.h>
#include <sys/secdefines.h>

#if	NPTY > 0

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/user.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/lock_types.h>
#include <kern/assert.h>
#include <kern/parallel.h>
#if	SEC_BASE
#include <sys/security.h>
#endif
#if	SEC_ARCH
#include <sys/secpolicy.h>
#endif
#include <kji.h>

#if	NPTY == 1
#undef	NPTY
#define NPTY	(64+16)		/* crude XXX */
#endif

#define BUFSIZ 100		/* Chunk size iomoved to/from user */

#ifndef	TNC
extern	node_t		this_node;
extern	node_t		root_fs_node;
#endif	/* TNC */
/*
 * pts == /dev/tty[pqrs]?
 * ptc == /dev/pty[pqrs]?
 */
struct	tty pt_tty[NPTY];
struct	pt_ioctl {
	int	pt_flags;
	queue_head_t	pt_selq;
	u_char	pt_send;
	u_char	pt_ucntl;
} pt_ioctl[NPTY];
int	npty = NPTY;		/* for pstat -t */

#if	SEC_ARCH
/*
 * Allocate space for pty tag pools.  On systems that allocate the pty
 * structures dynamically, the tag pools should also be dynamically
 * allocated at the same time as the ptys.
 */
tag_t	ptctag[NPTY * SEC_TAG_COUNT];
tag_t	ptstag[NPTY * SEC_TAG_COUNT];
#endif
int ptydebug = 0;

#define PF_NBIO		0x04
#define PF_PKT		0x08		/* packet mode */
#define PF_STOPPED	0x10		/* user told stopped */
#define PF_REMOTE	0x20		/* remote and flow controlled input */
#define PF_NOSTOP	0x40
#define PF_UCNTL	0x80		/* user control mode */

/*ARGSUSED*/
ptsopen(dev, flag)
	dev_t dev;
{
	register struct tty *tp;
	int error;
#if	SEC_ARCH
	int mode;
#endif

#ifdef	lint
	npty = npty;
#endif
#ifndef	TNC
	/* this code is only allowed on the root fs */
	if (this_node != root_fs_node) {
		return (EACCES);
	}
#endif	/* TNC */
	if (minor(dev) >= NPTY)
		return (ENXIO);
	tp = &pt_tty[minor(dev)];
	TTY_LOCK(tp);
	if ((tp->t_state & TS_ISOPEN) == 0) {
		ttychars(tp);		/* Set up default chars */
		bzero((caddr_t)&tp->t_winsize, sizeof(tp->t_winsize));
		tp->t_iflag = TTYDEF_IFLAG;
		tp->t_oflag = TTYDEF_OFLAG;
		tp->t_lflag = TTYDEF_LFLAG;
		tp->t_cflag = TTYDEF_CFLAG;
		tp->t_ispeed = tp->t_ospeed = TTYDEF_SPEED;
		ttsetwater(tp);		/* would be done in xxparam() */
	} else if (tp->t_state&TS_XCLUDE
#if	SEC_BASE
		   && !privileged(SEC_ALLOWDACACCESS, 0)
#else
		   && u.u_uid != 0
#endif
					) {
		TTY_UNLOCK(tp);
		return (EBUSY);
	}
	if (tp->t_oproc)			/* Ctrlr still around. */
		tp->t_state |= TS_CARR_ON;
	/* Let X work:  Don't wait if O_NDELAY is on. */
	/* was if (flag & O_NDELAY) */
	if (flag & (O_NDELAY|O_NONBLOCK)) {
		tp->t_state |= TS_ONDELAY;
	} else {
		while ((tp->t_state & TS_CARR_ON) == 0) {
			tp->t_state |= TS_WOPEN;
			/* was if (flag&FNDELAY) */
			if (flag&(FNDELAY|FNONBLOCK))
			        break;
			if (error = ttysleep(tp, (caddr_t)&tp->t_rawq, 
					     TTIPRI | PCATCH, ttopen))
				goto out;
		}
	}
#if	SEC_ARCH
	mode = 0;
	if (flag & FREAD)
		mode |= SP_READACC;
	if (flag & FWRITE)
		mode |= SP_WRITEACC;
	if (SP_ACCESS(SIP->si_tag, PTSTAG(dev, 0), mode, NULL)) {
		TTY_UNLOCK(tp);
		return u.u_error;	/* XXX */
	}
#endif
	error = (*linesw[tp->t_line].l_open)(dev, tp, flag);
	ptcwakeup(tp, FREAD|FWRITE);
out:
	TTY_UNLOCK(tp);
	return (error);
}

#ifdef OSF1_ADFS
ptsclose(dev, node)
#else
ptsclose(dev)
#endif
	dev_t dev;
#ifdef OSF1_ADFS
        node_t  node;
#endif
{
	register struct tty *tp;

	tp = &pt_tty[minor(dev)];
	TTY_LOCK(tp);
	(*linesw[tp->t_line].l_close)(tp);
	ttyclose(tp);
	ptcwakeup(tp, FREAD|FWRITE);
	TTY_UNLOCK(tp);
	return (0);
}

#ifdef OSF1_ADFS
ptsread(dev, node, uio, flag)
#else
ptsread(dev, uio, flag)
#endif
	dev_t dev;
	struct uio *uio;
#ifdef OSF1_ADFS
        node_t  node;
#endif
{
	register struct tty *tp = &pt_tty[minor(dev)];
	register struct pt_ioctl *pti = &pt_ioctl[minor(dev)];
	int error = 0;
#if	SEC_ILB
	int saved_resid = uio->uio_resid;
	tag_t ntag;
#endif

	TTY_LOCK(tp);
again:
	if (pti->pt_flags & PF_REMOTE) {
		unix_master();		/* sessions, sigh */
		while (isbackground(u.u_procp, tp)) {
			if (VPOP_TERMINAL_SIGPGRP(u.u_procp->p_vproc, SIGTTIN)){
				TTY_UNLOCK(tp);
				unix_release();
				return (EIO);
			}
			unix_release();
			if (error = ttysleep(tp, (caddr_t)&lbolt,
					TTIPRI | PCATCH, ttybg)) {
				goto out;
			}
			unix_master();
		}
		unix_release();
		if (tp->t_canq.c_cc == 0) {
			if (flag & IO_NDELAY) {
				TTY_UNLOCK(tp);
				return (EWOULDBLOCK);
			}
			if (error = ttysleep(tp, (caddr_t)&tp->t_canq,
					     TTIPRI | PCATCH, ttyin))
				goto out;
			goto again;
		}
#if	SEC_ILB
		if (saved_resid &&
		    SP_CHECK_FLOAT(UIO_READ, PTSTAG(dev,0), &ntag) {
			TTY_UNLOCK(tp);
			return (u.u_error);
		}
#endif
		while (tp->t_canq.c_cc > 1 && uio->uio_resid > 0)
			if (ureadc(getc(&tp->t_canq), uio) < 0) {
				error = EFAULT;
				break;
			}
		if (tp->t_canq.c_cc == 1)
			(void) getc(&tp->t_canq);
#if	SEC_ILB
		if (saved_resid != uio->uio_resid)
			SP_DO_FLOAT(UIO_READ, PTSTAG(dev,0), &ntag);
#endif
		if (tp->t_canq.c_cc) {
			TTY_UNLOCK(tp);
			return (error);
		}
#if	SEC_ILB
		else if (tp->t_rawq.c_cc == 0)
			SP_EMPTY_OBJECT(PTSTAG(dev,0));
#endif
	} else
		if (tp->t_oproc)
#if	SEC_ILB
		    if (saved_resid &&
			SP_CHECK_FLOAT(UIO_READ, PTSTAG(dev, 0), &ntag))
			error = u.u_error;
		    else {
			error = (*linesw[tp->t_line].l_read)(tp, uio, flag);
			if (error == 0 && saved_resid != uio->uio_resid)
				SP_DO_FLOAT(UIO_READ, PTSTAG(dev,0), &ntag);
			if (tp->t_canq.c_cc==0 && tp->t_rawq.c_cc==0)
				SP_EMPTY_OBJECT(PTSTAG(dev,0));
		    }
#else
			error = (*linesw[tp->t_line].l_read)(tp, uio, flag);
#endif
	ptcwakeup(tp, FWRITE);
out:
	TTY_UNLOCK(tp);
	return (error);
}

/*
 * Write to pseudo-tty.
 * Wakeups of controlling tty will happen
 * indirectly, when tty driver calls ptsstart.
 */
#ifdef OSF1_ADFS
ptswrite(dev, node, uio, flag)
#else
ptswrite(dev, uio, flag)
#endif
	dev_t dev;
	struct uio *uio;
#ifdef OSF1_ADFS
        node_t  node;
#endif
{
	register struct tty *tp;
	int error;
#if 	SEC_ILB
	int     saved_resid;
	tag_t   ntag;
#endif

	tp = &pt_tty[minor(dev)];
	if (tp->t_oproc == 0)
		return (EIO);
	TTY_LOCK(tp);
#if	SEC_ILB

	/*
	 * The flush ioctl commands are handled by the generic tty code
	 * and do not call back into the driver as in System V.  The check
	 * here is to determine if the output queue has been flushed.
	 */

	if (tp->t_outq.c_cc == 0)
		SP_EMPTY_OBJECT(PTCTAG(dev,0));
	saved_resid = uio->uio_resid;
	if (saved_resid && SP_CHECK_FLOAT(UIO_WRITE, PTCTAG(dev, 0), &ntag))
		error =  u.u_error;
	else {
		error = (*linesw[tp->t_line].l_write)(tp, uio, flag);
		if (saved_resid && saved_resid != uio->uio_resid)
			SP_DO_FLOAT(UIO_WRITE, PTCTAG(dev, 0), &ntag);
	}
#else
	error = (*linesw[tp->t_line].l_write)(tp, uio, flag);
#endif
	TTY_UNLOCK(tp);
	return (error);
}

/*
 * Start output on pseudo-tty.
 * Wake up process selecting or sleeping for input from controlling tty.
 */
ptsstart(tp)
	struct tty *tp;
{
	register struct pt_ioctl *pti = &pt_ioctl[minor(tp->t_dev)];

	LASSERT(TTY_LOCK_HOLDER(tp));
	if (tp->t_state & TS_TTSTOP)
		return;
	if (pti->pt_flags & PF_STOPPED) {
		pti->pt_flags &= ~PF_STOPPED;
		pti->pt_send = TIOCPKT_START;
	}
	ptcwakeup(tp, FREAD);
}

ptcwakeup(tp, flag)
	struct tty *tp;
{
	struct pt_ioctl *pti = &pt_ioctl[minor(tp->t_dev)];

	LASSERT(TTY_LOCK_HOLDER(tp));
	select_wakeup(&pti->pt_selq);
	if (flag & FREAD)
		thread_wakeup ((int)&tp->t_outq.c_cf);
	if (flag & FWRITE)
		thread_wakeup ((int)&tp->t_rawq.c_cf);
}

/*ARGSUSED*/
ptcopen(dev, flag)
	dev_t dev;
	int flag;
{
	register struct tty *tp;
	struct pt_ioctl *pti;

	if (minor(dev) >= NPTY)
		return (ENXIO);
	tp = &pt_tty[minor(dev)];
	TTY_LOCK(tp);
	/*
	 * Fail open if either the master or the slave is busy.
	 * Busy master indicated by t_oproce != 0; busy slave indicated
	 * by a state of TS_ISOPEN.  It is ok to open if the slave is
	 * open, but not yet connected.  In this case, TS_ISOPEN will be 0.
	 */
	if ((tp->t_oproc) || (tp->t_state & TS_ISOPEN)) {
		TTY_UNLOCK(tp);
		return (EIO);
	}
#if	SEC_ARCH
	bzero(PTCTAG(dev, 0), SEC_NUM_TAGS * sizeof(tag_t));
	SP_OBJECT_CREATE(SIP->si_tag, PTCTAG(dev, 0), (tag_t) 0, SEC_OBJECT);
	bzero(PTSTAG(dev, 0), SEC_NUM_TAGS * sizeof(tag_t));
	SP_OBJECT_CREATE(SIP->si_tag, PTSTAG(dev, 0), (tag_t) 0, SEC_OBJECT);
#if	SEC_ILB
	SP_EMPTY_OBJECT(PTCTAG(dev, 0));
	SP_EMPTY_OBJECT(PTSTAG(dev, 0));
#endif
#endif
	tp->t_oproc = ptsstart;
	(void)(*linesw[tp->t_line].l_modem)(tp, 1);
	pti = &pt_ioctl[minor(dev)];
	pti->pt_flags = 0;
	pti->pt_send = 0;
	pti->pt_ucntl = 0;
	TTY_UNLOCK(tp);
	return (0);
}

#ifdef OSF1_ADFS
ptcclose(dev, node)
#else
ptcclose(dev)
#endif
	dev_t dev;
#ifdef OSF1_ADFS
        node_t node;
#endif
{
	register struct tty *tp;

	tp = &pt_tty[minor(dev)];
	TTY_LOCK(tp);
	(void)(*linesw[tp->t_line].l_modem)(tp, 0);
	tp->t_state &= ~TS_CARR_ON;
	tp->t_oproc = 0;		/* mark closed */
	tp->t_sid = 0;
	TTY_UNLOCK(tp);
	return (0);
}

#ifdef OSF1_ADFS
ptcread(dev, node, uio, flag)
#else
ptcread(dev, uio, flag)
#endif
	dev_t dev;
	struct uio *uio;
#ifdef OSF1_ADFS
        node_t node;
#endif
{
	register struct tty *tp = &pt_tty[minor(dev)];
	struct pt_ioctl *pti = &pt_ioctl[minor(dev)];
	char buf[BUFSIZ];
	int error = 0, cc;
#if	SEC_ILB
	int saved_resid;
	tag_t ntag;
#endif

	TTY_LOCK(tp);
	/*
	 * We want to block until the slave
	 * is open, and there's something to read;
	 * but if we lost the slave or we're NBIO,
	 * then return the appropriate error instead.
	 */
	for (;;) {
		if (tp->t_state&TS_ISOPEN) {
			if (pti->pt_flags&PF_PKT && pti->pt_send) {
				error = ureadc((int)pti->pt_send, uio);
				if (!error)
					pti->pt_send = 0;
				goto out;
			}
			if (pti->pt_flags&PF_UCNTL && pti->pt_ucntl) {
				error = ureadc((int)pti->pt_ucntl, uio);
				if (!error)
					pti->pt_ucntl = 0;
				goto out;
			}
			if (tp->t_outq.c_cc && (tp->t_state&TS_TTSTOP) == 0)
				break;
		}
		if ((tp->t_state&TS_CARR_ON) == 0) {
			TTY_UNLOCK(tp);
			return (0);	/* EOF */
		}
		if (flag & IO_NDELAY) {
			TTY_UNLOCK(tp);
			return (EWOULDBLOCK);
		}
if (ptydebug) printf("SLEEP(1) c_cf %d\n", u.u_procp->p_pid);
		if (error = ttysleep (tp, (caddr_t)&tp->t_outq.c_cf, 
				      TTIPRI | PCATCH, ttyin))
			goto out;
	}
	if (pti->pt_flags & (PF_PKT|PF_UCNTL))
		error = ureadc(0, uio);
#if	SEC_ILB
	saved_resid = uio->uio_resid;
	if (saved_resid && SP_CHECK_FLOAT(UIO_READ, PTCTAG(dev, 0), &ntag)) {
		error = u.u_error;
		goto out;
	}
#endif
	while (uio->uio_resid > 0 && error == 0) {
		cc = q_to_b(&tp->t_outq, buf, MIN(uio->uio_resid, BUFSIZ));
		if (cc <= 0)
			break;
		uio->uio_rw = UIO_READ;
		error = uiomove(buf, cc, uio);
	}
#if	SEC_ILB
	if (saved_resid != uio->uio_resid)
		SP_DO_FLOAT(UIO_READ, PTCTAG(dev, 0), &ntag);
	if (tp->t_outq.c_cc == 0)
		SP_EMPTY_OBJECT(PTCTAG(dev, 0));
#endif
	if (tp->t_outq.c_cc <= tp->t_lowat) {
		if (tp->t_state&TS_ASLEEP) {
			tp->t_state &= ~TS_ASLEEP;
			thread_wakeup (&tp->t_outq);
		}
		select_wakeup(&tp->t_selq);
	}
out:
	TTY_UNLOCK(tp);
	return (error);
}

ptsstop(tp, flush)
	register struct tty *tp;
	int flush;
{
	struct pt_ioctl *pti = &pt_ioctl[minor(tp->t_dev)];
	int flag;

	LASSERT(TTY_LOCK_HOLDER(tp));
	/* note: FLUSHREAD and FLUSHWRITE already ok */
	if (flush == 0) {
		flush = TIOCPKT_STOP;
		pti->pt_flags |= PF_STOPPED;
	} else
		pti->pt_flags &= ~PF_STOPPED;
	pti->pt_send |= flush;
	/* change of perspective */
	flag = 0;
	if (flush & FREAD)
		flag |= FWRITE;
	if (flush & FWRITE)
		flag |= FREAD;
	ptcwakeup(tp, flag);
}

#ifdef OSF1_ADFS
ptcselect(dev, events, revents, scanning, node)
#else
ptcselect(dev, events, revents, scanning)
#endif
	int scanning;
	dev_t dev;
	short *events, *revents;
#ifdef OSF1_ADFS
        node_t     node;
#endif
{
	register struct tty *tp = &pt_tty[minor(dev)];
	struct pt_ioctl *pti = &pt_ioctl[minor(dev)];
	int enqueue = 0;
	TSPLVAR(s)

	TTY_LOCK(tp);
	if (!scanning) {
		select_dequeue(&pti->pt_selq);
		TTY_UNLOCK(tp);
		return (0);
	}

	if ((tp->t_state&TS_CARR_ON) == 0) {
		*revents |= POLLHUP;
		if (*events & POLLNORM)
			*revents |= POLLNORM;
		goto selout;
	}

	if (*events & (POLLNORM | POLLPRI)) {
		if (*events & POLLNORM) {
			/*
			 * Need to block timeouts (ttrstart).
			 */
			TSPLTTY(s);
			if ((tp->t_state&TS_ISOPEN) &&
			    tp->t_outq.c_cc && (tp->t_state&TS_TTSTOP) == 0) {
				TSPLX(s);
				*revents |= POLLNORM;
				goto selout;
			}
			TSPLX(s);
		}

		/* FALLTHROUGH */
	
		if ((tp->t_state&TS_ISOPEN) &&
		    (pti->pt_flags&PF_PKT && pti->pt_send ||
		    pti->pt_flags&PF_UCNTL && pti->pt_ucntl)) {
			if (*events & POLLNORM)
				*revents |= POLLNORM;
			if (*events & POLLPRI)
				*revents |= POLLPRI;
			goto selout;
		}
		enqueue++;
	}

	if (*events & POLLOUT) {
		if (tp->t_state&TS_ISOPEN) {
			if (pti->pt_flags & PF_REMOTE) {
				if (tp->t_canq.c_cc == 0) {
					*revents |= POLLOUT;
					goto selout;
				}
			} else {
				if (tp->t_rawq.c_cc + tp->t_canq.c_cc < TTYHOG-2) {
					*revents |= POLLOUT;
					goto selout;
				}
				if (tp->t_canq.c_cc == 0 && (tp->t_iflag&ICANON)) {
					*revents |= POLLOUT;
					goto selout;
				}
			}
		}
		enqueue++;
	}

	/* Only enqueue if we don't have a response */
	if (*revents == 0 && enqueue)
		select_enqueue(&pti->pt_selq);

selout:
	TTY_UNLOCK(tp);
	return (0);
}

#ifdef OSF1_ADFS
ptcwrite(dev, node, uio, flag)
#else
ptcwrite(dev, uio, flag)
#endif
	dev_t dev;
	register struct uio *uio;
#ifdef OSF1_ADFS
        node_t node;
#endif
{
	register struct tty *tp = &pt_tty[minor(dev)];
	register struct iovec *iov;
	register char *cp;
	register int cc = 0;
	char locbuf[BUFSIZ];
	int cnt = 0;
	struct pt_ioctl *pti = &pt_ioctl[minor(dev)];
	int error = 0;
#if	SEC_ILB
	tag_t ntag;
#endif

	TTY_LOCK(tp);
again:
#if	SEC_ILB
	/*
	 * The ioctl comands that cause the input queue to be flushed are
	 * handled by the generic tty code and do not call back into the
	 * driver as in System V.
	 * Determine if the input queue has been flushed.
	 */
	if (tp->t_canq.c_cc == 0 && tp->t_rawq.c_cc == 0)
		SP_EMPTY_OBJECT(PTSTAG(dev,0));
#endif
	if ((tp->t_state&TS_ISOPEN) == 0)
		goto block;
#if	SEC_ILB
	if (uio->uio_resid &&
	    SP_CHECK_FLOAT(UIO_WRITE, PTSTAG(dev, 0), &ntag)) {
		TTY_UNLOCK(tp);
		return u.u_error;
	}
#endif
	if (pti->pt_flags & PF_REMOTE) {
		if (tp->t_canq.c_cc)
			goto block;
		while (uio->uio_iovcnt > 0 && tp->t_canq.c_cc < TTYHOG - 1) {
			iov = uio->uio_iov;
			if (iov->iov_len == 0) {
				uio->uio_iovcnt--;	
				uio->uio_iov++;
				continue;
			}
			if (cc == 0) {
				cc = MIN(iov->iov_len, BUFSIZ);
				cc = MIN(cc, TTYHOG - 1 - tp->t_canq.c_cc);
				cp = locbuf;
				uio->uio_rw = UIO_WRITE;
				error = uiomove(cp, cc, uio);
				if (error) {
					TTY_UNLOCK(tp);
					return (error);
				}
				/* check again for safety */
				if ((tp->t_state&TS_ISOPEN) == 0) {
					TTY_UNLOCK(tp);
					return (EIO);
				}
			}
			if (cc) {
				(void) b_to_q(cp, cc, &tp->t_canq);
#if	SEC_ILB
				SP_DO_FLOAT(UIO_WRITE, PTSTAG(dev,0), &ntag);
#endif
			}
			cc = 0;
		}
		(void) putc(0, &tp->t_canq);
		ttwakeup(tp);
		TTY_UNLOCK(tp);
		thread_wakeup((int)&tp->t_canq);
		return (0);
	}
	while (uio->uio_iovcnt > 0) {
		iov = uio->uio_iov;
		if (cc == 0) {
			if (iov->iov_len == 0) {
				uio->uio_iovcnt--;	
				uio->uio_iov++;
				continue;
			}
			cc = MIN(iov->iov_len, BUFSIZ);
			cp = locbuf;
			uio->uio_rw = UIO_WRITE;
			error = uiomove(cp, cc, uio);
			if (error) {
#if	SEC_ILB
				goto dofloat;
#else
				TTY_UNLOCK(tp);
				return (error);
#endif
			}
			/* check again for safety */
			if ((tp->t_state&TS_ISOPEN) == 0) {
#if	SEC_ILB
				error = EIO;
				goto dofloat;
#else
				TTY_UNLOCK(tp);
				return (EIO);
#endif
			}
		}
		while (cc > 0) {
			if ((tp->t_rawq.c_cc + tp->t_canq.c_cc) >= TTYHOG - 2 &&
			   (tp->t_canq.c_cc > 0 || !(tp->t_iflag&ICANON))) {
				thread_wakeup ((int) &tp->t_rawq);
				goto block;
			}
			(*linesw[tp->t_line].l_rint)(*cp++&0377, tp);
			cnt++;
			cc--;
		}
		cc = 0;
	}
#if	SEC_ILB
dofloat:
	if (cnt)
		SP_DO_FLOAT(UIO_WRITE, PTSTAG(dev, 0), &ntag);
#endif
	TTY_UNLOCK(tp);
#if	SEC_ILB
	return (error);
#else
	return (0);
#endif
block:
	/*
	 * Come here to wait for slave to open, for space
	 * in outq, or space in rawq.
	 */
	if ((tp->t_state&TS_CARR_ON) == 0) {
		TTY_UNLOCK(tp);
		return (EIO);
	}
	if ((pti->pt_flags & PF_NBIO) || (flag & IO_NDELAY)) {
		iov->iov_base -= cc;
		iov->iov_len += cc;
		uio->uio_resid += cc;
		uio->uio_offset -= cc;
#if	SEC_ILB
		if (cnt)
			SP_DO_FLOAT(UIO_WRITE, PTSTAG(dev, 0), &ntag);
#endif
		TTY_UNLOCK(tp);
		if (cnt == 0)
			return (EWOULDBLOCK);
		return (0);
	}
if (ptydebug) printf("SLEEP(2) c_cf %d\n", u.u_procp->p_pid);
	if (error = ttysleep(tp, (caddr_t)&tp->t_rawq.c_cf, 
			     TTOPRI | PCATCH, ttyout)) {
		TTY_UNLOCK(tp);
		return (error);
	}
	goto again;
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
ptyioctl(dev, node, cmd, data, flag)
#else
ptyioctl(dev, cmd, data, flag)
#endif
	caddr_t data;
	dev_t dev;
#ifdef OSF1_ADFS
        node_t  node;
#endif
{
	register struct tty *tp = &pt_tty[minor(dev)];
	register struct pt_ioctl *pti = &pt_ioctl[minor(dev)];
	register u_char *cc = tp->t_cc;
	int stop, error;
	extern ttyinput();
#if NKJI > 0
	extern kji_ttyinput();
#endif

	TTY_LOCK(tp);
	/*
	 * IF CONTROLLER STTY THEN MUST FLUSH TO PREVENT A HANG.
	 * ttywflush(tp) will hang if there are characters in the outq.
	 */
	if (cdevsw[major(dev)].d_open == ptcopen)
		switch (cmd) {

		case TIOCPKT:
			if (*(int *)data) {
				if (pti->pt_flags & PF_UCNTL) {
					goto einval;
				}
				pti->pt_flags |= PF_PKT;
			} else
				pti->pt_flags &= ~PF_PKT;
			TTY_UNLOCK(tp);
			return (0);
		case TIOCUCNTL:
			if (*(int *)data) {
				if (pti->pt_flags & PF_PKT) {
					goto einval;
				}
				pti->pt_flags |= PF_UCNTL;
			} else
				pti->pt_flags &= ~PF_UCNTL;
			TTY_UNLOCK(tp);
			return (0);

		case TIOCREMOTE:
			if (*(int *)data)
				pti->pt_flags |= PF_REMOTE;
			else
				pti->pt_flags &= ~PF_REMOTE;
			ttyflush(tp, FREAD|FWRITE);
			TTY_UNLOCK(tp);
			return (0);

		case FIONBIO:
			if (*(int *)data)
				pti->pt_flags |= PF_NBIO;
			else
				pti->pt_flags &= ~PF_NBIO;
			TTY_UNLOCK(tp);
			return (0);

		case TIOCSETP:
		case TIOCSETN:
		case TIOCSETD:
		case TIOCSETA:
		case TIOCSETAW:
		case TIOCSETAF:
			while (getc(&tp->t_outq) >= 0)
				;
#if	SEC_ILB
			SP_EMPTY_OBJECT(PTCTAG(dev,0));
#endif
			break;
		}

#ifdef	sun	/* Yes, this is a property of SunOS */

/*
 * The following code was added in order to allow Suntools to run under Mach.
 * Note that TIOCSSIZE has the same ioctl number as _O_TIOCSSIZE and
 * TIOCSWINSZ.  Thus a call specifying any of these ioctl's will be
 * handled here.
 */
	switch (cmd) {

	case TIOCSSIZE:	/* This isn't very pretty, but what driver is? */
		/*
		 * Check to make sure this is not a TIOCSWINSZ which has
		 * the same ioctl number.  We distinguish between the two
		 * by examining the "ts_lines" field in the ttysize structure.
		 * If the upper 16 bits of this field are non-zero, we assume
		 * it is a TIOCSWINSZ. TIOCSWINSZ is handled by the regular
		 * tty driver.
		 */
		if ((((struct swsize *)data)->ts_lines&0xffff0000) != 0)
			break;

	case _N_TIOCSSIZE:
		tp->t_winsize.ws_row = ((struct swsize *)data)->ts_lines;
		tp->t_winsize.ws_col = ((struct swsize *)data)->ts_cols;
		TTY_UNLOCK(tp);
		return (0);

	/*
	 * There is no way to distinguish between a real TIOCGSIZE as
	 * in SunOS and the 4.3 TIOCGWINSZ since the kluge used above
	 * can't be used (we don't know what the user is doing).  We'll
	 * assume that it's a TIOCGWINSZ and punt to the tty driver.
	 */
	case _N_TIOCGSIZE:
		((struct swsize *)data)->ts_lines = tp->t_winsize.ws_row;
		((struct swsize *)data)->ts_cols = tp->t_winsize.ws_col;
		TTY_UNLOCK(tp);
		return (0);
        }

#endif	/* sun */

	error = (*linesw[tp->t_line].l_ioctl)(tp, cmd, data, flag);
	if (error < 0)
		 error = ttioctl(tp, cmd, data, flag);
	/*
	 * Since we use the tty queues internally,
	 * pty's can't be switched to disciplines which overwrite
	 * the queues.  We can't tell anything about the discipline
	 * from here...
	 * kji_ttyinput is also "blessed" - kanji discipline.
	 */
#if NKJI > 0
	if ((linesw[tp->t_line].l_rint != ttyinput) &&
	    (linesw[tp->t_line].l_rint != kji_ttyinput)) {
#else
	if (linesw[tp->t_line].l_rint != ttyinput) {
#endif
		(*linesw[tp->t_line].l_close)(tp);
		tp->t_line = 0;
		(void)(*linesw[tp->t_line].l_open)(dev, tp, flag);
		error = ENOTTY;
	}
	if (error < 0) {
		if (pti->pt_flags & PF_UCNTL &&
		    (cmd & ~0xff) == UIOCCMD(0)) {
			if (cmd & 0xff) {
				pti->pt_ucntl = (u_char)cmd;
				ptcwakeup(tp, FREAD);
			}
			TTY_UNLOCK(tp);
			return (0);
		}
		error = ENOTTY;
	}
	stop = (tp->t_iflag & IXON) && CCEQ(cc[VSTOP], CTRL('s'))
		&& CCEQ(cc[VSTART], CTRL('q'));
	if (pti->pt_flags & PF_NOSTOP) {
		if (stop) {
			pti->pt_send &= ~TIOCPKT_NOSTOP;
			pti->pt_send |= TIOCPKT_DOSTOP;
			pti->pt_flags &= ~PF_NOSTOP;
			ptcwakeup(tp, FREAD);
		}
	} else {
		if (!stop) {
			pti->pt_send &= ~TIOCPKT_DOSTOP;
			pti->pt_send |= TIOCPKT_NOSTOP;
			pti->pt_flags |= PF_NOSTOP;
			ptcwakeup(tp, FREAD);
		}
	}
	TTY_UNLOCK(tp);
	return (error);
einval:
	TTY_UNLOCK(tp);
	return EINVAL;
}

void
pty_initialization()
{
	int			unit;
	extern struct tty	pt_tty[];

	for (unit = 0; unit < NPTY; ++unit) {
#if	UNIX_LOCKS && NCPUS > 1
		lock_init2(&pt_tty[unit].t_lock,TRUE,LTYPE_PTY);
#endif
		queue_init(&pt_tty[unit].t_selq);
		queue_init(&pt_ioctl[unit].pt_selq);
	}
}

#if	0
struct tty *
pt_dev_to_tty(dev)
dev_t	dev;
{
	return &pt_tty[minor(dev)];
}

sel_queue_t *
pt_dev_to_selq(dev)
dev_t	dev;
{
	return (sel_queue_t *) &pt_ioctl[minor(dev)].pt_selq;
}
#endif
#endif	/* NPTY > 0 */
