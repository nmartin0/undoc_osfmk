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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * HISTORY
 * 23-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Moved ddb MP support to i386/db_interface.c as it is now
 *	commonn to all i386 MPs.
 *
 * $Log: mp.c,v $
 * Revision 1.5  1994/11/18  20:36:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:15  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:26:48  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:27:09  dleslie
 * First R1_0 release
 *
 * Revision 2.1.3.3  92/09/15  17:15:38  jeffreyh
 * 	Do not propagate clock int to all CPUs at the same time
 * 	[92/07/24            bernadat]
 * 
 * Revision 2.1.3.2  92/06/24  17:59:32  jeffreyh
 * 	Fix to be able to compile STD+COMPAQ (Single cpu)
 * 	[92/06/09            bernadat]
 * 
 * Revision 2.1.3.1  92/04/30  11:58:19  bernadat
 * 	MP code for AT386 platforms (Corollary, SystemPro)
 * 	[92/04/08            bernadat]
 * 
*/

#include <cpus.h>

#if NCPUS > 1

#include <sys/types.h>
#include <mach/machine.h>
#include <kern/lock.h>
#include <kern/processor.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <i386/db_machdep.h>
#include <ddb/db_run.h>
#include <i386at/mp/mp.h>
#include <i386/setjmp.h>

int	cpu_int_word[NCPUS];

slave_clock()
{
	register cpu;

	for (cpu=cpu_number()+1; cpu<NCPUS; cpu++)
		if ( machine_slot[cpu].running == TRUE) {
			i_bit_set(MP_CLOCK, &cpu_int_word[cpu]);
			cpu_interrupt(cpu);
			return;
		}
}

interrupt_processor(cpu) {
	i_bit_set(MP_TLB_FLUSH, &cpu_int_word[cpu]);
	cpu_interrupt(cpu);
}

init_ast_check() {}

cause_ast_check(processor)
processor_t	processor;
{
	int cpu = processor->slot_num;

	i_bit_set(MP_AST, &cpu_int_word[cpu]);
	cpu_interrupt(cpu);
}

setsoftclock(cpu)
{
	i_bit_set(MP_SOFTCLOCK, &cpu_int_word[cpu]);
	cpu_interrupt(cpu);
}

cpu_start()
{
	printf("cpu_start not implemented\n");
}

cpu_control()
{
	printf("cpu_control not implemented\n");
}

int real_ncpus;
int wncpu = NCPUS;

/*
 * Find out how many cpus will run
 */
 
mp_probe_cpus()
{
	register i;

	/* 
	 * get real number of cpus
	 */

	real_ncpus = get_ncpus();

	/*
	 * Get number of cpus to boot, passed as an optionnal argument
	 * boot: mach [-sah#]	# from 0 to 9 is the number of cpus to boot
	 * will be overwriten by RB_ASKNAME later
	 */

	wncpu = (boothowto  >> RB_SHIFT) & 0xff;
	if (!wncpu)
		wncpu = NCPUS;
	boothowto &= ~(0xff << RB_SHIFT);

	/*
	 * Ignore real number of cpus it if number of requested cpus
	 * is smaller.
	 * Keep it if number of requested cpu is null or larger.
	 */

	if (real_ncpus < wncpu)
		wncpu = real_ncpus;
	for (i=0; i < wncpu; i++)
		machine_slot[i].is_cpu = TRUE;
}

/*
 * invoke kdb on slave processors 
 */

remote_kdb() {
	int	my_cpu = cpu_number();
	register	i;

	for (i = 0; i < NCPUS; i++)
		if ( i != my_cpu && machine_slot[i].running == TRUE) {
			i_bit_set(MP_KDB, &cpu_int_word[i]);
			cpu_interrupt(i);
		}
}

#endif NCPUS > 1




