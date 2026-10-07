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
 
/*++ daepres.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/daepres.c,v $
 *
 * DESCRIPTION:
 *
 *	Determine if the local NQS daemon is present.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 3, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:28 $ $State: Exp $)
 * $Log: daepres.c,v $
 * Revision 1.3  1994/11/19  02:26:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:37  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:22  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:47:24  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)daepres.c	1.4 (daepres.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#if	BSD42 | BSD43 | ULTRIX 
#include "nqs.h"			/* NQS definitions */
#include <sys/file.h>
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include "nqspacket.h"			/* NQS local message packets */
#else
BAD SYSTEM TYPE
#endif
#endif
#include "transactcc.h"			/* TCMx_ codes */

#if	BSD42 | BSD43 | ULTRIX
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern long inter();			/* Send message packet */
extern void interclear();		/* Clear packet contents */
#else
BAD SYSTEM TYPE
#endif
#endif

/*** daepres
 *
 *
 *	int daepres():
 *	Determine if the local NQS daemon is present.
 *
 *	Returns:
 *		1: if the local NQS daemon IS present.
 *		0: if the local NQS daemon is NOT present.
 */
#if	BSD42 | BSD43 | ULTRIX
int daepres (runfile)
register struct confd *runfile;		/* Locked exclusively if the NQS */
					/* daemon is running just fine; */
					/* Unlocked if NQS is not running, */
					/* or is shutting down */
{
	if (flock (runfile->fd, LOCK_SH | LOCK_NB) == -1) {
		/*
		 *  The local NQS daemon has the queues file locked
		 *  exclusively with an advisory lock, and is therefore
		 *  running.
		 */
		return (1);		/* Local NQS daemon is running */
	}
	/*
	 *  We were able to successfully apply a shared advisory lock to
	 *  the queues file.  Therefore, the local NQS daemon is either
	 *  shutdown, or in the process of shutting down.
	 *
	 *  Before we return, we must give up the shared advisory lock,
	 *  so that a future incarnation of the resurrected NQS daemon, can
	 *  successfully apply an advisory exclusive lock on the queues
	 *  file.
	 */
	flock (runfile->fd, LOCK_UN);	/* Release the lock */
	return (0);			/* Local NQS daemon is not running */
}
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
int daepres ()
{
	interclear();
	if (inter (PKT_SENSEDAEMON) == TCML_COMPLETE) return (1);
	return (0);			/* The NQS daemon is not running */
}
#else
BAD SYSTEM TYPE
#endif
#endif
