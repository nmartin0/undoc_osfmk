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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_flick.c,v 1.7 1994/11/18 20:41:19 mtm Exp $
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

#define	POLL_INTR	0
#define DO_SWTCH	1
#define ALWAYS_SWTCH	0
#define SWTCH_AFTER	0

#if	POLL_INTR
extern int was_interrupt;
#endif	POLL_INTR
int was_switch = 1;

#define SPIN_COUNT 100
int switch_spin;

int n_flick = 0;
int n_intr_y1 = 0;
int n_intr_n1 = 0;
int n_intr_y2 = 0;
int n_intr_n2 = 0;
int n_swtch_y = 0;
int n_swtch_n = 0;

syscall_mcmsg_flick()
{
	struct i860_saved_state regs;

	n_flick++;

#if	ALWAYS_SWTCH
	thread_block();
#endif	ALWAYS_SWTCH

#if	DO_SWTCH
	if (was_switch) {
		switch_spin = SPIN_COUNT;
		was_switch = swtch();
		if (was_switch) {
			n_swtch_y++;
			return;
		}
		n_swtch_n++;
	}
#endif	DO_SWTCH

#if	POLL_INTR

	regs.pc = -1;
	if (was_interrupt) {
		was_interrupt = intr_poll(&regs);
#if	DO_SWTCH
		was_switch = was_interrupt;
#endif	DO_SWTCH
		if (was_interrupt)
			n_intr_y1++;
		else
			n_intr_n1++;
		return;
	} else {
		while (!intr_poll(&regs)) {
			n_intr_n2++;
			continue;
		}
		n_intr_y2++;
#if	DO_SWTCH
		was_switch = 1;
#endif	DO_SWTCH
	}

#else	POLL_INTR

	switch_spin--;
	if (switch_spin == 0) {
		was_switch = 1;
		thread_block();
	}

#endif	POLL_INTR

#if	SWTCH_AFTER
	thread_block();
#endif	SWTCH_AFTER

	return;
}

mcmsg_flick_reset()
{

	was_switch = 1;
}
