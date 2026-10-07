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
 * $Log: model_dep.c,v $
 * Revision 1.5  1994/11/18  20:40:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:34:57  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1992/12/21  23:26:35  terry
 * now handles the bootmagic BOOT_VM_PAGESIZE for changing the
 * soft vm page size at boot time.
 *
 * Revision 1.2  1992/11/24  23:17:54  andyp
 * Updates from OSF found in the norma13.16 tree.
 *
 *	16-Nov-92  Steve Sears (sjs) at Open Software Foundation
 *		Added regs interface for mach debugger commands.
 *
 * Revision 1.1  1992/09/22  18:12:19  regnier
 * Initial revision
 *
 * Revision 2.2.2.5  92/05/27  00:43:08  jeffreyh
 * 	Common values for a shared pmap_bootstrap() between Paragon nodes
 * 	and iPSC/860 nodes.
 * 	[andyp@ssd.intel.com]
 * 	Added ddb commands for MCMSG
 * 	[regnier@ssd.intel.com]
 * 	chip_step mask was 1 bit too big (3F00-->1F00)
 * 	[stans@ssd.intel.com]
 * 
 * Revision 2.2.2.4  92/04/08  15:44:03  jeffreyh
 * 	Add ddb support for machine commands.
 * 	[92/04/08            andyp]
 * 
 * Revision 2.2.2.3  92/03/28  10:08:56  jeffreyh
 * 	Saner vm mapping 
 * 
 * 	[Steven Sears 92/03/17   sjs@osf.org]
 * 	Added RB_VERBOSE flag to enable -v (verbose) flag getting
 * 	passed to the server.
 * 
 * Revision 2.2.2.2  92/03/03  16:18:06  jeffreyh
 * 	Changed kdb_init to ddb_init.
 * 	[92/02/26            jeffreyh]
 * 
 * Revision 2.2.2.1  92/02/18  19:04:02  jeffreyh
 * 	[stan smith] support for new boot config sequence.
 * 	[92/02/13  12:51:47  jeffreyh]
 * 
 * Revision 2.2  91/12/10  16:31:59  jsb
 * 	New files from Intel
 * 	[91/12/10  16:12:19  jsb]
 * 
 * Revision 2.6  91/08/28  11:12:56  jsb
 * 	From intel SSD: Fixed chip_state() and i860_interrupts_on().
 * 	[91/08/26  15:29:38  jsb]
 * 
 * Revision 2.5  91/06/18  20:52:27  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  19:02:12  jsb]
 * 
 * Revision 2.4  91/06/17  15:45:42  jsb
 * 	From Paul Pierce: support for new boot protocol.
 * 	[91/06/17  13:33:44  jsb]
 * 
 * Revision 2.3  91/05/18  14:31:02  rpd
 * 	Moved pmap_valid_page, pmap_free_pages, pmap_next_page here.
 * 	[91/05/17            rpd]
 * 
 * 	Changed pmap_bootstrap arguments.
 * 	[91/03/25            rpd]
 * 
 * Revision 2.2  90/12/04  14:50:08  jsb
 * 	First checkin.
 * 	[90/12/03  21:49:07  jsb]
 * 
 */

#include <platforms.h>
#include <mach_kdb.h>

#include <mach/machine/vm_param.h>

#include <mach/machine.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <vm/vm_page.h>
#include <sys/time.h>
#if	MACH_KDB
#if	DB_MACHINE_COMMANDS
#include <ddb/db_command.h>
#endif	DB_MACHINE_COMMANDS
#endif	MACH_KDB

#include <machine/psl.h>
#include <machine/pmap.h>
#include <i860ipsc/nodehw.h>
#include <i860ipsc/ctlreg.h>

#include <machine/coff.h>

#if	MACH_KDB
#include <sys/reboot.h>
#endif	MACH_KDB

vm_size_t	mem_size;
vm_size_t	rawmem_size;
vm_offset_t	addr_load;
vm_offset_t	addr_base;
vm_offset_t	addr_first;
vm_offset_t	addr_last;

vm_offset_t	avail_start, avail_end;
vm_offset_t	hole_start, hole_end;
vm_offset_t	virtual_avail, virtual_end;
vm_offset_t	avail_next;
unsigned int	avail_remaining;

extern int	paging_enabled;
extern char	version[];

unsigned long	control_shadow = 0;
int	_node_self = -1;

int	reentry = 0;

/*
 * Node information passed to start.s by the NX bootstrap loader.
 */
extern char	*getbootenv();
extern char	**bootenv, **envcopy();
int ipsc_physnode = 0;
int ipsc_basemem = 0;
int ipsc_slot = 0;

#if	MACH_KDB
int boothowto = RB_KDB;
#else	MACH_KDB
int boothowto = 0;
#endif	MACH_KDB

/*
 * End of symbol table.
 */
char *esym = (char *) 0;

char *system_name;

#if	DB_MACHINE_COMMANDS

extern void	db_show_switch_hist();
extern void	db_show_intr_hist();
extern void	db_show_trap_hist();
extern void	db_show_netipc_called_here();
extern void	db_show_vaddrs();
extern void	show_860_regs();
#if	MCMSG
extern void	mcmsg_db_trace();
extern void	mcmsg_db_buf();
extern void	mcmsg_db_avail();
extern void	mcmsg_db_mmem();
extern void	mcmsg_db_mt();
extern void	mcmsg_db_pid();
extern void	mcmsg_db_sel();
extern void	mcmsg_db_item();
extern void	mcmsg_db_xmsg();
extern void	mcmsg_db_xmsg();
extern void	mcmsg_db_nxreq();
#endif	MCMSG

void db_now()
{
	db_printf("%u\n", inb(COUNTER_PORT));
}

struct db_command	i860ipsc_db_commands[] = {
#if	i860
	{ "switchhist",	db_show_switch_hist,	0,	0 },
	{ "intrhist",	db_show_intr_hist,	0,	0 },
	{ "traphist",	db_show_trap_hist,	0,	0 },
	{ "callhere",	db_show_netipc_called_here,	0,	0 },
	{ "now",	db_now,			0,	0 },
	{ "vaddr",	db_show_vaddrs,		0,	0 },
	{ "regs",	show_860_regs,		0,	0 },
#endif	i860
#if	MCMSG
	{ "trace",	mcmsg_db_trace,		0,	0 },
	{ "t",		mcmsg_db_trace,		0,	0 },
	{ "buf",	mcmsg_db_buf,		0,	0 },
	{ "avail",	mcmsg_db_avail,		0,	0 },
	{ "mmem",	mcmsg_db_mmem,		0,	0 },
	{ "mt",		mcmsg_db_mt,		0,	0 },
	{ "pid",	mcmsg_db_pid,		0,	0 },
	{ "sel",	mcmsg_db_sel,		0,	0 },
	{ "item",	mcmsg_db_item,		0,	0 },
	{ "xmsg",	mcmsg_db_xmsg,		0,	0 },
	{ "nxreq",	mcmsg_db_nxreq,		0,	0 },
#endif	MCMSG
	{ (char *) 0,	0,			0,	0 }
};

#endif	DB_MACHINE_COMMANDS

void	inittodr();	/* forward */

node_self()
{
	return _node_self;
}


/*
 *	CPU initialization.
 *	First C routine called.
 */

machine_startup(a0, a1, a2, a3, a4, a5, a6)
	int	a0, a1, a2, a3, a4, a5, a6;
{
	/*
	 * Do basic VM initialization
	 */
	i860_init(a0, a1, a2, a3, a4, a5, a6);

	/*
	 * Initialize the console so we can print.
	 */
	cninit();

#if	MACH_KDB
	/*
	 * Initialize the kernel debugger.
	 */
#if	DB_MACHINE_COMMANDS
	db_machine_commands_install(i860ipsc_db_commands);
#endif	DB_MACHINE_COMMANDS
	ddb_init();

	/*
	 * ignore boothowto and believe bootmagic
	 */
	if (getbootenv("RB_KDB") || getbootenv("RB_860_KDB")) {
		gimmeabreak();
	}
#endif	MACH_KDB

	if (getbootenv("BOOT_860_SHOW_MAGIC") || getbootenv("BOOT_SHOW_MAGIC")){
		int	i;

		for (i = 0; bootenv[i]; i++) {
			printf("%d %x %s\n", i, bootenv[i], bootenv[i]);
		}
	}

	boothowto |= RB_SINGLE;
	if (getbootenv("RB_MULTIUSER") || getbootenv("RB_860_MULTIUSER")) {
		boothowto &= ~RB_SINGLE;
	}
#define	RB_VERBOSE_SERVER	RB_ALTBOOT
	if (getbootenv("RB_VERBOSE") || getbootenv("RB_860_VERBOSE")) {
		boothowto |= RB_VERBOSE_SERVER;
	}

	boothowto |= RB_ASKNAME;
	boothowto |= (RB_ASKNAME << RB_SHIFT);

	printf(version);

	machine_slot[0].is_cpu = TRUE;
	machine_slot[0].running = TRUE;
	machine_slot[0].cpu_type = CPU_TYPE_I860;
	machine_slot[0].cpu_subtype = CPU_SUBTYPE_iPSC860;

	/*
	 * Start the system.
	 */
	i860_interrupts_on();

	setup_main();
	/* NOTREACHED */
}


/*
 * Find devices.  The system is alive.
 */
void machine_init()
{
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
 * Halt the system or reboot.
 */
halt_all_cpus(reboot)
	boolean_t	reboot;
{
	halt_cpu();
}


halt_cpu()
{
	halt();
}


halt()
{
	int	i, k, port;

	k = 0x00100000;
	port = COUNTER_PORT;

	printf("System now halted.\n");
	(void) sploff();
	for (;;) {
		led_red_off();
		led_yellow_on();
		led_green_on();
		while ((inb(port) & k) == 0);
		while ((inb(port) & k) != 0);
		led_red_on();
		led_yellow_off();
		led_green_off();
		while ((inb(port) & k) == 0);
		while ((inb(port) & k) != 0);
	}

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

	if (prot != (VM_PROT_READ)) return (-1);

	return (i860_btop(pmap_extract(pmap_kernel(), (vm_offset_t) mtime)));
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
	return;
}


/*
 * Enable interrupts
 */
i860_interrupts_on()
{
	splon(PSR_IM);
}


/*
 * Program the timer to generate an interrupt
 */
clkstart()
{
	uart_clkstart();
}


size_memory()
{
	extern char	stext;
	char	*override;

	rawmem_size = ipsc_basemem;
	mem_size = rawmem_size;
	addr_base = (vm_offset_t) trunc_page((vm_offset_t) &stext);
	addr_load = trunc_page(&stext);
	addr_first = round_page((int) esym );

	/*
	 * Wayne S. wants to be able to artificially restrict the amount
	 * of memory available to a node.  Okay.
	 */
	if ((override = getbootenv("BOOT_860_MEMSIZE"))) {
		mem_size = atoi(override);
	}
	addr_last = trunc_page(addr_base + mem_size);

	vm_set_page_size();

}


unsigned int  chip_type, chip_step, chip_mode, chip_int, chip_dcs;
unsigned int  chip_lock, chip_prot;

#define XR	1
#define	XP	2
static char *series[] = {
	"??",	/* 0 */
	"XR",	/* 1 */
	"XP"	/* 2 */
};
static char *xr_steppings[] = {
	"~~",	/* 0 */		/* A-step or unknown */
	"B0",	/* 1 */	
	"B1",	/* 2 */
	"B2",	/* 3 */
	"C0",	/* 4 */
	"B3",	/* 5 */	
	"C1",	/* 6 */
	"D0"	/* 7 */
};


chip_state()
{
	unsigned int epsr;

	epsr = get_epsr();
	chip_type = (epsr & 0x000000FF);
	chip_step = ((epsr & 0x00001F00) >> 0x08);
	chip_mode = (epsr & 0x00800000) ;
	chip_int = (epsr & 0x00020000) ;
	chip_dcs = ((epsr & 0x003C0000) >> 18);
	chip_lock = (epsr & 0x00004000);
	chip_prot = (epsr & 0x00008000);

	printf("i860%s-", (chip_type > 2) ? "??" : series[chip_type]);
	switch (chip_type) {
	case XR:
		if (chip_step > 0 && chip_step < 8) {
			printf("%s", xr_steppings[chip_step]);
		} else {
			printf("{XR,step=0x%x}", chip_step);
		}
		break;
	case XP:
	default:
		printf("{step=0x%x}", chip_step);
		break;
	}
	/*printf(", %dMHz", (*((int *) CSR_ADDR_PH) & MHZ_40_33) ? 40 : 33);*/

	printf(", %dK dcache", (0x02 << ((int)(11 + chip_dcs))) >> 10);
	printf(", %s-endian", chip_mode ? "big" : "little");
	printf(", mmu=i%d-mode", chip_prot ? 860 : 386);
	printf(", epsr=0x%x\n", epsr);

	return(0);
}


/*
 * char_delay allows time for a character to go out
 */

char_delay()
{
	int	port = COUNTER_PORT;
	int	delay = 0x2000;

        while ((inb(port) & delay) == 0);
        while ((inb(port) & delay) != 0);
}

/*
 * inb and outb emulate I/O instructions.
 * These will work either before or after paging is enabled.
 * Note that they are named for byte I/O but do word I/O.
 */

int
inb(port)
	int	port;
{

	if ((port & ~0x1FFF) == (UART_ADDR & 0xE000)) {
		uart_delay();
	}
	if (paging_enabled) {
		return *(volatile int *)(port + IO_BASE);
	} else {
		return read_no_cache(phys_port_address(port));
	}
}

outb(port, val)
	int	port;
	int	val;
{

	if ((port & ~0x1FFF) == (UART_ADDR & 0xE000)) {
		uart_delay();
	}
	if (paging_enabled) {
		*(volatile int *)(port + IO_BASE) = val;
	} else {
		*(volatile int *)(phys_port_address(port)) = val;
	}
}

int
phys_port_address(port)
	int port;
{
	switch (port & ~0x1FFF) {

	case (FIFO_ADDR & 0xE000):
		return FIFO_ADDR_PH + (port & 0x1FFF);

	case (CSR_ADDR & 0xE000):
		return CSR_ADDR_PH + (port & 0x1FFF);

	case (PERFCNT_ADDR & 0xE000):
		return PERFCNT_ADDR_PH + (port & 0x1FFF);

	case (UART_ADDR & 0xE000):
		return UART_ADDR_PH + (port & 0x1FFF);

	default:
		panic("Invalid port address %x\n", port);
	}
}

unsigned int pmap_free_pages()
{
	return avail_remaining;
}

boolean_t pmap_next_page(addrp)
	vm_offset_t *addrp;
{
	if (avail_next == avail_end)
		return FALSE;

	if (avail_next == hole_start)
		avail_next = hole_end;

	*addrp = avail_next;
	avail_next += PAGE_SIZE;
	avail_remaining--;
	return TRUE;
}


boolean_t pmap_valid_page(x)
	vm_offset_t x;
{
	return (((avail_start <= x) && (x < avail_end)) &&
		!((hole_start <= x) && (x < hole_end)));
}

/*
 * Basic VM initialization.
 */
i860_init(basemem, physnode, slot, boothow, end_of_sym, env, entry)
	unsigned long basemem;
	unsigned long physnode;
	unsigned long slot;
	unsigned long boothow;
	unsigned long end_of_sym;
	unsigned long entry;
	char	**env;
{
	int s;
	char *new_page_size;
	extern char	stext, etext, sdata, edata, end;

	sploff();

	/*
	 * Clear the BSS.
	 */
	bzero((char *)&edata,(unsigned)(&end - &edata));

	/*
	/*
	 * Initialize control register and its RAM shadow
	 */
	control_shadow =	NPROGRAM        |
				YELLOW 		|
				NXRESET         |
				NTRESET         |
				FIFO_RESET      |
				FIFO_WRAP       |
				CLEAR_BUS       |
				CLEAR_PARITY    |
				PARITY_ENABLE   |
				BOOT_CTRL;
	outb(CONTROL_PORT, control_shadow);

	if (reentry) {
		printf("SYSTEM REENTRY\n");
		for (;;);
	}
	reentry = 1;

	system_name = "iPSC/860";
	ipsc_physnode = physnode;
	ipsc_slot = slot;
	ipsc_basemem = basemem;
	esym = (char *)end_of_sym;
	bootenv = envcopy(env);	/* bootmagic! */

	/*
	 * Set _node_self
	 */
	_node_self = ipsc_physnode;

	/*
	 *	We won't really know our cpu number till after
	 *	we configure... but we still need to have a valid
	 *	cpu number for routines that use it.  Fortunately,
	 *	set_cpu_number will do the right thing for us here
	 *	because cpu_number doesn't know we have multi-port
	 *	memory yet.
	 */
	if ((new_page_size = getbootenv("BOOT_VM_PAGESIZE"))) {
		PAGE_SIZE = atoi(new_page_size);
		for(s=0;s<32;s++){
			if(PAGE_SIZE & (1<<s)) {
				break;
			}
		}
		printf("page_shift = %x, page_size = %x\n",s,page_size);
		if((s < 12) || (s >= 32)) {
			panic("BOOT_PAGE_SIZE wrong");
		}
		PAGE_SHIFT = s;
		page_mask = PAGE_SIZE-1;
	}
	size_memory();

	/*
	 *	Initialize kernel physical map, mapping the
	 *	region from loadpt to avail_start.
	 *	Kernel virtual address starts at VM_KERNEL_MIN_ADDRESS.
	 */

	printf("i860 iPSC boot: ");
	printf("node %d, slot %d, memory from 0x%x to 0x%x (%d KB)\n",
	       ipsc_physnode, ipsc_slot, addr_base, addr_last,
	       (mem_size / 1024));
	printf("Kernel text %08x - %08x  data %08x - %08x - %08x\n",
		&stext, &etext, &sdata, &edata, &end);
	chip_state();	

	/*
	 * Create the initial kernel map, and enable address translation.
	 */
	hole_start = addr_load;
	hole_end = addr_first;
	avail_start = addr_base;
	avail_end = addr_last;
	pmap_bootstrap(addr_load);

	/*
	 *	Initialize for pmap_free_pages and pmap_next_page.
	 */

	avail_remaining = atop((avail_end - avail_start) -
			       (hole_end - hole_start));
	avail_next = avail_start;
}
