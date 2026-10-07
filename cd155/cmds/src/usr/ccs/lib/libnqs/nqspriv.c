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
 
/*++ nqspriv.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/nqspriv.c,v $
 *
 * DESCRIPTION:
 *
 *	Determine the Qmgr privilege bits for the user running the
 *	NQS Qmgr program.  The value returned is the privilege mask
 *	for the user.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:26 $ $State: Exp $)
 * $Log: nqspriv.c,v $
 * Revision 1.3  1994/11/19  02:27:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:50  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:23  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:52:25  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqspriv.c	1.2 (nqspriv.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include <pwd.h>
#include "nqs.h"			/* NQS definitions and types */
#include "nqsxdirs.h"			/* NQS external directories */

extern void closedb();			/* Close an NQS database file */
extern struct gendescr *nextdb();	/* Get the next allocated record */
					/* from an NQS database file */
extern struct confd *opendb();		/* Open the named NQS database file */

/*** nqspriv
 *
 *
 *	nqspriv():
 *	Returns:
 *		The Qmgr privilege bits for the user running the
 *		NQS Qmgr program.  The value returned is the
 *		privilege mask for the user.
 */
int nqspriv (real_uid, user_mid, local_mid)
register uid_t real_uid;		/* REAL user-id of user at */
register mid_t user_mid;		/* machine: machine_id. */
mid_t local_mid;			/* Machine-id of local machine */
{
	register struct confd *file;	/* NQS database file descriptor */
	register struct gendescr *descr;/* Generic descriptor */
	char path [MAX_PATHNAME+1];	/* Pathname of NQS mgr acct file */

	if (real_uid == 0 && user_mid == local_mid) {
		/*
		 *  The caller is running as root on the local machine,
		 *  and thus has all possible NQS Qmgr privileges.
		 */
		return (~0);
	}
	sprintf (path, "%s/%s", Nqs_root, Nqs_mgracct);
	if ((file = opendb (path, O_RDONLY)) == (struct confd *) 0) return (0);
	descr = nextdb (file);		/* Get next allocated record */
	while (descr != (struct gendescr *)0) {
		if (descr->v.mgr.manager_uid == real_uid &&
		    descr->v.mgr.manager_mid == user_mid) {
			closedb (file);
			return (descr->v.mgr.privileges);
					/* Access permitted */
		}
		descr = nextdb (file);
	}
	closedb (file);			/* Access denied */
	return (0);			/* NO NQS privileges */
}
