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
static char rcsid[] = "@(#)$RCSfile: ujtojis.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:21 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library Conversion Functions
 *
 * FUNCTIONS: utojis
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
/* ujtojis.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:28:00 */
/************************************************************************/
/*									*/
/*  SYNOPSIS								*/
/*	unsigned char *							*/
/*	ujtojis(s1, s2)							*/
/*	unsigned char *s1, *s2;						*/
/*									*/
/*  DESCRIPTION								*/
/*	UNIX-jis to jis string conversion routine. The input string    	*/
/*	s2, containing double-byte UNIX-jis characters, is converted 	*/
/*	to a string of double-byte jis characters, s1. Returns s1.	*/
/*									*/
/*  DIAGNOSTICS								*/
/*	No check for valid input is performed. Invalid input will	*/
/*	result in undefined output.					*/
/*									*/
/************************************************************************/
#ifdef KJI
unsigned char *
ujtojis(s1, s2)
unsigned char *s1, *s2;
{
	unsigned char *s0 = s1;

	while (*s2)
		*s1++ = *s2++ & 0x7f;		/* clear MSB */
        *s1 = '\0';
	return s0;
}
#endif  /* KJI */
