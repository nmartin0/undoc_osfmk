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
 * HISTORY
 * $Log: led.c,v $
 * Revision 1.5  1994/11/18  20:37:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:28:58  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:29  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:28:00  dleslie
 * First R1_0 release
 *
 * Revision 2.5.2.1  92/03/28  10:07:50  jeffreyh
 * 	Better light show for iPSC386(andyp)
 * 	[92/03/20  12:18:35  jeffreyh]
 * 
 * Revision 2.5  91/12/10  16:29:55  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:14  jsb]
 * 
 * Revision 2.4  91/06/18  20:50:22  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  20:06:51  jsb]
 * 
 * Revision 2.3  91/06/06  17:04:46  jsb
 * 	Added led_idle, led_active.
 * 	[91/05/13  17:07:21  jsb]
 * 
 * Revision 2.2  90/12/04  14:47:29  jsb
 * 	First checkin.
 * 	[90/12/04  10:57:09  jsb]
 * 
 */
#include <sys/varargs.h>

/* Send data out the LED */

/* Red and Green LED bits */
#define RED    		0x40
#define GREEN    	0x80

/* Port address for the trace port */
#define TRACE_PORT	0x84

#define BAUD_1200   0x1f0   /* best estimate for 1200 baud */

#define START_BIT   1
#define STOP_BIT    0

/* forward declaration */
static delay();

int led_state;

led(color)
int	color;
{
	led_state = color;
	outb(TRACE_PORT, led_state);
}


led_green_on()
{
	int	s;

	s = sploff();
	led(led_state | GREEN);
	splon(s);
}


led_green_off()
{
	int	s;

	s = sploff();
	led(led_state & ~GREEN);
	splon(s);
}


led_idle()
{
	led_green_off();
}


led_active()
{
	led_green_on();
}


static delay(time)
volatile unsigned int time;
{
	while(time--);
}


led_bit(b)
int b;
{
	if (b) {
		led(led_state | RED);
	} else {
		led(led_state & ~RED);
	}
	delay(BAUD_1200);
}


led_char(c)
char	c;
{
	int i, s;

	s = splhi();
	led_bit(START_BIT);
	for (i = 0; i < 8; i++) {
		led_bit(!((c >> i) & 1));
	}
	led_bit(STOP_BIT);
	delay(4 * BAUD_1200);
	splx(s);
}


led_putchar(c)
int	c;
{
	int	s = splhi();

	led_char((char) c);
	if (c == '\n') {
		led_char('\r');
	}

	splx(s);
}


/*VARARGS1*/
led_printf(fmt, va_alist)
	char	*fmt;
	va_dcl
{
	va_list	listp;
	int	s = splhi();

	va_start(listp);
	_doprnt(fmt, &listp, led_putchar, 0);
	va_end(listp);

	splx(s);
}
