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
 * Copyright 1995 by Intel Corporation,
 * 
 *			  All Rights Reserved
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
 *
 *	Instead of using the TI8374 Octal Latch to shift data
 *	into and out of the GP node, use the INT_TO_NODE and
 *	INT_FROM_NODE lines as in RS232. 
 *
 * $Id: fscan.c,v 1.28 1995/04/07 17:20:51 lenb Exp $
 *
 */
#include <mach_kdb.h>
#include <sys/types.h>
#include <mach/boolean.h>
#include <chips/busses.h>
#include <ddb/db_output.h>
#include <device/tty.h>
#include <device/conf.h>
#include <device/cirbuf.h>
#include <chips/serial_defs.h>
#include <i860/psl.h>		/* PSR_IM */
#include <i860paragon/baton.h>	/* baton_enter()/baton_exit() */
#include <i860paragon/fscan.h>
#include <i860paragon/fscan_defs.h>
#include <i860paragon/lbus.h>
#include <i860paragon/led.h>
#include <i860paragon/dp.h>
#include <i860paragon/mp.h>
#include <i860paragon/spl.h>
#include <kern/ast.h>

/*
 * External variables and routines.
 */
extern paragon_node_mhz;
extern int boot_mk_verbose;
extern unsigned char version[];

extern int getbootint(char *var, int dflt);
extern void delay(unsigned long microsecs);
extern unsigned long inl(unsigned long *address);

/*
 * Static Routines
 */
static void fscan_init(int primary_console);
static unsigned char fscan_read();
static int fscan_putchar(unsigned char ch);
static void fscan_send_next_block();
static void fscan_write(unsigned char *msg, unsigned char len);
static void fscan_push_char(unsigned char ch);
static void fscan_drain(int maxtimeout);
static unsigned char fscan_pop_char();
static void fscan_show_counts();
static void fscan_pulse();

/*
 * Static Data
 */
static struct fscan_softc fscan_softc_data;
static fscan_softc_t fscan = &fscan_softc_data;
static int fscan_protocol=TRUE;

/*
 *	Measure the time is takes to read & write to the node control
 *	register. Also, chain our routines onto the 'real' console.
 *	If 'fscan' is the 'real' console, then don't bother with the
 *	chain.
 */
static void
fscan_init(int primary_console)
{
	int s;
	int count;
	register unsigned long  *port, start, stop;

	simple_lock_init( &fscan->com_lock );

	/*
	 *  Do you want to revert to unreliable protocol for state info?
	 */
	fscan_protocol = getbootint("FSCAN_PROTOCOL", TRUE);

	/*
	 * We need to measure the time it takes to write to the Node
	 * Status Register. We'll take an average over 2500 writes.
	 * This number must be added to the width of the start bit in
	 * order to get an accurate sample rate.
	 */
	count=2500;
	port = ((unsigned long *) DP_EPOCH_LO);

	NCR_ENTER(s);
	start = inl(port);
	while (count > 0)
	{
		IntFromNodeLow_locked();
		IntFromNodeHigh_locked();
		count--;
	}
	stop = inl(port);
	NCR_EXIT(s);

	if (start < stop)
	{
		fscan->com_overhead=(stop-start);
	}
	else
	{
		fscan->com_overhead=stop + ((-1)-start);
	}
	fscan->com_overhead = fscan->com_overhead/2500;	/* Average */

	/*
	 * Initialize fscan software state.
	 */
	fscan->com_cnt_rx=0;
	fscan->com_cnt_tx=0;
	fscan->com_err_ic=0;
	fscan->com_err_sbs=0;
	fscan->com_err_sbe=0;
	fscan->com_err_noresp=0;
	fscan->com_err_hin=0;
	fscan->com_reboot=0;
	fscan->com_state = FSCAN_STATE_RUNNING;
	fscan->com_multiuser=0;
	fscan->com_last_cmd=0; 
	fscan->com_active=FALSE;
	fscan->com_skew=0; 
	fscan->com_probed	= TRUE;
	fscan->com_polling	= FALSE;
	if (primary_console)
		fscan->com_primary	= TRUE;
	else
		fscan->com_primary	= FALSE;

	fscan->com_active	= FALSE;

	IntFromNodeHigh();

	/*
	 * Record pointers to the real console routines
	 */
	fscan->com_pollc = console_pollc;
	console_pollc	= fscan_cons_pollc;
	fscan->com_pulse_width = getbootint("FSCAN_PULSE_WIDTH", PULSE_WIDTH);

	/*
	 * first kernel printfs
	 */
	if (boot_mk_verbose) {
		db_printf("Fscan Overhead = %d\n",fscan->com_overhead);
		db_printf("Fscan pulse width = %d uS.\n",fscan->com_pulse_width);
	}


}

/*
 *	This is the polling entry point for getting characters from
 *	the 'fscan' interface. It's almost always (if not always) called
 *	from the kernel debugger. If we are told to wait until we get
 *	character, then wait. Else, just see if the diag station is
 *	trying to get our attention.
 */
/*ARGSUSED*/
int
fscan_cons_getc(int unit, int line, int wait, int raw)
{
	unsigned char ch;
	int s;

	if (!fscan->com_polling) {
		return(-1);
	}
	if (!fscan->com_reboot)
	{
		s = sploff();
	}

	/*
	 * The first thing we need to do is to send all our
	 * data to the diagnostic station.
	 */
	while (fscan->com_buf_len || fscan->com_reboot)
	{
		if (fscan->com_buf_len) {
			IntFromNodeLow();
		}
		while (!INT_TO_NODE_ACTIVE());
		ch = fscan_read();
		if (fscan->com_reboot) {
			continue;
		}

		/*
		 * Just in case a character comes in while we're
		 * cleaning house.
		 */
		if (ch != 0)
		{
			splon(s);
			return(ch);
		}
	}

	if ((wait == FALSE) && !INT_TO_NODE_ACTIVE())
	{
		splon(s);
		return(-1);
	}

	/*
	 * Wait here until INT_TO_NODE becomes active. If we are told not
	 * to wait, no problem. We checked above and the line is active.
	 */
	ch=0;
	while (ch == 0)
	{
		if (fscan->com_buf_len) {
			IntFromNodeLow();
		}
		while (!INT_TO_NODE_ACTIVE());
		ch = fscan_read();
	}
	splon(s);
	return(ch);
}


/*
 * 	Read a character from the scan lines. This routine is called from
 *	interrupt level and will always run on the master processor.
 *
 */
static unsigned char
fscan_read()
{
	register unsigned long  *port, start, stop, width, now;
	register unsigned long  spinout;
	
	int s;
	int count;
	unsigned char ch;
	int z;
	unsigned char msg[BLOCK_SIZE];
	unsigned int sum;

	NCR_ENTER(s);	/* own the Node Control Register at sploff */

	fscan->com_active=TRUE;	/* Actively being used! */

	port = ((unsigned long *) DP_EPOCH_LO);

	count=0;		/* Intialize */
	inl(port);		/* Just access it for now */
	
	/* We're now ready. If our line is already low, then raise the line,
	 * wait a few microseconds, then drop it.
	 */
	if (fscan->com_buf_len > 0)
	{
		IntFromNodeHigh_locked();
		delay(100);
	}
	IntFromNodeLow_locked();

	/* 
	 * Now, let's wait for the start bit.
	 */
	while ((++count < RESPONSE_TIMEOUT) && INT_TO_NODE_ACTIVE());
	if (INT_TO_NODE_ACTIVE())
	{
		fscan->com_err_sbs++;	/* Start bit never started */
		IntFromNodeHigh_locked();
		NCR_EXIT(s);
		return(0);
	}

	/*
	 * We have seen the beginning of the start bit. Store the start
	 * time and drive our INT_FROM_NODE line high. This is used as
	 * a debugging tool and is very useful in finding timing errors.
	 */
	start = inl(port);
	IntFromNodeHigh_locked();

	/*
	 * Wait for the start bit to end.
	 */
	count=0;
	while ((++count < RESPONSE_TIMEOUT) && !INT_TO_NODE_ACTIVE());
	if (!INT_TO_NODE_ACTIVE())
	{
		fscan->com_err_sbe++;	/* Start bit never ended */
		NCR_EXIT(s);
		return(0);
	}

	/*
	 * The start bit has finished. Get the stop time and toggle the
	 * INT_FROM_NODE line, again as a debug aid.
	 */
	stop = inl(port);
	IntFromNodeLow_locked();
	IntFromNodeHigh_locked();

	/*
	 * Calculate the start bit width. Make sure we adjust this number
	 * for the overhead of reading from the Node Status Register.
	 */
	if (start < stop) {
		width=(stop-start);
	}
	else
	{
		width=stop + (0xFFFFFFFF-start);
	}

	/*
	 * Make sure the width is not too big. If it is, then the data we
	 * receive will be garbage and we should throw it away. Make sure
	 * the width is not so large that we end up sleeping for a long
	 * time. The sleeps happen with all interrupts turned off so the
	 * entire system will grind to a halt during this time.
	 */
	if (width > (FSCAN_MAX_WIDTH*paragon_node_mhz))
	{
		width = (FSCAN_MAX_WIDTH*paragon_node_mhz)+1;
	}

	/*
	 * Start sampling the data.
	 */
	z=(CHARBITS-1);
	ch=0;
	stop += (width - fscan->com_overhead + (width/2));
	width += fscan->com_overhead;
	while (z >= 0)
	{
		spinout=0;
		/*
	 	 * Wait here until the delay is over.
		 */
		now = inl(port);
		if (stop < now) {
			if (spinout++ > FSCAN_MAX_SPIN) {
				break;
			}
			while (inl(port) >= now)  /* Wait for the wrap */
			{
				if (spinout++ > FSCAN_MAX_SPIN) {
					break;
				}
			}
		}
		while (inl(port) < stop)
		{
			if (spinout++ > FSCAN_MAX_SPIN) {
				break;
			}
		}

		/*
		 * Timeout is over. Go sample the line.
		 */
		if (!INT_TO_NODE_ACTIVE()) {
			ch |= (1<<z);
		}
		stop+=width + (fscan->com_overhead*2);

		/*
		 * For debug, generate a small pulse. 
		 */
		IntFromNodeLow_locked();
		IntFromNodeHigh_locked();
		z--;
	}

	/*
	 * Wait until the INT_TO_NODE goes inactive. We need to do this to
	 * ensure the write cycle has completed.
	 */
	count=0;
	while ((++count < 8000) && INT_TO_NODE_ACTIVE());
	if (INT_TO_NODE_ACTIVE())
	{
		/* Something has gone wrong with the host. The INT_TO_NODE
		 * should have gone inactive. 
		 */
		fscan->com_err_hin++;
	}

	NCR_EXIT(s);	/* release the NCR */

	/*
	 * If we're only reading 7 bits, then mask the top bit off.
	 */

#if	CHARBITS == 7
	ch &= 0x7F;	/* Mask top bit */
#endif

	/*
	 * If the width was too big, then set the character to 0. 
	 * This will be thrown away later.
	 */
	if (width > (FSCAN_MAX_WIDTH*paragon_node_mhz))
	{
		ch = 0;
	}

	/*
	 * See if a command is in progress. If so, the next command
	 * will contain the actual command to execute.
	 */
	else if (fscan->com_cmd_in_prog)
	{
		fscan->com_cmd_in_prog=0;
		if (fscan->com_last_cmd == FSCAN_ADJUST)
		{
			if (ch <= 60) {
				fscan->com_skew = ch;
			}
			fscan->com_last_cmd = 0;
		}
		else
		{
			fscan->com_last_cmd = ch;
			switch(ch)
			{
				extern int strlen(unsigned char *s);

				case FSCAN_SEND_VERSION: 
					count = strlen(version);
					if (count > FSCAN_MAX_BLOCK) {
						count = FSCAN_MAX_BLOCK; 
					}
					fscan_write(version,
						(unsigned char) count);
					break;

				case FSCAN_SHOW_COUNTS:
					fscan_show_counts();
					break;

				case FSCAN_PULSE:	/* Send pulses */
					fscan_pulse();
					break;

				case FSCAN_ADJUST:	/* Adjust pulses */
					fscan->com_cmd_in_prog=1;
					break;

			}
		}
		ch=0;
	}

	else if ((ch >= FSCAN_LOW_CMD) && (ch <= FSCAN_HIGH_CMD))
	{
		switch(ch)
		{
			case FSCAN_SEND_STATE:	/* Send size of buffer */

			   if(fscan_protocol)
			    {
				msg[COM_BUF_LEN_BYTE] = 
					fscan->com_buf_len & FSCAN_MAX_BLOCK;
				msg[STATE1_BYTE] = fscan->com_state;
				if (fscan->com_multiuser) {
					msg[STATE1_BYTE] |= 
					FSCAN_STATE_MULTIUSER_BIT;
				      
				}

				/* the diag fscan app is paranoid and want to
				   use 2 state values to make sure the value
				   it got is good state value */
				msg[STATE2_BYTE] = msg[STATE1_BYTE];
				sum = msg[COM_BUF_LEN_BYTE] + 
				      msg[STATE1_BYTE] + msg[STATE2_BYTE];

				/* truncate to 1 byte */
				sum &= 0x7f;

				/* put the checksum in the first byte */
				msg[CHECKSUM_BYTE] = (unsigned char)sum;
				fscan_write(msg, NUM_STATE_BYTES);
			      }

			      /* use old method */
			      else
			      {
				msg[0] =
					fscan->com_buf_len % FSCAN_MAX_BLOCK;
				msg[1] = fscan->com_state;
				if (fscan->com_multiuser)
					msg[1] |= FSCAN_STATE_MULTIUSER_BIT;

				fscan_write(msg, 2);
			       }
				break;

			case FSCAN_SEND_NEXT:	 /* Send the buffer contents */
				fscan_send_next_block();
				break;

			case FSCAN_COMMAND:	 /* General command */
				fscan->com_cmd_in_prog=1;
				break;

			default:	/* Invalid command */
				fscan->com_err_ic++;
				break;
		}
		fscan->com_last_cmd=ch;
		ch=0;
	}
	else
	{
		if (!fscan->com_polling && fscan->com_primary && 
			!fscan->com_reboot) 
		{
#if	NOT_YET		/* can't put baton in front of ^p processing */
			baton_enter();
#endif	/* NOT_YET */
			cons_simple_rint(0, 0, ch, 0);
#if	NOT_YET
			baton_exit();
#endif	/* NOT_YET */
		}
		fscan->com_cnt_rx++;

	}

	if (fscan->com_buf_len && fscan->com_probed) {
		IntFromNodeLow();
	}

	if (fscan->com_polling) {
		return(ch);
	}
	return(0);
}

/*
 * like other interrupt handlers, fscan_interrupt() is called at
 * sploff, returns at splon, and its return value is previous spl
 * for interrupt() to restore.
 *
 * But since fscan_interrupt() actually runs at sploff(),
 * it simply does a splon before returning the current spl.
 */
int
fscan_interrupt()
{
	(void) fscan_read();
	splon(PSR_IM);
	return (get_spl());
}

/*
 * fscan_provide_state() is a polled entry point
 * for use when we're in the debugger on the boot node,
 * polling the serial port for input.
 * We need this entry point so that fscan can still get the
 * node state for fscan roll call and autoddb.
 */
void
fscan_provide_state()
{
	if(INT_TO_NODE_ACTIVE())
		(void) fscan_read();

	return;
}

/*
 * Send a character to the diag station via polling. Be careful here. This
 * protocol does not provide for asyncronous writes. We need to assert the
 * INT_FROM_NODE line, then wait for the diag station to ack us. Then, go
 * read the character/command which was sent.
 */
/*ARGSUSED*/
int
fscan_cons_putc(int unit, int line, unsigned char ch)
{
#if	NOTYET
	int s = sploff()
	SIMPLE_LOCK(&fscan_lock);
#endif
	fscan_push_char(ch); /* Push it on the stack */
#if	NOTYET
	SIMPLE_UNLOCK(&fscan_lock);
	splon(s);
#endif
	return(0);
}

static int
fscan_putchar(unsigned char ch)
{
	register delay_time, z, count;
	int s;

	/*
	 * Initialize variables 
	 */
	count=0;		/* Intialize */
	NCR_ENTER(s);

	/*
	 * Drive the INT_FROM_NODE line. This tells the diag station we
	 * want to transfer data
	 */
	IntFromNodeLow_locked();

	/*
	 * Wait here until we see the INT_TO_NODE become active. This means
	 * the diag station is saying go ahead..
	 */
	count=0;
	while ((++count < GOAHEAD_TIMEOUT) && !INT_TO_NODE_ACTIVE());
	if (!INT_TO_NODE_ACTIVE())
	{
		fscan->com_err_noresp++;
		IntFromNodeHigh_locked();
		NCR_EXIT(s);
		return(0);
	}

	/*
	 * Initialize variables in preparation for traansmitting the 
	 * characters.
 	 */
	z=CHARBITS-1;

	/*
	 * Send the start pulse followed by a 'dead' pulse. The dead
	 * pulse is needed to give the other end time to calculate the
	 * frequency of the pulses.
	 */
	IntFromNodeHigh_locked();
	delay(fscan->com_pulse_width+fscan->com_skew);
	IntFromNodeLow_locked();
	if (ch & (1<<z)) {	/* Next bit is a one */
		delay(fscan->com_pulse_width-fscan->com_skew);
	}
	else {
		delay(fscan->com_pulse_width);
	}

	/*
	 * Send the character down the wire MSB first.
	 */
	while (z >= 0)
	{
		delay_time = fscan->com_pulse_width;
		if (ch & (1<<z))	/* Bit is a one */
		{
			IntFromNodeHigh_locked();
			if (z == (CHARBITS-1)) {
				delay_time += fscan->com_skew;
			}
			else if ((z < (CHARBITS-1)) && (!(ch & (1<<(z+1))))) {
				delay_time += fscan->com_skew;
			}
		}
		else			/* Bit is a zero */
		{
			IntFromNodeLow_locked();
			if ((z != 0) && (ch & (1<<(z-1)))) {
				delay_time -= fscan->com_skew;
			}
		}
		delay(delay_time);
		z--;
	}
	IntFromNodeHigh_locked();
	fscan->com_cnt_tx++;

	NCR_EXIT(s);
	return(1);
}

/*
 * fscan_send_next_block()
 *
 * Send the next block to the diagnostic station.  If the last ack has not come
 * yet, just send the same buffer again.
 */
static void
fscan_send_next_block()
{
	int z;
	static unsigned char msg[FSCAN_MAX_BLOCK+1];
	static unsigned char len;
	int block_size = BLOCK_SIZE;

	/*
	 * If we're in polling mode, then the block size can be pushed
	 * all the way up to the limit. It should be set to the
	 * FSCAN_BUFFER_SIZE but the buffer size in the driver on the
	 * diag station is probably too small. This needs to change in the
	 * future so the entire 'com_buf_len' can be shovd through
	 */
	if (fscan->com_polling) 
		block_size = FSCAN_MAX_BLOCK;
	for (z=0,len=0; z<block_size; z++) {
		if (fscan->com_buf_len == 0) break;
		msg[z]=fscan_pop_char();
		len++;
	}
	fscan_write(msg, len);
}




/*
 * fscan_write()
 *
 * This routine accepts a message buffer and transmits the buffer to the
 * diagnostic station being careful to follow the correct protocol
 * procedures. 
 */
static void
fscan_write(unsigned char *msg, unsigned char len)
{
	unsigned char z;

	/*
	 * Write the length byte followed by the actual data bytes. Note that
	 * the 'len' byte can be zero. The driver on the other end is setup
	 * to handle this case.
	 */
	if (fscan_putchar(len))
	{
		for (z = 0; z < len; z++)
		{
			if (fscan_putchar(msg[z]) == 0)
				break;
		}
	}

	/*
	 * If we have more to give, then keep the INT_FROM_NODE low.
	 * This is the indication to the host program to keep reading
	 * data.
	 */
	if (fscan->com_buf_len == 0)
		IntFromNodeHigh();
	else
		IntFromNodeLow();	/* Keep it low */
	return;
}

/*
 * fscan_push_char
 *
 * Push a character onto the stack of data waiting to be transmitted.
 */
static void
fscan_push_char(unsigned char ch)
{
	int	x;

	FSCAN_COM_ENTER(x);

	fscan->com_buf[fscan->com_buf_head]=ch;
	if (++fscan->com_buf_head >= FSCAN_BUFFER_SIZE) {
		fscan->com_buf_head=0;
	}

	if (fscan->com_buf_len >= FSCAN_BUFFER_SIZE)
	{
		fscan->com_buf_tail++;
		if (fscan->com_buf_tail >= FSCAN_BUFFER_SIZE) {
			fscan->com_buf_tail=0;
		}
	}
	else
	{
		fscan->com_buf_len++;
	}
	IntFromNodeLow();	/* Let him know we have data */

	FSCAN_COM_EXIT(x);

	/*
	 * See if the output buffer needs to be drained. If so, wait for
	 * the number of outstanding sends drop below the high water mark.
	 */
	fscan_drain(500000);
	return;
}

/*
 * If the buffer length has hit the high water mark, and there is someone
 * connected to us reading data, wait for the buffer to drain.
 */
static void
fscan_drain(int maxtimeout)
{
	int timeout=0;

	while (fscan->com_active &&
		 (fscan->com_buf_len >= FSCAN_HIGH_WATER)) {
		if (INT_TO_NODE_ACTIVE()) {
			(void) fscan_read();
			timeout=0;
		} else {
			timeout++;
		}
		if (timeout > maxtimeout) {
			fscan->com_active=FALSE;
		}
	}
	return;
}


/*
 * fscan_pop_char
 *
 * Pop a character from the outgoing stack.
 */
static unsigned char
fscan_pop_char()
{
	unsigned char ch;
	int x;

	if (fscan->com_buf_len == 0) {
		return(0);
	}

	FSCAN_COM_ENTER(x);
	ch = fscan->com_buf[fscan->com_buf_tail++];
	if (fscan->com_buf_tail >= FSCAN_BUFFER_SIZE) {
		fscan->com_buf_tail=0;
	}
	fscan->com_buf_len--;
	FSCAN_COM_EXIT(x);

	return(ch);
}


boolean_t fscan_cons_autoconf()
{
	char    *s;
	extern  char *getbootenv();
	char    c;
 

	/*
	 * Check the value of BOOT_CONSOLE. If it indicates we should be
	 * using the FSCAN protocol, then return a TRUE.
	 */
	if ((s = getbootenv("BOOT_CONSOLE")) != 0) {
		while (c = *s++)  {
			switch (c) {
 
			case 'f':
			case 'F':
				return(TRUE);
			}
		}
		return(FALSE);
	}
	return(FALSE);
}
 



/*
 *	Called because BOOT_CONSOLE told the autoconf routines that FSCAN is
 *	either primary or secondary console.  Pass this to fscan_init().
 */
/*ARGSUSED*/
int
fscan_cons_probe(caddr_t secondary_console, struct bus_device *device)
{
	if (fscan->com_probed) {
		return(1);
	}

	fscan_init(!(int)secondary_console);
	return(1);
}
 
/*ARGSUSED*/
int
fscan_cons_param(struct tty *tp, int line)
{
	return(0);
}


/*
 *      This is the entry point for non-kernel output. Take characters
 *	from the circular buffer and store them in our local buffer.
 */
int
fscan_cons_start(struct tty *tp)
{
	int     c, x;
	extern	void splx();

	x = spltty(); 

	while ((c = getc(&tp->t_outq)) != -1) {
		fscan_push_char(c);
	}
	tp->t_state &= ~(TS_BUSY|TS_FLUSH);
	tt_write_wakeup(tp);
	splx(x); 

	return(0);
}
 

/*
 *	Turn polling ON or OFF depending upon the setting of 'on'. The only
 *	routines which call this are from the kernel debugger. This means
 *	that when polling is ON, the debugger is active. We can use this to
 *	set the state of this fscan driver.
 */
int
fscan_cons_pollc(int unit, boolean_t on)
{
	if (on)
	{
		fscan->com_polling=1;
		fscan->com_state = FSCAN_STATE_DEBUGGER;
	}
	else
	{
		fscan->com_polling=0;
		fscan->com_state = FSCAN_STATE_RUNNING;
	}

	/*
	 * If we are not the real console, then call the real console.
	 */
	if (!fscan->com_primary) {
		return((*fscan->com_pollc)(unit, on));
	}
	return(0);
}
 


/*
 *      Set modem control bits.
 */
/*ARGSUSED*/
int
fscan_cons_mctl(dev_t dev, int bits, int how)
{
	struct tty      *tp;
	extern int	ttychars(struct tty *tp);
 
	/*
	 *      fscan_cons_mctl() is called on open, close,
	 *      get/set status.  Make sure the tty structure
	 *      is intialized.
	 */
	tp  = console_tty[0];
	if ((tp->t_flags & TS_INIT) == 0) {
		(void) ttychars(tp);
	}
	return(0);
}
 

/*
 *      Raise or lower soft carrier.
 */
/*ARGSUSED*/
int
fscan_cons_softCAR(int unit, int line, int on_or_off)
{
	struct tty      *tp;
 
	tp = console_tty[unit];
	if (on_or_off) {
		tp->t_state |= TS_CARR_ON;
	} else {
		tp->t_state &= ~TS_CARR_ON;
	}
	return(0);
}
 
 
/*
 *	Display the driver counters to the user. Note that this is
 *	only callable via the special fscan command. This means the only
 *	way to see these values is to use 'fscan'.
 */
static void
fscan_show_counts()
{
	unsigned char tmpmsg[10];

	db_printf("\nMACH Kernel FSCAN Driver Communications Statistics.\n");
	db_printf("-----------------------------------------------------\n");
       	db_printf("Received Chars:           %d\n",fscan->com_cnt_rx);
	db_printf("Transmitted Chars:        %d\n",fscan->com_cnt_tx);
	db_printf("Invalid Commands:         %d\n",fscan->com_err_ic);
	db_printf("Start Bit no Start:       %d\n",fscan->com_err_sbs);
	db_printf("Start Bit no End:         %d\n",fscan->com_err_sbe);
	db_printf("Host didn't go inactive:  %d\n",fscan->com_err_hin);
	db_printf("No Response to xmit:      %d\n",fscan->com_err_noresp);
	db_printf("\n");	/* Blank line */

	strcpy(tmpmsg, "ok");
	fscan_write(tmpmsg, 2);
}


/*
 *	This routine is called when the CPU is halted. Its purpose is
 *	to enable polling, then dedicate the rest of the systems life
 *	to servicing requests from the diagnostic station.
 *	The reboot command is sent to the diagnostic station in a
 *	special packet. The first byte in the message will be FF followed
 *	by the number of bytes in the message, followed by the message
 *	itself.
 */
void
fscan_reboot()
{
	fscan->com_polling	= 1;
	fscan->com_primary	= 1;
	fscan->com_reboot	= 1;	/* In the reboot code */
	fscan->com_state	= FSCAN_STATE_REBOOT;
	for (;;)
		(void) fscan_cons_getc(0, 0, TRUE, FALSE);
}

/*
 *	This routine is called by the trap handler when the instructions
 *	"trap r0, r7, r0" is executed. It's purpose is to enable the
 *	watchdog program. In reality, it means the system is not in
 *	multiuser mode and the other processors can be 'pinged' to see
 *	if they are alive.
 */
void
fscan_start_watchdog(int r16)
{
	/*
	 * Assume unit is 0 for the console. Set the multiuser flag
	 * so the fscan program on the diagnostic station will know to
	 * begin talking with the other nodes.
	 */
	if (r16) {
		fscan->com_multiuser=1;
	}
	else {
		fscan->com_multiuser=0;
	}
}


/*
 *	Generate a square wave.
 */
static void
fscan_pulse()
{
	int s;
	int z;

	/*
	 * We have been given the go-ahead. Toggle the line between highs and
	 * lows 5 times. This will give the diag station ample time to
	 * measure the highs and lows.
	 */
	z=0;
	NCR_ENTER(s);
	while (z < 5)
	{
		IntFromNodeLow_locked(); delay(fscan->com_pulse_width);
		IntFromNodeHigh_locked(); delay(fscan->com_pulse_width);
		z++;
	}
	if (fscan->com_buf_len && fscan->com_probed) {
		IntFromNodeLow_locked();
	}
	NCR_EXIT(s);
	return;
}

