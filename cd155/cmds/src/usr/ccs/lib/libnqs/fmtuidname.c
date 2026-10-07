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
 
/*++ fmtuidname.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/fmtuidname.c,v $
 *
 * DESCRIPTION:
 *
 *	Return a pointer to the username of the account
 *	identified by the local user-id.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 29, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:45 $ $State: Exp $)
 * $Log: fmtuidname.c,v $
 * Revision 1.3  1994/11/19  02:26:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:10  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:37  root
 * Version 2.0 Source
 * 
 * Revision 2.3  87/08/25  16:53:26  hender
 * 1. deleted last reminate of nmap; 2. uid_t and gid_t are now defined
 *    in /usr/netinclude/sys/types.h (4.3)
 * ,
 * 
 * Revision 2.2  87/04/22  14:48:51  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)fmtuidname.c	1.2 (fmtuidname.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#if	BSD43 | UTS | SGI | OSF
#include <sys/types.h>			/* Uid_t, gid_t */
#else
#if	BSD42 | SYS52 | ULTRIX | UNICOS 
#else
BAD SYSTEM TYPE
#endif
#endif
#include NMAP				/* Mid_t (all OS's) */
					/* Uid_t and gid_t (if not BSD43) */
#include <pwd.h>			/* Password/account definitions */

/*
 *	External functions.
 */
extern struct passwd *fetchpwuid();
				/* Fetch local password entry for */
				/* loccal user-id */


/*** fmtuidname
 *
 *
 *	char *fmtuidname():
 *
 *	Return a pointer to the username of the account
 *	identified by the local user-id.
 */
char *fmtuidname (uid)
uid_t uid;				/* Local user-id */
{
	static char standinbuf [14];	/* Stand-in buffer */

	register char *name;		/* Pointer to name */
	register struct passwd *pw;	/* Password/account entry */

	pw = fetchpwuid ((int) uid);	/* Get account entry */
	if (pw == (struct passwd *) 0) {
		/*
		 *  No such user!  Format a "stand-in" name.
		 */
		sprintf (standinbuf, "[%1d]", (int) uid);
		name = standinbuf;
	}
	else name = pw->pw_name;
	return (name);
}
