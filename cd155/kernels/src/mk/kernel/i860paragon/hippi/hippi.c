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
 * Copyright 1992 by Intel Corporation,
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
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/hippi/hippi.c,v 1.12 1995/03/02 23:32:43 arlin Exp $
 *
 * $Log: hippi.c,v $
 * Revision 1.12  1995/03/02  23:32:43  arlin
 *  Added Multiple Packet per Connection support
 *  and CONTinuation support for multiple I/O
 *  per packet and connection. F/W is R1.4
 *
 *  Reviewer: Jerrie Coffman, Bernie Keany
 *  Risk: medium
 *  Benefit or PTS #: 12411
 *  Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *     Also developed special applications to test
 *     new MPC and CONT modes.
 *  Module(s):
 *     /i860paragon/hippi/
 *      hctlr.c, hctlr.h, rhippi.h, rhippi.c,
 *      hippi_status.h, hdc.c
 *     /ipi/ipi_misc.c ipi_defs.h, ipi.c,
 *     /device/ds_routines.c
 *
 * Revision 1.11  1994/11/29  17:59:21  arlin
 * revision comments added back into header.
 * deleted by mistake in R1.9.
 *
 *
 * Revision 1.8  1994/10/05  21:35:53  arlin
 * fixed hippi_intr nesting problems.
 * corrected spl level and added baton
 * support for MP.
 *
 *  Reviewer: Jerrie Coffman, Len Brown
 *  Risk: low
 *  Benefit or PTS #: 11095
 *  Testing: HiPPI IPI-3 Eat, 32 nodes.
 *  Module(s): hippi.c hippidev.h
 *
 * Revision 1.7  1994/07/12  19:19:53  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.6  1994/06/08  17:00:01  arlin
 * Updated for R1.3 IPI-3 support
 *
 */
/*
 *      File:   hippi.c
 *      Author: Arlin Davis    
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      Master Device Driver for HiPPI expansion board.
 */

#include <hippi.h>
#if	NHIPPI > 0

#include <sys/types.h>
#include <vm/vm_kern.h>
#include <chips/busses.h>
#include <i860paragon/baton.h>
#include <i860paragon/spl.h>
#include <i860paragon/expansion.h>
#include <i860paragon/hippi/hippidev.h>

caddr_t	hippi_std[NHIPPIDEVS] = {
	(caddr_t) HIPPI_SRAM_START,
	(caddr_t) HIPPI_FLASH_START 
};

struct bus_device	*hippi_dinfo[NHIPPIDEVS];
struct bus_ctlr		*hippi_minfo[NHIPPI];

int	hippi_probe(), hippi_slave(), hippi_attach(), hippi_intr();
vm_offset_t	hippi_mmap();

struct bus_driver hippi_driver = {
	hippi_probe,		/* is the hippi card attached? */
	hippi_slave,		/* any devices on the card? */
	hippi_attach,		/* setup driver after the probe */
	0,			/* start transfer */
	hippi_std,		/* device csr addresses */
	"hippi",		/* name of the device */
	hippi_dinfo,		/* backpointers to init structs */
	"hippi",		/* name of a controller */
	hippi_minfo,		/* backpointers to init structs */
	0			/* flags */
};

vm_offset_t	hippi_map_memory();
extern int	hctlr_ctr_i(), hctlr_src_i(), hctlr_dst_i();
extern int	interrupt_expansion_install();
extern int 	node_status_register_expansion_present();
static		(*previous_handler)();

extern int	show_spurious_interrupts;
int		hippi_spurious_interrupts;
boolean_t	hippi_probed_okay;


/*
 *	Return non-zero if an hippi card is present.
 *      Check for minimum board revision of 3.
 */
hippi_probe(vaddr, ctlr)
	caddr_t		vaddr;
	struct bus_ctlr	*ctlr;
{
	extern	int expansion_id();
	int fab,fw860,fw960;
			    
	if (expansion_id() != EXP_ID_HIPPI) {
		hippi_probed_okay = FALSE;
     		return(0);
	}
	
	/* get the board fab and firmware revisions */
	fab = (inb(EXP_ID_ADDR) & 0x00000007);

	/* an io read from 0x8500000 is necessary when getting
	 * a byte at a time from flash on a non-4byte boundary
	 */
	fw960 = inb(EXP_ID_ADDR+1);
	fw960 = inb(0x85000000);
	fw860 = inb(EXP_ID_ADDR+2);
	fw860 = inb(0x85000000);

        printf("HiPPI Controller Firmware Revision - R%d.%d (FAB %d)\n", 
				fw860,fw960,fab);

	/* controller MUST be at least a FAB 3 and R1.3 firmware */
	if (fab < 3) {
        	printf("\nERROR: HiPPI Controller FAB (%d) is downrev!\n",fab);
		hippi_probed_okay = FALSE;
		return(0);
	} 
	else if ((fw860 < 1) || (fw960 < 4)) {
        	printf("\nERROR: HiPPI Controller Firmware (%d.%d) is downrev!\n",
				fw860,fw960);
		hippi_probed_okay = FALSE;
		return(0);
	} 

	hippi_probed_okay = TRUE;
	hippi_interrupt_init();
	return 1;
}

/*
 *      Probe for any slave devices.  In this case, the
 *      slaves are the 128K SRAM and the 256K eeprom/flash on the HIPPI bd.
 */
hippi_slave(device, vaddr)
    struct bus_device       *device;
    caddr_t                 vaddr;
{
    switch(device->unit) {
	case HIPPI_DEV_SRAM:
		return hippi_probe_sram(device);
	case HIPPI_DEV_FLASH:
    		return hippi_probe_eeprom(device);
    	break;
    }
    printf("hippislave: bad unit number (unit==%d)\n", device->unit);
    return 0;
}

/*
 *	Map in the memory on the HIPPI card
 *      and setup other device types.
 */
hippi_attach(device)

	struct bus_device	*device;
{
	vm_offset_t	vaddr;
	char		*flavor;

	switch (device->unit) {
		case HIPPI_DEV_SRAM:
			flavor = "sram";
			break;
		case HIPPI_DEV_FLASH:
			flavor = "flash";
			break;
		default:
			printf("unsupported HiPPI device->unit (%d)",device->unit);
			return 0; 
	}

	vaddr = hippi_map_memory(device->phys_address, device->sysdep);
	if (vaddr == 0) {
		return 0;
	}
	device->address = (caddr_t) vaddr;

	hippi_dinfo[device->unit] = device;

	printf(" virt 0x%x, phys 0x%x, size %uKB (%s)",
		device->address,
		device->phys_address,
		((int) device->sysdep / 1024),
		flavor);

	return 0;
}

/*
 *	Setup for taking an interrupt from the HIPPI card.
 */
hippi_interrupt_init()
{
	/*
	 *	Install a handler for interrupts from
	 *	an expansion card.
	 */
	if (interrupt_expansion_install(hippi_intr, &previous_handler))
		panic("hippi_interrupt_init");
	return 0;
}


/*
 *	Map in a chunk of HIPPI  memory
 */
vm_offset_t hippi_map_memory(paddr, len)
	vm_offset_t		paddr;
	vm_size_t		len;
{
	vm_offset_t		vaddr, v;
	extern vm_offset_t	pmap_map_bd();

	if (!hippi_probed_okay)
		return 0;

	paddr = (vm_offset_t) trunc_page(paddr);
	len = (vm_size_t) round_page(len);

	/*
	 *	No, this isn't really pageable...this call
	 *	simply carves out a chunk of address space that
	 *	will very shortly be filled by physical
	 *	memory on the HIPPI card.
	 */
	if (kmem_alloc_pageable(kernel_map, &vaddr, len) != KERN_SUCCESS) {
		return (vm_offset_t) 0;
	}

	/*
	 *	Enter the physical pages into the kernel map.
	 *	pmap_map_bd() marks the pages as referenced,
	 *	modified, and cache-disable.
	 */
	(void) pmap_map_bd(vaddr, paddr, paddr + len, VM_PROT_WRITE);
	for (v = vaddr; v < v + len; v += PAGE_SIZE) {
		pmap_change_wiring(kernel_pmap, v, TRUE);
	}

	return vaddr;
}

/*
 *      Probe and autosize the static ram on the HIPPI card.
 *      XXX doesn't autosize yet.
 */
static int hippi_probe_sram(device)
	struct bus_device       *device;
{
	return 1;
}

/*
 *   Probe and autosize the onboard Flash(tm) EEPROM.
 *    XXX doesn't autosize yet.
 */
static int hippi_probe_eeprom(device)
  	struct bus_device       *device;
{
	return 1;
}

/*
 *	Dispatch an interrupt from the HIPPI controller.
 *      Master interrupt handler for all HiPPI drivers.
 *		hctlr_ctr_i() will handle controller interrupts
 *		hctlr_src_i() will handle SRC channel interrupts
 *		hctlr_dst_i() will handle DST channel interrupts
 * 	Will support a multiple types of interrupts.
 */
hippi_intr(regs)
	register struct i860_saved_state *regs;
{
	unsigned short	status;
	int	s = -1;
	int	unit = 0;	/* only one controller per driver supported */

	/* check for interrupt status on HiPPI board, mask to the 3 LSB */
        status = (inb(HIPPI_INTR_STATUS) & 0x0007);
	
	if (status) {

		s = SPLHIPPI_NOI();    /* splnet_noi */
		baton_enter();

		if (status & HIPPI_CTLR_INTR)
			hctlr_ctr_i(unit,status);
		if (status & HIPPI_SRC_INTR)
			hctlr_src_i(unit,status);
		if (status & HIPPI_DST_INTR)
			hctlr_dst_i(unit,status);

		baton_exit();
		return s;
	}
	hippi_spurious_interrupts++;
	if (show_spurious_interrupts) {
		printf("hippi_intr: spurious interrupt: status=0x%04x\n",
			status);
	}
	return s;
}

/*
 *	Map in the stats counters.
 */
vm_offset_t
hippi_mmap(dev, off, prot)
dev_t           dev;
vm_offset_t     off;
vm_prot_t       prot;
{
	return (i860_btop(HIPPI_PERF_COUNTERS_BASE_ADDR));
}

#endif	NHIPPI > 0
