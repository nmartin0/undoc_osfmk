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
 
/*++ grpnam.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/grpnam.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the two functions:  getgrpnam()  and
 *	endgrpnam() which return the group-name for the specified
 *	group-id, and discard the group-name cache respectively.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:54 $ $State: Exp $)
 * $Log: grpnam.c,v $
 * Revision 1.3  1994/11/19  02:26:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:36  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:48  root
 * Version 2.0 Source
 * 
 * Revision 2.3  87/08/25  16:53:32  hender
 * 1. deleted last reminate of nmap; 2. uid_t and gid_t are now defined
 *    in /usr/netinclude/sys/types.h (4.3)
 * ,
 * 
 * Revision 2.2  87/04/22  14:50:06  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)grpnam.c	1.2 (grpnam.c OSF/1 NQS2.0 GJK) 6/30/92";
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


/*
 *	Configurable parameters.
 */
#define	MAX_G_CACHESIZE	50	/* We will remember this many group names */

/*
 *	External functions.
 */
extern void closegrdb();	/* Close the group database */
extern char *fmtgidname();	/* Format group-name for local gid */
extern void free();		/* Free dynamic memory */
extern char *malloc();		/* Allocate dynamic memory */
extern char *strcpy();		/* String copy */
extern int strlen();		/* String length */


/*
 *	Data structure definitions local to this module.
 */
struct g_cache {
	struct g_cache *prev;	/* Previous group name cache entry */
	struct g_cache *next;	/* Next group name cache entry */
	gid_t gid;		/* Group-id */
	char *name;		/* Group-name */
};


/*
 *	Variables which are global to this module but not public.
 */
static struct g_cache *g_set = (struct g_cache *) 0;
					/* Group-id/name cache */
static int g_count = 0;			/* # of group-id/name cache entries */


/*** getgrpnam
 *
 *
 *	char *getgrpnam():
 *
 *	Return the groupname for the specified group-id on the local
 *	machine.
 */
char *getgrpnam (gid)
gid_t gid;				/* Group-id */
{
	struct g_cache *newgroup();	/* Allocate a new group cache entry */

	register struct g_cache *scan;	/* Current group cache entry */
	register struct g_cache *prev;	/* Previous group cache entry */
	register char *groupname;	/* Ptr to groupname for local gid */

	prev = (struct g_cache *) 0;
	scan = g_set;			/* Scan group cache */
	while (scan != (struct g_cache *) 0 && scan->gid != gid) {
		prev = scan;
		scan = scan->next;
	}
	if (scan == (struct g_cache *) 0) {
		/*
		 *  The group-id/name was not in the cache.
		 */
		groupname = fmtgidname (gid);	/* Format groupname for gid */
		if (g_count < MAX_G_CACHESIZE) scan = newgroup (groupname);
		while (scan == (struct g_cache *) 0 &&
		       prev != (struct g_cache *) 0) {
			/*
			 *  Discard the least recently used mapping and
			 *  try to add the new mapping to the head of
			 *  the mapping set.
			 */
			free (prev->name);	/* Dispose of LRU name part */
			scan = prev;
			prev = prev->prev;	/* Backup one entry */
			free ((char *) scan);	/* Dispose of LRU entry */
			g_count--;		/* One less entry */
			if (prev != (struct g_cache *) 0) {	/* Disconnect */
				prev->next = (struct g_cache *) 0;
			}
			else {			/* No more entries left */
				g_set = (struct g_cache *) 0;
			}
			scan = newgroup (groupname);
						/* Try to allocate new entry */
		}
		if (scan == (struct g_cache *) 0) {
			/*
			 *  Insufficient memory existed to add the mapping
			 *  cache entry.  g_set points to nothing.
			 */
			return (groupname);
		}
		/*
		 *  Add the new mapping to the head of the mapping cache.
		 */
		if (g_set != (struct g_cache *) 0) g_set->prev = scan;
		scan->prev = (struct g_cache *) 0;
		scan->next = g_set;
		g_set = scan;
		scan->gid = gid;			/* Save group-id */
		strcpy (scan->name, groupname);		/* Save group-name */
	}
	else {
		/*
		 *  The group-id/name pair has been found in the cache.
		 *  Move the entry to the front of the cache to keep track
		 *  of the least-recently used order of the cache entries.
		 */
		if (scan != g_set) {	/* If not already as the front.... */
			if (prev != (struct g_cache *) 0) {
				prev->next = scan->next;
			}
			if (scan->next != (struct g_cache *) 0) {
				scan->next->prev = prev;
			}
			scan->prev = (struct g_cache *) 0;
			scan->next = g_set;
			g_set = scan;
		}
	}
	return (scan->name);	/* Return ptr to groupname */
}


/*** endgrpnam
 *
 *
 *	void endgrpnam():
 *	Clear the group-id/name cache.
 */
void endgrpnam ()
{
	register struct g_cache *walk;
	register struct g_cache *next;

	walk = g_set;
	while (walk != (struct g_cache *) 0) {
		next = walk->next;
		free (walk->name);
		free ((char *) walk);
		walk = next;
	}
	g_count = 0;			/* Zero group-id/name cache entries */
	g_set = (struct g_cache *) 0;
	closegrdb();			/* Close the group database */
}


/*** newgroup
 *
 *
 *	struct g_cache *newgroup():
 *	Returns:
 *		A pointer to a new group-id/name cache entry if
 *		adequate heap space exists; otherwise a null ptr
 *		is returned.
 */
static struct g_cache *newgroup (name)
char *name;					/* Groupname */
{
	register struct g_cache *new;

	if ((new = (struct g_cache *)
		   malloc (sizeof (struct g_cache))) != (struct g_cache *) 0) {
		/*
		 *  We successfully got a new cache entry.
		 *  Now try to allocate sufficient name space.
		 */
		if ((new->name = malloc (strlen (name) + 1)) == (char *) 0) {
			/*
			 *  Insufficient heap space for name.
			 */
			free ((char *) new);
			new = (struct g_cache *) 0;
		}
		else g_count++;	/* One more entry */
	}
	return (new);
}
