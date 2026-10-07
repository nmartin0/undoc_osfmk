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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.3
 */
/* @(#)$RCSfile: print.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:23 $ */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)print.h	1.7  com/lib/c/prnt,3.1,8943 9/7/89 10:15:49 */
/*
 * COMPONENT_NAME: (LIBCPRNT) Standard C Library Print Functions 
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 3, 27 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/* Maximum number of digits in any integer representation */
#define MAXDIGS 11

/* Maximum total number of digits in E format */
#define MAXECVT 17

/* Maximum number of digits after decimal point in F format */
#define MAXFCVT 60

/* Maximum significant figures in a floating-point number */
#define MAXFSIG 17

/* This needs to be removed */
/* Largest (normal length) positive integer */
#ifndef MAXINT
#define	MAXINT	2147483647
#endif

/* Maximum number of characters in an exponent */
#define MAXESIZ 5

/* Maximum (positive) exponent */
#define MAXEXP 310

/* Data type for flags */
typedef char bool;

/* Convert a digit character to the corresponding number */
#define tonumber(x) ((x)-'0')

/* Convert a number between 0 and 9 to the corresponding digit */
#define todigit(x) ((x)+'0')

/* Max and Min macros */
#define max(a,b) ((a) > (b)? (a): (b))
#define min(a,b) ((a) < (b)? (a): (b))

/* duplicate of code in stdiom.h */
#define _WRTCHK(iop)	((((iop->_flag & (_IOWRT | _IOEOF)) != _IOWRT) \
				|| (iop->_base == NULL)  \
				|| (iop->_ptr == iop->_base && iop->_cnt == 0 \
					&& !(iop->_flag & (_IONBF | _IOLBF)))) \
			? _wrtchk(iop) : 0 )

/* The infamous NDIG for cvt buffers */
#define	NDIG	(max(MAXDIGS, 1+max(MAXFCVT+MAXEXP, MAXECVT)))
