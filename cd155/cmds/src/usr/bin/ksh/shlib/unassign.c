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
static char sccsid[] = "@(#)unassign.c	1.7  com/cmd/ksh/shlib,3.1,9013 11/3/89 15:19:19";
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
 *   UNASSIGN.C
 *
 *   Programmer:  D. G. Korn
 *
 *        Owner:  D. A. Lambeth
 *
 *         Date:  April 17, 1980
 *
 *
 *   UNASSIGN (NP)
 *        
 *        Nullify the value and the attributes of the namnod
 *        given by NP.
 *
 *   See Also:  nam_putval(III), nam_longput(III), nam_strval(III)
 */

#include	"name.h"


/*
 *   NAM_FREE (NP)
 *
 *       struct namnod *NP;
 * 
 *   Set the value of NP to NULL, and nullify any attributes
 *   that NP may have had.  Free any freeable space occupied
 *   by the value of NP.  If NP denotes an array member, it
 *   will retain its attributes.  Any node that has the
 *   indirect (N_INDIRECT) attribute will retain that attribute.
 */

extern void	free();

void	nam_free(struct namnod *np)
{
	register union Namval *up = &np->value.namval;
	register struct Nodval *nv = 0;
	int next;
#ifdef NAME_SCOPE
	if (nam_istype (np, N_CWRITE))
	{
		np->value.namflg |= N_AVAIL;
		return;
	}
#endif /* NAME_SCOPE */
	do
	{
		if (nam_istype (np, N_ARRAY))
		{
			if((nv=array_find(np,A_DELETE))==0)
				return;
			up = &nv->namval;
		}
		if (nam_istype (np, N_INDIRECT))
			up = up->up;
		if ((!nam_istype (np, N_FREE)) && (!isnull (np)))
			free(up->cp);
		up->cp = NULL;
		if (nam_istype (np, N_ARRAY))
			next = array_next(np);
		else
		{
			np->value.namflg &= N_INDIRECT;
			np->value.namsz = 0;
			next = 0;
		}
		if(nv)
			free((char*)nv);
	}
	while(next);
}
