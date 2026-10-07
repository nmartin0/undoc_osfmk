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
 
/*++ allodb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/allodb.c,v $
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:16 $ $State: Exp $)
 * $Log: allodb.c,v $
 * Revision 1.3  1994/11/19  02:26:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:12  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:09  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:46:24  hender
 * Sterling version 4/22/87
 * 
 *
 */
#include <stdio.h>
#include "nqs.h"

extern int errno;			/* System call error number */
extern void bytecopy();			/* Copy exactly N bytes */
extern long lseek();			/* Seek */
extern void nqs_abort();		/* Abort NQS execution */
extern struct gendescr *readdb();	/* Read descriptor function */
extern void seekdbb();			/* Buffered seek on a database file */
extern int sizedb();			/* Contents size of descriptor */

/*** allodb
 *
 *
 *	long allodb():
 *
 *	Allocate space for, and store a new generic descriptor
 *	structure into, the specified database/configuration file.
 *
 *	The next descriptor returned on a readdb() call will be
 *	the newly created descriptor.
 *
 *	Returns:
 *		The file offset position of the allocated descriptor.
 */
long allodb (file, newdescr, descrtype)
register struct confd *file;		/* Database file */
register struct gendescr *newdescr;	/* New descriptor */
int descrtype;				/* Descriptor type DSC_ */
{
	register struct gendescr *descr;
	register int need;		/* Total size of new descriptor */
	register int avail;		/* Available free space in entry */
	register long descrpos;
	long origblock;			/* Original block searched */
	int size;			/* Size of new descriptor contents */

	size = sizedb (newdescr, descrtype);	/* Size of contents */
	need = asizedb (newdescr, size);	/* Aligned size */
	/*
	 *  See if there exists adequate space in an existing descriptor.
	 *  Need has the number of free bytes required to hold the new
	 *  descriptor.
	 *
	 *  We first examine the block presently in the cache (provided
	 *  that the file has at least one block in it).  This speeds up
	 *  the allocation process if many sequential allodb() calls are
	 *  being made (as occurs during the NQS rebuild on startup).
	 *
	 *  If no free space can be found in the current cache block,
	 *  then a sequential search is made from the very beginning of
	 *  the file.
	 *
	 *  If no free space is found, then a new block is added onto
	 *  the end of the file.
	 *
	 *  Obviously, this algorithm assumes only one writer....
	 */
	if (file->size) {
		/*
		 *  The file has bytes in it.  Thus, the cache block
		 *  contains a block of the file.  Start searching at
		 *  the beginning of the current block.
		 */
		file->vposition = file->rposition - ATOMICBLKSIZ;
		origblock = file->vposition;		/* Original search blk*/
		do {
			descrpos = file->vposition;	/* Position of descr */
			descr = readdb (file);		/* 1st descr in block */
			if (descr->size < 0) avail = -(descr->size);
			else avail = descr->size
				   - asizedb (descr, sizedb(descr, descrtype));
			/*
			 *  Keep looping while within the current block,
			 *  and no space big enough is found....
			 */
		} while (avail < need && file->vposition < file->rposition);
		if (avail < need) {
			/*
			 *  No space was found in the current block.
			 *  Search from the very beginning of the
			 *  file.
			 */
			if (origblock != 0) {
				/*
				 *  We have not searched the very first						 *  block of the file.
				 */
				seekdbb (file, 0L);
			}
			/*
			 *  Otherwise, we have already searched the
			 *  first block of the file.  File->rposition
			 *  = file->vposition forcing read() in readdb().
			 */
			do {
				if (file->vposition == origblock) {
					/*
					 *  Skip this block, because we
					 *  have already searched it.
					 */
					seekdbb (file, origblock+ATOMICBLKSIZ);
				}
				descrpos = file->vposition;
				descr = readdb (file);	/* Get descriptor */
				if (descr != NULL) {
					if (descr->size < 0) {
						avail = -(descr->size);
					}
					else avail = descr->size
						   - asizedb (descr,
							      sizedb (
								descr,
								descrtype
							      ));
				}
			} while (avail < need && descr != NULL);
		}
	}
	else {
		descr = NULL;		/* No descriptors exist */
		descrpos = 0;		/* File offset 0 */
	}
	if (descr == NULL) {
		/*
		 *  No existing descriptor has the necessary free space.
		 *  We reached the end of the file.  Format the cache
		 *  block as a new block for later writing.
		 *
		 *  File->vposition, file->rposition, and descrpos
		 *  all equal:  file->size.
		 */
		seekdbb (file, file->size);	/* Zero fill cache */
		descr = (struct gendescr *) file->cache->v.chars;
		descr->size = ATOMICBLKSIZ;
		file->size += ATOMICBLKSIZ;	/* Increase file size */
	}
	else {
		/*
		 *  Space was located in an existing descriptor entry to
		 *  hold the new entry.  Please note that this entry is
		 *  completely contained in the cache block for the file.
		 */
		if (descr->size < 0) {
			/*
			 *  The very first entry in the block has enough
			 *  space.
			 */
			descr->size = -(descr->size);
		}
		else {
			descr->size -= avail;
			descrpos += descr->size;
			descr = (struct gendescr *)
				(((char *) descr) + descr->size);
			descr->size = avail;
		}
		/*
		 *  The contents of the read cache block will be changed
		 *  by the allocation of the descriptor space.  Seek back
		 *  to rewrite the block.
		 */
		lseek (file->fd, (file->rposition -= ATOMICBLKSIZ), 0);
	}
	/*
	 *  Copy the descriptor contents into the block.
	 */
	bytecopy ((char *) &descr->v, (char *) &newdescr->v, size);
	/*
	 *  Rewrite the modified block (or add a new formatted block).
	 */
	errno = 0;			/* In case write() works partway */
					/* writing some bytes out, then */
					/* nqs_abort() does not show spurious */
					/* error message */
	if (write (file->fd, file->cache->v.chars,
		   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		printf ("F$Write error in allodb().\n");
		nqs_abort();
	}
	file->rposition += ATOMICBLKSIZ;	/* Update real position */
	file->vposition = descrpos;		/* Descriptor just created */
	return (descrpos);			/* Return the address of the */
						/* allocated descriptor */
}


/*** asizedb
 *
 *
 *	int asizedb():
 *	Return aligned size for the specified descriptor.
 */
static int asizedb (descr, nasize)
struct gendescr *descr;				/* Descriptor */
int nasize;					/* Non-aligned size */
{
	register int asize;			/* Aligned size */
	register int residue;			/* Residual */

	asize = nasize + (((char *) &descr->v) - ((char *) &descr->size));
	residue = asize % sizeof (ALIGNTYPE);
	if (residue) asize += sizeof (ALIGNTYPE) - residue;
	return (asize);
}
