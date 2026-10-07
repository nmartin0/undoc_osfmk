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
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * $Log: himem.c,v $
 * Revision 1.6  1994/11/18  20:35:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  22:26:40  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:29:10  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:25:29  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:25:15  dleslie
 * First R1_0 release
 *

/*
 * Copyright 1992 by Open Software Foundation,
 * Grenoble, FRANCE
 *
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OSF or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 *   OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * support of memory above 16 Megs for DMA limited to memory
 * below 16 Megs. Copies high memory lo low memory before DMA
 * write operations and does the reverse at completion time for
 * DMA read operations
 */

#include <cpus.h>
#include <platforms.h>
#include <kern/lock.h>
#include <mach/vm_param.h>
#include <vm/vm_page.h>
#include <i386at/himem.h>
#include <kern/kalloc.h>
#include <device/device_types.h>
#include <mach/boolean.h>


hil_t hil_head;
simple_lock_data_t hil_lock;

#if	HIMEM_STATS
int himem_request;	/* number of requests */
int himem_used;		/* number of times used */
#endif	HIMEM_STATS

/* 
 * Called by drivers, this indicates himem that this driver might need
 * to allocate as many as npages pages in a single I/O DMA transfer
 */

himem_reserve(npages)
{
	register i = 0;
	vm_page_t free_head = VM_PAGE_NULL;
	vm_page_t low;
	hil_t	hil;
	int ipl;
	extern vm_offset_t avail_end;

	if (avail_end <= HIGH_MEM)
		return;
	hil = (hil_t)kalloc(npages*sizeof(struct himem_link));
	if (hil == (hil_t)0) 
		panic("himem_reserve: kalloc failed\n");

	for (i=0; i < npages-1; i++)
		(hil+i)->next = hil+i+1;

	/*
	 * This is the only way of getting low physical pages 
	 * wtithout changing VM internals
	 */
	for (i=0; i != npages;) {
		if ((low = vm_page_grab()) == VM_PAGE_NULL)
			panic("No low memory pages for himem\n");
		vm_page_gobble(low); /* mark as consumed internally */
		if (_high_mem_page(low->phys_addr)) {
			low->pageq.next = (queue_entry_t)free_head;
			free_head = low;
		} else {
			(hil+i)->low_page = low->phys_addr;
			i++;
		}
	}

	for (low = free_head; low; low = free_head) {
		free_head = (vm_page_t) low->pageq.next;
		vm_page_free(low);
        }

	ipl = splhi();
	simple_lock(&hil_lock);
	(hil+npages-1)->next = hil_head;
	hil_head = hil;
	simple_unlock(&hil_lock);
	splx(ipl);
}

/*
 * Called by driver at DMA initialization time. Converts a high memory
 * physical page to a low memory one. If operation is a write, 
 * [phys_addr, phys_addr+lenght-1] is copied to new page. Caller must
 * provide a pointer to a pointer to a himem_list. This is used to store
 * all the conversions and is use at completion time to revert the pages.
 * This pointer must point to a null hil_t value for the call on the first
 * page of a DMA transfer
 */

vm_offset_t
himem_convert(phys_addr, length, io_op, hil)
vm_offset_t phys_addr;
vm_size_t length;
hil_t	*hil;
{
	hil_t h;
	int ipl;

	assert ((phys_addr&(I386_PGBYTES - 1)) + length <= I386_PGBYTES);

	ipl = splhi();
	simple_lock(&hil_lock);
	while (!(h = hil_head)) { 
		printf("himem going to sleep\n");
		thread_sleep(&hil_head, &hil_lock, FALSE); 
	}
	hil_head = hil_head->next;
	simple_unlock(&hil_lock);
	splx(ipl);
	
	h->high_addr = phys_addr;

	if (io_op == D_WRITE) {
	  bcopy(phystokv(phys_addr), phystokv(h->low_page), length);
	  h->length = 0;
	} else {
	  h->length = length;
	}

	assert(!*hil || (*hil)->high_addr);

	h->next = *hil;
	*hil = h;
	return(h->low_page);
}

/*
 * Called by driver at DMA completion time. Converts a list of low memory
 * physical page to the original high memory one. If operation was read, 
 * [phys_addr, phys_addr+lenght-1] is copied to original page
 */

vm_offset_t
himem_revert(hil)
hil_t hil;
{
	hil_t next;
	boolean_t wakeup = FALSE;
	int ipl;

	while(hil) {
		if (hil->length)
	  		bcopy(phystokv(hil->low_page), phystokv(hil->high_addr),
			      hil->length);
		hil->high_addr = 0;
		hil->length = 0;
		next = hil->next;
		ipl = splhi();
		simple_lock(&hil_lock);
		if (!(hil->next = hil_head))
			wakeup = TRUE;
		hil_head = hil;
		simple_unlock(&hil_lock);
		splx(ipl);
		hil = next;
	}
	if (wakeup)
		thread_wakeup(&hil_head);
}





