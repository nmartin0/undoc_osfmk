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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_malloc.c,v 1.15 1995/02/28 21:21:47 lenb Exp $
mcmsg/RCS/mcmsg_malloc.c,v 1.5 92/06/02 16:49:06 regnier Exp Locker: regnier $
 */

/*
 * mcmsg_malloc.c
 *
 * Message passing memory manager
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_select.h>
#include <mach/kern_return.h>
#include <vm/vm_kern.h>

unsigned long	mcmsg_memalloc_count = 0;
unsigned long	mcmsg_memfree_count = 0;
unsigned long	mcmsg_memcoll_count = 0;
unsigned long	mcmsg_meminlp_count = 0;
unsigned long	mcmsg_memcfail_count = 0;
unsigned long	mcmsg_memifail_count = 0;
unsigned long	mcmsg_memalloc[MAXLOG2MMSIZE];
extern int max_num_nodes;

#define BASE(m,i) ((((unsigned long)(m)) - ((unsigned long)mcmsg_memory)) & \
		   ~(1 << (i)))


mcmsg_memory_init()
{
	register unsigned long	i,mmprocs;

#if TRACE_ALLOC
	mcmsg_trace_debug("memory_init", 0, 0, 0, 0, 0);
#endif TRACE_ALLOc

	mmprocs = getbootint("PROCS_PER_NODE",1);
	mmsize = (max_num_nodes > 512 ? max_num_nodes : 512) * (mmprocs * 256) +
		       ((getbootint("MSGS_PER_PROC",512) * 256) * mmprocs);
	log2mmsize = 1;
	while ((1 << log2mmsize) < mmsize) {
		log2mmsize++;
	}
	mmsize = 1 << log2mmsize;

	if((kmem_alloc_wired(kernel_map,&mcmsg_memory,mmsize)) != KERN_SUCCESS){
		panic("mcmsg_memory_init() kmem_alloc_wired() failed.");
	}

	for (i = 0; i < LOG2MMSIZE; i++) {
		mcmsg_memlist[i] = 0;
		mcmsg_memalloc[i] = 0;
	}
	mcmsg_memlist[LOG2MMSIZE] = mcmsg_memory;
	mcmsg_memory[0] = 0;

	mcmsg_init_si_cache();
}

unsigned long
l2size(req_size)
	register unsigned long	req_size;
{
	register unsigned long	size;
	register unsigned long	l2size;

	assert(req_size < mmsize);
	size = req_size;
	l2size = 0;
	if(LOG2MMSIZE >= 16) {
	    if (size & 0xFFFF0000) {
		size &= 0xFFFF0000;
		l2size |= 16;
	    }
	}
	if(LOG2MMSIZE >= 8) {
	    if (size & 0xFF00FF00) {
		size &= 0xFF00FF00;
		l2size |= 8;
	    }
	}
	if(LOG2MMSIZE >= 4) {
	    if (size & 0xF0F0F0F0) {
		size &= 0xF0F0F0F0;
		l2size |= 4;
	    }
	}
	if(LOG2MMSIZE >= 2) {
	    if (size & 0xCCCCCCCC) {
		size &= 0xCCCCCCCC;
		l2size |= 2;
	    }
	}
	if(LOG2MMSIZE >= 1) {
	    if (size & 0xAAAAAAAA) {
		size &= 0xAAAAAAAA;
		l2size |= 1;
	    }
	}
	if (size != req_size) {
		size <<= 1;
		l2size++;
	}
	if (l2size < 4)
		l2size = 4;
	return l2size;
}

unsigned long *
mcmsg_l2collect(l2size)
	register unsigned long	l2size;
{
	register unsigned long	i;
	register unsigned long	j;
	register unsigned long	*m1;
	register unsigned long	*m2;
	register unsigned long	*n1;
	register unsigned long	*n2;
	register unsigned long	save;
	register unsigned long	t;

#if TRACE_ALLOC
	mcmsg_trace_debug("l2collect", 1, 1 << l2size, 0, 0, 0);
#endif TRACE_ALLOC

	mcmsg_memcoll_count++;
	save = mcmsg_meminlp_count;
	assert(l2size <= LOG2MMSIZE);
	for (i = l2size-1; i >= 4; i--) {
		for (j = i; j < l2size; j++) {
			n1 = (unsigned long *)&mcmsg_memlist[j];
			assert((t = 100000) != 0);
			for (m1 = mcmsg_memlist[j];
			     m1 != 0;
			     m1 = (unsigned long *)(m1[0])) {
				assert(m1 >= mcmsg_memory);
				n2 = m1;
				for (m2 = (unsigned long *)(m1[0]);
				     m2 != 0;
				     m2 = (unsigned long *)(m2[0])) {
					mcmsg_meminlp_count++;
					if ( BASE(m1, j) == BASE(m2, j) ) {
						if (n2 == m1) {
							n1[0] =
							 (unsigned long)m2[0];
						} else {
							n1[0] =
							 (unsigned long)m1[0];
							n2[0] =
							 (unsigned long)m2[0];
						}
						m1 = (unsigned long *)
						     (BASE(m1, j) +
						      (unsigned long)
						      mcmsg_memory);
						if (j == l2size-1) {
							return  m1;
						} else {
							mcmsg_intfree(m1, j+1);
						}
						m1 = n1;
						break;
					}
					n2 = m2;
				}
				n1 = m1;
				assert(t-- != 0);
			}
		}
	}
	mcmsg_memcfail_count++;
	mcmsg_memifail_count += mcmsg_meminlp_count - save;
	return 0;
}

unsigned long *
mcmsg_l2malloc(l2size)
	register unsigned long	l2size;
{
	register unsigned long	i;
	register unsigned long	*m;

	mcmsg_memalloc_count++;
	assert(l2size >= 4);
	assert(l2size <= LOG2MMSIZE);
	for (i = l2size; mcmsg_memlist[i] == 0; i++) {
		if (i == LOG2MMSIZE) {
			m = mcmsg_l2collect(l2size);
#if TRACE_ALLOC
			mcmsg_trace_debug("l2malloc", 2, m, 1 << l2size, 0, 0);
#endif TRACE_ALLOC
			if (m != 0) {
				mcmsg_memalloc[l2size]++;
				m[1] = 0;
			}
			return m;
		}
	}
	m = mcmsg_memlist[i];
	mcmsg_memalloc[l2size]++;
#if TRACE_ALLOC
	mcmsg_trace_debug("l2malloc", 2, m, 1 << l2size, 0, 0);
#endif TRACE_ALLOC
	assert(m >= mcmsg_memory);
	assert((unsigned long *)(m[0]) >= mcmsg_memory || 
		(unsigned long *)(m[0]) == 0);
	mcmsg_memlist[i] = (unsigned long *)(m[0]);
	for (; i > l2size; i--) {
		mcmsg_intfree((unsigned long)m + (1 << (i-1)), i-1);
	}
	m[1] = 0;
	return m;
}

mcmsg_l2free(m, l2size)
	register unsigned long	*m;
	register unsigned long	l2size;
{

#if TRACE_ALLOC
	mcmsg_trace_debug("l2free", 2, m, 1 << l2size, 0, 0);
#endif TRACE_ALLOC
	assert(m >= mcmsg_memory);
	mcmsg_memfree_count++;
	mcmsg_memalloc[l2size]--;
	mcmsg_intfree(m, l2size);
}

mcmsg_intfree(m, l2size)
	register unsigned long	*m;
	register unsigned long	l2size;
{

	assert(l2size >= 4);
	assert(m != 0);
	assert(m >= mcmsg_memory);
#if	0
        assert(m[1] != 0xdead);	/* xxx this assert prevents DOT reboot */
#endif
	m[0] = (unsigned long)(mcmsg_memlist[l2size]);
	assert((m[1] = 0xdead) != 0);
	mcmsg_memlist[l2size] = m;
}


/*
 *	Routines:
 *		mcmsg_init_si_cache()		init si cache
 *		mcmsg_alloc_select_item()	allocate a select item
 *		mcmsg_free_select_item()	free a select item
 *
 *	Purpose:
 *		These routines utilize a cache to avoid the l2 allocator
 *		for the most common size of allocation (the select item).
 *
 *		When allocating, take one off the cache if available,
 *		otherwise allocate normally. When freeing, put the si
 *		back on the cache unless it is full.
 */

#define	MAX_SI_CACHE	64

int           mcmsg_si_cache_count;
select_item_t *mcmsg_si_cache[MAX_SI_CACHE];

mcmsg_init_si_cache()
{
	int i;

	for (i=0; i<MAX_SI_CACHE; i++) {
		mcmsg_si_cache[i] = mcmsg_l2malloc(l2size(sizeof(select_item_t)));
	}
	mcmsg_si_cache_count = MAX_SI_CACHE;
}

select_item_t *
mcmsg_alloc_select_item()
{
	assert(mcmsg_si_cache_count >= 0 && 
	       mcmsg_si_cache_count <= MAX_SI_CACHE);

	if (mcmsg_si_cache_count > 0) {
		return (mcmsg_si_cache[--mcmsg_si_cache_count]);
	} else {
		return ((select_item_t *)mcmsg_l2malloc(l2size(sizeof(select_item_t))));
	}
}

mcmsg_free_select_item(si)
select_item_t *si;
{
	assert(mcmsg_si_cache_count >= 0 && 
	       mcmsg_si_cache_count <= MAX_SI_CACHE);

	if (mcmsg_si_cache_count < MAX_SI_CACHE) {
		mcmsg_si_cache[mcmsg_si_cache_count++] = si;
	} else {
		mcmsg_l2free(si, l2size(sizeof(select_item_t)));
	}
}
