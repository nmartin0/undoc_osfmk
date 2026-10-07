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
 
/*++ fsizedb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/fsizedb.c,v $
 *
 * DESCRIPTION:
 *
 *	Update the current size of the specified database file.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:49 $ $State: Exp $)
 * $Log: fsizedb.c,v $
 * Revision 1.3  1994/11/19  02:26:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:21  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:41  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:49:12  hender
 * Sterling version 4/22/87
 * 
 *
 */
#include "nqs.h"

extern long lseek();
extern void seekdb();			/* Unbuffered seek on an NQS */
					/* database file */

/*** fsizedb
 *
 *
 *	void fsizedb():
 *	Update the current size of the specified NQS database file.
 */
void fsizedb (file)
register struct confd *file;
{
	register long filesize;

	filesize = lseek (file->fd, 0L, 2);
	/*
	 *  Seek back to the original position (also reloading the
	 *  contents of the cache buffer, since the cache block could
	 *  be out of date)!!!!
	 */
	file->size = filesize;		/* Update size */
	seekdb (file, file->vposition);	/* Restore file position and */
					/* reload cache block */
}
