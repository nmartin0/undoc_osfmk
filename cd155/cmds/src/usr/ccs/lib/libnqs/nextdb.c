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
 
/*++ nextdb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/nextdb.c,v $
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:20 $ $State: Exp $)
 * $Log: nextdb.c,v $
 * Revision 1.3  1994/11/19  02:27:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:33  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:21  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:52:18  hender
 * Sterling version 4/22/87
 * 
 *
 */
#include "nqs.h"

extern struct gendescr *readdb();	/* Read descriptor function */

/*** nextdb
 *
 *
 *	struct gendescr *nextdb()
 *
 *	Return a pointer to the next allocated descriptor in the
 *	specified database/configuration file;  If no more allocated
 *	descriptors exist in the specified file, then return NULL.
 */
struct gendescr *nextdb (file)
register struct confd *file;
{
	register struct gendescr *gendescr;
	short zero=0;	/* used to force signed comparison Intergraph */

	/*
	 *  Loop to get next generic descriptor from the database file
	 *  ignoring unallocated entries.
	 */
	do {
		gendescr = readdb (file);	/* Get next descriptor */
	} while (gendescr != (struct gendescr *)0 && gendescr->size < zero);
	return (gendescr);
}
