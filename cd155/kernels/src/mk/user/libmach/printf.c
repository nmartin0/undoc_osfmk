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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * HISTORY
 * $Log: printf.c,v $
 * Revision 1.6  1995/03/07  22:35:29  berg
 *  Reviewer: yazz@locus.com cfleck@ssd.intel.com
 *  Risk: Low
 *  Benefit or PTS #: 12387
 *  Testing: by inspection
 *  Module(s): doprnt.c printf.c sprintf.c
 *
 * added code to libmach functions doprnt.c printf.c sprintf.c to keep track of
 * the number characters output.
 *
 * Revision 1.5  1994/11/18  21:05:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:04:34  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:58:53  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:04:57  dleslie
 * First R1_0 release
 *
 * Revision 2.4.3.1  92/03/28  10:34:10  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:34:57  jeffreyh]
 * 
 * Revision 2.5  92/02/19  15:10:55  elf
 * 	Created.
 * 	[92/02/11            rpd]
 * 
 */

#include <mach.h>
#include <device/device.h>
#include <varargs.h>

static mach_port_t console_port;

void
printf_init(device_server_port)
	mach_port_t device_server_port;
{
	(void) device_open(device_server_port,
			   0,
			   "console",
			   &console_port);
}

#define	PRINTF_BUFMAX	128

struct printf_state {
	char buf[PRINTF_BUFMAX + 1]; /* extra for '\r\n' */
	unsigned int index;
};

static void
flush(state)
	struct printf_state *state;
{
	int amt;

	(void) device_write_inband(console_port, 0, 0,
				   state->buf, state->index, &amt);
	state->index = 0;
}

static void
putchar(arg, c)
	char *arg;
	int c;
{
	struct printf_state *state = (struct printf_state *) arg;

	if (c == '\n') {
	    state->buf[state->index] = '\r';
	    state->index++;
	}
	state->buf[state->index] = c;
	state->index++;

	if (state->index >= PRINTF_BUFMAX)
	    flush(state);
}

/*
 * Printing (to console)
 */
vprintf(fmt, args)
	char *fmt;
	va_list args;
{
	struct printf_state state;
	int	rv;

	state.index = 0;
	rv = _doprnt(fmt, args, 0, (void (*)()) putchar, (char *) &state);

	if (state.index != 0)
	    flush(&state);
	return (rv);
}

/*VARARGS1*/
printf(fmt, va_alist)
	char *fmt;
	va_dcl
{
	va_list	args;
	int	rv;

	va_start(args);
	rv = vprintf(fmt, args);
	va_end(args);
	return (rv);
}

safe_gets(str, maxlen)
	char *str;
	int  maxlen;
{
	register char *lp;
	register int c;

	char	inbuf[IO_INBAND_MAX];
	unsigned int count;
	register char *ip;
	char *strmax = str + maxlen - 1; /* allow space for trailing 0 */

	lp = str;
	for (;;) {
	    count = IO_INBAND_MAX;
	    (void) device_read_inband(console_port, 0, 0,
				      sizeof(inbuf), inbuf, &count);
	    for (ip = inbuf; ip < &inbuf[count]; ip++) {
		c = *ip;
		switch (c) {
		    case '\n':
		    case '\r':
			printf("\n");
			*lp++ = 0;
			return;

		    case '\b':
		    case '#':
		    case '\177':
			if (lp > str) {
			    printf("\b \b");
			    lp--;
			}
			continue;
		    case '@':
		    case 'u'&037:
			lp = str;
			printf("\n\r");
			continue;
		    default:
			if (c >= ' ' && c < '\177') {
			    if (lp < strmax) {
				*lp++ = c;
				printf("%c", c);
			    }
			    else {
				printf("%c", '\007'); /* beep */
			    }
			}
		}
	    }
	}
}
