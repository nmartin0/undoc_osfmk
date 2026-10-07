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
 * $Log: pit.c,v $
 * Revision 1.5  1994/11/18  20:33:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:24:50  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:23:30  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:23:20  dleslie
 * First R1_0 release
 *
 * Revision 2.9.4.3  92/04/30  11:50:51  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.9.4.2  92/03/28  10:05:35  jeffreyh
 * 	iPSC386 changes from Intel
 * 	[92/03/17            jeffreyh]
 * 
 * Revision 2.9.4.1  92/02/18  18:49:09  jeffreyh
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.9  91/10/07  17:25:03  af
 * 	tenmicrosec() was all wrong has been expunged, since noone uses
 * 	it.
 * 	[91/09/04            rvb]
 * 
 * Revision 2.8  91/06/19  11:55:29  rvb
 * 	cputypes.h->platforms.h
 * 	[91/06/12  13:45:16  rvb]
 * 
 * Revision 2.7  91/05/14  16:14:40  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/05  17:14:03  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:37:14  mrt]
 * 
 * Revision 2.5  91/01/09  19:25:33  rpd
 * 	Fixed clkstart to reset clock interrupt priority, etc.
 * 	[91/01/09            rpd]
 * 
 * 	Flush dead EXL code.
 * 	[90/11/27  11:38:08  rvb]
 * 
 * Revision 2.3  90/08/27  21:57:57  dbg
 * 	Fix Intel Copyright as per B. Davies authorization.
 * 	[90/08/14            dbg]
 * 	Add Intel copyright.
 * 	[90/01/08            rvb]
 * 	splall/backall -> splon/sploff
 * 	[89/10/20            rvb]
 * 
 * Revision 2.2  90/05/03  15:36:34  dbg
 * 	Converted for pure kernel.
 * 	[90/02/20            dbg]
 * 
 * Revision 2.2  89/09/25  12:32:40  rvb
 * 	File was provided by Intel 9/18/89.
 * 	[89/09/23            rvb]
 * 
 */

/*
  Copyright 1988, 1989 by Intel Corporation, Santa Clara, California.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appears in all
copies and that both the copyright notice and this permission notice
appear in supporting documentation, and that the name of Intel
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <cpus.h>
#include <platforms.h>
#include <kern/cpu_number.h>
#ifdef	MACH_KERNEL
#include <kern/time_out.h>
#else	MACH_KERNEL
#include <sys/param.h>
#include <sys/time.h>
#include <sys/kernel.h>
#endif	MACH_KERNEL
#include <i386/ipl.h>
#include <i386/pit.h>

int pitctl_port  = PITCTL_PORT;		/* For 386/20 Board */
int pitctr0_port = PITCTR0_PORT;	/* For 386/20 Board */
int pitctr1_port = PITCTR1_PORT;	/* For 386/20 Board */
int pitctr2_port = PITCTR2_PORT;	/* For 386/20 Board */
/* We want PIT 0 in square wave mode */

int pit0_mode = PIT_C0|PIT_SQUAREMODE|PIT_READMODE ;

unsigned int clknumb = CLKNUM;		/* interrupt interval for timer 0 */

clkstart()
{
	unsigned int	flags;
	unsigned char	byte;
	int s;

#if	NCPUS > 1 && AT386
	if (cpu_number() != master_cpu)
		return;
#endif	NCPUS > 1 && AT386
#if	iPSC386
	intpri[2] = SPLHI;
#else	iPSC386
	intpri[0] = SPLHI;
#endif	iPSC386
	form_pic_mask();

	s = sploff();         /* disable interrupts */
	/* Since we use only timer 0, we program that.
	 * 8254 Manual specifically says you do not need to program
	 * timers you do not use
	 */
	outb(pitctl_port, pit0_mode);
	clknumb = CLKNUM/hz;
	byte = clknumb;
	outb(pitctr0_port, byte);
	byte = clknumb>>8;
	outb(pitctr0_port, byte); 
	splon(s);         /* restore interrupt state */
}
