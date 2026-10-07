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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: subr_log.c,v $
 * Revision 1.7  1994/11/18  20:27:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  17:48:55  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:49:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:04:51  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:00  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:58:55  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:54:51  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:16:33  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:48  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.6  92/11/03  11:19:10  loverso
 * 	Revision 3.4  92/04/08  20:45:37  barbou
 * 	Fix for bug #80: master_mutex was not always released in logopen().
 * 
 * Revision 2.8  1993/02/04  11:48:53  durriya
 * 	Protect log select queue when calling select_wakeup().
 *
 * Revision 2.7  93/01/08  14:28:11  durriya
 * 	add node # as arg to logclose, read, select, ioctl       (durriya)
 * 
 * Revision 2.6  92/11/03  11:19:10  loverso
 * 	Revision 3.4  92/04/08  20:45:37  barbou
 * 	Fix for bug #80: master_mutex was not always released in logopen().
 * 
 * Revision 2.5  92/03/09  14:27:51  durriya
 * 	Revision 3.3  91/12/18  17:15:20  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.4  91/10/04  14:51:03  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:42:34  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Remove reference to pgrp structure. Done differently now with vprocs.
 * 
 * Revision 2.2  91/08/31  13:23:07  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/05  13:54:12  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  13:18:38  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:58:29  gm]
 * 
 * Revision 1.8  90/08/24  11:18:06  devrcs
 * 	changed sleep to tsleep
 * 	[90/08/20  01:35:08  gmf]
 * 
 * Revision 1.7  90/06/22  20:06:41  devrcs
 * 	nags merge
 * 
 * 	Condensed history:
 * 	Changed to use new select interface		coren@osf.org
 * 	     Change to use 4.4BSD pgrp structure.            coren@osf.org
 * 	     Removed include of <sys/dir.h>                  gmf@osf.org
 * 	     Fixes for first snapshot.                       gm@osf.org
 * 	     Changes for 4.4BSD uiomove.                     noemi@osf.org
 * 	     Mach 2.5 and Encore 0.6 merge                   gm@osf.org
 * 	     Mach 2.5 merged w/ Encore changes               alan@encore.com
 * 	     Encore parallelization changes                  alan@encore.com
 * 	     More cleanup.                                   rdp@cs.cmu.edu
 * 	     Changes for cleanup.                            gm0w@cs.cmu.edu
 * 	     Code cleanup cataclysm.                         mwyoung@cs.cmu.edu
 * 	     Mips support                                    af@cs.cmu.edu
 * 	     Fix logioctl so that it returns EINVAL          jsb@cs.cmu.edu
 * 	     Add include for "kern/thread.h"                 mja@cs.cmu.edu
 * 	     Changed log_select for new select scheme.       boykin@encore.com
 * 	     Use a thread when doing selwakeup.              mwyoung@cs.cmu.edu
 * 	[90/06/12  19:07:09  gmf]
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
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)subr_log.c	7.1 (Berkeley) 6/5/86
 */

/*
 * Error log buffer for kernel printf's.
 */

#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/ioctl.h>
#include <sys/msgbuf.h>
#include <sys/file.h>
#include <sys/errno.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <kern/parallel.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#ifdef	mips
struct	msgbuf *pmsgbuf;
#define msgbuf	(*pmsgbuf)	/* dynamically allocated on Mips. Sigh */
#else	
struct	msgbuf msgbuf;
#endif

#define LOG_RDPRI	(PZERO + 1)

#define LOG_NBIO	0x02
#define LOG_ASYNC	0x04
#define LOG_RDWAIT	0x08

struct logsoftc {
	int	sc_state;		/* see above for possibilities */
	struct queue_entry sc_selq;	/* threads waiting for select */
	int	sc_pgrp;		/* process group for async I/O */
} logsoftc;

int	log_open;			/* also used in log() */
/*
 * Serialize log access.  Note that the log can be written at interrupt level,
 * so any log manipulations that can be done from, or affect, another processor
 * at interrupt level must be guarded with a spin lock.
 */
udecl_simple_lock_data(,log_lock);	/* stop races dead in their tracks */
#define	LOG_LOCK()	usimple_lock(&log_lock)
#define	LOG_UNLOCK()	usimple_unlock(&log_lock)
#define	LOG_LOCK_INIT()	usimple_lock_init(&log_lock)

/*ARGSUSED*/
logopen(dev)
	dev_t dev;
{

	unix_master();			/* for pg_id, sigh */
	LOG_LOCK();
	if (log_open) {
		LOG_UNLOCK();
		unix_release();
		return (EBUSY);
	}
	log_open = 1;
	queue_init(&logsoftc.sc_selq);
	logsoftc.sc_pgrp = u.u_procp->p_pgid;
	/*
	 * Potential race here with putchar() but since putchar should be
	 * called by autoconf, msg_magic should be initialized by the time
	 * we get here.
	 */
	if (msgbuf.msg_magic != MSG_MAGIC) {
		register int i;

		msgbuf.msg_magic = MSG_MAGIC;
		msgbuf.msg_bufx = msgbuf.msg_bufr = 0;
		for (i=0; i < MSG_BSIZE; i++)
			msgbuf.msg_bufc[i] = 0;
	}
	LOG_UNLOCK();
	unix_release();
	return (0);
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
logclose(dev, node, flag)
#else
logclose(dev, flag)
#endif
	dev_t dev;
#ifdef OSF1_ADFS
        node_t   node;
#endif
{
	LOG_LOCK();
	select_wakeup(&logsoftc.sc_selq);
	select_dequeue_all(&logsoftc.sc_selq);
	log_open = 0;
	logsoftc.sc_state = 0;
	logsoftc.sc_pgrp = 0;
	LOG_UNLOCK();
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
logread(dev, node, uio)
#else
logread(dev, uio)
#endif
	dev_t dev;
#ifdef OSF1_ADFS
        node_t   node;
#endif
	struct uio *uio;
{
	register long l;
	register int s;
	int error = 0;

	s = splhigh();
	while (msgbuf.msg_bufr == msgbuf.msg_bufx) {
		if (logsoftc.sc_state & LOG_NBIO) {
			splx(s);
			return (EWOULDBLOCK);
		}
		logsoftc.sc_state |= LOG_RDWAIT;
		if (error = tsleep((caddr_t)&msgbuf, LOG_RDPRI | PCATCH,
				"klog", 0)) {
			splx(s);
			return (error);
		}
	}
	splx(s);
	logsoftc.sc_state &= ~LOG_RDWAIT;

	while (uio->uio_resid > 0) {
		l = msgbuf.msg_bufx - msgbuf.msg_bufr;
		if (l < 0)
			l = MSG_BSIZE - msgbuf.msg_bufr;
		l = MIN(l, uio->uio_resid);
		if (l == 0)
			break;
		uio->uio_rw = UIO_READ;
		error = uiomove((caddr_t)&msgbuf.msg_bufc[msgbuf.msg_bufr],
			(int)l, uio);
		if (error)
			break;
		msgbuf.msg_bufr += l;
		if (msgbuf.msg_bufr < 0 || msgbuf.msg_bufr >= MSG_BSIZE)
			msgbuf.msg_bufr = 0;
	}
	return (error);
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
logselect(dev, events, revents, scanning, node)
#else
logselect(dev, events, revents, scanning)
#endif
        dev_t	dev;
        short	*events, *revents;
        int	scanning;
#ifdef OSF1_ADFS
        node_t   node;
#endif

{
	LOG_LOCK();
	if (scanning) {
		if (*events & POLLNORM) {
			if(msgbuf.msg_bufr != msgbuf.msg_bufx)
				*revents |= POLLNORM;
			else
				select_enqueue(&logsoftc.sc_selq);
		}
	} else
		select_dequeue(&logsoftc.sc_selq);

	LOG_UNLOCK();
	return(0);
}

logwakeup()
{
	int pgrp;

	if (!log_open)
		return;

	LOG_LOCK();
	select_wakeup(&logsoftc.sc_selq);
	if (logsoftc.sc_state & LOG_ASYNC) {
		unix_master();
		pgrp = logsoftc.sc_pgrp;
#ifdef	OSF1_SERVER
		gsignal3(logsoftc.sc_pgrp, SIGIO, FALSE);
#else	OSF1_SERVER
		gsignal(logsoftc.sc_pgrp, SIGIO);
#endif	OSF1_SERVER
		unix_release();
	}
	LOG_UNLOCK();
	if (logsoftc.sc_state & LOG_RDWAIT) {
		wakeup((caddr_t)&msgbuf);
		logsoftc.sc_state &= ~LOG_RDWAIT;
	}
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
logioctl(dev, node, com, data, flag)
#else
logioctl(dev, com, data, flag)
#endif
        dev_t dev;
#ifdef OSF1_ADFS
        node_t   node;
#endif
	caddr_t data;
{
	long l;
	int s;

	switch (com) {

	/* return number of characters immediately available */
	case FIONREAD:
		s = splhigh();
		l = msgbuf.msg_bufx - msgbuf.msg_bufr;
		splx(s);
		if (l < 0)
			l += MSG_BSIZE;
		*(off_t *)data = l;
		break;

	case FIONBIO:
		if (*(int *)data)
			logsoftc.sc_state |= LOG_NBIO;
		else
			logsoftc.sc_state &= ~LOG_NBIO;
		break;

	case FIOASYNC:
		if (*(int *)data)
			logsoftc.sc_state |= LOG_ASYNC;
		else
			logsoftc.sc_state &= ~LOG_ASYNC;
		break;

	case TIOCSPGRP:
		LOG_LOCK();
		logsoftc.sc_pgrp = *(int *)data;
		LOG_UNLOCK();
		break;

	case TIOCGPGRP:
		LOG_LOCK();
		*(int *)data = logsoftc.sc_pgrp;
		LOG_UNLOCK();
		break;

	default:
		return EINVAL;
	}
	return (0);
}

void
log_init()
{
	LOG_LOCK_INIT();
}

#if	MACH_LDEBUG
/*
 * It's tough to export the definition of logsoftc
 * unless it's put into a .h file.  Rather than go
 * to the trouble, we define this function to return
 * a pointer to the logsoftc's select queue.
 */
sel_queue_t *
log_selq()
{
	return (sel_queue_t *) &logsoftc.sc_selq;
}
#endif	
