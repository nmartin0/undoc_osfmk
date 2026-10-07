/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_console.c,v 1.6 1994/11/18 20:44:04 mtm Exp $
 */

#define	MCMSG_MODULE	MCMSG_MODULE_CONSOLE

/*
 * mcmsg_console.c
 *
 * console message support
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_console.h>
#include <mach/kern_return.h>

/*
 * Client side
 */

syscall_mcmsg_console_open(node, chanp)
	long		node;
	unsigned long	*chanp;
{
	int	i;

	if ((i = mcmsg_call(POST_CONOPEN, node, current_task())) == -1) {
		return KERN_FAILURE;
	}
	*chanp = i;
	return KERN_SUCCESS;
}

syscall_mcmsg_console_close(chan)
	unsigned long	chan;
{

	return mcmsg_call(POST_CONCLOSE, chan, current_task());
}

syscall_mcmsg_console_read(chan, buf)
	unsigned long	chan;
	char		*buf;
{
	int		c;

	c = mcmsg_call(POST_CONREAD, chan, current_task());
	if (c == -1) {
		return KERN_FAILURE;
	}
	if (c == -2) {
		return KERN_NO_ACCESS;
	}
	*buf = c;
	return KERN_SUCCESS;
}

syscall_mcmsg_console_write(chan, c)
	unsigned long	chan;
	char		c;
{

	return mcmsg_call(POST_CONWRITE, chan, c, current_task());
}


/*
 * Server side
 */


mcmsg_console_init()
{

	mcmsg_console_need_int = 0;
	mcmsg_console_ready = 1;
}

mcmsg_console_tx_enb()
{

	mcmsg_console_state.tx_enable = 1;
}

mcmsg_console_tx_dis()
{

	mcmsg_console_state.tx_enable = 0;
}

mcmsg_console_rx_enb()
{

	mcmsg_console_state.rx_enable = 1;
}

mcmsg_console_rx_dis()
{

	mcmsg_console_state.rx_enable = 0;
}

boolean_t
mcmsg_console_tx_rdy()
{
	boolean_t	tx_save;
	boolean_t	rx_save;
	int		reentry_save;

	if (!mcmsg_console_ready) {
		return 0;
	}

	/*
	 * If message processor active, it will take care of incoming messages.
	 */

	if (mcmsg_mp_enable) {
		return mcmsg_console_state.tx_flag;
	}

	/*
	 * Usually when we come into this routine interrupts are disabled
	 * and the caller is looping waiting for this routine to return TRUE.
	 * Since interrupts are off, messages won't be received and the
	 * state won't change. To get around this problem without disturbing
	 * the generic console code, we poll for interrupts here.
	 */

	tx_save = mcmsg_console_state.tx_enable;
	mcmsg_console_state.tx_enable = 0;
	rx_save = mcmsg_console_state.rx_enable;
	mcmsg_console_state.rx_enable = 0;
#if	MACH_ASSERT
	reentry_save = mcmsg_reentry;
	mcmsg_reentry = 0;
#endif	MACH_ASSERT

	nic_interrupt_poll();

	mcmsg_console_state.tx_enable = tx_save;
	mcmsg_console_state.rx_enable = rx_save;
#if	MACH_ASSERT
	mcmsg_reentry = reentry_save;
#endif	MACH_ASSERT

	return mcmsg_console_state.tx_flag;
}

mcmsg_console_rx_rdy()
{
	boolean_t	tx_save;
	boolean_t	rx_save;
	int		reentry_save;

	if (!mcmsg_console_ready) {
		return 0;
	}

	/*
	 * If message processor active, it will take care of incoming messages.
	 */

	if (mcmsg_mp_enable) {
		return mcmsg_console_state.rx_flag;
	}

	/*
	 * Usually when we come into this routine interrupts are disabled
	 * and the caller is looping waiting for this routine to return TRUE.
	 * Since interrupts are off, messages won't be received and the
	 * state won't change. To get around this problem without disturbing
	 * the generic console code, we poll for interrupts here.
	 */

	tx_save = mcmsg_console_state.tx_enable;
	mcmsg_console_state.tx_enable = 0;
	rx_save = mcmsg_console_state.rx_enable;
	mcmsg_console_state.rx_enable = 0;
#if	MACH_ASSERT
	reentry_save = mcmsg_reentry;
	mcmsg_reentry = 0;
#endif	MACH_ASSERT

	nic_interrupt_poll();

	mcmsg_console_state.tx_enable = tx_save;
	mcmsg_console_state.rx_enable = rx_save;
#if	MACH_ASSERT
	mcmsg_reentry = reentry_save;
#endif	MACH_ASSERT

	return mcmsg_console_state.rx_flag;
}

mcmsg_console_put(c)
	char	c;
{
	int	reentry_save;

	if (!mcmsg_console_ready) {
		return;
	}

	mcmsg_console_state.tx_char = c;
	mcmsg_console_state.tx_flag = 0;

	assert((reentry_save = mcmsg_reentry) >= 0);
	assert((mcmsg_reentry = 0) == 0);
	mcmsg_call(POST_CONPUT);
	assert((mcmsg_reentry = reentry_save) >= 0);
}

mcmsg_console_get()
{

	mcmsg_console_state.rx_flag = 0;
	return mcmsg_console_state.rx_char;
}

mcmsg_console_ints()
{

#if	LOOPBACK_CONSOLE
#else	LOOPBACK_CONSOLE
	if (mcmsg_console_state.rx_flag && mcmsg_console_state.rx_enable) {
		mcmsg_console_rx_int();
	}
	if (mcmsg_console_state.tx_flag && mcmsg_console_state.tx_enable) {
		mcmsg_console_tx_int();
	}
#endif	LOOPBACK_CONSOLE
	mcmsg_console_need_int = 0;
}
