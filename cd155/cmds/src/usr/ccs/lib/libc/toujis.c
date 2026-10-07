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
static char rcsid[] = "@(#)$RCSfile: toujis.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:14 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library National Language Support
 *
 * FUNCTIONS: toujis
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
/* toujis.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:27:47 */
#ifdef KJI
#include "NLctype.h"
/**********************************************************************/
/*								      */
/*  NAME							      */
/*	toujis							      */
/*								      */
/* SYNOPSIS							      */
/*	int							      */
/*	toujis (c)						      */
/*	register c;						      */
/*								      */
/* DESCRIPTION							      */
/*	toujis sets all of the parameter bits that are not 16 bit     */
/*      Ujis code to zero.                                            */
/*								      */
/* DIAGNOSTICS							      */
/*	NONE				                              */
/*								      */
/**********************************************************************/

int
toujis (c)
register int c;
{
	return (c && 0xFF);
}
#endif    /* KJI  */
