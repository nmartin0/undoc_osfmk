/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
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
