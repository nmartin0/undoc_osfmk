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
 
/*++ destpath.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/destpath.c,v $
 *
 * DESCRIPTION:
 *
 *	Return a pointer to the path-name portion of a remote
 *	path specification:  machine_name:path.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	November 5, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:31 $ $State: Exp $)
 * $Log: destpath.c,v $
 * Revision 1.3  1994/11/19  02:26:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:49  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:26  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:47:52  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)destpath.c	1.2 (destpath.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#include "nqs.h"			/* NQS types and definitions */

#if	BSD42 | BSD43 | ULTRIX
extern char *index();			/* Locate character within string */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strchr();			/* Locate character within string */
#else
BAD SYSTEM TYPE
#endif
#endif
extern char *strncpy();			/* Anchored string copy */

/*** destpath
 *
 *
 *	char *destpath():
 *
 *	Return a pointer to the path-name portion of a remote
 *	path specification:  machine_name:path.
 */
char *destpath (remote_path)
register char *remote_path;		/* Remote path specification text */
{
	static char pathname [MAX_REQPATH+2];
					/* Remote path name of one */
					/* extra byte so that caller can */
					/* tell if the path name is too */
					/* long. */
	register char *local_path;	/* Pointer to local path */

#if	BSD42 | BSD43 | ULTRIX
	if ((local_path = index (remote_path, ':')) == (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if ((local_path = strchr (remote_path, ':')) == (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
		local_path = remote_path;
	}
	else local_path++;
	strncpy (pathname, local_path, MAX_REQPATH+2);
	pathname [MAX_REQPATH+1] = '\0';	/* Paranoia */
	return (pathname);
}
