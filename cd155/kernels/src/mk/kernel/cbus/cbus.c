/*
 * 
 * $Copyright
 * Copyright 1991, 1992, 1993, 1994, 1995 Intel Corporation
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
 * 09-Oct-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Allow softclock on all cpus.
 *
 * 24-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Moved code from cbus_debug.c to here.
 *	Fixed cbus_kvtoAT(virt) to detect no VGA remapping
 *
 * $Log: cbus.c,v $
 * Revision 1.5.8.1  1995/06/11  18:30:15  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.5  1994/11/18  20:25:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:16:34  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:14:07  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:13:41  dleslie
 * First R1_0 release
 *
 * Revision 2.1.9.5  92/09/15  17:13:53  jeffreyh
 * 	Fixed call to clock interrupt for accurate profiling
 * 	[92/08/20            bernadat]
 * 
 * Revision 2.1.9.4  92/06/24  17:58:08  jeffreyh
 * 	Due to tty code, had to change the at386_io_lock() routine,
 * 	drivers now can sleep while locked on master.
 * 	[92/06/03            bernadat]
 * 
 * Revision 2.1.9.3  92/05/26  11:18:10  jeffreyh
 * 	Fixed cbus_io_lock/cbus_io_unlock not to unbind
 * 	bound threads like X server. (This bug appeared
 * 	when merge with compaq code was done).
 * 	[92/05/18            bernadat]
 * 
 * Revision 2.1.9.2  92/04/30  11:44:54  bernadat
 * 	Moved shared CBUS/MBUS code to i386at/mp
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.1.9.1  92/02/18  18:35:25  jeffreyh
 * 	Added routines for converting virt. addr. to AT bus phys. addr.
 * 	[91/12/10            bernadat]
 * 
 * 	Added Missing call to ast_check
 * 	Fixed parameters passed to interrupt
 * 	Changed call to clock interrupt for profiling
 * 	[91/09/16            bernadat]
 * 
 * 	Created
 * 	[91/06/27  05:01:04  bernadat]
 * 
 */

/*
 * Corollary 386 MP
 * Copyright (c) 1990 OSF Research Institute 
 */

/*
 * Copyright 1990 by Open Software Foundation,
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

#include <cpus.h>
#include <platforms.h>
#include <eisa.h>
#include <mach_prof.h>
#include <mach_kdb.h>
#include <mp_slave_as_clk.h>

#include <sys/types.h>
#include <mach/i386/vm_types.h>

#include <mach/boolean.h>
#include <kern/thread.h>
#include <kern/zalloc.h>

#include <kern/lock.h>
#include <kern/processor.h>

#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <vm/vm_page.h>
#include <vm/vm_user.h>

#include <mach/i386/vm_param.h>
#include <i386/machparam.h>
#include <i386/trap.h>
#include <i386/eflags.h>

#include <i386at/eisa.h>
#include <i386at/mp/mp.h>
#include <i386at/mp/boot.h>
#include <cbus/cbus.h>

#include <mach/machine.h>

#include <kern/time_out.h>
#include <i386/thread.h>
#include <i386/ipl.h>

#include <device/tty.h>
#include <device/dev_master.h>

char 	*cbus_regs;	/* where cbus window and status registers	*/
			/* are mapped, for master only			*/
int	cbus_free_wind = 0;		/* first availbale free window	*/

int	cbus_cpu[CBUS_NSLOTS];	/* each entry gives the cpu number or	*/
				/* -1 if empty slot			*/
int	cbus_slot[CBUS_NCPUS];	/* each entry gives the slot number or	*/
				/* -1 if no such cpu */

#ifndef	Z1000
vm_offset_t cbus_io_slot[CBUS_NSLOTS];	/* pointer to io space, indexed */
					/* by slot number		*/
vm_offset_t cbus_io_cpu[CBUS_NCPUS];	/* pointer to io space, indexed */
					/* by cpu number 		*/
#else	Z1000
	/* for Z1000, the io space must be accessed through cbus 	*/
	/* windows from master						*/
vm_offset_t master_io_addr[CBUS_NCPUS];	/* pointer to IO space, for	*/
					/* master only, indexed by cpu	*/ 
vm_offset_t slave_io_addr[CBUS_NCPUS];	/* pointer to IO space, for 	*/
					/* slaves only, indexed by cpu	*/
#endif	Z1000

vm_offset_t phys_map();		/* Used to map phys space		*/

/* Write to CBUS IO space, through windows, used at init time from master */

w_cbus_io(slot, opcode) {
#ifdef	Z1000
	cbus_set_win(cbus_free_wind, CBUS_CIO_OP2AD(slot, opcode));
	*(u_char *)(cbus_get_win_vadd(cbus_free_wind)+opcode) = 0;
#else	Z1000
	*(u_char *)(cbus_io_slot[slot]+opcode) = 0;
#endif	Z1000
}

#ifdef	Z1000
/* Write to CBUS IO space */

cbus_op(cpu, opcode)
{
#if	NCPUS > 1
	if (cpu_number())
		*((u_char *)slave_io_addr[cpu] + opcode) = 0;
	else
#endif	NCPUS > 1
		*((u_char *)master_io_addr[cpu] + opcode) = 0;
}
#endif	Z1000

cpu_interrupt(cpu) {
	cbus_set_intr(cpu);
}

clear_led(cpu) {
	cbus_op(cpu, CBUS_CLED);
}

set_led(cpu) {
	cbus_op(cpu, CBUS_SLED);
}

u_char
cbus_getstat() {
#ifdef	Z1000
	return(*(u_char *)CBUS_REG2KV(CBUS_CBUS_STAT));
#else	Z1000
	return(inb(0xf1));
#endif	Z1000
}

/*
 * Map some physical space into virtual memory with R/W access
 * phys_addr:	Physical byte address
 * phys_size:	size in bytes
 *
 * returns virtual address
 *
 * Warning: Performs roundings to map on page boundaries, might map
 * more than needed
 */

vm_offset_t
phys_map(phys_addr, phys_size)
vm_offset_t	phys_addr;
unsigned	phys_size;
{
	vm_offset_t	vad, pad;
	unsigned length;

	pad = trunc_page(phys_addr);
	length = round_page((phys_addr - pad) + phys_size);
	if (kmem_alloc_pageable(kernel_map, &vad, length) != KERN_SUCCESS) {
		printf("phys_map(%x , %x) failed, could not allocate virtual memory\n", phys_addr, phys_size);
		return(vad);
	}
	(void)pmap_map_bd(vad, pad, 
			(vm_offset_t)pad+length, 
			VM_PROT_READ | VM_PROT_WRITE);
	return(vad + (phys_addr - pad));
}

#if	NCPUS > 1

pcb_t	prev_pcb[NCPUS];	/* used at thread switch time		*/

freeze_all_processors(cpu) {
	printf("dont know how to freeze other processors\n");
}
thaw_all_processors() {
	printf("dont know how to thaw other processors\n");
}

/* Word describing origin of interrupt */

int	cpu_int_word[NCPUS];

u_char	cbus_ochar, cbus_ichar;
int	cbus_wait_char;		/* must wait for character		*/
int	cbus_nintr[NCPUS];	/* total number of interrupts		*/
int	cbus_nclock[NCPUS];	/* number of clock interrupts		*/
int	cbus_nast[NCPUS];	/* number of remote ASTs 		*/
int	cbus_ntlb[NCPUS];	/* number of tlb shutdowns		*/


extern void	clock_interrupt();

/* CBUS interrupt routine */
/* Need to replace this code with some new one using ffs() */

mp_intr(vec, old_ipl, ret_addr, regs)
struct i386_interrupt_state *regs;
char *	ret_addr;	/* return address in interrupt handler */
{
	register mycpu = cpu_number();
	volatile int	*my_word = &cpu_int_word[mycpu];
	extern char	return_to_iret[];

	cbus_nintr[mycpu]++;
	do {
		if (i_bit(MP_CLOCK, my_word)) {
			i_bit_clear(MP_CLOCK, my_word);
			cbus_nclock[mycpu]++;
			hardclock(vec, old_ipl, ret_addr, regs);
		} else if (i_bit(MP_TLB_FLUSH, my_word)) {
			i_bit_clear(MP_TLB_FLUSH, my_word);
			cbus_ntlb[mycpu]++;
			pmap_update_interrupt();
		} else if (i_bit(MP_SOFTCLOCK, my_word)) {
			i_bit_clear(MP_SOFTCLOCK, my_word);
			softclock();
		} else if (i_bit(MP_AST, my_word)) {
			i_bit_clear(MP_AST, my_word);
			cbus_nast[mycpu]++;
			ast_check();
		} else if (i_bit(CBUS_PUT_CHAR, my_word)) {
			volatile u_char c = cbus_ochar;
			i_bit_clear(CBUS_PUT_CHAR, my_word);
			cnputc(c);
		} else if (i_bit(CBUS_GET_CHAR, my_word)) {
			if (cbus_wait_char)
				cbus_ichar = cngetc();
			else
				cbus_ichar = cnmaygetc();
			i_bit_clear(CBUS_GET_CHAR, my_word);
		} else if (i_bit(MP_KDB, my_word)) {
			extern kdb_is_slave[];

			i_bit_clear(MP_KDB, my_word);
			kdb_is_slave[mycpu]++;
			kdb_kintr();
		}
		cbus_clear_intr(mycpu);
	} while (*my_word);
}

/* print a character on master cpu console, called from a slave */

cbus_putc(c)
u_char c;
{
	volatile int	*master_word = &cpu_int_word[0];

	while(i_bit(CBUS_PUT_CHAR, master_word));
	cbus_ochar = c;
	i_bit_set(CBUS_PUT_CHAR, master_word);
	cpu_interrupt(0);
}

/* print a character on master cpu console, called from a slave */

cbus_getc(wait)
{
	volatile int	*master_word = &cpu_int_word[0];

	cbus_wait_char = wait;
	i_bit_set(CBUS_GET_CHAR, master_word);
	cpu_interrupt(0);
	while(i_bit(CBUS_GET_CHAR, master_word));
	return(cbus_ichar);
}


cbus_ill_io(pc) {
	extern int db_active;
	printf("386 IO instruction not allowed on slave cpus, called by %x\n", pc);
	if (!db_active)
		Debugger();
}

#endif	NCPUS > 1

/*
 * Allocate cbus windows
 */

cbus_alloc_win(size)
{
	int rc;

	if (is_eisa_bus)
		panic("No CBUS windows with EISA bus");
	if (cbus_free_wind + size > CBUS_N_WIN)
		panic("Out of Cbus windows");
	rc = cbus_free_wind;
	cbus_free_wind += size;
	return(rc);
}

#if	MP_SLAVE_AS_CLK && Z1000
hi_res_clk_mmap(dev,off,prot)
               vm_prot_t  prot;
{
               extern int *high_res_clock;
               if (prot & VM_PROT_WRITE) return (-1);
               return (i386_btop(pmap_extract(pmap_kernel(), high_res_clock)));
}

#endif	MP_SLAVE_AS_CLK && Z1000

#if	NCPUS > 1 && TIME_STAMP

/*
 *	Print time stamp rate
 */

stamp_rate()
{
	extern unsigned time_stamp, time_stamp_cum, nstamps, old_time_stamp;
	
	printf("%d stamps per clock interrupt\n", time_stamp_cum/nstamps);

}

#endif	NCPUS > 1 && TIME_STAMP


	/* 
         * I/O support.
	 * Convert virtual addresses to AT physical addresses.
         *
	 * for CBUS, addresses start at 4000000 (CBUS_START) and are
	 * not visible to AT boards. We must use regular AT addresses
         * which point to the same physical location. This only works
         * for addresses below 16 Megs. Otherwise must use windows.
	 */

cbus_kvtoAT_ww(virt, window) {
	cbus_set_win(window, kvtophys(virt));
	return(cbus_get_win_padd(window) + (virt & (I386_PGBYTES - 1)));
}

cbus_kvtoAT(virt) {
	vm_offset_t phys = kvtophys(virt);
	
 	if (phys > CBUS_START + MB(15) ||
 	    ((phys >= CBUS_START+0xA0000) && (phys < CBUS_START+0x100000))) {
		printf("cbus_kvtophys(%x), phys = %x\n", virt, phys);
		panic("Address not remapped to AT bus");
	} else 
		return(cbus2at(phys));
}

/*
 * lock on master for I/Os
 */

extern vm_offset_t	int_stack_top[];

boolean_t
cbus_io_lock(saved_processor)
processor_t	*saved_processor;
{
 	thread_t thread;
	
	if ((vm_offset_t) &saved_processor < int_stack_top[cpu_number()])
		return(TRUE);	/* Interrupt mode */
	thread = current_thread();
	*saved_processor = thread->bound_processor;
	thread_bind(thread, master_processor);
	if (current_processor() != master_processor)
	    thread_block((void (*)()) 0);
	return(TRUE);
}

cbus_io_unlock(saved_processor)
processor_t	saved_processor;
{
	if ((vm_offset_t) &saved_processor < int_stack_top[cpu_number()])
		return;	/* Interrupt mode */
	thread_bind(current_thread(), saved_processor);
	if ( saved_processor != PROCESSOR_NULL &&
	    current_processor() != saved_processor)
	    	thread_block((void (*)()) 0); 
}
#include <cpus.h>
#include <mach_kdb.h>
#include <sys/types.h>
#include <mach/i386/vm_types.h>
#include <kern/lock.h>

#if	(NCPUS > 1) && MACH_KDB

extern u_char	cbus_ochar, cbus_ichar;
extern	int	cbus_wait_char;

/*
 * Machine dependant code for kdb
 *
 * Console interrupt only pop on master CPU. When looping in lcok_kdb,
 * master cpu must check for console interrupts and pass characters
 * to ddb on the active cpu.
 * Also Characters can only be printed by master CPU, so
 * when looping in lock_kdb(), master must also be prepared to receive
 * characters to print from the ddb on the active cpu.
 */

kdb_console() {
	int	my_cpu = cpu_number();
	volatile int	*my_word = &cpu_int_word[my_cpu];

	if (!my_cpu) {
		if (i_bit(CBUS_PUT_CHAR, my_word)) {
			volatile u_char c = cbus_ochar;
			i_bit_clear(CBUS_PUT_CHAR, my_word);
			cnputc(c);
		} else if (i_bit(CBUS_GET_CHAR, my_word)) {
			if (cbus_wait_char)
				cbus_ichar = cngetc();
			else
				cbus_ichar = cnmaygetc();
			i_bit_clear(CBUS_GET_CHAR, my_word);
		/* trying to detect break */
#ifndef notdef
		} else if (!cnmaygetc()) { 
#else	notdef
		} else if (com_is_char() && !com_getc(TRUE)) {
#endif notdef
			simple_unlock(&kdb_lock);
			kdb_cpu = my_cpu;
		}
	}
}

#endif	(NCPUS > 1) && MACH_KDB
