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
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: phys.c,v $
 * Revision 2.10  1994/11/18  20:39:19  mtm
 * Copyright additions/changes
 *
 * Revision 2.9  1993/06/30  22:31:17  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.8  1993/06/09  01:32:14  terry
 * source sync with OSF
 *
 * Revision 2.7  1992/09/23  22:35:25  andyp
 * Removed dead code.  Added support for unsnooped physical
 * operations on Paragons.
 *
 * Revision 2.6  1992/09/22  17:59:05  SSD
 * *** empty log message ***
 *
 * Revision 2.4.2.2  92/04/08  16:03:16  jeffreyh
 * 	Use the experimental copy/zero routines
 * 	[92/04/08            andyp]
 * 
 * Revision 2.4.2.1  92/03/28  10:08:11  jeffreyh
 * 	Make phystokv() a real routine.
 * 	[92/03/20            andyp]
 * 
 * Revision 2.4  91/06/18  20:51:38  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:57:26  jsb]
 * 
 * Revision 2.3  91/05/18  14:30:55  rpd
 * 	Added vm_page_fictitious_addr assertions.
 * 	[91/05/15            rpd]
 * 
 * Revision 2.2  90/12/04  14:49:04  jsb
 * 	First checkin.
 * 	[90/12/03  21:31:58  jsb]
 * 
 */

#include <mach/boolean.h>
#include <kern/task.h>
#include <kern/thread.h>
#include <vm/vm_map.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <vm/vm_kern.h>

#include <i860/pmap.h>
#include <mach/i860/vm_param.h>


#if	PARAGON860 && NCPUS == 1
int	unsnooped_phys_operations;
#endif	PARAGON860 && NCPUS == 1

extern int	ptes_per_vm_page;	/* intel/pmap.c */

/*
 *	I moved this out of mach/i860/vm_param.h so it is
 *	less costly (wall-clock compile-time) to experiment.
 */
vm_offset_t phystokv(phys)
	vm_offset_t phys;
{
#if	PARAGON860 && NCPUS == 1
	if (unsnooped_phys_operations)
		return phys & ~(0x30000000);
	return phys;
#else	PARAGON860 && NCPUS == 1
	return phys;
#endif	PARAGON860 && NCPUS == 1
}


/*#define	PHYSTOKV(a) a*/
/*
 *	pmap_zero_page zeros the specified (machine independent) page.
 */
pmap_zero_page(p)
	register vm_offset_t p;
{
	register int	physPages=ptes_per_vm_page;

	assert(p != vm_page_fictitious_addr);

	p = phystokv(p);
	do {
		quad_zero((char *) p, I860_PGBYTES >> 4);
		p += I860_PGBYTES;
	} while ( --physPages > 0 );
}


/*
 *      pmap_zero_part_page
 *      zeros the specified (machine independent) part of a page.
 */
pmap_zero_part_page(p, offset, len)
        vm_offset_t     p;
        vm_offset_t     offset;
        vm_size_t       len;
{
        assert(p != vm_page_fictitious_addr);
        assert((offset+len) <= PAGE_SIZE);

        bzero(phystokv(p)+offset, len);
}

/*
 *	pmap_copy_page copies the specified (machine independent) pages.
 */
pmap_copy_page(src, dst)
	register vm_offset_t src, dst;
{
	register int	physPages=ptes_per_vm_page;

	assert(src != vm_page_fictitious_addr);
	assert(dst != vm_page_fictitious_addr);

	src = phystokv(src);
	dst = phystokv(dst);
	do {
		piped_page_copy((char *) src, (char *) dst);
		src += I860_PGBYTES;
		dst += I860_PGBYTES;
	} while ( --physPages > 0 );
}

/*
 *      pmap_copy_part_page copies the partial (machine independent) pages.
 */
pmap_copy_part_page(src, src_offset,  dst, dst_offset, len)
        vm_offset_t     src;
        vm_offset_t     dst;
        vm_offset_t     src_offset;
        vm_offset_t     dst_offset;
{
        assert(src != vm_page_fictitious_addr);
        assert(dst != vm_page_fictitious_addr);
        assert((dst_offset+len) <= PAGE_SIZE);
        assert((src_offset+len) <= PAGE_SIZE);
 
        bcopy(phystokv(src)+src_offset, phystokv(dst)+dst_offset, len);
}

/*
 *	kvtophys(addr)
 *
 *	Convert a kernel virtual address to a physical address
 */
kvtophys(addr)
vm_offset_t addr;
{
	pt_entry_t *pte;

	if ((pte = pmap_pte(kernel_pmap, addr)) == PT_ENTRY_NULL)
		return(0);
	return(intel_trunc_page(*pte) | (addr & INTEL_OFFMASK));
}
