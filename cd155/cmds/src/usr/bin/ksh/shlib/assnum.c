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
static char sccsid[] = "@(#)assnum.c	1.7  com/cmd/ksh/shlib,3.1,9013 11/3/89 16:17:57";
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *   ASSNUM.C
 *
 *   Programmer:  D. G. Korn
 *
 *        Owner:  D. A. Lambeth
 *
 *         Date:  April 17, 1980
 *
 *
 *   NAM_LONGPUT (NP, NUM)
 *
 *        Assign the long integer NUM to NP.  NP should have
 *        the L_FLAG and N_INTGER attributes.
 *
 *
 *   See Also:  nam_putval(III), nam_free(III), nam_strval(III), ltos(III)
 */

#include	"name.h"

#ifdef FLOAT
#   define ltos 	etos
#endif /* FLOAT */
extern char *ltos();

/*
 *   ASSLONG (NP, NUM)
 *
 *        struct namnod *NP;
 *
 *        int NUM;
 *
 *   Assign the value NUM to the namnod given by NP.  All 
 *   appropriate conversions are made.
 */

void nam_longput(
struct namnod *np,
#ifdef FLOAT
   double num)
#else
   long num)
#endif /* FLOAT */
{
	register union Namval *up = &np->value.namval;
	if (nam_istype (np, N_INTGER))
	{
		if (nam_istype (np, N_ARRAY))
			up = &(array_find(np,A_ASSIGN)->namval);
#ifdef NAME_SCOPE
		if (nam_istype (np, N_CWRITE))
			np = nam_copy(np,1);
#endif	/* NAME_SCOPE */
        	if (nam_istype (np, N_INDIRECT))
			up = up->up;
        	if (nam_istype (np, N_BLTNOD))
			(*up->fp->f_ap)((unsigned)num);
#ifdef FLOAT
		else if (nam_istype (np, N_DOUBLE))
		{
			if(up->dp==0)
				up->dp = new_of(double,0);
			*(up->dp) = num;
		}
#endif /* FLOAT */
		else
		{
			if(up->lp==0)
				up->lp = new_of(long,0);
			*(up->lp) = num;
			if(np->value.namsz == 0)
				np->value.namsz = sh_lastbase;
		}
	}
	else
		nam_putval(np,ltos(num,10));
}
