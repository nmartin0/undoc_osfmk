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
static char rcsid[] = "@(#)$RCSfile: tojkata.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:11 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library Conversion Functions
 *
 * FUNCTIONS: to_jkata, _tojkata
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
/* tojkata.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:27:12 */
/************************************************************************/
/*									*/
/*  SYNOPSIS								*/
/*	int								*/
/*	tojkata(c)							*/
/*	register c;							*/
/*									*/
/*	int								*/
/*	_tojkata(c)							*/
/*	int c;								*/
/*									*/
/*  DESCRIPTION								*/
/*	Returns the katakana equivalent of a hiragana character. The	*/
/*	unchecked form, _tojkata, does not check for a valid hiragana	*/
/*	character. 							*/
/*									*/
/*  DIAGNOSTICS								*/
/*	For the checked form tojkata, the input value will be returned	*/
/*	unchanged if the conversion cannot be made.			*/
/*									*/
/************************************************************************/
#ifdef KJI
#include <NLctype.h>

int
tojkata(c)	/* checked form */
register c;
{
	int _tojkata();

	if (isjhira(c))
		return _tojkata(c);
	else
		return c;
}

int
_tojkata(c)	/* unchecked form */
int c;
{
	register unsigned char c0, c1;

	/* get lsb */
	c1 = _NCbot(c);

	/* converted msb is always 0x83 */
	c0 = 0x83;
	
	/* convert lsb */
	c1 = c1 - 0x5f;

	/* correct for 0x7f gap */
	if (c1 >= 0x7f)
		c1++;

	return _NCd2(c0, c1);
}
#endif  /* KJI */
