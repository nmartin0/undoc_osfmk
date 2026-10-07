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
static char sccsid[] = "@(#)storage.c	1.2  com/cmd/prog/lint/pass2,3.1,9013 9/12/89 11:52:23";
/*
 * COMPONENT_NAME: (CMDPROG) Programming Utilites
 *
 * FUNCTIONS: MBMalloc, StoreMName, StoreSName
 *
 * ORIGINS: 00 03 10 27 32
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#include "mfile1.h"
#include "lint2.h"

/*
** Symbol name storage definitions.
*/
static char	*snameTbl;		/* symbol name table */
static int	snameEmpty;		/* remaining table space */
static char	*mnameTbl;		/* member name table */
static int	mnameEmpty;		/* remaining table space */

/*
** Copy symbol string into permanent string storage.
** Return pointer to saved string.
*/
char *
StoreSName(cp)
	register char *cp;
{
	register int len;

	/* Has storage area been exceeded? */
	len = strlen(cp) + 1;
	if (len > snameEmpty) {
		/* Bigger chuck than NAMEBLK needed? */
		snameEmpty = (len > NAMEBLK) ? len : NAMEBLK;
		snameTbl = (char *) getmem((unsigned) snameEmpty);
	}

	/* Copy string into storage. */
	(void) strncpy(snameTbl, cp, len);
	cp = snameTbl;
	snameTbl += len;
	snameEmpty -= len;
	return (cp);
}

/*
** Copy symbol member string into permanent string storage.
** Return pointer to saved string.
*/
char *
StoreMName(cp)
	register char *cp;
{
	register int len;

	/* Has storage area been exceeded? */
	len = strlen(cp) + 1;
	if (len > mnameEmpty) {
		/* Bigger chuck than NAMEBLK needed? */
		mnameEmpty = (len > NAMEBLK) ? len : NAMEBLK;
		mnameTbl = (char *) getmem((unsigned) mnameEmpty);
	}

	/* Copy string into storage. */
	(void) strncpy(mnameTbl, cp, len);
	cp = mnameTbl;
	mnameTbl += len;
	mnameEmpty -= len;
	return (cp);
}

/*
** Allocate memory for member symbol table.
*/
MBTAB *
MBMalloc()
{
	static int bunchsize = 0;
	static MBTAB *mbbunch;

	if (bunchsize == 0) {
		mbbunch = (MBTAB *) getmem(MBRBLK * sizeof(MBTAB));
		bunchsize = MBRBLK;
	}
	return (&mbbunch[--bunchsize]);
}
