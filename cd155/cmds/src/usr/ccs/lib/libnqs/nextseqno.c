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
 
/*++ nextseqno.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/nextseqno.c,v $
 *
 * DESCRIPTION:
 *
 *	Return the next available NQS request sequence number.
 *	The current directory of the caller MUST be the NQS
 *	root directory.
 *
 *	For the reasons behind the peculiar storage of the
 *	NQS request sequence number, see ../src/nqs_ldconf.c.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 20, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:21 $ $State: Exp $)
 * $Log: nextseqno.c,v $
 * Revision 1.3  1994/11/19  02:27:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:35  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:22  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:52:21  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqsxdirs.h"		/* Get Nqs_seqno definition */

#if OSF
#include "sys_types_OSF.h"       /* For sys/stat.h */ /* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>
#endif

#include <sys/stat.h>

/*** nextseqno
 *
 *
 *	long nextseqno():
 *
 *	Return the next available NQS request sequence number.
 *	The current directory of the caller MUST be the NQS
 *	root directory.
 */
long nextseqno ()
{
	struct stat stat_buf;		/* Stat() buffer */

	if (stat (Nqs_seqno, &stat_buf) == -1) return (0);
	if ((stat_buf.st_mode & 0777) == 0) return (0);
	return ((long) stat_buf.st_mtime);
}
