/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: ndbm.h,v $
 * Revision 1.3  1994/11/18  21:10:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:16  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:10  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:43  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:18  nandy
 * Initial revision
 *
 * Revision 2.5  92/04/13  14:38:06  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.5  90/10/07  20:32:25  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:52:49  gm]
 * 
 * Revision 2.4  90/03/13  21:23:08  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  17:49:33  tom]
 * 
 * Revision 2.3  90/01/02  19:15:21  gm
 * 	     Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  10:10:37  gm
 * 	     Added function prototype declarations and protection for
 * 	     recursive includes.
 * 	     [89/06/30  11:58:40  bww]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	ndbm.h	5.1 (Berkeley) 5/30/85
 */

#ifndef _NDBM_H_
#define _NDBM_H_

/*
 * Hashed key data base library.
 */
#define PBLKSIZ 1024
#define DBLKSIZ 4096

typedef struct {
	int	dbm_dirf;		/* open directory file */
	int	dbm_pagf;		/* open page file */
	int	dbm_flags;		/* flags, see below */
	long	dbm_maxbno;		/* last ``bit'' in dir file */
	long	dbm_bitno;		/* current bit number */
	long	dbm_hmask;		/* hash mask */
	long	dbm_blkptr;		/* current block for dbm_nextkey */
	int	dbm_keyptr;		/* current key for dbm_nextkey */
	long	dbm_blkno;		/* current page to read/write */
	long	dbm_pagbno;		/* current page in pagbuf */
	char	dbm_pagbuf[PBLKSIZ];	/* page file block buffer */
	long	dbm_dirbno;		/* current block in dirbuf */
	char	dbm_dirbuf[DBLKSIZ];	/* directory file block buffer */
} DBM;

#define _DBM_RDONLY	0x1	/* data base open read-only */
#define _DBM_IOERR	0x2	/* data base I/O error */

#define dbm_rdonly(db)	((db)->dbm_flags & _DBM_RDONLY)

#define dbm_error(db)	((db)->dbm_flags & _DBM_IOERR)
	/* use this one at your own risk! */
#define dbm_clearerr(db)	((db)->dbm_flags &= ~_DBM_IOERR)

/* for flock(2) and fstat(2) */
#define dbm_dirfno(db)	((db)->dbm_dirf)
#define dbm_pagfno(db)	((db)->dbm_pagf)

typedef struct {
	char	*dptr;
	int	dsize;
} datum;

/*
 * flags to dbm_store()
 */
#define DBM_INSERT	0
#define DBM_REPLACE	1

#ifdef _NO_PROTO
DBM	*dbm_open();
void	dbm_close();
datum	dbm_fetch();
datum	dbm_firstkey();
datum	dbm_nextkey();
long	dbm_forder();
int	dbm_delete();
int	dbm_store();
#else
extern DBM      *dbm_open(char*, int, int);
extern void     dbm_close(DBM*);
extern datum    dbm_fetch(DBM*, datum);
extern datum    dbm_firstkey(DBM*);
extern datum    dbm_nextkey(DBM*);
extern long     dbm_forder(DBM*, datum);
extern int      dbm_delete(DBM*, datum);
extern int      dbm_store(DBM*, datum, datum, int);
#endif  /* _NO_PROTO */

#endif /* _NDBM_H_ */
