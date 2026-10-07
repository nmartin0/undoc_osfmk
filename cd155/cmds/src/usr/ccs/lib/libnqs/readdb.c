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
 
/*++ readdb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/readdb.c,v $
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:39 $ $State: Exp $)
 * $Log: readdb.c,v $
 * Revision 1.3  1994/11/19  02:27:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:26  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:41  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:54:04  hender
 * Sterling version 4/22/87
 * 
 *
 */
#include "nqs.h"

extern void seekdbb();			/* Buffered seek */

/*** readdb
 *
 *
 *	struct gendescr *readdb():
 *
 *	Read a gendescr structure from the current position updating
 *	the specified configuration file position to the next gendescr
 *	structure in the file.  Note that the gendescr structure
 *	returned may not contain any data (it may be unallocated).
 *
 *	Returns:
 *		A pointer to the gendescr structure just read if
 *		successful; otherwise a NULL pointer value is returned
 *		(indicating EOF).
 */
struct gendescr *readdb (file)
register struct confd *file;
{
	register struct gendescr *gendescr;
	register int size;

	if (file->vposition >= file->size) return ((struct gendescr *) 0);
						/* EOF */
	seekdbb (file, file->vposition);
	gendescr = (struct gendescr *) (file->cache->v.chars
		 +  file->vposition % ATOMICBLKSIZ);
	file->lastread = file->vposition;	/* Remember last read address */
	size = gendescr->size;
	if (size < 0) size = -size;		/* Descriptor is unallocated */
	file->vposition += size;		/* Update virtual position */
	return (gendescr);			/* Return ptr to descriptor */
}
