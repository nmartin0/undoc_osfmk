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
 * HISTORY
 * $Log: ttydefaults.h,v $
 * Revision 1.4  1994/11/18  20:41:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:29:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:01  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:37  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:40  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:15:21  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:46:37  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/31  14:07:18  devrcs
 * 	Added ONLCR back to the default oflags.  Made kernel defaults
 * 	a subset of application defaults.
 * 	[90/10/12  19:59:42  ers]
 * 
 * Revision 1.5  90/10/07  14:55:45  devrcs
 * 	Add a Posix cc table to pass VSX.
 * 	[90/09/28  20:17:42  brezak]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:47:40  gm]
 * 
 * Revision 1.4  90/07/17  11:42:42  devrcs
 * 	Fixed CREPRINT, which was incorrectly being defined as 023
 * 	(ctrl-S) instead of 022 (cntrl-R).
 * 	[90/07/04  14:57:22  ers]
 * 
 * Revision 1.3  90/03/13  20:50:15  mbrown
 * 	Fixed typo in previous edit.
 * 	[90/03/08  20:01:30  mbrown]
 * 
 * 	Added  define CTRL(x)		((x)&037)
 * 	[90/02/28  10:04:20  mbrown]
 * 
 * 	centralized controls, removed non-ANSI CTRL(x) macro
 * 	[90/02/25  14:46:03  mbrown]
 * 
 * 	Changed NCC to NCCS as per POSIX.
 * 	[90/02/21  15:28:42  tom]
 * 
 * Revision 1.2  90/02/16  16:52:55  devrcs
 * 	POSIX tty support.
 * 	[90/02/15  15:05:43  ers]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ttydefaults.h	7.2 (Berkeley) 11/20/89
 */

/*
 * System wide defaults for terminal state.
 * Included by sys/termios.h.
 */
#ifndef _SYS_TTYDEFAULTS_
#define	_SYS_TTYDEFAULTS_

/*
 * Defaults on "first" open.
 */
#ifdef _KERNEL
#define	TTYDEF_IFLAG	(BRKINT | ISTRIP | ICRNL | IXON)
#else
#define	TTYDEF_IFLAG	(BRKINT | ISTRIP | ICRNL | IMAXBEL | IXON | IXANY)
#endif
#define TTYDEF_OFLAG	(OPOST | ONLCR)
#ifdef _KERNEL
#define TTYDEF_LFLAG	(ECHO | ICANON | ISIG)
#else
#define TTYDEF_LFLAG	(ECHO | ICANON | ISIG | IEXTEN | ECHOE|ECHOKE|ECHOCTL)
#endif
#define TTYDEF_CFLAG	(CREAD | CS7 | PARENB | HUPCL)
#define TTYDEF_SPEED	(B9600)

/*
 * Control Character Defaults
 */
#define CTRL(x)		((x)&037)
#define CBELL		((unsigned)'\007')	/* ASCII BELL control */
#define CESC		((unsigned)'\\')	/* back-slash */
#define CFORM		((unsigned)'\014')	/* ^L */
#define CNUL		((unsigned)'\0')	/* null char */
#define CVT		((unsigned)'\013')	/* ^K */
#define CEOL		((unsigned)'\377')	/* XXX avoid _POSIX_VDISABLE */
#define CDEL		CEOL			/* delete */
#define CSTATUS		((unsigned)'\377')	/* XXX avoid _POSIX_VDISABLE */
#define CEOF		((unsigned)'\004')	/* ^D */
#define CEOT		CEOF
#define CERASE		((unsigned)'\010')	/* ^H */
#define CINTR		((unsigned)'\003')	/* ^C */
#define CKILL		((unsigned)'\025')	/* ^U */
#define CMIN		1
#define CQUIT		((unsigned)'\034')	/* ^\ */
#define CSUSP		((unsigned)'\032')	/* ^Z */
#define CTIME		0
#define CDSUSP		((unsigned)'\031')	/* ^Y */
#define CSTART		((unsigned)'\021')	/* ^Q */
#define CSTOP		((unsigned)'\023')	/* ^S */
#define CLNEXT		((unsigned)'\026')	/* ^V */
#define CDISCARD	((unsigned)'\017')	/* ^O */
#define CWERASE		((unsigned)'\027')	/* ^W */
#define CREPRINT	((unsigned)'\022')	/* ^R */
/* compat */
#define	CBRK		CEOL
#define CRPRNT		CREPRINT
#define CFLUSHO		CDISCARD
#define CFLUSH		CDISCARD

/* PROTECTED INCLUSION ENDS HERE */
#endif /* _SYS_TTYDEFAULTS_ */

/*
 * #define TTYDEFCHARS to include an array of default control characters.
 */
#ifdef TTYDEFCHARS
#ifndef _KERNEL
cc_t	ttydefchars[NCCS] = {
	CEOF,	CEOL,	CEOL,	CERASE, CWERASE, CKILL,
	CREPRINT, _POSIX_VDISABLE, CINTR, CQUIT, CSUSP,	CDSUSP,
	CSTART,	CSTOP,	CLNEXT, CDISCARD, CMIN,	CTIME,
	CSTATUS, _POSIX_VDISABLE
};
#else
cc_t	ttydefchars[NCCS] = {
	CEOF,	CEOL, _POSIX_VDISABLE, CERASE, _POSIX_VDISABLE, CKILL,
	_POSIX_VDISABLE, _POSIX_VDISABLE, CINTR, CQUIT, CSUSP, _POSIX_VDISABLE,
	CSTART, CSTOP, _POSIX_VDISABLE, _POSIX_VDISABLE, CMIN,	CTIME,
	_POSIX_VDISABLE, _POSIX_VDISABLE
};
#endif /* _KERNEL */
#undef TTYDEFCHARS
#endif /* TTYDEFCHARS */
