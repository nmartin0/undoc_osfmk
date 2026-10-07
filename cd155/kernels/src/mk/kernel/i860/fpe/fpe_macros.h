/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*      Copyright (c) 1989,1990 Intel Corporation.         */
/*      All rights reserved.                               */
/*                                                         */
/*        INTEL CORPORATION PROPRIETARY INFORMATION        */
/*                                                         */
/* This software is supplied under the terms of a license  */
/* agreement or nondisclosure agreement with Intel Corp.   */
/* and may not be copied or disclosed except in accordance */
/* with the terms of that agreement.                       */

/*
 * $Id: fpe_macros.h,v 1.4 1994/11/18 20:40:24 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_macros.h,v $
 * Revision 1.4  1994/11/18  20:40:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:17  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_macros.h     1.1"

/* Single precision macros, d and s are pointers to Ulong */
#define spchgsign(s)	( (s)[0] ^= 0x80000000 )
#define spsign(s)	( (s)[0] &  0x80000000 )
#define spexponent(s)	(((s)[0] &  0x7f800000 ) >> 23)

/* Double precision macros, d and s are pointers to Ulong  */
#define dpchgsign(s)	( (s)[1] ^= 0x80000000 )
#define dpsign(s)	( (s)[1] &  0x80000000 )
#define dpexponent(s)	(((s)[1] &  0x7ff00000 ) >> 20)

/* General purpose macros, p is presicion, d and s are pointers to Ulong */
#define fpassign(p,d,s)	(p? dpassign(d, s) : spassign(d, s))
#define fpchgsign(p,s)	(p? dpchgsign(s) : spchgsign(s))
#define fpsign(p,s)	(p? dpsign(s) : spsign(s))
