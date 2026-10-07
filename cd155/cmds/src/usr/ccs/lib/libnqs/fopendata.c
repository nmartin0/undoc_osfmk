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
 
/*++ fopendata.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/fopendata.c,v $
 *
 * DESCRIPTION:
 *
 *	Open an NQS req data file as called from a server returning
 *	the opened file stream.
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
 * $Log: fopendata.c,v $
 * Revision 1.3  1994/11/19  02:26:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:16  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:38  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:49:00  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include NMAP				/* Get definition of mid_t */

extern int opendata();			/* Open NQS req data file */

/*** fopendata
 *
 *
 *	FILE *fopendata():
 *
 *	Open an NQS req data file as called from a server returning
 *	the opened file stream.
 *
 *	Note that the current directory of the caller must be the
 *	NQS root directory (which is the directory at the time that
 *	the server is exec'd from the NQS daemon).
 *
 *	Returns:
 *		The opened file stream if successful, otherwise
 *		NULL is returned.
 */
FILE *fopendata (orig_seqno, orig_mid, datano)
long orig_seqno;			/* Sequence number for req */
mid_t orig_mid;				/* Machine-id for req */
int datano;				/* Data file number */
{
	int fd;				/* Opened file descr */

	if ((fd = opendata (orig_seqno, orig_mid, datano)) != -1) {
		return (fdopen (fd, "r"));
	}
	return (NULL);
}
