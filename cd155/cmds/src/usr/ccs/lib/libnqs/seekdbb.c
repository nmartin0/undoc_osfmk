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
 
/*++ seekdbb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/seekdbb.c,v $
 *
 * DESCRIPTION:
 *
 *	Seek on an NQS database file without throwing away the current
 *	block cache, if the block in the cache is the one that we are
 *	interested in.  This function should only be called by the NQS
 *	daemon.
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
 * $Log: seekdbb.c,v $
 * Revision 1.3  1994/11/19  02:27:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:51  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:55  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:17  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"

extern int errno;			/* System call error number */
extern long blockdb();			/* Block number for file offset */
extern long lseek();			/* Seek */

/*** seekdbb
 *
 *
 *	void seekdbb():
 *
 *	Seek on an NQS database file without throwing away the current
 *	block cache, if the block in the cache is the one that we are
 *	interested in.  This function should only be called by the NQS
 *	daemon.
 */
void seekdbb (file, position)
register struct confd *file;		/* Configuration file */
long position;				/* File offset position */
{
	long blockoffset;
	register int i;
	register char *cp;

	if (position > file->size) position = file->size;
	else if (position < 0) position = 0;
	blockoffset = blockdb (position) * ATOMICBLKSIZ;
	if (position < file->size) {
		/*
		 *  There is a block in the file that we can read.
		 */
		if (file->rposition - ATOMICBLKSIZ != blockoffset) {
			/*
			 *  We do not have the proper block in the cache.
			 *  Correct this situation!
			 */
			lseek (file->fd, blockoffset, 0);
			errno = 0;	/* Set to zero, so that partial reads */
					/* do not cause a spurious error */
					/* message in nqs_abort(). */
			if (read (file->fd, file->cache->v.chars,
				  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
				printf ("E$Read error in seekdbb().\n");
				fflush (stdout);
			}
			file->rposition = blockoffset + ATOMICBLKSIZ;
		}
	}
	else {
		/*
		 *  We are at the very end of the file.  Fill the cache
		 *  buffer with all zero.  (allodb uses this fact).
		 */
		lseek (file->fd, blockoffset, 0);
		file->rposition = blockoffset;
		cp = file->cache->v.chars;
		for (i=0; i < ATOMICBLKSIZ; i++) *cp++ = '\0';
	}
	file->vposition = position;
}
