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
 
/*++ blockdb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/blockdb.c,v $
 *
 * DESCRIPTION:
 *
 *	Return block number for database file offset.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:21 $ $State: Exp $)
 * $Log: blockdb.c,v $
 * Revision 1.3  1994/11/19  02:26:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:25  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:15  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:46:48  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"			/* Get ATOMICBLKSIZ definition */

/*** blockdb
 *
 *
 *	long blockdb():
 *
 *	Return the block number of the database file for the specified
 *	database file offset.
 *
 *	NOTES:
 *		This function exists ONLY to implement the expression:
 *
 *			blockpos = ((offset / ATOMICBLKSIZ) * ATOMICBLKSIZ);
 *
 *		Unfortunately, we cannot say that directly since smart
 *		C compilers will reduce it to read:
 *
 *			blockpos = offset;
 *
 *		Thus we write (in other modules):
 *
 *			extern long blockdb();
 *				:
 *				:
 *			blockpos = blockdb (offset) * ATOMICBLKSIZ;
 */
long blockdb (filepos)
register long filepos;
{
	return (filepos / ATOMICBLKSIZ);
}
