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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nx/gops.h,v 1.3 1994/11/19 02:44:41 mtm Exp $
 *
 * This module contains the application interface to the global ops.
 * 
 */

/* $Id: gops.h,v 1.3 1994/11/19 02:44:41 mtm Exp $ */
/* Copyright info et.al. -- see file "gops.c" */

/* Prefix for user-visible global ops:
 *
 *   The following prefix is useful for comparing test and production
 *   versions of the global operations.  When "gops.c" is compiled,
 *   all of its user-visible global ops carry the USERPREFIX instead
 *   of the standard 'g'.  If USERPREFIX is set to something other
 *   than 'g', this generates a set of new names such that both sets
 *   of routines can be exercised by a single test program.  For
 *   generating a standard library, USERPREFIX must be set as
 *   follows:
 *
 *       #define USERPREFIX() g
 *
 *   Note that this mechanism will fail when/if these routines -become-
 *   the standard.  In that case, there will become lots of conflicts
 *   in names other than the user-visible ones, and the conventional
 *   approach of linking against one or the other sets of routines will
 *   have to be used.
 *
 *   gops.h 2.3 92/01/20 16:05:24
 */

#define USERPREFIX() _g

/* Tuning parameters:
 *
 *   The following parameters are used to make performance estimates for
 *   selecting algorithms.  See the paper by R.J. Littlefield, "Modeling 
 *   Node Bandwidth Limits and Their Effect on Vector Combining Algorithms"
 *   for a discussion of their significance.
 *
 *   All of these parameters are subject to change in going to a new
 *   platform.  alpha and XferPerByte are particularly vulnerable.
 */
  
#define alpha (54.e-6)    /* comm startup cost, seconds */
#define XferPerByte (.192e-6)       /* seconds/byte (slow mboot) */
/* #define XferPerByte (.096e-6)       /* seconds/byte (fast mboot) */
/* #define XferPerByte (.4e-6)       /* seconds/byte (iPSC/860) */
#define c2 (.25e-6)       /* seconds/combine computation */
#define c3 (2*c2)         /* seconds/3-way-combine (naive) */
#define f1 (1)            /* by definition, 1 link active */
#define f2 (1.1)          /* 2 links nominally active */
#define f3 (1.3)          /* 3 links nominally active */
#define f4 (3.9)          /* 4 links nominally active */
#define f6 (5.1)          /* 6 links nominally active */
#define GOPFB_PACKETSIZE (480)         /* bytes per packet */
#define MAX_GOPFB_BLOCKSIZE (6*480)    /* maximum without paying for ack */

#define GOP_ALG_THRESHOLD 3000  /* data volumes smaller than this use
                                   the tree algorithm, no questions asked. */

/* Message types:
 *
 *   BASETYPE should be redefined to put all these message types into
 *   the "system type" range. 
 *   
 *
 */

/* The Reserved System Message Types Bases */
#define	MSG_BASE_TYPE	1000000000
#define	FORCE_BASE_TYPE	2000000000
#define	COMM_BASE_TYPE	(MSG_BASE_TYPE + 4000)

#define BASETYPE   COMM_BASE_TYPE

#define	COMM_TYPE_1	(COMM_BASE_TYPE   +  1)
#define	COMM_TYPE_2	(COMM_BASE_TYPE   +  2)
#define	COMM_TYPE_3	(COMM_BASE_TYPE   +  3)

#define DOWNTYPE   (BASETYPE+1)
#define LEFTTYPE   (BASETYPE+2)
#define RIGHTTYPE  (BASETYPE+3)
#define UPTYPE     (BASETYPE+4)

/* The tree algorithm uses INTYPE+node to separate messages from various */
/* processors to ensure determinacy.  A gap of maxprocs should be left */
/* between INTYPE and OUTTYPE */

#define INTYPE     (BASETYPE+1)
#define OUTTYPE    (BASETYPE+800)

/* The sync types must be distinct from all of the global combining
   types.
*/

#define SYNC_TYPE_1 (BASETYPE+1000)
#define SYNC_TYPE_2 (BASETYPE+1001)

/* The gather tree algorithm uses GATHER_1+node to separate messages */
/* from various processors to ensure determinacy.  A gap of maxnodes should  */
/* be left between GATHER_1 and GATHER_2 		     */

#define GATHER_1 (BASETYPE+2000) 
#define GATHER_2 (BASETYPE+10192) 

#define NLIST_SIZE 16 	/* log2(numnodes) large enough for a 64K node system */
#define MAXNODES 8192   /* maximum nodes sending a GATHER msg */

/* Combining operations */

#define cop_land(a,b)	a = a && b
#define cop_and(a,b)    a &= b
#define cop_high(a,b)   if (b>a) a = b
#define cop_low(a,b)    if (b<a) a = b
#define cop_or(a,b)     a |= b
#define cop_lor(a,b)	a = a || b
#define cop_prod(a,b)   a *= b
#define cop_sum(a,b)    a += b
#define cop_xor(a,b)    a ^= b

/* Token pasting: macros 'tpaste{2,3}' concatenate tokens.  There is
   apparently no truly portable way to do this, so the construct
   must be defined appropriately for each compiler.
*/

#ifdef sun
#define tpaste2(a,b)   a/**/b
#define tpaste3(a,b,c) a/**/b/**/c
#else
#define tpaste2(a,b)   a ## b
#define tpaste3(a,b,c) a ## b ## c
#endif

#ifndef FORTRANTESTING    /* to allow inclusion in a Fortran code */

/* Common variables: */

long gopfb_pxlen;     /* length of piece currently being combined */
int gopfb_algused;    /* saved as code for which algorithm was used:
                           0=tree, 1=fence, 2=snake */
int gopfb_sopt;       /* saved as the block length that was used
                           (for fence and snake) */
double gopfb_treecost;  /* saved as estimated cost for tree algorithm */
double gopfb_fencecost; /* same, for fence */
double gopfb_snakecost; /* same, for snake */

#endif
