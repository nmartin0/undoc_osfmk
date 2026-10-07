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
static char rcsid[] = "@(#)$RCSfile: linknod.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:28 $";
#endif
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
 *   LINKNOD.C
 *
 *   Programmer:  D. G. Korn
 *
 *        Owner:  D. A. Lambeth
 *
 *         Date:  April 17, 1980
 *
 *
 *   NAM_LINK (NODP, ROOT)
 *        
 *        Link the node given by NODP into the memory tree
 *        given by ROOT.
 *
 *   RMNVAL (NV)
 *
 *        Remove freeable space associated with the Nodval NV.
 *
 *
 *   See Also:  nam_search(III), gettree(III)
 */

#include	"name.h"

#ifndef KSHELL
void	rmnval ();
extern void	free();
#endif /* !KSHELL */


/*
 *   NAM_LINK (NODP, ROOT)
 *
 *        struct namnod *NODP;
 *
 *        struct Amemory *ROOT;
 *
 *   Link the namnod pointed to by NODP into the memory tree
 *   denoted by ROOT.  If ROOT contains another namnod with
 *   the same namid as NODP, an array is created, with the
 *   previously inserted namnod as its first element and NODP as
 *   its second.  If a previously inserted node of the same namid
 *   already denotes an array of n elements, NODP becomes the
 *   n+1st element.
 */

#ifdef KSHELL
/* save code space by not handling the case of linking array elements */
void	nam_link(struct namnod *nodp, struct Amemory *root)
{
	register int i = nam_hash(nodp->namid);
	i &= root->memsize;
	nodp->namnxt = root->memhead[i];
	root->memhead[i] = nodp;
}
#else
void	nam_link(nodp,root)
struct namnod *nodp;
struct Amemory *root;
{
	register struct namnod *np,*nq,**npp;
	struct Nodval *nv;
	struct Namaray *ap;
	int dot;
	char *cp = nodp->namid;
	int i = nam_hash(cp);

	i &= root->memsize;
	nodp->namnxt = NULL;
	for(npp= &root->memhead[i],np= *npp;np;npp= &np->namnxt,np= *npp)
		if(strcmp(cp,np->namid)==0)
		{
			if (!(nam_istype (np, N_ARRAY)))
			{
				nq = nam_alloc(cp);
				nq->namnxt = np->namnxt;
				*npp = nq;
				nq->value.namflg =  np->value.namflg|N_ARRAY;
				nq->value.namval.aray = ap = array_grow((struct Namaray *)NULL,0);
				ap->val[0] = &np->value;
				nq->value.namsz = np->value.namsz;
				np = nq;
			}
			ap = array_ptr(np);
			dot = ++ap->adot;
			if (dot >= ap->maxi)
				np->value.namval.aray = ap = array_grow(ap,dot);
			if (nv = ap->val[dot])
				if (freeble (nv))
					rmnval (unmark (nv));
			ap->val[dot] = &nodp->value;
			return;
		}
	*npp = nodp;
}


/*
 *   RMNVAL (NV)
 *
 *        struct Nodval *NV;
 *
 *   Remove freeable string space attached to NV, and then
 *   free the Nodval structure itself.
 *
 */

void	rmnval (nv)
struct Nodval *nv;
{
	register int flag = nv->namflg;
	register union Namval *up = &nv->namval;

	up->cp = NULL;
	if (!(flag & N_FREE))
	{
		if (flag & N_INDIRECT)
			up = up->up;
		if (up->cp != NULL)
			free (up->cp);
	}
	free ((char*)nv);
	return;
}
#endif	/* KSHELL */
