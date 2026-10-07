/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * File: machdep.c
 *
 * This contains all the twiddling needed to start/restart vps and perform
 * asynchronous functino calls. This is machine dependent as it mainly
 * involves mucking about with data on stacks.
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: machdep.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:34:05 $";
#endif

/*
 * derived from vp (Virtual Processor) startup for Multimax implementation.
 */

#include <pthread.h>
#include "internal.h"

/*
 * Function:
 *      vp_setup
 *
 * Parameters:
 *      vp - the structure describing the MACH thread being set up
 *
 * Description:
 *      Set up the initial state of a MACH thread so that it will
 *      invoke pthread_body(vp) when it is resumed.
 */

void
vp_setup( register vp_t	vp )
{
	register struct i860_thread_state	*ts;
	struct i860_thread_state		state;

	extern	int pthread_body();

	ts = (struct i860_thread_state *) &state;

	/*
	 * Set up i860 call frame and registers.
	 * One argument (vp) will be on the stack.
	 * pthread_body will do the enter instruction itself.
	 */
	bzero((char *) &state, sizeof(state));

	ts->psr = 0xA0;				/* USER mode ONLY ! */
	ts->fsr =  0x20;			/* floating-point trap enable */
	ts->pc = (int) (int (*)()) pthread_body;/* function to call */
	ts->r16 = (int) vp;			/* argument to function */
	ts->r1 = 0;             		/* fake return address */
	/*
	 * stack MUST be 16-byte aligned
	 */
	ts->sp = ((int)(vp->stackbase + vp->stacksize) & ~ 0xF);

	if (thread_set_state(vp->id, i860_THREAD_STATE,
				   (thread_state_t) &state,
				   i860_THREAD_STATE_COUNT) != KERN_SUCCESS)
		pthread_internal_error("vp_setup");
}


/*
 * Function:
 *      vp_call_setup
 *
 * Parameters:
 *      vp - the structure describing the target vp to make the call
 *
 * Description:
 *      The MACH thread must be in a suspended state when this function
 *      is called. Registers used by the delivery glue code are pushed
 *      onto the stack and these are set to describe the async call.
 *
 *      pc = delivery glue code:
 *		vp_call_deliver( arg, func )
 *		  arg in r16 so glue can just calli address in r17 func(arg)
 *      r16 = arg to function which will be called
 *      r17 = function to call
 *      sp = stack pointer beyond the save area
 */

vp_call_setup( vp_t vp )
{
	thread_state_data_t		state;
	struct i860_thread_state	*statep;
	unsigned int			count;
	extern void			vp_call_deliver();

	statep = (struct i860_thread_state *)state;

	count = THREAD_STATE_MAX;
	if (thread_get_state(vp->id, i860_THREAD_STATE,
			(thread_state_t) &state, &count) != KERN_SUCCESS)
		pthread_internal_error("vp_call_getstate");

	statep->psr	= 0xA0;	/* USER mode ONLY ! */
	statep->pc	= (int)vp_call_deliver;
	statep->r17	= (int)vp->async_func;
	statep->r16	= (int)vp->async_arg;

	if (thread_set_state(vp->id, i860_THREAD_STATE,
				   (thread_state_t) &state,
				   i860_THREAD_STATE_COUNT) != KERN_SUCCESS)
		pthread_internal_error("vp_call_setstate");
}
