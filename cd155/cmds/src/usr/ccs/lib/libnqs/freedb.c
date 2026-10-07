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
 
/*++ freedb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/freedb.c,v $
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:48 $ $State: Exp $)
 * $Log: freedb.c,v $
 * Revision 1.3  1994/11/19  02:26:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:19  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:39  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:49:08  hender
 * Sterling version 4/22/87
 * 
 *
 */
#include <stdio.h>
#include "nqs.h"

extern int errno;			/* System call error number */
extern long lseek();			/* Seek */
extern void nqs_abort();		/* Abort NQS execution */
extern void seekdbb();			/* Buffered seek */

/*** freedb
 *
 *
 *	void freedb():
 *
 *	Free the descriptor entry at the specified file offset
 *	in the specified NQS database/configuration file.  The
 *	current position of the database file is placed at the
 *	descriptor record FOLLOWING the deleted descriptor.
 */
void freedb (file, position)
register struct confd *file;		/* Database/configuration file */
register long position;			/* Position of descriptor to be */
					/* freed */
{
	register struct gendescr *descr1;
	register struct gendescr *descr2;
	register long offset;
	register long doffset;

	offset = position % ATOMICBLKSIZ;	/* Offset within block */
	descr1 = (struct gendescr *) file->cache->v.chars;
	seekdbb (file, position);	/* Set virtual position to reference */
					/* descriptor targeted for deletion. */
	if (offset) {
		/*
		 *  Merge deleted descriptor with the previous descriptor
		 *  in the block.
		 */
		doffset = descr1->size;
		if (doffset < 0) {
			/*
			 *  The very first descriptor in the block is free.
			 *  (Only the size of the very FIRST descriptor in
			 *   a block can be negative).
			 */
			doffset = -doffset;
		}
		while (doffset < offset) {
			/*
			 *  We still have not found the descriptor immediately
			 *  previous to the descriptor that we are trying to
			 *  delete.
			 */
			descr1 = (struct gendescr *)
				 (file->cache->v.chars + doffset);
			doffset += descr1->size;
		}
		/*
		 *  Descr1 now points at the descriptor immediately previous
		 *  to the descriptor being deleted.
		 */
		descr2 = (struct gendescr *) (file->cache->v.chars+offset);
		file->vposition += descr2->size;/* Position at next descr */
		/*
		 *  Merge the deleted descriptor with the previous descriptor.
		 */
		if (descr1->size < 0) {
			descr1->size -= descr2->size;
		}
		else descr1->size += descr2->size;
	}
	else {
		file->vposition += descr1->size;/* Position at next descr */
		descr1->size = -descr1->size;	/* Mark descriptor at head of */
						/* block as free */
	}
	/*
	 *  File->vposition has the position of the next descriptor after
	 *  the deleted descriptor in the database file.
	 *  Now, rewrite the updated block.
	 */
	lseek (file->fd, file->rposition - ATOMICBLKSIZ, 0);
						/* Seek back to block */
	errno = 0;				/* Set to zero so partial */
						/* writes do not give spurious*/
						/* error message in nqs_abort.*/
	if (write (file->fd, file->cache->v.chars,
		   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		printf ("F$Write error in freedb().\n");
		nqs_abort();
	}
}
