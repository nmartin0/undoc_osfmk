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
static char sccsid[] = "@(#)namscan.c	1.6  com/cmd/ksh/shlib,3.1,9013 9/10/89 06:39:14";
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
 *   NAMSCAN.C
 *
 *   GSCAN_ALL (FN, ROOT)
 *        Execute FN at each node in the linked memory trees,
 *        which are given by ROOT.
 *
 *   GSCAN_SOME (FN, ROOT, MASK, FLAG)
 *        Execute FN at those nodes in the linked memory trees
 *        that have certain attributes, as determined by MASK and
 *        FLAG. ROOT is the first of the list of memory trees.
 *
 *   SCAN_ALL (FN, ROOT)
 *        Execute function FN at each of the namnods in the tree
 *        given by ROOT.
 *
 */

#include	"name.h"

/* These routines are defined by this module */
int	gscan_all();
int	scan_all();
int	gscan_some();

static int scanmask;
static int scanflag;


/*
 *   GSCAN_ALL (FN, ROOT)
 *   
 *        int (*FN)();
 *
 *	  struct Amemory *root;
 *
 *   Execute FN at each node in the linked memory trees. 
 *   Note that the first tree need not exist.
 */

int	gscan_all(fn, root)
void	(*fn)();
struct Amemory *root;
{
	register struct Amemory *app = root;
	register int n = 0;
	while(app)
	{
		n += scan_all(fn,app);
		app = app->nexttree;
	}
	return(n);
}


/*
 *   GSCAN_SOME (FN, ROOT, MASK, FLAG)
 *        int (*FN)();
 *        struct Amemory *ROOT;
 *        int MASK;
 *        int FLAG;
 *
 *   Execute FN at each of the namnods in the trees given by ROOT
 *   that meet certain criteria, as determined by MASK and FLAG.
 *   If flag is non-zero then at least one of these mask bits must be on.
 *   If flag is zero then all the mask bits must be off to match.
 */

int	gscan_some(fn,root,mask,flag)
void (*fn)();
int mask,flag;
struct Amemory *root;
{
	register int n;
	scanmask = mask;
	scanflag = flag;
	n = gscan_all(fn,root);
	scanmask = scanflag = 0;
	return(n);
}

/*
 *   SCAN_ALL (FN, ROOT)
 *        int (*FN)();
 *        struct Amemory *ROOT;
 *
 *   Execute FN at each node in the tree given by ROOT, according
 *   to the values of scanmask and scanflag, which are established
 *   in scan_some().
 */

int	scan_all(fn,root)
void (*fn)();
struct Amemory *root;
{
	register struct namnod *np;
	register int i;
#ifdef NAME_SCOPE
	register int smask = scanmask^N_AVAIL;
#else
	register int smask = scanmask;
#endif /* NAME_SCOPE */
	register int count = 0;
	int k;
	for(i=0;i <= root->memsize;i++)
	{
		for(np=root->memhead[i];np;np= np->namnxt)
		{
			k = np->value.namflg&smask;
			if((scanflag?scanflag&k:k==0))
			{
				if(np->value.namval.cp==0)
					if(nam_istype(np,~N_DEFAULT)==0)
						continue;
				if(fn)
				{
					if(nam_istype(np,N_ARRAY))
						array_dotset(np,0);
					(*fn)(np);
				}
				count++;
			}
		}
	}
	return(count);
}

