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
 
/*++ setmtime.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/setmtime.c,v $
 *
 * DESCRIPTION:
 *
 *	Please read the DESCRIPTION section of ../lib/transact.c
 *	to understand why this module exists.  This module allows
 *	us to implement a minimum transaction mechanism on top
 *	of UNIX implementations that do not support synchronous
 *	write file I/O (where when you return from the system call,
 *	the permanent memory associated with the write operation
 *	HAS ACTUALLY BEEN UPDATED--as opposed to sitting in some
 *	buffer cache....).
 *
 *	Again, see the comments in ../lib/transact.c.  There
 *	are also explanatory comments in ../src/nqs_ldconf.c.
 *
 *	The transaction implementation for UNIX implementations
 *	of the variety mentioned in the first paragraph, requires
 *	a procedure to synchronously update a file inode.  That
 *	is to say, upon successful return from this procedure,
 *	the inode of the named file will have been PHYSICALLY
 *	updated in the file system.
 *
 *	All UNIX implementations known to the author (Silicon
 *	Graphics, Berkeley, System V, UTS, and UNICOS) perform
 *	a synchronous update of the target file-inode (the file
 *	inode to link count is being incremented) referenced
 *	by a link(2) system call.
 *
 *	We exploit this property of the link(2) system call
 *	to great advantage in some of the transaction mecha-
 *	nisms implemented within NQS.
 *
 *	I really hate this function, but I need it until UNIX
 *	gets smarter.  Sorry.
 *
 *	This procedure is used by ../lib/transact.c, ../src/nqs_udb.c,
 *	and by ../src/nqs_ldconf.c.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 20, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:51 $ $State: Exp $)
 * $Log: setmtime.c,v $
 * Revision 1.3  1994/11/19  02:27:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:56  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:58  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:55:33  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)setmtime.c	1.2 (setmtime.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nqs.h"			/* NQS types and definitions */
					/* (and also sys/types.h) */
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
struct utimbuf {
	time_t acctime;			/* Access time */
	time_t modtime;			/* Modification time */
};
#else
BAD SYSTEM TYPE
#endif
#endif

extern char *strcat();			/* String concatenation */
extern char *strcpy();			/* String copy */
extern time_t time();			/* Get current time */

/*** setmtime
 *
 *
 *	int setmtime():
 *
 *	Set the modification time of the specified file using a synchronous
 *	algorithm.  (The access time of the file is set to the current time.)
 *	Upon successful return, the image of the inode will have been
 *	physically updated in the file system.
 *
 *	The total length of the file pathname must be LESS than MAX_PATHNAME,
 *	and the filename at the end of the pathname must be less than or equal
 *	to 9 characters in length, so that the linkname used to perform the
 *	synchronous update can be created by appending "_LINK" to the end
 *	of pathname (without overflowing the 14-character limit on System V
 *	based UNIX implementations).
 *
 *	Furthermore, the linkname must not conflict with some other file
 *	already existing in the same directory.
 *
 *	Returns:
 *		 0: if successful.
 *		-1: if unsuccessful (errno has reason).
 */
int setmtime (path, ulongval)
char *path;				/* Pathname of file */
unsigned long ulongval;			/* Unsigned long mtime value */
{
#if	BSD42 | BSD43 | ULTRIX
	struct timeval utimbuf [2];	/* Utimes() buffer */
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	struct utimbuf utimbuf;		/* utime() buffer */
#else
BAD SYSTEM TYPE
#endif
#endif
	char linkpath [MAX_PATHNAME+1];	/* Link name for ../database/seqno */
	
	/*
	 *  Remove any left-over synchronous link path.
	 */
	strcpy (linkpath, path);
	strcat (linkpath, "_LINK");
#if	BSD42 | BSD43 | ULTRIX
	time (&utimbuf [0].tv_sec);	/* Access time = current time */
	utimbuf [0].tv_usec = 0;
	utimbuf [1].tv_sec = ulongval;
	utimbuf [1].tv_usec = 0;
	if (utimes (path, utimbuf) == -1) return (-1);
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	time (&utimbuf.acctime);	/* Access time = current time */
	utimbuf.modtime = ulongval;
	if (utime (path, &utimbuf) == -1) return (-1);
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Now, make a link to the target inode to force the synchronous
	 *  writing of the inode to disk.
	 */
	if (link (path, linkpath) == -1) {
		/*
		 *  The link failed.  It may have failed because the
		 *  synchronous link-name was left over from a previous
		 *  system crash.  Try unlinking the link name, and
		 *  then try again....
		 */
		unlink (linkpath);
		if (link (path, linkpath) == -1) {
			/*
			 *  The link still fails.
			 */
			return (-1);
		}
	}
	/*
	 *  Now, dispose of the synchronous link name path, to avoid
	 *  cluttering up the transaction database subdirectory with
	 *  extraneous names (which would otherwise extend the size
	 *  of the directory unnecessarily--slowing everyone down).
	 */
	unlink (linkpath);		/* Unlink synchronous link */
	return (0);			/* Success */
}
