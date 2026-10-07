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
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
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
 * HISTORY
 * $Log: tty.h,v $
 * Revision 1.5  1994/11/18  20:41:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:29:39  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:53  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:20:31  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:44:55  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:34  cfj
 * Bump major revision number.
 *
 * Revision 2.12  1993/04/24  18:48:24  klh
 * 	Revision 2.10  93/04/08  11:32:57  loverso
 * 		Include synch.h to get null spl macro defs and avoid function calls.
 * 		[1992/06/04  14:17:04  condict]
 *
 * Revision 2.11  92/06/05  13:58:30  klh
 * 	Revision 2.9  92/05/01  10:14:13  rabii
 * 		Updated to Grenoble 3.6 (added in TS_ISSTOPPED flag value) (srl)
 * 
 * Revision 2.10  92/03/27  11:21:09  roman
 * Change PPROC_CTTY_SETUP() macro to call routine pproc_ctty_setup() and
 * 	add implicit call to it from isctty() macro.
 * 
 * Revision 2.9  92/03/24  10:20:34  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.7  92/03/09  12:18:49  durriya
 * 	[92/02/28  20:28:43  emcmanus]
 * 	Added t_nwrites and TS_DRAINING, to allow waiting for current writes
 * 	to finish.
 * 
 * 	[92/02/19  16:23:16  emcmanus]
 * 	Bigger TTYHOG for our VM, slow-response server.
 * 
 * Revision 2.6  92/01/09  16:12:54  roy
 * 	92/01/07  11:16:35  chrisp
 * 	Permit PPROC_CTTY_SETUP to operate with a null vproc pointer.
 * 
 * Revision 2.5  91/11/22  15:05:10  rabii
 * 	Locus Merge
 * 	Macro PPROC_CTTY_SETUP added to set SCTTY bit in p_flag to reflect
 * 	the setting of the calling proc in the dummy proc. (chrisp)
 * 
 * Revision 2.4  91/10/04  15:05:07  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  16:47:37  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Minor changes due to vprocs making process pointers unavailable.
 * 
 * Revision 2.2  91/08/31  14:15:07  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/07/31  15:46:29  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:55:35  devrcs
 * 	Move TSPLTTY and friends to tty.h.
 * 	[90/09/28  20:17:36  brezak]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:47:26  gm]
 * 
 * Revision 1.7  90/08/24  12:27:43  devrcs
 * 	Changes for ttysleep and setjmp/longjmp removal.
 * 	[90/08/20  06:41:20  gmf]
 * 
 * Revision 1.6  90/08/09  13:29:16  devrcs
 * 	Remove CMUCS dead code, dependencies.
 * 	[90/08/03  17:51:42  tmt]
 * 
 * Revision 1.5  90/06/22  20:54:29  devrcs
 * 	Added a state flag and a timeout value feild for processing the VMIN and
 * 	VTIME options in raw mode.
 * 	[90/06/14  23:05:16  havens]
 * 
 * 	Added form-feed character type to for FF delay processing.
 * 	[90/06/13  12:55:26  havens]
 * 
 * 	Conditionalize structures with _KERNEL.
 * 	[90/06/13  19:32:22  gm]
 * 
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Parallelized for OSF/1.					nags@encore.com
 * 	AIX merge first cut.					mbrown@osf.org
 * 	Merged Robert Coren's and Rich Morris's changes.	ers@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	Posix tty support.					morris@osf.org
 * 	Mach 2.5 and Encore 0.6 merge.				gm@osf.org
 * 	Merge early Mach 2.5 and Encore parallelization.	alan@encore.com
 * 	Encore changes to date:
 * 		MMAX_MP:  revamp select for mp environment.
 * 		MMAX_MP:  define TTY_SLEEP() in terms of mach_unix_sleep().
 * 		MMAX_MP:  add lock to tty structure.
 * 	More cleanup.						rpd@cmu.edu
 * 	Made all CMUCS stuff unconditional except for ttyloc.	mrt@cmu.edu
 * 	Adjusted include file references.			mwyoung@cmu.edu
 * 	CS_TTYLOC,CS_TTY => CS_GENERIC.				mja@cmu.edu
 * 	CS_GENERIC:  Added TS_LOGGEDIN and TS_LOGGEDOUT defs.	mja@cmu.edu
 * 	Added "TS_OUT" for dialout tty on Sun.			jjc@cmu.edu
 * 	Added TS_ONDELAY for X.					rvb@cmu.edu
 * 	Upgraded to 4.3.					avie@cmu.edu
 * 	Upgraded from 4.1BSD.  Carried over changes below.	mja@cmu.edu
 * 	CS_GENERIC:  Added t_ttyloc field to tty.		mja@cmu.edu
 * 	[90/06/12  21:39:38  gmf]
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
 *	@(#)tty.h	7.1 (Berkeley) 11/20/89
 */

#ifndef	_SYS_TTY_H_
#define _SYS_TTY_H_

#ifdef	_KERNEL
#include <sys/unix_defs.h>
#endif

#include <sys/types.h>
#include <sys/ioctl.h>		/* for struct winsize */
#include <sys/termios.h>
#include <sys/synch.h>		/* for the spl macros */

#ifdef _KERNEL
#include <kern/queue.h>
#endif

#ifdef _KERNEL
/* Some useful instruments */
enum csize {                            /* bits per character */
    bits5, bits6, bits7, bits8};
enum parity {                           /* parity */
    nopar, oddpar, markpar, evenpar, spacepar};
enum stopbits {                         /* stop bits */
    stop1, stop2};
enum status {
    good_char, overrun, parity_error, framing_error, break_interrupt,
    cts_on, cts_off, dsr_on, dsr_off, ri_on, ri_off, cd_on, cd_off,
    cblock_buf, other_buf};

typedef unsigned int baud_t;
typedef int csize_t;
typedef int parity_t;
typedef int stop_t;


/*
 * A clist structure is the head of a linked list queue
 * of characters.  The characters are stored in blocks
 * containing a link and CBSIZE (param.h) characters.
 * The routines in tty_subr.c manipulate these structures.
 */
struct clist {
	int	c_cc;		/* character count */
	char	*c_cf;		/* pointer to first char */
	char	*c_cl;		/* pointer to last char */
};

/*
 * Per-tty structure.
 *
 * Should be split in two, into device and tty drivers.
 * Glue could be masks of what to echo and circular buffer
 * (low, high, timeout).
 */
struct tty {
	struct	clist t_rawq;
	struct	clist t_canq;
	struct	clist t_outq;		/* device */
	int	(*t_oproc)();		/* device */
	int	(*t_param)();		/* device */
	struct	queue_entry t_selq;	/* Queue of waiting threads	*/
	caddr_t	T_LINEP;		/* ### */
	caddr_t	t_addr;			/* ??? */
	dev_t	t_dev;			/* device */
	int	t_flags;		/* (compat) some of both */
	int	t_state;		/* some of both */
	int	t_nwrites;		/* Number of writes happening */
	pid_t	t_sid;			/* session id */	
	pid_t	t_pgid;			/* foreground process group id */
	struct	vnode	*t_vnode;	/* corresponding vnode */
	char	t_line;			/* glue */
	char	t_col;			/* tty */
	char	t_rocount, t_rocol;	/* tty */
 	short	t_hiwat;		/* hi water mark */
 	short	t_lowat;		/* low water mark */
	struct	winsize t_winsize;	/* window size */
 	struct	termios t_termios;	/* termios state */
#define	t_iflag		t_termios.c_iflag
#define	t_oflag		t_termios.c_oflag
#define	t_cflag		t_termios.c_cflag
#define	t_lflag		t_termios.c_lflag
#define	t_min		t_termios.c_cc[VMIN]
#define	t_time		t_termios.c_cc[VTIME]
#define	t_cc		t_termios.c_cc
#define t_ispeed	t_termios.c_ispeed
#define t_ospeed	t_termios.c_ospeed
 	long	t_cancc;		/* stats */
 	long	t_rawcc;
 	long	t_outcc;
	long    t_shad_time;            /* Value of t_cc[VTIME] in ticks */
	short	t_gen;
#if	UNIX_LOCKS && NCPUS > 1
	lock_data_t	t_lock;
#endif
#ifdef	OSF1_SERVER
	mach_port_t	t_device_port;
	mach_port_t	t_reply_port;
#endif	OSF1_SERVER
};
#endif /* _KERNEL */

#define TTIPRI	28
#define TTOPRI	29

/* limits */
#define	NSPEEDS	16
#define TTMASK	15
#define OBUFSIZ	100
#ifdef OSF1_SERVER
/* The time before flow control can take effect is much greater than in the
 * integrated kernel, so we have to allow more characters to arrive before
 * complaining.  Since we have VM this is no big deal.
 */
#define TTYHOG	1023
#else
#define TTYHOG	255
#endif

#ifdef	KERNEL
short	tthiwat[NSPEEDS], ttlowat[NSPEEDS];
#define	TTHIWAT(tp)	tthiwat[(tp)->t_ospeed&TTMASK]
#define	TTLOWAT(tp)	ttlowat[(tp)->t_ospeed&TTMASK]
#endif	KERNEL

#ifdef	_KERNEL
#if	NCPUS == 1
#define	TSPLVAR(s)	int s;
#define	TSPLTTY(s)	s = spltty()
#define	TSPLX(s)	splx(s)
#else
#if	UNIX_LOCKS
#define	TSPLVAR(s)
#define	TSPLTTY(s)
#define	TSPLX(s)
#endif
#endif

#define TTMAXHIWAT	roundup(2048, CBSIZE)
#define TTMINHIWAT	roundup(100, CBSIZE)
#define TTMAXLOWAT	256
#define TTMINLOWAT	32

extern	struct ttychars ttydefaults;
#endif	/* _KERNEL */

/* internal state bits */
#define TS_TIMEOUT	0x000001	/* delay timeout in progress */
#define TS_WOPEN	0x000002	/* waiting for open to complete */
#define TS_ISOPEN	0x000004	/* device is open */
#define TS_FLUSH	0x000008	/* outq has been flushed during DMA */
#define TS_CARR_ON	0x000010	/* software copy of carrier-present */
#define TS_BUSY		0x000020	/* output in progress */

#define TS_DRAINING	0x4000000	/* Waiting for output to drain. */
#define TS_ISSTOPPED    0x8000000       /* Have stopped kernel I/O. */
#define TS_ASLEEP	0x000040	/* wakeup when output done */
#define TS_XCLUDE	0x000080	/* exclusive-use flag against open */
#define TS_TTSTOP	0x000100	/* output stopped by ctl-s */
#define TS_HUPCLS	0x000200	/* hang up upon last close */
#define TS_TBLOCK	0x000400	/* tandem queue blocked */
#define TS_NBIO		0x002000	/* tty in non-blocking mode */
#define TS_ASYNC	0x004000	/* tty in async i/o mode */
#define TS_ONDELAY	0x008000	/* device is open; software copy of
 					 * carrier is not present */
#ifdef	sun
#define TS_OUT          0x010000        /* tty in use for dialout only */
					/* NOTE: This was 0x008000 in
						 Sun Unix */
#endif
/* state for intra-line fancy editing work */
#define TS_BKSL		0x010000	/* state for lowercase \ work */
#define TS_ERASE	0x040000	/* within a \.../ for PRTRUB */
#define TS_LNCH		0x080000	/* next character is literal */
#define TS_TYPEN	0x100000	/* retyping suspended input (PENDIN) */
#define TS_CNTTB	0x200000	/* counting tab width, leave FLUSHO alone */

#define TS_RQUEUED	0x1000000	/* has a read been queued */

#define TS_INTIMEOUT    0x2000000       /* A input timeout is active. */

#define	TS_LOCAL	(TS_BKSL|TS_ERASE|TS_LNCH|TS_TYPEN|TS_CNTTB)


/* define partab character types */
#define ORDINARY	0
#define CONTROL		1
#define BACKSPACE	2
#define NEWLINE		3
#define TAB		4
#define VTAB		5
#define RETURN		6
#define FF              7

struct speedtab {
        int sp_speed;
        int sp_code;
};
/*
 * Flags on character passed to ttyinput
 */
#define TTY_CHARMASK    0x000000ff      /* Character mask */
#define TTY_QUOTE       0x00000100      /* Character quoted */
#define TTY_ERRORMASK   0xff000000      /* Error mask */
#define TTY_FE          0x01000000      /* Framing error or BREAK condition */
#define TTY_PE          0x02000000      /* Parity error */

/*
 * Macros
 */
#define	PPROC_CTTY_SETUP(p)	pproc_ctty_setup(p)
#define isctty(p, tp)	(PPROC_CTTY_SETUP(p), \
			 (p)->p_sid == (tp)->t_sid && \
			 (p)->p_flag&SCTTY)
#define isbackground(p, tp)	(isctty((p), (tp)) && \
				(p)->p_pgid != (tp)->t_pgid)

/*
 * Modem control commands (driver).
 */
#define	DMSET		0
#define	DMBIS		1
#define	DMBIC		2
#define	DMGET		3

#ifdef	_KERNEL
int	ttysleep();

#if	UNIX_LOCKS && NCPUS > 1
#define	TTY_LOCK(tp)		lock_write(&(tp)->t_lock)
#define	TTY_UNLOCK(tp)		lock_write_done(&(tp)->t_lock)
#define	TTY_LOCK_TRY(tp)	lock_try_write(&(tp)->t_lock)
#define TTY_LOCK_HOLDER(tp)	LOCK_HOLDER(&(tp)->t_lock)

#else	/* UNIX_LOCKS && NCPUS > 1 */

#define	TTY_LOCK(tp)
#define	TTY_UNLOCK(tp)
#define	TTY_LOCK_TRY(tp)	1
#define TTY_LOCK_HOLDER(tp)	1

#endif	/* UNIX_LOCKS && NCPUS > 1< */

/* symbolic sleep message strings */
extern   char ttyin[], ttyout[], ttopen[], ttclos[], ttybg[], ttybuf[];
#endif	/* _KERNEL */

#endif	_SYS_TTY_H_
