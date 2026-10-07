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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: tojlower.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:12 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library Conversion Functions
 *
 * FUNCTIONS: tojlower
 *
 * ORIGINS: 10
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */                                                                   
/* tojlower.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:27:23 */
/**********************************************************************/
/*								      */
/* SYNOPSIS							      */
/*	int							      */
/*	tojlower (c)						      */
/*	register c;						      */
/*								      */
/* DESCRIPTION							      */
/*	tojlower returns the Shift-JIS lowercase equivalent of a      */
/*	Shift-JIS uppercase letter.  The input should be the	      */
/*	Shift-JIS form of an uppercase ASCII letter.		      */
/*								      */
/* DIAGNOSTICS							      */
/*	If the input character is not the Shift-JIS form of an	      */
/*	uppercase ASCII letter, the function returns the input value. */
/*								      */
/**********************************************************************/
#ifdef KJI
#include <NLctype.h>

int
tojlower (c)
register c;
{
	if (c < 0x8260 || c > 0x8279)
		/* c is outside the range [A-Z] & has no lowercase equivalent */
		return (c);
	return (_tojlower (c));
}
#endif  /* KJI */
