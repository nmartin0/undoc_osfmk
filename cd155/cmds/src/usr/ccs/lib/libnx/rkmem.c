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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  Intel Corporation.
 */

/*
 * $Id: rkmem.c,v 1.12 1994/11/19 02:32:31 mtm Exp $
 *
 * RK memory management library
 *
 * HISTORY
 * $Log: rkmem.c,v $
 * Revision 1.12  1994/11/19  02:32:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/03/09  03:56:07  joel
 *  Reviewer: ellend
 *  Risk: low
 *  Benefit or PTS #: 8001
 *  Testing: EATs, PCCM2
 *  Module(s): libnx/rkmem.c
 *
 * Revision 1.10  1993/11/18  19:25:32  dleslie
 *  Reviewer: shala
 *  Risk: low
 *  Benefit or PTS #: get nx and mcmsg headers out of export tree, not obj
 * 	tree.  This allows users to build without having an obj tree fully
 * 	populated with headers
 *  Testing: built libnx
 *  Module(s):
 *     Makefile _gcol.c _gcolx.c _gops.c _gsync.c _load.c allocUser.c
 *     allocsys.c allocsys_.c autoinit.c bitmap.c bitmap2.c create.c
 *     nodeparser.c nx_initve.c nx_load.c nx_load_.c nx_loadve.c
 *     nx_loadve_.c nx_lock.c nx_part_ops.c nx_part_ops_.c nx_port.c
 *     nx_pri.c nxlib.c parsepart.c parsesched.c partlock.c
 *     partprint.c partutils.c rkassert.c rklib.c rkmem.c utils.c
 *     writepart.c
 *
 *
 * VS:    writepart.c
 *
 * Revision 1.9  1993/07/06  23:59:01  andrews
 * Added xmsg buffer coalescing.
 *
 * Revision 1.8  1993/06/22  22:09:15  prp
 * Move all buffered message matching to the kernel.
 *
 * Revision 1.7  1993/06/11  23:16:47  shala
 * Set adjacent->chain_number to 0 in _xmalloc() routine to
 * properly initialize adjacent buffer element. Fixed by andrews.
 *
 * Revision 1.6  1993/06/10  16:36:24  andrews
 * Added split xmsg buffer support.
 *
 * Revision 1.5  1992/12/16  17:53:55  leonard
 * Added the function (and support for) _xavail() to rkmem: this function returns
 * the size of the largest available contiguous block of memory.
 *
 * The function _xprovide(), in rklib, now uses _xavail() to determine how
 * much memory to allocate if the first call to _xmalloc() fails.
 *
 * These modifications fix bug 2757.
 *
 * Revision 1.4  1992/06/09  21:30:03  stans
 * added ID and ident strings
 *
 */
char _rkmem_c_id[]="$Id: rkmem.c,v 1.12 1994/11/19 02:32:31 mtm Exp $";

#include <stdio.h>
#include <mcmsg/mcmsg_xmsg.h>
#include <nx/assert.h>
#include <nx/debugxmsg.h>

#define FORWARD_COALESCE	1
#define BACKWARD_COALESCE	1

#ifdef VERIFY_XMSG
#define xmsg_verify(xmsg) _xmsg_verify(xmsg)
#else
#define xmsg_verify(xmsg)
#endif

extern xmsg_t	*mcmsg_buffer;
extern xmsg_t	*mcmsg_buffer_end;
xmsg_t		*mcmsg_free;

/*
 * largest found contiguous block of memory: it should be set to 0 when
 * a block of memory is freed or malloced
 */
static unsigned long	mcmsg_max_free = 0;
unsigned long		forward_coalesce = 0;
unsigned long		backward_coalesce = 0;
unsigned long		dual_coalesce = 0;
unsigned long		no_coalesce = 0;

_xminit()
{

	assert((sizeof(xmsg_t) & (sizeof(xmsg_t) - 1)) == 0); /* power of 2 */
	mcmsg_free = 0;
}

_xfree(xm)
	xmsg_t	*xm;
{
	xmsg_t	*next_adjacent;
	xmsg_t	*prev_adjacent;
	xmsg_t  *next_chain;
	char	coalesced;

	xmsg_verify(xm);

	xmsg_verify(mcmsg_free);

	debugxmsg(xm, "xfree");
	for (;;) {
		assert(xm >= mcmsg_buffer);
		assert(xm < mcmsg_buffer_end);
		assert(xm->state != XMSG_FREE);
		/* Save the link, in case xm is part of a chain;
		   xm->link may be modified below, but we'll need
		   the original value to find the next chain element */
		next_chain = xm->link;
		xm->state = XMSG_FREE;
		coalesced = 0;

#if FORWARD_COALESCE
		next_adjacent = xm->next_adjacent;
		if ((next_adjacent != 0) &&
		    (next_adjacent->state == XMSG_FREE)) {
			/* Forward coalesce */
			forward_coalesce++;
			assert((next_adjacent < mcmsg_buffer_end) &&
			       (next_adjacent > mcmsg_buffer));
#if DEBUG
			debugxmsg(adjacent, "xfree coalesce");
#endif
			if (next_adjacent->link == next_adjacent) {
				xm->link = xm;
				xm->backlink = xm;
			} else {
				xm->link = next_adjacent->link;
				xm->backlink = next_adjacent->backlink;
				xm->link->backlink = xm;
				xm->backlink->link = xm;
			}
			xm->next_adjacent = next_adjacent->next_adjacent;
			if (next_adjacent->next_adjacent != 0) {
				next_adjacent->next_adjacent->prev_adjacent = xm;
			}
			xm->size += next_adjacent->size + sizeof(xmsg_t);
			if (mcmsg_free == next_adjacent) {
				mcmsg_free = xm;
			}
			coalesced = 1;
		}
#endif FORWARD_COALESCE

#if BACKWARD_COALESCE
		prev_adjacent = xm->prev_adjacent;
		if ((prev_adjacent != 0) &&
		    (prev_adjacent->state == XMSG_FREE)) {
			/* Backward coalesce */
			backward_coalesce++;
			assert((prev_adjacent >= mcmsg_buffer) &&
			       (prev_adjacent < mcmsg_buffer_end));
			if (coalesced) {
				/* xm is already linked into the
				   free list via a forward coalesce,
				   so unlink it from the list.  The
				   backward coalesce will automatically
				   re-add it */
				forward_coalesce--;
				backward_coalesce--;
				dual_coalesce++;
				xm->link->backlink = xm->backlink;
				xm->backlink->link = xm->link;
			}
			prev_adjacent->next_adjacent = xm->next_adjacent;
			if (xm->next_adjacent != 0) {
				xm->next_adjacent->prev_adjacent = prev_adjacent;
			}
			prev_adjacent->size += xm->size + sizeof(xmsg_t);
			if (mcmsg_free == xm) {
				mcmsg_free = prev_adjacent;
			}
			coalesced = 1;
		}
#endif BACKWARD_COALESCE

		if (!coalesced) {
			no_coalesce++;
			if (mcmsg_free == 0) {
				xm->link = xm;
				xm->backlink = xm;
				mcmsg_free = xm;
			} else {
				xm->backlink = mcmsg_free->backlink;
				xm->link = mcmsg_free;
				mcmsg_free->backlink->link = xm;
				mcmsg_free->backlink = xm;
			}
		}
		if (xm->chain_number == 0)
			break;
		xm->chain_number = 0;
		xm = next_chain;
	}

	mcmsg_max_free = 0;
#if DEBUG
	debugxmsg(mcmsg_free, "xfree first");
#endif
	assert(mcmsg_free != 0);
	assert(mcmsg_free->backlink->link == mcmsg_free);
	assert(mcmsg_free->link->backlink == mcmsg_free);
	xmsg_verify(mcmsg_free);
}

xmsg_t *
_xmalloc(size)
	unsigned long	size;
{
	xmsg_t		*xm;
	xmsg_t		*next_adjacent;
	unsigned long	t1;
	unsigned long	max;

	if (mcmsg_free == 0) {
		debugxmsg(0, "xmalloc no free");
		return 0;
	}

	max = 0;
	xm = mcmsg_free;
	assert( (t1 = TIMEOUT) != 0 );
	for (;;) {

#if DEBUG
		debugxmsg(xm, "xmalloc test");
#endif
		assert((xm->state == XMSG_FREE) && (xm->chain_number == 0));
		if (xm->size >= size) {
			if (xm->size > size + 2*sizeof(xmsg_t)) {
				next_adjacent = xm->next_adjacent;
				next_adjacent->size = xm->size -
						(size + sizeof(xmsg_t));
				next_adjacent->chain_number = 0;
				next_adjacent->state = XMSG_FREE;
				if (xm->link == xm) {
					next_adjacent->link = next_adjacent;
					next_adjacent->backlink = next_adjacent;
				} else {
					next_adjacent->link = xm->link;
					next_adjacent->backlink = xm->backlink;
					next_adjacent->link->backlink =
						next_adjacent;
					next_adjacent->backlink->link =
						next_adjacent;
				}
				next_adjacent->next_adjacent = xm->next_adjacent;
				next_adjacent->prev_adjacent = xm;
				if (xm->next_adjacent != 0) {
					xm->next_adjacent->prev_adjacent =
						next_adjacent;
				}
				xm->next_adjacent = next_adjacent;

				if (mcmsg_free == xm) {
					mcmsg_free = next_adjacent;
				}
#if DEBUG
				debugxmsg(next_adjacent, "xmalloc split");
#endif
				xm->size = size;
			} else {
				assert(xm->backlink >= mcmsg_buffer);
				assert(xm->backlink < mcmsg_buffer_end);
				xm->backlink->link = xm->link;
				xm->link->backlink = xm->backlink;
				if (mcmsg_free == xm) {
					mcmsg_free = xm->link;
					if (mcmsg_free == xm) {
						mcmsg_free = 0;
					}
				}
			}
			xm->state = XMSG_FULL;
			xm->chain_number = 0;
			mcmsg_max_free = 0;
			debugxmsg(xm, "xmalloc return");
			return xm;
		}
		if (xm->size > max) {
			max = xm->size;
		}
		xm = xm->link;
		if (xm == mcmsg_free) {
			mcmsg_max_free = max;
			debugxmsg(0, "xmalloc all small");
			return 0;
		}
		assert(t1-- > 0);
	}
}

xmsg_t *
_xmalloc_largest()
{
	xmsg_t		*xm;
	xmsg_t		*best;
	unsigned long	max;

	if (mcmsg_free == 0) {
		debugxmsg(0, "xmalloc_largest no free");
		return 0;
	}

	max = 0;
	best = 0;
	xm = mcmsg_free;
	for (;;) {
		if (xm->size > max) {
			max = xm->size;
			best = xm;
		}
		xm = xm->link;
		if (xm == mcmsg_free) {
			break;
		}
	}
	xm = best;

	if (xm == 0) {
		return 0;
	}

	assert((xm->state == XMSG_FREE) && (xm->chain_number == 0));
	assert(xm->backlink >= mcmsg_buffer);
	assert(xm->backlink < mcmsg_buffer_end);
	xm->backlink->link = xm->link;
	xm->link->backlink = xm->backlink;
	if (mcmsg_free == xm) {
		mcmsg_free = xm->link;
		if (mcmsg_free == xm) {
			mcmsg_free = 0;
		}
	}
	xm->state = XMSG_FULL;
	xm->chain_number = 0;
	mcmsg_max_free = 0;
	debugxmsg(xm, "xmalloc_largest return");
	return xm;
}

/*
 * return the size of the largest contiguous block of available memory
 */
unsigned long
_xavail ()
{
	register int t1;
	unsigned long max;
	/*
	 * if the max free record is 0, find the largest block of
	 * free memory by trying to _xmalloc() more memory than
	 * is available: this will force malloc() to coalesce all
	 * possible fragments, and set mcmsg_max_free to the largest
	 * contiguous block of free memory
	 */
	if (mcmsg_max_free == 0) {
		xmsg_t	*xm;
		register int t1;
		unsigned long max;

		if (mcmsg_free == 0) {
			return 0;
		}

		max = 0;
		xm = mcmsg_free;
		assert( (t1 = TIMEOUT) != 0 );
		for (;;) {
			if (xm->size > max) {
				max = xm->size;
			}
			xm = xm->link;
			if (xm == mcmsg_free) {
				mcmsg_max_free = max;
				break;
			}
			assert(t1-- > 0);
		}
	}

	return mcmsg_max_free;
}
