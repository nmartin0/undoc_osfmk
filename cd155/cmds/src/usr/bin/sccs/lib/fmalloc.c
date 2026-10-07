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
static char rcsid[] = "@(#)$RCSfile: fmalloc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:15 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: fmalloc, ffree, ffreeall
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/* fmalloc.c 1.4 com/cmd/sccs/lib/comobj,3.1,9021 9/15/89 13:59:07"; */

#include "defines.h"

char    *ptrlist;
void	free();

/* allocate asize bytes;
 * chain to ptrlist for later ffreeall
 */
char *
fmalloc(asize)
unsigned asize;
{
	char *ptr, *malloc();

	asize += sizeof(char *);

	if (!(ptr = malloc(asize)))
		fatal(MSGCO(OTOFSPC, 
                  "\nThere is not enough memory available now.(ut9)\n"));

	*(char **)ptr = ptrlist;
	ptrlist = ptr;
	return ptr + sizeof(char *);
}

/* free space allocated by fmalloc; unlink from ptrlist */
ffree(aptr)
char *aptr;
{
	register char **pp, *ptr;

	aptr -= sizeof(char *);
	for (pp = &ptrlist;; pp = (char **)ptr) {
		if (!(ptr = *pp))
			fatal(MSGCO(FFREE, 
                           "\nAn attempt to free memory failed.\n"));  /* MSG */
		if (aptr == ptr)
			break;
	}
	*pp = *(char **)ptr;
	free(aptr);
}

/* free all memory on ptrlist */
ffreeall()
{
	register char *ptr;

	while (ptr = ptrlist) {
		ptrlist = *(char **)ptr;
		free(ptr);
	}
}
