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
 * $Id: model_dep.c,v 0.119 1995/04/01 00:52:27 cfleck Exp $
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
 */

#include <cpus.h>
#include <platforms.h>
#include <norma_ipc.h>
#include <mach_kdb.h>
#include <mach_assert.h>
#include <console_logging.h>

#include <mach/mach_types.h>
#include <mach/machine/vm_param.h>
#include <mach/machine.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <kern/host.h>
#if	NORMA_IPC || NORMA2
#include <norma/ipc_node.h>
#include <norma/node_status.h>
#endif
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <sys/time.h>
#include <machine/psl.h>
#include <machine/pmap.h>
#include <machine/coff.h>

#include <i860paragon/baton.h>
#include <i860paragon/dp.h>
#include <i860paragon/fscan.h>
#include <i860paragon/lbus.h>
#include <i860paragon/led.h>
#include <i860paragon/nic.h>
#include <i860paragon/rpm.h>
#include <i860paragon/ecc.h>
#include <i860paragon/expansion.h>

#include <kern/cpu_number.h>
#include <kern/thread.h>
#include <kern/lock.h>
#include <sys/reboot.h>

#include <ctrap_history.h>
#include <machine/mach_param.h> /* HZ */

#if	MACH_KDB
#if	DB_MACHINE_COMMANDS
#include <ddb/db_command.h>
#endif	DB_MACHINE_COMMANDS
#endif	MACH_KDB

/*
 *	Physical Memory Layout
 *
 * 0xFFFFFFFF	+-----------------------+
 *		|			|
 * 0xF0000000	+			+ addr_base
 *		|			|
 * 0xE0000000	+ Baseboard RAM	*	+
 *		|			|
 * 0xD0000000	+			+
 *		|			|
 * 0xC0000000	+-----------------------+ DRAM_BASE
 *		|   mio eeprom (245KB)	|
 * 0xBFFC0000	+   ------------------  +
 *		|			|
 * 0xB0000000	+			+ memexp_start
 *		|			|
 * 0xA0000000	+ Expansion Bus	*	|
 *		|			|
 * 0x90000000	+			|
 *		|			|
 * 0x80080000	+    ----------------   +
 *		|    mio sram (512KB)	|
 * 0x80000000	+-----------------------+
 *		| DP ASIC (256 MB)	|
 * 0x70000000	+-----------------------+
 *		| Local Bus (256 MB)	|
 * 0x60000000	+-----------------------+
 *		| NIC EOD (256 MB)	|
 * 0x50000000	+-----------------------+
 *		| NIC (256 MB)		|
 * 0x40000000	+-----------------------+
 *		|			|
 * 0x30000000	+			|
 *		|			|
 * 0x20000000	+ LTU Transfer *	|
 *		|			|
 * 0x10000000	+			|
 *		|			|
 * 0x00000000	+-----------------------+
 *
 *	* GP and MP nodes differ in snooping policy:
 *
 *	pa[29:28]	GP		MP
 *	------------------------------------------------
 *	1 1		CPU1, CPU0	CPU2, CPU1, CPU0
 *	1 0		CPU1		CPU2, CPU1, CPU0
 *	0 1		CPU0		none
 *	0 0		none		none
 */

/*
 *	Virtual Memory Layout
 *
 *	All of physical RAM is mapped into the kernel address space
 *	with vaddr = paddr as a message passing optimization.
 *
 * 0xFFFFFFFF	+-----------------------+ VM_MAX_KERNEL_ADDRESS
 *		| i860 trap page	|
 * 0xFFFFF000	+-----------------------+
 *		| 			|
 *		| unused (8 pages)	|
 *		| 			|
 * 0xFFFF7000	+-----------------------+
 *		| rpmsoft (kernel write)| RPMSOFT_BASE_VADDR_KERN
 * 0xFFFF6000	+-----------------------+
 *		| rpmsoft		|
 * 0xFFFF5000	+-----------------------+ RPMSOFT_BASE_VADDR
 *		| DP ASIC		|
 * 0xFFFF4000	+-----------------------+ DP_ADDR
 *		| Local Bus (RPM)	|
 * 0xFFFF3000	+-----------------------+ RPM_BASE_VADDR + INTEL_PGBYTES
 *		| Local Bus (RPM)	|
 * 0xFFFF2000	+-----------------------+ RPM_BASE_VADDR
 *		| NIC EOD		|
 * 0xFFFF1000	+-----------------------+ EOD_ADDR
 *		| NIC			|
 * 0xFFFF0000	+-----------------------+ NIC_ADDR
 *		| 			|
 * 0xFFFEF000	+-----------------------+
 *		| last kernel mem page	|
 * 0xFFFEE000	+-----------------------+ virtual_end
 *		|			|
 *		|			|
 * 0xFC000000	+-----------------------+ virtual_avail ( 64MB kernel_map)
 * 0xFA000000	+-----------------------+ virtual_avail ( 96MB kernel_map)
 * 0xF8000000	+-----------------------+ virtual_avail (128MB kernel_map)
 *		|  (not mapped)		|
 * 0xF8000000	+-----------------------+ addr_last (128MB RAM)
 * 0xF4000000	+-----------------------+ addr_last ( 64MB RAM)
 * 0xF2000000	+-----------------------+ addr_last ( 32MB RAM)
 * 0xF1000000	+-----------------------+ addr_last ( 16MB RAM)
 *		| last physical page	|
 * 		+-----------------------+ avail_end
 *		|			|
 *		|			|
 * 		+-----------------------+ hole_end
 *		|			|
 * 		+-----------------------+ addr_first
 *		| emulator		|
 * 		+-----------------------+
 *		| server		|
 * 		+-----------------------+
 *		| pmap_bootstrap stuff	|
 * 		+-----------------------+ esym
 *		| kernel symbols	|
 * 		+-----------------------+ _end:
 *		| kernel bss		|
 * 		+-----------------------+ _edata:
 *		| kernel data		|
 * 		+-----------------------+ _sdata:
 *		|			|
 * 		+-----------------------+ _etext:
 *		| kernel text		|
 * 0xF00800C0	|			+ _stext:
 * 0xF0080000	+-----------------------+ addr_load, hole_start
 *		|			|
 * 0xF0000000	+-----------------------+ addr_base, avail_start
 *		|			|
 *		|			|
 *		|			|
 *		|			|
 * 		+-----------------------+ memexp_end
 *		| Memory Daughter Card	|
 *		|    (memexp_pages)	|
 *		|     mapped 1-to-1	|
 * 0xB0000000	+-----------------------+ memexp_start
 *		|			|
 *		|			|
 *		|			|
 * 0x80000000	+-----------------------+ VM_MIN_KERNEL_ADDRESS
 *		|			| VM_MIN_TEXT_ADDRESS
 *		|			| VM_MAX_ADDRESS
 *		|			|
 * 0x00000000	+-----------------------+ VM_MIN_ADDRESS
 */

/*	If a ram disk is loaded, it starts on the next page boundary
 *	after the end of the symbol table.
 *	Pre-loaded servers and emulators are also page aligned
 *	and are placed after the symbol table.
 */
vm_size_t	mem_size;		/* amount of physical node memory */
vm_offset_t	addr_load;		/* where the kernel was loaded */
vm_offset_t	addr_base;		/* base address of node memory */
vm_offset_t	addr_first;		/* first address after symbol table */
vm_offset_t	addr_last;		/* last address of node memory */
vm_offset_t	hole_start;		/* start of "hole" (kernel actually) */
vm_offset_t	hole_end;		/* end of "hole" */
vm_offset_t	avail_start;		/* used by pmap_bootstrap() */
vm_offset_t	avail_end;		/* used by pmap_bootstrap() */
vm_offset_t	avail_next;		/* next free page */
int		base_pages;		/* total VM pages in baseboard dram */
unsigned int	avail_remaining;	/* number of free pages */
vm_offset_t	virtual_avail;		/* used in pmap_bootstrap() */
vm_offset_t	virtual_end;		/* used in pmap_bootstrap() */

/*
 *	If a memory expansion card is attached, the following values
 *	are non-zero.  Expansion memory has no hole...it's one
 *	contiguous piece.
 */
boolean_t	memexp_present = FALSE;	/* memory expansion card present */
vm_offset_t	memexp_start = 0;	/* start address of exp ram */
vm_offset_t	memexp_end = 0;		/* end address of exp ram */
int		memexp_pages = 0;	/* number of VM pages in exp ram */

/*
 *	"bootmesh" can broadcast not only the microkernel,
 *	but a server and an emulator as well.
 *	This small table is indexed by name, usually
 *	"server" and/or "emulator".
 */
#define	NPRELOADS	2
struct preload {
	char		*pre_name;	/* "name" of program */
	unsigned char	*pre_base;	/* base address of program */
	unsigned long	pre_size;	/* length of program */
	vm_offset_t	pre_first;	/* trunc'ed page address */
	vm_offset_t	pre_last;	/* rounded page address */
};
static struct preload preloaded_program[NPRELOADS];

#include <md.h>
#if	NMD > 0
extern unsigned char	*md_address;	/* starting address of the ram disk */
extern unsigned long	md_size;	/* size in bytes, 0 == not loaded */
#endif	NMD > 0

#include <mioe.h>
#if	NMIOE > 0
#if	MACH_KDB
extern void	mioe_print_stats();	/* print MIO ethernet stats */	
#endif	MACH_KDB
#endif	NMIOE > 0

int	_node_self = -1;
int	ipsc_physnode;	/* so it matches mcmsg stuff that works on iPSC/860 */

unsigned long dpset[NDPS];
unsigned long dpclr[NDPS];
unsigned long dptimer[NDPS];
unsigned long dpstatus[NDPS];

void halt_all_cpus(boolean_t reboot);
static void	(*expansion_reboot)();	/* load and go to expansion rom */

extern unsigned long	inl();
extern void	outl();
extern int	l3break;

extern char	*getbootenv();
extern char	*getbootstring(char *var, char *dflt,
			char *result, int result_max);
extern char	**bootenv, **envcopy();
extern char	version[];

extern unsigned int	ecc_control_ce;
extern unsigned int	parity_control_nic;
extern unsigned int	parity_control_trap;
extern unsigned int	parity_control_intr;
extern unsigned int	fptrap_verbose;
extern int		mcmsg_mp_enable;

int	boot_mk_verbose;
int	break_exception_flag;
int	exception_debug_hint_flag;
int	enable_watchdog;
int	fscan_secondary_console;

#define	DYNAMIC_NUM_NODES 1
int     max_num_nodes=1024;

char *green_led_string = GREEN_LED_STRING;
char *red_led_string = RED_LED_STRING;

int	map_rpm;

#define	MAX_NCPUS 3
#if	NCPUS > MAX_NCPUS
FORCE SYNTAX ERROR
#endif

char	boot_cpu_mode[MAX_NCPUS+1]= "ama";
unsigned int	boot_msg_proc;

#if	MACH_KDB
/* int	boothowto = RB_KDB; */
int	boothowto = 0;
#else	MACH_KDB
int	boothowto = 0;
#endif	MACH_KDB

char	*esym = (char *) 0;
char	*system_name;

#if	MACH_KDB
#if	DB_MACHINE_COMMANDS

#define	MIOCOPYSTATS	1
#if	MIOCOPYSTATS
extern void	db_show_mio_copystats();
#endif	MIOCOPYSTATS

extern void	db_show_switch_hist();
extern void	db_show_intr_hist();
#if	CTRAP_HISTORY
extern void	db_show_trap_hist();
#endif	CTRAP_HISTORY
extern void	db_show_vaddrs();
extern void	show_860_regs();
extern void	db_kvtophys();

/*
 *	reboot from the kernel debugger
 */
void db_reboot()
{
	halt_all_cpus(TRUE);
}


#if	MCMSG
extern void	mcmsg_db_trace();
extern void	mcmsg_db_buf();
extern void	mcmsg_db_avail();
extern void	mcmsg_db_attach();
extern void	mcmsg_db_mcpregs();
extern void	mcmsg_db_mcptrace();
extern void	mcmsg_db_mmem();
extern void	mcmsg_db_mt();
extern void	mcmsg_db_pid();
extern void	mcmsg_db_postpage();
extern void	mcmsg_db_rbuf();
extern void	mcmsg_db_selpath();
extern void	mcmsg_db_selseq();
extern void	mcmsg_db_sendstore();
extern void	mcmsg_db_sendwait();
extern void	mcmsg_db_sel();
extern void	mcmsg_db_item();
extern void	mcmsg_db_xmsg();
extern void	mcmsg_db_xmsg();
extern void	mcmsg_db_nxreq();
extern void	mcmsg_db_nicreset();
extern void	mcmsg_db_nicregs();
#if     PUMA
extern void     mcmsg_db_puma();
#endif  PUMA
extern void mcmsg_db_comm();
#endif	MCMSG

struct db_command	i860paragon_db_commands[] = {
#if	MACH_KDB && MACH_ASSERT && DB_MACHINE_COMMANDS
	{ "switchhist",	db_show_switch_hist,	0,	0 },
#endif	MACH_KDB && MACH_ASSERT && DB_MACHINE_COMMANDS
	{ "intrhist",	db_show_intr_hist,	0,	0 },
#if	CTRAP_HISTORY
	{ "traphist",	db_show_trap_hist,	0,	0 },
#endif	CTRAP_HISTORY
	{ "kvtophys",	db_kvtophys,	0,	0 },
	{ "reboot",	db_reboot,	0,	0 },
#if	NMIOE > 0
#if	MACH_KDB
	{ "mioe",	mioe_print_stats,	0,	0 },
#endif	/* MACH_KDB */
#endif	/* NMIOE > 0 */
#if	MIOCOPYSTATS
	{ "miocopy",	db_show_mio_copystats,	0,	0 },
#endif	MIOCOPYSTATS
	{ "vaddrs",	db_show_vaddrs,	0,	0 },
	{ "regs",	show_860_regs,	0,	0 },
#if	MCMSG
	{ "trace",	mcmsg_db_trace,		0,	0 },
	{ "nicreset",	mcmsg_db_nicreset,	0,	0 },
	{ "nicregs",	mcmsg_db_nicregs,	0,	0 },
#if     PUMA
	{ "puma",       mcmsg_db_puma,  0,      0 },
#endif  PUMA
	{ "comm",		mcmsg_db_comm,		0,	0 },
#if	NX
	{ "buf",	mcmsg_db_buf,		0,	0 },
	{ "avail",	mcmsg_db_avail,		0,	0 },
	{ "attach",	mcmsg_db_attach,	0,	0 },
	{ "mcpregs",    mcmsg_db_mcpregs,       0,      0 },
 	{ "mcptrace",   mcmsg_db_mcptrace,      0,      0 },
	{ "mmem",	mcmsg_db_mmem,		0,	0 },
	{ "mt",		mcmsg_db_mt,		0,	0 },
	{ "pid",	mcmsg_db_pid,		0,	0 },
       	{ "postpage",   mcmsg_db_postpage,      0,      0 },
       	{ "rbuf",       mcmsg_db_rbuf,          0,      0 },
       	{ "selpath",    mcmsg_db_selpath,       0,      0 },
       	{ "selseq",     mcmsg_db_selseq,        0,      0 },
       	{ "sendstore",  mcmsg_db_sendstore,     0,      0 },
       	{ "sendwait",   mcmsg_db_sendwait,      0,      0 },
	{ "sel",	mcmsg_db_sel,		0,	0 },
	{ "item",	mcmsg_db_item,		0,	0 },
	{ "xmsg",	mcmsg_db_xmsg,		0,	0 },
	{ "nxreq",	mcmsg_db_nxreq,		0,	0 },
#endif	NX
#endif	MCMSG
#if	ASMP
	{ "baton",	db_show_baton,		0,	0 },
#endif	/* ASMP */
	{ (char *) 0,	0,			0,	0 }
};
#endif	DB_MACHINE_COMMANDS
#endif	MACH_KDB

/*
 *	Determine and return the card's clock frequency
 */
static void	paragon_determine_clock_freq();
int	paragon_node_mhz = 50;			/* 40, 45, 50 */
unsigned long	paragon_node_spin_factor = 25;	/* see delay() */
int	paragon_mp3=0;
int	clock_hz = HZ; /* Clock frequency */
extern int	hz;	/* also clock frequency */

/*
 *	Print some useful information at bootup
 */
static void	print_checksum();
static void	print_node_status();
static void	print_expansion();
static void	print_rpm();
static void	print_dprev();
static void	print_range();
void		print_chip_type();


node_self()
{
	return _node_self;
}


/*
 *	CPU initialization.
 *	First C routine called.
 *
 *	Limited to 10 arguments because of NIC workaround
 */

machine_startup(symtab_end, ramdisk_start, ramdisk_size, env,
		svr_start, svr_size, em_start, em_size)
	char		*symtab_end;
	char		*ramdisk_start;
	unsigned long	ramdisk_size;
	char		*env;
	unsigned char	*svr_start;
	unsigned long	svr_size;
	unsigned char	*em_start;
	unsigned long	em_size;
{

#if	NORMA_IPC
	extern int	netipc_page_list_low;
	extern int	netipc_page_list_refill;
	extern int	netipc_page_list_high;
        extern int      netipc_assembly_wrapper_max;
#endif	NORMA_IPC

#if	NORMA_FAST_OOL
	extern int	use_norma_fast_ool;
#endif	NORMA_FAST_OOL
#if	MACH_ASSERT
	extern int 	trace_zone_info;
#endif	MACH_ASSERT


	/*
	 * Do basic VM initialization
	 */
	i860_init(symtab_end, ramdisk_start, ramdisk_size, env,
		svr_start, svr_size, em_start, em_size);

	clock_hz = getbootint("BOOT_HZ", HZ);
	hz = clock_hz;
	if ((clock_hz != HZ) && (boot_mk_verbose)) {
		printf("WARNING: Clock set to %d Hz!\n",clock_hz);
	}

#if	MACH_KDB
	/*
	 * Check for a bootenv var which specifies a KDB keyboard interrupt char
	 * that overrides the chips/serial_console.c default ("^p"). Emacs and
	 * tcsh users don't like "^p" as the default.
	 */

	l3break = getbootint("KDB_INTR_CHAR", l3break);

	/*
	 * Initialize the kernel debugger.
	 */
#if	DB_MACHINE_COMMANDS
	db_machine_commands_install(i860paragon_db_commands);
#endif	DB_MACHINE_COMMANDS
	ddb_init();
#if	MCMSG
	mcmsg_basic_init();
#endif	MCMSG

	/*
	 * ignore boothowto and believe bootmagic
	 */
	if (getbootint("RB_KDB", 0) || getbootint("RB_860_KDB", 0)) {
		gimmeabreak();
	}
#endif	MACH_KDB

#if	MACH_ASSERT
	trace_zone_info = getbootint("BOOT_ZONE_TRACE", 1);
#endif	MACH_ASSERT

	/*
	 *	Display bootmagic?
	 */
	if (getbootint("BOOT_860_SHOW_MAGIC", 0) ||
	    getbootint("BOOT_SHOW_MAGIC", 0)) {
		int	i;

		for (i = 0; bootenv[i]; i++) {
			printf("%d %x %s\n", i, bootenv[i], bootenv[i]);
		}
	}

	boothowto |= (RB_SINGLE);
	if (getbootint("RB_MULTIUSER", 0) ||
	    getbootint("RB_860_MULTIUSER", 0)) {
		boothowto &= ~RB_SINGLE;
	}
#define	RB_VERBOSE_SERVER	RB_ALTBOOT
	if (getbootint("RB_VERBOSE", 0) || getbootint("RB_860_VERBOSE", 0)) {
		boothowto |= RB_VERBOSE_SERVER;
	}
#if	NCPUS > 1
#ifdef	FORCE_MASTER
	{
		extern int	syscalls_on_master;
		extern int	pagefaults_on_master;
		extern int	max_psyscall;

		pagefaults_on_master = getbootint("PAGEFAULTS_ON_MASTER", 1);
		syscalls_on_master = getbootint("SYSCALLS_ON_MASTER", 1);
		max_psyscall = getbootint("MAX_PSYSCALL", 75);
	}
#endif	/* FORCE_MASTER */
#endif	/* NCPUS > 1 */

	boothowto |= RB_ASKNAME;
	boothowto |= (RB_ASKNAME << RB_SHIFT);

	enable_watchdog = getbootint("ENABLE_WATCHDOG", 1); /* Default on */

#if	NORMA_IPC
	/*
	 * Allocate more NORMA IPC netipc_page_list pool pages if we export
	 * paging or have a local I/O device. Exporting paging implies we
	 * anticipate HEAVY IPC traffic. Currently, only export paging is
	 * handled. See norma/ipc_net.c
	 */
	if (do_i_export_paging(node_self(),getbootenv("EXPORT_PAGING"))) {
	    netipc_page_list_low = getbootint("NETIPC_PLIST_LOW",103);
	    netipc_page_list_refill = getbootint("NETIPC_PLIST_REFILL",123);
	    netipc_page_list_high = getbootint("NETIPC_PLIST_HIGH",153);
	} else {
	    netipc_page_list_low = getbootint("NETIPC_PGLIST_LOW",
					      netipc_page_list_low);
	    netipc_page_list_refill = getbootint("NETIPC_PGLIST_REFILL",
						 netipc_page_list_refill);
	    netipc_page_list_high = getbootint("NETIPC_PGLIST_HIGH",
					       netipc_page_list_high);
        netipc_assembly_wrapper_max = getbootint("NETIPC_ASSEMBLY_WRAPPER_MAX",
                                                netipc_assembly_wrapper_max);
	}
#endif	/* NORMA_IPC */

#if	NORMA_FAST_OOL
	/*
	 *  See if we should not use fast OOL transfers and ack on last
	 *  fast OOL page.
	 */
	use_norma_fast_ool = getbootint("NORMA_FAST_OOL", use_norma_fast_ool);
#endif	NORMA_FAST_OOL

#if   NORMA2
	/*
	 *  Do any dipc initialization that needs to be done early
	 *  in the game.
	 */
	dipc_early_init();
#endif

#if	DYNAMIC_NUM_NODES
	/*
	 * dynamically configure NORMA PCS (Protocol Control Structures) with
	 * respect to the total number of NORMA nodes for this system.
	 */
	{
		register int	X, Y;
		extern int	max_num_nodes;
#if	NORMA
		extern int 	max_rts_reqs;
#endif	NORMA


		/* default to 1024 nodes (32x32) */
		X = getbootint("BOOT_MESH_X",32);
		Y = getbootint("BOOT_MESH_Y",32);
		max_num_nodes = X * Y;
		if ( max_num_nodes > IP_NORMA_MAX_NODE ) {
			printf("\tWARNING: MESH_X(%d) * MESH_Y(%d) > IP_NORMA_MAX_NODE(%d)\n",
				X,Y,IP_NORMA_MAX_NODE);
		}
#if	NORMA
		/* set maximum number of IPC requests to sends */
		if((max_rts_reqs =
			(getbootint("IPC_RTS_CTS_TABLE_SIZE",576))) == 576){
		    if (max_num_nodes > 512) {
		    	max_rts_reqs = (max_num_nodes + 128) & ~(63);
		    }
		}
#endif	NORMA

		/* mark all nodes as up, server will reset from bootmesh info */
		set_norma_node_status_alive( getbootenv("BOOT_NODE_LIST") );

		/*
		 * If DEBUG_NODE is defined then mark it as alive.
		 */
		if ( (X = getbootint("DEBUG_NODE", -1 )) != -1 ) { 
			norma_node_status_op( 2, X );
		}
	}
#endif	/* DYNAMIC_NUM_NODES */

	if (boot_mk_verbose) {
		/* The server just repeats this, anyway */
		printf(version);
	}

	printf("Node %d\n", ipsc_physnode);

	/*
	 * Initialize machine slot structures, All CPU's except the master
	 * are marked as present but NOT running. Could be smarter for MP.
	 * "is_cpu = TRUE" required for kern/startup.c, start_kernel_threads()
	 * tp see presence of other CPU's and create idle threads for those
	 * CPUs.
	 */
	{
		register int	j;

		for(j=0; j < NCPUS; j++)
		{
			machine_slot[j].is_cpu = TRUE;
			machine_slot[j].running = FALSE;
			machine_slot[j].cpu_type = CPU_TYPE_I860;
			if (paragon_mp3)
			  	machine_slot[j].cpu_subtype =
					CPU_SUBTYPE_PARAGON860_MP;
			else
				machine_slot[j].cpu_subtype =
					CPU_SUBTYPE_PARAGON860;
		}
		machine_slot[master_cpu].running = TRUE;

	}

	/*
	 * Determine how to boot the other CPUS.
	 *
	 * input: boot_cpu_mode[] default and BOOT_CPU_MODE over-ride
	 * output: boot_cpu_mode[], boot_msg_proc.
	 *
	 * boot_cpu_mode[i] controls if/how we boot cpu i:
	 *
	 *	m = Start in MCP mode. Valid only for cpu1
	 *	a = Start in APP mode. Mode switches permitted
	 *	o = Off. Don't startup.
	 */

/* XXX duplicate from i860paragon/mp.c */
#define CPU_OFF			'o'
#define CPU_MCPMODE		'm'
#define CPU_APPMODE		'a'

#if	! CPU2_IO_IS_EVALUATED && (NCPUS > 2)
	/*
	 * If we have an expansion card, alter the default boot_cpu_mode
	 * by disabling cpu2.
	 *
	 * Delete this clause when "AMA mode" has been evaluated on IO nodes.
	 */
	if (node_status_register_expansion_present())
	{
		boot_cpu_mode[2] = CPU_OFF;
	}
#endif	/* ! CPU2_IO_IS_EVALUATED && NCPUS > 2 */

	(void) getbootstring("BOOT_CPU_MODE", boot_cpu_mode, boot_cpu_mode,
				MAX_NCPUS + 1);

	/*
	 * hard-code cpu0 to CPU_APPMODE
	 */
	boot_cpu_mode[0] = CPU_APPMODE;

	/*
	 * boot_msg_proc is based on boot_cpu_mode[1].
	 */

	boot_msg_proc = (boot_cpu_mode[1] == CPU_MCPMODE);

	/*
	 * Start the system.
	 */

	{
		char	*s;
		int	i;
		extern int sw_refresh_enable;

		/*
		 *	LED mania.
		 */
		if ((s = getbootenv("BOOT_GREEN_LED")) != 0) {
			green_led_flag = 0;
			while (*s) {
				for (i = 0;
				     green_led_string[i] != '\0' &&
				     green_led_string[i] != *s;
				     i++);
				green_led_flag |= 1 << i;
				s++;
			}
		}
		if ((s = getbootenv("BOOT_RED_LED")) != 0) {
			red_led_flag = 0;
			while (*s) {
				for (i = 0;
				     red_led_string[i] != '\0' &&
				     red_led_string[i] != *s;
				     i++);
				red_led_flag |= 1 << i;
				s++;
			}
		}


		/*
		 *	Stop in exception()?
		 */
		break_exception_flag = getbootint("BOOT_BREAK_EXCEPTION", 0);

		/*
		 *	Make noise and cough up state when delivering an
		 *	exception to the server or something that looks
		 *	like its running in an emulator?
		 */
		exception_debug_hint_flag=getbootint("EXCEPTION_DEBUG_HINT", 1);


		/*
		 *	If refresh has been deferred, is it time to turn
		 *	it back on?  And, should we perform the software
		 *	refresh loop?
		 */
		if ((s = getbootenv("DEFER_REFRESH")) != 0) {
			if (strcmp(s, "startup") == 0) {
				node_control_register_clear(LB_NODE_CTRL_REF_DEF);
			}
			if (strcmp(s, "sw") == 0) {
				sw_refresh_enable = 1;
			}
			if (strcmp(s, "swt") == 0) {
				sw_refresh_enable = 1;
				node_control_register_clear(LB_NODE_CTRL_REF_DEF);
			}
		}
	}
	RED_OFF(RED_COMEUP);
	GREEN_OFF(GREEN_COMEUP);

	ASSERT_BATON_OWNER();

	i860_interrupts_on_early();
	setup_main();
	/* NOTREACHED */
}


/*
 * Find devices.  The system is alive, called from kern/startup.c setup_main().
 */
machine_init()
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

#if   NCPUS > 1
	/*
	 * allocate slave processor(s) startup kernel thread stack(s)
	 */
	slave_stack_alloc();
#endif
	i860_interrupts_on();
}

/*
 * Halt the system or reboot.
 * Called from syscall context or from ddb.
 */
void
halt_all_cpus(boolean_t reboot)
{
	int i, s;

#if	CONSOLE_LOGGING
	/*
	 * disable console logging
	 * from here on out we're just trying to qietly go away
	 */
	conslog_stop();
#endif	/* CONSOLE_LOGGING */

	/*
	 * turn on all 5 green LEDs and make sure clock ticks don't touch.
	 */
	if (green_led_flag & (1 << GREEN_COMEUP))
	{
		green_led_flag = (1 << GREEN_COMEUP);

		node_control_register_set(
			LB_NODE_CTRL_TOP_LED |
			LB_NODE_CTRL_OVER_LED |
			LB_NODE_CTRL_MIDDLE_LED |
			LB_NODE_CTRL_UNDER_LED |
			LB_NODE_CTRL_BOTTOM_LED);
	}

#if	NCPUS > 1
	/*
	 * if not running on cpu0, go there
	 */
	if (cpu_number() != master_cpu)
	{
		thread_bind(current_thread(), master_processor);
		thread_block((void (*)()) 0);
		thread_bind(current_thread(), PROCESSOR_NULL);
	}

	/*
	 * take cpu1, cpu2 offline
	 * (this doorbell simply sends them to cpu_halt() at spl6)
	 */
	for (i = 1; i < NCPUS; ++i)
	{
		if (machine_slot[i].running == TRUE)
		{
			mp_halt(i);
		}
	}
#endif	/* NCPUS > 1 */

	delay(500000);
	/*
	 * turn off interrupt driver console output
	 * and flush existing buffered output
	 */
	cnpollc(TRUE);
	db_printf("System is down.  ");

	(void) sploff();

	/*
	 * pull the plug on cpu1 and cpu2
	 */
	node_control_register_clear(LB_NODE_CTRL_CPU1_RESET);
	if (paragon_mp3)
		node_control_register_clear(LB_NODE_CTRL_CPU2_RESET);

	if (reboot)
	{
		/*
 		 * Call the 'fscan' reboot module. This module will wait
  		 * until the fscan query command is sent, then the state
		 * 'REBOOT' will be sent to the diag station. This will
		 * invoke the 'reset' script. This can be disabled by
 		 * using 'ENABLE_WATCHDOG=0' in MAGIC.MASTER
		 */

		if (enable_watchdog)
		{
			db_printf("Calling FSCAN reboot...\n");
			fscan_reboot();
		}

		if (node_status_register_expansion_present())
		{
			db_printf("Calling expansion reboot...\n");
			(*expansion_reboot)();
		}
		/* NOTREACHED */
	}

	db_printf("Please reset the system.\n");
	GREEN_OFF(GREEN_COMEUP);

	/*
	 * cpu0 hangs out at sploff looking for your ^p
	 */
	cnpollc(TRUE);

	for(;;)
	{
		if ((cngetc() & 0x7f) == l3break)
			gimmeabreak();
	}

	return; /* NOTREACHED */
}

/*
 * end of the line for this CPU
 */
halt_cpu()
{
	GREEN_OFF(GREEN_COMEUP);

	flush();
	for(;;);
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

	if (prot != (VM_PROT_READ))
		return (-1);

	return (i860_btop(pmap_extract(pmap_kernel(), (vm_offset_t) mtime)));
}




/*
 * Program the DP asic {low/high} timer to generate a clock-tick interrupt
 *
 * 10000us   = 10ms   = 0.01sec (100Hz)
 * 100000us  = 100ms  = 0.1sec  (10Hz)
 * 1000000us = 1000ms = 1.0sec  (1Hz)
 *
 * CPU0 uses the DP asic low timer while CPU1 uses DP asic high.
 */
 
startrtclock()
{
        int mycpu = cpu_number();
        unsigned long value;
	int clock_hz = HZ;
 
        /*
         * We mask the lower 8 bits because the third timer on the MP3
         * (in the vdp) ignores these bits. In order to provide a little
         * less skew between clocks, all timers will have their lower 8
         * bits ignored.
         */
        value = (-1) - ((paragon_node_mhz * (1000000/clock_hz)) & 0xFFFFFF00);
 
        /*
         * Since the DP timers get reloaded from a 20nS clock and the VDP
         * gets reloaded from a 256*20 (5.12uS) clock, there is a 1-clock
         * skew which must be accounted for but only on the third processor
         */
        if (mycpu == 2) value += 256;
 
        /*
         * Set the timeout value.
         */
        outl(dptimer[mycpu], value);
}
 

inittodr()
{
	time_value_t	new_time;
	int	s;

	new_time.seconds = 0;
	new_time.microseconds = 0;

	(void) readtodc(&new_time.seconds);
	s = splhigh();
	time = new_time;
	splx(s);
}


resettodr()
{
	writetodc();
}


/*
 *	Enable interrupts
 */
i860_interrupts_on()
{
	extern void scan_intr_on(int);

	spl_init();
#if	NOTYET
	scan_intr_on(1 << DP_IMSK0_NET_CPU);
#endif	NOTYET
	splon(PSR_IM);
}

i860_interrupts_on_early()
{
	spl_init_early();
	splon(PSR_IM);
}


/*
 *	Make an entry in the preloaded program table.
 */
int paragon_preloaded_program_protect(name, base, size)
	char	*name;
	unsigned char	*base;
	unsigned long	size;
{
	struct preload	*pl;
	int	i;

	if (size == 0)
		return 0;

	pl = &preloaded_program[0];
	for (i = 0; i < NPRELOADS; pl++, i++) {
		if (pl->pre_name == 0) {
			pl->pre_name = name;
			pl->pre_base = base;
			pl->pre_size = size;
			pl->pre_first = i860_trunc_page(base);
			pl->pre_last = i860_round_page(base + size);
			return i;
		}
	}

	return -1;
}


/*
 *	Adjust max to the last page of all preloaded programs.
 */
vm_offset_t paragon_preload_program_max(max)
	vm_offset_t	max;
{
	struct preload	*pl;
	int	i;

	pl = &preloaded_program[0];
	for (i = 0; i < NPRELOADS; pl++, i++) {
		if (pl->pre_size == 0)
			continue;
		if (pl->pre_last > max)
			max = pl->pre_last;
	}
	return max;
}


/*
 *	Return the preload descriptor for "name."
 *	XXX internal use only...
 */
struct preload *paragon_preload_lookup(name)
	char	*name;
{
	struct preload	*pl;
	int	i;

	pl = &preloaded_program[0];
	for (i = 0; i < NPRELOADS; pl++, i++) {
		if ((pl->pre_size == 0) || (pl->pre_name == 0))
			continue;
		if (strcmp(pl->pre_name, name) == 0)
			return pl;
	}
	return 0;
}


/*
 *	Is the named program in the preloaded program table?
 *	Return TRUE if so as well as the base address
 *	and the size.
 */
boolean_t preloaded_program_present(name, addr, size)
	char		*name;
	unsigned char	**addr;	/* OUT */
	unsigned long	*size;	/* OUT */
{
	struct preload	*pl;
	int	i;

	pl = paragon_preload_lookup(name);
	if (pl == 0) {
		*addr = 0;
		*size = 0;
		return FALSE;
	}

	*addr = pl->pre_base;
	*size = pl->pre_size;
	return TRUE;
}


/*
 *	Advance "*next" over any preloaded programs.
 *	Called from pmap_next_page().
 */
void paragon_preloaded_program_skip_pages(next)
	vm_offset_t	*next;	/* OUT */
{
	struct preload	*pl;
	int	i;
	vm_offset_t	tmp;

	pl = &preloaded_program[0];
	for (i = 0; i < NPRELOADS; pl++, i++) {
		if (pl->pre_size == 0)
			continue;
		tmp = *next;
		if ((tmp >= pl->pre_first) && (tmp < pl->pre_last)) {
			*next = pl->pre_last;
			return;
		}
	}
}


/*
 *	Return the space used by a preloaded program
 *	to the VM physical page pool.
 */
int preloaded_program_release(name)
	char	*name;
{
	struct preload	*pl;
	int	s;
	char	*str;
	vm_offset_t	tmp;
	extern int	vm_page_wire_count;

	pl = paragon_preload_lookup(name);
	if (pl == 0) {
		return 0;
	}

	if (getbootint("GREEDY_SERVER_MEMORY", 0)) {
		return 0;
	}

	s = splvm();

	for (tmp = pl->pre_first; tmp < pl->pre_last; tmp += PAGE_SIZE) {
		vm_page_wire_count--;
	}

	vm_page_create(pl->pre_first, pl->pre_last);

	pl->pre_name = 0;
	pl->pre_base = 0;
	pl->pre_size = 0;
	pl->pre_first = 0;
	pl->pre_last = 0;

	splx(s);

	return 1;
}


/*
 * size_memory() sets a bunch of globals:
 *
 * page_size	(bytes)
 * addr_base	0xF0000000 (hard-coded in node_memory_base())
 * addr_load	oxF0080000
 * addr_first
 * addr_last
 * base_pages	(pages on baseboard)
 * memexp_pages	(pages on MDC)
 * mem_size	(bytes)
 */

size_memory()
{
	extern char	stext;
	int		override;

	/*
	 *	set the kernel's view of soft VM page size
	 *	Enables macros "round_page() and trunc_page() to work correctly.
	 */
	page_size = getbootint("BOOT_VM_PAGESIZE", 8192);
	vm_set_page_size();     /* see ../vm/vm_resident.c */

	mem_size = node_memory_size();
	addr_base = node_memory_base();
	addr_load = i860_trunc_page(&stext);
#if	NMD > 0
	if (md_size != 0) {
		addr_first = i860_round_page((md_address + md_size));
	} else {
		addr_first = i860_round_page((int) esym);
	}
#else	NMD > 0
	addr_first = i860_round_page((int) esym);
#endif	NMD > 0

	addr_first = paragon_preload_program_max(addr_first);

	/*
	 * Ability to limit node memory size [bytes] via bootmagic.
	 */
	mem_size = (vm_size_t) getbootint("BOOT_860_MEMSIZE", (int) mem_size);

	addr_last = i860_trunc_page(addr_base + mem_size);

	/* set total number of VM sized baseboard dram pages */
	base_pages = atop(addr_last - addr_base);

	/*
	 *	Don't use memory found on a memory expansion card?
	 */
	if (getbootint("MEMORY_EXPANSION_DISABLED", 0) != 0) {
		if (memexp_present)
			printf("NOTICE: MEMORY_EXPANSION_DISABLED\n");
		memexp_present = FALSE;
	}

	if (memexp_present) {
		mem_size += (memexp_end - memexp_start);
		/* set total number of VM sized expansion card dram pages */
		memexp_pages = atop(memexp_end - memexp_start);
	}
	return;
}


/*
 *	Some old GP nodes run at 40MHz (fab5), most run at 50MHz.
 */

void paragon_determine_clock_freq()
{
	int	fab, mhz = paragon_node_mhz;
	int s;

	/*
	 * For an MP board, the clock will always run at 50 Mhz.
	 */
	if (paragon_mp3) {
		mhz = 50;
	}
	else {
		if ((mhz = getbootint("GP_NODE_MHZ", 0)) == 0) {
			fab = 8;
			mhz = 50;
			if (boot_mk_verbose) {
				printf("GP Fab %d card detected.\n", fab);
			}
		}
	}

	if ((mhz == 40) || (mhz == 45) || (mhz == 50)) {
		paragon_node_mhz = mhz;
	} else {
		char	*s;

		printf("unusual clock frequency of %dMHz (", mhz);
		if ((mhz >= 10) && (mhz <= 1000)) {
			paragon_node_mhz = mhz;
			s = "okay";
		} else {
			s = "ignored";
		}
		printf("%s)\n", s);
	}
	if (boot_mk_verbose) {
		printf("%dMhz clock.\n", paragon_node_mhz);
	}

	/*
	 *	the "spin factor" is two times the length of a clock
	 *	cycle (in nanoseconds) and is used by delay() in
	 *	computing the iteration argument to _spin().
	 */
	paragon_node_spin_factor = 1000 / (2 * (1000 / paragon_node_mhz));
}


/*
 *	Calculate and check text checksum
 */

static void print_checksum()
{
	extern char	stext, etext, sdata, edata, end;
	register unsigned char	*p;
	register unsigned char	*q;
	register char		*s;
	register unsigned short	ctext;
	register unsigned long	btext;
	register unsigned short	ktext;

	ctext = 0;
	btext = 0;
	for (p = (unsigned char *)&stext, q = (unsigned char *)&etext;
	     p < q; p++) {
		ctext = *p + (ctext << 1);
		ctext = (ctext & ((1 << 13) - 1)) + (ctext >> 13);
		btext++;
	}
	if (boot_mk_verbose) {
		printf("Text size: %d  checksum: %d\n", btext, ctext);
	}

	ktext = getbootint("BOOT_MK_TEXT_CHECK", ctext);
	if (ctext != ktext) {
		printf("bad MK checksum: %d != %d\n", ctext, ktext);
		for (;;);
	}
}


/*
 *	Print the rev and type of the NIC
 */
static void print_nicrev()
{
	nic_reg		t;
	extern int	paging_enabled;

	if (paging_enabled) {
		t.full = NIC.status.full;
	} else {
		pfldd(&NIC_PH.status, &t);
	}
	printf("nic type %d  rev %d\n",
		(t.halfs.hi >> 6) & 0x1f,
		(t.halfs.hi >> 3) & 0x7);
}

/*
 *	Print Node Status Register
 *	Global "paragon_mp3" is set up before this is called.
 */
static void print_node_status()
{
	int	status;
	int	mbytes;
	char	*node_str;
	int	fab = 0;

	status = node_status_register_read();

	if (paragon_mp3)
	{
		int fruid;

		node_str = "MP";
		fruid = MP_NODE_STATUS_FRUID(status);
		mbytes = MP_FRUID_MB(fruid);
		fab = MP_FRUID_FAB(fruid);
	} else {
		node_str = "GP";
		mbytes = (status & LB_GP_NODE_STAT_MEMSIZE) ? 32 : 16;
	}


	printf("%s node status = 0x%08x<rev %d, slot %d, %dMB",
		node_str,
		status,
		NODE_STATUS_NODEREV(status),
		NODE_STATUS_SLOTID(status),
		mbytes);

	if (!(status & LB_NODE_STAT_EXPPRESENT))
		printf(", exp card present");

	if (fab)
		printf(", fab %d", fab);

	printf(">\n");
}


/*
 *	Print the presence of an expansion card.
 */
static void print_expansion()
{
	extern int node_status_register_expansion_present();
	char	*exp;
	int	memx = 0;

	if (node_status_register_expansion_present()) {
		switch (expansion_id()) {
		case EXP_ID_HIPPI:
			exp = "hippi";
			break;

		case EXP_ID_MIO:
			exp = "mio";
			break;

		case EXP_ID_SCSI:
			exp = "scsi";
			break;

		case EXP_ID_MEMX16:
			exp = "16MB";
			memx++;
			break;

		case EXP_ID_MEMX32:
			exp = "32MB";
			memx++;
			break;

		case EXP_ID_MEMX64:
			exp = "64MB";
			memx++;
			break;

		case EXP_ID_MEMX128:
			exp = "128MB";
			memx++;
			break;

		default:
			exp = "phantom";
			break;
		}
	} else {
		exp = "no";
	}

	printf("%s expansion card detected.\n", exp);

	if (memx) {
		print_dprev(dpstatus[MXDP0], MXDP0);
	}
}


/*
 *	Print the presence of an RPM counter.
 */
static void print_rpm()
{
	int	s;

	s = paragon_rpm_present();
	printf("rpm counters %spresent.\n", (s) ? "" : "not ");
}



/*
 *	Print the rev of each DP ASIC
 */
static void print_dprev(dpaddr, base)
	unsigned long	dpaddr, base;
{
	unsigned long	rev;
	int	i;

	for (i = 0; i < 2; i++) {
		rev = inl(dpaddr + (i << 2)) >> 12;
		printf("dp%d asic: rev %d%s",
			base + i,
			rev & 15,
			(i == 0) ? ", " : "\n");
	}
}


/*
 *	What chip step are we?  We don't have
 *	i860XR's on GP nodes, but we can carry
 *	the logic along...
 */
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
static char *xp_steppings[] = {
	"s0?",	/* 0 */
	"A0",	/* 1 */
	"B0",	/* 2 */
	"B1",	/* 3 */
	"B2",	/* 4 */
	"B3?",	/* 5 */
	"s6?",	/* 6 */
	"s7?"	/* 7 */
};


void print_chip_type()
{
	unsigned int chip_type, chip_step, chip_mode, chip_int, chip_dcs;
	unsigned int chip_prot;
	unsigned int epsr;

	epsr = get_epsr();
	chip_type = (epsr & 0x000000FF);
	chip_step = ((epsr & 0x00001F00) >> 0x08);
	chip_mode = (epsr & 0x00800000) ;
	chip_int = (epsr & 0x00020000) ;
	chip_dcs = ((epsr & 0x003C0000) >> 18);
	chip_prot = (epsr & 0x00004000);

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
		if (chip_step > 0 && chip_step < 8) {
			printf("%s", xp_steppings[chip_step]);
		} else {
			printf("{XP,step=0x%x}", chip_step);
		}
		break;
	}

	printf(", %dK dcache", (0x02 << ((int)(11 + chip_dcs))) >> 10);
	printf(", %s-endian", chip_mode ? "big" : "little");
	printf(", mmu=i%d-mode", chip_prot ? 860 : 386);
	printf(", epsr=0x%x\n", epsr);
}


static void print_range(s, from, to)
	char	*s, *from, *to;
{
	unsigned long	length;

	length = to - from;
	printf("%s from 0x%x to 0x%x (%dKB)\n", s, from, to, (length / 1024));
}


unsigned int pmap_free_pages()
{
	return avail_remaining;
}


/*
 *	Return an index into the byte vector 'pv_list' (physical to virtual
 *	list) given a physical 	address. The 'pv_list' allocates one byte per
 *	VM page frame which may be comprized of multiple physical/hardware
 *	page frames (see pmap.c ptes_per_vm_page). Memory on a Paragon GP node
 *	can be in two contiguous chunks (baseboard and expansion), separated
 *	by a wide unpopulated gap in physical address space.
 *
 *	The mapping of physical address to index:
 *
 *		avail_start .. avail_end:	0 .. N-1
 *		memexp_start .. memexp_end:	N .. M-1
 *
 *	where "N" is the total number of pages in baseboard memory,
 *	and where "M" is the total number of physical pages.
 */
int pa_index(phys)
	vm_offset_t	phys;
{
	if ((phys >= avail_start) && (phys < avail_end))
		return atop(phys - avail_start);

	if ((memexp_present) && (phys >= memexp_start) && (phys < memexp_end))
		return base_pages + atop(phys - memexp_start);

	assert(0);
	return 0;
}


/*
 *	Return the total number of VM sized dram pages
 *	that the pmap system will need to know about.
 */
long paragon_total_vm_pages()
{
	return base_pages + memexp_pages;
}


/*
 *	Return a pointer to the next available page while
 *	bootstrapping pmap and vm.
 */
boolean_t pmap_next_page(addrp)
	vm_offset_t *addrp;
{
	if ((memexp_present) && (avail_next == memexp_end))
		avail_next = avail_start;

	if (avail_next == avail_end)
		return FALSE;

	if (avail_next == hole_start)
		avail_next = hole_end;

	paragon_preloaded_program_skip_pages(&avail_next);

	*addrp = avail_next;
	avail_next += PAGE_SIZE;
	avail_remaining--;
	return TRUE;
}


boolean_t pmap_valid_page(x)
	vm_offset_t x;
{
	if ((memexp_present) && (x >= memexp_start) && (x < memexp_end))
		return TRUE;
	if ((x >= avail_start) && (x < avail_end))
		return TRUE;
	return FALSE;
}


/*
 *	Read and write back all of physical memory
 */
static void paragon_dram_ecc_prescrub()
{
	vm_offset_t	mem;
	vm_size_t	i;
	unsigned long	bit, status;
	extern void	piped_page_copy();

	/*
	 *	clear any pending ECC errors
	 *	generated before the pre-scrub.
	 */
	bit = 1 << DP_CTRL_CLEAR_HWERR;
	for (i=0; i<NDPS; i++) 
		outl(dpset[i], bit);

	/*
	 *	read and re-write all of baseboard memory;
	 *	this will generate and write ECC bits.
	 */
	mem = node_memory_base();
	i = node_memory_size();
	while (i > 0) {
		piped_page_copy(mem, mem);
		i -= 4096;
		mem += 4096;
	}

	/*
	 *	if a memory expansion card is present,
	 *	read and re-write it too.
	 */
	if (memexp_present) {
		for (mem = memexp_start; mem != memexp_end; mem += 4096)
			piped_page_copy(mem, mem);
	}


	/*
	 *	safety's sake...probably just paranoia
	 */
	flush();

	/*
	 *	read the status registers
	 *	(probably redundant)
	 */
	for (i=0; i<NDPS; i++) 
		status = inl(dpstatus[i]);

	/*
	 *	clear any pending ECC errors
	 *	generated during the pre-scrub.
	 */
	bit = 1 << DP_CTRL_CLEAR_HWERR;
	for (i=0; i<NDPS; i++) 
		outl(dpset[i], bit);
}


/*
 *	Enable/Disable ECC generation.
 */
static boolean_t paragon_dram_ecc(enable)
	boolean_t	enable;
{
	int i;
	boolean_t	old;
	static boolean_t	ecc_state = FALSE;

	if (enable) {
		for (i=0; i<NDPS; i++)
			outl(dpset[i], 1 << DP_CTRL_ECCENABLE);
	} else {
		for (i=0; i<NDPS; i++)
			outl(dpclr[i], 1 << DP_CTRL_ECCENABLE);
	}

	old = ecc_state;
	ecc_state = enable;
	return old;
}


/*
 *	Is there an RPM (Multikron compatible) on
 *	the card?  Returns true if present.
 *	XXX this routine is a clone of rpm_probe() in the RPM driver.
 *	Why are there two?
 */
boolean_t	__paragon_rpm_presence;	/* to force the matter, if needed */
boolean_t	__paragon_rpm_probed;
int paragon_rpm_present()
{
	unsigned long	bits;
	int	i, max, wait;
	struct rpm	*rpm;

	if (!map_rpm)
		return (FALSE);

	if (__paragon_rpm_probed) return __paragon_rpm_presence;

	__paragon_rpm_probed = TRUE;

	/*
	 *	The trick is to *read* the node *control*
	 *	register (a legal bus cycle that returns
	 *	semi-junk). The RPM has pull-downs on the
	 *	high 8 bits of the data bus.  If we get
	 *	back 0xff in the high byte, the RPM
	 *	isn't present.
	 *	For the MP3 board, the RPM will ALWAYS be present.
	 */

	max = 20;
	wait = 50;
	for (i = 0; i < max; i++) {
		delay(wait);
		bits = inl(LB_NODE_CONTROL);
		if (paragon_mp3 || (((bits >> 24) & 0xff) != 0xff)) {
			__paragon_rpm_presence = TRUE;
			/*
			 *	Reset and enable the counters...
			 */
			rpm = (struct rpm *) RPM_BASE_PADDR;
			outl(&rpm->rpm_control, 0xffff0077);
			return TRUE;
		}
	}
	__paragon_rpm_presence = FALSE;
	return FALSE;
}


/*
 *	Is a memory expansion card present?
 */
boolean_t __paragon_memory_expansion_probed = FALSE;
int paragon_memory_expansion_probe()
{
	int	exp_size;

	if (__paragon_memory_expansion_probed) 
		return memexp_present;

	/*
	 *	Is a memory expansion board installed?
	 */
	switch (expansion_id()) {
	case EXP_ID_MEMX16:
		exp_size = 16;
		break;

	case EXP_ID_MEMX32:
		exp_size = 32;
		break;

	case EXP_ID_MEMX64:
		exp_size = 64;
		break;

	case EXP_ID_MEMX128:
		exp_size = 128;
		break;

	default:
		exp_size = 0;
		break;
	}
	__paragon_memory_expansion_probed = TRUE;

	if (exp_size == 0)
		return 0;

	memexp_present = TRUE;
	memexp_start = (vm_offset_t) 0xb0000000;
	memexp_end = memexp_start + (exp_size * 1024 * 1024);

	return 1;
}


/*
 *      enable/disable hardware error interrupts
 *      from an expansion memory card if present.
 */
void paragon_memexp_ecc_intr(enable)
        boolean_t       enable;
{
        extern void     memexp_hw_err_interrupt();
        void    (*old)();
 
	if (enable) {
		outl(dpset[MXDP0], 0x10001);
		outl(dpset[MXDP1], 0x10000);
		interrupt_expansion_install( memexp_hw_err_interrupt, &old);
	} else {
		outl(dpclr[MXDP0], 0x10001);
		outl(dpclr[MXDP1], 0x10000);
	}
}



/*
 *	Enter physical mappings for any expansion memory
 *	that may be present.
 */
void paragon_pmap_bootstrap_expansion_memory(dir)
	pt_entry_t	*dir;
{
	if (paragon_memory_expansion_probe()) {
		pmap_bootstrap_i860_memrange(dir, memexp_start, memexp_end);
		pmap_bootstrap_i860_memrange(dir,
			memexp_start ^ 0x30000000, memexp_end ^ 0x30000000);
	}
}


/*
 *	Grant write permission to the RPM page for
 *	the specified thread.
 */
void paragon_rpm_enable_write(thread)
	thread_t	thread;
{
	pt_entry_t	*ptep;

	assert(thread != 0);
	assert(thread->task != 0);
	assert(thread->task->map != 0);
	ptep = pmap_pte(vm_map_pmap(thread->task->map), RPM_BASE_VADDR);
	if (ptep != 0) {
		*ptep |= INTEL_PTE_WRITE;
		flush_tlb();
	}
}


/*
 *	Map in the RPM.  (called from pmap_bootstrap()).
 */
void paragon_bootstrap_map_rpm(dir, present)
	pt_entry_t      *dir;
	boolean_t	present;
{
	vm_offset_t	rpmpages, rpmsoft;

	if (present) {
		rpmpages = (vm_offset_t) RPM_BASE_PADDR;
	} else {
		/* steal 2 pages and zero-fill them */
		rpmpages = hole_end;
		hole_end += (INTEL_PGBYTES * 2);
		bzero((char *) rpmpages, 2 * INTEL_PGBYTES);
	}

	pmap_bootstrap_i860_io(dir,
		(vm_offset_t) rpmpages,
		(vm_offset_t) RPM_BASE_VADDR,
		TRUE);
	pmap_bootstrap_i860_io(dir,
		(vm_offset_t) rpmpages + INTEL_PGBYTES,
		(vm_offset_t) RPM_BASE_VADDR + INTEL_PGBYTES,
		TRUE);


	/*
	 *	Steal a page for the soft RPM page
	 */
	rpmsoft = hole_end;
	hole_end += INTEL_PGBYTES;
	bzero(rpmsoft, INTEL_PGBYTES);
	pmap_bootstrap_i860_io(dir,
		(vm_offset_t) rpmsoft,
		(vm_offset_t) RPMSOFT_BASE_VADDR,
		TRUE);
	pmap_bootstrap_i860_io(dir,
		(vm_offset_t) rpmsoft,
		(vm_offset_t) RPMSOFT_BASE_VADDR_KERN,
		FALSE);
}

#if	RPMSOFT
#define	DOUBLE_SEC_PER_TICK	((double)1.0 / (double)HZ)
/*
 * Called once per clock tick on every CPU
 * If CPU is idle, add a tick's worth of seconds
 * to the appropriate rpmsoft idle counter.
 *
 * If CPU is master_cpu and the MCP is enabled, do this for the MCP also.
 */
void
rpmsoft_idle_tick(int cpu)
{
#if	MCMSG && NCPUS > 1

	if ((cpu == master_cpu) && mcmsg_mp_enable)
		rpmsoft_idle_tick(1);
#endif	MCMSG && NCPUS > 1

	if (cpu_to_processor(cpu)->state != PROCESSOR_IDLE)
		return;

	((struct rpmsoft *)RPMSOFT_BASE_VADDR_KERN)[cpu].rpms_idle =
	(double)(machine_slot[cpu].cpu_ticks[CPU_STATE_IDLE]) *
	DOUBLE_SEC_PER_TICK;

	return;
}
#endif	RPMSOFT

/*
 *	convert a string of hexadecimal digits
 *	to an unsigned long.
 */
static unsigned long hexconvert(s)
	char	*s;
{
	unsigned long	n, t;
	char	c;

	if (s == 0)
		return 0;
	n = 0;
	while ((c = *s++)) {
		if ((c >= '0') && (c <= '9'))
			t = c - '0';
		else if (((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F')))
			t = c - 'a' + 10;
		else
			t = 0;
		n = (n << 4) + t;
	}
	return n;
}


/*
 *	A hook for the VMP folks.
 */
void paragon_bootstrap_map_vmp(dir)
	pt_entry_t	*dir;
{
	char	*phys, *virt;
	unsigned long	pa, va;
	pt_entry_t	pde, *ptep;

	/*
	 *	make sure that they really want to do this...
	 */
	if (getbootint("VMP_HOOK_ENABLED", 0) != 103163)
		return;
	pa = hexconvert(getbootenv("VMP_PHYS"));
	if (pa == 0)
		return;
	va = hexconvert(getbootenv("VMP_VIRT"));
	if (va == 0)
		return;
	/*
	 *	map it in, read-only from user mode.
	 */
	pmap_bootstrap_i860_io(dir, pa, va, TRUE);

	/*
	 *	give write permission.
	 */
	pde = dir[pdenum(va)];
	if ((pde & INTEL_PTE_VALID) == 0)
		panic("bootstrap map vmp (pde)");

	ptep = &((pt_entry_t *) (pde & INTEL_PTE_PFN))[ptenum(va)];
	if ((*ptep & INTEL_PTE_VALID) == 0)
		panic("bootstrap map vmp (*ptep)");

	*ptep |= INTEL_PTE_WRITE;
}


/*
 *	Map in crucial, fixed VM-address devices.
 *	(nic, dp, rpm, etc.)
 */
void paragon_pmap_bootstrap_io(dir)
	pt_entry_t	*dir;
{
	pmap_bootstrap_i860_io(dir, NIC_ADDR_PH, NIC_ADDR, FALSE);
	pmap_bootstrap_i860_io(dir, EOD_ADDR_PH, EOD_ADDR, FALSE);
	paragon_bootstrap_map_rpm(dir, paragon_rpm_present());
	pmap_bootstrap_i860_io(dir, DP_ADDR_PH, DP_ADDR, TRUE);
	paragon_bootstrap_map_vmp(dir);
}


/*
 *	Reinitialize the DP ASIC's -- the firmware
 *	may have left them in an unacceptable state.
 */
static void dp_asic_init()
{
	int	i, bits;

	/*
	 * Initialize the dpset and dpclr arrays. These are used
	 * in routines like the interprocessor interrupt routines.
	 */
	for (i=0; i<NDPS; i++) {
		dpset[i]	= DP_BITBUCKET;
		dpclr[i]	= DP_BITBUCKET;
		dptimer[i]	= DP_BITBUCKET;
		dpstatus[i]	= DP_BITBUCKET;
	}

	/*
	 * DP0 and DP1 are always present, on GP and MP boards.
	 */
	dpset[DP0]	= DP_CONTROL0_SET;
	dpclr[DP0]	= DP_CONTROL0_CLEAR;
	dptimer[DP0]	= DP_TIMER0;
	dpstatus[DP0]	= DP_STATUS_LO;

	dpset[DP1]	= DP_CONTROL1_SET;
	dpclr[DP1]	= DP_CONTROL1_CLEAR;
	dptimer[DP1]	= DP_TIMER1;
	dpstatus[DP1]	= DP_STATUS_HI;

	/*
	 * For the MP3, define real address for the VDP
	 */
	if (paragon_mp3) {
		dpset[VDP]	= DP_CONTROL2_SET;
		dpclr[VDP]	= DP_CONTROL2_CLEAR;
		dptimer[VDP]	= DP_TIMER2;
		dpstatus[VDP]	= VDP_STATUS;
	}

	/*
	 * Same for the Memory daughtercard.
	 */
	if (memexp_present) {
	/*	printf("Memory expansion is present...\n"); */
		dpset[MXDP0]	= MXDP_CONTROL0_SET;
		dpset[MXDP1]	= MXDP_CONTROL1_SET;
		dpclr[MXDP0]	= MXDP_CONTROL0_CLEAR;
		dpclr[MXDP1]	= MXDP_CONTROL1_CLEAR;
		dptimer[MXDP0]	= MXDP_TIMER0;
		dptimer[MXDP1]	= MXDP_TIMER1;
		dpstatus[MXDP0] = MXDP_STATUS_LO;
		dpstatus[MXDP1] = MXDP_STATUS_HI;
	}

	/*
	 *	Bits <17:0> inclusive are interrupt
	 *	mask bits.  Clear them.
	 */
	bits = (1 << 18) - 1;
	for (i=0; i<NDPS; i++)
		outl(dpclr[i], bits);

	/*
	 *	Clear any previous clock interrupts
	 */
	bits = 1 << DP_CTRL_CLEAR_TIMER;
	for (i=0; i<NDPS; i++)
		outl(dpset[i], bits);

	/*
	 *	Reprogram the up counters to buy ~80 more seconds
	 *	before a clock interrupt. startrtclock() will
	 *	reprogram when appropriate.
	 *
	 *	XXX MORAL HIGH GROUND WOULD BE TO DISABLE CLOCK
	 *	XXX INTERRUPTS (MASK THEM OUT).
	 */
	for (i=0; i<NDPS; i++)
		outl(dptimer[i], 1);

	/*
	 *	Clean any previous HW errors
	 */
	bits = 1 << DP_CTRL_CLEAR_HWERR;
	for (i=0; i<NDPS; i++)
		outl(dpset[i], bits);
}

/*
 * using the definitions in ecc.h,
 * set the following knobs according to the following bootmagic:
 *
 *	ecc_control_ce, "BOOT_ECC_CONTROL"
 *	parity_control_nic, "BOOT_PARITY_CONTROL_NIC"
 *	parity_control_trap, "BOOT_PARITY_CONTROL_TRAP"
 *	parity_control_intr, "BOOT_PARITY_CONTROL_INTR"
 *	fptrap_verbose, "BOOT_FPTRAP_VERBOSE"
 */
static void
error_control_bootmagic()
{
	int knob, option;

	struct {
		unsigned int *ctlp;
		char *magicstr;
	} knobs[] =
		{
			{ &ecc_control_ce, "BOOT_ECC_CONTROL" },
			{ &parity_control_nic, "BOOT_PARITY_CONTROL_NIC" },
			{ &parity_control_trap, "BOOT_PARITY_CONTROL_TRAP" },
			{ &parity_control_intr, "BOOT_PARITY_CONTROL_INTR" },
			{ &fptrap_verbose, "BOOT_FPTRAP_VERBOSE" },
			{ 0, 0}
		};

	struct {
		char ch;
		int  bits;
	} options[] = {
			{ 'p', ECC_CONTROL_PRINT_ALL },
			{ '2', ECC_CONTROL_PRINT_2 },
			{ 'r', ECC_CONTROL_RED_LED },
			{ 's', ECC_CONTROL_STOP },
			{ 'I', ECC_CONTROL_DISABLE_INTR },
			{ 'C', ECC_CONTROL_DISABLE_CORRECTION },
			{ 'P', ECC_CONTROL_DISABLE_PRESCRUB },
			{ '\0', 0}
		};

	for (knob = 0; knobs[knob].ctlp; ++knob)
	{
		char *str = getbootenv(knobs[knob].magicstr);

		if (!str)
			continue;

		*(knobs[knob].ctlp) = 0;	/* clear default options */

		while (*str)
		{
			for (option = 0; options[option].ch != '\0'; ++option)
			{
				if (options[option].ch == *str)
				{
					*(knobs[knob].ctlp) |=
						options[option].bits;
				}
			}
			str++;
		}
	}
}

paragon_ecc_init()
{
	char	*enabled, *disabled, *completed, *fmt;
	char	*abled;

	/* save a little memory... */
	enabled = "enabled";
	disabled = "disabled";
	completed = "completed";

	if ( ecc_control_ce & ECC_CONTROL_DISABLE_PRESCRUB ) {
		abled = disabled;
	} else {
		paragon_dram_ecc_prescrub();
		abled = completed;
	}
	if (boot_mk_verbose) {
		printf("dram ecc pre-scrub %s.\n", abled);
	}

	/* see i860paragon/ecc.h */
        if ( ecc_control_ce & ECC_CONTROL_DISABLE_CORRECTION ) {
		(void) paragon_dram_ecc(FALSE);
		abled = disabled;
	} else {
		(void) paragon_dram_ecc(TRUE);
		abled = enabled;
	}
	if (boot_mk_verbose) {
		printf("dram ecc correction %s.\n", abled);
	}

        if ( ecc_control_ce & ECC_CONTROL_DISABLE_INTR ) {
		(void) ecc_intr( FALSE );
		abled = disabled;
	} else {
		(void) ecc_intr( TRUE );
                if (memexp_present) {
                        (void) paragon_memexp_ecc_intr( TRUE );
                }
		abled = enabled;
	}
	if (boot_mk_verbose) {
		printf("dram ecc interrupt %s.\n", abled);
	}

}



/*
 *	Basic initialization.
 */
i860_init(symtab_end, ramdisk_start, ramdisk_size, env,
	  svr_start, svr_size, em_start, em_size)
	char		*symtab_end;
	unsigned char	*ramdisk_start;
	unsigned long	ramdisk_size;
	char		*env;
	unsigned char	*svr_start;
	unsigned long	svr_size;
	unsigned char	*em_start;
	unsigned long	em_size;
{
	extern char	stext, etext, sdata, edata, end;
	char *str;

	sploff();

#if	paranoid
	/*
	 *	Clear the BSS.
	 */
	bzero((char *)&edata,(unsigned)(&end - &edata));
#endif	paranoid

	/* paragon_cons_init(); */
	/*
	 *	Initialize the node control register
	 */
	node_control_register_init();
	GREEN_ON(GREEN_COMEUP);
	RED_ON(RED_COMEUP);

	/*
	 *	Determine what kind of board this is. This function will
	 * 	return a TRUE for MP3 and a FALSE for a GP node.
	 */
	paragon_mp3 = node_type();

	/*
	 *	Check for a memory expansion card
	 */
	(void) paragon_memory_expansion_probe();

	/*
	 *	Initialize DP ASICs
	 */
	dp_asic_init();

	system_name = "Paragon XP/S";

	bootenv = envcopy(env);		/* bootmagic! */

	/*
	 *	process bootmagic switches...
	 */
	{
		int			import_paging = TRUE;
		extern pt_entry_t	i860_global_pmap_cache_policy;
		extern int	 	i860_misalignment_policy;
		extern int		la_trigger_spurious;	/* XXX */
		extern int		show_spurious_interrupts; /* XXX */
		extern boolean_t	vm_page_deactivate_behind;
		extern boolean_t	vm_page_deactivate_hint;
		extern boolean_t	vm_map_aggressive_enter_max;
		extern boolean_t	vm_map_aggressive_enter_always;
		extern boolean_t	zone_check;
		extern boolean_t	zone_ignore_overflow;
                extern boolean_t        zone_map_size;
		extern int		vm_page_wire_count_warning;
		extern int		vm_page_gobble_count_warning;
		extern int		kalloc_aligned;
		extern int		kalloc_minsize;
/* %%% */	extern int		old_style_doorbell_service;
#if	MACH_ASSERT
		extern int		port_queue_length_warning;
		extern int		port_queue_length_enabled;
                extern int              port_count_warning;
#endif	MACH_ASSERT

/* %%% */	old_style_doorbell_service =
			getbootint("OLD_DOORBELL", old_style_doorbell_service);

		/*
		 *	Keep the microkernel's symbol table?
		 *	(Default is to keep them)
		 *
		 */
		if (getbootint("KERNEL_SYMBOLS", 1) == 0) {
			esym = &end;
		} else {
			esym = symtab_end;
		}

		/*
		 *	Set warnings for wire and gobble counts.
		 *
		 *	XXX these variables should take a percentage
		 *	XXX count from bootmagic and translate it into
		 *	XXX a real page count.
		 */
		vm_page_wire_count_warning = getbootint("VM_WIRE_WARNING",
					vm_page_wire_count_warning);
		vm_page_gobble_count_warning = getbootint("VM_GOBBLE_WARNING",
					vm_page_gobble_count_warning);

#if	NORMA_IPC
		/*
		 *	Some debugging for NORMA IPC.
		 */
		{
			extern int	netipc_ticks;
			extern int	netipc_resume_timeout_ticks;
			extern int	netipc_resume_backoff;
			extern int	netipc_sync_backoff;
			extern int	max_backoff_ticks;
			extern int	norma_ipc_kmsg_accept_disabled;
			extern int	netipc_congestion_factor;
			extern int	kserver_pageout_thread_priority_init;
			extern int	kserver_thread_max;
			extern int	iodone_thread_fixpri;
			extern int	default_pager_fixpri;

			/*
			 *	override compile-time settings for
			 *	suspend/resume/query timers.
			 */
			netipc_ticks = getbootint("NETIPC_TICKS", netipc_ticks);
			netipc_resume_timeout_ticks =
				getbootint("NETIPC_RESUME_TICKS",
					netipc_resume_timeout_ticks);
			netipc_resume_backoff =
				getbootint("NETIPC_RESUME_BACKOFF",
					netipc_resume_backoff);
			netipc_sync_backoff =
				getbootint("NETIPC_SYNC_BACKOFF",
					netipc_sync_backoff);
			netipc_congestion_factor = 
				getbootint("NETIPC_CONGESTION_FACTOR",
					netipc_congestion_factor);
			max_backoff_ticks = getbootint("MAX_BACKOFF_TICKS",
					max_backoff_ticks);

			/*
			 *	optionally defeat the idle loop in
			 *	norma/ipc_wait.c:norma_ipc_kmsg_accept()
			 */
			norma_ipc_kmsg_accept_disabled =
				getbootint("IPC_KMSG_ACCEPT_DISABLED",
					norma_ipc_kmsg_accept_disabled);

			/*
			 *	default is BASEPRI_SYSTEM; rwd and I had it
			 *	set to 2.  Leave a hook for later...
			 */
			kserver_pageout_thread_priority_init =
				getbootint("KSERVER_PAGEOUT_THREAD_PRIORITY", 2);

			/*
			 *	override the max number of kserver threads.
			 */
			kserver_thread_max =
				getbootint("KSERVER_THREAD_MAX",
					kserver_thread_max);


			/*
			 *	should the iodone thread be fixpri?
			 *
			 *	 < 0 means not fixed pri
			 *	>= 0 means fixed pri at pri
			 */
			iodone_thread_fixpri =
				getbootint("IODONE_THREAD_FIXPRI", 0);

			/*
			 *	should the default pager threads be fixpri?
			 *
			 *	 < 0 means not fixed pri
			 *	>= 0 means fixed pri at pri
			 */
			default_pager_fixpri = 
				getbootint("DEFAULT_PAGER_FIXPRI", 0);
		}
#endif	NORMA_IPC

		/*
		 *	some switches for kalloc():
		 *
		 *	kalloc_aligned - cause kalloc() to return
		 *		"naturally aligned" memory.
		 *
		 *	kalloc_minsize - configure the minimum size
		 *		of memory to be returned by kalloc().
		 *		(It must be >0 and a power of 2)
		 */
		kalloc_aligned =
			getbootint("KALLOC_ALIGNED", kalloc_aligned);
		kalloc_minsize =
			getbootint("KALLOC_MINSIZE", kalloc_minsize);
		if (kalloc_minsize < 4)
			kalloc_minsize = 4;
		if (kalloc_minsize > 256)
			kalloc_minsize = 256;
		assert((kalloc_minsize & (kalloc_minsize - 1)) == 0);

		/*
		 *	some VM tuning parameters
		 */
		vm_page_deactivate_behind =
			getbootint("VM_PAGE_DEACTIVATE_BEHIND",
					vm_page_deactivate_behind);
		vm_page_deactivate_hint =
			getbootint("VM_PAGE_DEACTIVATE_HINT",
					vm_page_deactivate_hint);
		vm_map_aggressive_enter_max =
			getbootint("AGGRESSIVE_ENTER_MAX",
					vm_map_aggressive_enter_max);
		vm_map_aggressive_enter_always =
			getbootint("AGGRESSIVE_ENTER_ALWAYS",
					vm_map_aggressive_enter_always);

		/*
		 *	enable zone checking?
		 */
		zone_check = getbootint("ZONE_CHECK", zone_check);

		/*
		 *	panic if a non-expandable, non-exhaustible zone
		 *	is about to overflow?
		 */
		zone_ignore_overflow = getbootint("ZONE_IGNORE_OVERFLOW",
					zone_ignore_overflow);
                zone_map_size = getbootint("ZONE_MAP_SIZE",zone_map_size);

#if	MACH_ASSERT
		/*
		 *	if the number of messages queued on any port
		 *	exceeds port_queue_length_warning and
		 *	port_queue_length_enabled is non-zero,
		 *	trigger an assertion.
		 */
		port_queue_length_warning =
			getbootint("PORT_QUEUE_LENGTH_WARNING",
					port_queue_length_warning);
		port_queue_length_enabled =
			getbootint("PORT_QUEUE_LENGTH_ENABLED",
					port_queue_length_enabled);
                port_count_warning =
                        getbootint("PORT_COUNT_WARNING", port_count_warning);
#endif	/* MACH_ASSERT */


		/*
		 *	This node's node id
		 */
		ipsc_physnode = getbootint("BOOT_MY_NODE", ipsc_physnode);


		/*
		 * 	Verbosity control during bootup.
		 */
		boot_mk_verbose = getbootint("BOOT_MK_VERBOSE", FALSE);
		if (ipsc_physnode == getbootint("BOOT_FIRST_NODE", -1)) {
			boot_mk_verbose =
				getbootint("BOOT_FIRST_MK_VERBOSE",
					   boot_mk_verbose);
		}

		/*
		 * set ECC and Parity handling policies
		 */
		error_control_bootmagic();

		/*
		 *	A long time ago, it was useful to not enable
		 *	DRAM refresh for awhile.
		 */
		if (getbootenv("DEFER_REFRESH") != 0) {
			node_control_register_set(LB_NODE_CTRL_REF_DEF);
		}


		/*
		 *	While debugging the pre-loaded program stuff,
		 *	it's useful to lie to the bootnode and pretend
		 *	that the "ramdisk" is the server executable.
		 */
		if (getbootint("RAMDISK_IS_SERVER", 0)) {
			svr_start = ramdisk_start;
			svr_size = ramdisk_size;
			ramdisk_start = 0;
			ramdisk_size = 0;
		}


		/*
		 *	Misaligned access policy choice
		 *
		 *	SIGNAL_MISALIGNED=1 (default) raise exception
		 *	SIGNAL_MISALIGNED=0 silent handling
		 */
		i860_misalignment_policy =
		    getbootint("SIGNAL_MISALIGNED", i860_misalignment_policy);


		/*
		 *	More hardware debug...these could be removed.
		 */
/* XXX */	la_trigger_spurious =
			getbootint("LA_TRIGGER_SPURIOUS", la_trigger_spurious);

		show_spurious_interrupts =
			getbootint("SHOW_SPURIOUS", show_spurious_interrupts);


		/*
		 *	Cache policy choice (fully cached, write-thru,
		 *	or caches disabled).
		 */
		i860_global_pmap_cache_policy =
			getbootint("PMAP_CACHE_POLICY",
				   i860_global_pmap_cache_policy) &
			(INTEL_PTE_NCACHE|INTEL_PTE_WTHRU);

		/*
		 * If an RPM is present, we map it into all address spaces.
		 * But a DOT with an RPM needs a way to disable this mapping
		 * so that dclock(3) doesn't get confused by a 56-bit global
		 * counter that isn't counting.  (no 10MHz backplane signal)
		 * Set map_rpm before calling print_rpm(), or pmap_bootstrap().
		 */
		map_rpm = getbootint("BOOT_MAP_RPM", 1);

		/*
		 * Lights
		 */
		init_lights();

	}

#if	NMD > 0
	md_address = ramdisk_start;
	md_size = ramdisk_size;
#endif	NMD > 0

	/*
	 * Set _node_self
	 */
	_node_self = ipsc_physnode;


#if	NCPUS > 1
	/*
	 * read hardware to get my cpu number, save it in processor protected
	 * register so cpu_number() can work quickly.
	 * Initialize startup_lock.
	 */
	set_my_cpu_number( master_cpu );	/* so cpu_number() works */
#endif	/*  NCPUS > 1 */

	/*
	 *	Pre-loaded server logic...
	 */
	if (paragon_preloaded_program_protect("server", svr_start, svr_size)<0){
		panic("protect preloaded server");
	}
	if (paragon_preloaded_program_protect("emulator", em_start, em_size)<0){
		panic("protect preloaded emulator");
	}


	/*
	 * Determine memory size of this node
	 */
	size_memory();


	/*
	 * Bring up a console prior to autoconfig
	 */
	paragon_cons_init();

	if (fscan_secondary_console)
		fscan_cons_probe((caddr_t)1, NULL);

	/* if 0, don't map the rpm, even if present */
	map_rpm = getbootint("BOOT_MAP_RPM", 1);

	/*
	 * Some interesting information about this card
	 */
	if (boot_mk_verbose) {
		printf("%s boot: ", system_name);
		print_chip_type();
	}
	paragon_determine_clock_freq();
	if (boot_mk_verbose) {
		print_node_status();
		print_nicrev();
		print_dprev(dpstatus[DP0], 0);
		print_expansion();
		print_rpm();
	} else {
		node_status_register_expansion_present();
		paragon_rpm_present();
	}

	/*
	 * Stop here if kernel download garbled
	 */
	print_checksum();

	/*
	 * Optionally Enable ECC
	 */
	paragon_ecc_init();

	/*
	 *	Debugging for kernel memory layout...
	 */
	if (boot_mk_verbose) {
		print_range("text", &stext, &etext);
		print_range("data", &sdata, &edata);
		print_range("bss ", &edata, &end);
		print_range("syms", &end, esym);
#if	NMD > 0
		if (md_size != 0) {
			print_range("ramd", md_address, md_address + md_size);
		}
#endif	NMD > 0
		if (svr_size != 0) {
			print_range("serv", svr_start, svr_start + svr_size);
		}
		if (em_size != 0) {
			print_range("emul", em_start, em_start + em_size);
		}

		print_range("baseboard memory", addr_base, addr_last);

		if (memexp_start) {
			print_range("expansion memory",
				memexp_start, memexp_end);
		}

		printf("VM page size: %dK bytes.\n",(page_size/1024));

		printf("physical node %d\n", ipsc_physnode);
	}

	/*
	 * Create the initial kernel map, and enable address translation.
	 */
	hole_start = trunc_page(addr_load);
	hole_end = round_page(addr_first);
	avail_start = trunc_page(addr_base);
	avail_end = round_page(addr_last);
	pmap_bootstrap(addr_load);

	if (!getbootint("BOOT_KERNEL_CACHE_TEXT", 1))
		pmap_bootstrap_mark_nocache(&stext, &etext);
	if (!getbootint("BOOT_KERNEL_CACHE_DATA", 1))
		pmap_bootstrap_mark_nocache(&sdata, &edata);

	/*
	 *	Initialize for pmap_free_pages and pmap_next_page.
	 */
	avail_remaining = atop((avail_end - avail_start) -
			       (hole_end - hole_start));
	if (memexp_present) {
		avail_remaining += atop(memexp_end - memexp_start);
		avail_next = memexp_start;
	} else {
		avail_next = avail_start;
	}
}


#if	1
/*
 *	delay for a few microsecs using a deterministic spin loop.
 */
delay(microsecs)
	unsigned long	microsecs;
{
	int	s;

	/*
	 *	_spin() requires 2 CPU clocks per iteration plus
	 *	6 clocks for setup and return.  Examples:
	 *
	 *	CPU	time/	iter/
	 *	MHz	iter	microsecond
	 *	---------------------------
	 *	25	80ns	12 (approx)
	 *	33	60ns	16 (approx)
	 *	40	50ns	20
	 *	50	40ns	25
	 *
	 *	Limit the amount of time spent spinning to no more
	 *	than 5 seconds
	 */
	if (microsecs > (5 * 1000000))
		microsecs = (5 * 1000000);
	if (microsecs > 0) {
		s = sploff();
		_spin(paragon_node_spin_factor * microsecs);
		splon(s);
	}
}
#endif	1


reboot_expansion_install(new, old)
	void	(*new)(), (**old)();
{
	*old = expansion_reboot;
	expansion_reboot = new;

	return 0;
}


static char	*default_startup_name = "/mach_servers/startup";
extern char	*startup_name, *emulator_name;

boolean_t paragon_bootstrap_askname()
{
	char		*server, *emul;

	if (getbootint("BOOT_860_NO_SERVER", FALSE) ||
	    getbootint("BOOT_NO_SERVER", FALSE)) {
		return FALSE;
	}

	if ((server = getbootenv("BOOT_860_STARTUP_NAME")) != 0) {
		startup_name = server;
	} else if ((server = getbootenv("BOOT_STARTUP_NAME")) != 0) {
		startup_name = server;
	} else {
		startup_name = default_startup_name;
	}

	if ((emul = getbootenv("BOOT_860_EMULATOR_NAME")) != 0) {
		emulator_name = emul;
	} else if ((emul = getbootenv("BOOT_EMULATOR_NAME")) != 0) {
		emulator_name = emul;
	}

	if (boot_mk_verbose) {
		printf("Server name is %s.\n", startup_name);
		printf("Emulator name is %s.\n", emulator_name);
	}

	return TRUE;

}


/*
 *	a place holder for some experimental back-off
 *	logic for NORMA IPC.
 */
int pseudo_random(max)
	int	max;
{
	unsigned long	now;

	now = inl(DP_EPOCH_LO);
	return now % max;
}


/*
 *	A hook from the pmap module for memory mapped devices.
 *
 *	XXX REVISIT WHEN THE MEMORY DAUGHTERCARD IS AVAILABLE.
 */
boolean_t paragon_is_dram_address(paddr)
	vm_offset_t	paddr;
{
	static vm_offset_t	dram_base, dram_end;

	if (dram_end == 0) {
		dram_base = node_memory_base();	
		dram_end = dram_base + node_memory_size();
	}

	if (memexp_present) {
		if ((paddr >= memexp_start) && (paddr < memexp_end))
			return TRUE;
	}

	if ((paddr >= dram_base) && (paddr < dram_end))
		return TRUE;

	return FALSE;
}

/*
 * Supply a timestamp, called from norma/tr.c
 */
void fc_get(unsigned int *ts)
{
	ts[0] = inl(DP_EPOCH_LO);
	ts[1] = 0;
}

/*
 * bitvector representing NORMA nodes. 1 == node is alive, otherwise dead.
 */
unsigned norma_node_status[ (IP_NORMA_MAX_NODE+1)/32 ];

/*
 * perform requested operation on norma node status bitvector.
 *
 *	op		function:
 * ------------------------------
 *	0	is specified node valid? Return a boolean indicating
 *		  if the norma node is alive( !=0 ) or dead( == 0).
 *	1	set node as dead; don't care about return value.
 *	2	set node as alive; don't care about return value.
 *
 * inputs:
 *	op	operation to perform.
 *	n	norma node number 0..max_num_nodes
 *
 * outputs:
 *	see above.
 */

norma_node_status_op( op, n )
	int		op;
	unsigned	n;
{
	register unsigned	bits, rc;
	register unsigned	*status;
	extern	boolean_t	debug_kernel;

	/*
	 * range check, catch the easy ones first.
	 */
	if ( n < 0 || n > max_num_nodes )
		return FALSE;

	bits = (1 << (n % 32));
	status = &norma_node_status[ ( n / 32) ];

	switch ( op ) {
	  case 0:
		/*
		 * is this node valid? If we are a DEBUG node then lie and say
		 * any node is valid. Assumes the user is aware!!
		 */
		if ( debug_kernel )
			rc = TRUE;
		else
			rc = *status & bits;
		break;
	  case 1:
		/* clear: node is dead */
		*status &= ~bits;
		break;
	  case 2:
		/* set node alive */
		*status |= bits;
		break;
	}
	return rc;
}

/*
 * process the bootnode list assuming all listed nodes will come online during
 * bootmesh. "bootmesh" will reset the nodes which did not boot.
 *
 * This routine will go away when 'bootmesh' does the proper get/set node
 * status MK syscalls.
 */

set_norma_node_status_alive(bnl)
	char	*bnl;
{
	register boolean_t	done;
	char			*term;

	if ( bnl == (char *)0 )
		return;

	for( term=bnl,done=FALSE; !done; term++ ) {
		register int	node;

		node = atoi_term(term, &term);
		if ( *term == '\0' )
			done = TRUE;
		/*
		 * process a node range, example: "4..9"
		 */
		if ( *term == '.' ) {
			register int	enode;

			if ( *(term+1) != '.' ) {
				term++;	/* bad syntax */
				continue;
			}
			term += 2;	/* skipover ".." */
			enode = atoi_term(term, &term);	/* get ending node # */
			if ( *term == '\0' )
				done = TRUE;
			/*
			 * mark the nodes in the range as alive.
			 * do start thru end-1; fall into single-node code
			 * for the range-end node.
			 */
			for( ; node < enode; node++ ) {
				norma_node_status_op( 2, node );
			}
		}
		norma_node_status_op( 2, node );
	}
}


/*
 * Interface exported to User land.
 *
 * inputs:
 *	host	mach_host_self()
 *	node	norma node of interest
 *	status	pointer to status boolean
 * outputs:
 *	KERN_SUCCESS | KERN_INVALID_ARGUMENT
 *
 * side effects:
 *	*status modified to reflect the specified node's status on/off-line
 */

norma_node_is_valid( host, node, status)
	host_t		host;
	int		node;
	boolean_t	*status;	/* OUT */
{
	*status = FALSE;	/* assume the worst */

	if ( host == HOST_NULL ) {
		return KERN_INVALID_ARGUMENT;
	}

	if ( norma_node_status_op( 0, node ) )
		*status = TRUE;

	return(KERN_SUCCESS);
}

/*
 * Return Norma node status bitvector to the caller.
 *
 * inputs:
 *	host		mach_host_self()
 *	nodeStatus	pointer to bitvector of 128 ints (4096 NOrma nodes).
 *
 * outputs:
 *	KERN_SUCCESS | KERN_INVALID_ARGUMENT
 *
 * side effects:
 *	none.
 */

norma_node_get_status( host, nodeStatus )
	host_t		host;
	node_status_t	nodeStatus;
{
	register int	cnt;

	if  (host == HOST_NULL ) {
		return KERN_INVALID_ARGUMENT;
	}

	bcopy(norma_node_status, nodeStatus, sizeof(node_status_t) );

	return(KERN_SUCCESS);
}

/*
 * Set Norma node status bitvector according to passed in bitvector.
 *
 * inputs:
 *	host		host priv port.
 *	nodeStatus	pointer to bitvector of 128 ints (4096 NOrma nodes).
 *
 * outputs:
 *	KERN_SUCCESS | KERN_INVALID_ARGUMENT
 *
 * side effects:
 *	*nodeStatus array is modified.
 */

norma_node_set_status( host, nodeStatus )
	host_t		host;
	node_status_t	nodeStatus;
{
	register int	cnt;

	if  (host == HOST_NULL ) {
		return KERN_INVALID_ARGUMENT;
	}

	bcopy(nodeStatus, norma_node_status, sizeof(node_status_t) );

	return(KERN_SUCCESS);
}



unsigned long	flush_stats_tlb;
unsigned long	flush_stats_switch;
unsigned long	flush_stats_flush;

#if	MACH_KDB
int db_flush_stats(int reset)
{
	unsigned long	total;

	if (reset) {
		flush_stats_tlb = 0;
		flush_stats_switch = 0;
		flush_stats_flush = 0;
		return 0;
	}

	total = flush_stats_tlb + flush_stats_switch + flush_stats_flush;

	db_printf("flush statistics:\n");
	db_printf("  tlb ......... %8u\n", flush_stats_tlb);
	db_printf("  switch ...... %8u\n", flush_stats_switch);
	db_printf("  flush loop .. %8u\n", flush_stats_flush);
	db_printf("  total ....... %8u\n", total);

	return (int) total;

}
#endif	/* MACH_KDB */

#if	NORMA2
int dipc_max_nodes()
{
	return max_num_nodes;
}
#endif	/* NORMA2 */

/*
 * Return current value of DP cycle counter.
 * format is double, units are seconds.
 */
double
dclock()
{
        union {
                unsigned short  words[4];
                double          dbl;
        } now;

        now.dbl = *DP_EPOCH_DADDR;	/* read 64-bit DP cycle counter */
        now.words[3] = (now.words[3] & 0x000f) | 0x4330;

        return (1.0 / (paragon_node_mhz * 1000000.0))
                * (now.dbl - 4503599627370496.0);
}

