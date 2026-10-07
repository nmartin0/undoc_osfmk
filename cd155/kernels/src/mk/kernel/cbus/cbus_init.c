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
 * $Log: cbus_init.c,v $
 * Revision 1.5  1994/11/18  20:25:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:16:40  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:14:13  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:13:48  dleslie
 * First R1_0 release
 *
 * Revision 2.1.9.3  92/09/15  17:13:57  jeffreyh
 * 	fixed minor bug preventing STD+COROLLARY single cpu to boot
 * 	[92/07/02            bernadat]
 * 
 * 	Fixes to be able to compile STD+COROLLARY (Single cpu)
 * 	[92/06/09            bernadat]
 * 
 * Revision 2.1.9.2  92/04/30  11:45:18  bernadat
 * 	Moved shared CBUS/MBUS code to i386at/mp
 * 	[92/04/08            bernadat]
 * 
 * 	Numbers of cpus to boot must be detected earlier because
 * 	of RB_ASKNAME << RB_SHIFT (see model_dep.c)
 * 	[92/03/19            bernadat]
 * 
 * Revision 2.1.9.1  92/02/18  18:36:06  jeffreyh
 * 	Renamed cbus_get_win_add Macro
 * 	Adapted to new MI configuration
 * 	[91/09/27            bernadat]
 * 
 * 	Added code to detect number of cpus before vm initialized
 * 	to allocate interrupt stacks earlier.
 * 	High resolution clock initialization (Jimmy Benjamin @ osf.org)
 * 	[91/08/22            bernadat]
 * 
 * 	Created
 * 	[91/06/27  05:01:40  bernadat]
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
#include <mp_slave_as_clk.h>
#include <time_stamp.h>

#include <sys/types.h>

#include <mach/i386/vm_types.h>

#include <mach/boolean.h>
#include <kern/thread.h>
#include <kern/zalloc.h>



#include <kern/lock.h>

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
#include <i386/tss.h>
#include <i386/seg.h>

#include <i386at/mp/boot.h>
#include <cbus/cbus.h>

#include <mach/machine.h>

#include <chips/busses.h>
#include <i386/ipl.h>
#include <i386/seg.h>
#include <chips/busses.h>
#include <i386at/mp/mp.h>

vm_offset_t cbus_startvec;	/* to map slave boot vector		*/
vm_offset_t phys_map();		/* Used to map phys space		*/
int	cbus_ncpus = 1;

extern nulldev();
int	cbus_init();


caddr_t	cbus_std[1] = { 0 };
struct	bus_device *cbus_info[1];
struct	bus_ctlr *cbus_minfo[1];
struct	bus_driver	cbusdriver = {
	cbus_init, nulldev, nulldev, 0, cbus_std, "cbus", cbus_info, "cbus", cbus_minfo, 0};

cbus_init(port, ui)
struct bus_ctlr *ui;
{
	struct mp_conf *mp;
	int i, j;
	int window;

	/* map cbus registers */
	if ((cbus_regs = (char *)phys_map(0xf00000, 0xe0000)) == 0) {
		printf("could not map cbus regs on at bus\n");
	}

#if	NCPUS > 1
	take_ctlr_irq(ui);

	/* map start vect for extra cpus */
	if ((cbus_startvec = phys_map(CBUS_STARTVEC, 16)) == NULL) {
		printf("could not map start vec on cbus\n");
	}
#endif	NCPUS > 1

#ifndef	Z1000
	for (i = 0; i < CBUS_NSLOTS; i++)
		if ((cbus_io_slot[i] = phys_map( CBUS_CIO_OP2AD(i, 0), 0x1000)) == NULL)
			printf("could not map cbus io\n");
#endif	Z1000

	for (i = 0; i < CBUS_NSLOTS; i++)
		cbus_cpu[i] = -1;
	for (i = 0; i < CBUS_NCPUS; i++)
		cbus_slot[i] = -1;

#if	NCPUS > 1
	cbus_cpuarb();
#endif	NCPUS > 1

	cbus_cpu[CBUS_BRIDGE_SLOT] = 0;		/* host processor */
	cbus_slot[0] = CBUS_BRIDGE_SLOT;	/* host processor */

	for (i = 0; i < (CBUS_NCPUS < NCPUS ? CBUS_NCPUS : NCPUS); i++)
		if (cbus_slot[i] != -1)  {
			printf("cpu %d at CBUS slot %d\n", i, cbus_slot[i]);
			machine_slot[i].is_cpu = TRUE;

#ifdef	Z1000
			window = cbus_free_wind++;
			cbus_set_win(window, CBUS_CIO_OP2AD(cbus_slot[i], 0));
			master_io_addr[i] = (vm_offset_t)cbus_get_win_vadd(window);

			if ((slave_io_addr[i] = phys_map( CBUS_CIO_OP2AD(cbus_slot[i], 0), 0x1000)) == NULL)
				printf("could not map cbus regs for slave %d\n", i);
#else	Z1000
			cbus_io_cpu[i] = cbus_io_slot[cbus_slot[i]];
#endif	Z1000
		}
	cbus_clear_intr(0);

}

#if	NCPUS > 1

int	next_cpu;

slave_boot(cpu)
{
	extern char slave_boot_code[];
	extern int slave_boot_size;
	extern pstart(), vstart();
	extern pt_entry_t	*kpde;
	register i;
	int opri;
	extern struct fake_descriptor gdtptr;
	extern struct fake_descriptor gdt[];
	int old_ncpus = cbus_ncpus;
	extern char df_stack[];

	if (cbus_slot[cpu] == -1 || machine_slot[cpu].running == TRUE) {
		printf("Invalid cpu number\n");
		return;
	}
	next_cpu = cpu;
	bcopy(slave_boot_code, phystokv(MP_BOOT), slave_boot_size);
	bzero(phystokv(MP_BOOTSTACK+MP_BOOT)-0x400, 0x400);
	*(int *)phystokv(MP_MACH_START+MP_BOOT) = (int)pstart;
	opri = splhi();
	load_slave_vec();
	start_slave(cpu);
	for (i=0; i <100000; i++);
	splx(opri);
	if (cbus_ncpus != old_ncpus+1)
		printf("failed to start extra cpu\n");
	kpde[0] = 0;
}

/*
 * It is important to note that there is no real CBUS memory at the
 * start-vector address (unless all 64MB exists).  The 16 bytes are
 * held in the cache only.  Therefore, from the time the 16 bytes are
 * stored, until the time the ATM cpu fetches the code, no other cpu
 * in the system can make a memory reference that will cause that 
 * cache entry to be flushed to memory.  This restriction is true
 * whenever an ATM cpu is being started.  The start-vector is always
 * the last memory to be loaded.
 */

u_char bootjmp[] = { 0xea, 0, 0, MP_BOOT >> 20, MP_BOOT >> 12 };
u_char bootjmp2[] = { 0, 0, 0, 0, 0};

load_slave_vec()
{
	u_char *mp = (u_char *)cbus_startvec;
	int n;
	pt_entry_t *pte;

	for (n = 0; n < sizeof(bootjmp); n++ )
		mp[n] = bootjmp[n];

	for (n = 0; n < sizeof(bootjmp); n++ )
		bootjmp2[n] = mp[n];
}

/*
 * write to cbus i/o registers. Must use windows, can only work 
 * from master cpu
 */

start_slave(cpu) {
	cbus_op(cpu, CBUS_CRESET);
}


cbus_cpuarb()
{
	int slot;
	int max;
	int ncpus = 1;

	w_cbus_io(CBUS_ALL_SLOTS, CBUS_SRESET);
	for (max = CBUS_CPUID_MIN; max <= CBUS_CPUID_MAX; max++) {
		w_cbus_io(CBUS_CONTEND_SLOT, CBUS_CONTEND);
		w_cbus_io(CBUS_ALL_SLOTS, CBUS_CONTEND);
		slot = cbus_getstat() & CBUS_STAT_ARB_MASK;
		if (slot == 0) {
			if (ncpus != real_ncpus)
				printf("cbus_cpuarb() found %d cpus instead of %d\n", ncpus, real_ncpus);
			return;
		}
		w_cbus_io(slot, CBUS_SETIDA);
		if ((slot >= CBUS_CPUID_MIN) && (slot < CBUS_CPUID_MAX)) {
			cbus_cpu[slot] = ncpus;
			cbus_slot[ncpus++] = slot;
		}
		slot = cbus_getstat() & CBUS_STAT_ARB_MASK;
	};
	printf("cpu arbitration failed to complete\n");
}


/*
 * Find out how many real cpus there are
 */

get_ncpus() {
	register i;
	register j;
	register u_char *cpt;
	register int *ipt;
	int	n;

	for (i = MB(1) - sizeof(int); i >= 0; i --) {
		ipt = (int *)phystokv(i);
#ifdef	Z1000
		if (*ipt == 0xdeadbeef) {
			cpt = (u_char *)ipt + 0x60;
#else	Z1000
		if (*ipt == 0xfeedbeef) {
			cpt = (u_char *)ipt;
#endif	Z1000
			cpt--;
			n = 0;
			for (j=0; j<8; j++) {
				if (*cpt > 8)
					break;
				if (*cpt)
					n++;
				cpt--;
			}
#ifdef	Z1000
			if (j == 8)
				return(n+1);
#else	Z1000
			if (j == 8 && n)
				return(n);
#endif	Z1000
		}
	}
	printf("could not guess number of cpus, assume 1\n");
	return(1);
}

#endif	NCPUS > 1

cbus_memsz() {
	register i;
	register j;
	register u_char *cpt;
	register int *ipt;
	int	sz;

	for (i = MB(1) - sizeof(int); i >= 0; i --) {
		ipt = (int *)phystokv(i);
		if (*ipt == 0xdeadbeef) {
			cpt = (u_char *)ipt;
			cpt += 4;
			sz = 0;
			for (j=0; j<64; j++) {
				if (*cpt != 0 && *cpt != 1)
					break;
				sz += *cpt++;
			}
			if (j == 64 && sz)
				return(sz);
		}
	}
	printf("could not guess memory size assume 8 MB\n");
	return(8);
}
			
#if	NCPUS > 1

#if	MP_SLAVE_AS_CLK
extern clock_thread_386();
int     *high_res_clock;                 /* high-resolution clock             */
#define TIGHT_LOOP_ITERATIONS_PER_TICK 1 /* on 16MHz i386, 1 tick = 2E-6 sec  */
                                         /* T_L_I_P_T vs time given in clock.s*/
int     hr_clock_cpu;                       /* remembers last cpu started up,    */
                                         /* will use it to update the clock   */
#endif	MP_SLAVE_AS_CLK

unsigned	time_stamp;

#if	TIME_STAMP
int	clock_cpu = -1;
#endif	TIME_STAMP

slave_machine_init()
{
	register	my_cpu;
	extern	int	curr_ipl[];

	my_cpu = cpu_number();
	curr_ipl[my_cpu] = SPLHI;
	if(is_486()) /* set cache on */
		set_cr0(get_cr0() & ~0x60000000);

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

start_other_cpus()
{
	register i;
	extern	int	upyet;
	decl_simple_lock_data(extern, start_lock)

	upyet = 1;
	simple_unlock(&start_lock);
#if	MP_SLAVE_AS_CLK
	hr_clock_cpu = wncpu - 1;
#endif	MP_SLAVE_AS_CLK
	if (!cpu_number())
		for (i=1; i<wncpu; i++)
			if (cbus_slot[i] != -1)
				slave_boot(i);
}

#endif	NCPUS > 1




