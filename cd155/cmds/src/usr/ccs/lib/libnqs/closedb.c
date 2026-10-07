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
 
/*++ closedb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/closedb.c,v $
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:25 $ $State: Exp $)
 * $Log: closedb.c,v $
 * Revision 1.3  1994/11/19  02:26:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:35  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:21  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:47:20  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"

extern void free();			/* Release heap space */

/*** closedb
 *
 *
 *	void closedb():
 *	Close the specified NQS database/configuration file.
 */
void closedb (file)
register struct confd *file;
{
	if (file->fd >= 0) {
		close (file->fd);	/* Close the file */
		file->fd = -1;		/* Mark confd file descriptor */
					/* as free */
		free ((char *) file->cache);
					/* Free read cache buffer */
	}
}
