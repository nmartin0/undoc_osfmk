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
static char rcsid[] = "@(#)$RCSfile: kutentojis.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:42 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library Conversion Functions
 *
 * FUNCTIONS: kutentojis
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
/* kutentojis.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:25:51 */
/************************************************************************/
/*									*/
/*  SYNOPSIS								*/
/*	int								*/
/*	kutentojis(c)							*/
/*	int c;								*/
/*									*/
/*  DESCRIPTION								*/
/*	Convert a kuten code to corresponding jis code. Kuten code 	*/
/*	range is 0 < code < 121.					*/
/*									*/
/*  DIAGNOSTICS								*/
/*	No check for valid input is performed. Returns 0 for invalid	*/
/*	kuten codes.							*/
/*									*/
/************************************************************************/
#ifdef KJI
kutentojis(c)
int c;
{
	int	ku;			/* number of 'ku' */

	if (0 < (ku = c / 100) && ku < 121 && 0 < (c %= 100) && c < 95)
		c += ((ku + 0x20) << 8) + 0x20;
	else				/* invalid kuten code */
		c = 0;
	return c;
}
#endif  /* KJI */
