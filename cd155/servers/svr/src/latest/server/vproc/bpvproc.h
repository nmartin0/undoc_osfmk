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
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: bpvproc.h,v $
 * Revision 1.5  1994/11/18  20:50:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:47:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:10:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:10:47  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:58:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:47:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:58:08  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:55:17  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.6  1992/09/29  08:27:29  roman
 * Change names of vproc list pointers in structure to have a single
 * 	(uniform, better, predictable) style.
 * Add concept of foster children - children of init that give their rusage
 * 	statistics to a foster parent rather than init.
 *
 * Revision 3.5  92/03/19  15:00:58  chrisp
 * PV_ADD/PV_REMOVE become PV_ADD/PV_SUBTRACT for new pgrp handling.
 * 
 * Revision 3.4  92/01/07  11:18:57  chrisp
 * Re-assign PV_SCTTY for consistency with tnc.
 * 
 * Revision 3.3  91/12/20  16:54:06  chrisp
 * Add flags for orphan pgrp handling distinguishing cases add/remove/exit.
 * 
 * Revision 3.2  91/11/22  10:28:05  chrisp
 * Redundant vnode pointer removed.
 * 
 * Revision 3.1  91/11/14  14:58:08  chrisp
 * Flag bit PV_SCTTY added for controlling termanal handling in Vproc layer.
 * 
 * Revision 3.0  91/10/25  10:32:40  roman
 * Initial submission of version of pvproc.h that does not allow
 * distributed pvproc operations.
 * 
 * Revision 2.3  91/10/18  18:37:54  chrisp
 * 	Proper PVPOPs. Debugging message formats added for each OP.
 * 
 * Revision 2.2  91/09/16  16:47:17  rabii
 * 	Initilal Checkin (locus check-in by roman)
 * 
 *
 */
#ifndef _H_BPVPROC
#define _H_BPVPROC


#include <sys/vproc.h>

/*
 * pvproc data structure
 */
struct pvproc {
	unsigned long	pvp_flag;	/* pvproc attributes */
	struct proc	*pvp_pproc;	/* physical process structure */
	struct vproc	*pvp_head_childl; /* first child of parent */
	struct vproc	*pvp_childl;	/* next child of parent */
	struct vproc	*pvp_head_foster_childl; /* first foster child of ...
					   parent */
	struct vproc	*pvp_foster_childl; /* next foster child of parent */
					/* NOTE: foster children are ...
					   orphaned children that will ...
					   contribute their accounting ...
					   stats to this process when they ...
					   are reaped by the init process */
	struct vproc	*pvp_head_pgrpl; /* pointer to first pgrp member */
	struct vproc	*pvp_pgrpl;	/* next pgrp member */
	struct vproc	*pvp_sessionl;	/* pointer to pgrp leaders in session */
	pid_t		pvp_ppid;	/* parent process id */
	pid_t		pvp_foster_ppid;/* pid of process that will inherit ...
					   accounting stats when this proc ...
					   is reaped (normally == pvp_ppid) */
	pid_t		pvp_sid;	/* session leader id */
	pid_t		pvp_pgid;	/* process group leader id */
	struct tty	*pvp_cttyp;	/* controlling TTY tty structure */
	unsigned long	pvp_jobc;	/* vprocs contributing to non-orphan
					   status (valid on
					   group leader vproc only) */
};

/*
 * Shortcut for getting directly from vproc to pvproc structure member.
 */
#define PVP(vp)		((struct pvproc *)((vp)->vp_data))

/*
 * Actual pvproc table
 */
struct pvproc *pvproc, *pvprocNPVPROC;

/*
 * flag values for the pvproc structure
 */
#define	PV_SCTTY		0x0001	/* Process has a controlling tty */
#define PV_PGRPLEADER		0x0002	/* Process is a pgrp leader */
#define PV_SESSIONLEADER	0x0004	/* Process is a session leader */
#define	PV_SZOMB		0x0010	/* Process is in zombie state */
#define	PV_SSTOP		0x0020	/* Process is in stopped state */

/*
 * Flag values modifying semantics of operations
 */
#define	PV_EXIT			0x0001	/* Requests special processing
					   associated with process exit */
#define	PV_ADD			0x0002	/* Add to pgrp job count */
#define	PV_SUBTRACT		0x0004	/* Subtract from pgrp job count */

#endif  /* _H_BPVPROC */
