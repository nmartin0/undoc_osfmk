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
 * 09-Oct-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Allow softclock on all cpus.
 *	Fixed clock_interrupt handling.
 *
 * 24-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Moved common ddb code to i386/db_interface.c
 *
 * $Log: mbus.c,v $
 * Revision 1.5  1994/11/18  20:55:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:49:27  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:44:40  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:46:29  dleslie
 * First R1_0 release
 *
 * Revision 2.1.3.1  92/04/30  11:59:47  bernadat
 * 	Code for MBUS based machines: SystemPro
 * 	[92/04/08            bernadat]
 * 
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

/*
 * Support for the COMPAQ SystemPro MBUS architecture
 *
 * WARNING: 	The only tested configurations are 2 or 2 486 cpus with
 * 		8 or 40 Megs of memory.
 *
 * The hd and wd drivers have been parallelized. The com, kd and fd
 * ones didnt seem to have any trouble. The fd driver has been changed
 * tu support memory addresses above 16 Megs.
 */

#include <cpus.h>
#include <mach_mp_debug.h>
#include <mach_lock_mon.h>
#include <time_stamp.h>
#include <mach_kdb.h>
#include <sys/types.h>
#include <mach/i386/vm_types.h>
#include <mach/i386/vm_param.h>
#include <i386/machparam.h>
#include <i386/trap.h>
#include <chips/busses.h>
#include <kern/time_out.h>
#include <i386/setjmp.h>
#include <i386/ipl.h>
#include <i386/db_machdep.h>
#include <mach/machine.h>
#include <kern/cpu_number.h>
#include <i386at/mp/boot.h>
#include <i386/seg.h>
#include <vm/vm_map.h>
#include <i386at/mp/mp.h>
#include <mbus/mbus.h>

#if	NCPUS > 1

extern nulldev();
int	mbus_init();


caddr_t	mbus_std[1] = { 0 };
struct	bus_device *mbus_info[1];
struct	bus_ctlr *mbus_minfo[1];
struct	bus_driver	mbusdriver = {
	mbus_init, nulldev, nulldev, 0, mbus_std, "mbus", mbus_info, "mbus", mbus_minfo, 0};


mbus_init(port, ui)
struct bus_ctlr *ui;
{
	printf("P1_ID: %d, P2_ID %d\n", inb(P1_ID), inb(P2_ID));
/*	take_ctlr_irq(ui); Uses same level as fpu, see i386a/pic_isa.c */
}

int	mp_nintr[NCPUS];	/* total number of interrupts		*/
int	mp_nclock[NCPUS];	/* number of clock interrupts		*/
int	mp_nast[NCPUS];	/* number of remote ASTs 		*/
int	mp_ntlb[NCPUS];	/* number of tlb shutdowns		*/

mp_intr(vec, old_ipl, ret_addr, regs)
struct i386_interrupt_state *regs;
char *	ret_addr;	/* return address in interrupt handler */
{
	register mycpu = cpu_number();
	volatile int	*my_word = &cpu_int_word[mycpu];
	extern char	return_to_iret[];

	if(!(inb((mycpu) ? P2_CTRL : P1_CTRL) & MBUS_INT_SET)) {
		fpintr();
		return;
	}

	mp_nintr[mycpu]++;
	do {
		if (i_bit(MP_CLOCK, my_word)) {
			i_bit_clear(MP_CLOCK, my_word);
			mp_nclock[mycpu]++;
			hardclock(vec, old_ipl, ret_addr, regs);
		} else if (i_bit(MP_TLB_FLUSH, my_word)) {
			i_bit_clear(MP_TLB_FLUSH, my_word);
			mp_ntlb[mycpu]++;
			pmap_update_interrupt();
		} else if (i_bit(MP_SOFTCLOCK, my_word)) {
			i_bit_clear(MP_SOFTCLOCK, my_word);
			softclock();
		} else if (i_bit(MP_AST, my_word)) {
			i_bit_clear(MP_AST, my_word);
			mp_nast[mycpu]++;
			ast_check();
		} else if (i_bit(MP_KDB, my_word)) {
			extern kdb_is_slave[];

			i_bit_clear(MP_KDB, my_word);
			kdb_is_slave[mycpu]++;
			kdb_kintr();
		}
		mbus_clear_intr(mycpu);
	} while (*my_word);
}

cpu_interrupt(cpu) {
	outb((cpu) ? P2_CTRL : P1_CTRL, MBUS_INT_SET|MBUS_CACHE_EN);
}

mbus_clear_intr(cpu) {
	outb((cpu) ? P2_CTRL : P1_CTRL, MBUS_CACHE_EN);
}

slave_machine_init()
{
	register	my_cpu;
	extern	int	curr_ipl[];

	my_cpu = cpu_number();
	curr_ipl[my_cpu] = SPLHI;
	if(is_486()) /* set cache on */
		set_cr0(get_cr0() & ~0x60000000);

	outb(P2_VECT, 0x40+13);	/* use int 13 for processor interrupts */
	outb(P2_CTRL, MBUS_INT_DIS|MBUS_FLUSH|MBUS_CACHE_EN);

#if	MP_SLAVE_AS_CLK
	if (my_cpu == hr_clock_cpu)  {  /* start clock thread on last cpu */
              if (kmem_alloc(kernel_map, &high_res_clock, PAGE_SIZE) == KERN_SUCCESS) {
                    *high_res_clock = 0;
                    printf("cpu %d starts high-resolution clock\n", my_cpu);
                    clock_thread_386 (TIGHT_LOOP_ITERATIONS_PER_TICK,
                                      high_res_clock);  /* doesn't return */
              }
              printf ("high-res clock on cpu %d failed -- kmem_alloc returned 0\n", my_cpu);
              printf ("booting cpu %d as a normal slave processor\n", my_cpu);
        }
#endif	MP_SLAVE_AS_CLK

#if	TIME_STAMP
	if (my_cpu == clock_cpu)
		while(1)
			time_stamp++;
#endif	TIME_STAMP
	machine_slot[my_cpu].is_cpu = TRUE;
	machine_slot[my_cpu].running = TRUE;
	machine_slot[my_cpu].cpu_type = CPU_TYPE_I386;
	machine_slot[master_cpu].cpu_subtype = CPU_SUBTYPE_CBUS;
	init_fpu();
	printf("cpu %d active\n", my_cpu);
}


start_other_cpus() {
	register i;
	extern	int	upyet;
	decl_simple_lock_data(extern, start_lock)
	extern pt_entry_t	*kpde;
	extern char slave_boot_code[];
	extern int slave_boot_size;
	extern pstart();


	upyet = 1;
	simple_unlock(&start_lock);

	if (wncpu != 2)
		return;

#if	MP_SLAVE_AS_CLK
	hr_clock_cpu = 1;	/* clock cpu is P2 */
#endif	MP_SLAVE_AS_CLK

	bcopy(slave_boot_code, phystokv(MP_BOOT), slave_boot_size);

	bzero(phystokv(MP_BOOTSTACK+MP_BOOT)-0x400, 0x400);
	*(int *)phystokv(MP_MACH_START+MP_BOOT) = (int)kvtophys(pstart);

	*(u_short *)phystokv(P2_RESET_VECT) = 0;
	*(((u_short *)phystokv(P2_RESET_VECT))+1) = MP_BOOTSEG;

	outb(P2_CTRL, MBUS_INT_DIS|MBUS_CACHE_EN|MBUS_RESET);
	outb(P1_CTRL, MBUS_FLUSH|MBUS_CACHE_EN);
	kpde[0] = 0;
}

/*
 * Find out how many real cpus there are
 */

get_ncpus()
{
	int want_ncpus = wncpu;
	register i;

	if (inb(P2_ID) == MBUS_386 || inb(P2_ID) == MBUS_486)
		return(2);
	return(1);
}

/*
 * Device driver synchronization.
 * The mutual exclusion is not an issue as long as there is
 * a single device handling thread. This is the case in MK
 * right now.
 * But spl() functions are local.
 *
 * On the SystemPro, device interrupts only occur on master (P1).
 * Nevertheless, the current ipl is local, when P2 raises IPL to 6
 * this has no influence on P1. We still need to prevent interrupts
 * on P1. 
 *
 * So we still need some kind of mutual exclusion.
 *
 * For each distinct device unit we use a distinct mp_dev_lock structure.
 * Inside this structure, we register the fact that a thread has locked
 * the device, and the fact that some device operations could not
 * be processed because the device was locked at the time it was requested.
 *
 */

/*
 * Lock a device
 * if no func is specified, wait for any other thread that already locked it.
 * if func is non null and device is already locked, just queue this function.
 * Process pending interrupt/start op if clearing ipl.
 */

boolean_t
mbus_dev_lock(mpq, op)
struct mp_dev_lock *mpq;
{
	int ret = TRUE;

	thread_t self = current_thread();
	simple_lock(&mpq->lock);
	if (mpq->count && mpq->holder != self) { 
		if (op != MP_DEV_WAIT) {
			i_bit_set(op, &mpq->pending_ops);
			ret = FALSE;
		} else {
			while(mpq->holder) {
				simple_unlock(&mpq->lock);
				printf("mbus_dev_lock: busy waiting\n");
				delay(100);
				simple_lock(&mpq->lock);
			}
			mpq->holder = self;
			mpq->count++;
		}
	} else {
		mpq->holder = self;
		mpq->count++;
	}
	simple_unlock(&mpq->lock);
	return(ret);
}

mbus_dev_unlock(mpq)
struct mp_dev_lock *mpq;
{
	simple_lock(&mpq->lock);
	while (mpq->pending_ops) {
		register i;
		for (i=0; i<MP_DEV_OP_MAX; i++)
			if (i_bit(i, &mpq->pending_ops)) {
				i_bit_clear(i, &mpq->pending_ops); 
				simple_unlock(&mpq->lock);
				(*mpq->op[i])(mpq->unit);
				simple_lock(&mpq->lock);
			}
	}
	if (--mpq->count == 0)
		mpq->holder = 0;
	simple_unlock(&mpq->lock);
}

#if	MACH_KDB
kdb_console() {}
#endif	MACH_KDB

#endif	NCPUS > 1
