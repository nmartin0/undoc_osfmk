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
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * 24-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Moved common ddb code to i386/db_interface.c
 *
 * $Log: cbus_debug.c,v $
 * Revision 1.5  1994/11/18  20:25:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:16:38  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:14:11  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:13:46  dleslie
 * First R1_0 release
 *
 * Revision 2.1.9.2  92/04/30  11:45:15  bernadat
 * 	Moved shared CBUS/MBUS code to i386at/mp
 * 	[92/04/13            bernadat]
 * 
 * Revision 2.1.9.1  92/02/18  18:35:51  jeffreyh
 * 	Moved the MP debug code to machine independant (kern/lock_mon.c)
 * 	Renamed some macro
 * 	Added a entry point to print the time stamp rate
 * 	[91/12/06            bernadat]
 * 
 * 	Adapt kdb synchronization for new kdb_kintr interface
 * 	[91/08/22            bernadat]
 * 
 * 	Created
 * 	[91/06/27  05:01:26  bernadat]
 * 
 */

/*
 * Corollary 386 MP
 * Copyright (c) 1990 OSF Research Institute 
 */

/*
 * Copyright 1990 by Open Software Foundation,
 * Grenoble, FRANCE
 *
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OSF or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 *   OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <cpus.h>
#include <mach_kdb.h>
#include <sys/types.h>
#include <mach/i386/vm_types.h>
#include <kern/lock.h>

#if	(NCPUS > 1) && MACH_KDB

extern u_char	cbus_ochar, cbus_ichar;
extern	int	cbus_wait_char;

/*
 * Machine dependant code for kdb
 *
 * Console interrupt only pop on master CPU. When looping in lcok_kdb,
 * master cpu must check for console interrupts and pass characters
 * to ddb on the active cpu.
 * Also Characters can only be printed by master CPU, so
 * when looping in lock_kdb(), master must also be prepared to receive
 * characters to print from the ddb on the active cpu.
 */

kdb_console() {
	int	my_cpu = cpu_number();
	volatile int	*my_word = &cpu_int_word[my_cpu];

	if (!my_cpu) {
		if (i_bit(CBUS_PUT_CHAR, my_word)) {
			volatile u_char c = cbus_ochar;
			i_bit_clear(CBUS_PUT_CHAR, my_word);
			cnputc(c);
		} else if (i_bit(CBUS_GET_CHAR, my_word)) {
			if (cbus_wait_char)
				cbus_ichar = cngetc();
			else
				cbus_ichar = cnmaygetc();
			i_bit_clear(CBUS_GET_CHAR, my_word);
		/* trying to detect break */
#ifndef notdef
		} else if (!cnmaygetc()) { 
#else	notdef
		} else if (com_is_char() && !com_getc(TRUE)) {
#endif notdef
			simple_unlock(&kdb_lock);
			kdb_cpu = my_cpu;
		}
	}
}

#if	(NCPUS > 1) && MACH_KDB


