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
 
/*++ fmtgidname.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/fmtgidname.c,v $
 *
 * DESCRIPTION:
 *
 *	Return a pointer to the groupname of the group
 *	identified by the local group-id.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 25, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:40 $ $State: Exp $)
 * $Log: fmtgidname.c,v $
 * Revision 1.3  1994/11/19  02:26:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:05  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:34  root
 * Version 2.0 Source
 * 
 * Revision 2.3  87/08/25  16:53:16  hender
 * 1. deleted last reminate of nmap; 2. uid_t and gid_t are now defined
 *    in /usr/netinclude/sys/types.h (4.3)
 * ,
 * 
 * Revision 2.2  87/04/22  14:48:36  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)fmtgidname.c	1.2 (fmtgidname.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#if	BSD43 | UTS | SGI | OSF

#if OSF
#include "sys_types_OSF.h"      /* Uid_t, gid_t */ /* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>			/* Uid_t, gid_t */
#endif

#else
#if	BSD42 | SYS52 | ULTRIX | UNICOS 
#else
BAD SYSTEM TYPE
#endif
#endif
#include NMAP				/* Mid_t (all OS's) */
					/* Uid_t and gid_t (if not BSD43) */
#include <grp.h>			/* Group database definitions */

/*
 *	External functions.
 */
extern struct group *fetchgrgid();
				/* Fetch local group entry for local */
				/* group-id */


/*** fmtgidname
 *
 *
 *	char *fmtgidname():
 *
 *	Return a pointer to the groupname of the group
 *	identified by the local group-id.
 */
char *fmtgidname (gid)
gid_t gid;				/* Local group-id */
{
	static char standinbuf [14];	/* Stand-in buffer */

	register char *name;		/* Pointer to name */
	register struct group *group;	/* Group entry */

	group = fetchgrgid ((int) gid);	/* Get group entry */
	if (group == (struct group *) 0) {
		/*
		 *  No such group!  Format a "stand-in" name.
		 */
		sprintf (standinbuf, "[%1d]", (int) gid);
		name = standinbuf;
	}
	else name = group->gr_name;
	return (name);
}
