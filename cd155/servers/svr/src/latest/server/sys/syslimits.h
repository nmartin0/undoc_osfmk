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
 * $Log: syslimits.h,v $
 * Revision 1.4  1994/11/18  20:41:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:28:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:57  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:25  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:01  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:13:40  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:45:36  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:54:40  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:45:58  gm]
 * 
 * Revision 1.6  90/07/27  09:08:12  devrcs
 * 	Set pipe atomicity to sensible, accurate 4096.
 * 	[90/07/19  15:51:14  tmt]
 * 
 * Revision 1.5  90/03/13  20:49:55  mbrown
 * 	AIX merge first cut.
 * 	[90/02/09  18:04:45  tom]
 * 
 * Revision 1.4  90/02/16  16:52:42  devrcs
 * 	Changed definition of CHILD_MAX in conjuntion with changes in param.h.
 * 	[90/01/25  18:53:29  coren]
 * 
 * Revision 1.3  90/01/02  20:16:35  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:40:22  gm
 * 	BSD4.4 changes.
 * 	[89/12/25            gm]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)syslimits.h	7.2 (Berkeley) 2/16/89
 */

#ifndef	_SYS_SYSLIMITS_H_
#define _SYS_SYSLIMITS_H_

#define	ARG_MAX		20480	/* max bytes for an exec function */
#if	defined(multimax) || defined(balance)
#define CHILD_MAX	100     /* max processes per user */
#else	defined(multimax) || defined(balance)
#define CHILD_MAX	64	/* max processes per user */
#endif	defined(multimax) || defined(balance)
#define	LINK_MAX	32767	/* max file link count */
#define	MAX_CANON	255	/* max bytes in terminal canonical input line */
#define	MAX_INPUT	255	/* max bytes in terminal input */
#define	NAME_MAX	255	/* max number of bytes in a file name */
#define	NGROUPS_MAX	16	/* max number of supplemental group id's */
#define	OPEN_MAX	64	/* max open files per process */
#define	PATH_MAX	1024	/* max number of bytes in pathname */
#define	PIPE_BUF	4096	/* max number of bytes for atomic pipe writes */

#endif	/* _SYS_SYSLIMITS_H_ */
