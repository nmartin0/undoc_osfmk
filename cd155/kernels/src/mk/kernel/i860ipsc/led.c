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
 * Revision 1.4  1994/11/18  20:40:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/07/12  19:19:09  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.2.8.1  1994/03/11  22:58:48  andyp
 * Corrected the use of varargs to be more portable.
 *
 * Revision 1.2  1993/06/30  22:34:56  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/09/22  18:12:09  regnier
 * Initial revision
 *
 * Revision 2.6  91/12/10  16:31:55  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:32  jsb]
 * 
 * Revision 2.5  91/08/28  11:13:01  jsb
 * 	Added spls where necessary.
 * 	[91/08/26  15:37:21  jsb]
 * 
 * Revision 2.4  91/06/18  20:52:32  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  19:02:45  jsb]
 * 
 * Revision 2.3  91/06/17  15:45:48  jsb
 * 	Added led_idle, led_active definitions.
 * 	[91/06/17  10:39:38  jsb]
 * 
 * Revision 2.2  90/12/04  14:50:18  jsb
 * 	First checkin.
 * 	[90/12/03  21:50:15  jsb]
 * 
 */

#include <i860ipsc/nodehw.h>
#include <i860/psl.h>
#include <sys/varargs.h>


#define BAUD32 8889
#define BAUD40 8333

#define SET_CONTROL(x)	outb(CONTROL_PORT, control_shadow |=  (x))
#define CLR_CONTROL(x)	outb(CONTROL_PORT, control_shadow &= ~(x))

extern unsigned long control_shadow;

led_idle()
{
	SET_CONTROL(RED);
	CLR_CONTROL(GREEN);
}

led_active()
{
	CLR_CONTROL(RED);
	SET_CONTROL(GREEN);
}

static void
waitabit(nanos)
unsigned long	nanos;
{
	unsigned long	now, then, ticks;

	ticks = (nanos == BAUD32) ? BAUD32 : BAUD40;
	now = (unsigned long) inb(COUNTER_PORT);
	then = now + ticks;

	/*
	 * impending rollover?
	 */
	if (then < now) {
		while ((unsigned long) inb(COUNTER_PORT) > now);
	}

	while ((unsigned long) inb(COUNTER_PORT) < then);
}


led_char(c)
	char	c;
{
	int		b, i, s;

	if (STATUS_REG & MHZ_40_33) {
		b = BAUD40;
	} else {
		b = BAUD32;
	}

	s = sploff();

	SET_CONTROL(RED);
	waitabit(b);
	for (i = 0; i < 8; i++) {
		if (c & 1) {
			CLR_CONTROL(RED);
		} else {
			SET_CONTROL(RED);
		}
		c >>= 1;
		waitabit(b);
	}
	CLR_CONTROL(RED);
	waitabit(b);
	waitabit(b);

	splon(s);
}


led_debug(msg)
char *msg;
{
	int	s = sploff();

	for (; *msg != '\0'; msg++) 
		led_char(*msg);
	splon(s);
}


led_putchar(c)
	int	c;
{
	int	s = sploff();

	led_char((char) c);
	if (c == '\n') {
		led_char('\r');
	}

	splon(s);
}


/*VARARGS1*/
led_printf(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;
	int	s = sploff();

	va_start(listp);
	fmt = va_arg(listp, char *);
	_doprnt(fmt, &listp, led_putchar, 0);
	va_end(listp);

	splon(s);
}


led_yellow_on()
{
	int	s = sploff();
	SET_CONTROL(YELLOW);
	splon(s);
}

led_yellow_off()
{
	int	s = sploff();
	CLR_CONTROL(YELLOW);
	splon(s);
}

led_green_on()
{
	int	s = sploff();
	SET_CONTROL(GREEN);
	splon(s);
}

led_green_off()
{
	int	s = sploff();
	CLR_CONTROL(GREEN);
	splon(s);
}

led_red_on()
{
	int	s = sploff();
	SET_CONTROL(RED);
	splon(s);
}

led_red_off()
{
	int	s = sploff();
	CLR_CONTROL(RED);
	splon(s);
}
