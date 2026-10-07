/*
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
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: subr_prf.c,v $
 * Revision 1.22  1995/03/17  23:52:35  yazz
 * Correct varargs placement in earlier checkin, so bootnode_printf()
 * matches printf().
 *
 * Revision 1.21  1995/03/02  19:12:10  stans
 *  Lint picking plus Standard ANSI C over varargs
 *
 *  Reviewer:lenb,suri
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW07 sats
 *
 * Revision 1.20  1995/02/21  22:03:02  yazz
 * Corrected typo in previous checkin.  Cannot refer to rpm_contents()
 * unless both i860 and TNC are defined.
 *
 * Revision 1.19  1995/02/18  01:23:30  yazz
 *  Reviewer: John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 12240, including emul console logging cleanup
 *  Testing: EATs controlc, sched
 *  Module(s):
 * 	svr/emulator/bsd_user_side.c
 * 	svr/emulator/emul_chkpnt.c
 * 	svr/emulator/emul_init.c
 * 	svr/emulator/emul_mapped.c
 * 	svr/emulator/fsvr_user_side.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/bsd/mach_signal.c
 * 	svr/server/bsd/subr_prf.c
 * 	svr/server/tnc/dvp_vpops.c
 * 	svr/server/uxkern/boot_config.c
 * 	svr/server/uxkern/bsd_server_side.c
 * 	svr/server/uxkern/credentials.c
 * 	svr/server/uxkern/rpm_clock.c
 *
 * General cleanup of emulator console logging.  Added bootnode_printf()
 * routine to server.  Added server bootmagic variable ENABLE_RPM_TIMESTAMP
 * so printf() and bootnode_printf() messages are timestamped with the
 * 56-bit RPM global clock value.  This enables very fine timings to be
 * observable in console log output.
 *
 * Revision 1.18  1995/02/07  02:19:04  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: 12332
 *  Testing: controlc EAT + add'l server sprintf() calls
 *  Module(s): server/bsd/subr_prf.c
 *
 * Revision 1.17  1994/11/18  20:27:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.16  1994/11/08  19:54:20  jlitvin
 * Minor change to add back the "Repeating message:" text for when a
 * person tries to continue from a panic.  This was lost by the previous
 * change.
 *
 *  Reviewer: yazz
 *  Risk: low
 *  Benefit or PTS #: 11540
 *  Testing: developer
 *  Module(s): server/bsd/subr_prf.c
 *
 * Revision 1.15  1994/09/13  18:30:56  jlitvin
 * Author of change: Bob Yasi (yazz)
 * Reviewer(s): Brent Olsen, Bob Yasi
 * Benefit or PTS #: #10851
 *       Exception routine's crucial panic info lost by server, harms PTS
 *       reports.
 * Testing: Cause an catch_exception_raise panic and view the messages.
 * Module(s):
 * 	server/bsd/subr_prf.c
 * 	server/uxkern/ux_exception.c
 *
 * Incorporate crucial information into the official panic message produced
 * when a catch_exception_raise panic occurs.  Added additional printf's.
 *
 * Revision 1.14  1994/07/25  20:01:12  jlitvin
 * A misunderstanding of how the server enters the kernel debugger caused
 * PTS #9009 to not cover all cases.
 *
 *  Reviewer: yazz
 *  Risk: low
 *  Benefit or PTS #: 10342
 *  Testing: developer
 *  Module(s): server/bsd/subr_prf.c
 *
 * Revision 1.13  1994/06/24  00:34:33  yazz
 *  Reviewer: Chris Peak
 *  Risk: lo
 *  Benefit or PTS #: #9952
 *  Testing: none (only matters *after* a panic)
 *  Module(s): server/bsd/subr_prf.c
 *             server/uxkern/cons.c
 *
 * Stop pretending divide-by-zero halts the server.  Plus some slight cleanup
 * of panic().
 *
 * Revision 1.12  1994/04/16  21:27:38  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Very Low; changes do not come into play until after a panic hits.
 *  Benefit or PTS #: #9009, false PTS reports avoided.
 *  Testing: EATs tcpip
 *  Module(s): server/bsd/subr_prf.c
 *
 * Do not allow panic() to return to its caller!  Unless global panic_continue_-
 * flag is patched, typing "c" to the mk debugger just reprints the same panic
 * message and unintended horrible consequences of panic() returning to its
 * caller are avoided..
 *
 * Revision 1.11  1994/03/24  20:43:16  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: #8334
 *  Testing: boot + full multiuser (only panic msgs changed)
 *  Module(s): server/bsd/subr_prf.c
 *
 * Change panic() messages to better catch MACH_RCV_TOO_LARGE return values.
 *
 * Revision 1.10  1994/03/14  02:00:44  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.9  1993/10/28  03:07:43  yazz
 * Add instrumentation to panic() so it will display the number of ports
 * allocated, and their types.
 *
 * Revision 1.8  1993/09/09  15:26:30  cfj
 * Fix for PTS bug #4257.  The function tprintf was not concatinating
 * the rest of the string to the buffer.
 *
 * Revision 1.7  1993/07/14  17:49:03  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.6  1993/05/06  19:05:14  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  18:49:20  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:05:14  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:05  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/03/29  15:36:24  cfj
 * Merge with T9.
 *
 * Revision 1.3.6.1  1993/03/29  15:35:34  cfj
 * Added ux_server_thread_blocking/unblocking around calls to ubsd_remote_print().
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:59:00  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:54:54  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:16:39  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:54  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.20  92/12/01  11:12:04  chrisp
 * boot() has been renamed pps_boot().
 * 
 * Revision 2.19  92/11/23  16:01:10  klh
 * 	Revision 2.17  92/11/13  13:13:24  loverso
 * 		Allow panic to work before we've gotten the root_fs_port. (loverso)
 * 
 * Revision 2.21  1993/04/24  18:47:13  klh
 * 	Revision 2.19  93/04/08  11:20:24  loverso
 * 		Add blocking|unblocking around calls to ubsd_remote_print().
 * 		(bug #603).  (loverso)
 *
 * Revision 2.20  92/12/01  11:12:04  chrisp
 * boot() has been renamed pps_boot().
 * 
 * Revision 2.19  92/11/23  16:01:10  klh
 * 	Revision 2.17  92/11/13  13:13:24  loverso
 * 		Allow panic to work before we've gotten the root_fs_port. (loverso)
 * 
 * Revision 2.18  92/10/06  12:10:57  roman
 * Fix RCS comments.
 * 
 * Revision 2.17  92/10/05  13:47:58  klh
 * 	Revision 2.16  92/09/11  09:26:40  rabii
 * 		Modified sbsd_remote_print to go through 
 *		{start/end}_fsvrport_op (rabii)
 * 
 * Revision 2.16  92/08/06  16:38:40  roman
 * Fix RCS comments.
 * 
 * Revision 2.15  92/08/06  13:30:51  klh
 * 	Revision 2.15  92/07/14  14:58:08  rabii
 * 		Enhanced panic to send all panic messages to the root_fs_node
 *		(rabii)
 * 
 * Revision 2.14  92/07/07  15:12:11  roman
 * Add invocation of va_end() for error case while processing varargs list.
 * Fix incorrect checking for errors in VPOP_CTTY_GETATTR().
 * 
 * Revision 2.13  92/06/05  14:01:36  klh
 * 	Revision 2.13  92/05/24  14:17:46  pjg
 * 		92/03/24  21:03:24  barbou
 * 		Fix for bug #113: new cprintf() to print only on console.
 * 
 * 		92/03/23  18:02:22  condict
 * 		Allow NCPUS == 1 in the server, to compile optimally for a 
 * 	uni-processor.
 * 		[92/05/20            srl]
 * 
 * Revision 2.12  92/04/14  10:02:32  roman
 * Remove conditions on TNC being defined.
 *
 * Revision 2.11  92/04/08  10:39:26  rabii
 * 	Calls to "prf" now pass in a va_list pointer instead of "" so
 * 	the i860 expecting a va_list does not get confused
 * 
 * Revision 2.10  92/04/05  16:48:02  pjg
 * 	Added support for remote logging and printing (rabii)
 * 	Add extra parameter to VPOP_CTTY_GETATTR(). (roman)
 * 
 * Revision 2.9  92/03/09  14:31:06  durriya
 * 	[Revision 3.14  92/01/20  10:45:50  bernadat]
 * 	Added call to xpr_dump(0) in case of panic.
 * 
 * 	[Revision 3.13  91/12/18  17:15:27  sp]
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.8  92/02/11  22:19:15  pjg
 * 	Panic now takes printf-style arguments, as God intended.  Panicked
 * 	spelled correctly (yazz@locus).
 * 
 * Revision 2.7  91/12/17  13:45:58  roy
 * 	91/11/26  15:32:53  sp
 * 	Upgrade to 1.0.3
 * 
 * 	91/10/30  17:42:11  bernadat
 * 	Added illegal instruction in panic to make debugger happy. [barbou]
 * 
 * 	91/10/17  18:32:57  barbou
 * 	Mach should enter the debugger instead of rebooting after a panic.
 * 
 * Revision 2.6  91/11/22  14:54:40  rabii
 * 	Locus Merge
 * 	VPOP_CTTY_GETATTR supersedes VPOP_GET_CTTY. (chrisp)
 * 
 * Revision 2.5  91/10/14  12:09:29  sjs
 * 	91/09/18  17:44:44  condict
 * 	Fix problem which prevents printf when two threads panic at the 
 * 	same time.  The first one could be suspended by the second before 
 *	the message appears.
 * 
 * 	91/09/10  12:00:16  barbou
 * 	More console flush. Output user messages to console if no user terminal.
 * 
 * Revision 2.4  91/10/04  14:51:20  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:43:20  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	References through session structures replaced by session ids. 
 * 
 * Revision 2.2  91/08/31  13:23:12  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.7  91/08/28  14:00:31  condict
 * Update to OSF 1.0.2.  Also, add cnflush to printf, to make kernel printf
 * work in stand-alone server.
 * 
 * Revision 1.16.9.2  91/09/12  16:33:32  dwm
 *	Fix undefined new_printf_cpu_number, bug 3016.
 *	[91/09/12  15:34:46  dwm]
 *
 * Revision 1.16  90/10/31  13:49:28  devrcs
 * 	Make panic call Debugger on i386 if configured.
 * 	[90/10/21  16:58:35  jeffc]
 * 
 * 	Call kdb for i386 via Bpt.
 * 	[90/10/18  15:35:24  brezak]
 * 
 * Revision 1.15  90/10/07  13:18:48  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:58:47  gm]
 * 
 * Revision 1.14  90/09/23  15:43:28  devrcs
 * 	Moved ttyprintf here from tty.c
 * 	[90/09/14  14:17:36  ers]
 * 
 * 	Fixed for ANSI compliance
 * 	[90/09/05  17:12:33  rossi]
 * 
 * Revision 1.13  90/08/24  11:18:12  devrcs
 * 	HP/Apollo M68K
 * 	[90/08/13  17:36:02  mcg]
 * 
 * Revision 1.12  90/08/09  13:14:40  devrcs
 * 	Fix constty support (and delete xcons_tp) (bug 340).
 * 	Update to Reno release, somewhat - [tu]printf remain as before.
 * 	[90/08/01  10:41:19  tmt]
 * 
 * Revision 1.11  90/07/05  23:07:58  devrcs
 * 	Use constty in console putchar if no xcons_tp. Always use console
 * 	if panicking. Don't reference mips kdb if !MACH_KDB.
 * 	[90/07/03  11:27:38  tmt]
 * 
 * 	Updated to use varargs.h. Removed or rearranged as many
 * 	machine dependencies as possible. Added casts, etc.
 * 	Multimax code continues to use hackery, until fix
 * 	compiler and varargs.h (see #if BROKEN). This change
 * 	also fixes log() trashing multimax registers on return.
 * 	[90/06/29  16:30:14  tmt]
 * 
 * Revision 1.10  90/06/22  20:06:45  devrcs
 * 	nags merge
 * 
 * 	Condensed history:
 * 	Parallelized for OSF/1				nags@encore.com
 * 	Fix to printing zero-padded fields		jvs@osf.org
 * 	Changes for gcc varargs				brezak@osf.org
 * 	     define log_open in the addlog routine.          jvs@osf.org
 * 	     Posix tty support: defined constty.             morris@osf.org
 * 	     Merged Robert Coren's and Rich Morris's changes ers@osf.org
 * 	     Removed include of <sys/dir.h>                  gmf@osf.org
 * 	     Fixes for first snapshot.                       gm@osf.org
 * 	     Fixed problem with panic not dumping (partly);
 * 	     turned off stack printing also.                 gmf@osf.org
 * 	     Added addlog() function.                        gmf@osf.org
 * 	     do prstack() on panic w/ debug                  shashi@encore.com
 * 	     Fixed incorrect locking                         boykin@encore.com
 * 	     Mach 2.5 and Encore changes merge               alan@encore.com
 * 	     Bug fix to prf for field widths (i.e. %8x)      boykin@encore.com
 * 	     Old history - contributors
 * 	     mwyoung@cs.cmu.edu              rvb@cs.cmu.edu
 * 	     jjc@cs.cmu.edu                  af@cs.cmu.edu
 * 	     mrt@cs.cmu.edu                  rpd@cs.cmu.edu
 * 	     dlb@cs.cmu.edu                  mja@cs.cmu.edu
 * 	     sanzi@cs.cmu.edu                bolosky@cs.cmu.edu
 * 	     beck@sequent.com                avie@cs.cmu.edu
 * 	[90/06/12  19:07:14  gmf]
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
 *	@(#)subr_prf.c	7.1 (Berkeley) 6/5/86
 */

#include <uxkern/bsd_types_gen.h>
#include <cpus.h>
#include <cputypes.h>
#include <varargs.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/reboot.h>
#include <sys/msgbuf.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/syslog.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#ifdef	mips
#include <kern/xpr.h>		/* reg_desc goodies (see below) */
#endif	

#include <kern/lock.h>
#include <kern/parallel.h>

#include <machine/cpu.h>	/* for cpu_number() */
#ifdef	vax
#include <vax/mtpr.h>
#endif
#include <xpr_debug.h>

#ifdef NX
#include "machine/endian.h"
#endif

#define TOCONS	0x1
#define TOTTY	0x2
#define TOLOG	0x4
#define	TOSTR	0x8
#ifdef	mips
#define TOPROM	0x8
#endif	
#ifdef	multimax
#define cons	slc_tty		/* Multimax console == slc_tty[0] */
#endif

#define PRINTF_BUFSIZE 4096
/*
 * Defines for remote printing (used only internally by this file)
 */
#define	BSD_PRINT	0x01
#define	BSD_UPRINT	0x02
#define	BSD_TPRINT	0x03
#define	BSD_TTYPRINT	0x04
#define	BSD_LOG		0x05

extern	mach_port_t	root_fs_port;
extern	node_t		this_node;
extern	node_t		root_fs_node;

/*
 * In case console is off,
 * panicstr contains argument to last
 * call to panic.
 */
char	*panicstr;

extern	cnputc();			/* standard console putc */
int	(*v_putc)() = cnputc;		/* routine to putc on virtual console */
extern	struct tty cons;		/* standard console tty */
struct	tty *constty;			/* pointer to console "window" tty */

/*
 *	Record cpu that panic'd and lock around panic data
 */
decl_simple_lock_data(,panic_lock)
int paniccpu;

/*
 *	For kernel debugging, allow panic() to return if
 *	so elected.
 */
int panic_return = 0;

int enable_rpm_timestamp = 0;		/* BOOTMAGIC ENABLE_RPM_TIEMSTAMP */

#ifdef	SECOND_SERVER
extern	int 	second_server;
#endif	/* SECOND_SERVER */

/*
 * Scaled down version of C Library printf.
 * Used to print diagnostic information directly on console tty.
 * Since it is not interrupt driven, all system activities are
 * suspended.  Printf should not be used for chit-chat.
 *
 * One additional format: %b is supported to decode error registers.
 * Usage is:
 *	printf("reg=%b\n", regval, "<base><arg>*");
 * Where <base> is the output base expressed as a control character,
 * e.g. \10 gives octal; \20 gives hex.  Each arg is a sequence of
 * characters, the first of which gives the bit number to be inspected
 * (origin 1), and the next characters (up to a control character, i.e.
 * a character <= 32), give the name of the register.  Thus
 *	printf("reg=%b\n", 3, "\10\2BITTWO\1BITONE\n");
 * would produce output:
 *	reg=3<BITTWO,BITONE>
 *
 * Mips version also has other goodies for %r and %R formats,
 * see mips/mips_debug.c
 *
 * Varargs.h is needed. Hackery discouraged.
 */

/* Foo. We've got multimax work to do to get rid of this. */
/* The problem is argument passing via registers. */
#if defined(multimax)
#define BROKEN	1
#endif

#if	!BROKEN
/*VARARGS1*/
printf(va_alist)	/* fmt, va_alist */
	va_dcl
{
	char *fmt;
	va_list ap;

#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	fmt = va_arg(ap, char *);

#if defined(i860) && defined(TNC)
	if (enable_rpm_timestamp != 0 && *fmt != '<') {
		long rpmlong[2];
		double rpm_contents();

		(void)rpm_contents(rpmlong);	/* discard flting pt return */
		/* recurse just once. stop because of '<' */
		printf("<<%x,%08x>> node %d:\n  ", rpmlong[0], rpmlong[1],
		 this_node);
	}
#endif /* i860 */

	prf(fmt, ap, TOCONS | TOLOG, (struct tty *)0);

	if (!panicstr)
		logwakeup();
	cnflush();

	va_end(ap);
}
#else	/* BROKEN */
/*VARARGS1*/
printf(fmt, x1, x2)
	register char *fmt;
	unsigned x1, x2;
{
	prf(fmt, &x1, TOCONS | TOLOG, (struct tty *)0, &x2);
	if (!panicstr)
		logwakeup();
	cnflush();
}
#endif	/* BROKEN */

bootnode_printf(va_alist)	/* fmt, va_alist */
	va_dcl
{
	struct tty *tp = NULL;
	char *fmt;
	va_list ap;
	char *bp;	/* can't assign bp = buf here-compiler? */
	char buf[PRINTF_BUFSIZE];

	bp = buf;

#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	fmt = va_arg(ap, char *);

#if defined(i860) && defined(TNC)
	if (enable_rpm_timestamp != 0 && *fmt != '<') {
		long rpmlong[2];
		double rpm_contents();

		(void)rpm_contents(rpmlong);	/* discard flting pt return */
		bp += sprintf(bp, "<<%x,%08x>> node %d:\n  ",
				rpmlong[0], rpmlong[1], this_node);
	}
#endif /* i860 */

	bp += sprintf(bp, "[node: %d] ", this_node);
	prf(fmt, ap, TOSTR, (struct tty *)&bp);
					/* prf incrs dest char ptr bp */
	va_end(ap);

	/*
	 * Check to see if we are on bootnode and can effect a local printf.
	 */
	if (this_node == root_fs_node || !MACH_PORT_VALID(root_fs_port)) {
		printf("%s", buf);
	} else {
		ux_server_thread_blocking();
		(void)ubsd_remote_print(root_fs_port, BSD_PRINT,
				0, 0, 0, buf, (int)(bp-buf));
		ux_server_thread_unblocking();
	}
}

/*
 * NOTE:
 *	uprintf/tprintf have changed in 4.4.
 *	These are the _old_ interfaces. 
 */

/*
 * Uprintf prints to the current user's terminal.
 * It may block if the tty queue is overfull.
 * Should determine whether current terminal user is related
 * to this process.
 */
#if	!BROKEN
/*VARARGS1*/
uprintf(va_alist)	/* fmt, va_alist */
	va_dcl
{
	struct tty *tp = NULL;
	char *fmt;
	va_list ap;
	char *bp;	/* can't assign bp = buf here-compiler? */
	char buf[PRINTF_BUFSIZE];

	bp = buf;
	bp += sprintf(bp, "[node: %d] ", this_node);
#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	fmt = va_arg(ap, char *);
	prf(fmt, ap, TOSTR, (struct tty *)&bp);
	/* Check to see if we are remote */
	if (this_node != root_fs_node) {
		if (MACH_PORT_VALID(root_fs_port)) {
		        ux_server_thread_blocking();
			(void)ubsd_remote_print(root_fs_port, BSD_UPRINT,
					u.u_procp->p_pid, u.u_procp->p_sid, 0,
					buf, (int)(bp-buf));
			ux_server_thread_unblocking();
		}
		goto done;
	}
	unix_master();		/* sessions, sigh */
	if (this_node == root_fs_node &&
	    VPOP_CTTY_GETATTR(u.u_procp->p_vproc, 0, 0, 0, 0, &tp) != ESUCCESS)
		tp = NULL;
	if (tp != NULL && tp->t_sid == u.u_procp->p_sid) {
		(void)ttycheckoutq(tp, 1);
		prf(buf, ap, TOTTY, tp);
	} else {
		prf(buf, ap, TOCONS, (struct tty *)0);
		cnflush();
	}
	unix_release();
done:
	va_end(ap);
}
#else	/* BROKEN */
/*VARARGS1*/
uprintf(fmt, x1, x2)
	char *fmt;
	int x1, x2;
{
	struct tty *tp = NULL;

	unix_master();		/* sessions, sigh */
	if (VPOP_CTTY_GETATTR(u.u_procp->p_vproc, 0, 0, 0, 0, &tp) != ESUCCESS)
		tp = NULL;
	if (tp != NULL && tp->t_sid == u.u_procp->p_sid) {
		(void)ttycheckoutq(tp, 1);
		prf(fmt, &x1, TOTTY, tp, &x2);
	} else {
		prf(fmt, &x1, TOCONS, (struct tty *)0, &x2);
		cnflush();
	}
	unix_release();
}
#endif	/* BROKEN */

/*
 * tprintf prints on the specified terminal (console if none)
 * and logs the message.  It is designed for error messages from
 * single-open devices, and may be called from interrupt level
 * (does not sleep).
 */
#if	defined(OSF1_ADFS)
tprintf(va_alist)	/* pid, fmt, va_alist */
	va_dcl
{
	struct tty *tp;
	char *fmt;
	int flags = TOTTY | TOLOG;
	va_list ap;
	pid_t	pid;
	char *bp;
	int	n;
	int rc;
	char buf[PRINTF_BUFSIZE];

	bzero((caddr_t)buf, PRINTF_BUFSIZE);
	sprintf(buf,"[node: %d] ",this_node);
	n = PRINTF_BUFSIZE - strlen(buf);	/* buffer room left */
	bp = buf + PRINTF_BUFSIZE - n;

#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	pid = va_arg(ap, pid_t);
	fmt = va_arg(ap, char *);
	prf(fmt, ap, TOSTR, (struct tty *)&bp);
	if (this_node == root_fs_node) {
		/* get tp */
		struct vproc *vp;
		unix_master();
		vp = LOCATE_VPROC_PID(pid);
		rc = VPOP_CTTY_GETATTR(vp, 0, 0, 0, 0, &tp);
		if (rc != ESUCCESS)
			tp = NULL;
		unix_release();
		if (tp == (struct tty *)NULL) {
			va_end(ap);
			return;
		}
		if (ttycheckoutq(tp, 0) == 0)
			flags = TOLOG;
	} else {
		if (MACH_PORT_VALID(root_fs_port)) {
			ux_server_thread_blocking();
			(void)ubsd_remote_print(root_fs_port, BSD_TPRINT, pid,
					0, 0, buf, strlen(buf));
			ux_server_thread_unblocking();
		}
		goto done;
	}
	if (flags & TOLOG)
		logpri(LOG_INFO);
	prf(buf, ap, flags, tp);
done:
	va_end(ap);
	logwakeup();
	cnflush();
}

/*
 * cprintf prints only on the console.
 * It is designed for system call trace messages, which shouldn't
 * be logged, because the log daemon would then do a system call
 * to sync the log file, and we would have to log this system call
 * again and again...
 */
#if	!BROKEN
/*VARARGS2*/
cprintf(va_alist)	/* fmt, va_alist */
	va_dcl
{
	char *fmt;
	va_list ap;

#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	fmt = va_arg(ap, char *);
	prf(fmt, ap, TOCONS, (struct tty *)0);
	va_end(ap);
	cnflush();
}
#else	/* BROKEN */
cprintf(fmt, x1, x2)
	register struct tty *tp;
	char *fmt;
	unsigned x1, x2;
{
	prf(fmt, &x1, TOCONS, (struct tty *)0, &x2);
	cnflush();
}
#endif	/* BROKEN */

ttyprintf(va_alist)	/* pid, fmt, va_alist */
	va_dcl
{
	struct tty *tp;
	char *fmt;
	int flags = TOTTY | TOLOG;
	va_list ap;
	pid_t	pid;
	char *bp;
	int	n;
	char buf[PRINTF_BUFSIZE];

	bzero((caddr_t)buf, PRINTF_BUFSIZE);
	sprintf(buf,"[node: %d] ",this_node);
	n = PRINTF_BUFSIZE - strlen(buf);	/* buffer room left */
	bp = buf + PRINTF_BUFSIZE - n;

#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	pid = va_arg(ap, pid_t);
	fmt = va_arg(ap, char *);
	prf(fmt, ap, TOSTR, (struct tty *)&bp);
	if (this_node == root_fs_node) {
		/* get tp */
		struct vproc *vp;
		unix_master();
		vp = LOCATE_VPROC_PID(pid);
		if (VPOP_CTTY_GETATTR(vp, 0, 0, 0, 0, &tp) != ESUCCESS)
			tp = NULL;
		unix_release();
	} else {
		if (MACH_PORT_VALID(root_fs_port)) {
			ux_server_thread_blocking();
			(void)ubsd_remote_print(root_fs_port, BSD_TTYPRINT,
					pid, 0, 0, buf, strlen(buf));
			ux_server_thread_unblocking();
		}
		goto done;
	}
	if (tp != NULL) {
		prf(buf, ap, flags, tp);
	}
done:
	va_end(ap);
}

#else	/* OSF1_ADFS */

#if	!BROKEN
/*VARARGS2*/
tprintf(va_alist)	/* ttyp, fmt, va_alist */
	va_dcl
{
	register struct tty *tp;
	char *fmt;
	int flags = TOTTY | TOLOG;
	va_list ap;

	logpri(LOG_INFO);
#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	tp = va_arg(ap, struct tty *);
	if (tp == (struct tty *)NULL)
		return;
	if (ttycheckoutq(tp, 0) == 0)
		flags = TOLOG;
	fmt = va_arg(ap, char *);
	prf(fmt, ap, flags, tp);
	va_end(ap);
	logwakeup();
	cnflush();
}
#else	/* BROKEN */
tprintf(tp, fmt, x1, x2)
	register struct tty *tp;
	char *fmt;
	unsigned x1, x2;
{
	int flags = TOTTY | TOLOG;

	logpri(LOG_INFO);
	if (tp == (struct tty *)NULL)
		return;
	if (ttycheckoutq(tp, 0) == 0)
		flags = TOLOG;
	prf(fmt, &x1, flags, tp, &x2);
	logwakeup();
	cnflush();
}
#endif	/* BROKEN */

#if	!BROKEN
/*VARARGS2*/
ttyprintf(va_alist)
	va_dcl
{
	register struct tty *tp;
	char *fmt;
	va_list ap;

#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	tp = va_arg(ap, struct tty *);
	if (tp != NULL) {
		fmt = va_arg(ap, char *);
		prf(fmt, ap, TOTTY, tp);
	}
	va_end(ap);
}
#else	/* BROKEN */
ttyprintf(tp, fmt, x1, x2)
	struct tty *tp;
	char *fmt;
	unsigned x1;
{
	prf(fmt, &x1, TOTTY, tp, &x2);
}
#endif	/*BROKEN */
#endif	/* OSF1_ADFS */

#ifdef	mips
/*
 * dprintf uses the prom code to get to the console,
 * presumably to the debugger's remote console
 */
/*VARARGS1*/
dprintf(va_alist)	/* fmt, va_alist */
	va_dcl
{
	char *fmt;
	va_list ap;

#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	fmt = va_arg(ap, char *);
	prf(fmt, ap, TOPROM, (struct tty *)0);
	va_end(ap);
}


puts(s, flags, ttyp)
	register char *s;
	int flags;
	struct tty *ttyp;
{
	while (*s)
		putchar(*s++, flags, ttyp);
}
#endif	/* mips */

/*
 * Log writes to the log buffer,
 * and guarantees not to sleep (so can be called by interrupt routines).
 * If there is no process reading the log yet, it writes to the console also.
 */
#if	!BROKEN
/*VARARGS2*/
log(va_alist)		/* level, fmt, va_alist */
	va_dcl
{
	register s = splhigh();
	int level;
	char *fmt;
	char *bp;
	int	n;
	extern int log_open;
	va_list ap;
	char buf[PRINTF_BUFSIZE];

	bzero((caddr_t)buf, PRINTF_BUFSIZE);
	sprintf(buf,"[node: %d] ",this_node);
	n = PRINTF_BUFSIZE - strlen(buf);	/* buffer room left */
	bp = buf + PRINTF_BUFSIZE - n;
#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	level = va_arg(ap, int);
	fmt = va_arg(ap, char *);
	prf(fmt, ap, TOSTR, (struct tty *)&bp);
	/* Check to see if we are remote */
	if (this_node != root_fs_node) {
		if (MACH_PORT_VALID(root_fs_port)) {
			ux_server_thread_blocking();
			(void)ubsd_remote_print(root_fs_port, BSD_LOG, 0, 0,
						level, buf, strlen(buf));
			ux_server_thread_unblocking();
		} else
			prf(buf, ap, TOCONS, (struct tty *)0);
		goto done;
	}

	logpri(level);
	prf(buf, ap, TOLOG, (struct tty *)0);
	splx(s);
	if (!log_open)
		prf(buf, ap, TOCONS, (struct tty *)0);
	logwakeup();
done:
	va_end(ap);
}
#else	/* BROKEN */
/*VARARGS2*/
log(level, fmt, x1, x2)
	char *fmt;
	unsigned x1, x2;
{
	register s = splhigh();
	extern int log_open;

	logpri(level);
	prf(fmt, &x1, TOLOG, (struct tty *)0, &x2);
	splx(s);
	if (!log_open)
		prf(fmt, &x1, TOCONS, (struct tty *)0, &x2);
	logwakeup();
}
#endif	/* BROKEN */

logpri(level)
	int level;
{

	putchar('<', TOLOG, (struct tty *)0);
	printn((u_long)level, 10, TOLOG, (struct tty *)0, 0, 0);
	putchar('>', TOLOG, (struct tty *)0);
}


#if	!BROKEN
/*VARARGS1*/
addlog(va_alist)	/* fmt, va_alist */
	va_dcl
{
	extern int log_open;
	register s = splhigh();
	char *fmt;
	va_list ap;

#ifdef	__STDC__
	va_start(ap,0);
#else
	va_start(ap);
#endif
	fmt = va_arg(ap, char *);
	prf(fmt, ap, TOLOG, (struct tty *)0);
	splx(s);
	if (!log_open)
		prf(fmt, ap, TOCONS, (struct tty *)0);
	va_end(ap);
	logwakeup();
}
#else	/* BROKEN */
/*VARARGS1*/
addlog(fmt, x1, x2)
	char *fmt;
	unsigned x1, x2;
{
	extern int log_open;
	register s = splhigh();

	prf(fmt, &x1, TOLOG, (struct tty *)0, &x2);
	splx(s);
	if (!log_open)
		prf(fmt, &x1, TOCONS, (struct tty *)0, &x2);
	logwakeup();
}
#endif	/* BROKEN */

/*
 * Sprintf prints to a string buffer, and returns the # of chars appended.
 */
/*VARARGS2*/
int
sprintf(s, fmt, va_alist)
	char s[];	/* out */
	char *fmt;
	va_dcl
{
	va_list	listp;
	char	*cp;

	cp = s;
#ifdef	__STDC__
	va_start(listp,0);
#else
	va_start(listp);
#endif
	/* for TOSTR, prf null-terminates string */
	prf(fmt, listp, TOSTR, (struct tty *)&cp);
				/* prf's TOSTR now zeros final byte (*cp) */
	va_end(listp);

	return((int)(cp-s));	/* sprintf returns # chars output */
}

decl_simple_lock_data(,printf_lock)

#ifndef multimax
#if     NCPUS > 1
boolean_t new_printf_cpu_number;
#endif
#endif

#if	!BROKEN
prf(fmt, adx, flags, ttyp)
	va_list adx;
#else	/* mmax register arg passing hackaround */
#undef	va_arg
#define va_arg(a, t)	(t)(adv++ ? *adx2++ : *adx1)
prf(fmt, adx1, flags, ttyp, adx2)
	unsigned *adx1, *adx2;
#endif
	register char *fmt;
	int flags;
	struct tty *ttyp;
{
#if	BROKEN
	register adv = 0;
#endif
	register int b, c, i;
	char *s;
	int any;
	int base;
	int zf, fld_size, precision, left_adjust;
	int	spl;

#if	NCPUS > 1
	int cpun = cpu_number();

	if (flags == TOSTR) {
		;		/* take no locks; store into caller's buf */
	} else if (ttyp == 0) {
#if	UNIX_LOCKS && defined(multimax)
		/*
		 * Printf can be called from a number of interrupt states
		 * that are "higher" than splhigh, including the NMI
		 * interrupt level.  (Honest!)  So this lock must be held
		 * at an unusual priority.  The splnmi restriction is rare.
		 */
		spl = splnmi();
#endif
		simple_lock(&printf_lock);
	} else {
		TTY_LOCK(ttyp);
	}

#if	VAX6200
	if (cpu == VAX_6200) {
		extern int printing_cpu;
		if (printing_cpu != cpun)
			acquire_console(cpun);
	}
#endif	
#ifndef	multimax
	if (cpun != master_cpu)
		new_printf_cpu_number = TRUE;

	if (new_printf_cpu_number) {
		putchar('{', flags, ttyp);
		printn((u_long)cpun, 10, flags, ttyp, 0, 0);
		putchar('}', flags, ttyp);
	}
#endif	
#endif	/* NCPUS > 1 */
loop:
	while ((c = *fmt++) != '%') {
		if (c == '\0') {
			if (flags == TOSTR) {	/* ttyp points to a (char *) */
				char *p = *((char **)ttyp);
				*p = '\0';	/* get (char *) & end string */
				return;		/* no unlock or flush w/TOSTR */
			}

#if	NCPUS > 1
			if (ttyp == 0) {
				simple_unlock(&printf_lock);
#if	UNIX_LOCKS && defined(multimax)
				splnmix(spl);
#endif
			} else
				TTY_UNLOCK(ttyp);
#endif	

			cnflush();
			return;
		}
		putchar(c, flags, ttyp);
	}
again:
	c = *fmt++;

	zf = 0, fld_size = 0, precision = -1, left_adjust = 0;

	if (c == '-') {
	    left_adjust = 1;
	    c = *fmt++;
	}
	if (c == '0')
	    zf = '0';
	if (c == '0')
		zf = '0';
	for (;c <= '9' && c >= '0'; c = *fmt++)
		fld_size = fld_size * 10 + c - '0';

        if (c == '.') {
            c = *fmt++;
            if (c >= '0' && c <= '9') {
                precision = 0;
                for (; c >= '0' && c <= '9'; c = *fmt++)
                    precision = precision * 10 + c - '0';
            }
        }

	/* THIS CODE IS VAX DEPENDENT IN HANDLING %l? AND %c */
	switch (c) {

	case 'l':
		goto again;
	case 'x': case 'X':
		b = 16;
		goto number;
	case 'd': case 'D':
	case 'u':		/* what a joke */
		b = 10;
		goto number;
	case 'o': case 'O':
		b = 8;
number:
		printn((u_long)va_arg(adx, int), b, flags, ttyp, zf, fld_size);
		break;
	case 'c':
		b = va_arg(adx, int);
#if BYTE_ORDER == LITTLE_ENDIAN
		for (i = 24; i >= 0; i -= 8)
			if (c = (b >> i) & 0x7f)
				putchar(c, flags, ttyp);
#endif
#if BYTE_ORDER == BIG_ENDIAN
		if (c = (b & 0x7f))
			putchar(c, flags, ttyp);
#endif
		break;
#ifdef	mips
	case 'C':
		b = va_arg(adx, int);
#if BYTE_ORDER == LITTLE_ENDIAN
		for (i = 24; i >= 0; i -= 8)
			if (c = (b >> i) & 0xff)
				putchar(c, flags, ttyp);
#endif
#if BYTE_ORDER == BIG_ENDIAN
		if (c = (b & 0xff))
			putchar(c, flags, ttyp);
#endif
		break;

#endif	
	case 'b':
		b = va_arg(adx, int);
		s = va_arg(adx, char *);
		base = *s++;
		printn((u_long)b, base, flags, ttyp, 0, 0);
		any = 0;
		if (b) {
			while (i = *s++) {
				if (*s <= 32) {
					register int j;
					register int fld;

					j = *s++;
					fld = ((b>>(j-1)) & ((2<<(i-j))-1));
					if (fld) {
						putchar(any++ ? ',' : '<',
							flags, ttyp);
						for (; (c = *s) > 32 ; s++)
							putchar(c, flags, ttyp);
						putchar('=', flags, ttyp);
						printn((u_long)fld, base,
							flags, ttyp, 0, 0);
					} else /* skip name */
						for (; (c = *s) > 32 ; s++);
				} else if (b & (1 << (i-1))) {
					putchar(any? ',' : '<', flags, ttyp);
					any = 1;
					for (; (c = *s) > 32; s++)
						putchar(c, flags, ttyp);
				} else
					for (; *s > 32; s++)
						;
			}
				putchar('>', flags, ttyp);
		}
		break;

#ifdef	mips
	case 'r':
	case 'R':
		b = va_arg(adx, int);
		s = va_arg(adx, char *);
		if (c == 'R') {
			puts("0x", flags, ttyp);
			printn((u_long)b, 16, flags, ttyp);
		}
		any = 0;
		if (c == 'r' || b) {
			register struct reg_desc *rd;
			register struct reg_values *rv;
			register unsigned field;

			putchar('<', flags, ttyp);
			for (rd = (struct reg_desc *)s; rd->rd_mask; rd++) {
				field = b & rd->rd_mask;
				field = (rd->rd_shift > 0)
				    ? field << rd->rd_shift
				    : field >> -rd->rd_shift;
				if (any &&
				      (rd->rd_format || rd->rd_values
				         || (rd->rd_name && field)
				      )
				)
					putchar(',', flags, ttyp);
				if (rd->rd_name) {
					if (rd->rd_format || rd->rd_values
					    || field) {
						puts(rd->rd_name, flags, ttyp);
						any = 1;
					}
					if (rd->rd_format || rd->rd_values) {
						putchar('=', flags, ttyp);
						any = 1;
					}
				}
				if (rd->rd_format) {
					if (flags==0 || (flags&TOCONS))
						printf(rd->rd_format, field);
					if (flags&TOPROM)
						dprintf(rd->rd_format, field);
					if (flags&TOTTY)
						uprintf(rd->rd_format, field);
					any = 1;
					if (rd->rd_values)
						putchar(':', flags, ttyp);
				}
				if (rd->rd_values) {
					any = 1;
					for (rv = rd->rd_values;
					    rv->rv_name;
					    rv++) {
						if (field == rv->rv_value) {
							puts(rv->rv_name,
								flags, ttyp);
							break;
						}
					}
					if (rv->rv_name == NULL)
						puts("???", flags, ttyp);
				}
			}
			putchar('>', flags, ttyp);
		}
		break;

	case 'n':
	case 'N':
		{
			register struct reg_values *rv;

			b = va_arg(adx, int);
			s = va_arg(adx, char *);
			for (rv = (struct reg_values *)s; rv->rv_name; rv++) {
				if (b == rv->rv_value) {
					puts(rv->rv_name, flags, ttyp);
					break;
				}
			}
			if (rv->rv_name == NULL)
				puts("???", flags, ttyp);
			if (c == 'N' || rv->rv_name == NULL) {
				putchar(':', flags, ttyp);
				printn((u_long)b, 10, flags, ttyp);
			}
		}
		break;
#endif	/* mips */
	case 's':
		s = va_arg(adx, char *);
		if (fld_size > 0 && !left_adjust) {
		    i = strlen(s);
		    if (i > precision)
			i = precision;
		    while (i < fld_size) {
			putchar(' ', flags, ttyp);
			i++;
		    }
		}
		i = 0;
		while (c = *s++) {
		    if (++i > precision && precision != -1)
			break;
		    putchar(c, flags, ttyp);
		}
		if (i < fld_size && left_adjust) {
		    while (i < fld_size) {
			putchar(' ', flags, ttyp);
			i++;
		    }
		}
		break;

	case '%':
		putchar('%', flags, ttyp);
		break;
	}
	goto loop;
}

/*
 * Printn prints a number n in base b.
 * We don't use recursion to avoid deep kernel stacks.
 */
printn(n, b, flags, ttyp, zf, fld_size)
	u_long n;
	struct tty *ttyp;
{
	char prbuf[11];
	register char *cp;	/* can't assign cp = prbuf here-compiler? */

	if (b == 10 && (int)n < 0) {
		putchar('-', flags, ttyp);
		n = (unsigned)(-(int)n);
	}
	cp = prbuf;
	do {
		*cp++ = "0123456789abcdef"[n%b];
		n /= b;
	} while (n);
	if (fld_size) {
		for (fld_size -= cp - prbuf; fld_size > 0; fld_size--)
			if (zf)
				putchar('0', flags, ttyp);
			else
				putchar(' ', flags, ttyp);
	}
	do
		putchar(*--cp, flags, ttyp);
	while (cp > prbuf);
}


void
panic_count_ports(
	int			*port_totalp,
	int			*port_sendp,
	int			*port_receivep,
	int			*port_sendrecvp,
	int			*port_sendoncep,
	int			*port_deadnamep,
	int			*port_portsetp,
	int			*port_otherp)
{
	int			i;
	int			ncnt, tcnt;
	mach_port_t		*names;
	mach_port_type_t	*types, type;

	*port_totalp = 0;
	*port_sendp = 0;
	*port_receivep = 0;
	*port_sendrecvp = 0;
	*port_sendoncep = 0;
	*port_deadnamep = 0;
	*port_portsetp = 0;
	*port_otherp = 0;

	i = mach_port_names( mach_task_self(), (mach_port_array_t *)&names,
				(mach_msg_type_number_t *)&ncnt,
				(mach_port_type_array_t *)&types,
				(mach_msg_type_number_t *)&tcnt);
	if (i != KERN_SUCCESS) {
		printf("panic_count_ports: mach_port_names() fail=%d\n", i);
		return;
	}
	for (i=0; i<ncnt; ++i) {
		++*port_totalp;
		type = types[i] &
				(MACH_PORT_TYPE_SEND |
				MACH_PORT_TYPE_RECEIVE |
				MACH_PORT_TYPE_SEND_ONCE |
				MACH_PORT_TYPE_PORT_SET |
				MACH_PORT_TYPE_DEAD_NAME);
		switch (type) {
		case MACH_PORT_TYPE_SEND:
			++*port_sendp;
			break;
		case MACH_PORT_TYPE_RECEIVE:
			++*port_receivep;
			break;
		case MACH_PORT_TYPE_SEND | MACH_PORT_TYPE_RECEIVE:
			++*port_sendrecvp;
			break;
		case MACH_PORT_TYPE_SEND_ONCE:
			++*port_sendoncep;
			break;
		case MACH_PORT_TYPE_PORT_SET:
			++*port_portsetp;
			break;
		case MACH_PORT_TYPE_DEAD_NAME:
			++*port_deadnamep;
			break;
		default:
			++*port_otherp;
			break;
		}
	}
	i = vm_deallocate(mach_task_self(), (vm_address_t)names,
				(vm_size_t)(ncnt*sizeof(*names)));
	if (i != KERN_SUCCESS) {
		printf("panic_count_ports: vm_dealloc of names fail: %d\n", i);
	}
	i = vm_deallocate(mach_task_self(), (vm_address_t)types,
				(vm_size_t)(ncnt*sizeof(*types)));
	if (i != KERN_SUCCESS) {
		printf("panic_count_ports: vm_dealloc of types fail: %d\n", i);
	}
	return;
}

panic_init()
{
	simple_lock_init(&panic_lock);
}


/*
 * Panic is called on unresolvable fatal errors.  It prints
 * "[node: <node>] <message>" and either drops into the debugger or
 * reboots.  Attempts to continue from a panic cause the message to
 * just be repeated.  If a developer really wants the routine to return
 * to its caller (not what callers have in mind!) set the global
 * "panic_continue_flag" non-zero.  If we are called twice, then we avoid
 * trying to sync the disks as this often leads to recursive panics.
 */
int panic_continue_flag = 0;		/* set only by patching via debugger */

panicck(){}	/* does nothing but provide a post-panic breakpoint spot */

panic(s, x1, x2, x3, x4, x5, x6, x7, x8, x9)
	char		*s;
	int		x1, x2, x3, x4, x5, x6, x7, x8, x9;
{
	int bootopt = RB_AUTOBOOT|RB_DEBUGGER;
	static int	 panicking = 0;	/* Yup, "panicking" is spelled right */
	char *panic_reprint = "";
	char *panic_prefix = "*** ";
	char *p, *q;
	kern_return_t	ret;
	int		port_total, port_send, port_receive, port_sendrecv,
			port_sendonce, port_deadname, port_portset, port_other;
        char buf[PRINTF_BUFSIZE], buf2[PRINTF_BUFSIZE];

	if (panicstr)
		bootopt |= RB_NOSYNC;
	else {
		panicstr = s;		 /* remembers if we've EVER panicked */
	}

reprint:
	if (!panicking) {
		panicking = 1;		/* cleared when we're done panicking */
		p = buf;
		p += sprintf(p, panic_prefix);
		p += sprintf(p, "[node %d] server panic: ", this_node);
		p += sprintf(p, s, x1, x2, x3, x4, x5, x6, x7, x8, x9);
		panic_count_ports(&port_total, &port_send, &port_receive,
				&port_sendrecv, &port_sendonce, &port_deadname,
				&port_portset, &port_other);
		p += sprintf(p,
		 "\n#PORTS=%d: Sn=%d Re=%d S+R=%d SO=%d DN=%d PS=%d oth=%d\n",
		 port_total, port_send, port_receive, port_sendrecv,
		 port_sendonce, port_deadname, port_portset, port_other);

		/*
		 * Format the message for easy identification.
		 */
		p = buf; q = buf2;
		q += sprintf(q, "%s", panic_reprint);
		q += sprintf(q, "\n%s%s\n", panic_prefix, panic_prefix);
		while (*q = *p++) {
			if (*q++ == '\n' ) {
				q += sprintf(q, "%s", panic_prefix);
			}
		}
		sprintf(q, "%s\n", panic_prefix);

		if (this_node != root_fs_node &&
		 MACH_PORT_VALID(root_fs_port)) {
			/*
			 * We overload the root_fs_port for remote printing.
			 */
		        ux_server_thread_blocking();
			ret = ubsd_remote_print(root_fs_port, BSD_PRINT, 
						0, 0, 0, buf2, strlen(buf2));
			ux_server_thread_unblocking();
			if (ret != KERN_SUCCESS) {
				printf("[node: %d] error within panic: ubsd_"
				 "_remote_print: buf2=0x%x len=%d ret=0x%x\n",
				 buf2, strlen(buf2), ret);
				printf(buf2);
			}
		}
		/*
		 * Print the panic message on the current node, whether we are
		 * the boot node or not.
		 */
		printf(buf2);
		panicking = 0;		/* say we're done panicking */
	} else {
		/* Introduce delay to give the other panicker time to do his
		 * printf, in case we are a different thread from
		 * the one already doing a panic.  If we are the same thread,
		 * this is a recursive panic (caused by the printf) and we
		 * will just go on to the reboot or Debugger invocation after
		 * the delay:
		 */
		volatile int i;
		for (i=1; i < 100000; i++);
	}

	if (boothowto&RB_KDB) {
		Debugger();
	} else
		pps_boot(RB_PANIC, bootopt);

	if (panic_continue_flag == 0) {	/* normally it *is* 0 */
		panic_reprint = "\nRepeating message:\n";
		goto reprint;
	}

	panicck();		/* allow a bkpt; routine does nothing */

#if   XPR_DEBUG
	xpr_dump(0);
#endif	/* XPR_DEBUG */
}

/*
 * Warn that a system table is full.
 */
tablefull(tab)
	char *tab;
{

	log(LOG_ERR, "%s: table is full\n", (unsigned) tab);
}

/*
 * Hard error is the preface to plaintive error messages
 * about failing disk transfers.
 */
#ifdef	mips
#define minorbits	4
#else
#define minorbits	3
#endif
harderr(bp, cp)
	struct buf *bp;
	char *cp;
{
	printf("%s%d%c: hard error sn%d ",
		cp, minor(bp->b_dev) >> minorbits,
		'a' + (minor(bp->b_dev) & ~(~0 << minorbits)), bp->b_blkno);
}

/*
 * Print a character on console or users terminal.
 * If destination is console then the last MSGBUFS characters
 * are saved in msgbuf for inspection later.
 */
/*ARGSUSED*/
putchar(c, flags, tp)
	register int c;
	struct tty *tp;
{
	if (flags & TOSTR) {
		register char **cpp = (char **)tp;

		*(*cpp)++ = c;
		return;
	}
	if (panicstr)
		constty = 0;
	if ((flags & TOCONS) && tp == NULL && constty) {
		tp = constty;
		flags |= TOTTY;
	}
#ifdef	mips
#define msgbuf (*pmsgbuf)

	if (flags & TOPROM)
		prom_putchar(c);
#endif	
	if ((flags & TOTTY) && tp && tputchar(c, tp) < 0 &&
	    (flags & TOCONS) && tp == constty)
		constty = 0;
	if ((flags & TOLOG) && c != '\0' && c != '\r' && c != 0177
#ifdef	vax
	    && mfpr(MAPEN)
#endif
	    ) {
		if (msgbuf.msg_magic != MSG_MAGIC) {
			register int i;

			msgbuf.msg_magic = MSG_MAGIC;
			msgbuf.msg_bufx = msgbuf.msg_bufr = 0;
			for (i=0; i < MSG_BSIZE; i++)
				msgbuf.msg_bufc[i] = 0;
		}
		msgbuf.msg_bufc[msgbuf.msg_bufx++] = c;
		if (msgbuf.msg_bufx < 0 || msgbuf.msg_bufx >= MSG_BSIZE)
			msgbuf.msg_bufx = 0;
	}
	if ((flags & TOCONS) && constty == 0 && c != '\0')
		(*v_putc)(c);
}

server_bsd_uprint(pid, sid, buf, buflen)
pid_t		pid;
pid_t		sid;
char *		buf;
int		buflen;
{
	va_list ap;
	struct tty *tp = NULL;
	struct vproc *vp;

	/* obtain the vproc for process whose tty we want to write to */
	vp = LOCATE_VPROC_PID(pid);

	/* This is guts of uprintf */
	unix_master();		/* sessions, sigh */
	if (VPOP_CTTY_GETATTR(vp, 0, 0, 0, 0, &tp) != ESUCCESS)
		tp = NULL;
	if (tp != NULL && tp->t_sid == sid) {
		(void)ttycheckoutq(tp, 1);
		prf(buf, ap, TOTTY, tp);
	} else {
		prf(buf, ap, TOCONS, (struct tty *)0);
		cnflush();
	}
	unix_release();
}

server_bsd_tprint(pid, buf, buflen)
pid_t		pid;
char		*buf;
int		buflen;
{
	va_list ap;
	struct tty *tp;
	int flags = TOTTY | TOLOG;
	struct vproc *vp;

	unix_master();		/* sessions, sigh */
	/* obtain the vproc for process whose tty we want to write to */
	vp = LOCATE_VPROC_PID(pid);
	if (VPOP_CTTY_GETATTR(vp, 0, 0, 0, 0, &tp) != ESUCCESS) {
		unix_release();
		return;
	} else {
		unix_release();
	}

	logpri(LOG_INFO);
	if (ttycheckoutq(tp, 0) == 0)
		flags = TOLOG;
	prf(buf, ap, flags, tp);
	logwakeup();
	cnflush();
}

server_bsd_ttyprint(pid, buf, buflen)
pid_t		pid;
char		*buf;
int		buflen;
{
	va_list ap;
	struct tty *tp;
	struct vproc *vp;

	unix_master();		/* sessions, sigh */
	/* obtain the vproc for process whose tty we want to write to */
	vp = LOCATE_VPROC_PID(pid);
	if (VPOP_CTTY_GETATTR(vp, 0, 0, 0, 0, &tp) != ESUCCESS) {
		unix_release();
		return;
	} else {
		unix_release();
	}

	prf(buf, ap, TOTTY, tp);
}

server_bsd_log(level, buf, buflen)
int		level;
char		*buf;
int		buflen;
{
	va_list ap;
	register s = splhigh();
	extern int log_open;

	logpri(level);
	prf(buf, ap, TOLOG, (struct tty *)0);
	splx(s);
	if (!log_open)
		prf(buf, ap, TOCONS, (struct tty *)0);
	logwakeup();
}

server_bsd_print(buf , buflen)
char		*buf;
int		buflen;
{
	va_list ap;

	prf(buf, ap, TOCONS | TOLOG, (struct tty *)0);
	cnflush();
}

sbsd_remote_print(root_port, action, pid, sid, level, buf, buflen)
	mach_port_t	root_port;
	int		action;
	pid_t		pid;
	pid_t		sid;
	int		level;
	char		*buf;
	int		buflen;
{
	int		error;
	int		syscode = 2010;
	struct uthread	*uth = &u;

	*(buf+buflen) = '\0';
#ifdef	SYSCALLTRACE
	uth->uu_spare[0] = 0; 	/* hack */
#endif	/* SYSCALLTRACE */

	/*
	 * The root_fs_port is overloaded for remote printing.
	 */
	error = start_fsvrport_op(root_port, MACH_PORT_NULL, 0, syscode, 0);
	if (error) {
                return(error);
	}

	switch(action) {
	case BSD_PRINT:
		server_bsd_print(buf , buflen);
		break;
	case BSD_UPRINT:
		server_bsd_uprint(pid, sid, buf, buflen);
		break;
	case BSD_TPRINT:
		server_bsd_tprint(pid, buf, buflen);
		break;
	case BSD_TTYPRINT:
		server_bsd_ttyprint(pid, buf, buflen);
		break;
	case BSD_LOG:
		server_bsd_log(level, buf, buflen);
		break;
	default:
		panic("bogus remote printing request");
	}

	error = end_fsvrport_op(error, 0);

	return(KERN_SUCCESS);
}
