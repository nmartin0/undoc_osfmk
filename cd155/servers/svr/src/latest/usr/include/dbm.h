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
 * $Log: dbm.h,v $
 * Revision 1.3  1994/11/18  21:09:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:15  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:56  nandy
 * Initial revision
 *
 * Revision 2.5  92/04/13  14:34:57  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.5  90/10/07  20:26:42  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:44:19  gm]
 * 
 * Revision 2.4  90/03/13  21:21:57  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  17:49:26  tom]
 * 
 * Revision 2.3  90/01/02  19:15:11  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  10:09:50  gm
 * 	Added function prototype declarations.
 * 	[89/06/30  11:55:50  bww]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)dbm.h	5.1 (Berkeley) 3/27/86
 */

#ifndef _DBM_H_
#define _DBM_H_

#ifndef NULL
/*
 * this is lunacy, we no longer use it (and never should have
 * unconditionally defined it), but, this whole file is for
 * backwards compatability - someone may rely on this.
 */
#define	NULL	((char *) 0)
#endif

#include <ndbm.h>

#if _NO_PROTO
datum	fetch();
datum	firstkey();
datum	nextkey();
#else
extern datum	fetch(datum);
extern datum	firstkey(void);
extern datum	nextkey(datum);
#endif	/* _NO_PROTO */

#if 0
datum	makdatum();
datum	firsthash();
long	calchash();
long	hashinc();
#endif

#endif /* _DBM_H_ */
