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
 
/* #define NOCOM */
/*
 * Copyright 1992 by Intel Corporation,
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
 * $Id: cons.c,v 0.29 1995/04/07 17:21:09 lenb Exp $
 */
#include <cpus.h>

#include <sys/types.h>
#include <mach/boolean.h>
#include <kern/cpu_number.h>
#include <chips/busses.h>
#include <device/tty.h>
#include <device/conf.h>
#include <device/cirbuf.h>
#include <chips/serial_defs.h>
#include <i860paragon/fscan.h>
#include <i860paragon/lbus.h>
#include <i860paragon/led.h>
#include <i860paragon/dp.h>
#include <i860paragon/spl.h>
#include <com.h>
#include <sdbs.h>

/*
 *	andyp@ssd.intel.com
 *
 *	A multicomputer console solution; certainly not the best one.
 *	Not all paragon nodes have a uart for console-like
 *	activity; some nodes might.
 *
 *	Selection of a console is controlled by:
 *	 - the order of entries in the config table
 *	 - whether a com port exists
 *	 - the BOOT_CONSOLE boot string
 *
 *	The BOOT_CONSOLE string contains letters which identify the
 *	set of console possibilities that will be considered.
 *	If the BOOT_CONSOLE string is not present, everything is considered.
 *	The default console is always considered even if not explicitly present.
 *	The letters can be used in combination. They are:
 *
 *		c	Com (serial) port on daughtercard, but only on bootnode.
 *		C	Com (serial) port on daughtercard on all nodes.
 *		m	Multicomputer console.
 *		S	Scan console.
 *		s	Scan console, only when looking for input.
 *		d	Default console (Always enabled; this is a placeholder)
 *		l	LED display while waiting for input
 *		k	Stop in LED display while waiting for input
 */

#define SCAN	MCMSG	/* only compile in SCAN console for MCMSG kernels */

extern	paragon_node_mhz;

#ifndef	B19200
#define	B19200	EXTA
#endif	B19200

/*	XXX The multiple internal cons buffers should
 *	XXX should be collapsed into just one.
 */
#define	CONS_TX_BUF_SIZE	2048	/* internal buffer size */

short	screen_console;		/* XXX bogus symbol from chips/screen_defs.h */

struct cons_ops {
	boolean_t	(*cons_op_autoconf)();	/* "pre-probe" for a device */
	int		(*cons_op_probe)();	/* use this console */
	int		(*cons_op_param)();	/* tty state to/from hardware */
	int		(*cons_op_start)();	/* start I/O */
	int		(*cons_op_putc)();	/* polled output */
	int		(*cons_op_getc)();	/* polled input */
	int		(*cons_op_pollc)();	/* switch to/from polling */
	int		(*cons_op_mctl)();	/* set/get modem state */
	int		(*cons_op_softCAR)();	/* raise/lower soft carrier */
};


#if	NCOM > 0
boolean_t	com_cons_autoconf_firstnode();
int	com_probe(),
	com_param(),
	com_start(),
	com_putc(),
	com_getc(),
	com_pollc(),
	com_mctl(),
	com_softCAR();
#endif	NCOM > 0

#if	NSDBS > 0
boolean_t	sdbs_cons_autoconf_firstnode();
int	sdbs_probe(),
	sdbs_param(),
	sdbs_start(),
	sdbs_putc(),
	sdbs_getc(),
	sdbs_pollc(),
	sdbs_mctl(),
	sdbs_softCAR();
#endif	NSDBS > 0

#if	MCMSG
boolean_t	multicomputer_cons_autoconf();
int	multicomputer_cons_probe(),
	multicomputer_cons_param(),
	multicomputer_cons_start(),
	multicomputer_cons_putc(),
	multicomputer_cons_getc(),
	multicomputer_cons_pollc(),
	multicomputer_cons_mctl(),
	multicomputer_cons_softCAR();
#endif	MCMSG

#if	SCAN
boolean_t	scan_cons_autoconf();
int	scan_cons_probe(),
	scan_cons_param(),
	scan_cons_start(),
	scan_cons_putc(),
	scan_cons_getc(),
	scan_cons_pollc(),
	scan_cons_mctl(),
	scan_cons_softCAR();
#endif	SCAN

boolean_t       fscan_cons_autoconf();
extern int     fscan_cons_probe(),
        fscan_cons_param(),
        fscan_cons_start(),
        fscan_cons_putc(),
        fscan_cons_getc(),
        fscan_cons_pollc(),
        fscan_cons_mctl(),
        fscan_cons_softCAR();

boolean_t	default_cons_true();
int	default_cons_start(),
	default_cons_mctl(),
	default_cons_softCAR(),
	default_cons_getc();

/*
 *	Searched in order
 */
struct cons_ops cons_ops[] = {
#ifndef	NOCOM
#if	NCOM > 0
	/*
	 *	Try the UART on the MIO card.
	 */
	{ com_cons_autoconf_firstnode,
	  com_probe,
	  com_param,
	  com_start,
	  com_putc,
	  com_getc,
	  com_pollc,
	  com_mctl,
	  com_softCAR },
#endif	NCOM > 0
#if	NSDBS > 0
	/*
	 *	Try the UART on the SCSI-16 daughter board.
	 */
	{ sdbs_cons_autoconf_firstnode,
	  sdbs_probe,
	  sdbs_param,
	  sdbs_start,
	  sdbs_putc,
	  sdbs_getc,
	  sdbs_pollc,
	  sdbs_mctl,
	  sdbs_softCAR },
#endif	NSDBS > 0
#endif	NOCOM

#if	MCMSG
	/*
	 *	Send and receive console traffic over the
	 *	the communications net.
	 */
	{ multicomputer_cons_autoconf,
	  multicomputer_cons_probe,
	  multicomputer_cons_param,
	  multicomputer_cons_start,
	  multicomputer_cons_putc,
	  multicomputer_cons_getc,
	  multicomputer_cons_pollc,
	  multicomputer_cons_mctl,
	  multicomputer_cons_softCAR },
#endif	MCMSG

#if	SCAN
	/*
	 *	Send and receive console traffic over scan
	 */
	{ scan_cons_autoconf,
	  scan_cons_probe,
	  scan_cons_param,
	  scan_cons_start,
	  scan_cons_putc,
	  scan_cons_getc,
	  scan_cons_pollc,
	  scan_cons_mctl,
	  scan_cons_softCAR },
#endif	SCAN

        { fscan_cons_autoconf,
          fscan_cons_probe,
          fscan_cons_param,
          fscan_cons_start,
          fscan_cons_putc,
          fscan_cons_getc,
          fscan_cons_pollc,
          fscan_cons_mctl,
          fscan_cons_softCAR },

	/*
	 *	Must be last.  This is the console of last resort.
	 */
	{ default_cons_true,	/* autoconf */
	  default_cons_true,	/* probe */
	  default_cons_true,	/* param */
	  default_cons_start,	/* start */
	  default_cons_true,	/* putc */
	  default_cons_getc,	/* getc */
	  default_cons_true,	/* pollc */
	  default_cons_mctl,	/* mctl */
	  default_cons_softCAR },	/* softCAR */

	{ 0 }
};


/*
 *	Look for a device that would be willing
 *	to accept console traffic.
 */
struct cons_ops *paragon_cons_autoconf()
{
	struct cons_ops *cons;

	for (cons = &cons_ops[0]; cons->cons_op_autoconf; cons++)
		if ((*cons->cons_op_autoconf)() == TRUE)
			return cons;

	return (struct cons_ops *) 0;
}


/*
 *	Initialize the console.  Very little of the
 *	kernel is running at this point.
 */
paragon_cons_init()
{
	struct cons_ops	*cons;

	rcline = 0;

	cons = paragon_cons_autoconf();
	if (cons == 0) {
		/*
		 *	The morally right thing to do is panic, but
		 *	it is probably useless -- the string will never
		 *	be seen by a human.
		 */
		panic("you lose -- default console didn't autoconfigure\n");
	}

	console_probe = cons->cons_op_probe;
	console_param = cons->cons_op_param;
	console_start = cons->cons_op_start;
	console_putc  = cons->cons_op_putc;
	console_getc  = cons->cons_op_getc;
	console_pollc = cons->cons_op_pollc;
	console_mctl  = cons->cons_op_mctl;
	console_softCAR = cons->cons_op_softCAR;
	cons_find(1);
}


#if	MCMSG

/*
 *	Multicomputer console routines.
 *	Console traffic travels over the communications net
 *	and is demultiplexed by a remote node.
 */
boolean_t multicomputer_cons_autoconf()
{
	char	*s;
	extern	char *getbootenv();
	char	c;

	if ((s = getbootenv("BOOT_CONSOLE")) != 0) {
		while (c = *s++)  {
			switch (c) {

			case 'm':
				goto do_mc;
			}
		}
		return FALSE;
	}
do_mc:

	return TRUE;
}


/*
 *	Assume the presence of the network.
 *	Called by the generic console routines.
 */
int multicomputer_cons_probe(xxx, ui)
	struct bus_device *ui;
{
	return 1;
}


/*
 *	There isn't a baud rate entry for a device that
 *	shovels bits at roughly 2,097,152,000 bits/sec.
 *	Nor can we easily throttle it down to 110 baud... :^)
 */
static char multicomputer_cons_tx_buf[CONS_TX_BUF_SIZE];
static struct cirbuf multicomputer_cons_buf;
static int multicomputer_cons_initialized;
static int multicomputer_cons_waiting;

int multicomputer_cons_param(tp, line)
	struct tty	*tp;
{
	extern void	mcmsg_console_rx_enb();

	if (!multicomputer_cons_initialized) {

		/*
		 *	Initialize a circular buffer for transmission.
		 *	I'd like to use cb_alloc() but kalloc()
		 *	hasn't been initialized yet.
		 */

		multicomputer_cons_buf.c_start = multicomputer_cons_tx_buf;
		multicomputer_cons_buf.c_end = multicomputer_cons_tx_buf +
						CONS_TX_BUF_SIZE;
		multicomputer_cons_buf.c_cf = multicomputer_cons_tx_buf;
		multicomputer_cons_buf.c_cl = multicomputer_cons_tx_buf;
		multicomputer_cons_buf.c_cc = 0;

		multicomputer_cons_initialized = 1;
		multicomputer_cons_waiting = 0;
	}

	mcmsg_console_rx_enb();
	/*mcmsg_console_tx_enb();	/* XXX */

	return 0;
}


/*
 *	Drain the tty buffer immediately and place the
 *	characters into our local buffer.
 *	Post a wakeup for the producer of the traffic.
 *	Enable transmit interrupts and allow the
 *	underlying mechanism to make the upcall to
 *	mcmsg_console_tx_int().
 */
multicomputer_cons_start(tp)
	struct tty	*tp;
{
	int	c;
	extern void	mcmsg_console_tx_enb();

	while ((c = getc(&tp->t_outq)) != -1) {
		(void) multicomputer_cons_buffer(c);
	}
	tp->t_state &= ~(TS_BUSY|TS_FLUSH);
	tt_write_wakeup(tp);

	mcmsg_console_tx_enb();
}


/*
 *	Pull a character from the local circular buffer;
 *	return -1 if emtpy.
 */
multicomputer_cons_drain()
{
	return getc(&multicomputer_cons_buf);
}

/*
 *	Buffer up the output until a remote node asks for
 *	the characters.  If full, drop a character from the
 *	front of the buffer.
 */

multicomputer_cons_buffer(c)
{
	int	drop = -1;

	while (putc(c, &multicomputer_cons_buf))
		drop = multicomputer_cons_drain();

	return drop;
}


/*
 *	Push characters out the network by polling.
 */
multicomputer_cons_putc(unit, line, c)
{


	/*
	 *	Buffer the character.
	 */

	(void) multicomputer_cons_buffer(c);

	/*
	 *	If can't transmit enable transmit interrupts.
	 */

	if (!mcmsg_console_tx_rdy()) {
		mcmsg_console_tx_enb();
		return;
	}

	/*
	 *	Transmit a character from the buffer.
	 *	(Might be the one we just put in)
	 */

	mcmsg_console_put(multicomputer_cons_drain());
}


/*
 *	Receive a character from the network by polling.
 *	(also transmit buffered output if possible.)
 *	(roll the transmit buffer out through the led.)
 */
multicomputer_cons_getc(unit, line, wait, raw)
	boolean_t	wait;
	boolean_t	raw;
{
	int	c, bch;
	extern int	sw_refresh_enable;

	if (sw_refresh_enable) {
		sw_refresh();
	}
	if (!wait && !mcmsg_console_rx_rdy())
		return -1;

	multicomputer_cons_waiting = 1;
	RED_ON(RED_GETC);
	while (!mcmsg_console_rx_rdy()) {
		if (mcmsg_console_tx_rdy()) {
			if ((bch = multicomputer_cons_drain()) != -1) {
				mcmsg_console_put(bch);
			}
		}
		roll_led_out();
		if (sw_refresh_enable) {
			sw_refresh();
		}
	}

	c = mcmsg_console_get();
	if (c == '\r')
		c = '\n';
	multicomputer_cons_waiting = 0;
	RED_OFF(RED_GETC);

	return c;
}

/*
 *	Switch to/from polling mode.
 */
multicomputer_cons_pollc(unit, on)
	boolean_t	on;
{
	if (on) {
		mcmsg_console_rx_dis();
		mcmsg_console_tx_dis();
	} else {
		mcmsg_console_rx_enb();
		mcmsg_console_tx_enb();
	}
}


/*
 *	Set modem control bits.
 */
multicomputer_cons_mctl(dev, bits, how)
	dev_t	dev;
	int	bits, how;
{
	struct tty	*tp;

	/*
	 *	xxx_cons_mctl() is called on open, close,
	 *	get/set status.  Make sure the tty structure
	 *	is intialized.
	 */
	tp  = console_tty[0];
	if ((tp->t_flags & TS_INIT) == 0) {
		ttychars(tp);
	}

	return 0;
}


/*
 *	Raise or lower soft carrier.
 */
multicomputer_cons_softCAR(unit, line, on_or_off)
{
	struct tty	*tp;

	tp = console_tty[unit];
	if (on_or_off) {
		tp->t_state |= TS_CARR_ON;
	} else {
		tp->t_state &= ~TS_CARR_ON;
	}
}


/*
 *	Transmit interrupt
 */
mcmsg_console_tx_int()
{
	int	c;
	int	s = spltty();

	/*
	 *	1. Look for buffered output first.
	 *	2. Look for tty output.
	 *	3. Transmit a character if one was found, otherwise
	 *	   disable transmit interrupts.
	 */
	if ((c = multicomputer_cons_drain()) == -1) {
		if ((c = cons_simple_tint(0)) == -1) {
			mcmsg_console_tx_dis();
		}
	}
	if (c != -1) {
		mcmsg_console_put(c);
	}

	splx(s);
}


/*
 *	Receive interrupt
 */
mcmsg_console_rx_int()
{
	struct tty	*tp;

	/*
	 *	Receive interrupts that arrive too soon during
	 *	bootup cannot be handled.  Ignore them for now.
	 */
#if 0
	tp  = console_tty[0];
	if ((tp->t_flags & TS_INIT) == 0) {
		(void) mcmsg_console_get();
		return;
	}
#endif

	cons_simple_rint(0, 0, mcmsg_console_get(), 0);
}
#endif	MCMSG

#if	SCAN

/*
 *	Scan string console routines.
 */

int	scan_console_always;
int	scan_console_rx_enable;
int	scan_console_tx_enable;

char scan_buffer[CONS_TX_BUF_SIZE];
struct cirbuf scan_cons_buf;

boolean_t scan_cons_autoconf()
{
	char	*s;
	extern	char *getbootenv();
	char	c;

	scan_console_always = 0;
	if ((s = getbootenv("BOOT_CONSOLE")) != 0) {
		while (c = *s++)  {
			switch (c) {

			case 's':
				scan_console_always++;
				goto do_scan;

			}
		}
		return FALSE;
	}
do_scan:

	return TRUE;
}


/*
 *	Assume the presence of the network.
 *	Called by the generic console routines.
 */
int scan_cons_probe(xxx, ui)
	struct bus_device *ui;
{
	return 1;
}


/*
 *	There isn't a baud rate entry for a device that
 *	shovels bits this slow.
 */
static int scan_cons_initialized;
int scan_cons_no_console = 0;
int	scan_cons_firstnode;

int scan_cons_param(tp, line)
	struct tty	*tp;
{
	char	*s;

	if (!scan_cons_initialized) {

		/*
		 *	Initialize a circular buffer for transmission.
		 */
		scan_cons_buf.c_start = scan_buffer;
		scan_cons_buf.c_end = scan_buffer + CONS_TX_BUF_SIZE;
		scan_cons_buf.c_cf = scan_buffer;
		scan_cons_buf.c_cl = scan_buffer;
		scan_cons_buf.c_cc = 0;
		scan_cons_firstnode = 0;
		if ((s = getbootenv("BOOT_FIRST_NODE")) != 0) {
			scan_cons_firstnode = atoi(s);
		}
		if(scan_cons_firstnode == node_self()) {
			scan_cons_no_console = 1;
		}
		else {
			scan_cons_no_console = 0;
		}
	}
	scan_console_rx_enable = 1;
	scan_cons_initialized = 1;
	return 0;
}

/*
 *	Drain the tty buffer immediately and place the
 *	characters into our local buffer.
 *	Post a wakeup for the producer of the traffic.
 */
scan_cons_start(tp)
	struct tty	*tp;
{
	int	c;
	int	x;

	x = spltty();

	while ((c = getc(&tp->t_outq)) != -1) {
		(void) scan_cons_buffer(c);
	}
	tp->t_state &= ~(TS_BUSY|TS_FLUSH);
	tt_write_wakeup(tp);
	splx(x);
}

/*
 *	Pull a character from the local circular buffer;
 *	return -1 if emtpy.
 */
int
scan_cons_drain()
{
	return(getc(&scan_cons_buf));
}


/*
 *	Buffer up the output until it can be printed out.
 *	If full, drop a character from the
 *	front of the buffer.
 */
int
scan_cons_buffer(c)
{
	int drop = -1;

	while(putc(c,&scan_cons_buf)) {
		drop = scan_cons_drain();
	}
	return drop;
}

/*
 *	Push characters out the network by polling.
 *      if scanio program not running then just buffer the
 *      characters.
 */
scan_cons_putc(unit, line, c)
{
	int error;

	int x = spltty();

	(void) scan_cons_buffer(c);

	/*
	 * put out the next output character if the scanio program is available.
	 * may not be the same as the input character.
	 */

	if( scan_cons_no_console) {
		scan_console_put(scan_cons_drain());
		splx(x);
		return;
	}
	splx(x);
}


/*
 *	Receive a character from the network by polling.
 *	(also transmit buffered output if possible.)
 *	(roll the transmit buffer out through the led.)
 */
scan_cons_getc(unit, line, wait, raw)
	boolean_t	wait;
	boolean_t	raw;
{
	int	c, bch;
	extern int	sw_refresh_enable;
	int	x = spltty();

	if (sw_refresh_enable) {
		sw_refresh();
	}
	if (!wait && !scan_console_rx_rdy()) {
		splx(x);
		return -1;
	}

	RED_ON(RED_GETC);
	while (!scan_console_rx_rdy());

	c = scan_console_get();
	if (c == '\r')
		c = '\n';
	RED_OFF(RED_GETC);

	splx(x);
	return c;
}

/*
 *	Switch to/from polling mode.
 */
scan_cons_pollc(unit, on)
	boolean_t	on;
{

	if (on) {
		scan_console_rx_enable = 0;
		scan_console_tx_enable = 0;
		if(scan_cons_firstnode != node_self()) {
			scan_cons_no_console = 1;
		}
	} else {
		scan_console_rx_enable = 1;
		scan_console_tx_enable = 1;
		if(scan_cons_firstnode != node_self()) {
			scan_cons_no_console = 0;
		}
	}
	return;
}


/*
 *	Set modem control bits.
 */
scan_cons_mctl(dev, bits, how)
	dev_t	dev;
	int	bits, how;
{
	struct tty	*tp;

	/*
	 *	scan_cons_mctl() is called on open, close,
	 *	get/set status.  Make sure the tty structure
	 *	is intialized.
	 */
	tp  = console_tty[0];
	if ((tp->t_flags & TS_INIT) == 0) {
		ttychars(tp);
	}

	return 0;
}


/*
 *	Raise or lower soft carrier.
 */
scan_cons_softCAR(unit, line, on_or_off)
{
	struct tty	*tp;

	tp = console_tty[unit];
	if (on_or_off) {
		tp->t_state |= TS_CARR_ON;
	} else {
		tp->t_state &= ~TS_CARR_ON;
	}
}

/*
 * Base routines
 */

/*
 * raise the scan ready line
 */
scan_signal_set()
{

	node_control_register_clear(LB_NODE_CTRL_INTOUT);
#if 1
	RED_ON(RED_SCAN);
#endif
}

scan_signal_drop()
{

	node_control_register_set(LB_NODE_CTRL_INTOUT);
#if 1
	RED_OFF(RED_SCAN);
#endif
}

/*
 * see if host is ready?
 */
scan_host_active()
{

#if 1
	if ((inl(DP_STATUS_LO) & DP_ISTAT_NET) != 0) {
/*		GREEN_ON(GREEN_SCAN); */
		return 1;
	} else {
/*		GREEN_OFF(GREEN_SCAN); */
		return 0;
	}
#else
	return (inl(DP_STATUS_LO) & DP_ISTAT_NET) != 0;
#endif
}

/*
 * ok to transmit data ?
 */
scan_console_tx_rdy()
{
	int	i;

	if ( (inl(LB_NODE_STATUS) & LB_NODE_STAT_RDSCANRDY) != 0) {
		i = !scan_host_active();
		if (i) {
			return 1;
		}
	}
	return 0;
}

int scan_console_tx_active = 0;

/*
 * if console is ready then put out one character from buffer
 */
scan_console_put(c)
	char	c;
{
	if(!scan_console_tx_rdy()) {
		return(-1);
	}
	return(scan_console_putr(c));
}

/*
 * puts one character out using scan.  This is the real hardware interface.
 * if a scanio program on the DS has not been found yet then possibly time
 * out.  If there is a scanio program then don't timeout.  This
 * may cause a problem if a scanio is run on a particular node and
 * discontinued.  this routine will then spin for ever again.
 */
int
scan_console_putr(c)
char c;
{
	unsigned long t,x;
	int count;

	x = spltty();
	while (!scan_console_tx_rdy());
	outb(LB_DIAG_OUT, c);
	scan_signal_set();
	if(scan_cons_no_console == 0) { /* if not scanio yet */
		count = 0;
		while (!scan_host_active()) {
			if(count++ == 8000) {
				scan_signal_drop();
				splx(x);
				return(-1); /* if time exceeded then timeout */
			}
		}
	}
	else { /* if scanio yet */
/*		while (!scan_host_active()); */
		count=0;
		while (!scan_host_active()) {
#if 0
			if(count++ == 1000000) {
				scan_signal_drop();
				if(scan_cons_firstnode != node_self()) {
					scan_cons_no_console = 0;
				}
				splx(x);
				return(-1); /* if time exceeded then timeout */
			}
#endif 0
		}
	}
	scan_signal_drop();
	while (scan_host_active());
	for (t = 6000; t; t--);  /* wait a few cycles before sending
				    next character */
	splx(x);
	return;
}

/*
 * is scan recieve ready?
 */
scan_console_rx_rdy()
{
	int	i;

	i = scan_host_active();
	return i;
}

/*
 * get a character using scan hardware
 */
scan_console_get()
{
	char	c;
	int	t, x;

	x = spltty();

	/*
	 * flush out the buffer before reading in the read
	 */
	scan_cons_no_console = 1;
 	while ((t = getc(&scan_cons_buf)) != -1) {
			scan_console_putr(t);
	}

	/*
	 * now get the character from scan
	 */
	while (!scan_console_rx_rdy());
	c = inb(LB_DIAG_IN);
	scan_signal_set();
	while (scan_host_active());
	scan_signal_drop();
	for (t = 6000; t; t--);
	splx(x);
	return c;
}

#endif	SCAN

int scan_rx_int = 0;
int scan_tx_int = 0;
/*
 * when a scan interrupt happens come here
 */
int scan_interrupt()
{
#if	SCAN
	int x;

	if (!scan_console_always || !scan_cons_initialized) {
		return(fscan_interrupt());
	}

	x = splnet_noi();

	/*
	 * transmit interrupt? 
	 */
	if(scan_console_tx_active && scan_console_tx_enable) {
		scan_tx_int++;
        	scan_console_tx_int();
	}
	/*
	 * recieve interrupt? 
	 */
	if(scan_console_rx_rdy() && scan_console_rx_enable) {
		scan_rx_int++;
		scan_console_rx_int();
	}
	return(x);

#else	SCAN
	printf("spurious scan interrupt! (not configured)\n");
	return(splnet_noi());
#endif	SCAN
}

/*
 * transmit interrupt is when an io is started and then
 * and needs to be completed here.
 */
scan_console_tx_int()
{
#if	SCAN
	int t;

	while (!scan_host_active());
       	scan_signal_drop();
       	while (scan_host_active());
       	for (t = 6000; t; t--);
	scan_console_tx_active = 0;
	cons_simple_tint(0);
#endif	SCAN
}
	
/*
 * recieve interrupts are handled here
 */
scan_console_rx_int()
{
#if	SCAN

	scan_cons_no_console = 1;

	cons_simple_rint(0, 0, scan_console_get(), 0);

#endif	SCAN
}

/*
 * scan polling interrupt is called once each clock
 * tick.  test for a scanio program if one not found yet
 * and output any bufferred chars, if there is a scanio
 * program running.
 */
scan_intr_poll()
{
#if	SCAN
	int	s, c;
	int error;
	struct tty *tp;

	assert (cpu_number() == master_cpu);

	tp  = console_tty[0];

	s = splnet();
	if (!scan_console_always || !scan_cons_initialized) {
		splx(s);
		return;
	}

	if(scan_cons_no_console) {
		if(scan_console_tx_rdy()) {
	 		while ((c = getc(&scan_cons_buf)) != -1) {
				scan_console_put(c);
				cons_simple_tint(0);
			}
		}
	}
	splx(s);
#endif	SCAN
}

#if	MCMSG | SCAN

/*
 * Display the current contents of the transmit buffer on the red LED
 */

static
ttless(t1, t2)
	unsigned long	t1[2];
	unsigned long	t2[2];
{

	if (t1[1] < t2[1] ||
	    (t1[1] == t2[1] &&
	     t1[0] < t2[0])) {
		return 1;
	}
	return 0;
}

static
tsadd(t1, v, t2)
	unsigned long	t1[2];
	unsigned long	v;
	unsigned long	t2[2];
{

	t2[0] = t1[0] + v;
	if (t2[0] >= v) {
		t2[1] = t1[1];
	} else {
		t2[1] = t1[1] + 1;
	}
}

int	cons_led_flag;

roll_led_out()
{
	static	unsigned long	nexttime[2];
	static	unsigned long	thistime[2];
	static	unsigned long	ctime;
	static	char		*cp;
	static	int		bit;
	char	*s;
	extern	char *getbootenv();
	char	c;
	int	rate;
	int	x;

	if (nexttime[0] == 0 && nexttime[1] == 0) {

		if ((s = getbootenv("BOOT_CONSOLE")) != 0) {
			while (c = *s++)  {
				switch (c) {

				case 'l':
					cons_led_flag |= 1;

				case 'k':
					cons_led_flag |= 2;
				}
			}
		}
		if (cons_led_flag == 0) {
			nexttime[0] = 0xFFFFFFFF;
			nexttime[1] = 0xFFFFFFFF;
			return;
		}
		cp = multicomputer_cons_buf.c_start;
		bit = 0;
		if ((s = getbootenv("BOOT_LED_RATE")) != 0) {
			rate = atoi(s);
		} else {
			rate = 4800;
		}
		ctime = paragon_node_mhz * 1000000 / rate;
	}
	if (nexttime[0] == 0xFFFFFFFF && nexttime[1] == 0xFFFFFFFF) {
		return;
	}

	x = splhigh();
	do {
		mcmsg_hwclock(thistime);
		if (ttless(thistime, nexttime)) {
			continue;
		}
		tsadd(nexttime, ctime, nexttime);
		if (ttless(nexttime, thistime)) {
			tsadd(thistime, ctime, nexttime);
		}

		if (bit == 0) {
			RED_ON(RED_LIGHTPEN);
			bit++;
		} else if (bit < 9) {
			if (*cp & (1 << (bit-1))) {
				RED_OFF(RED_LIGHTPEN);
			} else {
				RED_ON(RED_LIGHTPEN);
			}
			bit++;
		} else {
			RED_OFF(RED_LIGHTPEN);
			tsadd(nexttime, 3*ctime, nexttime);
			bit = 0;
			cp++;
			if (cp == multicomputer_cons_buf.c_end) {
				cp = multicomputer_cons_buf.c_start;
			}
		}

	} while ((cons_led_flag & 2) != 0);
	splx(x);
}

#endif	MCMSG | SCAN


/*
 *	These functions implement the console of last resort.
 *	Simply, it is a five-function bit bucket.  Note that
 *	reads from this console may not do the right thing,
 *	but this will suffice for output.
 */
boolean_t default_cons_true()
{
	return TRUE;
}


/*
 *	Drain the output queue as if it had been sent.
 */
default_cons_start(tp)
	struct tty	*tp;
{
	while (getc(&tp->t_outq) != -1)
		;

	cons_simple_tint(0);
}


/*
 *	I wonder if returning a 0 is the right thing to do...
 */
default_cons_getc(unit, line, wait, raw)
	boolean_t	wait;
	boolean_t	raw;
{
	return 0;
}


default_cons_mctl(dev, bits, how)
	dev_t	dev;
	int	bits, how;
{
	struct tty	*tp;

	tp  = console_tty[0];
	if ((tp->t_flags & TS_INIT) == 0) {
		ttychars(tp);
	}

	return 0;
}


default_cons_softCAR(unit, line, on_or_off)
{
	struct tty	*tp;

	tp = console_tty[unit];
	if (on_or_off) {
		tp->t_state |= TS_CARR_ON;
	} else {
		tp->t_state &= ~TS_CARR_ON;
	}
}

void
scan_input_ast()
{
	printf("oh no you don't!\n");
	assert(0);
}
