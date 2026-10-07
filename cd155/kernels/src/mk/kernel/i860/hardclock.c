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
 * Clock interrupt.
 */
#include <cpus.h>
#include <mach_prof.h>
#include <mach_kdb.h>

#include <machine/thread.h>
#include <machine/ipl.h>
#include <machine/psl.h>
#include <kern/time_out.h>
#if	PARAGON860
#include <i860paragon/rpm.h>
#include <kern/cpu_number.h>
#endif	PARAGON860	

extern void	clock_interrupt();
extern int clock_hz;

#if     MACH_KDB
extern int db_active;
#endif

hardclock(ivect, old_ipl, regs)
	int	ivect;		/* interrupt number */
	int	old_ipl;	/* old interrupt level */
	struct i860_saved_state *regs;	/* registers */
{
	extern void rpmsoft_idle_tick(int);

#if	iPSC860
	/*
	 *	On the iPSC860, the clock in the i82510 needs
	 *	to be retriggered after every clock interrupt.
	 *	Paragon nodes don't need to do this as the
	 *	clock interrupt comes from an up-counter in
	 *	the DP ASIC.
	 */
	clkstart();
#endif	iPSC860

#if     MACH_KDB
	/*
	 * if 'kdb' is NOT active then process the clock interrupt
	 */
	if (!db_active)
	{
#endif
		clock_interrupt(tick,		/* usec per clock tick */
			USERMODE(regs->psr),	/* user mode */
			old_ipl == SPL0		/* base priority */
#if	MACH_PROF
			,regs->pc);		/* current pc */
#else
			);
#endif
#if	PARAGON860
		rpmsoft_idle_tick(cpu_number());
#endif	PARAGON860
#if	MACH_KDB
        }
#endif
}



