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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_console.h,v 1.5 1994/11/18 20:44:05 mtm Exp $
 */

#define LOOPBACK_CONSOLE 0

/*
 * mcmsg_console.c
 *
 * console message support
 */

/*
polling:

	boolean_t mcmsg_console_tx_rdy()

	Returns TRUE when the message passing system's console
	transmit buffer is empty.


	boolean_t mcmsg_console_rx_rdy()

	Returns TRUE when the message passing system's console
	receive buffer is full.


interrupts:

	mcmsg_console_tx_int()

	Upcall made by the message passing system if console transmit
	interrupts are enabled and the transmit buffer is empty.


	mcmsg_console_rx_int()

	Upcall made by the message passing system if console receive
	interrupts are enabled and the receive buffer is full.

control:

	mcmsg_console_tx_enb()

	Enables message passing console transmit interrupts.


	mcmsg_console_tx_dis()

	Disables message passing console transmit interrupts.


	#if	0
	mcmsg_console_rx_enb()

	Enables message passing console receive interrupts.


	void mcmsg_console_rx_dis()

	Disables message passing console receive interrupts.
	#endif	0

I/O:

	mcmsg_console_put(c)
	int c;

	Transmit character c.  It is an error to call mcmsg_console_put()
	when a call to mcmsg_console_tx_rdy() would return FALSE.


	int mcmsg_console_get()

	Receive a character.  It is an error to call mcmsg_console_get()
	when a call to mcmsg_console_rx_rdy() would return FALSE.

*/


/*
 * Client side
 */

#define NUM_CONSOLE_CHAN	4
#define CONSOLE_BUF_SIZE	8

struct console_chan {
	task_t		task;
	unsigned long	node;
	unsigned long	route;
	short		rx_flag;
	short		rx_char;
} mcmsg_console_chan[NUM_CONSOLE_CHAN];

/*
 * Server side
 */

int mcmsg_console_ready;
int mcmsg_console_need_int;


struct {
	boolean_t	tx_enable;
	boolean_t	rx_enable;
	long		tx_chan;
	unsigned long	tx_node;
	unsigned long	tx_route;
	long		tx_id;
	boolean_t	tx_flag;
	long		tx_char;
	boolean_t	rx_flag;
	long		rx_char;
} mcmsg_console_state;
