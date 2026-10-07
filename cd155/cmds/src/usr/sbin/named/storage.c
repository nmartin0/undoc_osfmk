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
static char rcsid[] = "@(#)$RCSfile: storage.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:10:11 $";
#endif
/*
static char sccsid[] = "@(#)storage.c	1.3  com/sockcmd/named,3.1,8943 10/11/89 13:40:57";
*/
/* 
 * COMPONENT_NAME: TCPIP storage.c
 * 
 * FUNCTIONS: calloc, cfree, openlog, rt_free, rt_malloc, rt_prmem, 
 *            syslog 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 *			S T O R A G E . C
 *
 * Ray Tracing program, storage manager.
 *
 *  Functions -
 *	rt_malloc	Allocate storage, with visibility & checking
 *	rt_free		Similarly, free storage
 *	rt_prmem	When debugging, print memory map
 *	calloc, cfree	Which call rt_malloc, rt_free
 *
 *  Author -
 *	Michael John Muuss
 *  
 *  Source -
 *	SECAD/VLD Computing Consortium, Bldg 394
 *	The U. S. Army Ballistic Research Laboratory
 *	Aberdeen Proving Ground, Maryland  21005-5066
 *  
 *  Copyright Notice -
 *	This software is Copyright (C) 1987 by the United States Army.
 *	All rights reserved.
 */
/*
#ifndef lint
static char RCSstorage[] = "@(#)$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/named/storage.c,v 1.2 1994/11/19 03:10:11 mtm Exp $";
#endif
*/

#include <sys/param.h>
#if BSD >= 43
#include <sys/syslog.h>
#else
#include <stdio.h>
#define	LOG_ERR	0
#endif BSD

#undef malloc
#undef free

#define MDB_SIZE	20000
#define MDB_MAGIC	0x12348969
struct memdebug {
	char	*mdb_addr;
	char	*mdb_str;
	int	mdb_len;
} rt_mdb[MDB_SIZE];

/*
 *			R T _ M A L L O C
 */
char *
rt_malloc(cnt)
unsigned int cnt;
{
	register char *ptr;
	extern char *malloc();

	cnt = (cnt+2*sizeof(int)-1)&(~(sizeof(int)-1));
	ptr = malloc(cnt);

	if( ptr==(char *)0 ) {
		syslog(LOG_ERR, "rt_malloc: malloc failure");
		abort();
	} else 	{
		register struct memdebug *mp = rt_mdb;
		for( ; mp < &rt_mdb[MDB_SIZE]; mp++ )  {
			if( mp->mdb_len > 0 )  continue;
			mp->mdb_addr = ptr;
			mp->mdb_len = cnt;
			mp->mdb_str = "???";
			goto ok;
		}
		syslog(LOG_ERR, "rt_malloc:  memdebug overflow\n");
	}
ok:	;
	{
		register int *ip = (int *)(ptr+cnt-sizeof(int));
		*ip = MDB_MAGIC;
	}
	return(ptr);
}

/*
 *			R T _ F R E E
 */
void
rt_free(ptr)
char *ptr;
{
	register struct memdebug *mp = rt_mdb;
	for( ; mp < &rt_mdb[MDB_SIZE]; mp++ )  {
			if( mp->mdb_len <= 0 )  continue;
		if( mp->mdb_addr != ptr )  continue;
		{
			register int *ip = (int *)(ptr+mp->mdb_len-sizeof(int));
			if( *ip != MDB_MAGIC )  {
				syslog(LOG_ERR, "ERROR rt_free(x%x, %s) corrupted! x%x!=x%x\n", ptr, "???", *ip, MDB_MAGIC);
				abort();
			}
		}
		mp->mdb_len = 0;	/* successful free */
		goto ok;
	}
	syslog(LOG_ERR, "ERROR rt_free(x%x, %s) bad pointer!\n", ptr, "???");
	abort();
ok:	;

	*((int *)ptr) = -1;	/* zappo! */
	free(ptr);
}

/*
 *			R T _ P R M E M
 * 
 *  Print map of memory currently in use.
 */
void
rt_prmem(str)
char *str;
{
	register struct memdebug *mp = rt_mdb;
	register int *ip;

	printf("\nRT memory use\t\t%s\n", str);
	for( ; mp < &rt_mdb[MDB_SIZE]; mp++ )  {
		if( mp->mdb_len <= 0 )  continue;
		ip = (int *)(mp->mdb_addr+mp->mdb_len-sizeof(int));
		printf("%7x %5x %s %s\n",
			mp->mdb_addr, mp->mdb_len, mp->mdb_str,
			*ip!=MDB_MAGIC ? "-BAD-" : "" );
		if( *ip != MDB_MAGIC )
			printf("\t%x\t%x\n", *ip, MDB_MAGIC);
	}
}

char *
calloc(num, size)
	register unsigned num, size;
{
	extern char *malloc();
	register char *p;

	size *= num;
	if (p = rt_malloc(size))
		bzero(p, size);
	return (p);
}

cfree(p, num, size)
	char *p;
	unsigned num;
	unsigned size;
{
	rt_free(p);
}

#if BSD < 43
openlog() {}

syslog(x, str, a, b, c, d, e, f)
int	x;
char	*str;
int	a, b, c, d, e, f;
{
	fprintf(stderr, str, a, b, c, d, e, f);
}
#endif BSD
