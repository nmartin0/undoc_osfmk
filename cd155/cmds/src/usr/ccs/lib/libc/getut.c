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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: getut.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:58 $";
#endif
/*
 * FUNCTIONS: getutent, getutid, getutline, pututline, setutent entutent, 
 *            utmpname, gdebug 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * getut.c	1.5  com/lib/c/adm,3.1,8943 9/13/89 10:28:56
 */

/*                                                                    
 * FUNCTION: Routines to read and write the /etc/utmp file.
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - struct utmp on success
 *	     - NULL on failure or EOF
 */

#include	<sys/types.h>
#include	<sys/stat.h>
#include        <sys/param.h>
#include	<utmp.h>
#include	<errno.h>
#include        <time.h>
#include        <unistd.h>
#include	<limits.h>
#ifdef _THREAD_SAFE
#include	"rec_mutex.h"

extern struct rec_mutex _utmp_rmutex;
#endif

#define	FAILURE	-1

#define MAXFILE PATH_MAX /* Maximum pathname length for "utmp" file */

#ifdef	DEBUGX

#undef	UTMP_FILE
#define	UTMP_FILE "utmp"

#endif

static int fd = FAILURE;	/* File descriptor for the utmp file. */
static char utmpfile[MAXFILE+1] = UTMP_FILE;	/* Name of the current
						 * "utmp" like file.
						 */
static long loc_utmp;	/* Where in "utmp" the current "ubuf" was
			 * found.
			 */
static struct utmp ubuf;	/* Copy of last entry read in. */

/*	"getutent" gets the next entry in the utmp file.		*/

#ifdef _THREAD_SAFE
int
getutent_r(struct utmp *utmp)
#else
struct utmp *
getutent()
#endif
{
	register char *u;
	register int i;
	int	err;

	/*
	 * If the "utmp" file is not open, attempt to open it for
	 * reading.  If there is no file, attempt to create one.  If
	 * both attempts fail, return NULL.  If the file exists, but
	 * isn't readable and writeable, do not attempt to create.
	 */
#ifdef _THREAD_SAFE
	if (utmp == NULL) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_utmp_rmutex);
#endif

	if (fd == FAILURE) {
		if ((fd = open(&utmpfile[0],2)) == FAILURE) {
			/* If the open failed because the file didn't exist,
			 * then try to create one.
			 */
#ifdef _THREAD_SAFE
			if ((err = geterrno()) == ENOENT) {
#else
			if ((err = errno) == ENOENT) {
#endif
				if (creat(&utmpfile[0],0644) == FAILURE) {
#ifdef _THREAD_SAFE
					rec_mutex_unlock(&_utmp_rmutex);
					return(-1);
#else
					return(NULL);
#endif
				}
				close(fd);
				if ((fd = open(&utmpfile[0],2)) == FAILURE) {
#ifdef _THREAD_SAFE
					rec_mutex_unlock(&_utmp_rmutex);
					return(-1);
#else
					return(NULL);
#endif
				}
			} else if (err == EACCES) {
				/*
				 * If the open failed for permissions, try
				 * opening it only for reading.  All
				 * "pututline()" later will fail the writes.
				 */

				if ((fd = open(&utmpfile[0],0)) == FAILURE) {
#ifdef _THREAD_SAFE
					rec_mutex_unlock(&_utmp_rmutex);
					return(-1);
#else
					return(NULL);
#endif
				}
			} else {
#ifdef _THREAD_SAFE
				rec_mutex_unlock(&_utmp_rmutex);
				return(-1);
#else
				return(NULL);
#endif
			}
		}
	}

	/*
	 * Try to read in the next entry from the utmp file.  If the
	 * read fails, return NULL.
	 */

	if (read(fd, (char *)&ubuf, (unsigned)sizeof(ubuf)) != sizeof(ubuf)) {
		/*
		 * Make sure ubuf is zeroed.
		 */

		for (i=0,u= (char *)(&ubuf); i < sizeof(ubuf); i++)
			*u++ = '\0';
		loc_utmp = 0;
#ifdef _THREAD_SAFE
		rec_mutex_unlock(&_utmp_rmutex);
		return(-1);
#else
		return(NULL);
#endif
	}

	/*
	 * Save the location in the file where this entry was found.
	 */

	loc_utmp = lseek(fd,0L,1) - (long)(sizeof(struct utmp));
#ifdef _THREAD_SAFE
	if (utmp != &ubuf)
		bcopy(&ubuf, utmp, sizeof(ubuf));
	rec_mutex_unlock(&_utmp_rmutex);
	return(0);
#else
	return(&ubuf);
#endif
}

/*	"getutid" finds the specified entry in the utmp file.  If	*/
/*	it can't find it, it returns NULL.				*/

#ifdef _THREAD_SAFE
int
getutid_r(struct utmp *entry, struct utmp  *result)
#else
struct utmp *
getutid(entry)
register struct utmp *entry;
#endif
{
	register short type;

#ifdef _THREAD_SAFE
	if(entry == NULL || result == NULL) {
		seterrno(EINVAL);
		return(-1);
	}

	rec_mutex_lock(&_utmp_rmutex);
#endif

	/*
	 * Start looking for entry.  Look in our current buffer before
	 * reading in new entries.
	 */

	do {
		/*
		 * If there is no entry in "ubuf", skip to the read.
		 */

		if (ubuf.ut_type != EMPTY) {
			switch(entry->ut_type) {
			case EMPTY :
				/*
				 * Do not look for an entry if the user sent
				 * us an EMPTY entry.
				 */
#ifdef _THREAD_SAFE
				rec_mutex_unlock(&_utmp_rmutex);
				seterrno(EINVAL);
				return(-1);
#else
				return(NULL);
#endif
			case RUN_LVL :
			case BOOT_TIME :
			case OLD_TIME :
			case NEW_TIME :
				/*
				 * For RUN_LVL, BOOT_TIME, OLD_TIME, and
				 * NEW_TIME entries, only the types have to
				 * match.  If they do, return the address of
				 * internal buffer.
				 */

				if (entry->ut_type == ubuf.ut_type) {
#ifdef _THREAD_SAFE
					bcopy(&ubuf, result, sizeof(ubuf));
					rec_mutex_unlock(&_utmp_rmutex);
					return(0);
#else
					return(&ubuf);
#endif
				}
				break;

			case INIT_PROCESS :
			case LOGIN_PROCESS :
			case USER_PROCESS :
			case DEAD_PROCESS :
				/*
				 * For INIT_PROCESS, LOGIN_PROCESS,
				 * USER_PROCESS, and DEAD_PROCESS the type
				 * of the entry in "ubuf", must be one of the
				 * above and id's must match.
				 */
				type = ubuf.ut_type;
				if ((type == INIT_PROCESS || 
				     type == LOGIN_PROCESS || 
				     type == USER_PROCESS  || 
				     type == DEAD_PROCESS) &&
				     (strcmp (ubuf.ut_id, entry->ut_id) == 0)) {
#ifdef _THREAD_SAFE
					bcopy(&ubuf, result, sizeof(ubuf));
					rec_mutex_unlock(&_utmp_rmutex);
					return(0);

#else
					return(&ubuf);
#endif
				}
				break;
			default :
				/*
				 * Do not search for illegal types of entry.
				 */
#ifdef _THREAD_SAFE
				rec_mutex_unlock(&_utmp_rmutex);
				seterrno(EINVAL);
				return(0);
#else
				return(NULL);
#endif
			}
		}
#ifdef _THREAD_SAFE
	} while (getutent_r(&ubuf) == 0);
#else
	} while (getutent() != NULL);
#endif

	/*
	 * Return failure since the proper entry wasn't found.
	 */
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_utmp_rmutex);
	seterrno(ESRCH);
	return(-1);
#else
	return(NULL);
#endif
}

/*	"getutline" searches the "utmp" file for a LOGIN_PROCESS or	*/
/*	USER_PROCESS with the same "line" as the specified "entry".	*/

#ifdef _THREAD_SAFE
int
getutline_r(struct utmp *entry, struct utmp *result)
#else
struct utmp *
getutline(entry)
register struct utmp *entry;
#endif
{
	register struct utmp *cur;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_utmp_rmutex);
#endif
	/*
	 * Start by using the entry currently incore.  This prevents
	 * doing reads that aren't necessary.
	 */

	cur = &ubuf;
	do {
		/*
		 * If the current entry is the one we are interested in, return
		 * a pointer to it.
		 */

		if (cur->ut_type != EMPTY && (cur->ut_type == LOGIN_PROCESS ||
		    cur->ut_type == USER_PROCESS) &&
		    strncmp(entry->ut_line, cur->ut_line,sizeof(cur->ut_line)) == 0) {
#ifdef _THREAD_SAFE
			bcopy(&ubuf, result, sizeof(ubuf));
			rec_mutex_unlock(&_utmp_rmutex);
			return(0);
#else
			return(cur);
#endif
		}
#ifdef _THREAD_SAFE
	} while ((getutent_r(cur)) == 0);
#else
	} while ((cur = getutent()) != NULL);
#endif

	/*
	 * Since entry wasn't found, return NULL.
	 */
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_utmp_rmutex);
	seterrno(ESRCH);
	return(-1);
#else
	return(NULL);
#endif
}

/*	"pututline" writes the structure sent into the utmp file.	*/
/*	If there is already an entry with the same id, then it is	*/
/*	overwritten, otherwise a new entry is made at the end of the	*/
/*	utmp file.							*/

#ifdef _THREAD_SAFE
int
pututline_r(struct utmp *entry)
#else
struct utmp *
pututline(entry)
struct utmp *entry;
#endif
{
	char utmplock[MAXFILE+1];
	struct stat statbuf;
	int	err;
#ifndef _THREAD_SAFE
	struct utmp *answer;
	struct utmp tmpbuf;
#else
	struct utmp result;
#endif

#ifndef _THREAD_SAFE
	/*
	 * Copy the user supplied entry into our temporary buffer to
	 * avoid the possibility that the user is actually passing us
	 * the address of "ubuf".
	 */

	tmpbuf = *entry;
#endif

	/*
	 * Create a lock link file before attempting to modify the utmp
	 * file.  Wait for up to sixty seconds, but then continue
	 * regardless of the lock file. If sixty seconds isn't enough time,
	 * the original creator of the lock probably has died.
	 */

	sprintf(utmplock, "%s.lck", utmpfile);

	while (link(utmpfile, utmplock) == FAILURE) {
#ifdef _THREAD_SAFE
		if ((err = geterrno()) == EEXIST) {
#else
		if ((err = errno) == EEXIST) {
#endif
			if (stat(utmpfile, &statbuf) != FAILURE) {
				if (time((time_t *)0) - statbuf.st_ctime > 60)
					unlink(utmplock);
				else
					sleep((unsigned)3);
			}
		} else if (err == ENOENT) {
			/*
			 * If the utmp file doesn't exist, make one by trying
			 * to find the entry of interest.  "getutent()" will
			 * create the file.
			 */
#ifdef _THREAD_SAFE
			rec_mutex_lock(&_utmp_rmutex);
			(void)getutent_r(&ubuf);
#else
			(void)getutent();
#endif
			if (fd == FAILURE) {
#ifdef _THREAD_SAFE
				rec_mutex_unlock(&_utmp_rmutex);
				return(-1);
#else
				return((struct utmp *)NULL);
#endif
			}
#ifdef _THREAD_SAFE
			rec_mutex_unlock(&_utmp_rmutex);
#endif
		} else {
#ifdef _THREAD_SAFE
			return(-1);
#else
			return((struct utmp *)NULL);
#endif
		}
	}

	/*
	 * Find the proper entry in the utmp file.  Start at the current
	 * location.  If it isn't found from here to the end of the
	 * file, then reset to the beginning of the file and try again.
	 * If it still isn't found, then write a new entry at the end of
	 * the file.  (Making sure the location is an integral number of
	 * utmp structures into the file incase the file is scribbled.)
	 */

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_utmp_rmutex);
	if (getutid_r(entry, &result) != 0) {
#else
	if (getutid(&tmpbuf) == NULL) {
#endif
		setutent();
#ifdef _THREAD_SAFE
		if (getutid_r(entry, &result) != 0) {
#else
		if (getutid(&tmpbuf) == NULL) {
#endif
			loc_utmp = lseek(fd,0L,1);
			loc_utmp -= loc_utmp % sizeof(struct utmp);
	  	}
	}

	/*
	 * Seek to the proper place on the file descriptor for writing.
	 */

	lseek(fd,loc_utmp,0);

	/*
	 * Write out the user supplied structure.  If the write fails,
	 * then the user probably doesn't have permission to write the
	 * utmp file.
	 */

#ifdef _THREAD_SAFE
	if (write(fd, (char *)entry, (unsigned)sizeof(struct utmp)) !=
#else
	if (write(fd, (char *)&tmpbuf, (unsigned)sizeof(tmpbuf)) !=
#endif
			sizeof(struct utmp)) {
#ifdef _THREAD_SAFE
		err = -1;
#else
		answer = (struct utmp *)NULL;
#endif
	} else {
		/*
		 * Copy the user structure into ubuf so that it will be up to
		 * date in the future.
		 */

#ifdef _THREAD_SAFE
		ubuf = *entry;
		err = 0;
#else
		ubuf = tmpbuf;
		answer = &ubuf;
#endif

	}

	/*
	 * Remove the lock file (even if it wasn't your own creation).
	 */

	unlink(&utmplock[0]);
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_utmp_rmutex);
	return(err);
#else
	return(answer);
#endif
}

/*	"setutent" just resets the utmp file back to the beginning.	*/

void
setutent()
{
	register char *ptr;
	register int i;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_utmp_rmutex);
#endif

	if (fd != FAILURE)
		lseek(fd,0L,0);

	/*
	 * Zero the stored copy of the last entry read, since we are
	 * resetting to the beginning of the file.
	 */

	for (i=0,ptr=(char*)&ubuf; i < sizeof(ubuf);i++)
		*ptr++ = '\0';
	loc_utmp = 0L;

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_utmp_rmutex);
#endif
}

/*	"endutent" closes the utmp file.				*/

void
endutent()
{
	register char *ptr;
	register int i;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_utmp_rmutex);
#endif

	if (fd != FAILURE)
		close(fd);
	fd = FAILURE;
	loc_utmp = 0;
	for (i=0,ptr= (char *)(&ubuf); i < sizeof(ubuf);i++)
		*ptr++ = '\0';

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_utmp_rmutex);
#endif
}

/*	"utmpname" allows the user to read a file other than the	*/
/*	normal "utmp" file.						*/

void
utmpname(newfile)
char *newfile;
{
	/*
	 * Determine if the new filename will fit.  If not, return FALSE.
	 */
	if (strlen(newfile) > MAXFILE)
		return;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_utmp_rmutex);
#endif

	/* copy in the new file name.  */
	strcpy(utmpfile, newfile);

	/*
	 * Make sure everything is reset to the beginning state.
	 */
	endutent();

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_utmp_rmutex);
#endif
}
