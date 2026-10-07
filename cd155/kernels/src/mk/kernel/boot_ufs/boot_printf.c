/*
 * 
 * $Copyright
 * Copyright 1991, 1994, 1995 Intel Corporation
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
 * $Log: boot_printf.c,v $
 * Revision 1.6  1994/11/18  20:24:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/07/12  19:16:42  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.4.8.1  1994/03/11  23:06:01  andyp
 * Corrected the use of varargs to be more portable.
 *
 * Revision 1.4  1993/06/30  22:16:03  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:13:33  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:13:04  dleslie
 * First R1_0 release
 *
 * Revision 2.8.2.1  92/02/18  18:33:09  jeffreyh
 * 	iPSC changes from Intel. Should be generalized at some point.
 * 	[92/02/18            jeffreyh]
 * 
 * Revision 2.8  91/05/14  15:20:47  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/03/13  16:50:19  rpd
 * 	Should initialize the "count" before a device_read_inband().
 * 	Bug found by laverne@sumitomo.com.
 * 	[91/03/13  15:03:15  af]
 * 
 * Revision 2.6  91/02/05  17:00:25  mrt
 * 	Changed to new copyright
 * 	[91/01/28  14:54:14  mrt]
 * 
 * Revision 2.5  90/10/25  14:41:34  rwd
 * 	Modified boot_gets to allocate arrays from heap, not stack.
 * 	[90/10/23            rpd]
 * 
 * Revision 2.4  90/08/27  21:44:20  dbg
 * 	Pass extra argument to _doprnt.
 * 	[90/08/21            dbg]
 * 
 * Revision 2.3  90/06/02  14:45:13  rpd
 * 	Converted to new IPC and new host port mechanism.
 * 	[90/03/26  21:28:27  rpd]
 * 
 * Revision 2.2  90/01/11  11:40:53  dbg
 * 	Created.
 * 	[89/12/20            dbg]
 * 
 */
/*
 * Printf from bootstrap task - use device interface.
 */
#include <mach/port.h>
#include <mach/mach_user_internal.h>
#include <mach/mach_port_internal.h>
#include <sys/varargs.h>
#include <sys/reboot.h>

#include <kern/host.h>
#include <kern/kalloc.h>

#include <device/device_types.h>

mach_port_t		console_port;
extern mach_port_t	bootstrap_master_device_port;


#if	iPSC386 || iPSC860
mach_port_t	hardcons_port;

void boot_ipsc_cons_init()
{
	(void) device_open(bootstrap_master_device_port,
			   0,
			   "console",
			   &console_port);
}
#endif	iPSC386 || iPSC860


char	*console_device_name = "console";

void
boot_printf_init()
{
#if	iPSC386 || iPSC860
	extern char	*ipsc_remote_console();
	char		*remote_console_name;

	boot_ipsc_cons_init();
	if ((remote_console_name = ipsc_remote_console()) == (char *) 0) {
		return;
	}
	console_device_name = remote_console_name;
	hardcons_port = console_port;
	console_port = (mach_port_t) 0;
#endif	iPSC386 || iPSC860

	(void) device_open(bootstrap_master_device_port,
			   0,
			   console_device_name,
			   &console_port);
}


#define	BOOT_PRINTF_BUFMAX	128
char	boot_printf_buf[BOOT_PRINTF_BUFMAX + 1]; /* extra for '\r\n' */
unsigned int	boot_printf_index = 0;

boot_putchar(c)
	int	c;
{
	boot_printf_buf[boot_printf_index] = c;
	boot_printf_index++;
	if (c == '\n') {
	    boot_printf_buf[boot_printf_index] = '\r';
	    boot_printf_index++;
	}

	if (boot_printf_index >= BOOT_PRINTF_BUFMAX) {
	    int	amt;
	    (void) device_write_inband(console_port, 0, 0,
			boot_printf_buf, boot_printf_index, &amt);
	    boot_printf_index = 0;
	}
}

/*VARARGS1*/
boot_printf(va_alist)
	va_dcl
{
	va_list	listp;
	char *fmt;

	va_start(listp);
	fmt = va_arg(listp, char *);
	_doprnt(fmt, &listp, boot_putchar, 0);
	va_end(listp);

	if (boot_printf_index != 0) {
	    int	amt;
	    (void) device_write_inband(console_port, 0, 0,
			boot_printf_buf, boot_printf_index, &amt);
	    boot_printf_index = 0;
	}
}

boot_gets(str, maxlen)
	char *str;
	int  maxlen;
{
	register char *lp;
	register int c;

	char *inbuf;
	unsigned int count;
	register char *ip;
	char *strmax = str + maxlen - 1; /* allow space for trailing 0 */

	inbuf = (char *) kalloc(IO_INBAND_MAX);
	lp = str;
	for (;;) {
	    count = IO_INBAND_MAX;
	    (void) device_read_inband(console_port, 0, 0,
				      IO_INBAND_MAX, inbuf, &count);
	    for (ip = inbuf; ip < &inbuf[count]; ip++) {
		c = *ip;
		switch (c) {
		    case '\n':
		    case '\r':
			boot_printf("\n");
			*lp++ = 0;
			kfree((vm_offset_t)inbuf, IO_INBAND_MAX);
			return;

		    case '\b':
		    case '#':
		    case '\177':
			if (lp > str) {
			    boot_printf("\b \b");
			    lp--;
			}
			continue;
		    case '@':
		    case 'u'&037:
			lp = str;
			boot_printf("\n\r");
			continue;
		    default:
			if (c >= ' ' && c < '\177') {
			    if (lp < strmax) {
				*lp++ = c;
				boot_printf("%c", c);
			    }
			    else {
				boot_printf("%c", '\007'); /* beep */
			    }
			}
		}
	    }
	}
}

/*VARARGS1*/
boot_panic(va_alist)
	va_dcl
{
	va_list listp;
	char *s;

	boot_printf("panic: ");
	va_start(listp);
	s = va_arg(listp, char *);
	_doprnt(s, &listp, boot_putchar, 0);
	va_end(listp);
	boot_printf("\n");

	/*
	 * XXX - use internal form of host_reboot,
	 * since I am not exporting a 'host_internal'
	 * file yet.
	 */
	(void) host_reboot(realhost.host_priv_self, RB_DEBUGGER);
}
