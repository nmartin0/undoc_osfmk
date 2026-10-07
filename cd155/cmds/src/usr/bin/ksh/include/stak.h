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
/* @(#)stak.h	1.7  com/cmd/ksh/sh,3.1,9013 11/6/89 19:37:20 */
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *	UNIX shell
 *
 *	S. R. Bourne
 *	AT&T Bell Laboratories
 *
 */
#ifdef KJI
typedef unsigned char   *STKPTR;
#else
typedef char		*STKPTR;
#endif
/*
 * To use stack as temporary workspace across
 * possible storage allocation (eg name lookup)
 * a) get ptr from `stak_offset'
 * b) can now use `stak_push'
 * c) then reset with `stak_set'
 * d) `stak_address' gives real address if needed
 */

#define		stak_offset()	(sh.staktop-sh.stakbot)
#define		stak_address(x)	(sh.stakbot+(x))
#define		stak_set(x)	(sh.staktop=sh.stakbot+(x))
#if (defined(NLS) || defined(KJI))
#	define		stak_push(c)	((c)&~0377) ? \
				 (*sh.staktop++=(c)>>8,*sh.staktop++=(c)&0xff,0) \
				 : (*sh.staktop++=(c),0)
#else
#	define		stak_push(c)	(*sh.staktop++=(c))
#endif
#define		stak_zero()	(*sh.staktop=0)

/*
 * Used to address current word on the top of
 * the stack (very temporary)
 */
#define 	stak_word()	(sh.stakbot)

/*
 * for local use only since it hands
 * out a real address for the stack top
 */
#ifdef _NO_PROTO
    extern STKPTR		stak_begin();
#else /* ~ _NO_PROTO */
    extern STKPTR		stak_begin(void);
#endif /* _NO_PROTO */

/*
 * Will allocate the item being used and return its
 * address (safe now).
 */
#define		stak_fix()	stak_end(sh.staktop)

/*
 * For use after `stak_begin' to hand back
 * new stack top and then allocate item
 */
#ifdef _NO_PROTO
    extern STKPTR		stak_end();
#else /* ~ _NO_PROTO */
    extern STKPTR		stak_end(char*);
#endif /* _NO_PROTO */

/*
 * Copy a string onto the stack and
 * allocate the space.
 */
#ifdef _NO_PROTO
    extern STKPTR		stak_copy();
#else /* ~ _NO_PROTO */
    extern STKPTR		stak_copy(const char*);
#endif /* _NO_PROTO */

/* Allocate given amount of stack space */
#ifdef _NO_PROTO
    extern STKPTR		stak_alloc();
#else /* ~ _NO_PROTO */
    extern STKPTR		stak_alloc(unsigned);
#endif /* _NO_PROTO */

/* Bring the stack back to a given address */
#ifdef _NO_PROTO
    extern void 	stak_reset();
#else /* ~ _NO_PROTO */
    extern void 	stak_reset(STKPTR);
#endif /* _NO_PROTO */

/* Check to see if stack can be shrunk */
#ifdef _NO_PROTO
    extern int		stak_check();
#else /* ~ _NO_PROTO */
    extern int		stak_check(void);
#endif /* _NO_PROTO */

