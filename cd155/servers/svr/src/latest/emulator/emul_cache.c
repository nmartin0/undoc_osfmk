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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: emul_cache.c,v $
 * Revision 1.5  1994/11/18  20:22:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:30:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:22:09  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:14:37  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:19  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:08:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:15:32  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 4.1  1992/11/03  23:57:05  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/30  16:39:59  rabii
 * 	Initial V2 Checkin
 *
 * Revision 3.0  91/01/17  12:05:05  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  89/10/17  11:23:57  rwd
 * 	Removed reference to ERROR macro.
 * 	[89/09/26            rwd]
 * 
 */
/*
 * Routines which can cache values for performance.
 */

#ifdef ECACHE
#include <uxkern/bsd_msg.h>

#define ROUTINE(routine) int routine(serv_port, syscode, argp, rvalp)\
	int serv_port;\
	int syscode;\
	int *argp;\
	int *rvalp;\
{\
	register int rv=0;\
	if (emul_cache.routine[0] == 0) {\
	    rv = emul_generic(serv_port, syscode, argp, rvalp);\
	    if (rv) return (rv);\
	    emul_cache.routine[0] = rvalp[0];\
	    emul_cache.routine[1] = rvalp[1];\
	} else {\
	    rvalp[0] = emul_cache.routine[0];\
	    rvalp[1] = emul_cache.routine[1];\
	}\
	return (rv);\
}

struct emul_cache_struct {
	int e_getpid[2];
	int e_getdtablesize[2];
	int e_getpagesize[2];
} emul_cache;

emul_cache_init()
{
	bzero(&emul_cache,sizeof(struct emul_cache_struct));
}

ROUTINE(e_getpid)
ROUTINE(e_getdtablesize)
ROUTINE(e_getpagesize)
#endif ECACHE
