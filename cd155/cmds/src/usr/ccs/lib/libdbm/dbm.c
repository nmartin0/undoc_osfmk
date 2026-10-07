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
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: dbm.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 02:19:43 $";
#endif


#include	"dbm.h"

#define	NODB	((DBM *)0)

static DBM *cur_db = NODB;

static char no_db[] = "dbm: no open database\n";

dbminit(file)
	char *file;
{
	if (cur_db != NODB)
		dbm_close(cur_db);

	cur_db = dbm_open(file, 2, 0);
	if (cur_db == NODB) {
		cur_db = dbm_open(file, 0, 0);
		if (cur_db == NODB)
			return (-1);
	}
	return (0);
}

long
forder(key)
datum key;
{
	if (cur_db == NODB) {
		printf(no_db);
		return (0L);
	}
	return (dbm_forder(cur_db, key));
}

datum
fetch(key)
datum key;
{
	datum item;

	if (cur_db == NODB) {
		printf(no_db);
		item.dptr = 0;
		return (item);
	}
	return (dbm_fetch(cur_db, key));
}

delete(key)
datum key;
{
	if (cur_db == NODB) {
		printf(no_db);
		return (-1);
	}
	if (dbm_rdonly(cur_db))
		return (-1);
	return (dbm_delete(cur_db, key));
}

store(key, dat)
datum key, dat;
{
	if (cur_db == NODB) {
		printf(no_db);
		return (-1);
	}
	if (dbm_rdonly(cur_db))
		return (-1);

	return (dbm_store(cur_db, key, dat, DBM_REPLACE));
}

datum
firstkey()
{
	datum item;

	if (cur_db == NODB) {
		printf(no_db);
		item.dptr = 0;
		return (item);
	}
	return (dbm_firstkey(cur_db));
}

datum
nextkey(key)
datum key;
{
	datum item;

	if (cur_db == NODB) {
		printf(no_db);
		item.dptr = 0;
		return (item);
	}
	return (dbm_nextkey(cur_db));
}


/*
 *  Added for portablility of applications from systems
 *  which use dbmclose()
 */

dbmclose()
{
	return(0);
}

