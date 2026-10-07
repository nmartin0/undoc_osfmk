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
 * $Log: ttychars.h,v $
 * Revision 1.4  1994/11/18  20:41:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:29:45  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:37  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:15:14  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:46:32  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:55:40  devrcs
 * 	changed USE_OLD_TTY to _USE_OLD_TTY, USE_NEW_TTY to _USE_NEW_TTY
 * 	[90/10/04  09:39:25  rabin]
 * 
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:09:10  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:47:33  gm]
 * 
 * 	Posix tty support.
 * 	[90/02/06  13:32:25  morris]
 * 
 * Revision 1.3  90/02/16  16:52:52  devrcs
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:17:37  ers]
 * 
 * Revision 1.2  90/01/02  20:16:09  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:42  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.3  89/03/09  22:09:08  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:57:29  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 0.0  86/02/18            bolosky
 * 	Added different definition of CERASE for romp.
 * 	[86/02/18            bolosky]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ttychars.h	7.3 (Berkeley) 10/18/88
 */

#ifndef	_SYS_TTYCHARS_H_
#define _SYS_TTYCHARS_H_

/*
 * 4.3 COMPATIBILITY FILE
 *
 * User visible structures and constants
 * related to terminal handling.
 */

struct ttychars {
	char	tc_erase;	/* erase last character */
	char	tc_kill;	/* erase entire line */
	char	tc_intrc;	/* interrupt */
	char	tc_quitc;	/* quit */
	char	tc_startc;	/* start output */
	char	tc_stopc;	/* stop output */
	char	tc_eofc;	/* end-of-file */
	char	tc_brkc;	/* input delimiter (like nl) */
	char	tc_suspc;	/* stop process signal */
	char	tc_dsuspc;	/* delayed stop process signal */
	char	tc_rprntc;	/* reprint line */
	char	tc_flushc;	/* flush output (toggles) */
	char	tc_werasc;	/* word erase */
	char	tc_lnextc;	/* literal next character */
};

#ifdef _USE_OLD_TTY
#include <sys/ttydefaults.h>	/* to pick up character defaults */
#endif

#endif /* _SYS_TTYCHARS_H_ */
