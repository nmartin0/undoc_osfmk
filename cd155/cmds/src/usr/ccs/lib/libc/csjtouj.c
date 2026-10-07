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
static char rcsid[] = "@(#)$RCSfile: csjtouj.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:07 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library Conversion Functions
 *
 * FUNCTIONS: csjtouj
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
/* csjtouj.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:24:09 */
/************************************************************************/
/*									*/
/*  SYNOPSIS								*/
/*	unsigned char *							*/
/*	csjtouj(s1, s2)							*/
/*	unsigned char *s1, *s2;						*/
/*									*/
/*  DESCRIPTION								*/
/*	Shift-jis to UNIX-jis conversion routine. Converts the double-	*/
/*	byte shift-jis character pointed to by s2 to a double-byte 	*/
/*	UNIX-jis character pointed to by s1. Returns s1.		*/
/*									*/
/*  DIAGNOSTICS								*/
/*	No check for valid input is performed. Invalid input will	*/
/*	result in undefined output.					*/
/*									*/
/************************************************************************/
#ifdef KJI
unsigned char *
csjtouj(s1, s2)
unsigned char *s1, *s2;
{
	int c1, c2, p;

	c1 = s2[0];
	c2 = s2[1];
	p = 0;
	if (c2 < 0x7f)
		c2 -= 0x1f;
	else if (c2 < 0x9f)
		c2 -= 0x20;
	else {
		c2 -= 0x7e;
		p++;
	}
	if (c1 < 0xa0)
		s1[0] = (((c1 - 0x81) << 1) + 0x21 + p) | 0x80;
	else
		s1[0] = (((c1 - 0xe0) << 1) + 0x5f + p) | 0x80;
	s1[1] = c2 | 0x80;
	return s1;
}
#endif  /* KJI */
