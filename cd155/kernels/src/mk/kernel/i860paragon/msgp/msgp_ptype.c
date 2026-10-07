/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_ptype.c,v 1.4 1994/11/18 20:47:41 mtm Exp $
 */

/*
 * mcmsg_ptype.c
 *
 * Ptype selection
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>

#define NEWLIB 1

mcmsg_reserve_ptype(mt, ptype, count, statusp)
	mcmsg_task_t	*mt;
	long		ptype;
	long		count;
	long		statusp;
{
	ptype_list_t	*task_ptype_list;
	long		*sp;

	sp = mcmsg_validate_long(mt, statusp);
#if NEWLIB
	if (sp == 0) {
		return -1;
	}
#endif NEWLIB

	/*
	 * Attach to Mach task
	 */

	task_ptype_list = mt->task_ptype_list;

	/*
	 * Insert ptypes
	 */

	/* XXX Replace with range based add and eliminate limit of 1000 */

	if (count <= 0 || count > 1000) {
		mcmsg_trace_drop("reserve_ptype bad count", count);
#if NEWLIB
#else NEWLIB
if (sp != 0)
#endif NEWLIB
		*sp = -1;
		return -1;
	}
	if (ptype <= INVALID_PTYPE && ptype + count >= INVALID_PTYPE) {
		mcmsg_trace_drop("reserve_ptype bad ptype", ptype);
#if NEWLIB
#else NEWLIB
if (sp != 0)
#endif NEWLIB
		*sp = -1;
		return -1;
	}

	for (; count; count--, ptype++) {
		if (mcmsg_ptype_add(task_ptype_list,
				    ptype,
				    (void *)mt,
				    0) == -1) {
			mcmsg_trace_drop("reserve_ptype add failed", ptype);
#if NEWLIB
#else NEWLIB
if (sp != 0)
#endif NEWLIB
			*sp = -1;
			return -1;
		}
	}
	mcmsg_flush_pna(mt);
#if NEWLIB
#else NEWLIB
if (sp != 0)
#endif NEWLIB
	*sp = 1;
	return 0;
}

mcmsg_ptype_select(plist, ptype)
	register ptype_list_t	*plist;
	register long		ptype;
{
	register long		i;
	register long		j;
	register long		m;
	register long		n;
	register long		p;
	register unsigned long	t;

	m = 0;
	n = plist->last_entry;
	i = j = n >> 1;
	assert((t = MAXLOOP) != 0);
	for (;;) {
		p = plist->ptype_entry[i].ptype;
		if (ptype == p) {
			return i;
		}
		if (ptype > p) {
			if (i == n) {
				return i;
			}
			m = i;
		} else {
			if (i == m) {
				return i-1;
			}
			n = i-1;
		}
		j >>= 1;
		if (j == 0 && m != n) {
			j = 1;
		}
		i = m + j;
		assert(t-- != 0);
	}
}

mcmsg_ptype_add(plist, ptype, item, offset)
	register ptype_list_t	*plist;
	register long		ptype;
	register void		*item;
	register long		offset;
{
	register long		i;
	register long		j;
	register long		n;
	register long		a;

	/* Special case if this is the first entry being added */
	if (plist->last_entry == 0) {
		plist->ptype_entry[0].ptype = ptype;
		plist->ptype_entry[0].item = item;
		plist->ptype_entry[0].offset = offset;
		plist->ptype_entry[1].ptype = ptype + 1;
		plist->ptype_entry[1].item = 0;
		plist->last_entry = 1;
		return 1;
	}

	n = plist->last_entry;
	i = mcmsg_ptype_select(plist, ptype);
	if (i >= 0 && plist->ptype_entry[i].item == item) {
		return 0;	/* Already there, same task */
	}
	if (i >= 0 && plist->ptype_entry[i].item != 0) {
		return -1;	/* Already there, trying to change task */
	}
	if (i > 0 &&
	    plist->ptype_entry[i-1].item == item &&
	    plist->ptype_entry[i].ptype == ptype) {
		if (i < n &&
		    plist->ptype_entry[i+1].ptype == ptype + 1) {
			if (plist->ptype_entry[i+1].item == item) {
				a = 2;
			} else {
				a = 1;
			}
			plist->last_entry = n - a;
			for (j = i; j <= n - a; j++) {
				plist->ptype_entry[j] = plist->ptype_entry[j+a];
			}
		} else {
			plist->ptype_entry[i].ptype = ptype + 1;
		}
		return 0;	/* Expand existing range up */
	}
	if (i < n &&
	    plist->ptype_entry[i+1].item == item &&
	    plist->ptype_entry[i+1].ptype == ptype + 1) {
		plist->ptype_entry[i+1].ptype = ptype;
		if (i >= 0 &&
		    plist->ptype_entry[i].ptype == ptype) {
			plist->last_entry = n - 1;
			for (j = i; j <= n - 1; j++) {
				plist->ptype_entry[j] = plist->ptype_entry[j+1];
			}
		}
		return 0;	/* Expand existing range down */
	}
	if (i >= 0 &&
	    plist->ptype_entry[i].ptype == ptype) {
		if (i == n ||
		    plist->ptype_entry[i+1].ptype != ptype + 1) {
			if (n == PTYPE_ENTRIES-1) {
				return -1;	/* No room */
			}
			plist->last_entry = n+1;
			for (j = n; j >= i; j--) {
				plist->ptype_entry[j+1] = plist->ptype_entry[j];
			}
			plist->ptype_entry[i+1].ptype = ptype + 1;
		}
		plist->ptype_entry[i].item = item;
		plist->ptype_entry[i].offset = offset;
		return 0;	/* Add one entry at start of range */
	}
	if (i < n &&
	    plist->ptype_entry[i+1].ptype == ptype + 1) {
		if (n == PTYPE_ENTRIES-1) {
			return -1;	/* No room */
		}
		plist->last_entry = n+1;
		for (j = n; j > i; j--) {
			plist->ptype_entry[j+1] = plist->ptype_entry[j];
		}
		plist->ptype_entry[i+1].ptype = ptype;
		plist->ptype_entry[i+1].item = item;
		plist->ptype_entry[i+1].offset = offset;
		return 0;	/* Add one entry at end of range */
	}
	if (i < n &&
	    plist->ptype_entry[i+1].item == 0) {
		if (n == PTYPE_ENTRIES-1) {
			return -1;	/* No room */
		}
		plist->last_entry = n+1;
		for (j = n; j > i; j--) {
			plist->ptype_entry[j+1] = plist->ptype_entry[j];
		}
		plist->ptype_entry[i+2].ptype = ptype + 1;
		plist->ptype_entry[i+1].ptype = ptype;
		plist->ptype_entry[i+1].item = item;
		plist->ptype_entry[i+1].offset = offset;
		return 0;	/* Add one entry and extend upper range */
	}
	if (n >= PTYPE_ENTRIES-2) {
		return -1;	/* No room */
	}
	plist->last_entry = n+2;
	for (j = n; j > i; j--) {
		plist->ptype_entry[j+2] = plist->ptype_entry[j];
	}
	plist->ptype_entry[i+1].ptype = ptype;
	plist->ptype_entry[i+1].item = item;
	plist->ptype_entry[i+1].offset = offset;
	plist->ptype_entry[i+2].ptype = ptype + 1;
	plist->ptype_entry[i+2].item = 0;
	return 0;	/* Add two entries */
}

mcmsg_ptype_clear_item(plist, item)
	register ptype_list_t	*plist;
	register void		*item;
{
	register long		i;
	register long		j;
	register long		n;
	register long		a;

	n = plist->last_entry;
	for (i = 0; i <= n; i++) {
		if (plist->ptype_entry[i].item == item) {
			plist->ptype_entry[i].item = 0;
			plist->ptype_entry[i].offset = 0;
			a = 0;
			if (i < n &&
			    plist->ptype_entry[i+1].item == 0) {
				a++;
			}
			if (i > 0 &&
			    plist->ptype_entry[i-1].item == 0) {
				a++;
				i--;
			}
			if (a > 0) {
				plist->last_entry = n - a;
				for (j = i + 1; j <= n - a; j++) {
					plist->ptype_entry[j] =
						plist->ptype_entry[j+a];
				}
				n -= a;
			}
		}
	}
}

mcmsg_ptype_free_all(plist, l2size)
	register ptype_list_t	*plist;
	register unsigned long	l2size;
{
	register long		i;
	register long		j;
	register long		n;
	register long		a;

	n = plist->last_entry;
	for (i = 0; i <= n; i++) {
		if (plist->ptype_entry[i].item != 0) {
			mcmsg_l2free(plist->ptype_entry[i].item, l2size);
		}
	}
}

mcmsg_ptype_init(plist)
	register ptype_list_t	*plist;
{

	plist->method = 0 /* SELMETH_PTYPELIST */;
	plist->last_entry = 0;
	plist->ptype_entry[0].ptype = 0;
	plist->ptype_entry[0].item = 0;
	plist->ptype_entry[0].offset = 0;
}
