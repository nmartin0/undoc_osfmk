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
 
/*++ fetchgrdb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/fetchgrdb.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains three functions:
 *
 *		fetchgrgid(),
 *		fetchgrnam(), and
 *		closegrdb()
 *
 *	that respectively return the group structure for the specified
 *	local group-id, return the group structure for the specified
 *	local group-name, and close the group-database opened by fetchgrgid()
 *	or fetchgrnam().
 *
 *	This module is provided as an "onion-skin layer" residing above
 *	the locally implemented group-database access functions.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	January 20, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:38 $ $State: Exp $)
 * $Log: fetchgrdb.c,v $
 * Revision 1.3  1994/11/19  02:26:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:59  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:31  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:48:17  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <grp.h>			/* Password file structures */


/*
 *	External functions.
 */
extern void endgrent();			/* Close the local group file */
extern struct group *getgrent();	/* Read next group file entry */
extern void setgrent();			/* Rewind for group searches */
extern int strcmp();			/* String comparison */


/*** fetchgrgid
 *
 *
 *	struct group *fetchgrgid():
 *	Return struct group structure for the specified local group-id.
 *
 *	NOTE:	This function does not open and close the
 *		group-database on successive invocations.
 *		It is up to the caller to invoke closegrdb()
 *		when access is no longer required to the
 *		group-database.
 */
struct group *fetchgrgid (gid)
register int gid;			/* Group-id to be identified */
{
	register struct group *p;

	setgrent();			/* Seek to start of group file */
	while ((p = getgrent()) != (struct group *) 0 && p->gr_gid != gid)
		;
	return (p);
}


/*** fetchgrnam
 *
 *
 *	struct group *fetchgrnam():
 *	Return struct group structure for the specified group-name.
 *
 *	NOTE:	This function does not open and close the
 *		group-database on successive invocations.
 *		It is up to the caller to invoke closegrdb()
 *		when access is no longer required to the
 *		group-database.
 */
struct group *fetchgrnam (name)
register char *name;
{
	register struct group *p;

	setgrent();			/* Seek to start of group file */
	while ((p = getgrent()) && strcmp (p->gr_name, name))
		;
	return(p);
}


/*** closegrdb
 *
 *
 *	void closegrdb():
 *
 *	Close the local group-database opened by fetchgrgid(), or
 *	by fetchgrnam().
 */
void closegrdb()
{
	endgrent();			/* Close the local group file */
}
