/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_select.c,v 1.16 1994/11/18 20:41:56 mtm Exp $
 */

/*
 * mcmsg_select.c
 *
 * Selection
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>

mcmsg_select_init()
{

	return;
}

mcmsg_selector_init(sel, method)
	select_t	*sel;
	unsigned long	method;
{

	bzero(sel, sizeof(select_t));
	sel->method = method;
}

mcmsg_selector_install(sel, value, item, method)
	register select_t	*sel;
	register unsigned long	value;
	register void		*item;
	register unsigned long	method;
{
	register select_item_t	*st;
	register select_item_t	*sh;
	register select_item_t	*sn;
	register unsigned long	hi;

#if DEBUG
printf("mcmsg_selector_install(%08X, %u, %08X, %d)\n", sel, value, item, method);
#endif DEBUG
	sn = mcmsg_alloc_select_item();
	assert(sn != 0);
	sn->value = value;
	sn->item = item;
	sn->method = method;
	sn->link = sn;
	sn->mcmsg_task = mcmsg_task;

	if (value == 0) {
		st = sel->zero;
		sel->zero = sn;
	} else {
		hi = SELECT_HASH_FUN(value);
		assert(hi < SELECT_HASH_LEN);
		st = sel->hash[hi];
		sel->hash[hi] = sn;
	}
#ifdef DEBUG
if (sel == &mcmsg_pid_sel) {
	mcmsg_trace_debug("  install mcmsg_pid_sel", 2, hi, st, 0, 0);
}
#endif
	if (st != 0) {
		sh = st->link;
		sn->link = sh;
		st->link = sn;
	}
#if DEBUG
printf(" install %08x (%d, %08X, %u, %08X)\n",
sn, sn->method, sn->link, sn->value, sn->item);
#endif DEBUG
}

mcmsg_selector_install_si(sel, si, value, method)
	register select_t	*sel;
	register select_item_t	*si;
	register unsigned long	value;
	register unsigned long	method;
{
	register select_item_t	*st;
	register select_item_t	*sh;
	register unsigned long	hi;

#if DEBUG
printf("mcmsg_selector_install_si(%08X, %08X)\n", sel, si);
#endif DEBUG
	si->link = si;
	si->mcmsg_task = mcmsg_task;
	si->value = value;
	si->method = method;
	si->item = (void *)si;

	if (value == 0) {
		st = sel->zero;
		sel->zero = si;
	} else {
		hi = SELECT_HASH_FUN(value);
		assert(hi < SELECT_HASH_LEN);
		st = sel->hash[hi];
		sel->hash[hi] = si;
	}
	if (st != 0) {
		sh = st->link;
		si->link = sh;
		st->link = si;
	}
#if DEBUG
printf(" install %08x (%d, %08X, %u, %08X)\n",
si, si->method, si->link, si->value, si->item);
#endif DEBUG
}

select_item_t *
mcmsg_selector_lookup_si(sel, value)
	register select_t	*sel;
	register unsigned long	value;
{
	register select_item_t	*st;
	register select_item_t	*sl;
	register unsigned long	hi;

#if DEBUG
printf("mcmsg_selector_lookup(%08X, %u)\n", sel, value);
#endif DEBUG
	if (value == 0) {
		st = sel->zero;
		if (st == 0) {
#if DEBUG
printf(" return 0, no zero items\n");
#endif DEBUG
			return 0;
		}
	} else {
		unsigned long t;

		hi = SELECT_HASH_FUN(value);
		sl = sel->hash[hi];
		if (sl == 0) {
#if DEBUG
printf(" return 0, no items\n");
#endif DEBUG
			return 0;
		}
		assert((t = MAXLOOP) != 0);
		for (st = sl->link; ; st = st->link) {
			assert(st != 0);
			if (st->value == value)
				break;
			if (st == sl) {
#if DEBUG
printf(" return 0, item not found\n");
#endif DEBUG
				return 0;
			}
			assert(t-- != 0);
		}
	}
#if DEBUG
printf(" found   %08x (%d, %08X, %u, %08X)\n",
st, st->method, st->link, st->value, st->item);
#endif DEBUG
	mcmsg_task = st->mcmsg_task;
	return st;
}

void *
mcmsg_selector_lookup(sel, value)
	register select_t	*sel;
	register unsigned long	value;
{
	register select_item_t	*si;

	si = mcmsg_selector_lookup_si(sel, value);
	if (si == 0)
		return 0;
	return si->item;
}

select_item_t *
mcmsg_selector_detach(sel, value)
	register select_t	*sel;
	register unsigned long	value;
{
	register select_item_t	*si;
	register select_item_t	*st;
	register select_item_t	*sl;
	register void		**selh;
	register unsigned long	hi;

#if DEBUG
printf("mcmsg_selector_detach(%08X, %u)\n", sel, value);
#endif DEBUG
	if (value == 0) {
		selh = &sel->zero;
	} else {
		hi = SELECT_HASH_FUN(value);
		selh = &sel->hash[hi];
	}
	st = *selh;
	if (st == 0) {
		return 0;
	}
	sl = st;
	si = st->link;
	if (si == st) {
		if (si->value != value)
			return 0;
		*selh = 0;
	} else {
		unsigned long t;

		assert((t = MAXLOOP) != 0);
		for (;;) {
			if (si->value == value)
				break;
			if (si == st)
				return 0;
			sl = si;
			si = si->link;
			assert(t-- != 0);
		}
		sl->link = si->link;
		if (si == st) {
			*selh = sl;
		}
	}
#if DEBUG
printf(" found   %08x (%d, %08X, %u, %08X)\n",
si, si->method, si->link, si->value, si->item);
#endif DEBUG
	return si;
}

void *
mcmsg_selector_remove(sel, value)
	register select_t	*sel;
	register unsigned long	value;
{
	register select_item_t	*st;

	st = mcmsg_selector_detach(sel, value);
	if (st == 0) {
		return 0;
	}
	mcmsg_free_select_item(st);
}

mcmsg_selector_clear(sel)
	register select_t	*sel;
{
	register int		hi;
	register select_item_t	*si;
	register select_item_t	*st;
	register select_item_t	*sn;
	register void		**sh;
	unsigned long		t;

	mcmsg_trace_debug("sel clear", 1, sel, 0, 0, 0);
	for (hi = 0; hi <= SELECT_HASH_LEN; hi++) {
		if (hi == SELECT_HASH_LEN) {
			sh = &sel->zero;
		} else {
			sh = &sel->hash[hi];
		}
		
		st = *sh;
		if (st != 0) {
			si = st->link;
			assert((t = MAXLOOP) != 0);
			for (;;) {
				sn = si->link;
				mcmsg_free_select_item(si);
				if (si == st)
					break;
				si = sn;
				assert(t-- != 0);
			}
		}
		*sh = 0;
	}
}

/*
 *	Free all the message sequence select items for a task.
 */
mcmsg_selector_clear_task_seq(sel, mcmsg_task)
	register select_t	*sel;
	mcmsg_task_t		*mcmsg_task;
{
	register int		hashi;
	register select_item_t	*seltail, *selprev, *selcurr;
	register select_item_t	*ss;
	register select_item_t	*sx;
	register select_item_t	*sy;
	register void		**sh;
	unsigned long		t;


	mcmsg_trace_debug("sel clear seq", 2, sel, mcmsg_task, 0, 0);
	/*
	 * Loop thru hash table.
	 */
	for (hashi = 0; hashi <= SELECT_HASH_LEN; hashi++) {
		if (hashi == SELECT_HASH_LEN) {
			sh = &sel->zero;
		} else {
			sh = &sel->hash[hashi];
		}
		
		seltail = *sh;
		if (seltail == 0)
			continue;
		selprev = seltail;
		selcurr = seltail->link;
		assert((t = MAXLOOP) != 0);
		/*
		 * Loop thru pids
		 */
		for (;;) {
			if (selcurr->mcmsg_task == mcmsg_task &&
			    selcurr->method != SELMETH_TASK) {
				/*
				 * Loop thru nx requests
				 */
				for (ss = selcurr->nxrq.seq_link;
				     ss != 0 && ss->mcmsg_task == mcmsg_task;
				     ) {
					sx = ss->nxrq.seq_link;
#ifdef DEBUG
					mcmsg_trace_debug("  clear seq free 1",
						2, (long)ss, hashi, 0, 0);
#endif
					mcmsg_free_select_item(ss);
					ss = sx;
					assert(t-- != 0);
				}
				if (ss != 0) {
					seltail->link = ss;
					ss->link = selcurr->link;
					sy = ss;
					for (ss = sy->nxrq.seq_link; ss != 0;) {
					    sx = ss->nxrq.seq_link;
					    if (ss->mcmsg_task == mcmsg_task) {
						sy->nxrq.seq_link = sx;
#ifdef DEBUG
						mcmsg_trace_debug(
						    "  clear seq free 2",
						    2, (long)ss, hashi, 0, 0);
#endif
						mcmsg_free_select_item(ss);
					    } else {
						sy = ss;
					    }
					    ss = sx;
					    assert(t-- != 0);
					}
				} else {
					seltail->link = selcurr->link;
				}
#ifdef DEBUG
				mcmsg_trace_debug("  clear seq free 3", 
					2, (long)selcurr, hashi, 0, 0);
#endif
				if (selcurr == *sh) {
					*sh = selcurr->link;
				}
				mcmsg_free_select_item(selcurr);
				if (selcurr == seltail) {
					*sh = 0;
					break;
				}
			} else {
				seltail = selcurr;
			}
			if (selcurr == selprev)
				break;
			selcurr = seltail->link;
			assert(t-- != 0);
		}
	}
}

#if 1
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_select.c,v 1.16 1994/11/18 20:41:56 mtm Exp $
 */

/*
 * mcmsg_ptype.c
 *
 * Ptype selection
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>

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
#if DEBUG
printf("select(%d) return A = %d\n", ptype, i);
#endif DEBUG
			return i;
		}
		if (ptype > p) {
			if (i == n) {
#if DEBUG
printf("select(%d) return B = %d\n", ptype, i);
#endif DEBUG
				return i;
			}
			m = i;
		} else {
			if (i == m) {
#if DEBUG
printf("select(%d) return C = %d\n", ptype, i-1);
#endif DEBUG
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

#if DEBUG
printf("add %d  task 0x%x\n", ptype, item);
#endif DEBUG

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
#if DEBUG
printf(" Already there, same task  last %d\n", plist->last_entry);
#endif DEBUG
		return 0;	/* Already there, same task */
	}
	if (i >= 0 && plist->ptype_entry[i].item != 0) {
#if DEBUG
printf(" Already there, trying to change task  last %d\n", plist->last_entry);
#endif DEBUG
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
#if DEBUG
printf(" Expand existing range up  last %d\n", plist->last_entry);
#endif DEBUG
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
#if DEBUG
printf(" Expand existing range down  last %d\n", plist->last_entry);
#endif DEBUG
		return 0;	/* Expand existing range down */
	}
	if (i >= 0 &&
	    plist->ptype_entry[i].ptype == ptype) {
		if (i == n ||
		    plist->ptype_entry[i+1].ptype != ptype + 1) {
			if (n == PTYPE_ENTRIES-1) {
#if DEBUG
printf(" No room A  last %d\n", plist->last_entry);
#endif DEBUG
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
#if DEBUG
printf(" Add one entry at start of range  last %d\n", plist->last_entry);
#endif DEBUG
		return 0;	/* Add one entry at start of range */
	}
	if (i < n &&
	    plist->ptype_entry[i+1].ptype == ptype + 1) {
		if (n == PTYPE_ENTRIES-1) {
#if DEBUG
printf(" No room B  last %d\n", plist->last_entry);
#endif DEBUG
			return -1;	/* No room */
		}
		plist->last_entry = n+1;
		for (j = n; j > i; j--) {
			plist->ptype_entry[j+1] = plist->ptype_entry[j];
		}
		plist->ptype_entry[i+1].ptype = ptype;
		plist->ptype_entry[i+1].item = item;
		plist->ptype_entry[i+1].offset = offset;
#if DEBUG
printf(" Add one entry at end of range  last %d\n", plist->last_entry);
#endif DEBUG
		return 0;	/* Add one entry at end of range */
	}
	if (i < n &&
	    plist->ptype_entry[i+1].item == 0) {
		if (n == PTYPE_ENTRIES-1) {
#if DEBUG
printf(" No room A  last %d\n", plist->last_entry);
#endif DEBUG
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
#if DEBUG
printf(" Add one entry and extend upper range  last %d\n", plist->last_entry);
#endif DEBUG
		return 0;	/* Add one entry and extend upper range */
	}
	if (n >= PTYPE_ENTRIES-2) {
#if DEBUG
printf(" No room C  last %d\n", plist->last_entry);
#endif DEBUG
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
#if DEBUG
printf(" Add two entries  last %d\n", plist->last_entry);
#endif DEBUG
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

#if DEBUG
printf("\n");
#endif DEBUG
	plist->method = 0 /* SELMETH_PTYPELIST */;
	plist->last_entry = 0;
	plist->ptype_entry[0].ptype = 0;
	plist->ptype_entry[0].item = 0;
	plist->ptype_entry[0].offset = 0;
}
#endif
