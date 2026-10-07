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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
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
 * $Log: tty.c,v $
 * Revision 1.12  1995/02/01  21:29:28  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/11/18  20:28:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/04/22  18:27:37  dbm
 * Mainline merge of R1.2 version R1.8.2.2
 *
 * Revision 1.9  1994/01/12  13:37:03  nandy
 * Merged from the R1.2 branch.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8.2.2  1994/04/22  18:14:29  dbm
 * Added check for return of VPOP_GET_ATTR()
 *  Reviewer: Charlie Johnson
 *  Risk: Low
 *  Benefit or PTS #: 8729
 *  Testing: Specific test case, VSX Eats.
 *  Module(s):
 * 	bsd/tty.c
 * 	tnc/dvp_pvpops.c
 * 	tnc/dvp_vpops.c
 * 	nx/nx.c
 *
 * Revision 1.8.2.1  1994/01/07  16:54:09  nandy
 * Master lock shouldn't be held before making tnc calls that block
 * on other mutexes. Ifdefed out unix_master() and unix_release() before
 * calling gsignal4().
 *
 *  Reviewer: Chris Peak
 *  Risk: Medium
 *  Benefit or PTS #: 7634
 *  Testing: Tested at KFA
 *  Module(s): tty.c
 *
 * Revision 1.8  1993/10/11  13:20:07  nandy
 * VPOP_GET_ATTR() has two extra parameters; uid and ruid.
 *
 * Revision 1.7  1993/07/19  22:58:20  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.6  1993/07/14  17:49:50  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:50:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:06:31  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:24:21  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:59:09  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:55:03  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:17:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:07:19  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.23  93/07/13  15:57:08  slively
 *
 *      Revision 2.17  93/06/29  16:17:34  rabii
 *      Put in UFS conditional (rabii)
 *
 *
 * Revision 2.19  92/11/23  16:01:26  klh
 * 	Revision 2.15  92/11/17  19:47:50  loverso
 * 		Correctly calculate the load average.
 * 
 * Revision 2.22  93/06/25  11:19:06  slively
 * Backout the LITE server changes, #if UFS and include UFS.
 * 
 * Revision 2.21  93/06/22  19:54:47  slively
 * Lite server changes, #if UFS, and include <ufs.h>
 * 
 * Revision 2.20  93/04/24  18:47:30  klh
 * 	Revision 2.16  93/01/08  14:28:25  durriya
 * 		add node # as arg to ttselect                           (durriya)
 *
 * Revision 2.19  92/11/23  16:01:26  klh
 * 	Revision 2.15  92/11/17  19:47:50  loverso
 * 		Correctly calculate the load average.
 * 
 * Revision 2.18  92/10/05  13:59:19  klh
 * 	Revision 2.13  92/08/26  12:10:20  loverso
 * 		Fix ttyinfo to at least get load avg of current host/node.
 * 
 * Revision 2.17  92/09/28  16:35:48  roman
 * Fix RCS comments.
 * Change calls to VPOP_SIGPGRP() with VPROC_TERMINAL_ACCESS to use the
 * 	new vproc op VPOP_TERMINAL_SIGPGRP().
 * 
 * Revision 2.16  92/06/05  13:56:25  klh
 * 	Revision 2.12  92/05/12  00:06:39  loverso
 * 		Changes ala OSF/1 1.1 to only call select_enqueue() once, and to
 * 		call select_dequeue() faster.
 * 
 * 	Revision 2.11  92/05/01  10:19:46  rabii
 * 		Grenoble 3.6 merge by srl
 * 
 * 		92/03/31  15:40:07  emcmanus
 * 		Wakeup the tty output queue after flushing it, in case someone 
 *		is waiting for space on it.
 * 
 * 		92/03/23  18:02:34  condict
 * 		Allow NCPUS == 1 in the server, to compile optimally for a
 * 		uni-processor.
 * 
 * 		92/03/06  17:33:42  emcmanus
 * 		Reinstated fixes for correct output draining, disabled because 
 *		of some forgotten decrements of t_nwrites.
 * 		Template for correct XON and XOFF sending; these should be 
 *		sent even if output has been disabled.  Needs work in 
 *		uxkern/tty_io.c.
 * 
 * Revision 2.15  92/04/14  10:05:01  roman
 * Add #ifndef's for TNC around disallowing tty operations on non root fs nodes.
 * 
 * Revision 2.14  92/04/06  19:07:26  klh
 * For OSF merge, update version # to match LCC#
 *
 * Revision 2.10  92/04/05  16:48:32  pjg
 * 	Don't allow tty code to execute outside the root FS (rabii).
 * 
 * 	Add extra parameter to VPOP_SET_CTTY() and remove calls to macro
 * 	PPROC_CTTY_SETUP() which is now called implicitly by isctty().
 * 	(roman)
 * 
 * Revision 2.9  92/03/09  14:32:29  durriya
 * 	Revision 3.6  92/02/28  20:28:34  emcmanus
 * 	Fixes to drain output properly in certain cases, notable tcdrain().
 * 	(Some of these commented out for this installation, to be restored
 * 	after debugging.)
 * 	Characters must have their top bit stripped before being given to the
 * 	microkernel in 7-bit mode.
 * 
 * 	Revision 3.5  92/02/19  16:22:55  emcmanus
 * 	Don't support CS5 and CS6 options, precluded by microkernel interface.
 * 	Don't send BEL characters on input buffer overflow if also sending
 * 	XOFFs.
 * 
 * Revision 2.8  92/01/09  16:27:24  roy
 * 	92/01/07  11:15:20  chrisp
 * 	Remove redundant PPROC_CTTY_SETUPs in ttread() and ttwrite() and ensure
 * 	all others are within a master lock.
 * 
 * Revision 2.7  92/01/02  18:55:07  roy
 * 	1991/11/12  19:35:28  noemi
 * 	Changed arguments in call to clearalias, for OSF1_ADFS.
 * 
 * Revision 2.6  91/12/08  09:55:03  rabii
 * 	Merge of locus bug fixes
 * 	Use PPROC_CTTY_SETUP macro only for those ioctls needing p_flag set-up.
 * 	This avoids implied close of controlling terminals on process exit (via
 * 	no more sender notification) attempting to use a null vproc context.
 * 	(chrisp).
 * 
 * Revision 2.5  91/11/22  16:57:13  rabii
 * 	Locus Merge
 * 	Copious changes to handling controlling terminals correctly in the dummy
 * 	process environment of the fileserver. (chrisp)
 * 
 * 	Allow LOCATE_VPROC_PID() to return a null pointer, which must be
 * 	checked for and recovered from. (roman)
 * 
 * 	VPOP_GET_ATTR replaced VPOP_GET_PGRP_SID. (chrisp)
 * 
 * Revision 2.4  91/10/04  14:53:43  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:48:40  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Change to use vproc operations when handling controlling tty.
 * 	Also, process group and session structures replaced by ids.
 * 
 * Revision 2.2  91/08/31  13:23:37  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/27  15:27:06  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.3  91/08/05  13:55:51  sp
 * 2
 * 
 * Revision 1.18  90/10/31  13:49:53  devrcs
 * 	Added missing splx() in ttread().
 * 	[90/10/28  14:55:23  ers]
 * 
 * 	INFO => STATUS, FLUSHO => DISCARD
 * 	[90/10/14  13:44:57  ers]
 * 
 * 	posixtty hack is no longer needed.
 * 	Also, fixed ttread() to pick up value of lflag each time through
 * 	loop, instead of only at the beginning, since tp->t_lflag may
 * 	change while it is sleeping.
 * 	[90/10/12  19:58:44  ers]
 * 
 * 	Change sched_stamp to the new sleep_stamp.
 * 	[90/10/09  13:20:21  jvs]
 * 
 * Revision 1.17  90/10/07  13:19:40  devrcs
 * 	fixes for tty info support
 * 	[90/10/02  10:10:48  garyf]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:00:47  gm]
 * 
 * 	Moved handling of ONOCR flag up in ttyoutput, so that it would be
 * 	checked before we putc() the character.
 * 	[90/09/29  19:50:10  brezak]
 * 
 * 	Set t_col and t_rocol to 0 in ttyopen().
 * 	When a break is received in ttyinput(), it should ONLY check
 * 	IGNBRK and BRKINT when deciding whether to generate a SIGINT
 * 	signal and flush the queues.  It should not look at the settings
 * 	of ISIG and NOFLSH, nor should it check whether the interrupt
 * 	character is currently set to _POSIX_VDISABLE.
 * 	Move TSPLTTY and friends to tty.h.
 * 	Check for -1 for pgrp in TIOCSPGRP.
 * 	Set SIGTTOU if TIOCSPGRP ocurs in background.
 * 	Add posixtty to get a POSIX cc table.
 * 	ttwrite() returns EIO if a background process in an orphaned process
 * 	group attempts to write.
 * 	In ttylclose, turn off any flow control that's in effect, so that
 * 	output can drain.
 * 	[90/09/28  20:16:37  brezak]
 * 
 * 	For TCSBREAK drain output only if argument is -1 otherwise drain and
 * 	set break condition via driver. Return error if ttywait()
 * 	returns with error in TCSBREAK.
 * 	[90/09/19  21:57:58  brezak]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:34:24  jeffc]
 * 
 * Revision 1.16  90/09/23  15:43:36  devrcs
 * 	Fixed ttyinfo() not to called ttycheckoutq(), since the latter
 * 	locks the tty structure, which is already locked in ttyinfo().
 * 	Instead, ttyinfo() checks for space itself.
 * 	[90/09/17  19:10:37  ers]
 * 
 * 	Fixed printouts in ttyinfo().
 * 	[90/09/14  14:17:43  ers]
 * 
 * 	POSIX fix: if input baud rate is specified is zero, input baud rate
 * 	is taken from the output baud rate.
 * 	[90/09/07  09:12:52  ers]
 * 
 * 	Fixed typo in TIOCSETAF: was always flushing.
 * 	Fixed proc_compare.
 * 	Fixed TCSBREAK to return -1 so that driver break handling will occur.
 * 	[90/09/07  09:06:55  ers]
 * 
 * 	Fixed sleep problem in ttycheckoutq (bug #932).  It needs to call
 * 	mpsleep directly to get proper semantics.
 * 	[90/09/06  17:38:22  gmf]
 * 
 * Revision 1.15  90/09/13  11:42:30  devrcs
 * 	Fix ttysleep to guarantee that lock is locked on return (bug 796).
 * 	[90/08/27  12:52:19  gmf]
 * 
 * 	Fixed some problems with ttyinfo().  Also, sending of SIGINFO
 * 	needs to be surrounded by unix_master()/unix_release().
 * 	[90/08/23  19:36:17  ers]
 * 
 * Revision 1.14  90/08/24  11:19:07  devrcs
 * 	Eliminate dependencies on setjmp/longjmp.
 * 	Change TTY_SLEEP to use ttysleep, which can return interruptions
 * 	synchronously.
 * 	Upgraded to 4.3 reno code in some areas.
 * 	[90/08/20  03:36:29  gmf]
 * 
 * 	Changes for new system call interface (new pgsignal interface).
 * 	[90/08/17  17:39:41  nags]
 * 
 * 	Redid some of the controlling tty compatibility code.
 * 	[90/08/12  12:06:35  ers]
 * 
 * Revision 1.13  90/08/09  13:15:11  devrcs
 * 	Delete vcons, ttyloc support (bug 340).
 * 	[90/08/01  10:42:30  tmt]
 * 
 * 	Removed the stripping of the high bit and the check for special
 * 	characters from ttyecho().  The driver will now correctly echo 8-bit
 * 	characters.
 * 	Changed the tty info routine to display more information and send
 * 	SIGINFO.
 * 	[90/07/24  13:59:38  havens]
 * 
 * Revision 1.12  90/07/17  11:20:00  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:52:52  seiden]
 * 
 * Revision 1.11  90/06/29  13:35:01  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:54:19  seiden]
 * 
 * Revision 1.10  90/06/22  20:07:18  devrcs
 * 	Added in code for VTIME and VMIN processing in RAW mode.
 * 	[90/06/14  22:59:21  havens]
 * 
 * 	Added code for SVID compatibility.  Main output processing flags plus
 * 	the XCASE stuff. Removed JUNK_ ioctls and ignore cflags
 * 	[90/06/13  12:44:59  havens]
 * 
 * 	nags merge
 * 
 * 	Condensed relevant history (reverse chronology):
 * 	Secureware changes.				seiden@osf.org
 * 	Parallelized for OSF/1:  turned on tty locks.	nags@encore.com
 * 	Fixes for controlling tty.			ers@osf.org
 * 	New select interface				coren@osf.org
 * 	Posix ttys don't need ttylogout, ttydetach.	noemi@osf.org
 * 	Use EPERM, not ENOTTY, for non-existent pgrp.	ers@osf.org
 * 	4.4BSD:  pgrp structure, psignal not gsignal.	coren@osf.org
 * 	Merge error -  extra sleep in ttywait.		ers@osf.org
 * 	ENOTTY if trying to set tty into bogus pgrp.	ers@osf.org
 * 	Posix 1003.1 tty support:  termios, etc.	morris@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	4.4BSD:  uiomove, new suser interface.		ers@osf.org
 * 	Mach 2.5 and Encore 0.6 merge.			gm@osf.org
 * 	Merged Mach 2.5 with Encore parallelization	alan@encore.com
 * 	Fixed problem for TIOCSPGRP; security checking	boykin@encore.com
 * 	Fixed panic: ttyrub				boykin@encore.com
 * 	Added tty locks; sleep ==> TTY_SLEEP		alan@encore.com
 * 	Changed ttwrite_xcons to call (*xcons_write)()	berman@cmu.edu
 * 	Added declaration of selwait.			rpd@cmu.edu
 * 	Fix ttioctl so that it returns EINVAL on error	jsb@cmu.edu
 * 	Added TIOCVCONS and TIOCGCONS; cleanup		mja@cmu.edu
 * 	Virtual console fixes				mja@cmu.edu
 * 	Fixed ttyretype for t_rprntc 			dbg@cmu.edu
 * 	Corrected setting of cons_tp in ttyclose().	sanzi@cmu.edu
 * 	Implement TIOCCONS 				mja@cmu.edu
 * 	Corrected u_char problem in ttbreakc on the RT. mja@cmu.edu
 * 	Select routines check if thread is waiting	dbg@cmu.edu
 * 	Added several 'spltty's 			bolosky@cmu.edu
 * 	Changes for dynix				rvb@cmu.edu
 * 	Can't switch consoles on Multimax.		dlb@cmu.edu
 * 	Support for multiple threads.			avie@cmu.edu
 * 	Fix for first terminal open if host ID not set	mja@cmu.edu
 * 	Added terminal location init to ttyopen() 	mja@cmu.edu
 * 	ttsread, ttwrite use TS_ONDELAY if !TS_CARR_ON	rvb@cmu.edu
 * 	Upgraded to 4.3.				avie@cmu.edu
 * 	Fix ttwrite() to avoid dead-lock 		mja@cmu.edu
 * 	[90/06/12  21:17:06  gmf]
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
 *	%W% (Berkeley) %G%
 */

#include <ufs.h>
#include <sys/secdefines.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#define TTYDEFCHARS
#include <sys/tty.h>
#undef TTYDEFCHARS
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/vnode.h>
#include <sys/file.h>
#include <sys/conf.h>
#include <sys/dk.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#include <sys/poll.h>
#include <machine/reg.h>
#include <kern/parallel.h>
#include <kern/assert.h>
#include <mach/thread_info.h>
#if	SEC_BASE
#include <sys/security.h>
#endif

#include <cputypes.h>
#include <sys/termio.h>
#ifdef OSF1_SERVER
#include <mach/host_info.h>
#endif

#ifndef	TNC
extern	node_t		this_node;
extern	node_t		root_fs_node;
#endif	/* TNC */
short	tthiwat[NSPEEDS], ttlowat[NSPEEDS];

/* symbolic sleep message strings */
char ttyin[] = "ttyin";
char ttyout[] = "ttyout";
char ttopen[] = "ttyopn";
char ttclos[] = "ttycls";
char ttybg[] = "ttybg";
char ttybuf[] = "ttybuf";

extern ttin_timeout();
extern tty_start();

/*
 * Table giving parity for characters and indicating
 * character classes to tty driver. The 8th bit
 * indicates parity, the 7th bit indicates the character
 * is an alphameric or underscore (for ALTWERASE), and the
 * low 6 bits indicate delay type.  If the low 6 bits are 0
 * then the character needs no special processing on output.
 */

char partab[] = {
	0001,0201,0201,0001,0201,0001,0001,0201,	/* nul - bel */
	0202,0004,0003,0207,0005,0206,0201,0001,	/* bs - si */
	0201,0001,0001,0201,0001,0201,0201,0001,	/* dle - etb */
	0001,0201,0201,0001,0201,0001,0001,0201,	/* can - us */
	0200,0000,0000,0200,0000,0200,0200,0000,	/* sp - ' */
	0000,0200,0200,0000,0200,0000,0000,0200,	/* ( - / */
	0100,0300,0300,0100,0300,0100,0100,0300,	/* 0 - 7 */
	0300,0100,0000,0200,0000,0200,0200,0000,	/* 8 - ? */
	0200,0100,0100,0300,0100,0300,0300,0100,	/* @ - G */
	0100,0300,0300,0100,0300,0100,0100,0300,	/* H - O */
	0100,0300,0300,0100,0300,0100,0100,0300,	/* P - W */
	0300,0100,0100,0200,0000,0200,0200,0300,	/* X - _ */
	0000,0300,0300,0100,0300,0100,0100,0300,	/* ` - g */
	0300,0100,0100,0300,0100,0300,0300,0100,	/* h - o */
	0300,0100,0100,0300,0100,0300,0300,0100,	/* p - w */
	0100,0300,0300,0000,0200,0000,0000,0201,	/* x - del */
	/*
	 * meta chars
	 */
	0001,0201,0201,0001,0201,0001,0001,0201,	/* nul - bel */
	0202,0004,0003,0201,0005,0206,0201,0001,	/* bs - si */
	0201,0001,0001,0201,0001,0201,0201,0001,	/* dle - etb */
	0001,0201,0201,0001,0201,0001,0001,0201,	/* can - us */
	0200,0000,0000,0200,0000,0200,0200,0000,	/* sp - ' */
	0000,0200,0200,0000,0200,0000,0000,0200,	/* ( - / */
	0100,0300,0300,0100,0300,0100,0100,0300,	/* 0 - 7 */
	0300,0100,0000,0200,0000,0200,0200,0000,	/* 8 - ? */
	0200,0100,0100,0300,0100,0300,0300,0100,	/* @ - G */
	0100,0300,0300,0100,0300,0100,0100,0300,	/* H - O */
	0100,0300,0300,0100,0300,0100,0100,0300,	/* P - W */
	0300,0100,0100,0200,0000,0200,0200,0300,	/* X - _ */
	0000,0300,0300,0100,0300,0100,0100,0300,	/* ` - g */
	0300,0100,0100,0300,0100,0300,0300,0100,	/* h - o */
	0300,0100,0100,0300,0100,0300,0300,0100,	/* p - w */
	0100,0300,0300,0000,0200,0000,0000,0201,	/* x - del */
};

extern struct tty *constty;		/* temporary virtual console */

extern char partab[], maptab[];

/*
 * Is 'c' a line delimiter ("break" character)?
 */
#define ttbreakc(c) (c == '\n' || CCEQ(cc[VEOF], c) || \
		CCEQ(cc[VEOL], c) || CCEQ(cc[VEOL2], c))

ttychars(tp)
	struct tty *tp;
{
	LASSERT(TTY_LOCK_HOLDER(tp));
	bcopy(ttydefchars, tp->t_cc, sizeof(ttydefchars));
}

/*
 * Wait for output to drain, then flush input waiting.
 */
ttywflush(tp)
	struct tty *tp;
{
	int error;

	LASSERT(TTY_LOCK_HOLDER(tp));
	if ((error = ttywait(tp)) == 0)
		ttyflush(tp, FREAD);
	return (error);
}

/*
 * Wait for output to drain.
 */
int
ttywait(tp)
	register struct tty *tp;
{
	int error = 0;
	TSPLVAR(s)

	LASSERT(TTY_LOCK_HOLDER(tp));
	TSPLTTY(s);
	while ( tp->t_nwrites > 0 ||
	       (tp->t_outq.c_cc || tp->t_state&TS_BUSY) &&
#ifdef	ibmrt
	    tp->t_state&TS_CARR_ON && tp->t_oproc)	/* kludge for pty */
#else
	    (tp->t_state&TS_CARR_ON || tp->t_cflag&CLOCAL) &&
	    tp->t_oproc)
#endif
	{
		(*tp->t_oproc)(tp);
		tp->t_state |= TS_ASLEEP | TS_DRAINING;
		if (error = ttysleep(tp, (caddr_t)&tp->t_outq, TTOPRI | PCATCH,
				     ttyout))
			break;
	}
#ifdef OSF1_SERVER
	/* Mach typically replies to device_writes to a tty before the write
	   has actually completed.  This is clearly broken, but a naive fix
	   would have us introducing delays between receiving the reply and
	   sending the next device_write.  The Right Thing would be to
	   allow multiple device_writes to be outstanding, but for now we
	   explicitly sync with the microkernel here.  XXX */
		if (tp->t_oproc == tty_start)
			tty_drain(tp);
#endif
	tp->t_state &= ~TS_DRAINING;
	TSPLX(s);
	return (error);
}

/*
 * Flush all TTY queues
 */
ttyflush(tp, rw)
	register struct tty *tp;
{
	int error;

	TSPLVAR(s)

	LASSERT(TTY_LOCK_HOLDER(tp));
	TSPLTTY(s);
	if (rw & FREAD) {
		while (getc(&tp->t_canq) >= 0)
			;
		ttwakeup(tp);
	}
	if (rw & FWRITE) {
		thread_wakeup ((int) &tp->t_outq);
		tp->t_state &= ~TS_TTSTOP;
		CDEVSW_STOP(major(tp->t_dev), tp, rw, error);
		while (getc(&tp->t_outq) >= 0)
			;
		thread_wakeup ((int) &tp->t_outq);
	}
	if (rw & FREAD) {
		while (getc(&tp->t_rawq) >= 0)
			;
		tp->t_rocount = 0;
		tp->t_rocol = 0;
		tp->t_state &= ~TS_LOCAL;
	}
	TSPLX(s);
	select_wakeup(&tp->t_selq);
	/* This next call is needless, and possible dangerous - loverso */
	select_dequeue_all(&tp->t_selq);
}

/*
 * Send stop character on input overflow.
 */
ttyblock(tp)
	register struct tty *tp;
{
	register x;

	LASSERT(TTY_LOCK_HOLDER(tp));
	x = tp->t_rawq.c_cc + tp->t_canq.c_cc;
	if (tp->t_rawq.c_cc > TTYHOG) {
		ttyflush(tp, FREAD|FWRITE);
		tp->t_state &= ~TS_TBLOCK;
	}
	/*
	 * Block further input iff:
	 * Current input > threshold AND input is available to user program
	 */
	if (x >= TTYHOG/2 &&
	    (!(tp->t_lflag&ICANON)) || (tp->t_canq.c_cc > 0) &&
	    tp->t_cc[VSTOP] != _POSIX_VDISABLE) {
	    /* Even if we are XOFFed we should be able to send an XOFF,
	       so we need some magic to accomplish this. */
	    if ((tp->t_oproc == tty_start) ? tty_xoff(tp) :
		putc(tp->t_cc[VSTOP], &tp->t_outq)==0) {
	      tp->t_state |= TS_TBLOCK;
	      ttstart(tp);
	    }
	        if (putc(tp->t_cc[VSTOP], &tp->t_outq)==0) {
			tp->t_state |= TS_TBLOCK;
			ttstart(tp);
		}
	}
	LASSERT(TTY_LOCK_HOLDER(tp));
}

/*
 * Restart typewriter output following a delay
 * timeout.
 * The name of the routine is passed to the timeout
 * subroutine and it is called during a clock interrupt.
 *
 * WARNING:  this routine must always be called in thread
 * context and so can't be called directly from a clock interrupt!
 */
ttrstrt(tp)
	struct tty *tp;
{
	TSPLVAR(ipl)

	TSPLTTY(ipl);
	TTY_LOCK(tp);
	if (tp == 0)
		panic("ttrstrt");
	tp->t_state &= ~TS_TIMEOUT;
	ttstart(tp);
	TTY_UNLOCK(tp);
	TSPLX(ipl);
}

/*
 * Start output on the typewriter. It is used from the top half
 * after some characters have been put on the output queue,
 * from the interrupt routine to transmit the next
 * character, and after a timeout has finished.
 */
ttstart(tp)
	struct tty *tp;
{
	LASSERT(TTY_LOCK_HOLDER(tp));
	if (tp->t_oproc)		/* kludge for pty */
		(*tp->t_oproc)(tp);
}

/*
 * Common code for tty ioctls.
 */
/*ARGSUSED*/
ttioctl(tp, com, data, flag)
	register struct tty *tp;
	caddr_t data;
{
	extern int nldisp;
	int error;
	TSPLVAR(s)

	LASSERT(TTY_LOCK_HOLDER(tp));
	/*
	 * If the ioctl involves modification,
	 * hang if in the background.
	 */
	switch (com) {

	case TIOCSETD:
	case TIOCFLUSH:
	case TIOCSPGRP:
	case TIOCSTI:
	case TIOCSWINSZ:
	case TIOCSETA:
	case TIOCSETAW:
	case TIOCSETAF:
#ifdef COMPAT_43
	case TIOCSETP:
	case TIOCSETN:
	case TIOCSETC:
	case TIOCSLTC:
	case TIOCLBIS:
	case TIOCLBIC:
	case TIOCLSET:
	case OTIOCSETD:
#endif

/* SVID ioctls */
  	case TCXONC:
	case TCFLSH:
	case TCGETA:
	case TCSETAW:
	case TCSETAF:
	case TCSETA:
        case TCSBREAK:
		unix_master();
		while (isbackground(u.u_procp, tp)) {
			if (VPOP_TERMINAL_SIGPGRP(u.u_procp->p_vproc, SIGTTOU))
				break;
			unix_release();
			if (error = ttysleep(tp, (caddr_t)&lbolt,
				TTOPRI | PCATCH, ttybg)) {
				return (error);
			}
			unix_master();
		}
		unix_release();
		break;
	}

	/*
	 * Process the ioctl.
	 */
	switch (com) {

	/* get discipline number */
	case TIOCGETD:
#ifdef	balance
		if (tp->t_line == 2)
			*(int *)data = 1;
		else
#else
		*(int *)data = tp->t_line;
#endif
		break;

	/* set line discipline */
	case TIOCSETD: {
		register int t = *(int *)data;
		dev_t dev = tp->t_dev;
#ifdef	balance
		if (t == 1)
			t = 2;
#endif

		if ((unsigned)t >= nldisp)
			return (ENXIO);
		if (t != tp->t_line) {
			TSPLTTY(s);
			(*linesw[tp->t_line].l_close)(tp);
			error = (*linesw[t].l_open)(dev, tp, flag);
			if (error) {
				(void)(*linesw[tp->t_line].l_open)(dev, tp, flag);
				TSPLX(s);
				return (error);
			}
			tp->t_line = t;
			TSPLX(s);
		}
		break;
	}

	/* prevent more opens on channel */
	case TIOCEXCL:
		TSPLTTY(s);
		tp->t_state |= TS_XCLUDE;
		TSPLX(s);
		break;

	case TIOCNXCL:
		TSPLTTY(s);
		tp->t_state &= ~TS_XCLUDE;
		TSPLX(s);
		break;

	case TIOCHPCL:
		TSPLTTY(s);
		tp->t_state |= TS_HUPCLS;
		TSPLX(s);
		break;

	case TIOCFLUSH: {
		register int flags = *(int *)data;

		if (flags == 0)
			flags = FREAD|FWRITE;
		else
			flags &= FREAD|FWRITE;
		ttyflush(tp, flags);
		break;
	}

	case FIOASYNC:
		if (*(int *)data)
			tp->t_state |= TS_ASYNC;
		else
			tp->t_state &= ~TS_ASYNC;
		break;

	case FIONBIO:
		break;	/* XXX remove */

	/* return number of characters immediately available */
	case FIONREAD:
		TSPLTTY(s);
		*(off_t *)data = ttnread(tp);
		TSPLX(s);
		break;

	case TIOCOUTQ:
		*(int *)data = tp->t_outq.c_cc;
		break;

	case TIOCSTOP:
		TSPLTTY(s);
		if ((tp->t_state&TS_TTSTOP) == 0) {
			tp->t_state |= TS_TTSTOP;
			CDEVSW_STOP(major(tp->t_dev), tp, 0, error);
		}
		TSPLX(s);
		break;

	case TIOCSTART:
		TSPLTTY(s);
		if ((tp->t_state&TS_TTSTOP) || (tp->t_lflag&FLUSHO)) {
			tp->t_state &= ~TS_TTSTOP;
			tp->t_lflag &= ~FLUSHO;
			ttstart(tp);
		}
		TSPLX(s);
		break;

	/*
	 * Simulate typing of a character at the terminal.
	 */
	case TIOCSTI:
#if	SEC_BASE
		if ((flag & FREAD) == 0 && !privileged(SEC_ALLOWDACACCESS, 0))
			return (EPERM);
		unix_master();		/* only for isctty XXX */
		if (!isctty(u.u_procp, tp) && !privileged(SEC_ALLOWDACACCESS,0)) {
			unix_release();
			return (EACCES);
		}
#else
		if (u.u_uid && (flag & FREAD) == 0)
			return (EPERM);
		unix_master();		/* only for isctty XXX */
		if (u.u_uid && !isctty(u.u_procp, tp)) {
			unix_release();
			return (EACCES);
		}
#endif
		unix_release();
		TSPLTTY(s);
		(*linesw[tp->t_line].l_rint)(*(char *)data, tp);
		TSPLX(s);
		break;

	case TIOCGETA: {
		struct termios *t = (struct termios *)data;

		bcopy(&tp->t_termios, t, sizeof(struct termios));
		break;
	}

	case TIOCSETA:
	case TIOCSETAW:
	case TIOCSETAF: {
		register struct termios *t = (struct termios *)data;
#ifdef OSF1_SERVER	/* Microkernel does not support CS5 or CS6. */
		switch (t->c_cflag & CSIZE) {
		case CS5: case CS6:
			return EINVAL;
		}
#endif
		TSPLTTY(s);
		if (com == TIOCSETAW || com == TIOCSETAF) {
			if (error = ttywait(tp)) {
				TSPLX(s);
				return (error);
			}
			if (com == TIOCSETAF)
				ttyflush(tp, FREAD);
		}
			/*
			 * set device hardware
			 */
			if (tp->t_param && (error = (*tp->t_param)(tp, t))) {
				TSPLX(s);
				return (error);
			} else {
				if (!(tp->t_state&TS_CARR_ON) &&
				    (tp->t_cflag&CLOCAL) &&
				    !(t->c_cflag&CLOCAL)) {
					tp->t_state &= ~TS_ISOPEN;
					tp->t_state |= TS_WOPEN;
					ttwakeup(tp);
				}
				tp->t_cflag = t->c_cflag;
				tp->t_ospeed = t->c_ospeed;
				if (t->c_ispeed == 0)
					tp->t_ispeed = t->c_ospeed;
				else
					tp->t_ispeed = t->c_ispeed;
			}
			ttsetwater(tp);
		if (com != TIOCSETAF) {
			if ((t->c_lflag&ICANON) != (tp->t_lflag&ICANON))
				if (t->c_lflag&ICANON) {	
					tp->t_lflag |= PENDIN;
					ttwakeup(tp);
				}
				else {
					struct clist tq;

					catq(&tp->t_rawq, &tp->t_canq);
					tq = tp->t_rawq;
					tp->t_rawq = tp->t_canq;
					tp->t_canq = tq;
				}
		}
		tp->t_iflag = t->c_iflag;
		tp->t_oflag = t->c_oflag;
		tp->t_lflag = t->c_lflag;
		bcopy(t->c_cc, tp->t_cc, sizeof(t->c_cc));
		tp->t_shad_time = t->c_cc[VTIME] * hz / 10;
		TSPLX(s);
		break;
	}

	/*
	 * Set controlling terminal.
	 * Session ctty vnode pointer set in vnode layer.
	 */
	case TIOCSCTTY: {
		register struct proc *p;

		unix_master();
		p = u.u_procp;
		if ((p->p_pid != p->p_sid ) ||
		   (tp->t_sid) && (tp->t_sid != p->p_sid)) {
			unix_release();
			return (EPERM);
		}
		tp->t_sid = p->p_sid;
		tp->t_pgid = p->p_pgid;
		(void) VPOP_SET_CTTY(p->p_vproc, p->p_vproc, tp, VPROC_SET);	
		p->p_flag |= SCTTY;
		unix_release();
		break;
	}
		
	/*
	 * Set terminal process group.
	 */
	case TIOCSPGRP: {
		register struct proc *p;
		register struct vproc *v;
		register struct vproc *g;
		pid_t pgrp_sid;
		int jobc, is_pgrpleader = FALSE;
		int error;

		unix_master();
		p = u.u_procp;
		v = p->p_vproc;
                if (*(int *)data == -1) {
                        unix_release();
                        return (EINVAL);
                }
		if (!isctty(p, tp)) {
			unix_release();
			return (ENOTTY);
		}
		if ((g = LOCATE_VPROC_PID(*(pid_t *) data)) != NULL) {
			error = VPOP_GET_ATTR(g, 0, 0, &pgrp_sid, 0, 0, 
					      &jobc, 0);
			if (error) { 
				unix_release();
				return (error);
			}

			is_pgrpleader = (jobc != VPROC_NOT_PGRPLEADER);
			VPROC_RELEASE(g, "ttioctl(TIOCSPGRP)");
		}
#ifdef COMPAT_43
                /*
                 * C-shell tries to set the tty's pgrp before it
                 * makes itself a pgrp leader.  Therefore, we now
                 * create a new process group, in the expectation
                 * that the program is about to do a setpgrp(0, pid).
                 * The fact that the C shell uses such a sequence
                 * is wrong, but we're trying to maintain compatibility.
                 */
                if (!is_pgrpleader && *(int *)data == p->p_pid) {
			if (VPOP_SETPGID(v, v, p->p_pid, p->p_sid) != 0) {
				unix_release();
				return(EPERM);
			}
                } else
#endif /* COMPAT_43 */
                if (!is_pgrpleader || pgrp_sid != p->p_sid) {
			unix_release();
			return (EPERM);
		}
		tp->t_pgid = *(pid_t *)data;
		unix_release();
		break;
	}

	case TIOCGPGRP:
		unix_master();
		if (!isctty(u.u_procp, tp)) {
			unix_release();
			return (ENOTTY);
		}
		*(int *)data = tp->t_pgid;
		unix_release();
		break;

	case TIOCSWINSZ:
		if (bcmp((caddr_t)&tp->t_winsize, data,
		    sizeof (struct winsize))) {
			tp->t_winsize = *(struct winsize *)data;
			unix_master();
			gsignal_chktty(tp->t_pgid, SIGWINCH);
			unix_release();
		}
		break;

	case TIOCGWINSZ:
		*(struct winsize *)data = tp->t_winsize;
		break;

	case TIOCCONS:
		if (*(int *)data) {
			if (constty != NULL)
				return (EBUSY);
#ifndef	UCONSOLE
#if	SEC_BASE
			if (!privileged(SEC_ALLOWDACACCESS, 0))
				return (EPERM);
#else
			if (error = suser(u.u_cred, &u.u_acflag))
				return (error);
#endif
#endif
			constty = tp;
		} else if (tp == constty)
			constty = NULL;
		break;

#ifdef COMPAT_43
	case TIOCGETP:
	case TIOCSETP:
	case TIOCSETN:
	case TIOCGETC:
	case TIOCSETC:
	case TIOCSLTC:
	case TIOCGLTC:
	case TIOCLBIS:
	case TIOCLBIC:
	case TIOCLSET:
	case TIOCLGET:
	case OTIOCGETD:
	case OTIOCSETD:
		return(ttcompat(tp, com, data, flag));
#endif

/* SVID ioctls */
  	case TCXONC:
	case TCFLSH:
	case TCGETA:
	case TCSETAW:
	case TCSETAF:
	case TCSETA:
		return(tt_sysv_compat(tp, com, data, flag));
		
	case TCSBREAK:
		if (error = ttywait(tp))
                        return (error);
                if (*(int *)data >= 0)
                        return (-1);
                break;
                
	default:
		return (-1);
	}
	return (0);
}

ttnread(tp)
	struct tty *tp;
{
	int nread = 0;

	LASSERT(TTY_LOCK_HOLDER(tp));
	if (tp->t_lflag & PENDIN)
		ttypend(tp);
	nread = tp->t_canq.c_cc;
	if ((tp->t_lflag & ICANON) == 0)
		nread += tp->t_rawq.c_cc;
	return (nread);
}

int
#ifdef OSF1_ADFS
ttselect(dev, events, revents, scanning, node)
#else
ttselect(dev, events, revents, scanning)
#endif
	int scanning;
	dev_t dev;
	short *events, *revents;
#ifdef OSF1_ADFS
        node_t    node;
#endif
{
	register struct tty *tp;
	int nread, enqueue = 0;
	TSPLVAR(s)

	TSPLTTY(s);
	CDEVSW_TTYS(major(dev), minor(dev), tp);
	DEVSW_FUNNEL(c,major(dev));
	TTY_LOCK(tp);

	if (scanning) {
		if (*events & POLLNORM) {
			nread = ttnread(tp);
			if (nread > 0 ||
			   (!(tp->t_cflag&CLOCAL) && !(tp->t_state&TS_CARR_ON)))
				*revents |= POLLNORM;
			else
				enqueue++;
		}
		if (*events & POLLOUT) {
			if (tp->t_outq.c_cc <= tp->t_lowat)
				*revents |= POLLOUT;
			else
				enqueue++;
		}

		if (!(tp->t_state&TS_CARR_ON))
			*revents |= POLLHUP;
		
		/* Only enqueue if we don't have a response */
		if (*revents == 0 && enqueue)
			select_enqueue(&tp->t_selq);
	} else
		select_dequeue(&tp->t_selq);

	TTY_UNLOCK(tp);
	DEVSW_UNFUNNEL(c,major(dev));
	TSPLX(s);
	return (0);
}

#ifdef	balance
/*
 * Sequent drivers allocate their tty structures at boot time to allow
 * for binary configuration and more flexibility in minor number use.
 * Thus, need a version of ttselect() that takes a struct tty pointer,
 * rather than assuming it knows how to generate it.
 */
/*
 * XXX -- needs fixing for new interface.  This won't work anymore!
 */

tpselect(tp, events, revents, scanning)
	register struct tty *tp;
	short *events, *revents;
	int scanning;
{
	int nread, enqueue = 0;
	TSPLVAR(s)

	TSPLTTY(s);
	LASSERT(TTY_LOCK_HOLDER(tp));

	if (scanning) {
		if (*events & POLLNORM) {
			nread = ttnread(tp);
			if (nread > 0 ||
			(!(tp->t_cflag&CLOCAL) && !(tp->t_state&TS_CARR_ON)))
				*revents |= POLLNORM;
			else
				enqueue++;
		}
		if (*events & POLLOUT) {
			if (tp->t_outq.c_cc <= tp->t_lowat)
				*revents |= POLLOUT;
			else
				enqueue++;
		}

		if (!(tp->t_state&TS_CARR_ON))
			*revents |= POLLHUP;

		if (enqueue)
			select_enqueue(&tp->t_selq);
	} else
		select_dequeue(&tp->t_selq);

	TSPLX(s);
	return (0);
}
#endif	/* balance */
/*
 * Initial open of tty, or (re)entry to line discipline.
 */
ttyopen(dev, tp, flags)
	dev_t dev;
	register struct tty *tp;
	int flags;
{
	register struct proc *pp;

	pp = u.u_procp;

#ifndef	TNC
	/* tty code is only allowed on the root fs */
	if (this_node != root_fs_node) {
		return (EACCES);
	}
#endif	/* TNC */
	LASSERT(TTY_LOCK_HOLDER(tp));
	tp->t_dev = dev;
	tp->t_shad_time = tp->t_cc[VTIME] * hz / 10;

#ifdef COMPAT_43
	unix_master();
	if (!(flags&O_NOCTTY) && pp->p_pgid == 0 && tp->t_sid == 0) {
		if ((pp->p_pid != pp->p_sid) && pp->p_pgid != pp->p_pid) {
			(void) VPOP_SETSID(pp->p_vproc);
		}
		unix_release();
		(void) ttioctl(tp, TIOCSCTTY, 0, 0);
	}
	else
		unix_release();
#endif /* COMPAT_43 */

	tp->t_state &= ~TS_WOPEN;
	if ((tp->t_state & TS_ISOPEN) == 0) {
		tp->t_state |= TS_ISOPEN;
		bzero((caddr_t)&tp->t_winsize, sizeof(tp->t_winsize));
	}
	return (0);
}

/*
 * "close" a line discipline
 */
ttylclose(tp)
	register struct tty *tp;
{

	LASSERT(TTY_LOCK_HOLDER(tp));
	/* Restart output if blocked */
	ttioctl(tp, TIOCSTART, 0, 0);
	ttywflush(tp);
}

/*
 * clean tp on last close
 */
ttyclose(tp)
	register struct tty *tp;
{
	if (constty == tp)
		constty = NULL;
	LASSERT(TTY_LOCK_HOLDER(tp));
	ttyflush(tp, FREAD|FWRITE);
	tp->t_sid = 0;
	tp->t_pgid = 0;
	tp->t_state = 0;
	tp->t_col = tp->t_rocol = 0;
	tp->t_gen++;
	return (0);
}

/*
 * Handle modem control transition on a tty.
 * Flag indicates new state of carrier.
 * Returns 0 if the line should be turned off, otherwise 1.
 */
ttymodem(tp, flag)
	register struct tty *tp;
{

	int error;

	LASSERT(TTY_LOCK_HOLDER(tp));
	if ((tp->t_state&TS_WOPEN) == 0 && (tp->t_lflag & MDMBUF)) {
		/*
		 * MDMBUF: do flow control according to carrier flag
		 */
		if (flag) {
			tp->t_state &= ~TS_TTSTOP;
			ttstart(tp);
		} else if ((tp->t_state&TS_TTSTOP) == 0) {
			tp->t_state |= TS_TTSTOP;
			CDEVSW_STOP(major(tp->t_dev), tp, 0, error);
		}
	} else if (flag == 0) {
		/*
		 * Lost carrier.
		 */
		tp->t_state &= ~TS_CARR_ON;
		if (tp->t_state&TS_ISOPEN && (tp->t_cflag&CLOCAL) == 0) {
			if (tp->t_sid) {
				register struct vproc *s;
				unix_master();
				s = LOCATE_VPROC_PID(tp->t_sid);
				if (s != 0) {
					(void) VPOP_SIGPROC(s, 
							    SIGHUP,
							    0,
							    VPROC_HAS_PRIV);	
					VPROC_RELEASE(s,"ttymodem()");
				}
				unix_release();
			}
			ttyflush(tp, FREAD|FWRITE);
			return (0);
		}
	} else {
		/*
		 * Carrier now on.
		 */
		tp->t_state |= TS_CARR_ON;
		ttwakeup(tp);
	}
	return (1);
}

/*
 * Default modem control routine (for other line disciplines).
 * Return argument flag, to turn off device on carrier drop.
 */
nullmodem(tp, flag)
	register struct tty *tp;
	int flag;
{
	LASSERT(TTY_LOCK_HOLDER(tp));
	if (flag)
		tp->t_state |= TS_CARR_ON;
	else {
		tp->t_state &= ~TS_CARR_ON;
		if ((tp->t_cflag & CLOCAL) == 0) {
			if (tp->t_sid) {
				register struct vproc *s;
				s = LOCATE_VPROC_PID(tp->t_sid);
				if (s != 0) {
					(void) VPOP_SIGPROC(s, 
							    SIGHUP, 
							    0,
							    VPROC_HAS_PRIV);	
					VPROC_RELEASE(s,"nullmodem()");
				}
				unix_release();
				return (0);
			}
		}
	}
	return (1);
}

/*
 * reinput pending characters after state switch
 * call at spltty().
 */
ttypend(tp)
	register struct tty *tp;
{
	struct clist tq;
	register c;

	LASSERT(TTY_LOCK_HOLDER(tp));
	tp->t_lflag &= ~PENDIN;
	tp->t_state |= TS_TYPEN;
	tq = tp->t_rawq;
	tp->t_rawq.c_cc = 0;
	tp->t_rawq.c_cf = tp->t_rawq.c_cl = 0;
	while ((c = getc(&tq)) >= 0)
		ttyinput(c, tp);
	tp->t_state &= ~TS_TYPEN;
}

/*
 *
 * Place a character on raw TTY input queue,
 * putting in delimiters and waking up top
 * half as needed.  Also echo if required.
 * The arguments are the character and the
 * appropriate tty structure.
 */
ttyinput(c, tp)
	register c;
	register struct tty *tp;
{
	register int iflag = tp->t_iflag;
	register int lflag = tp->t_lflag;
	register u_char *cc = tp->t_cc;
	int i, err;

	LASSERT(TTY_LOCK_HOLDER(tp));
	/*
	 * If input is pending take it first.
	 */
	if (lflag&PENDIN)
		ttypend(tp);
	/*
	 * Gather stats.
	 */
	tk_nin++;
	if (lflag&ICANON) {
		tk_cancc++;
		tp->t_cancc++;
	} else {
		tk_rawcc++;
		tp->t_rawcc++;
	}
	/*
	 * Handle exceptional conditions (break, parity, framing).
	 */
	if (err = (c&TTY_ERRORMASK)) {
		c &= ~TTY_ERRORMASK;
		if ((err&TTY_FE) && !c) {	/* break */
			if (iflag&IGNBRK)
				goto endcase;
                        else if (iflag&BRKINT) {
                                ttyflush(tp, FREAD|FWRITE);
                                unix_master();
                                gsignal_chktty(tp->t_pgid, SIGINT);
                                unix_release();
                                goto endcase;
                        }
			else {
				c = 0;
				if (iflag&PARMRK)
					goto parmrk;
			}
		} else if (((err&TTY_PE) && (iflag&INPCK)) || (err&TTY_FE)) {
			if (iflag&IGNPAR)
				goto endcase;
			else if (iflag&PARMRK) {
parmrk:
				putc(0377|TTY_QUOTE, &tp->t_rawq);
				putc(0|TTY_QUOTE, &tp->t_rawq);
				putc(c|TTY_QUOTE, &tp->t_rawq);
				if (tp->t_rawq.c_cc >= tp->t_cc[VMIN])
				  ttwakeup(tp);
				goto endcase;
			} else
				c = 0;
		}
	}
	/*
	 * In tandem mode, check high water mark.
	 */
	if (iflag&IXOFF)
		ttyblock(tp);
	if ((tp->t_state&TS_TYPEN) == 0 && (iflag&ISTRIP))
		c &= 0177;
	else {
		if ((iflag & PARMRK) && !(iflag & ISTRIP) && c == 0377) {
			putc(0377|TTY_QUOTE, &tp->t_rawq);
			putc(0377|TTY_QUOTE, &tp->t_rawq);
			goto endcase;
		}
	}		
			
	/*
	 * Check for literal nexting very first
	 */
	if (tp->t_state&TS_LNCH) {
		c |= TTY_QUOTE;
		tp->t_state &= ~TS_LNCH;
	}
	/*
	 * Scan for special characters.  This code
	 * is really just a big case statement with
	 * non-constant cases.  The bottom of the
	 * case statement is labeled ``endcase'', so goto
	 * it after a case match, or similar.
	 */
	/*
	 * Control chars which aren't controlled
	 * by ICANON, ISIG, or IXON.
	 */
	if (lflag&IEXTEN) {
		if (CCEQ(cc[VLNEXT],c)) {
			if (lflag&ECHO) {
				if (lflag&ECHOE)
					ttyoutstr("^\b", tp);
				else
					ttyecho(c, tp);
			}
			tp->t_state |= TS_LNCH;
			goto endcase;
		}
		if (CCEQ(cc[VDISCARD],c)) {
			if (lflag&FLUSHO)
				tp->t_lflag &= ~FLUSHO;
			else {
				ttyflush(tp, FWRITE);
				ttyecho(c, tp);
				if (tp->t_rawq.c_cc + tp->t_canq.c_cc)
					ttyretype(tp);
				tp->t_lflag |= FLUSHO;
			}
			goto startoutput;
		}
	}
	/*
	 * Signals.
	 */
	if (lflag&ISIG) {
		if (CCEQ(cc[VINTR], c) || CCEQ(cc[VQUIT], c)) {
			if ((lflag&NOFLSH) == 0)
				ttyflush(tp, FREAD|FWRITE);
			ttyecho(c, tp);
#ifndef TNC
			unix_master();
#endif
			gsignal4(tp->t_pgid,
				CCEQ(cc[VINTR],c) ? SIGINT : SIGQUIT, 1, FALSE);
#ifndef TNC
			unix_release();
#endif
			goto endcase;
		}		
		if (CCEQ(cc[VSUSP],c)) {
			if ((lflag&NOFLSH) == 0)
				ttyflush(tp, FREAD);
			ttyecho(c, tp);
#ifndef TNC
			unix_master();
#endif
			gsignal4(tp->t_pgid, SIGTSTP, 1, FALSE);
#ifndef TNC
			unix_release();
#endif
			goto endcase;
		}
		if (CCEQ(cc[VSTATUS],c)) {
#ifndef TNC
			unix_master();
#endif
			gsignal4(tp->t_pgid, SIGINFO, 1, FALSE);
#ifndef TNC
			unix_release();
#endif
			if ((lflag & NOKERNINFO) == 0)
				ttyinfo(tp);
			goto endcase;
		}
	}
	/*
	 * Handle start/stop characters.
	 */
	if (iflag&IXON) {
		if (CCEQ(cc[VSTOP],c)) {
			if ((tp->t_state&TS_TTSTOP) == 0) {
				tp->t_state |= TS_TTSTOP;
				CDEVSW_STOP(major(tp->t_dev), tp, 0, err);
				return;
			}
			if (!CCEQ(cc[VSTART], c))
				return;
			/*
			 * if VSTART == VSTOP then toggle
			 */
			goto endcase;
		}
		if (CCEQ(cc[VSTART], c))
			goto restartoutput;
	}
	/*
	 * IGNCR, ICRNL, & INLCR
	 */
	if (c == '\r') {
		if (iflag&IGNCR)
			goto endcase;
		else if (iflag&ICRNL)
			c = '\n';
	}
	else if (c == '\n' && iflag&INLCR)
		c = '\r';

				/* Map Upper case to lower case */
	if (iflag&IUCLC && 'A' <= c && c <= 'Z')
	    c += 'a' - 'A';

	/*
	 * Non canonical mode; don't process line editing
	 * characters; check high water mark for wakeup.
	 *
	 */
	if (!(lflag&ICANON)) {
		if (tp->t_rawq.c_cc > TTYHOG) {
			if ((iflag&(IMAXBEL|IXOFF)) == IMAXBEL) {
				if (tp->t_outq.c_cc < tp->t_hiwat)
					(void) ttyoutput(CTRL('g'), tp);
			} else
				ttyflush(tp, FREAD | FWRITE);
		} else {
			if (putc(c, &tp->t_rawq) >= 0) {
				ttyecho(c, tp);
				if (tp->t_rawq.c_cc >= tp->t_cc[VMIN])
				    ttwakeup(tp);
				else if (tp->t_shad_time > 0) {
 					if (tp->t_state & TS_INTIMEOUT)
						untimeout(ttin_timeout, tp);
					timeout(ttin_timeout, (caddr_t) tp,
						tp->t_shad_time);
					tp->t_state |= TS_INTIMEOUT;
				}
				
			}
		}
		goto endcase;
	}
	/*
	 * From here on down canonical mode character
	 * processing takes place.
	 */
	/*
	 * erase (^H / ^?)
	 */
	if (CCEQ(cc[VERASE], c)) {
		if (tp->t_rawq.c_cc)
			ttyrub(unputc(&tp->t_rawq), tp);
		goto endcase;
	}
	/*
	 * kill (^U)
	 */
	if (CCEQ(cc[VKILL], c)) {
		if (lflag&ECHOKE && tp->t_rawq.c_cc == tp->t_rocount &&
		    !(lflag&ECHOPRT)) {
			while (tp->t_rawq.c_cc)
				ttyrub(unputc(&tp->t_rawq), tp);
		} else {
			ttyecho(c, tp);
			if (lflag&ECHOK || lflag&ECHOKE)
				ttyecho('\n', tp);
			while (getc(&tp->t_rawq) > 0)
				;
			tp->t_rocount = 0;
		}
		tp->t_state &= ~TS_LOCAL;
		goto endcase;
	}
	/*
	 * word erase (^W)
	 */
	if (CCEQ(cc[VWERASE], c)) {	
		int ctype;

#define CTYPE(c) ((lflag&ALTWERASE) ? (partab[(c)&TTY_CHARMASK]&0100) : 0)
		/*
		 * erase whitespace
		 */
		while ((c = unputc(&tp->t_rawq)) == ' ' || c == '\t')
			ttyrub(c, tp);
		if (c == -1)
			goto endcase;
		/*
		 * special case last char of token
		 */
		ttyrub(c, tp);
		c = unputc(&tp->t_rawq);
		if (c == -1 || c == ' ' || c == '\t') {
			if (c != -1)
				(void) putc(c, &tp->t_rawq);
			goto endcase;
		}
		/*
		 * erase rest of token
		 */
		ctype = CTYPE(c);
		do {
			ttyrub(c, tp);
			c = unputc(&tp->t_rawq);
			if (c == -1)
				goto endcase;
		} while (c != ' ' && c != '\t' && CTYPE(c) == ctype);
		(void) putc(c, &tp->t_rawq);
		goto endcase;
#undef CTYPE
	}
	/*
	 * reprint line (^R)
	 */
	if (CCEQ(cc[VREPRINT], c)) {
		ttyretype(tp);
		goto endcase;
	}
	/*
	 * Check for input buffer overflow
	 */
	if (tp->t_rawq.c_cc+tp->t_canq.c_cc > TTYHOG) {
		if (iflag&IMAXBEL) {
			if (tp->t_outq.c_cc < tp->t_hiwat)
				(void) ttyoutput(CTRL('g'), tp);
		} else
			ttyflush(tp, FREAD | FWRITE);
		goto endcase;
	}

	/*
	 * Put data char in q for user and
	 * wakeup on seeing a line delimiter.
	 */
	if (putc(c, &tp->t_rawq) >= 0) {
		if (ttbreakc(c)) {
			tp->t_rocount = 0;
			catq(&tp->t_rawq, &tp->t_canq);
			ttwakeup(tp);
		} else if (tp->t_rocount++ == 0)
			tp->t_rocol = tp->t_col;
		if (tp->t_state&TS_ERASE) {
			/*
			 * end of prterase \.../
			 */
			tp->t_state &= ~TS_ERASE;
			(void) ttyoutput('/', tp);
		}
		i = tp->t_col;
		ttyecho(c, tp);
		if (CCEQ(cc[VEOF], c) && lflag&ECHO) {
			/*
			 * Place the cursor over the '^' of the ^D.
			 */
			i = MIN(2, tp->t_col - i);
			while (i > 0) {
				(void) ttyoutput('\b', tp);
				i--;
			}
		}
	}
endcase:
	/*
	 * IXANY means allow any character to restart output.
	 */
	if ((tp->t_state&TS_TTSTOP) && !(iflag&IXANY)
	    && cc[VSTART] != cc[VSTOP])
		return;
restartoutput:
	tp->t_state &= ~TS_TTSTOP;
	tp->t_lflag &= ~FLUSHO;
startoutput:
	ttstart(tp);
}

/*
 * Put character on TTY output queue, adding delays,
 * expanding tabs, and handling the CR/NL bit.
 * This is called both from the top half for output,
 * and from interrupt level for echoing.
 * The arguments are the character and the tty structure.
 * Returns < 0 if putc succeeds, otherwise returns char to resend
 * Must be recursive.
 */
ttyoutput(c, tp)
	register c;
	register struct tty *tp;
{
	register char *colp;
	register ctype;
	register long oflag = tp->t_oflag;
	
	LASSERT(TTY_LOCK_HOLDER(tp));

	if (!(oflag&OPOST) || (c & TTY_QUOTE)) {
		if (tp->t_lflag&FLUSHO)
			return (-1);
		if (putc(c & TTY_CHARMASK, &tp->t_outq))
			return (c);
		tk_nout++;
		tp->t_outcc++;
		return (-1);
	}
	c &= TTY_CHARMASK;
	/*
	 * Turn tabs to spaces as required
	 */

	if (c == '\t' && ((oflag&OXTABS)|| ((oflag & TABDLY) == TAB3))) {
		TSPLVAR(s)

		c = 8 - (tp->t_col&7);
		if ((tp->t_lflag&FLUSHO) == 0) {
			TSPLTTY(s);		/* don't interrupt tabs */
			c -= b_to_q("        ", c, &tp->t_outq);
			tk_nout += c;
			tp->t_outcc += c;
			TSPLX(s);
		}
		tp->t_col += c;
		return (c ? -1 : '\t');
	}
	if (c == CEOT && (oflag&ONOEOT))
		return(-1);
	tk_nout++;
	tp->t_outcc++;
	/*
	 * Generate escapes for upper-case-only terminals.
	 */
	if (tp->t_lflag&XCASE) {
		colp = "({)}!|^~'`\\\\";
		while(*colp++)
		    if (c == *colp++) {
			    ttyoutput('\\'|TTY_QUOTE, tp);
			    c = colp[-2];
			    break;
		    }
		if ('A' <= c && c <= 'Z')
		    ttyoutput('\\' | TTY_QUOTE, tp);
	}

	if (oflag&OLCUC && 'a' <= c && c <= 'z')
	    c += 'A' - 'a';
	
	/*
	 * turn <nl> to <cr><lf> if desired.
	 */
	if (c == '\n' && (oflag&ONLCR) && ttyoutput('\r', tp) >= 0)
		return (c);

	if (c == '\r' && (oflag&ONOCR) && tp->t_col == 0)
		return(-1);
	if (c == '\r' && (oflag&OCRNL)) {
		c = '\n';
		if ((tp->t_lflag&FLUSHO) == 0 && putc(c, &tp->t_outq))
			return ('\r');
	}
	else {
		if ((tp->t_lflag&FLUSHO) == 0 && putc(c, &tp->t_outq))
			return (c);

		if (c == '\n' && (oflag & ONLRET)) {
			c = '\r';
		}
	}
	    
	/*
	 * Calculate delays.
	 * The numbers here represent clock ticks
	 * and are not necessarily optimal for all terminals.
	 *
	 * SHOULD JUST ALLOW USER TO SPECIFY DELAYS
	 *
	 * (actually, should THROW AWAY terminals which need delays)
	 */
	colp = &tp->t_col;
	ctype = partab[c];
	c = 0;
	switch (ctype&077) {

	case ORDINARY:
		(*colp)++;

	case CONTROL:
		break;


	/*
	 * This macro is close enough to the correct thing;
	 * it should be replaced by real user settable delays
	 * in any event...
	 */
#define mstohz(ms)	((((ms) * hz) >> 10) & 0X0FF)
	case BACKSPACE:
		if (oflag & BSDLY)
			if (oflag & OFILL)
				c = 1;
			else
				 c = mstohz(100);
			
		if (*colp)
			(*colp)--;
		break;
	case NEWLINE:
		ctype = oflag & NLDLY;
		if (ctype == NL2) { /* tty 37 */
			if (*colp > 0) {
				c = (((unsigned)*colp) >> 4) + 3;
				if ((unsigned)c > 6)
					c = mstohz(60);
				else
					c = mstohz(c * 10);
			}
		}
		else if (ctype == NL1) /* vt05 */
			if (oflag & OFILL)
				c = 2;
			else
				c = mstohz(100);
		/* *colp = 0; */
		break;

	case TAB:
		ctype = oflag & TABDLY;
		if (ctype == TAB1) { /* tty 37 */
			c = 1 - (*colp | ~07);
			if (c < 5)
				c = 0;
			else
			    c = mstohz(10 * c);
			
		}
		else if (ctype == TAB2)
			c = mstohz(200);
				/* TAB3 is handled earlier. */
		else
			c = 0;
		if (ctype && (oflag & OFILL))
		    c = 2;
		
		*colp |= 07;
		(*colp)++;
		break;

	case VTAB:
		if (oflag&VTDELAY) /* tty 37 */
			c =  177;
		break;

	case RETURN:
		ctype = oflag & CRDLY;
		if (ctype == CR2) /* tn 300 */
			if (oflag & OFILL)
				c = 2;
			else
				c = mstohz(100);
		else if (ctype == CR3) /* ti 700 */
			c = mstohz(166);
		else if (ctype == CR1) { /* concept 100 */
			    
			if (oflag & OFILL)
				c = 4;
			else {
				c = (*colp >>4) + 3;
				c = c < 6 ? 6 : c;
				c = mstohz(c * 10);
			}
		    
			
		}
		*colp = 0;
		break;
		
	case FF:
		if (oflag & FFDLY)
			c = 0177;
		break;
			
	}
	
	if (c && (tp->t_lflag&FLUSHO) == 0) {
		if (oflag & OFILL) {
			ctype = oflag & OFDEL ? '\177' : '\0';
			for(;c > 0;c--)
				(void)putc(ctype, &tp->t_outq);
		}
		else 
			(void) putc(c|TTY_QUOTE, &tp->t_outq);
	}
	return (-1);
}
	
#undef mstohz

struct{
	char from;
	char to;
	}xcase_map[] = 
{
	{'\'', '`'},
	{'!', '|'},
	{'^', '~'},    
	{'(', '{'},    
	{')', '}'},
	{'\\', '\\'},
	{'\0', '\0'}
	 
};


/*
 * Called from device's read routine after it has
 * calculated the tty-structure given as argument.
 */
ttread(tp, uio, flag)
	register struct tty *tp;
	struct uio *uio;
{
	register struct clist *qp;
	register int c;
	register long lflag;
	register u_char *cc = tp->t_cc;
	int first, error = 0;
	TSPLVAR(s)

	LASSERT(TTY_LOCK_HOLDER(tp));
	first = 1;
	
loop:
	lflag = tp->t_lflag;
	TSPLTTY(s);
	/*
	 * take pending input first
	 */
	if (lflag&PENDIN)
		ttypend(tp);
	/*
	 * Handle carrier.
	 */
	if (!(tp->t_state&TS_CARR_ON) && !(tp->t_cflag&CLOCAL)) {
		if (tp->t_state&TS_ISOPEN) {
			TSPLX(s);
			return (0);	/* EOF */
		} else if (flag & IO_NDELAY) {
			TSPLX(s);
			return (EWOULDBLOCK);
		} else {
			/*
			 * sleep awaiting carrier
			 */
			error = ttysleep(tp, (caddr_t)&tp->t_rawq, 
				TTIPRI | PCATCH, ttyin);
			TSPLX(s);
			if (error)
				return (error);
			goto loop;
		}
	}
	TSPLX(s);
	/*
	 * Hang process if it's in the background.
	 */
	unix_master();
	if (isbackground(u.u_procp, tp)) {
		if (VPOP_TERMINAL_SIGPGRP(u.u_procp->p_vproc, SIGTTIN)) {
			unix_release();
			return (EIO);
		}
		unix_release();
		if (error = ttysleep(tp, (caddr_t)&lbolt, TTIPRI | PCATCH, 
				     ttybg))
			return (error);
		goto loop;
	}
	unix_release();
	/*
	 * If canonical, use the canonical queue,
	 * else use the raw queue.
	 *
	 * XXX - should get rid of canonical queue.
	 * (actually, should get rid of clists...)
	 */
	if (lflag&ICANON) {
		qp = &tp->t_canq;
		
		TSPLTTY(s);
		if (qp->c_cc <= 0) {
			/** XXX ??? ask mike why TS_CARR_ON was (once) necessary here
			  if ((tp->t_state&TS_CARR_ON) == 0 ||
			  (tp->t_state&TS_NBIO)) {
			  TSPLX(s);
			  return (EWOULDBLOCK);
			  }
			  **/
			if (flag & IO_NDELAY) {
				TSPLX(s);
				return (EWOULDBLOCK);
			}
			error = ttysleep(tp, (caddr_t)&tp->t_rawq, 
				TTIPRI | PCATCH, ttyin);
			TSPLX(s);
			if (error)
				return (error);
			goto loop;
		}
		TSPLX(s);
	}
	else {
		qp = &tp->t_rawq;
		TSPLTTY(s);
		if (qp->c_cc < tp->t_cc[VMIN] || qp->c_cc == 0) {
			/** XXX ??? ask mike why TS_CARR_ON was (once) necessary here
			  if ((tp->t_state&TS_CARR_ON) == 0 ||
			  (tp->t_state&TS_NBIO)) {
			  TSPLX(s);
			  return (EWOULDBLOCK);
			  }
			  **/
			if (flag & IO_NDELAY) {
				TSPLX(s);
				return (EWOULDBLOCK);
			}
			if (tp->t_cc[VMIN] == 0 &&
			    (tp->t_shad_time == 0 || !first)) {
				/*
				 * If there are no chars and the caller
				 * doesn't want to wait or has waited once
				 * then return
				 */
				TSPLX(s);
				return(error);
			}
			if (tp->t_shad_time > 0 && !(tp->t_state&TS_INTIMEOUT) 
			    && (qp->c_cc > 0 || tp->t_cc[VMIN] == 0)) {
				first = 0;
				timeout(ttin_timeout, tp, tp->t_shad_time);
				tp->t_state |= TS_INTIMEOUT;
			}

			if (error = ttysleep (tp, (caddr_t)&tp->t_rawq, 
					      TTIPRI | PCATCH, ttyin)) {
				TSPLX(s);
				return (error);
			}
			if (qp->c_cc <= 0) {
				/* If there are no characters wait again. */
				TSPLX(s);
				goto loop;
			}
		}

		TSPLX(s);
	}		
	/*
	 * Input present, check for input mapping and processing.
	 */
	first = 1;
	while ((c = getc(qp)) >= 0) {
		/*
		 * delayed suspend (^Y)
		 */
		if (CCEQ(cc[VDSUSP], c) && lflag&ISIG) {
			unix_master();
			gsignal_chktty(tp->t_pgid, SIGTSTP);
			unix_release();
			if (first) {
				if (error = ttysleep(tp, (caddr_t)&lbolt, 
					TTIPRI | PCATCH, ttybg))
					break;
				goto loop;
			}
			break;
		}
		/*
		 * Interpret EOF only in canonical mode.
		 */
		if (CCEQ(cc[VEOF], c) && lflag&ICANON)
			break;
			
		/*
		 * Connical upper/lower presentation
		 */
		if((lflag&ICANON) && (lflag&XCASE)) {
			int next_char;	
			/*
			 * If this character is a quote and
			 * if the next character is one we want to
			 * remap then discard the \\ and send new
			 * remap character otherwise send the \\
			 */
			if ((c == '\\') && (qp->c_cc > 0)) {
				next_char = *qp->c_cf;
				if ( next_char >= 'a' && next_char <= 'z') {
					(void) getc(qp);
					c = next_char - 'a' + 'A';
				}
				else {
					int cnt;
					cnt = 0;
					while(xcase_map[cnt].from) {
						if (xcase_map[cnt].from == next_char) {
							(void) getc(qp);
							c = xcase_map[cnt].to;
							break;
						}
						cnt++;
					}
				}
			}
		}
		
		/*
		 * Give user character.
		 */
 		error = ureadc(c , uio);
		if (error)
			break;
 		if (uio->uio_resid == 0)
			break;
		/*
		 * In canonical mode check for a "break character"
		 * marking the end of a "line of input".
		 */
		if ((lflag&ICANON) && ttbreakc(c)) {
			break;
		}
		first = 0;
	}
	/*
	 * Look to unblock output now that (presumably)
	 * the input queue has gone down.
	 */
	if (tp->t_state&TS_TBLOCK && tp->t_rawq.c_cc < TTYHOG/5) {
		if (cc[VSTART] != _POSIX_VDISABLE) {
		  /* Even if we are XOFFed we must be able to send XON. */
		  if ((tp->t_oproc == tty_start) ? tty_xon(tp) :
		      putc(cc[VSTART], &tp->t_outq) == 0) {
		    TSPLTTY(s);
		    tp->t_state &= ~TS_TBLOCK;
		    TSPLX(s);
		    ttstart(tp);
		  }
		}
	}
	return (error);
}

/*
 * Check the output queue on tp for space for a kernel message
 * (from uprintf/tprintf).  Allow some space over the normal
 * hiwater mark so we don't lose messages due to normal flow
 * control, but don't let the tty run amok.
 * Sleeps here are not interruptible, but we return prematurely
 * if new signals come in.
 */
ttycheckoutq(tp, wait)
	register struct tty *tp;
	int wait;
{
	int hiwat, oldsig;
	extern int wakeup();
	TSPLVAR(s)

	TTY_LOCK(tp);
	hiwat = tp->t_hiwat;
	TSPLTTY(s);
	oldsig = u.u_procp->p_sig;
	if (tp->t_outq.c_cc > hiwat + 200)
		while (tp->t_outq.c_cc > hiwat) {
			ttstart(tp);
			if (wait == 0 || u.u_procp->p_sig != oldsig) {
				TSPLX(s);
				TTY_UNLOCK(tp);
				return (0);
			}
			tp->t_state |= TS_ASLEEP;
#if	UNIX_LOCKS && NCPUS > 1
			(void) mpsleep((caddr_t)&tp->t_outq, PZERO-1, ttyout, 
					hz, &tp->t_lock, 
					MS_LOCK_WRITE|MS_LOCK_ON_ERROR);
#else
			(void) mpsleep((caddr_t)&tp->t_outq, PZERO-1, ttyout, 
					hz, (void *)NULL, 0);
#endif
		}
	TSPLX(s);
	TTY_UNLOCK(tp);
	return (1);
}

/*
 * Called from the device's write routine after it has
 * calculated the tty-structure given as argument.
 */
ttwrite(tp, uio, flag)
	register struct tty *tp;
	register struct uio *uio;
{
#if     !UFS
        return EINVAL;
#else   /* UFS */
	register char *cp;
	register int cc = 0, ce;
	int i, hiwat, cnt, error, s;
	char obuf[OBUFSIZ];

	LASSERT(TTY_LOCK_HOLDER(tp));

	if (tp->t_state & TS_DRAINING)
		ttywait(tp);

	tp->t_nwrites++;
	hiwat = tp->t_hiwat;
	cnt = uio->uio_resid;
	error = 0;
loop:
	TSPLTTY(s);
	if (!(tp->t_state&TS_CARR_ON) && !(tp->t_cflag&CLOCAL)) {
		if (tp->t_state&TS_ISOPEN) {
			TSPLX(s);
			tp->t_nwrites--;
			return (EIO);
		} else if (flag & IO_NDELAY) {
			TSPLX(s);
			error = EWOULDBLOCK;
			goto out;
		} else {
			/*
			 * sleep awaiting carrier
			 */
			error = ttysleep(tp, (caddr_t)&tp->t_rawq, 
					TTIPRI | PCATCH, ttopen);
			TSPLX(s);
			if (error)
				goto out;
			goto loop;
		}
	}
	TSPLX(s);
	/*
	 * Hang the process if it's in the background.
	 */
	unix_master();
	if (isbackground(u.u_procp, tp) &&
	    (tp->t_lflag&TOSTOP)) {
		if (VPOP_TERMINAL_SIGPGRP(u.u_procp->p_vproc, SIGTTOU)) {
			unix_release();
			tp->t_nwrites--;
			return(EIO);
		}
		unix_release();
		if (error = ttysleep(tp, (caddr_t)&lbolt,
				TTIPRI | PCATCH, ttybg))
			goto out;
		goto loop;
	}
	unix_release();
	/*
	 * Process the user's data in at most OBUFSIZ
	 * chunks.  Perform any output translation.
	 * Keep track of high water mark, sleep on overflow 
	 * awaiting device aid in acquiring new space.
	 */
	while (uio->uio_resid > 0 || cc > 0) {
		if (tp->t_lflag&FLUSHO) {
			uio->uio_resid = 0;
			tp->t_nwrites--;
			return (0);
		}
		if (tp->t_outq.c_cc > hiwat)
			goto ovhiwat;
		/*
		 * Grab a hunk of data from the user.
		 * unless we have some left over from last time.
		 */
		if (cc == 0) {
			cc = min(uio->uio_resid, OBUFSIZ);
			cp = obuf;
			error = uiomove(cp, cc, uio);
			if (error) {
				cc = 0;
				break;
			}
		}
		/*
		 * If nothing fancy need be done, grab those characters we
		 * can handle without any of ttyoutput's processing and
		 * just transfer them to the output q.  For those chars
		 * which require special processing (as indicated by the
		 * bits in partab), call ttyoutput.  After processing
		 * a hunk of data, look for FLUSHO so ^O's will take effect
		 * immediately.
		 */
		while (cc > 0) {
			if (!(tp->t_oflag&OPOST))
				ce = cc;
			else {
				if ((tp->t_oflag & OLCUC) ||
				    (tp->t_lflag & XCASE))
					/* 
					 * Process all the characters 
					 * one by one 
					 */
					ce = 0;
				else
					ce = cc - scanc((unsigned)cc,
							(u_char *)cp,
							(u_char *)partab, 077);
				/*
				 * If ce is zero, then we're processing
				 * a special character through ttyoutput.
				 */
				if (ce == 0) {
					tp->t_rocount = 0;
					if (ttyoutput(*cp, tp) >= 0) {
					    /* no c-lists, wait a bit */
					    ttstart(tp);
					    if (error = ttysleep(tp, 
						(caddr_t)&lbolt,
						 TTOPRI | PCATCH, ttybuf))
						    break;
					    goto loop;
					}
					cp++, cc--;
					if ((tp->t_lflag&FLUSHO) ||
					    tp->t_outq.c_cc > hiwat)
						goto ovhiwat;
					continue;
				}
			}
			/*
			 * A bunch of normal characters have been found,
			 * transfer them en masse to the output queue and
			 * continue processing at the top of the loop.
			 * If there are any further characters in this
			 * <= OBUFSIZ chunk, the first should be a character
			 * requiring special handling by ttyoutput.
			 */
			tp->t_rocount = 0;
#ifdef OSF1_SERVER
			/* In 7-bit (!LITOUT) mode, the Mach3.0 drivers
			   treat an 8-bit character as a delay, so we must
			   strip the top bit before sending. */
			if ((tp->t_cflag & CSIZE) != CS8) {
				for (i = 0; i < ce; i++)	
					cp[i] &= 0177;
			}
#endif
			i = b_to_q(cp, ce, &tp->t_outq);
			ce -= i;
			tp->t_col += ce;
			cp += ce, cc -= ce, tk_nout += ce;
			tp->t_outcc += ce;
			if (i > 0) {
				/* out of c-lists, wait a bit */
				ttstart(tp);
				if (error = ttysleep(tp, (caddr_t)&lbolt,
					    TTOPRI | PCATCH, ttybuf))
					break;
				goto loop;
			}
			if (tp->t_lflag&FLUSHO || tp->t_outq.c_cc > hiwat)
				break;
		}
		ttstart(tp);
	}
out:
	/*
	 * If cc is nonzero, we leave the uio structure inconsistent,
	 * as the offset and iov pointers have moved forward,
	 * but it doesn't matter (the call will either return short
	 * or restart with a new uio).
	 */
	uio->uio_resid += cc;
	tp->t_nwrites--;
	return (error);

ovhiwat:
	ttstart(tp);
	TSPLTTY(s);
	/*
	 * This can only occur if FLUSHO is set in t_lflag,
	 * or if ttstart/oproc is synchronous (or very fast).
	 */
	if (tp->t_outq.c_cc <= hiwat) {
		TSPLX(s);
		goto loop;
	}
	if (flag & IO_NDELAY) {
		TSPLX(s);
		uio->uio_resid += cc;
		tp->t_nwrites--;
		if (uio->uio_resid == cnt)
			return (EWOULDBLOCK);
		return (0);
	}
	tp->t_state |= TS_ASLEEP;
	error =  ttysleep(tp, &tp->t_outq, TTOPRI | PCATCH, ttyout);
	TSPLX(s);
	if (error)
		goto out;
	goto loop;
#endif  /* UFS */
}

/*
 * Rubout one character from the rawq of tp
 * as cleanly as possible.
 */
ttyrub(c, tp)
	register c;
	register struct tty *tp;
{
	register char *cp;
	register int savecol;
	char *nextc();
	TSPLVAR(s)

	LASSERT(TTY_LOCK_HOLDER(tp));
	if ((tp->t_lflag&ECHO) == 0)
		return;
	tp->t_lflag &= ~FLUSHO;	
	if (tp->t_lflag&ECHOE) {
		if (tp->t_rocount == 0) {
			/*
			 * Screwed by ttwrite; retype
			 */
			ttyretype(tp);
			return;
		}
		if (c == ('\t'|TTY_QUOTE) || c == ('\n'|TTY_QUOTE))
			ttyrubo(tp, 2);
		else switch (partab[c&=0377]&077) {

		case ORDINARY:
			ttyrubo(tp, 1);
			break;

		case VTAB:
		case BACKSPACE:
		case CONTROL:
		case RETURN:
		case NEWLINE:
		case FF:
			if (tp->t_lflag&ECHOCTL)
				ttyrubo(tp, 2);
			break;

		case TAB: {
			int c;

			if (tp->t_rocount < tp->t_rawq.c_cc) {
				ttyretype(tp);
				return;
			}
			TSPLTTY(s);
			savecol = tp->t_col;
			tp->t_state |= TS_CNTTB;
			tp->t_lflag |= FLUSHO;
			tp->t_col = tp->t_rocol;
			cp = tp->t_rawq.c_cf;
			if (cp)
				c = *cp;	/* XXX FIX NEXTC */
			for (; cp; cp = nextc(&tp->t_rawq, cp, &c))
				ttyecho(c, tp);
			tp->t_lflag &= ~FLUSHO;
			tp->t_state &= ~TS_CNTTB;
			TSPLX(s);
			/*
			 * savecol will now be length of the tab
			 */
			savecol -= tp->t_col;
			tp->t_col += savecol;
			if (savecol > 8)
				savecol = 8;		/* overflow screw */
			while (--savecol >= 0)
				(void) ttyoutput('\b', tp);
			break;
		}

		default:
			/* XXX */
			printf("ttyrub: would panic c = %d, val = %d\n",
				c, partab[c&=0377]&077);
			/*panic("ttyrub");*/
		}
	} else if (tp->t_lflag&ECHOPRT) {
		if ((tp->t_state&TS_ERASE) == 0) {
			(void) ttyoutput('\\', tp);
			tp->t_state |= TS_ERASE;
		}
		ttyecho(c, tp);
	} else
		ttyecho(tp->t_cc[VERASE], tp);
	tp->t_rocount--;
}

/*
 * Crt back over cnt chars perhaps
 * erasing them.
 */
ttyrubo(tp, cnt)
	register struct tty *tp;
	int cnt;
{

	LASSERT(TTY_LOCK_HOLDER(tp));
	while (--cnt >= 0)
		ttyoutstr("\b \b", tp);
}

/*
 * Reprint the rawq line.
 * We assume c_cc has already been checked.
 */
ttyretype(tp)
	register struct tty *tp;
{
	register char *cp;
	char *nextc();
	int c;
	TSPLVAR(s)

	LASSERT(TTY_LOCK_HOLDER(tp));
	if (tp->t_cc[VREPRINT] != _POSIX_VDISABLE)
		ttyecho(tp->t_cc[VREPRINT], tp);
	(void) ttyoutput('\n', tp);
	TSPLTTY(s);
	/*** XXX *** FIX *** NEXTC IS BROKEN - DOESN'T CHECK QUOTE
	  BIT OF FIRST CHAR ****/
	for (cp = tp->t_canq.c_cf, c=(cp?*cp:0); cp; cp = nextc(&tp->t_canq, cp, &c)) {
		ttyecho(c, tp);
	}
	for (cp = tp->t_rawq.c_cf, c=(cp?*cp:0); cp; cp = nextc(&tp->t_rawq, cp, &c)) {
		ttyecho(c, tp);
	}
	tp->t_state &= ~TS_ERASE;
	TSPLX(s);
	tp->t_rocount = tp->t_rawq.c_cc;
	tp->t_rocol = 0;
}

/*
 * Echo a typed character to the terminal.
 */
ttyecho(c, tp)
	register c;
	register struct tty *tp;
{
	LASSERT(TTY_LOCK_HOLDER(tp));
	if ((tp->t_state&TS_CNTTB) == 0)
		tp->t_lflag &= ~FLUSHO;
	if ((tp->t_lflag&ECHO) == 0 && !(tp->t_lflag&ECHONL && c == '\n'))
		return;
	if (tp->t_lflag&ECHOCTL) {
		if ((c&TTY_CHARMASK)<=037 && c!='\t' && c!='\n' || c==0177) {
			(void) ttyoutput('^', tp);
			c &= TTY_CHARMASK;
			if (c == 0177)
				c = '?';
			else
				c += 'A' - 1;
		}
	}
	
        /*
	 * If we are doing XCASE processing then send '\\' to ttyoutput
	 * quoted so it won't be double echoed.
         */
	if ((tp->t_lflag & XCASE) && c == '\\') {
		(void) ttyoutput(c | TTY_QUOTE, tp);
		return;
	}
		    
	/*
	 * Do not echo non-printing control characters.  Mainly
	 * to stop causing special actions on most terminals.
	 */
#if 0
	/*
	 * This code prevents the driver from echoing 8 bit characters
	 * which are legal in other languages such as latin-1.  If this
	 * causes problems it will have to be made another flag.
	 */
	c &= 0177;
	if ((040 <= c && c <= 0176) || (07 <= c && c <= 012) || c == 015)
#endif
	    (void) ttyoutput(c, tp);
}

/*
 * send string cp to tp
 */
ttyoutstr(cp, tp)
	register char *cp;
	register struct tty *tp;
{
	register char c;

	LASSERT(TTY_LOCK_HOLDER(tp));
	while (c = *cp++)
		(void) ttyoutput(c, tp);
}

ttwakeup(tp)
	struct tty *tp;
{
	LASSERT(TTY_LOCK_HOLDER(tp));
	
	if (tp->t_state & TS_INTIMEOUT) {
		untimeout(ttin_timeout, tp);
		tp->t_state &= ~TS_INTIMEOUT;
	}
	
	select_wakeup(&tp->t_selq);
	if (tp->t_state & TS_ASYNC) {
#ifndef TNC
		unix_master();
#endif
		gsignal4(tp->t_pgid, SIGIO, 1, FALSE);
#ifndef TNC
		unix_release();
#endif
	}
	thread_wakeup ((int) &tp->t_rawq);
}

/*
 * set tty hi and low water marks
 *
 * Try to arrange the dynamics so there's about one second
 * from hi to low water.
 *
 */
ttsetwater(tp)
	struct tty *tp;
{
	register cps = tp->t_ospeed / 10;
	register x;

#define clamp(x, h, l) ((x)>h ? h : ((x)<l) ? l : (x))
	tp->t_lowat = x = clamp(cps/2, TTMAXLOWAT, TTMINLOWAT);
	x += cps;
	x = clamp(x, TTMAXHIWAT, TTMINHIWAT);
	tp->t_hiwat = roundup(x, CBSIZE);
#undef clamp
}

ttspeedtab(speed, table)
	struct speedtab table[];
{
	register int i;

	for (i = 0; table[i].sp_speed != -1; i++)
		if (table[i].sp_speed == speed)
			return(table[i].sp_code);
	return(-1);
}

int ttyhostname = 0;
/*
 * (^T)
 * Report on state of foreground process group.
 * This routine assumes that the utask and thread strcutures  are not paged.
 */
/* Look closely at this one -- XXX -- gmf */
ttyinfo(tp)
	struct tty *tp;
{
#ifndef TTYINFO
	printf("ttyinfo() --- Not implemented ---\n");
#else
	register struct proc *p, *pick = NULL;
	register struct utask *ut;
#ifndef OSF1_SERVER
	register thread_t th;
#else
	register uthread_t uth;
	host_load_info_data_t	 load_info;
	extern mach_port_t	 host_port;
#endif
	register char *cp = hostname;
	pid_t pid;
	int x, s;
	time_value_t utime, stime;
	char ucomm[MAXCOMLEN+1];
#define	pgtok(a)	(((a)*NBPG)/1024)

	/*
	 * Allow some space over the normal hiwater mark so
	 * we don't lose messages due to normal flow
	 * control, but don't let the tty run amok.
	 */
	if (tp->t_outq.c_cc > tp->t_hiwat + 200)
		return;
	/* 
	 * hostname.  Not locked, but the worst that can
	 * happen is that we print a garbled hostname.
	 * Doesn't seem worth it.
	 */
	if (ttyhostname) {
		if (*cp == '\0')
			ttyoutstr("amnesia", tp);
		else
			while (*cp && *cp != '.')
				tputchar(*cp++, tp);
		tputchar(' ', tp);
	}
	/* 
	 * load average 
	 */
#ifdef OSF1_SERVER
	x = HOST_LOAD_INFO_COUNT;
	(void) host_info(host_port, HOST_LOAD_INFO,
			 &load_info, &x);
	x = load_info.avenrun[0]/10;
#else
	x = (avenrun[0] * 100 + FSCALE/2) >> FSHIFT;
#endif
	ttyoutstr("load: ", tp);
	ttyoutint(x/100, 10, 1, tp);
	tputchar('.', tp);
	ttyoutint(x%100, 10, 2, tp);
	if (tp->t_sid == 0)
		ttyoutstr(" not a controlling terminal\n", tp);
	else if (tp->t_pgid == 0 )
		ttyoutstr(" no foreground process group\n", tp);
#ifndef OSF1_SERVER
	else if ((p = tp->t_pgrp->pg_mem) == NULL)
		ttyoutstr(" empty foreground process group\n", tp);
#endif
	else {
		/* pick interesting process */
#ifndef OSF1_SERVER
		unix_master();
		for (; p != NULL; p = p->p_pgrpnxt) {
			if (proc_compare(pick, p))
				pick = p;
		}
		unix_release();
#else
		pick = u.u_procp;
#endif

		if (pick == NULL) {
			ttyoutstr(" no information on current process\n", tp);
			return;
		}
		
		pid = pick->p_pid;
#ifdef OSF1_SERVER
		ut = &pick->p_utask;
		uth = &u;

		if (pid <= 0 || ut == NULL) {
			ttyoutstr(" no information on current process\n", tp);
			return;
		}
		
		bcopy(ut->uu_comm, ucomm, MAXCOMLEN+1);
		ttyoutstr("  cmd: ", tp);
		ttyoutstr(ucomm, tp);
		tputchar(' ', tp);
		ttyoutint(pid, 10, 1, tp);
		ttyoutstr(" [", tp);
		ttyoutstr(uth->uu_wait_mesg ? uth->uu_wait_mesg : "running", tp);
		ttyoutstr("] ", tp);
#else
		th = pick->thread;
		ut = pick->utask;
		if (pid <= 0 || th == NULL || ut == NULL) {
			ttyoutstr(" no information on current process\n", tp);
			return;
		}
		
		thread_reference(th);

		usimple_lock(&ut->uu_handy_lock);
		bcopy(ut->uu_comm, ucomm, MAXCOMLEN+1);
		usimple_unlock(&ut->uu_handy_lock);
		ttyoutstr("  cmd: ", tp);
		ttyoutstr(ucomm, tp);
		tputchar(' ', tp);
		ttyoutint(pid, 10, 1, tp);
		ttyoutstr(" [", tp);
		ttyoutstr(th->wait_mesg ? th->wait_mesg : "running", tp);
		ttyoutstr("] ", tp);
#endif
		

#if 0
		/* 
		 * cpu time 
		 */
		if (u.u_procp == pick)
			s = splclock();
		thread_read_times(th, &utime, &stime);
		if (u.u_procp == pick)
			splx(s);
		/* user time */
		x = utime.microseconds / 10000; /* scale to 100's */
		ttyoutint(utime.seconds, 10, 1, tp);
		tputchar('.', tp);
		ttyoutint(x, 10, 2, tp);
		tputchar('u', tp);
		tputchar(' ', tp);
		/* system time */
		x = stime.microseconds / 10000; /* scale to 100's */
		ttyoutint(stime.seconds, 10, 1, tp);
		tputchar('.', tp);
		ttyoutint(x, 10, 2, tp);
		tputchar('s', tp);
		tputchar(' ', tp);
		/* 
		 * pctcpu 
		 */
		x = th->cpu_usage / (TIMER_RATE/TH_USAGE_SCALE);
		x = (x * 3) / 5;
#if	SIMPLE_CLOCK
		/*
		 *	Clock drift compensation.
		 */
		x = (x * 1000000)/sched_usec;
#endif	SIMPLE_CLOCK
		ttyoutint(x/100, 10, 1, tp);
#ifdef notdef	/* do we really want this ??? */
		tputchar('.', tp);
		ttyoutint(x%100, 10, 2, tp);
#endif
		ttyoutstr("% ",tp);
		ttyoutint(pgtok(ut->uu_ssize + ut->uu_dsize), 10, 1, tp);
		ttyoutstr("k\n", tp);
		thread_deallocate(th);
#else
		ttyoutstr("\n", tp);
#endif
	}
	tp->t_rocount = 0;	/* so pending input will be retyped if BS */
#endif /*TTYINFO*/
}

ttyoutint(n, base, min, tp)
	register int n, base, min;
	register struct tty *tp;
{
	char info[16];
	register char *p = info;

	while (--min >= 0 || n) {
		*p++ = "0123456789abcdef"[n%base];
		n /= base;
	}
	while (p > info)
		ttyoutput(*--p, tp);
}

/*
 * Returns 1 if p2 is "better" than p1
 *
 * The algorithm for picking the "interesting" process is thus:
 *
 *	1) (Only foreground processes are eligable - implied)
 *	2) Runnable processes are favored over anything
 *	   else.  The runner with the highest cpu
 *	   utilization is picked (p_cpu).  Ties are
 *	   broken by picking the highest pid.
 *	3  Next, the sleeper with the shortest sleep
 *	   time is favored.  With ties, we pick out
 *	   just "short-term" sleepers (thread->interruptible == 0).
 *	   Further ties are broken by picking the highest
 *	   pid.
 *
 */
#define isrun(th)	(((th)->state & TH_RUN) == TH_RUN)
#define TESTAB(a, b)    ((a)<<1 | (b))
#define ONLYA   2
#define ONLYB   1
#define BOTH    3

proc_compare(p1, p2)
	register struct proc *p1, *p2;
{
#ifdef OSF1_SERVER
	printf("proc_compare() --- Not implemented ---\n");
	return(1);
#else /* OSF1_SERVER */
	thread_t th1, th2;
	pid_t pid1, pid2;
	long slptime1, slptime2;
	
	if (p1 == NULL)
		return (1);

	if ((th1 = p1->thread) == NULL || (pid1 = p1->p_pid) <= 0)
		return(1);	/* The process has exited. */
	thread_reference(th1);

	if ((th2 = p2->thread) == NULL || (pid2 = p2->p_pid) <= 0) {
		thread_deallocate(th1);
		return(0);	/* The process has exited. */
	}
	thread_reference(th2);

#undef RETURN
#define RETURN(x) thread_deallocate(th1); \
	thread_deallocate(th2); \
	    return(x);
	
	/*
	 * see if at least one of them is runnable
	 */
	switch (TESTAB(isrun(th1), isrun(th2))) {
	case ONLYA:
		RETURN (1);
	case ONLYB:
		RETURN (0);
	case BOTH:
		/*
		 * tie - favor one with highest recent cpu utilization
		 */
		if (th2->cpu_usage > th1->cpu_usage) {
			RETURN (1);
		}
		
		if (th1->cpu_usage > th2->cpu_usage) {
			RETURN (0);
		}
		
		RETURN (pid2 > pid1);	/* tie - return highest pid */
	}
	/* 
	 * pick the one with the smallest sleep time
	 */
	slptime1 = sched_tick - th1->sleep_stamp;
	slptime2 = sched_tick - th2->sleep_stamp;
	if (slptime2 > slptime1) {
		RETURN (0);
	}
	
	if (slptime1 > slptime2) {
		RETURN (1);
	}
	
	/*
	 * favor one sleeping in a non-interruptible sleep
	 */
	if (th1->interruptible && (th2->interruptible) == 0) {
		RETURN (1);
	}
	
	if (th2->interruptible && (th1->interruptible) == 0) {
		RETURN (0);
	}
	
	RETURN(pid2 > pid1);		/* tie - return highest pid */

#undef RETURN	
#endif /* OSF1_SERVER */
}

/*
 * Output char to tty; console putchar style.
 */
tputchar(c, tp)
	int c;
	struct tty *tp;
{
	TSPLVAR(s)

	TSPLTTY(s);
	if ((tp->t_state & (TS_CARR_ON | TS_ISOPEN))
	    == (TS_CARR_ON | TS_ISOPEN)) {
		if (c == '\n')
			(void) ttyoutput('\r', tp);
		(void) ttyoutput(c, tp);
		ttstart(tp);
		TSPLX(s);
		return (0);
	}
	TSPLX(s);
	return (-1);
}

ttin_timeout(tp)
register struct tty *tp;
{
	
/*
 * This function performs a wakeup on the rawq.  It is used for
 * inter-character timings, and is called by timeout.
 */
	TSPLVAR(ipl)

	TSPLTTY(ipl);
	TTY_LOCK(tp);
	if (tp == 0)
		panic("ttin_timeout");
	tp->t_state &= ~TS_INTIMEOUT;
	ttwakeup(tp);
	TTY_UNLOCK(tp);
	TSPLX(ipl);
}

/*
 * Takes a locked struct tty.
 * If no errors, returns it locked; otherwise it's unlocked.
 */
ttysleep(tp, chan, pri, wmesg)
	register struct tty *tp;
	caddr_t chan;
	int pri;
	char *wmesg;
{
	int error;
	short gen = tp->t_gen;

	LASSERT(TTY_LOCK_HOLDER(tp));
#if	UNIX_LOCKS && NCPUS > 1
	if (error = mpsleep(chan, pri, wmesg, 0, 
			    &tp->t_lock, MS_LOCK_WRITE|MS_LOCK_ON_ERROR))
#else
	if (error = tsleep(chan, pri, wmesg, 0))
#endif
		return (error);
	if (tp->t_gen != gen) {
		return (ERESTART);
	}
	return (0);
}

pproc_release_cttyv(tp)
	struct tty *tp;
{
	if (tp->t_vnode) {
#ifdef	OSF1_ADFS
		clearalias(tp->t_vnode);
#else
		clearalias(tp->t_vnode, 0);
#endif
		vrele(tp->t_vnode);
	}
}
