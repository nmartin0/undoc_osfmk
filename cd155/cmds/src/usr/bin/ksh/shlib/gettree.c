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
static char sccsid[] = "@(#)gettree.c	1.6  com/cmd/ksh/shlib,3.1,9013 9/10/89 06:37:09";
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
 *   GETTREE.C
 *
 *   Programmer:  D. A. Lambeth
 *
 *        Owner:  D. A. Lambeth
 *
 *         Date:  April 17, 1980
 *
 *
 *
 *   GETTREE (MSIZE)
 *
 *        Create a shell associative memory with MSIZE buckets,
 *        and return a pointer to the root of the memory.
 *        MSIZE must be a power of 2.
 *
 *
 *
 *   See Also:  nam_link(III), nam_search(III), libname.h
 */

#include "name.h"

/*
 *   GETTREE (MSIZE)
 *
 *      int MSIZE;
 *
 *   Create an associative memory containing MSIZE headnodes or
 *   buckets, and return a pointer to the root of the memory.
 *
 *   Algorithm:  Memory consists of a hash table of MSIZE buckets,
 *               each of which holds a pointer to a linked list
 *               of namnods.  Nodes are hashed into a bucket by
 *               namid.
 */

struct Amemory *gettree(msize)
register int msize;
{
	register struct Amemory *root;

	--msize;
	root = new_of(struct Amemory,msize*sizeof(struct namnod*));
	root->memsize = msize;
	root->nexttree = NULL;
	while (msize>=0)
		root->memhead[msize--] = NULL;
	return (root);
}
