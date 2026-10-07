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
 
/*++ opendata.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/opendata.c,v $
 *
 * DESCRIPTION:
 *
 *	Open an NQS request data file as called from a server returning
 *	the opened file descriptor number.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:27 $ $State: Exp $)
 * $Log: opendata.c,v $
 * Revision 1.3  1994/11/19  02:27:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:55  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:25  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:52:42  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)opendata.c	1.2 (opendata.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nqs.h"
#include "nqsxdirs.h"			/* NQS directory definitions */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif

extern void pack6name();		/* Compute name on 6-bit alphabet */

/*** opendata
 *
 *
 *	int opendata():
 *
 *	Open an NQS req data file as called from a server returning
 *	the opened file descriptor number.
 *
 *	Note that the current directory of the caller must be the
 *	NQS root directory (which is the directory at the time that
 *	the server is exec'd from the NQS daemon).
 *
 *	Returns:
 *	      >=0: as the file descriptor of the opened file if
 *		   successful;
 *	       -1: if an error occurs in which case errno is set.
 */
int opendata (orig_seqno, orig_mid, datano)
long orig_seqno;			/* Sequence# for req */
mid_t orig_mid;				/* Machine-id for req */
int datano;				/* Data file number */
{
	char path [MAX_PATHNAME+1];	/* Pathname for data file */

	pack6name (path, Nqs_data, (int) (orig_seqno % MAX_OUTPSUBDIRS),
		  (char *) 0, (long) orig_seqno, 5, (long) orig_mid, 6,
		   datano-1, 3);
	return (open (path, O_RDONLY));
}
