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
 * HISTORY
 * $Log: tty_conf.c,v $
 * Revision 1.5  1994/11/18  20:28:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:49:59  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:50:38  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:06:52  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:35  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:23:47  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/05  13:56:07  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  13:19:59  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:52:02  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:01:33  gm]
 * 
 * 	Remove defunct fe.h include.
 * 	[90/09/28  15:00:46  tmt]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:34:37  jeffc]
 * 
 * Revision 1.4  90/08/09  13:15:27  devrcs
 * 	 Added support for Kanji Shift JIS discipline.
 * 	[90/07/26  13:29:04  morris]
 * 
 * 	Posix tty support.
 * 	[90/02/06  15:14:59  morris]
 * 
 * Revision 1.3  90/02/16  16:49:01  devrcs
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  13:57:21  ers]
 * 
 * Revision 1.2  90/01/02  20:00:42  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:27:44  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  15:35:37  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.6  89/07/11  13:13:13  jjc
 * 	Changed "#ifdef sun3" into "if defined(sun3) || defined(sun4)".
 * 	[89/05/06            jjc]
 * 
 * Revision 2.5  89/03/09  19:32:03  rpd
 * 	More cleanup.
 * 
 * Revision 2.4  89/02/26  11:27:27  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.3  89/02/09  04:33:41  mwyoung
 * 	Code cleanup cataclysm.
 * 
 * Revision 2.2  88/11/14  14:31:46  gm0w
 * 	Added CSNET DialupIP support.
 * 	[88/11/14            gm0w]
 * 
 * Revision 0.0  87/04/09            avie
 * 	Handle ROMP_TB.
 * 	[87/04/09            avie]
 * 
 * Revision 0.0  86/12/24            bolosky
 * 	romp: Added definitions for the l_select field used by X on the
 * 	RT.  Also removed now defunct aed_tty entries (on line 7).
 * 	[86/12/24            bolosky]
 * 
 * Revision 0.0  86/08/05            jjc
 * 	Added support for Sun mouse and keyboard.
 * 	[86/08/05            jjc]
 * 
 * Revision 0.0  86/02/16            bolosky
 * 	Added support for Sailboat MS and AED devices under switch romp.
 * 	[86/02/16            bolosky]
 * 
 * Revision 0.0  86/01/25            avie
 * 	Upgraded to 4.3.
 * 	[86/01/25            avie]
 * 
 * Revision 0.0  85/07/30            mja
 * 	NFE:  Added Front End line discipline.
 * 	[V1(1)]
 * 	[85/07/30            mja]
 * 
 * $EndLog$
 */


/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)tty_conf.c	7.4 (Berkeley) 5/1/89
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/conf.h>

int	nodev();
int	nulldev();

int	ttyopen(),ttylclose(),ttread(),ttwrite(),nullioctl(),ttstart();
int	ttymodem(), nullmodem(), ttyinput();

#ifdef	ibmrt
int	ttselect();
#endif	ibmrt

int	ottyopen(), ottylclose(), ottread(), ottwrite();
int	ottyinput(), ottstart(), ottymodem();

#ifdef	ibmrt
#include <romp_tb.h>
#undef	NTB
#define NTB	NROMP_TB
#else	ibmrt
#include <tb.h>
#endif	ibmrt
#if	NTB > 0 || NROMP_TB > 0
int	tbopen(),tbclose(),tbread(),tbinput(),tbioctl(),tbselect();
#endif
#ifdef	ibmrt
/* This is a kludge; supposed to be machine independent -- sac */
#include <ms.h>
#if	NMS > 0
int	msdopen(),msdclose(),msdread(),msdinput(),msdioctl(),msdselect();
#endif	NMS
#endif	ibmrt

#if	defined(sun3) || defined(sun4)
#include <ms.h>
#if	NMS > 0
int     msopen(), msclose(), msread(), msioctl(), msinput();
#endif

#include <kb.h>
#if	NKB > 0
int     kbdopen(), kbdclose(), kbdread(), kbdioctl(), kbdinput();
#endif
#endif	defined(sun3) || defined(sun4)

#include <sl.h>
#if	NSL > 0
int	slopen(),slclose(),slinput(),sltioctl(),slstart();
#endif

#ifdef	vax
#include <du.h>
#if	NDU > 0
int	dutopen(),dutclose(),dutinput(),dutioctl(),dutstart(),dutmodem();
#endif
#endif

#include <kji.h>
#if NKJI > 0
int	kji_ttread(), kji_ttyinput(), kji_ttwrite();
#endif

struct	linesw linesw[] =
{
	ttyopen, ttylclose, ttread, ttwrite, nullioctl,
	ttyinput, nodev, nulldev, ttstart, ttymodem,	/* 0- termios */
#ifdef	ibmrt
	ttselect,
#endif	ibmrt
	nodev, nodev, nodev, nodev, nodev,		/* 1- defunct */
	nodev, nodev, nodev, nodev, nodev,
#ifdef	ibmrt
	nodev,
#endif	ibmrt
	nodev, nodev, nodev, nodev, nodev,		/* 2- defunct */
	nodev, nodev, nodev, nodev, nodev,
#ifdef	ibmrt
	nodev,
#endif	ibmrt
#if	NTB > 0
	tbopen, tbclose, tbread, nodev, tbioctl,
	tbinput, nodev, nulldev, ttstart, nullmodem,	/* 3- TABLDISC */
#ifdef	ibmrt
	tbselect,
#endif	ibmrt
#else
	nodev, nodev, nodev, nodev, nodev,
	nodev, nodev, nodev, nodev, nodev,
#ifdef	ibmrt
	nodev,
#endif	ibmrt
#endif
#if	NSL > 0
	slopen, slclose, nodev, nodev, sltioctl,
	slinput, nodev, nulldev, slstart, nullmodem,	/* 4- SLIPDISC */
#ifdef	ibmrt
	nodev,
#endif	ibmrt
#else
	nodev, nodev, nodev, nodev, nodev,
	nodev, nodev, nodev, nodev, nodev,
#ifdef	ibmrt
	nodev,
#endif	ibmrt
#endif
#if	defined(ibmrt) && NMS > 0
	msdopen, msdclose, msdread, nodev, msdioctl,
	msdinput, nodev, nulldev, ttstart, nulldev,	/* 5- MOUSELDISC */
	msdselect,
#else	defined(ibmrt) && NMS > 0
#if	(defined(sun3) || defined(sun4)) && NMS > 0
	msopen, msclose, msread, nodev, msioctl,
	msinput, nodev, nulldev, nulldev, nulldev,
#else	(defined(sun3) || defined(sun4)) && NMS > 0
	nodev, nodev, nodev, nodev, nodev,
	nodev, nodev, nodev, nodev, nodev,
#ifdef	ibmrt
	nodev,
#endif	ibmrt
#endif	(defined(sun3) || defined(sun4)) && NMS > 0
#endif	defined(ibmrt) && NMS > 0
#if	(defined(sun3) || defined(sun4)) && NKB > 0
        kbdopen, kbdclose, kbdread, ttwrite, kbdioctl,
	kbdinput, nodev, nulldev, ttstart, nulldev,	/* 6- KDBLDISC */
#else	(defined(sun3) || defined(sun4)) && NKB > 0
	nodev, nodev, nodev, nodev, nodev,
	nodev, nodev, nodev, nodev, nodev,
#ifdef	ibmrt
	nodev,
#endif	ibmrt
#endif	(defined(sun3) || defined(sun4)) && NKB > 0
#if	NDU > 0
	dutopen, dutclose, nodev, nodev, dutioctl,
	dutinput, nodev, nulldev, dutstart, dutmodem,	/* 7- DUDISC */
#ifdef	ibmrt
	nodev,
#endif	ibmrt
#else
	nodev, nodev, nodev, nodev, nodev,
	nodev, nodev, nodev, nodev, nodev,
#ifdef	ibmrt
	nodev,
#endif	ibmrt
#endif
#if NKJI > 0		/* kanji shift JIS */
	ttyopen, ttylclose, kji_ttread, kji_ttwrite, nullioctl,
	kji_ttyinput, nodev, nulldev, ttstart, ttymodem, /* 8- KJIDISC */
#endif
};

int	nldisp = sizeof (linesw) / sizeof (linesw[0]);

/*
 * Do nothing specific version of line
 * discipline specific ioctl command.
 */
/*ARGSUSED*/
nullioctl(tp, cmd, data, flags)
	struct tty *tp;
	char *data;
	int flags;
{

#ifdef	lint
	tp = tp; data = data; flags = flags;
#endif
	return (-1);
}
