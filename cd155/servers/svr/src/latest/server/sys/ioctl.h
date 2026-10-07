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
 
/* @(#)ioctl.h	2.1 16:08:02 4/20/90 SecureWare */
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
 * HISTORY
 * $Log: ioctl.h,v $
 * Revision 1.8  1994/11/18  20:40:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  18:25:03  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:33:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/27  22:19:31  hobbes
 * Added ioctl values for setting and deleting HIPPI address
 * resolution entries, and the value for getting the device
 * port of the HIPPI device.
 *
 * Revision 1.5  1993/05/06  19:19:12  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:07:34  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:01:03  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:59:58  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:42:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  92/11/06  12:48:20  mjl
 * Added FIOGETNODE ioctl under #ifdef OSF1_ADFS.
 * 
 * Revision 2.2  91/08/31  14:06:45  rabii
 * Revision 2.3  1992/11/06  12:48:20  mjl
 * Added FIOGETNODE ioctl under #ifdef OSF1_ADFS.
 *
 * Revision 2.2  91/08/31  14:06:45  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:43:02  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11  90/10/07  14:50:58  devrcs
 * 	changed USE_OLD_TTY to _USE_OLD_TTY, USE_NEW_TTY to _USE_NEW_TTY
 * 	[90/10/04  09:38:58  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:40:16  gm]
 * 
 * Revision 1.10  90/08/09  13:28:40  devrcs
 * 	Delete CMU ttyloc, vcons, rpause ioctl's.
 * 	Update to BSD Reno (pty EXT, SIG).
 * 	Add multicast add/delete.
 * 	[90/08/01  14:49:02  tmt]
 * 
 * 	Add KJI line discipline.
 * 	[90/07/26  14:03:41  morris]
 * 
 * Revision 1.9  90/07/17  11:42:07  devrcs
 * 	Add new security STREAMS ioctls.
 * 	[90/07/10  22:01:10  seiden]
 * 
 * Revision 1.8  90/06/29  13:53:49  devrcs
 * 	Add new STREAMS ioctls.
 * 	[90/06/26  12:19:19  brezak]
 * 
 * Revision 1.7  90/06/22  20:53:02  devrcs
 * 	Added SVID ioctls for tty.
 * 	[90/06/14  23:01:50  havens]
 * 
 * 	Removed the JUNK_* ioctls and added the SVID ioctls.
 * 	[90/06/13  12:50:41  havens]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:46:01  seiden]
 * 
 * 	Posix tty support.
 * 	[90/02/06  12:07:49  morris]
 * 
 * Revision 1.6  90/05/13  20:33:35  devrcs
 * 	Changes for gcc for osc.10
 * 	[90/05/02  19:42:59  brezak]
 * 
 * 	Changes for gcc
 * 	[90/05/01  21:49:35  brezak]
 * 
 * Revision 1.5  90/04/27  19:25:50  devrcs
 * 	Add ioctl's for SLIP, modify some SIOC ioctl's to follow
 * 	compat for bsd4.4-format sockaddr's, also see socket.h.
 * 	Removed #ifdef mips socket ioctl's - not supported.
 * 	Cleaned up #ifdef's: _KERNEL, commented #endif tags.
 * 	[90/04/19  16:35:13  tmt]
 * 
 * Revision 1.4  90/03/13  20:49:21  mbrown
 * 	duplicates with sys/termio.h...added ifdef kernel to play safe.
 * 	[90/02/25  14:46:55  mbrown]
 * 
 * Revision 1.3  90/02/16  16:52:14  devrcs
 * 	Fixed FSPARAM and NOSPC ioctls.
 * 	[90/02/16  00:14:30  ers]
 * 
 * 	Do not define USE_OLD_TTY if KERNEL.
 * 	[90/02/15  15:33:59  ers]
 * 
 * 	Define USE_OLD_TTY unless USE_NEW_TTY.
 * 	[90/02/15  14:41:19  ers]
 * 
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:04:34  ers]
 * 
 * Revision 1.2  90/01/02  20:15:56  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:15  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:18:01  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.15  89/07/12  01:27:56  jjc
 * 	Defined NTABLDISC for Sun.
 * 	[89/07/05  21:14:36  jjc]
 * 
 * Revision 2.14  89/05/03  14:58:31  mikeg
 * 	Defined TIOCGSIZE and TIOCSSIZE to be TIOCGWINSZ and
 * 	TIOCSWINSZ respectively and made other modifications
 * 	to the TIOC[GS]SIZE family of ioctl's so that both X
 * 	and Suntools will run correctly on Sun's.
 * 	[89/04/12  12:08:28  mikeg]
 * 
 * Revision 2.13  89/03/09  22:05:08  rpd
 * 	More cleanup.
 * 
 * Revision 2.12  89/02/25  17:54:17  gm0w
 * 	Made all code that depended on CMUCS unconditional
 * 	except for ttyloc stuff.
 * 	[89/02/14            mrt]
 * 
 * Revision 2.11  89/01/23  22:26:42  af
 * 	Added a bunch of Mips-specific ioctls.
 * 	[89/01/04            af]
 * 
 * Revision 2.10  88/12/13  13:19:29  mikeg
 * 	Enclosed new Sun specific ioctl's I added previously
 * 	in "#ifdef sun" conditional.
 * 
 * Revision 2.9  88/12/08  15:36:01  mikeg
 * 	Added the Sun ioctl's TIOCSSIZE, _O_TIOCSSIZE,
 * 	_N_TIOCSSIZE, TIOCGSIZE, _O_TIOCGSIZE, and _N_TIOCGSIZE
 * 	to allow Suntools to run on Mach.
 * 	[88/11/30  12:16:14  mikeg]
 * 
 * Revision 2.7  88/11/14  14:34:08  gm0w
 * 	Added CSNET DialupIP support.
 * 	[88/11/14            gm0w]
 * 
 * Revision 2.6  88/09/27  17:31:58  rvb
 * 	Fixed Log
 * 
 * Revision 2.5  88/09/27  16:08:19  rvb
 * 	QIOCCIFNCTRS: Clear interface counters.
 * 	[88/09/23            rvb]
 * 
 * Revision 2.4  88/08/24  02:29:23  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:13:38  mwyoung]
 * 
 * Revision 2.3  88/08/22  21:29:21  mja
 * 	Flush obsolete SIOCBROAD definition.
 * 	[88/08/09  09:54:45  mja]
 * 
 * 	Delete obsolete TIOCSCONS definition;  add TIOCVCONS and
 * 	TIOCGCONS definitions;  coalesce CMUCS conditionsals.
 * 	[88/08/04  17:59:21  mja]
 * 
 * Revision 0.0  87/11/10            mja
 * 	Fix to conditionalize all local exported names on only CMU or
 * 	CMUCS and retain special purpose kernel conditionals as
 * 	comments only; define TIOCCONS on all architectures.
 * 	[ V5.1(XF21) ]
 * 	[87/11/10            mja]
 * 
 * Revision 0.0  87/03/17            mja
 * 	CMUCS:  Added TIOCSLOC definition.
 * 	[ V5.1(F7) ]
 * 	[87/03/17            mja]
 * 
 * Revision 0.0  87/02/07            avie
 * 	Merge VICE changes -- include vice.h and change to #if VICE.
 * 	The ioctl's for console munging should be consolidated.
 * 	[87/02/07            avie]
 * 
 * Revision 0.0  87/01/30            mja
 * 	CMUCS: Added TIOCLOGINDEV definition;
 * 	CMUCS: Added FIOCNOSPC definition.
 * 	[ V5.1(F1) ]
 * 	[87/01/30            mja]
 * 
 * Revision 0.0  87/01/29            jjk
 * 	VICE:  restored 4.2 manifest TIOCSCONS.
 * 	[87/01/29            jjk]
 * 
 * Revision 0.0  86/12/24            bolosky
 * 	romp: Added TIOCCONS ioctl for RT console silliness.
 * 	[86/12/24            bolosky]
 * 
 * Revision 0.0  86/11/19            mja
 * 	CMUCS:  Added FIOCFSPARAM.
 * 	[ V5.1(F1) ]
 * 	[86/11/19            mja]
 * 
 * Revision 0.0  86/08/05            jjc
 * 	Added for Sun:
 * 	1) Added mouse and keyboard disciplines for Sun
 * 	[86/08/05            jjc]
 * 
 * Revision 0.0  86/02/22            mja
 * 	CMUCS:  Fix incorrect read/write sense in TIOCCSET,
 * 	TIOCCLOG, TIOCCHECK, and TIOCATTACH definitions.
 * 	[86/02/22            mja]
 * 
 * Revision 0.0  86/01/25            avie
 * 	Upgraded to 4.3.
 * 	[86/01/25            avie]
 * 
 * Revision 0.0  85/05/09            mja
 * 	Upgraded from 4.1BSD.  Carried over changes below.
 * 	CMUCS:  Added new TIOCCSET call and CDETHUP and CBRKINH
 * 	mode bits; added new TIOCCLOG, TIOCCHECK and TIOCATTACH calls;
 * 	CMUCS:  Fixed to include sgtty.h from kernel area.
 * 	[V1(1)]
 * 	[85/05/09            mja]
 * 
 * Revision 0.0  83/03/28            mja
 * 	CMUCS:  Added TIOCGLOC definition (V3.06h).
 * 	[83/03/28            mja]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ioctl.h	7.12 (Berkeley) 5/18/90
 */
/*
 * Ioctl definitions
 */

#ifndef	_SYS_IOCTL_H_
#define _SYS_IOCTL_H_

#include <sys/types.h>
#include <sys/secdefines.h>

/*
 * Window/terminal size structure.
 * This information is stored by the kernel
 * in order to provide a consistent interface,
 * but is not used by the kernel.
 */
struct winsize {
	unsigned short	ws_row;			/* rows, in characters */
	unsigned short	ws_col;			/* columns, in characters */
	unsigned short	ws_xpixel;		/* horizontal size, pixels */
	unsigned short	ws_ypixel;		/* vertical size, pixels */
};

/*
 * Pun for SUN.
 */
struct ttysize {
	unsigned short	ts_lines;
	unsigned short	ts_cols;
	unsigned short	ts_xxx;
	unsigned short	ts_yyy;
};
#define	TIOCGSIZE	TIOCGWINSZ
#define	TIOCSSIZE	TIOCSWINSZ

#if	defined(_KERNEL) && defined(sun)
/*
 * The following structure and ioctl's have been added to allow
 * Suntools to run under Mach.
 */

/*
 * Sun version of the winsize struct.
 */
struct swsize {
	int	ts_lines;	/* number of lines on terminal */
	int	ts_cols;	/* number of columns on terminal */
};

/*
 * These ioctl's are used in Suntools.  Although they may seem to conflict
 * with existing 4.3 ioctl's, either the resulting value is different
 * from the other ioctl with the same number or there is code in the
 * appropriate driver to distinguish between them.
 */
#define	_O_TIOCSSIZE	_IOW('t',103,struct swsize)	/* set tty size */
#define	_O_TIOCGSIZE	_IOR('t',102,struct swsize)	/* get tty size */
#define	_N_TIOCSSIZE	_IOW('t',37,struct swsize)	/* set tty size */
#define	_N_TIOCGSIZE	_IOR('t',38,struct swsize)	/* get tty size */

#endif	/* _KERNEL && sun */


#ifndef _IO
/*
 * Ioctl's have the command encoded in the lower word,
 * and the size of any in or out parameters in the upper
 * word.  The high 3 bits of the upper word are used
 * to encode the in/out status of the parameter.
 */
#define	IOCPARM_MASK	0x1fff		/* parameter length, at most 13 bits */
#define	IOCPARM_LEN(x)	(((x) >> 16) & IOCPARM_MASK)
#define	IOCBASECMD(x)	((x) & ~IOCPARM_MASK)
#define	IOCGROUP(x)	(((x) >> 8) & 0xff)

#define	IOCPARM_MAX	NBPG		/* max size of ioctl, mult. of NBPG */
#define IOC_VOID	0x20000000	/* no parameters */
#define IOC_OUT		0x40000000	/* copy out parameters */
#define IOC_IN		0x80000000	/* copy in parameters */
#define IOC_INOUT	(IOC_IN|IOC_OUT)
#define	IOC_DIRMASK	0xe0000000	/* mask for IN/OUT/VOID */

#define _IOC(inout,group,num,len) \
	(inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
#define	_IO(g,n)	_IOC(IOC_VOID,	(g), (n), 0)
#define	_IOR(g,n,t)	_IOC(IOC_OUT,	(g), (n), sizeof(t))
#define	_IOW(g,n,t)	_IOC(IOC_IN,	(g), (n), sizeof(t))

/* this should be _IORW, but stdio got there first */
#define	_IOWR(g,n,t)	_IOC(IOC_INOUT,	(g), (n), sizeof(t))
#endif

/*
 * tty ioctl commands
 */
						/* 0-2 compat */
#define	TIOCMODG	_IOR('t', 3, int)	/* get modem control state */
#define	TIOCMODS	_IOW('t', 4, int)	/* set modem control state */
#define		TIOCM_LE	0001		/* line enable */
#define		TIOCM_DTR	0002		/* data terminal ready */
#define		TIOCM_RTS	0004		/* request to send */
#define		TIOCM_ST	0010		/* secondary transmit */
#define		TIOCM_SR	0020		/* secondary receive */
#define		TIOCM_CTS	0040		/* clear to send */
#define		TIOCM_CAR	0100		/* carrier detect */
#define		TIOCM_CD	TIOCM_CAR
#define		TIOCM_RNG	0200		/* ring */
#define		TIOCM_RI	TIOCM_RNG
#define		TIOCM_DSR	0400		/* data set ready */
						/* 8-10 compat */
#define	TIOCEXCL	_IO('t', 13)		/* set exclusive use of tty */
#define	TIOCNXCL	_IO('t', 14)		/* reset exclusive use of tty */
						/* 15 unused */
#define	TIOCFLUSH	_IOW('t', 16, int)	/* flush buffers */
						/* 17-18 compat */
#define	TIOCGETA	_IOR('t', 19, struct termios) /* get termios struct */
#define	TIOCSETA	_IOW('t', 20, struct termios) /* set termios struct */
#define	TIOCSETAW	_IOW('t', 21, struct termios) /* drain output, set */
#define	TIOCSETAF	_IOW('t', 22, struct termios) /* drn out, fls in, set */
/* System V tty ioctls */
#define TCGETA	        _IOR('t', 23, struct termio) /* get termio struct */
#define TCSETA	        _IOW('t', 24, struct termio) /* set termio struct */
#define TCSETAW	        _IOW('t', 25, struct termio) /* drain output, set */
#define TCSETAF	        _IOW('t', 28, struct termio) /* drn out, flsh, set */
#define TCSBREAK        _IO('t', 29)		/* Send break */
#define TCSBRK          TCSBREAK
#define TCXONC          _IO('t', 30)		/* Set flow control */
#define TCFLSH          _IO('t', 31)		/* Flush queue */

#define	TIOCGETD	_IOR('t', 26, int)	/* get line discipline */
#define	TIOCSETD	_IOW('t', 27, int)	/* set line discipline */


/* locals, from 127 down */
						/* 127-124 compat */
#define	TIOCSBRK	_IO('t', 123)		/* set break bit */
#define	TIOCCBRK	_IO('t', 122)		/* clear break bit */
#define	TIOCSDTR	_IO('t', 121)		/* set data terminal ready */
#define	TIOCCDTR	_IO('t', 120)		/* clear data terminal ready */
#define	TIOCGPGRP	_IOR('t', 119, int)	/* get pgrp of tty */
#define	TIOCSPGRP	_IOW('t', 118, int)	/* set pgrp of tty */
						/* 117-116 compat */
#define	TIOCOUTQ	_IOR('t', 115, int)	/* output queue size */
#define	TIOCSTI		_IOW('t', 114, char)	/* simulate terminal input */
#define	TIOCNOTTY	_IO('t', 113)		/* void tty association */
#define	TIOCPKT		_IOW('t', 112, int)	/* pty: set/clear packet mode */
#define		TIOCPKT_DATA		0x00	/* data packet */
#define		TIOCPKT_FLUSHREAD	0x01	/* flush packet */
#define		TIOCPKT_FLUSHWRITE	0x02	/* flush packet */
#define		TIOCPKT_STOP		0x04	/* stop output */
#define		TIOCPKT_START		0x08	/* start output */
#define		TIOCPKT_NOSTOP		0x10	/* no more ^S, ^Q */
#define		TIOCPKT_DOSTOP		0x20	/* now do ^S ^Q */

#define	TIOCSTOP	_IO('t', 111)		/* stop output, like ^S */
#define	TIOCSTART	_IO('t', 110)		/* start output, like ^Q */
#define	TIOCMSET	_IOW('t', 109, int)	/* set all modem bits */
#define	TIOCMBIS	_IOW('t', 108, int)	/* bis modem bits */
#define	TIOCMBIC	_IOW('t', 107, int)	/* bic modem bits */
#define	TIOCMGET	_IOR('t', 106, int)	/* get all modem bits */
#define	TIOCREMOTE	_IOW('t', 105, int)	/* remote input editing */
#define	TIOCGWINSZ	_IOR('t', 104, struct winsize)	/* get window size */
#define	TIOCSWINSZ	_IOW('t', 103, struct winsize)	/* set window size */
#define	TIOCUCNTL	_IOW('t', 102, int)	/* pty: set/clr usr cntl mode */
#define		UIOCCMD(n)	_IO('u', n)		/* usr cntl op "n" */
#define	TIOCCONS	_IOW('t', 98, int)		/* become virtual console */
#define	TIOCSCTTY	_IO('t', 97)		/* become controlling tty */
#define TIOCEXT		_IOW('t', 96, int)	/* pty: external processing */
#define TIOCSIG		_IO('t', 95)		/* pty: generate signal */

/* SLIP (Serial Line IP) ioctl's */
#define	SLIOGUNIT	_IOR('t', 88, int)	/* get slip unit number */
#define	SLIOCSFLAGS	_IOW('t', 89, int)	/* set configuration flags */
#define	SLIOCGFLAGS	_IOR('t', 90, int)	/* get configuration flags */

#define TTYDISC		0		/* termios tty line discipline */
#define TABLDISC	3		/* tablet discipline */
#define SLIPDISC	4		/* serial IP discipline */
#ifdef	sun
#define MOUSELDISC      5               /* mouse discipline */
#define KBDLDISC        6               /* up/down keyboard trans (console) */
#define NTABLDISC	7		/* gtco tablet discipline */
#endif
#define DUDISC		7		/* Dialup IP discipline */
#define KJIDISC         8               /* Kanji Shift JIS discipline */

/*
 * Compatability with old terminal driver
 *
 * Source level -> #define _USE_OLD_TTY
 * Kernel level -> options COMPAT_43
 */
#if	!defined(_KERNEL) && !defined(_USE_OLD_TTY) && !defined(_USE_NEW_TTY)
#define _USE_OLD_TTY 1
#endif

#if defined(_USE_OLD_TTY) || defined(COMPAT_43)
#include <sys/ioctl_compat.h>
#endif

#define	FIOCLEX		_IO('f', 1)		/* set close on exec on fd */
#define	FIONCLEX	_IO('f', 2)		/* remove close on exec */
#define FIODEVPORT	_IO('f', 100)	/* get the device port for raw HIPPI */
#define	FIONREAD	_IOR('f', 127, int)	/* get # bytes to read */
#define	FIONBIO		_IOW('f', 126, int)	/* set/clear non-blocking i/o */
#define	FIOASYNC	_IOW('f', 125, int)	/* set/clear async i/o */
#define	FIOSETOWN	_IOW('f', 124, int)	/* set owner */
#define	FIOGETOWN	_IOR('f', 123, int)	/* get owner */
#ifdef	OSF1_ADFS
#define FIOGETNODE	_IOR('f', 122, node_t)	/* get fileserver node */
#endif

/* socket i/o controls */

#define	SIOCSHIWAT	_IOW('s',  0, int)		/* set high watermark */
#define	SIOCGHIWAT	_IOR('s',  1, int)		/* get high watermark */
#define	SIOCSLOWAT	_IOW('s',  2, int)		/* set low watermark */
#define	SIOCGLOWAT	_IOR('s',  3, int)		/* get low watermark */
#define	SIOCATMARK	_IOR('s',  7, int)		/* at oob mark? */
#define	SIOCSPGRP	_IOW('s',  8, int)		/* set process group */
#define	SIOCGPGRP	_IOR('s',  9, int)		/* get process group */
#if SEC_ARCH
#define	SIOCGPEERPRIV	_IOR('s', 10, int)		/* get peer privs */
#endif

#define	SIOCADDRT	_IOW('r', 10, struct ortentry)	/* add route */
#define	SIOCDELRT	_IOW('r', 11, struct ortentry)	/* delete route */

#define	SIOCSIFADDR	_IOW('i', 12, struct ifreq)	/* set ifnet address */
#define	SIOCSIFDSTADDR	_IOW('i', 14, struct ifreq)	/* set p-p address */
#define	SIOCSIFFLAGS	_IOW('i', 16, struct ifreq)	/* set ifnet flags */
#define	SIOCGIFFLAGS	_IOWR('i',17, struct ifreq)	/* get ifnet flags */
#define	SIOCSIFBRDADDR	_IOW('i', 19, struct ifreq)	/* set broadcast addr */
#define	SIOCSIFNETMASK	_IOW('i', 22, struct ifreq)	/* set net addr mask */
#define	SIOCGIFMETRIC	_IOWR('i',23, struct ifreq)	/* get IF metric */
#define	SIOCSIFMETRIC	_IOW('i', 24, struct ifreq)	/* set IF metric */
#define	SIOCDIFADDR	_IOW('i', 25, struct ifreq)	/* delete IF addr */
#define	SIOCAIFADDR	_IOW('i', 26, struct ifaliasreq)/* add/chg IF alias */
#define SIOCSIFADDR	_IOW('i', 12, struct ifreq)	/* set ifnet address */
#define	SIOCSARP	_IOW('i', 30, struct arpreq)	/* set arp entry */
#define	SIOCDARP	_IOW('i', 32, struct arpreq)	/* delete arp entry */
#define SIOCSHART	_IOW('i', 40, struct art_pentry) /* set HIPPI ART entry */
#define SIOCDHART	_IOW('i', 41, struct art_pentry) /* delete HIPPI ART */

#define SIOCADDMULTI	_IOW('i', 49, struct ifreq)	/* add m'cast addr */
#define SIOCDELMULTI	_IOW('i', 50, struct ifreq)	/* del m'cast addr */

#if	defined(_SOCKADDR_LEN) || defined(_KERNEL)
/* BSD4.4 sockaddr format accepted and returned - also see sys/socket.h */
#define	OSIOCGIFADDR	_IOWR('i',13, struct ifreq)	/* get ifnet address */
#define	SIOCGIFADDR	_IOWR('i',33, struct ifreq)	/* get ifnet address */
#define	OSIOCGIFDSTADDR	_IOWR('i',15, struct ifreq)	/* get p-p address */
#define	SIOCGIFDSTADDR	_IOWR('i',34, struct ifreq)	/* get p-p address */
#define	OSIOCGIFBRDADDR	_IOWR('i',18, struct ifreq)	/* get broadcast addr */
#define	SIOCGIFBRDADDR	_IOWR('i',35, struct ifreq)	/* get broadcast addr */
#define	OSIOCGIFCONF	_IOWR('i',20, struct ifconf)	/* get ifnet list */
#define	SIOCGIFCONF	_IOWR('i',36, struct ifconf)	/* get ifnet list */
#define	OSIOCGIFNETMASK	_IOWR('i',21, struct ifreq)	/* get net addr mask */
#define	SIOCGIFNETMASK	_IOWR('i',37, struct ifreq)	/* get net addr mask */
#define	OSIOCGARP	_IOWR('i',31, struct arpreq)	/* get arp entry */
#define	SIOCGARP	_IOWR('i',38, struct arpreq)	/* get arp entry */
#else
/* BSD4.3 sockaddr format */
#define	SIOCGIFADDR	_IOWR('i',13, struct ifreq)	/* get ifnet address */
#define	SIOCGIFDSTADDR	_IOWR('i',15, struct ifreq)	/* get p-p address */
#define	SIOCGIFBRDADDR	_IOWR('i',18, struct ifreq)	/* get broadcast addr */
#define	SIOCGIFCONF	_IOWR('i',20, struct ifconf)	/* get ifnet list */
#define	SIOCGIFNETMASK	_IOWR('i',21, struct ifreq)	/* get net addr mask */
#define	SIOCGARP	_IOWR('i',31, struct arpreq)	/* get arp entry */
#endif

/*
 * STREAMS ioctl commands - group 'S'
 */

#define	I_NREAD		_IO('S', 1) /* return the number of bytes in 1st msg */
#define	I_PUSH		_IO('S', 2) /* push module just below stream head */
#define	I_POP		_IO('S', 3) /* pop module below stream head */
#define	I_LOOK		_IO('S', 4) /* retrieve name of first stream module */
#define	I_FLUSH		_IO('S', 5) /* flush all input and/or output queues */
#define	I_SRDOPT	_IO('S', 6) /* set the read mode */
#define	I_GRDOPT	_IO('S', 7) /* get the current read mode */
#define	I_STR		_IO('S', 8) /* create an internal ioctl message */
#define	I_SETSIG	_IO('S', 9) /* request SIGPOLL signal on events */
#define	I_GETSIG	_IO('S',10) /* query the registered events */
#define	I_FIND		_IO('S',11) /* check for module in stream */
#define	I_LINK		_IO('S',12) /* connect stream under mux fd */
#define	I_UNLINK	_IO('S',13) /* disconnect two streams */
#define	I_PEEK		_IO('S',15) /* peek at data on read queue */
#define	I_FDINSERT	_IO('S',16) /* create a message and send downstream */
#define	I_SENDFD	_IO('S',17) /* send an fd to a connected pipe stream */
#define	I_RECVFD	_IO('S',18) /* retrieve a file descriptor */
#define	I_GETMSG	_IO('S',40) /* getmsg() system call */
#define	I_PUTMSG	_IO('S',41) /* putmsg() system call */
#if SEC_BASE
#define I_STR_ATTR      _IO('S',60)
#define I_PEEK_ATTR     _IO('S',61)
#define I_FDINSERT_ATTR _IO('S',62)
#define I_SENDFD_ATTR   _IO('S',63)
#define I_RECVFD_ATTR   _IO('S',64)
#define I_GETMSG_ATTR   _IO('S',65)
#define I_PUTMSG_ATTR   _IO('S',66)
#endif  /* SEC_BASE */
#endif	/* _SYS_IOCTL_H_ */
