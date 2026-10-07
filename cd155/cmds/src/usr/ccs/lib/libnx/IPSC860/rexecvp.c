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
 
/*
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: rexecvp.c,v $
 * Revision 3.7  1994/11/19  02:33:29  mtm
 * Copyright additions/changes
 *
 * Revision 3.6  1993/07/13  15:20:42  shala
 * New version from LOCUS.
 *
 * Revision 1.3  1993/05/06  19:30:46  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  23:02:56  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:13:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:04:16  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:58:30  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.5  1992/05/05  14:18:45  yazz
 * FIXES BUG #5.
 * An off-by-one bug in string processing fixed.  searchPATH() routine
 * now parses the dollars_PATH string correctly.
 *
 * Revision 3.4  92/05/01  15:38:33  yazz
 * Comment change.
 * 
 * Revision 3.3  92/04/16  21:06:42  yazz
 * Use sys/mode.h include file and fix null PATH entry bug.
 * 
 * Revision 3.2  92/04/14  15:39:07  yazz
 * Handle case where simplecmd string includes slashes (./mycommand or
 * /bin/ls) to conform to the plain execvp() call's behavior.
 * 
 * Revision 3.1  92/04/14  15:00:51  klh
 * Fix RCS Log
 * 
 * $EndLog$
 */
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mode.h>

#ifndef PATH_MAX
# ifdef MAXPATHLEN
#  define PATH_MAX MAXPATHLEN
# else
#  define PATH_MAX 1024
# endif /* MAXPATHLEN */
#endif /* PATH_MAX */

static int searchPATH();

extern char *getenv();
extern char **environ;
extern int errno;


/*
 * Simplecmd is supposed to be a command name without any slashes in it,
 * like "ls", not "/bin/ls", or "mycommand", not "./mycommand".  But we
 * do have to handle the slash, or pathname, case because execvp does.
 * Therefore we use strchr() to check for any slashes in the pathname.
 */

int rexecvp(
	char	*simplecmd,
	char	*argv[],
	int	node)
{
	char	patharray[PATH_MAX + 2];
	int	retval;

	if( strchr(simplecmd, '/') != NULL ) {
		return( rexecve(simplecmd, argv, environ, node) );
	}				/* simplecmd was really a pathname */

	if( (retval=searchPATH(simplecmd, patharray)) != 0 ) {
		errno = retval;
		return( -1 );
	}
	return( rexecve(patharray, argv, environ, node) );
}


/*
 * Search the directories in the PATH environment variable for an
 * executable file named in simplecmd.  Pass back the full pathname
 * in patharray, a character array at least PATH_MAX bytes long.
 *
 * Return value is 0 for success, ENOENT for no file found (including null
 * PATH or simplecmd), and EACCES if only non-executable files were found.
 */

static int
searchPATH(
	char	*simplecmd,		/* points to simple cmd, holds "ls" */
	char	patharray[])		/* pass back array , holds "/bin/ls" */
{
	char	*pathptr;
	char	*p;
	int	retval = ENOENT;	/* assume one won't be found */
	int	len, entrymax;
	int	st;
	int	gid, uid, mymask;
	struct	stat statbuf;

	if( *simplecmd == '\0' )
		{ return( ENOENT ); }	/* 0 len simplecmd */

	pathptr = getenv("PATH");
	if( pathptr == NULL )
		{ return( ENOENT ); }	/* no PATH variable exists */

	/*
	 * For each entry in $PATH, make a full pathname and stat() it.  If
	 * If it exists and is accessible, send the full pathname back to
	 * the caller.  Prevent overflow of caller's patharray by refusing
	 * to generate any full pathnames that would exceed the system max.
	 */
	entrymax = PATH_MAX - strlen(simplecmd) - 2;	/* -2 for '/' & '\0' */
	while( *pathptr != '\0' ) {		/* while any entries left */
		p = patharray;
		len = 0;
		while( *pathptr != ':' && *pathptr != '\0' ) {
			if( ++len < entrymax ) {
				*p++ = *pathptr++;
			}
		}
		if( *pathptr == ':' )	/* pass colon for next time */
			{ ++pathptr; }
		if( len >= entrymax )	/* ignore this entry if too long */
			{ continue; }

		/*
		 * If the current entry from $PATH that we just copied
		 * isn't 0 length (which implies "./") and it doesn't
		 * end with a slash already, put a slash at the end to
		 * separate the directory name entry from the simplecmd.
		 */
		if( len > 0  &&  *(p-1) != '/' )
			*p++ = '/';
		strcpy(p, simplecmd);	/* append simplecmd to pathname */

		st = stat(patharray, &statbuf);
		if( st == -1 )
			{ continue; }	/* no such file */

		/*
		 * Check permissions: any X bit if we're root; otherwise,
		 * only the X bit we qualify for (user, group or other).
		 */
		uid = geteuid();
		gid = getegid();

		if( uid == 0 )
			mymask = (S_IXUSR|S_IXGRP|S_IXOTH);
		else if( uid == statbuf.st_uid )
			mymask = S_IXUSR;
		else if( gid == statbuf.st_gid )
			mymask = S_IXGRP;
		else
			mymask = S_IXOTH;

		if( (statbuf.st_mode & mymask) != 0 ) {
			return( 0 );	/* success.  patharray holds fullpath */
		}
		retval = EACCES;	/* flag we found an inaccessible one */
	}
	return( retval );		/* either EACCES or ENOENT */
}
