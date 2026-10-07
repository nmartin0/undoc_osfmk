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
 
/*++ fetchpwdb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/fetchpwdb.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains three functions:
 *
 *		fetchpwuid(),
 *		fetchpwnam(), and
 *		closepwdb()
 *
 *	that respectively return the passwd structure for the specified
 *	local user-id, return the passwd structure for the specified
 *	local username, and close the local account/password database
 *	opened by fetchpwuid() or fetchpwnam().
 *
 *	This module is provided as an "onion-skin layer" residing above
 *	the locally implemented account/password database access
 *	functions.
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
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:26:39 $ $State: Exp $)
 * $Log: fetchpwdb.c,v $
 * Revision 1.4  1994/11/19  02:26:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/04/06  22:12:23  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s): fetchpwdb.c listq.c listr.c main_dsp.c
 *
 * Revision 1.2  1992/10/09  20:16:01  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:32  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:48:21  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)fetchpwdb.c	1.2 (fetchpwdb.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#include <stdio.h>			/* for definition of L_cuserid */
#include <pwd.h>			/* Password file structures */


/*
 *	External functions.
 */
extern void endpwent();			/* Close password file */
extern struct passwd *getpwent();	/* Read next password file entry */
#if 	UNICOS & NEWPWD
extern void getpwsysent();		/* Force use of new password file */
#endif
extern void setpwent();			/* Rewind for password searches */
#if	BSD42 | BSD43 | ULTRIX
extern int strcmp();			/* String comparison */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern int strncmp();			/* Anchored string comparison */
#else
BAD SYSTEM TYPE
#endif
#endif


/*** fetchpwuid
 *
 *
 *	struct passwd *fetchpwuid():
 *	Return struct passwd structure for the specified local user-id.
 *
 *	NOTE:	This function does not open and close the
 *		account/password database on successive
 *		invocations.  It is up to the caller to
 *		invoke closepwdb() when access to the
 *		account/password database is no longer
 *		required.
 */
struct passwd *fetchpwuid (uid)
register int uid;			/* User-id to be identified */
{
	register struct passwd *p;

#if 	UNICOS & NEWPWD
	getpwsysent();			/* Force use of new password file */
#endif
#ifdef SDSC
	endpwent();			/* close the open passwd file */
#else
	setpwent();			/* seek to start of passwd file */
#endif
	while ((p = getpwent()) != (struct passwd *) 0 && p->pw_uid != uid)
		;
	return (p);
}


/*** fetchpwnam
 *
 *
 *	struct passwd *fetchpwnam():
 *	Return struct passwd structure for the specified local username.
 *
 *	NOTE:	This function does not open and close the
 *		account/password database on successive
 *		invocations.  It is up to the caller to
 *		invoke closepwdb() when access to the
 *		account/password database is no longer
 *		required.
 */
struct passwd *fetchpwnam (name)
register char *name;
{
	register struct passwd *p;

	setpwent();			/* Seek to start of passwd file */
#if	SGI | SYS52 | UNICOS | UTS
#if 	NEWPWD
	getpwsysent();			/* Force use of new password file */
#endif
	while ((p = getpwent()) && strncmp (name, p->pw_name, L_cuserid - 1))
#else
#if	BSD42 | BSD43 | ULTRIX | OSF
	while ((p = getpwent()) && strcmp (name, p->pw_name))
#else
BAD SYSTEM TYPE
#endif
#endif
		;
	return (p);
}


/*** closepwdb
 *
 *
 *	void closepwdb():
 *
 *	Close the local account/password database opened by fetchpwuid(),
 *	or by fetchpwnam().
 */
void closepwdb()
{
	endpwent();			/* Close the local password file */
}
