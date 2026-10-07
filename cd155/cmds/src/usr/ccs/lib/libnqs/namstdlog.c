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
 
/*++ namstdlog.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/namstdlog.c,v $
 *
 * DESCRIPTION:
 *
 *	Return pointer to name of spooled stdlog output file for
 *	a batch request.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:19 $ $State: Exp $)
 * $Log: namstdlog.c,v $
 * Revision 1.3  1994/11/19  02:27:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:29  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:19  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:59  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"			/* For MAX_OUTPSUBDIRS */
#include "nqsxdirs.h"

extern void pack6name();		/* Compute name on 6-bit alphabet */
extern char *strcat();			/* String concatenate */

/*** namstdlog
 *
 *
 *	char *namstdlog():
 *
 *	Return pointer to name of spooled stdlog output file for
 *	a batch request.
 *
 *	NOTE:	The name is computed relative to the NQS root directory.
 */
char *namstdlog (orig_seqno, orig_mid)
long orig_seqno;			/* Req sequence number */
mid_t orig_mid;				/* Req machine-id */
{
	static char path [48];		/* Pathname for file.  This size */
					/* may need to be increased.... */

	pack6name (path, Nqs_output, (int) (orig_seqno % MAX_OUTPSUBDIRS),
		  (char *) 0, (long) orig_seqno, 5, (long) orig_mid, 6, 0, 0);
	strcat (path, "l");
	return (path);
}
