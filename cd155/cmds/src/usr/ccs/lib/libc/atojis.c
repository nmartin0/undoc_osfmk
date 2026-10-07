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
static char rcsid[] = "@(#)$RCSfile: atojis.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:32 $";
#endif
/*
 *  COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library Conversion Functions
 *
 * FUNCTIONS: atojis
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
/* atojis.c	1.2  com/lib/c/gen/KJI,3.1,9021 1/30/90 14:38:33 */

/**********************************************************************/
/*								      */
/* SYNOPSIS							      */
/*	int							      */
/*	atojis (c)						      */
/*	register c;						      */
/*								      */
/* DESCRIPTION							      */
/*	atojis returns the Shift-JIS equivalent of an ASCII character.*/
/*	The function uses the _atojis macro, which in turn does a     */
/*	lookup into the _atojistab table.			      */
/*								      */
/* DIAGNOSTICS							      */
/*	If the input character does not have a Shift-JIS equivalent,  */
/*	the function returns the input value.			      */
/*								      */
/**********************************************************************/

#ifdef KJI
#include <NLctype.h>

int
atojis (c)
register c;
{
	if (c < 0x20 || c > 0x7e)
		/* c has no Shift-JIS equivalent */
		return (c);
	return (_atojis (c));
}
#endif  /* KJI */
