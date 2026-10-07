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
 
/*++ macnam.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/macnam.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the two functions:  getmacnam()  and
 *	endmacnam() which return the principal name of the machine
 *	for the specified machine-id, and discard the machine-name
 *	cache respectively.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:11 $ $State: Exp $)
 * $Log: macnam.c,v $
 * Revision 1.3  1994/11/19  02:27:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:08  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:07  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:17  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)macnam.c	1.2 (macnam.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include NMAP			/* Include network mapping definitions */


/*
 *	Configurable parameters.
 */
#define	MAX_M_CACHESIZE	200	/* We will remember this many machine names */

/*
 *	External functions.
 */
extern char *fmtmidname();	/* Format name for machine-id */
extern void free();		/* Free dynamic memory */
extern char *malloc();		/* Allocate dynamic memory */
extern char *strcpy();		/* String copy */
extern int strlen();		/* String length */


/*
 *	Data structure definitions local to this module.
 */
struct m_cache {
	struct m_cache *prev;	/* Previous machine name cache entry */
	struct m_cache *next;	/* Next machine name cache entry */
	mid_t mid;		/* Machine-id */
	char *name;		/* Machine principal name for mid */
};


/*
 *	Variables which are global to this module but not public.
 */
static struct m_cache *m_set = (struct m_cache *) 0;
					/* Machine-id/name cache */
static int m_count = 0;			/* # of machine-id/name cache entries */


/*** getmacnam
 *
 *
 *	char *getmacnam():
 *
 *	Return the principal name of the machine whose machine-id
 *	is: mid.
 */
char *getmacnam (mid)
mid_t mid;				/* Machine-id */
{
	struct m_cache *newmach();	/* Allocate a new machine cache entry */
	register struct m_cache *scan;	/* Current machine cache entry */
	register struct m_cache *prev;	/* Previous machine cache entry */
	register char *name;		/* Machine-name */

	prev = (struct m_cache *) 0;
	scan = m_set;			/* Scan machine cache */
	while (scan != (struct m_cache *) 0 && scan->mid != mid) {
		prev = scan;
		scan = scan->next;
	}
	if (scan == (struct m_cache *) 0) {
		/*
		 *  The machine-id/name was not in the cache.
		 */
		name = fmtmidname (mid);	/* Format name */
		if (m_count < MAX_M_CACHESIZE) scan = newmach (name);
		while (scan == (struct m_cache *) 0 &&
		       prev != (struct m_cache *) 0) {
			/*
			 *  Discard the least recently used mapping and
			 *  try to add the new mapping to the head of
			 *  the mapping set.
			 */
			free (prev->name);	/* Dispose of LRU name part */
			scan = prev;
			prev = prev->prev;	/* Backup one entry */
			free ((char *) scan);	/* Dispose of LRU entry */
			m_count--;		/* One less entry */
			if (prev != (struct m_cache *) 0) {	/* Disconnect */
				prev->next = (struct m_cache *) 0;
			}
			else {			/* No more entries left */
				m_set = (struct m_cache *) 0;
			}
			scan = newmach (name);	/* Try to allocate new entry */
		}
		if (scan == (struct m_cache *) 0) {
			/*
			 *  Insufficient memory existed to add the mapping
			 *  cache.  m_set points to nothing.
			 */
			return (name);
		}
		/*
		 *  Add the new mapping to the head of the mapping cache.
		 */
		if (m_set != (struct m_cache *) 0) m_set->prev = scan;
		scan->prev = (struct m_cache *) 0;
		scan->next = m_set;
		m_set = scan;
		scan->mid = mid;		/* Save machine-id */
		strcpy (scan->name, name);	/* Save machine-name */
	}
	else {
		/*
		 *  The machine-id/name pair has been found in the cache.
		 *  Move the entry to the front of the cache to keep track
		 *  of the least-recently used order of the cache entries.
		 */
		if (scan != m_set) {	/* If not already at the front... */
			if (prev != (struct m_cache *) 0) {
				prev->next = scan->next;
			}
			if (scan->next != (struct m_cache *) 0) {
				scan->next->prev = prev;
			}
			scan->prev = (struct m_cache *) 0;
			scan->next = m_set;
			m_set = scan;
		}
	}
	return (scan->name);	/* Return ptr to machine-name */
}


/*** endmacnam
 *
 *
 *	void endmacnam():
 *	Clear the machine-id/name cache.
 */
void endmacnam ()
{
	register struct m_cache *walk;
	register struct m_cache *next;

	walk = m_set;
	while (walk != (struct m_cache *) 0) {
		next = walk->next;
		free (walk->name);
		free ((char *) walk);
		walk = next;
	}
	m_count = 0;			/* Zero machine-id/name cache entries */
	m_set = (struct m_cache *) 0;
}


/*** newmach
 *
 *
 *	struct m_cache *newmach():
 *	Returns:
 *		A pointer to a new machine-id/name cache entry if
 *		adequate heap space exists; otherwise a null ptr
 *		is returned.
 */
static struct m_cache *newmach (name)
char *name;
{
	register struct m_cache *new;

	if ((new = (struct m_cache *)
		   malloc (sizeof (struct m_cache))) != (struct m_cache *) 0) {
		/*
		 *  We successfully got a new cache entry.
		 *  Now try to allocate sufficient name space.
		 */
		if ((new->name = malloc (strlen (name) + 1)) == (char *) 0) {
			/*
			 *  Insufficient heap space for name.
			 */
			free ((char *) new);
			new = (struct m_cache *) 0;
		}
		else m_count++;	/* One more entry */
	}
	return (new);
}
