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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_flick.c,v 1.13 1995/01/24 22:10:16 lenb Exp $
 */

/*
 * mcmsg_flick.c
 *
 * Flick system call
 */

#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <mach/vm_prot.h>
#include <mach/vm_param.h>
#include <kern/assert.h>
#include <i860/thread.h>
#include <i860/psl.h>
#include <i860paragon/dp.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>

#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <kern/cpu_number.h>

int was_nic_interrupt = 0;
extern int mcmsg_flick_enable;
extern int mcmsg_mp_enable;

/*
 *	Routine:
 *		syscall_mcmsg_flick()
 *
 *	Description:
 *		This system call is for the uni-processor case.
 *
 *		1) Spin until any interrupt occurs.
 *		2) If the interrupt is a nic interrupt, handle it.
 *
 *		This allows us to bypass interrupt overhead when waiting
 *		for an interrupt.
 *
 *	N.B. This works only if you happen to be running on master_cpu.
 */		

syscall_mcmsg_flick()	
{
	register int        s;
	register int        x;
	register unsigned long stat0;

	if (mcmsg_mp_enable || !mcmsg_flick_enable ||
		cpu_number() != master_cpu ) {
			return(0);
	}
	s = sploff();
	assert(mcmsg_reentry++ == 0);
	CHECK_REENTRY("entry flick");
	mcmsg_send_tail_count = 0;
	if (was_nic_interrupt) {
		was_nic_interrupt = 0;
		CHECK_REENTRY("exit1 flick");
		assert(--mcmsg_reentry == 0);
		splon(s);
		return 0;
	}

	/*
	 * Spin waiting for interrupt.
	 */
	{
		int	t;

		assert(t = 10000000);
		while ((get_epsr() & EPSR_INT) == 0) {
			assert(t--);
		}
	}

	/*
	 * Process NIC Interrupts
	 */
	stat0 = inl(DP_STATUS_LO);
	if (stat0 & DP_ISTAT_NIC) {
		was_nic_interrupt = 1;
		x = splmsg_noi();
		splon(s);
		mcmsg_intr();
		CHECK_REENTRY("exit2 flick");
		assert(--mcmsg_reentry == 0);
		splx(x);
	} else {
		CHECK_REENTRY("exit3 flick");
		assert(--mcmsg_reentry == 0);
		splon(s);
	}
	return 0;
}
