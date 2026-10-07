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
 
/*++ opendb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/opendb.c,v $
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:28 $ $State: Exp $)
 * $Log: opendb.c,v $
 * Revision 1.3  1994/11/19  02:27:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:57  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:27  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:52:46  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)opendb.c	1.2 (opendb.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include "nqs.h"
#include <sys/stat.h>
#define	MAX_CONFIGFILES	9		/* Maximum number of open configur- */
					/* ation files */

extern int errno;			/* System call error number */
extern char *malloc();			/* Allocate heap space */

/*** opendb
 *
 *
 *	struct confd *opendb():
 *
 *	Open the named file of the NQS database.
 *	Returns:
 *		A pointer to the allocated NQS database file access
 *		structure if successful;
 *		Otherwise NULL is returned (errno has reason).
 */
struct confd *opendb (name, flags)
register char *name;			/* Name of file */
register int flags;			/* file access flags */
{
	static short initialized = 0;	/* Tables not initialized */
	static struct confd confiles [MAX_CONFIGFILES];

	struct stat statbuf;		/* Fstat() buffer */
	register struct confd *file;	/* NQS database file descriptor */
	register int descr;

	if (!initialized) {
		/*
		 *  We have not yet initialized the configuration
		 *  file descriptor table.
		 */
		initialized = 1;	/* Set initialized flag */
		for (descr = 0; descr < MAX_CONFIGFILES; descr++) {
			confiles [descr].fd = -1;	/* Mark slot as free */
		}
	}
	descr = 0;		/* Search for a free confd descriptor */
	while (descr < MAX_CONFIGFILES && confiles [descr].fd != -1) {
		descr++;
	}
	if (descr >= MAX_CONFIGFILES) {
		/*
		 *  There is not a free confd file descriptor
		 *  to be found.
		 */
		printf ("E$Too many files in opendb().\n");
		fflush (stdout);
		errno = 0;		/* Not a system call error */
		return (NULL);
	}
	file = &confiles [descr];	/* Get address of free descriptor */
	if (flags & O_CREAT) file->fd = open (name, flags & ~O_EXCL, 0644);
	else file->fd = open (name, flags);
	if (file->fd == -1) return (NULL);	/* The open failed */
	/*
	 *  The open succeeded.  Initialize confd file descriptor.
	 */
	if ((file->cache = (struct rawblock *)
			    malloc (sizeof (struct rawblock))) == NULL) {
		/*
		 *  Insufficient heap space to open database file.
		 */
		printf ("E$No heap space in opendb().\n");
		fflush (stdout);
		close (file->fd);	/* Close the file */
		file->fd = -1;		/* Free the descriptor */
		return (NULL);
	}
	file->vposition = 0;		/* Virtual file position */
	file->rposition = 0;		/* Real file position */
	file->lastread = -1;		/* No descriptor has even been read */
	fstat (file->fd, &statbuf);	/* Fstat() file */
	file->size = statbuf.st_size;	/* File size */
	if (file->size % ATOMICBLKSIZ) {
		/*
		 *  The file size is not a multiple of ATOMICBLKSIZ bytes.
		 */
		printf ("E$Invalid file size in opendb().\n");
		fflush (stdout);
		close (file->fd);	/* Close the file */
		file->fd = -1;		/* Free the descriptor */
		return (NULL);
	}
	/*
	 *  Nature abhors a vacuum.  Load the first block of the file
	 *  into the cache (if the file has 1 or more blocks).
	 */
	if (file->size) {
		errno = 0;		/* No system call error on partial */
					/* reads please */
		if (read (file->fd, file->cache->v.chars,
			  ATOMICBLKSIZ) != ATOMICBLKSIZ) {
			printf ("E$Read error in opendb().\n");
			fflush (stdout);
			close (file->fd);	/* Close the file */
			file->fd = -1;		/* Free the descriptor */
			return (NULL);
		}
		file->rposition += ATOMICBLKSIZ;
	}
	return (file);
}
