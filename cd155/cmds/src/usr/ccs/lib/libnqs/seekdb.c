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
 
/*++ seekdb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/seekdb.c,v $
 *
 * DESCRIPTION:
 *
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:49 $ $State: Exp $)
 * $Log: seekdb.c,v $
 * Revision 1.3  1994/11/19  02:27:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:49  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:54  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:10  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"

extern long blockdb();			/* Return block number for offset */
extern long lseek();

/*** seekdb
 *
 *
 *	void seekdb():
 *
 *	Seek on the specified NQS database file.
 */
void seekdb (file, newposition)
register struct confd *file;
register long newposition;
{
	if (newposition > file->size) newposition = file->size;
	else if (newposition < 0) newposition = 0;
	file->rposition = blockdb (newposition) * ATOMICBLKSIZ;
	file->vposition = newposition;
	if (lseek (file->fd, file->rposition, 0) == -1) {
		printf ("E$Lseek error in seekdb().\n");
		fflush (stdout);
	}
	if (file->rposition < file->size) {
		/*
		 *  The file has at least one block in it that we
		 *  can read.
		 *
		 *  Read the current block into the cache.  We ALWAYS
		 *  throw away the current cache block, since many of
		 *  the NQS utilities scan NQS database files AFTER an
		 *  update has been made BY THE NQS DAEMON, and with-
		 *  out this forced reloading of the cache block, such
		 *  programs would not necessarily get the updated
		 *  version of the database file....
		 */
		if (read (file->fd, file->cache->v.chars,
			  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
			printf ("E$Read error in seekdb().\n");
			fflush (stdout);
		}
		file->rposition += ATOMICBLKSIZ;
	}
}
