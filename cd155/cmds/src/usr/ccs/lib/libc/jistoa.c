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
static char rcsid[] = "@(#)$RCSfile: jistoa.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:37 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library Conversion Functions
 *
 * FUNCTIONS: jistoa
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
/* jistoa.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:25:27 */
/**********************************************************************/
/*								      */
/* SYNOPSIS							      */
/*	int							      */
/*	jistoa (c)						      */
/*	register c;						      */
/*								      */
/* DESCRIPTION							      */
/*	jistoa returns the ASCII equivalent of a Shift-JIS character. */
/*	The function uses the _jistoa macro, which in turn does a     */
/*	lookup into the _jistoatab table.			      */
/*								      */
/* DIAGNOSTICS							      */
/*	If the input character does not have an ASCII equivalent,     */
/*	the function returns the input value.			      */
/*								      */
/**********************************************************************/
#ifdef KJI
#include <NLctype.h>

int
jistoa (c)
register c;
{
	int b3, b4;		/* 3rd and 4th bytes of c */

	if (c > 0x829a)
		return (c);
	if ((b3 = c >> 8) != 0x81 && b3 != 0x82)
		/* c has no ASCII equivalent--first byte is not 0x81 or 0x82 */
		return (c);
	if ((b4 = c & 0xff) < 0x40 || b4 > 0x9a)
		/* c has no ASCII equivalent--second byte is not valid */
		return (c);
	return (_jistoa (c));
}
#endif  /* KJI */
