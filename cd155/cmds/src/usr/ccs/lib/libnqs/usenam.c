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
 
/*++ usenam.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/usenam.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the two functions:  getusenam()  and
 *	endusenam() which return the user-name account for the specified
 *	user-id, and discard the user-name cache respectively.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:28:03 $ $State: Exp $)
 * $Log: usenam.c,v $
 * Revision 1.3  1994/11/19  02:28:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:00:04  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:19  root
 * Version 2.0 Source
 * 
 * Revision 2.3  87/08/25  16:53:36  hender
 * 1. deleted last reminate of nmap; 2. uid_t and gid_t are now defined
 *    in /usr/netinclude/sys/types.h (4.3)
 * ,
 * 
 * Revision 2.2  87/04/22  14:56:58  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)usenam.c	1.2 (usenam.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#if	BSD43 | UTS | SGI | OSF

#if OSF
#include "sys_types_OSF.h"      /* Uid_t, gid_t */ /* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>
#endif

#include <sys/types.h>			/* Uid_t, gid_t */
#else
#if	BSD42 | SYS52 | ULTRIX | UNICOS
#else
BAD SYSTEM TYPE
#endif
#endif
#include NMAP				/* Mid_t (all OS's) */
					/* Uid_t and gid_t (if not BSD43) */

/*
 *	Configurable parameters.
 */
#define	MAX_U_CACHESIZE	400	/* We will remember this many user names */

/*
 *	External functions.
 */
extern void closepwdb();	/* Close the account/password database */
extern char *fmtuidname();	/* Format name for local user-id */
extern void free();		/* Free dynamic memory */
extern char *malloc();		/* Allocate dynamic memory */
extern char *strcpy();		/* String copy */
extern int strlen();		/* String length */


/*
 *	Data structure definitions local to this module.
 */
struct u_cache {
	struct u_cache *prev;	/* Previous user name cache entry */
	struct u_cache *next;	/* Next user name cache entry */
	uid_t uid;		/* User-id */
	char *name;		/* User-name */
};


/*
 *	Variables which are global to this module but not public.
 */
static struct u_cache *u_set = (struct u_cache *) 0;
					/* User-id/name cache */
static int u_count = 0;			/* # of user-id/name cache entries */


/*** getusenam
 *
 *
 *	char *getusenam():
 *
 *	Return the username for the specified user-id on the local
 *	machine.
 */
char *getusenam (uid)
uid_t uid;				/* User-id */
{
	struct u_cache *newuser();	/* Allocate a new user cache entry */

	register struct u_cache *scan;	/* Current user cache entry */
	register struct u_cache *prev;	/* Previous user cache entry */
	register char *name;		/* Name for local user-id */

	prev = (struct u_cache *) 0;
	scan = u_set;			/* Scan user cache */
	while (scan != (struct u_cache *) 0 && scan->uid != uid) {
		prev = scan;
		scan = scan->next;
	}
	if (scan == (struct u_cache *) 0) {
		/*
		 *  The user-id/name was not in the cache.
		 */
		name = fmtuidname (uid);
		if (u_count < MAX_U_CACHESIZE) scan = newuser (name);
		while (scan == (struct u_cache *) 0 &&
		       prev != (struct u_cache *) 0) {
			/*
			 *  Discard the least recently used mapping and
			 *  try to add the new mapping to the head of
			 *  the mapping set.
			 */
			free (prev->name);	/* Dispose of LRU name part */
			scan = prev;
			prev = prev->prev;	/* Backup one entry */
			free ((char *) scan);	/* Dispose of LRU entry */
			u_count--;		/* One less entry */
			if (prev != (struct u_cache *) 0) {	/* Disconnect */
				prev->next = (struct u_cache *) 0;
			}
			else {			/* No more entries left */
				u_set = (struct u_cache *) 0;
			}
			scan = newuser (name);
						/* Try to allocate new entry */
		}
		if (scan == (struct u_cache *) 0) {
			/*
			 *  Insufficient memory existed to add the mapping
			 *  cache entry.  u_set points to nothing.
			 */
			return (name);
		}
		/*
		 *  Add the new mapping to the head of the mapping cache.
		 */
		if (u_set != (struct u_cache *) 0) u_set->prev = scan;
		scan->prev = (struct u_cache *) 0;
		scan->next = u_set;
		u_set = scan;
		scan->uid = uid;		/* Save user-id */
		strcpy (scan->name, name);	/* Save user-name */
	}
	else {
		/*
		 *  The user-id/name pair has been found in the cache.
		 *  Move the entry to the front of the cache to keep track
		 *  of the least-recently used order of the cache entries.
		 */
		if (scan != u_set) {	/* If not already as the front.... */
			if (prev != (struct u_cache *) 0) {
				prev->next = scan->next;
			}
			if (scan->next != (struct u_cache *) 0) {
				scan->next->prev = prev;
			}
			scan->prev = (struct u_cache *) 0;
			scan->next = u_set;
			u_set = scan;
		}
	}
	return (scan->name);	/* Return ptr to username */
}


/*** endusenam
 *
 *
 *	void endusenam():
 *	Clear the user-id/name cache.
 */
void endusenam ()
{
	register struct u_cache *walk;
	register struct u_cache *next;

	walk = u_set;
	while (walk != (struct u_cache *) 0) {
		next = walk->next;
		free (walk->name);
		free ((char *) walk);
		walk = next;
	}
	u_count = 0;		/* Zero user-id/name cache entries */
	u_set = (struct u_cache *) 0;
	closepwdb();		/* Close the account/password database */
}


/*** newuser
 *
 *
 *	struct u_cache *newuser():
 *	Returns:
 *		A pointer to a new user-id/name cache entry if
 *		adequate heap space exists; otherwise a null ptr
 *		is returned.
 */
static struct u_cache *newuser (name)
char *name;					/* Username */
{
	register struct u_cache *new;

	if ((new = (struct u_cache *)
		   malloc (sizeof (struct u_cache))) != (struct u_cache *) 0) {
		/*
		 *  We successfully got a new cache entry.
		 *  Now try to allocate sufficient name space.
		 */
		if ((new->name = malloc (strlen (name) + 1)) == (char *) 0) {
			/*
			 *  Insufficient heap space for name.
			 */
			free ((char *) new);
			new = (struct u_cache *) 0;
		}
		else u_count++;	/* One more entry */
	}
	return (new);
}
