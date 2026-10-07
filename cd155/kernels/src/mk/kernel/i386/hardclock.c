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
 * HISTORY
 * 24-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Added code to debug lost of clock ticks on MP configurations
 *
 * $Log: hardclock.c,v $
 * Revision 1.5  1994/11/18  20:32:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:23:53  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:22:31  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:22:11  dleslie
 * First R1_0 release
 *
 * Revision 2.8.6.3  92/09/15  17:14:51  jeffreyh
 * 	Added support for profiling of kernel tasks & threads
 * 	For MPs do not propagate clock int to all CPUs at the same time
 * 	[92/07/24            bernadat]
 * 
 * Revision 2.8.6.2  92/04/30  11:50:12  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.8.6.1  92/02/18  18:45:23  jeffreyh
 * 	Added timestamp measurement variables for Corollary
 * 	[91/12/06            bernadat]
 * 
 * 	Changed clock_interrupt interface for profiling
 * 	(Bernard Tabib & Andrei Danes @ gr.osf.org)
 * 	[91/09/16            bernadat]
 * 
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.8  91/07/31  17:36:33  dbg
 * 	New interrupt save area.
 * 	[91/07/30  16:50:59  dbg]
 * 
 * Revision 2.7  91/06/19  11:55:09  rvb
 * 	cputypes.h->platforms.h
 * 	[91/06/12  13:44:49  rvb]
 * 
 * Revision 2.6  91/05/14  16:08:38  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/05/08  12:37:56  dbg
 * 	Include sqt/intctl.h if building for Sequent.
 * 	[91/03/21            dbg]
 * 
 * Revision 2.4  91/02/05  17:12:01  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:34:29  mrt]
 * 
 * Revision 2.3  91/01/08  17:32:02  rpd
 * 	EFL_VM => user_mode
 * 	[90/12/21  10:50:54  rvb]
 * 
 * Revision 2.2  90/05/03  15:27:35  dbg
 * 	Created.
 * 	[90/03/06            dbg]
 * 
 */

/*
 * Clock interrupt.
 */
#include <cpus.h>
#include <time_stamp.h>
#include <kern/cpu_number.h>
#include <platforms.h>
#include <mach_prof.h>

#include <kern/time_out.h>
#include <i386/thread.h>
#include <i386/eflags.h>
#include <kern/assert.h>

#ifdef	SYMMETRY
#include <sqt/intctl.h>
#endif
#if	defined(AT386) || defined(iPSC386)
#include <i386/ipl.h>
#endif

extern void	clock_interrupt();
extern char	return_to_iret[];

#if	TIME_STAMP && AT386 && NCPUS > 1
extern	unsigned time_stamp;
unsigned old_time_stamp, time_stamp_cum, nstamps;
/*
 *	If H/W provides a counter, record number of ticks and cumulated
 *	time stamps to know timestamps rate.
 *	This should go away when ALARMCLOCKS installed
 */

#define time_stamp_stat()					\
	if (my_cpu == 0)					\
	if (!old_time_stamp) {					\
		old_time_stamp = time_stamp;			\
		nstamps = 0;					\
	} else {						\
		nstamps++;					\
		time_stamp_cum = (time_stamp - old_time_stamp);	\
	}
#else	TIME_STAMP && AT386 && NCPUS > 1
#define time_stamp_stat()
#endif	TIME_STAMP && AT386 && NCPUS > 1


#if	MACH_PROF
int	masked_pc[NCPUS];
int	missed_clock[NCPUS];
int	detect_lost_tick = 0;
#endif	MACH_PROF

hardclock(ivect, old_ipl, ret_addr, regs)
	int	ivect;		/* interrupt number */
	int	old_ipl;	/* old interrupt level */
	char *	ret_addr;	/* return address in interrupt handler */
	struct i386_interrupt_state *regs;
				/* saved registers */
{
	int mycpu = cpu_number();
	register unsigned pc;
	register boolean_t usermode, basepri;
	extern char spl_return[]; 


	time_stamp_stat();

	if (ret_addr == return_to_iret) {
		usermode =  (regs->efl & EFL_VM) || ((regs->cs & 0x03) != 0);
		basepri = (old_ipl == SPL0);
		pc = (unsigned)regs->eip;
	} else {
		usermode = basepri = FALSE;
		pc = (unsigned)((struct i386_interrupt_state *)&old_ipl)->eip;
	}

#if	MACH_PROF
	/*
	 * If we were masked against the clock, just memorize pc
	 * and tha fact that the clock interrupt is delayed
	 */
	if (old_ipl >= SPL7) {
		assert(!usermode);
		if (missed_clock[mycpu]++ && detect_lost_tick > 1)
			Debugger();
		masked_pc[mycpu] = pc;
	} else
#endif	MACH_PROF

	clock_interrupt(tick, usermode, basepri, pc);

#if	NCPUS >1 && AT386
	slave_clock();
#endif	NCPUS >1 && AT386
}

#if	MACH_PROF
delayed_clock()
{
	int my_cpu = cpu_number();
	if (missed_clock[my_cpu] > 1 && detect_lost_tick)
		printf("missed %d clock interrupt(s) at %x\n", missed_clock[my_cpu]-1, masked_pc[my_cpu]);
	clock_interrupt(tick, FALSE, FALSE, masked_pc[my_cpu]);
	missed_clock[my_cpu] = 0;
}
#endif	MACH_PROF





