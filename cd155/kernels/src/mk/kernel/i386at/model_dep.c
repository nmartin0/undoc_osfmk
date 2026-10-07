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
 * Copyright (c) 1991,1990,1989, 1988 Carnegie Mellon University
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
 * HISTORY
 * 28-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Probe EISA bus.
 *	Added HIMEM support
 *
 * $Log: model_dep.c,v $
 * Revision 1.5  1994/11/18  20:36:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:27:31  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:26:26  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:26:19  dleslie
 * First R1_0 release
 *
 * Revision 2.6.7.5  92/09/15  17:15:33  jeffreyh
 * 	Fix to be able to compile STD+COMPAQ (Single cpu).
 * 	Fixed garbled comments.
 * 	[92/06/09            bernadat]
 * 
 * Revision 2.6.7.4  92/04/30  11:56:56  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * 	Get the boot string passed by the boot and initialize
 * 	server name and arguments if typed at boot prompt.
 * 	Convert RB_ASKNAME to RB_ASKNAME << RB_SHIFT for the server
 * 	and use RB_HALT for initial halt. 
 * 	Detect number of cpus for Corollary before overwriting
 * 	boothowto with RB_ASKNAME << RB_SHIFT
 * 	[92/03/19            bernadat]
 * 
 * Revision 2.6.7.3  92/03/28  10:06:45  jeffreyh
 * 	Comment out line that does a warm reboot instead of a full
 * 	reboot. This fixes problems with vga cards that do not reset
 * 	correctly. It would be nice to find a way to do a soft reset
 * 	on all machines.
 * 	[92/03/10            jeffreyh]
 * 
 * Revision 2.6.7.2  92/03/03  16:17:40  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:39:10  jeffreyh]
 * 
 * Revision 2.7  92/01/03  20:11:49  dbg
 * 	Fixed so that mem_size can be patched to limit physical memory
 * 	use.
 * 	[91/09/29            dbg]
 * 
 * 	Rename kdb_init to ddb_init.
 * 	[91/09/11            dbg]
 * 
 * Revision 2.6.7.1  92/02/18  18:56:57  jeffreyh
 * 	Moved initialization of debug_user_with_kdb to FALSE to
 * 	kern/exception.c file
 * 	[91/12/09            bernadat]
 * 
 * 	Added call to cbus_stack_alloc for Corollary
 * 	[91/08/30            bernadat]
 * 
 * 	Support for Corollary MP
 * 	Added code for -h (halt option )
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.6  91/07/31  17:42:41  dbg
 * 	Remove call to pcb_module_init (in machine-independent code).
 * 	[91/07/26            dbg]
 * 
 * Revision 2.5  91/06/19  11:55:48  rvb
 * 	cputypes.h->platforms.h
 * 	[91/06/12  13:45:37  rvb]
 * 
 * Revision 2.4  91/05/18  14:30:38  rpd
 * 	Changed pmap_bootstrap arguments.
 * 	Moved pmap_free_pages and pmap_next_page here.
 * 	[91/05/15            rpd]
 * 
 * Revision 2.3  91/05/14  16:29:13  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:44:52  dbg
 * 	Initialization for i386 AT bus machines only.
 * 	Combine code that was in i386/init.c and i386/i386_init.c.
 * 	[91/04/26  14:40:43  dbg]
 * 
 * Revision 2.3  90/09/23  17:45:10  jsb
 * 	Added support for iPSC2.
 * 	[90/09/21  16:39:41  jsb]
 * 
 * Revision 2.2  90/05/03  15:27:39  dbg
 * 	Alter for pure kernel.
 * 	[90/02/15            dbg]
 * 
 * Revision 1.5.1.4  90/02/01  13:36:37  rvb
 * 	esym must always be defined.  This is as good a place as any.
 * 	[90/01/31            rvb]
 * 
 * Revision 1.5.1.3  89/12/28  12:43:10  rvb
 * 	Fix av_start & esym initialization, esp for MACH_KDB.
 * 	[89/12/26            rvb]
 * 
 * Revision 1.5.1.2  89/12/21  17:59:49  rvb
 * 	enable esym processing.
 * 
 * 
 * Revision 1.5.1.1  89/10/22  11:30:41  rvb
 * 	Setup of rootdevice should not be here.  And it was wrong.
 * 	[89/10/17            rvb]
 * 
 * 	Scary!  We've changed sbss to edata.  AND the coff loader
 * 	following the vuifile spec was actually aligning the bss 
 * 	on 4k boundaries.
 * 	[89/10/16            rvb]
 * 
 * Revision 1.5  89/04/05  12:57:39  rvb
 * 	Move extern out of function scope for gcc.
 * 	[89/03/04            rvb]
 * 
 * Revision 1.4  89/02/26  12:31:25  gm0w
 * 	Changes for cleanup.
 * 
 * 31-Dec-88  Robert Baron (rvb) at Carnegie-Mellon University
 *	Derived from MACH2.0 vax release.
 *
 */

/*
 *	File:	model_dep.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Copyright (C) 1986, Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Basic initialization for I386 - ISA bus machines.
 */

#include <cpus.h>
#include <platforms.h>
#include <mach_kdb.h>
#include <eisa.h>
#include <himem.h>

#include <mach/i386/vm_param.h>

#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <mach/machine.h>
#include <kern/time_out.h>
#include <sys/time.h>
#include <vm/vm_page.h>

#include <i386at/mp/mp.h>
#include <i386at/boot/boot.h>
#include <i386at/eisa.h>

#if	CBUS
#include <cbus/cbus.h>
#endif	CBUS

#if	MBUS
#include <mbus/mbus.h>
#endif	MBUS

#if	MACH_KDB
#include <sys/reboot.h>
#endif	MACH_KDB

int		loadpt;

vm_size_t	mem_size = 0;
vm_size_t	rawmem_size;
vm_offset_t	first_addr = 0;	/* set by start.s - keep out of bss */
vm_offset_t	first_avail = 0;/* first after page tables */
vm_offset_t	last_addr;

vm_offset_t	avail_start, avail_end;
vm_offset_t	virtual_avail, virtual_end;
vm_offset_t	hole_start, hole_end;
vm_offset_t	avail_next;
unsigned int	avail_remaining;

/* parameters passed from bootstrap loader */
int		cnvmem = 0;		/* must be in .data section */
int		extmem = 0;
int		boottype = 0;
char *		esym = (char *)0;

extern char	edata, end;

extern char	version[];

extern void	setup_main();

void		inittodr();	/* forward */

int		rebootflag = 0;	/* exported to kdintr */

extern char	*startup_name, *startup_args;
char		boot_args[BOOT_LINE_LENGTH];

#define	isspace(c)	((c) == ' ' || (c) == '\t')

/*
 *	Cpu initialization.  Running virtual, but without MACH VM
 *	set up.  First C routine called.
 */
void machine_startup()
{
	/*
	 * Do basic VM initialization
	 */
	i386_init();

	/*
	 * Initialize the console so we can print.
	 */
	cninit();


	if (boothowto & RB_ASKNAME) {
		boothowto |= (RB_ASKNAME << RB_SHIFT);
		boothowto &= ~RB_ASKNAME;
	}

#if	MACH_KDB

	/*
	 * Initialize the kernel debugger.
	 */
	ddb_init();

	/*
	 * Cause a breakpoint trap to the debugger before proceeding
	 * any further if the proper option bit was specified in
	 * the boot flags.
	 *
	 * XXX use -a switch to invoke kdb, since there's no
	 *     boot-program switch to turn on RB_HALT!
	 */

	if ((boothowto & RB_HALT) && (boothowto & RB_KDB)) {
		printf("inline call to debugger(machine_startup)\n");
	        Debugger();
	}
#endif	MACH_KDB

	printf(version);

	machine_slot[0].is_cpu = TRUE;
	machine_slot[0].running = TRUE;
	machine_slot[0].cpu_type = CPU_TYPE_I386;
	machine_slot[0].cpu_subtype = CPU_SUBTYPE_AT386;

	/*
	 * Start the system.
	 */
#if	NCPUS > 1
	mp_desc_init(0);
#endif	NCPUS
	setup_main();
}

/*
 * Find devices.  The system is alive.
 */
void machine_init()
{
	/*
	 * Set up to use floating point.
	 */
	init_fpu();

	/*
	 * Look for eisa bus
	 */

	probe_eisa();

	/*
	 * Find the devices
	 */
	probeio();

	/*
	 * Find the root device
	 */
	get_root_device();

	/*
	 * Get the time
	 */
	inittodr();
}

/*
 * Halt a cpu.
 */
halt_cpu()
{
}

/*
 * Halt the system or reboot.
 */
halt_all_cpus(reboot)
	boolean_t	reboot;
{
	if (reboot) {
	    /*
	     * Tell the BIOS not to clear and test memory.
	     */
/*	    *(unsigned short *)phystokv(0x472) = 0x1234; */

	    kdreboot();
	}
	else {
	    rebootflag = 1;
	    printf("In tight loop: hit ctl-alt-del to reboot\n");
	    (void) spl0();
	}
	for (;;)
	    continue;
}

/*
 * Basic VM initialization.
 */
i386_init()
{

	/*
	 * Zero the BSS.
	 */
	bzero((char *)&edata,(unsigned)(&end - &edata));

	/*
 	 * Get the boot string. It is located just after symbols.
	 *
	 * The general syntax is:
	 *
	 * hd(0,a)/mach_kernel:server -a|-h|-s|-d|-<digit> server_options
	 *
	 * Almost every field is optional.
	 *
	 * First we have to find the optional server name delimited by ':'
	 * and initialize startup_name.
	 *
	 * Then pass to the server, all the arguments following the
	 * kernel:server optional string
	 */	

	strcpy(startup_args = boot_args, esym);
	
	/* skip spaces and tabs */

	while(*startup_args && isspace(*startup_args)) 
		startup_args++;

	/* look for server name */

	if (*startup_args != '-')
		while(*startup_args && !isspace(*startup_args))
			if (*startup_args++ == ':') {
				startup_name = startup_args;
				/* null terminate the server name */
				while (*startup_args && !isspace(*startup_args))
					startup_args++;
				*startup_args++ = '\0';
				/* startup args now points to server args */
				break;
			}

	/*
	 * Initialize the pic prior to any possible call to an spl.
	 */
	picinit();

	vm_set_page_size();


	/*
	 * Compute the memory size.
	 */

#if	defined(CBUS) || defined(MBUS)

#if	CBUS
	loadpt = CBUS_START;
	first_addr = round_page(CBUS_START);
	last_addr = CBUS_START + cbus_memsz() * MB(1);
#endif	CBUS

#if	MBUS
	/*
	 * XXX
	 * Figuring out how much real memory there is on a SystemPro
	 * is tough. You must use BIOS int calls to probe the EISA
	 * bus for each board.
	 * For the present time I assume that two configurations exist:
	 * Base machine with 8 Megs
	 * Base machine with 8 Megs + 32 Megs Memory extension
	 * For the second configuration extmem is slightly less than 16 Megs.
	 */
	 
	first_addr = 0;
	if (extmem > (8*1024))	
	  	extmem += (24 * 1024);	
	last_addr = 1024*1024 + extmem*1024;
#endif	MBUS

#else	defined(CBUS) || defined(MBUS)

	first_addr = 0x1000;
		/* BIOS leaves data in low memory */
	last_addr = 1024*1024 + extmem*1024;
		/* extended memory starts at 1MB */
       
#endif	defined(CBUS) || defined(MBUS)

#if	NCPUS > 1

	/*
	 * Do not use the 2 first memory pages, they are used to
	 * boot the other cpus
	 */
	first_addr += 0x2000;

#endif	NCPUS > 1

	if (mem_size != 0) {
	    if (mem_size < last_addr - loadpt)
		last_addr = loadpt + mem_size;
	}
	mem_size = last_addr - loadpt;

	first_addr = round_page(first_addr);
	last_addr = trunc_page(last_addr);

	/*
	 *	Initialize kernel physical map, mapping the
	 *	region from loadpt to avail_start.
	 *	Kernel virtual address starts at VM_KERNEL_MIN_ADDRESS.
	 */

	avail_start = first_addr;
	avail_end = last_addr;

#if	NCPUS > 1 && AT386
	/*
	 * Must Allocate interrupt stacks before kdb is called and also
	 * before vm is initialized. Must find out number of cpus first.
	 */

	mp_probe_cpus();
	interrupt_stack_alloc();

#endif	NCPUS > 1 && AT386

	printf("AT386 boot: memory from 0x%x to 0x%x\n", avail_start,
			avail_end);

	pmap_bootstrap(loadpt);

	/*
	 *	Initialize for pmap_free_pages and pmap_next_page.
	 *	These guys should be page-aligned.
	 */

#ifdef	CBUS
	hole_start = CBUS_START + MB(1);
#else	CBUS
	hole_start = trunc_page((vm_offset_t)(1024 * cnvmem));
#endif	CBUS
	hole_end = round_page((vm_offset_t)first_avail);

	avail_remaining = atop((avail_end - avail_start) -
			       (hole_end - hole_start));
#if	MBUS
	avail_remaining -= atop(MBUS_BIOS_REMAP_END-MBUS_BIOS_REMAP_START);
#endif	MBUS

#if	!HIMEM
	avail_next = avail_start;
#else	!HIMEM
	avail_next = avail_end - PAGE_SIZE;
#endif	!HIMEM

	printf("i386_init: virtual_avail = %x, virtual_end = %x\n",
		virtual_avail, virtual_end);
}

#include <mach/vm_prot.h>
#include <vm/pmap.h>
#include <mach/time_value.h>

timemmap(dev,off,prot)
	vm_prot_t prot;
{
	extern time_value_t *mtime;

#ifdef	lint
	dev++; off++;
#endif	lint

	if (prot & VM_PROT_WRITE) return (-1);

	return (i386_btop(pmap_extract(pmap_kernel(), (vm_offset_t) mtime)));
}

startrtclock()
{
	clkstart();
}

void
inittodr()
{
	time_value_t	new_time;

	new_time.seconds = 0;
	new_time.microseconds = 0;

	(void) readtodc(&new_time.seconds);

	{
	    int	s = splhigh();
	    time = new_time;
	    splx(s);
	}
}

void
resettodr()
{
	writetodc();
}

unsigned int pmap_free_pages()
{
	return avail_remaining;
}

#if	!HIMEM
boolean_t pmap_next_page(addrp)
	vm_offset_t *addrp;
{
	if (avail_next == avail_end)
		return FALSE;

	/* skip the hole */

	if (avail_next == hole_start)
		avail_next = hole_end;

#if	MBUS

	/* skip BIOS ROM remapping */

	if (avail_next == MBUS_BIOS_REMAP_START)
	  	avail_next = MBUS_BIOS_REMAP_END;
#endif	MBUS

	*addrp = avail_next;
	avail_next += PAGE_SIZE;
	avail_remaining--;
	return TRUE;
}

#else	!HIMEM

	/*
	 * In case of HIMEM, let the low memory pages be at head of free
	 * list to prevent use of himem when machine underloaded
	 */
boolean_t pmap_next_page(addrp)
	vm_offset_t *addrp;
{

	if (avail_next < avail_start)
		return FALSE;

	/* skip the hole */

	if (avail_next == hole_end - PAGE_SIZE)
		avail_next = hole_start - PAGE_SIZE;

#if	MBUS

	/* skip BIOS ROM remapping */
	if (avail_next == MBUS_BIOS_REMAP_END - PAGE_SIZE)
	  	avail_next = MBUS_BIOS_REMAP_START - PAGE_SIZE);
#endif	MBUS

	*addrp = avail_next;
	avail_next -= PAGE_SIZE;
	avail_remaining--;
	return TRUE;
}

#endif	!HIMEM

boolean_t pmap_valid_page(x)
	vm_offset_t x;
{
	return (((avail_start <= x) && (x < avail_end)) &&
		!((hole_start <= x) && (x < hole_end)));
}
