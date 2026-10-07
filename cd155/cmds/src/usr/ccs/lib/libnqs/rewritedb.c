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
 
/*++ rewritedb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/rewritedb.c,v $
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:41 $ $State: Exp $)
 * $Log: rewritedb.c,v $
 * Revision 1.3  1994/11/19  02:27:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:34  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:47  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:54:29  hender
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
extern void seekdbb();			/* Buffered seek */
extern int sizedb();			/* Descriptor contents size function */

/*** rewritedb
 *
 *
 *	void rewritedb():
 *
 *	Rewrite an existing gendescr structure in the specified file
 *	at the specified offset position.  The file position is left
 *	pointing at the descriptor that was just re-written.
 */
void rewritedb (file, position, descr, descrtype)
register struct confd *file;		/* Configuration file */
long position;				/* File offset position */
struct gendescr *descr;			/* New version of descriptor */
int descrtype;				/* Descriptor type */
{
	register struct gendescr *target;

	seekdbb (file, position);
	/*
	 *  Rewrite the descriptor contents.
	 */
	target = (struct gendescr *) (file->cache->v.chars
	       + position % ATOMICBLKSIZ);
	bytecopy ((char *) &target->v, (char *) &descr->v,
		  sizedb (descr, descrtype));
	/*
	 *  Seek for write().
	 */
	lseek (file->fd, file->rposition - ATOMICBLKSIZ, 0);
	errno = 0;			/* Set to zero so partial writes do */
					/* not show spurious error message */
					/* text in nqs_abort(). */
	if (write (file->fd, file->cache->v.chars,
		   ATOMICBLKSIZ) != ATOMICBLKSIZ) {
		printf ("F$Write error in rewritedb().\n");
		nqs_abort();
	}
}
