/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * $Log: sdb.c,v $
 * Revision 1.3.8.1  1995/06/11  18:33:23  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.3  1995/03/28  01:11:51  jerrie
 * Added SCSI16_CONFIGURATION bootmagic to control software work arounds for
 * missing hardware features.  Added sdb_open routine for performance counters.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Tested bootmagic by resetting with various settings.
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:43:36  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	sdb.c
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 *	SCSI-16 daughter board driver
 *
 *	Based on the MIO board driver written by Andy Pfiffer
 */


#include <sdb.h>
#if	NSDB > 0

#include <sdbs.h>
#include <siop.h>
#include <sdbe.h>

#include <sys/types.h>
#include <vm/vm_kern.h>
#include <chips/busses.h>

#include <i860/coff.h>
#include <i860/syms.h>

#include <i860paragon/lbus.h>
#include <i860paragon/dram.h>
#include <i860paragon/baton.h>
#include <i860paragon/expansion.h>

#include <i860paragon/sdb/sdb.h>
#include <i860paragon/sdb/sdb_dev.h>
#include <i860paragon/sdb/sdb_flash.h>


caddr_t	sdb_std[NSDB_DEVS] = {
	(caddr_t)SDB_SRAM_START,
	(caddr_t)SDB_DPRAM_START,
	(caddr_t)SDB_TOPOF_FLASH
};

struct bus_device	*sdb_dinfo[NSDB_DEVS];
struct bus_ctlr		*sdb_minfo[NSDB_DEVS];
int	sdb_probe(), sdb_slave(), sdb_attach(), sdb_intr();

struct bus_driver sdb_driver = {
	sdb_probe,		/* probe  - is the sdb attached ?	 */
	sdb_slave,		/* slave  - any devices on the card ?	 */
	sdb_attach,		/* attach - setup driver after the probe */
	0,			/* go	  - start operation		 */
	sdb_std,		/* addr	  - device csr addresses	 */
	"sdb",			/* dname  - device driver name		 */
	sdb_dinfo,		/* dinfo  - bus_device backpointers	 */
	"sdb",			/* mname  - name of controller		 */
	sdb_minfo,		/* minfo  - bus_ctlr backpointers	 */
	0			/* flags  - flags			 */
};

extern void		outb();
extern void		outw();
extern void		outl();
extern unsigned char	inb();
extern unsigned short	inw();
extern unsigned long	inl();

extern int		show_spurious_interrupts;
int			sdb_spurious_interrupts;

int			show_sdb_parity_interrupts = 1;
int			sdb_parity_interrupts;

int			show_sdb_lan_parity_interrupts = 1;
int			sdb_lan_parity_interrupts;

/*
 * The sdb_configuration variable allows control of the
 * software and hardware initialization via bootmagic.
 */
int			sdb_configuration;

static boolean_t	sdb_probed_okay;

static int		(*previous_handler)();
static int		(*previous_reboot)();


/*
 * Return non-zero iff a SCSI-16 daughter card is present.
 */
sdb_probe(vaddr, ctlr)
	caddr_t		vaddr;
	struct bus_ctlr	*ctlr;
{
	extern int	expansion_id();

	/* check for a SCSI-16 expansion board type */
	if (expansion_id() != EXP_ID_SCSI)
		return 0;

	/*
	 * Read the SCSI-16 configuration bootmagic
	 */
	sdb_configuration = getbootint("SCSI16_CONFIGURATION", 0);

	/* initialize the SCSI-16 board registers to default */
	outw(SDB_IMASK,	  SDB_IMASK_DEFAULT);
	if (sdb_configuration & SDB_CONFIGURATION_DISABLE_DMA) {
		/* No DMA, bypass dual-port  */
		outw(SDB_DIAG, SDB_DIAG_BYPASS);
	} else {
		/* Enable SCSI DMA transfers */
		outw(SDB_DIAG, 0);
	}
	outw(SDB_CONTROL, inw(SDB_CONTROL) | SDB_CONTROL_DEFAULT);
	outw(SDB_ISTAT,	  SDB_ISTAT_DEFAULT);

	sdb_interrupt_init();

	if (!(sdb_configuration & SDB_CONFIGURATION_IGNORE_PARITY)) {
		/* enable SCSI-16 expansion board slave parity interrupts */
		sdb_interrupt_enable(SDB_IMASK_PARITY);
		/* enable SCSI-16 expansion board LAN parity interrupts */
		sdb_interrupt_enable(SDB_IMASK_ETHER_PARITY);
	}

	sdb_reboot_init();

	sdb_probed_okay = TRUE;
	return 1;
}


/*
 * Probe for any slave devices on the SCSI-16 daughter card.
 * In this case, the slaves are the 512K SRAM, 32K Dual Port
 * RAM, and the 256K flash.
 */
sdb_slave(device, vaddr)
	struct bus_device	*device;
	caddr_t			vaddr;
{
	switch (device->unit) {

		case SDB_DEV_SRAM:
			return sdb_probe_sram(device);

		case SDB_DEV_DPRAM:
			return sdb_probe_dpram(device);

		case SDB_DEV_FLASH:
			return sdb_probe_flash(device);

		default:
			break;
	}

	printf("sdb_slave: bad unit number (unit == %d)\n", device->unit);
	return 0;
}


/*
 * Map in the memory on the SCSI-16 daughter card.
 */
sdb_attach(device)
	struct bus_device	*device;
{
	vm_offset_t	sdb_map_memory();
	vm_offset_t	vaddr;
	char		*flavor;
	boolean_t	clear;

	switch (device->unit) {

		case SDB_DEV_SRAM:
			flavor = "sram";
			clear = TRUE;
			break;

		case SDB_DEV_DPRAM:
			flavor = "dpram";
			clear = TRUE;
			break;

		case SDB_DEV_FLASH:
			flavor = "flash";
			clear = FALSE;
			break;

		default:
			flavor = "unknown";
			clear = FALSE;
			break;
	}

	vaddr = sdb_map_memory(device->phys_address, device->sysdep);
	if (vaddr == 0) {
		return 0;
	}
	device->address = (caddr_t)vaddr;

	sdb_dinfo[device->unit] = device;

	printf(" virt 0x%x, phys 0x%x, size %3uKB (%s)",
		device->address,
		device->phys_address,
		((int)device->sysdep / 1024),
		flavor);

	if (clear) {
		sdbzero(device->address, device->sysdep);
	}

	return 0;
}


/*
 * Setup for taking an interrupt from the SCSI-16 daughter card.
 */
sdb_interrupt_init()
{
	extern int	interrupt_expansion_install();

	/*
	 * Install a handler for interrupts from the expansion card.
	 */
	if (interrupt_expansion_install(sdb_intr, &previous_handler))
		panic("sdb_interrupt_init");

	return 0;
}


/*
 * Install a function pointer to reload and run
 * from expansion ROM at reboot time.
 */
sdb_reboot_init()
{
	static void	sdb_reboot_from_flash();

	if (reboot_expansion_install(sdb_reboot_from_flash, &previous_reboot))
		panic("sdb_reboot_init");

	return 0;
}


/*
 * Set bits in the interrupt mask register.
 */
sdb_interrupt_disable(bits)
	int	bits;
{
	unsigned short	mask;

	mask = inw(SDB_IMASK) | bits;
	outw(SDB_IMASK, mask);

	return bits;
}

/*
 * Clear bits in the interrupt mask register.
 */
sdb_interrupt_enable(bits)
	int	bits;
{
	unsigned short	mask;

	mask = inw(SDB_IMASK) & ~bits;
	outw(SDB_IMASK, mask);

	return bits;
}


/*
 * Dispatch an interrupt from the SCSI-16 daughter card.
 */
sdb_intr(regs)
	register struct i860_saved_state *regs;
{
	unsigned short	status, mask;
	int		s = -1;

	status = inw(SDB_ISTAT);
	mask   = inw(SDB_IMASK);

#if	NSDBS > 0
	if ((status & SDB_ISTAT_SERIAL) && !(mask & SDB_IMASK_SERIAL)) {
		s = splsdb_noi();
		baton_enter();
		sdbs_intr(0);
		baton_exit();
		return s;
	}
#endif

	if (sdb_configuration & SDB_CONFIGURATION_DISABLE_DMA) {

		/*
		 * SCSI DMA is *not* enabled
		 */

#if	NSIOP > 0
		if ((status & SDB_ISTAT_SCSI0) && !(mask & SDB_IMASK_SCSI0)) {
			s = splsdb_noi();
			baton_enter();
			siop_intr(0);
			baton_exit();
			return s;
		}

#if	NSIOP > 1
		if ((status & SDB_ISTAT_SCSI1) && !(mask & SDB_IMASK_SCSI1)) {
			s = splsdb_noi();
			baton_enter();
			siop_intr(1);
			baton_exit();
			return s;
		}
#endif	NSIOP > 1
#endif	NSIOP > 0

	} else {

		/*
		 * SCSI DMA *is* enabled
		 */

#if	NSIOP > 0
		if ((status & SDB_ISTAT_DMA0) && !(mask & SDB_IMASK_DMA0)) {
			s = splsdb_noi();
			baton_enter();
			outw(SDB_ISTAT, SDB_ISTAT_DMA0); /* clear sticky bit */
			siop_intr(0);
			baton_exit();
			return s;
		}

#if	NSIOP > 1
		if ((status & SDB_ISTAT_DMA1) && !(mask & SDB_IMASK_DMA1)) {
			s = splsdb_noi();
			baton_enter();
			outw(SDB_ISTAT, SDB_ISTAT_DMA1); /* clear sticky bit */
			siop_intr(1);
			baton_exit();
			return s;
		}
#endif	NSIOP > 1
#endif	NSIOP > 0

	}

#if	NSDBE > 0
	if ((status & SDB_ISTAT_ETHER) && !(mask & SDB_IMASK_ETHER)) {
		s = splsdb_noi();
		baton_enter();
		outw(SDB_ISTAT, SDB_ISTAT_ETHER);	 /* clear sticky bit */
		sdbe_intr(0);
		baton_exit();
		return s;
	}
#endif

	if ((status & SDB_ISTAT_PARITY) && !(mask & SDB_IMASK_PARITY)) {
		sdb_parity_interrupts++;
		if (show_sdb_parity_interrupts) {
			printf("%s %d: byte lanes = 0x%02x\n",
				"sdb_intr: slave parity interrupt",
				sdb_parity_interrupts, inw(SDB_STATUS) >> 8);
		}
		outw(SDB_ISTAT, SDB_ISTAT_PARITY);	 /* clear sticky bit */
		return s;
	}

	if ((status & SDB_ISTAT_ETHER_PARITY) &&
	    !(mask  & SDB_IMASK_ETHER_PARITY)) {
		sdb_lan_parity_interrupts++;
		if (show_sdb_lan_parity_interrupts) {
			printf("sdb_intr: LAN parity interrupt %d\n",
				sdb_lan_parity_interrupts);
		}
		outw(SDB_ISTAT, SDB_ISTAT_ETHER_PARITY); /* clear sticky bit */
		return s;
	}

	sdb_spurious_interrupts++;
	{
		/*
		 * XXX Hardware debug... these could be removed.
		 */
		extern int	la_trigger_spurious;	/* XXX */

		if (la_trigger_spurious)		/* XXX */
			outw(SDB_TRIGGER, 0);		/* XXX */

	}
	if (show_spurious_interrupts) {
		printf("%s %d: status = 0x%04x, mask = 0x%04x\n",
			"sdb_intr: spurious interrupt",
			sdb_spurious_interrupts, status, mask);
	}

	return s;
}


/*
 * Map in a chunk of SCSI-16 memory
 */
vm_offset_t
sdb_map_memory(paddr, len)
	vm_offset_t		paddr;
	vm_size_t		len;
{
	extern vm_offset_t	pmap_map_bd();
	vm_offset_t		vaddr, v;

	if (!sdb_probed_okay)
		return 0;

	paddr = (vm_offset_t)trunc_page(paddr);
	len   = (vm_size_t)round_page(len);

	/*
	 * No, this isn't really pageable... this call
	 * simply carves out a chunk of address space that
	 * will very shortly be filled by physical memory
	 * on the SCSI-16 daughter card.
	 */
	if (kmem_alloc_pageable(kernel_map, &vaddr, len) != KERN_SUCCESS) {
		return (vm_offset_t)0;
	}

	/*
	 * Enter the physical pages into the kernel map.
	 * pmap_map_bd() marks the pages as referenced,
	 * modified, and cache-disable.
	 */
	(void)pmap_map_bd(vaddr, paddr, paddr + len, VM_PROT_WRITE);
	for (v = vaddr; v < v + len; v += PAGE_SIZE) {
		pmap_change_wiring(kernel_pmap, v, TRUE);
	}

	return vaddr;
}


/*
 * Probe and autosize the Static RAM on the SCSI-16 daughter card.
 *
 * XXX doesn't autosize yet.
 * XXX doesn't probe either.
 */
static int
sdb_probe_sram(device)
	struct bus_device	*device;
{
	return 1;
}

/*
 * Probe and autosize the Dual Port RAM on the SCSI-16 daughter card.
 *
 * XXX doesn't autosize yet.
 * XXX doesn't probe either.
 */
static int
sdb_probe_dpram(device)
	struct bus_device	*device;
{
	return 1;
}

/*
 * Probe and autosize the Flash(tm) EEPROM on the SCSI-16 daughter card.
 *
 * XXX doesn't autosize yet.
 * XXX doesn't probe either.
 */
static int
sdb_probe_flash(device)
	struct bus_device	*device;
{
	return 1;
}


/*
 * Shut down the software and as much of the CPU as possible;
 * copyout the flash from the SCSI-16 daugther card and call it.
 *
 * XXX The danger is that the current kernel stack that
 * XXX we're running on will be occupied by the flash.
 *
 * XXX CAN ONLY ACCESS FLASH VIA 8-bit, 16-bit, OR 32-bit LOADS
 */
static void
sdb_reboot_from_flash()
{
	char		*src, *dst, *flash;
	char		*bss, *entry;
	struct filehdr	fh;
	struct scnhdr	sh, *scn;
	struct aouthdr	ah;
	int		i, offset;

	/*
	 * XXX This assumes the coff file header will start at the
	 * XXX phys_address in the bus_device structure (which was
	 * XXX mapped to address by the attach routine).
	 */
	flash = (char *)(sdb_dinfo[SDB_DEV_FLASH]->address);

	sdbcopy((char *)flash, &fh, sizeof(fh));
	if (fh.f_magic != I860MAGIC)
		return;

	sdbcopy((char *)flash + sizeof(fh), &ah, sizeof(ah));

	/*
	 * Copy out each section.
	 */
	offset = sizeof(fh) + fh.f_opthdr;
	scn = (struct scnhdr *)(flash + offset);
	for (i = 0; i < fh.f_nscns; i++, scn++) {
		sdbcopy(scn, &sh, sizeof(sh));
		if (sh.s_flags & (STYP_TEXT|STYP_DATA)) {
			src = flash + sh.s_scnptr;
			dst = (char *)sh.s_vaddr;
			dst = (char *)((long)dst & ~(0x0F << 24) |
				DRAM_HIGH_ALIAS);
			sdbcopy(src, dst, sh.s_size);
		} else if (sh.s_flags & STYP_BSS) {
			dst = (char *)sh.s_vaddr;
			dst = (char *)((long)dst & ~(0x0F << 24) |
				DRAM_HIGH_ALIAS);
			bzero(dst, sh.s_size);
		} else {
			/* we don't get it... */
		}
	}

	/*
	 * Flush the caches, disable address translation,
	 * set the stack pointer to 0xc000ffc0, and call
	 * the SCSI-16 bootstrap with a single parameter of -1.
	 */
	flush();
	printf("flushed and rebooting...\n");
	outl(0xc0000ffc, node_control_register_read());
	i860_reboot(ah.entry, 0xc0000ff0, 0);
}


/*
 * sdbcopy - bcopy like interface
 *	supports: 4 byte transfer maximum
 *		  typical 6 byte ether address copies
 *		  and other odd transfer sized transfers
 *		
 */
#define THRES_HOLD	16		/* Educated guess */
sdbcopy(src, dst, count)
        register caddr_t src;
        register caddr_t dst;
        register int     count;
{
	/*
	 * The maximum SCSI-16 SRAM access can only be 32 bits wide.
	 * The bcopy function is optimized to do doubles and quads.
	 * Hence, this function.
	 */

	if (count == 6) {
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = src[3];
		dst[4] = src[4];
		dst[5] = src[5];
	}
	else if (count > THRES_HOLD &&
		 ((u_short)src & 3) == ((u_short)dst & 3)) {
		/*
		 * Large transfers
		 */
		while (((u_short)src & 3) && count-- > 0) /* Align to 4 bytes */
			*dst++ = *src++;
		while (count > 3) {			/* Copy in chunks */
			count -= sizeof(u_long);
			*(u_long *)dst = *(u_long *)src;
			dst += sizeof(u_long);
			src += sizeof(u_long);
		}
		while (count-- > 0)			/* Finish it off */
			*dst++ = *src++;
	} else {
		/*
		 * Everything else...
		 */
		while (count-- > 0)
			*dst++ = *src++;
	}
}


/*
 * sdbzero - bzero like interface
 *	supports: 4 byte maximum and other odd sizes
 *		
 */
sdbzero(dst, count)
        register caddr_t dst;
        register int     count;
{
	/*
	 * The maximum SCSI-16 SRAM access can only be 32 bits wide.
	 * The bcopy function is optimized to do doubles and quads.
	 * Hence, this function.
	 */

	if (count > THRES_HOLD) {
		/*
		 * Large clears
		 */
		while (((u_short)dst & 3) && count-- > 0) /* Align to 4 bytes */
			*dst++ = 0;
		while (count > 3) {			/* Clear in chunks */
			count -= sizeof(u_long);
			*(u_long *)dst = 0;
			dst += sizeof(u_long);
		}
		while (count-- > 0)			/* Finish it off */
			*dst++ = 0;
	} else {
		/*
		 * Everything else...
		 */
		while (count-- > 0)
			*dst++ = 0;
	}
}


/*
 * Reset the SCSI processor
 */
sdb_scsi_reset(unit)
	int	unit;
{
	int	s, siop;

	if ((unit != 0) && (unit != 1)) {
		printf("sdb_scsi_reset: bad unit number (unit == %d)\n", unit);
		return 0;
	}
	siop = (unit == 0) ? SDB_CONTROL_SCSI0_RESET : SDB_CONTROL_SCSI1_RESET;

	s = sploff();
	outw(SDB_CONTROL, inw(SDB_CONTROL) |  siop);
	delay(10);
	outw(SDB_CONTROL, inw(SDB_CONTROL) & ~siop);
	splon(s);

	return 0;
}


/*
 * Reset the Serial Controller
 */
sdb_serial_reset(unit)
	int	unit;
{
	int	s;

	s = sploff();
	outw(SDB_CONTROL, inw(SDB_CONTROL) |  SDB_CONTROL_SERIAL_RESET);
	delay(10);
	outw(SDB_CONTROL, inw(SDB_CONTROL) & ~SDB_CONTROL_SERIAL_RESET);
	splon(s);

	return 0;
}


/*
 * Dump interesting board register values for debugging
 */
sdb_dump()
{
	extern int	node_status_register_expansion_present();
	extern int	expansion_id();
	unsigned char	enet_addr[6];

	/* check for any old expansion board */
	if (!node_status_register_expansion_present()) {
		db_printf("Expansion board not found\n");
		db_printf("Node Status Register: 0x%08x\n",
			node_status_register_read());
		return 0;
	}

	/* check for a SCSI-16 expansion board type */
	if (expansion_id() != EXP_ID_SCSI) {

		db_printf("SCSI-16 Daughter Board not installed\n");
		db_printf("Expansion ID: 0x%02x ", expansion_id());

		return 0;
	}

	/*
	 * Ok, we've got a SCSI-16 daughter board, start dumping registers
	 */
	db_printf("SCSI-16 Daughter Board Registers:\n\n");

	/* Control Register */
	db_printf("\tControl:\t\t0x%04x\n", inw(SDB_CONTROL));

	/* Status Register */
	db_printf("\tStatus:\t\t\t0x%04x\n", inw(SDB_STATUS));

	/* Interrupt Status Register */
	db_printf("\tInterrupt Status:\t0x%04x\n", inw(SDB_ISTAT));

	/* Interrupt Mask Register */
	db_printf("\tInterrupt Mask:\t\t0x%04x\n", inw(SDB_IMASK));

	/* FPGA Configuration Register */
	db_printf("\tFPGA Configuration:\t0x%04x\n", inw(SDB_FPGA_CONFIG));

	/* Diagnostic Control Register */
	db_printf("\tDiagnostic Control:\t0x%04x\n", inw(SDB_DIAG));

	/* Ethernet Address */
	db_printf("\n");
	if (x24c02_read(enet_addr,sizeof(enet_addr),0,0) != sizeof(enet_addr)) {
	    db_printf("X24C02 read error - ethernet address\n");
	} else {
	    db_printf("\tEthernet Address:\t%02x:%02x:%02x:%02x:%02x:%02x\n",
		enet_addr[0], enet_addr[1], enet_addr[2],
		enet_addr[3], enet_addr[4], enet_addr[5]);
	}

	/* Performance Counter Registers */
	db_printf("\n");
	db_printf("\tEnet Bus Utilization:\t\t\t0x%08x\n",
		inl(SDB_PERF_ETHER));
	db_printf("\tSIOP 0 Bus Utilization:\t\t\t0x%08x\n",
		inl(SDB_PERF_SCSI0));
	db_printf("\tSIOP 1 Bus Utilization:\t\t\t0x%08x\n",
		inl(SDB_PERF_SCSI1));
	db_printf("\tLocal Bus Idle Time:\t\t\t0x%08x\n",
		inl(SDB_PERF_IDLE));
	db_printf("\tNode Utilization of Daughter Board:\t0x%08x\n",
		inl(SDB_PERF_NODE));
	db_printf("\tDaughter Board Utilization of Node:\t0x%08x\n",
		inl(SDB_PERF_SDB));

	return 0;
}


/************************************
 * Device driver interface routines *
 ************************************/

#include <device/io_req.h>

/*
 * Return D_SUCCESS iff a SCSI-16 daughter card is present
 */
sdb_open(dev, mode, ior)
	dev_t		dev;
	dev_mode_t	mode;
	io_req_t	ior;
{
	extern int	expansion_id();

	/* check for a SCSI-16 expansion board type */
	if (expansion_id() != EXP_ID_SCSI)
		return D_NO_SUCH_DEVICE;

	return D_SUCCESS;
}

/*
 * Map in the performance counters
 */
vm_offset_t
sdb_mmap(dev, off, prot)
	dev_t           dev;
	vm_offset_t     off;
	vm_prot_t       prot;
{
	/*
	 *  No way to return an error on an invalid offset so default
	 *  to the physical address of the lowest performance counter
	 */
	return i860_btop(SDB_PERF_START);
}

#endif	NSDB > 0
