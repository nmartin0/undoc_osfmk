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
 * $Log: tty_tty.c,v $
 * Revision 1.7  1995/02/01  21:29:57  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:28:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/03/14  02:00:49  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.4  1993/07/14  17:50:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:51:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:07:18  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  18:51:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.1.1.1  1993/05/03  17:25:44  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.12  1993/04/24  18:47:52  klh
 * 	Revision 2.9  93/01/08  14:28:50  durriya
 * 		add node # as arg to syread, sywrite, syioctl, syselect
 *
 * Revision 2.11  92/04/14  10:05:43  roman
 * Add #ifndef's for TNC around disallowing tty operations on non root fs nodes.
 * 
 * Revision 2.10  92/04/06  19:07:41  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.7  92/04/05  16:49:01  pjg
 * 	Don't allow tty code to execute outside the root FS (rabii).
 * 
 * 	Routine pproc_ctty_setup() added to support the PPROC_CTTY_SETUP() 
 * 	macro.  * CTTY vpops now have an extra parameter, the session vproc. 
 * 	(chrisp)
 * 
 * Revision 2.6  92/02/11  22:19:49  pjg
 * 	Remove the RESIGN_PGRP call for COMPAT_43 in sysioctl(TIOCNOTTY)
 * 	(chrips@locus).
 * 
 * Revision 2.5  91/11/22  14:54:56  rabii
 * 	Locus Merge
 * 	Correct test in macro cttyvp so that open(/dev/tty) succeeds. (chrisp)
 * 
 * 	Macro cttyvp altered to use VPOP_CTTY_GETATTR and to set SCTTY
 * 	in the dummy proc.
 * 	TIOCNOCTTY now calls into vproc layer. (chrisp)
 * 
 * Revision 2.4  91/10/04  14:54:09  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:50:16  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Change to use vproc operations when handling controlling tty.
 * 	Also, process group and session structures replaced by ids.
 * 
 * Revision 2.2  91/08/31  13:24:02  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:56:24  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9.3.2  91/03/05  10:19:46  gmf
 * 	Remove VOP_ACCESS check from syopen().  You
 * 	always have access to your controlling terminal.
 * 	[91/02/28  14:02:35  gmf]
 * 
 * Revision 1.9  90/10/07  13:20:25  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:02:49  gm]
 * 
 * Revision 1.8  90/07/27  08:44:49  devrcs
 * 	VOP_OPEN changes for clone driver support.
 * 	[90/07/20  13:51:33  nags]
 * 
 * 	Include mount.h for all cases (funnelling).
 * 	[90/07/17  08:44:00  nags]
 * 
 * Revision 1.7  90/06/22  20:07:36  devrcs
 * 	nags merge
 * 
 * 	Condensed relevant history:
 * 	Parallelized for OSF/1.					nags@encore.com
 * 	Added compatibility to TIOCNOTTY.			ers@osf.org
 * 	New select interface					coren@osf.org
 * 	Turned on TTY locks, threw sessions on unix_master.	nags@encore.com
 * 	Encore parallelization changes and sync. w/4.4.		gmf@osf.org
 * 	Merged Robert Coren's and Rich Morris's changes		ers@osf.org
 * 	Posix tty support.					morris@osf.org
 * 	syselect() must take an additional argument		alan@encore.com
 * 	[90/06/12  21:17:54  nags]
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
 *	@(#)tty_tty.c	7.7 (Berkeley) 2/21/90
 */

/*
 * Indirect driver for controlling tty.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/user.h>
#include <sys/mount.h>			/* funnelling through mnt struct */
#include <sys/vnode.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/uio.h>
#include <kern/parallel.h>

#ifndef	TNC
extern	node_t		this_node;
extern	node_t		root_fs_node;
#endif	/* TNC */

#define cttyvp(vp, p) { \
	struct tty *ttyp; \
	struct vproc *vs; \
	PROC_LOCK(p); \
	vs = LOCATE_VPROC_PID(p->p_sid); \
	if (!VPOP_CTTY_GETATTR((p)->p_vproc, vs, 0, 0, 0, &ttyp)) { \
		p->p_flag |= SCTTY; \
		vp = ttyp->t_vnode; \
	} else { \
		p->p_flag &= ~SCTTY; \
		vp = NULL; \
	} \
	VPROC_RELEASE(vs, "cttyvp()"); \
	PROC_UNLOCK(p); \
}

pproc_ctty_setup(p)
	struct proc *p;
{
	if (p->p_vproc == NULL)
		p->p_flag &= ~SCTTY;
	else {
		register struct vproc *vs;
		vs = LOCATE_VPROC_PID(p->p_sid);
		if (VPOP_CTTY_GETATTR(p->p_vproc, vs, 0, 0, 0, 0) == ENOTTY)
			p->p_flag &= ~SCTTY;
		else
			p->p_flag |= SCTTY;
		VPROC_RELEASE(vs, "pproc_ctty_setup");
	}
}

/*ARGSUSED*/
syopen(dev, flag)
	dev_t dev;
	int flag;
{
	struct vnode *ttyvp;
	int error;

#ifndef	TNC
	/* tty code is only allowed on the root fs */
	if (this_node != root_fs_node) {
		return (EACCES);
	}
#endif	/* TNC */
	unix_master();			/* sessions, sigh */
	cttyvp(ttyvp, u.u_procp);

	if (ttyvp == NULL) {
		unix_release();
		return (ENXIO);
	}
	VOP_OPEN(&ttyvp, flag, NOCRED, error);
	unix_release();
	return (error);
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
syread(dev, node, uio, flag)
#else
syread(dev, uio, flag)
#endif
	dev_t dev;
	struct uio *uio;
#ifdef OSF1_ADFS
        node_t   node;
#endif
{
	struct vnode *ttyvp;
	int error;

	unix_master();			/* sessions, sigh */
	cttyvp(ttyvp, u.u_procp);
	if (ttyvp == NULL) {
		unix_release();
		return (ENXIO);
	}
	VOP_READ(ttyvp, uio, flag, NOCRED, error);
	unix_release();
	return (error);
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
sywrite(dev, node, uio, flag)
#else
sywrite(dev, uio, flag)
#endif
	dev_t dev;
	struct uio *uio;
#ifdef OSF1_ADFS
        node_t   node;
#endif
{
	struct vnode *ttyvp;
	int error;

	unix_master();			/* sessions, sigh */
	cttyvp(ttyvp, u.u_procp);

	if (ttyvp == NULL) {
		unix_release();
		return (ENXIO);
	}
	VOP_WRITE(ttyvp, uio, flag, NOCRED, error);
	unix_release();
	return (error);
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
syioctl(dev, node, cmd, addr, flag)
#else
syioctl(dev, cmd, addr, flag)
#endif
	dev_t dev;
#ifdef OSF1_ADFS
        node_t   node;
#endif
	int cmd;
	caddr_t addr;
	int flag;
{
	struct vnode *ttyvp;
	int error;

	unix_master();			/* sessions, sigh */
	cttyvp(ttyvp, u.u_procp);

	if (ttyvp == NULL) {
		unix_release();
		return (ENXIO);
	}
	if (cmd == TIOCNOTTY) {
		if (!SESS_LEADER(u.u_procp)) {
			register struct vproc *vs;
			register struct proc *p = u.u_procp;
			PROC_LOCK(p);
			vs = LOCATE_VPROC_PID(p->p_sid);
			(void) VPOP_SET_CTTY(p->p_vproc, vs, 0, VPROC_CLEAR);
			VPROC_RELEASE(vs, "syioctl()");
			p->p_flag &= ~SCTTY;
			PROC_UNLOCK(p);
			unix_release();
			return (0);
		} else {
			unix_release();
			return (EINVAL);
		}
	}
	VOP_IOCTL(ttyvp, cmd, addr, flag, NOCRED, error);
	unix_release();
	return (error);
}

/*ARGSUSED*/
#ifdef OSF1_ADFS
syselect(dev, events, revents, scanning, node)
#else
syselect(dev, events, revents, scanning)
#endif
	dev_t dev;
	short *events, *revents;
	int scanning;
#ifdef OSF1_ADFS
        node_t     node;
#endif
{
	struct vnode *ttyvp;
	int error;

	unix_master();			/* sessions, sigh */
	cttyvp(ttyvp, u.u_procp);

	if (ttyvp == NULL) {
		unix_release();
		return (ENXIO);
	}
	VOP_SELECT(ttyvp, events, revents, scanning, NOCRED, error);
	unix_release();
	return (error);
}
