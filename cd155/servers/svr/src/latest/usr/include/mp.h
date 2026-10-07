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
 * $Log: mp.h,v $
 * Revision 1.3  1994/11/18  21:10:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:56  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:27  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:59  nandy
 * Initial revision
 *
 * Revision 2.6  92/04/13  14:37:42  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.6  90/10/07  20:32:00  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:52:11  gm]
 * 
 * Revision 2.5  90/04/27  23:07:02  devrcs
 * 	Changed free argument cast to (void *).
 * 	[90/04/16  13:48:41  tom]
 * 
 * Revision 2.4  90/03/13  21:22:57  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  17:51:35  tom]
 * 
 * Revision 2.3  90/01/02  19:15:20  gm
 * 	     Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  10:10:35  gm
 * 	     Changed vax test to BYTE_ORDER test.
 * 	     [87/11/04            gm0w]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *      @(#)mp.h        5.1 (Berkeley) 5/30/85
 */


#ifndef _MP_H_
#define _MP_H_

#ifndef BYTE_ORDER
#include <machine/endian.h>
#endif

#define MINT struct mint
MINT
{	int len;
	short *val;
};

#define FREE(x) {if(x.len!=0) {free((void *)x.val); x.len=0;}}

#ifndef DBG
#define shfree(u) free((void *)u)
#else
#include <stdio.h>
#define shfree(u) { if(dbg) fprintf(stderr, "free %o\n", u); free((void *)u);}
extern int dbg;
#endif /* DBG */

#if     BYTE_ORDER == BIG_ENDIAN
struct half
{       short high;
        short low;
};
#else
struct half
{       short low;
        short high;
};
#endif /* BYTE_ORDER */

extern MINT *itom();
extern short *xalloc();

#ifdef lint
extern xv_oid;
#define VOID xv_oid =
#else
#define VOID
#endif

#endif /* _MP_H_ */
