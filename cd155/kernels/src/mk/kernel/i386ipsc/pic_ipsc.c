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
 * $Log: pic_ipsc.c,v $
 * Revision 1.5  1994/11/18  20:37:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:29:15  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:35  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:28:07  dleslie
 * First R1_0 release
 *
 * Revision 2.6.2.2  92/03/28  10:07:52  jeffreyh
 * 	Changes needed for generic scsi on iPSC386
 * 	[92/03/20  12:19:14  jeffreyh]
 * 
 * Revision 2.6.2.1  92/01/09  18:44:02  jsb
 * 	Ethernet fixes from Intel: remove fakehardclock hack.
 * 	[92/01/08  15:21:21  jsb]
 * 
 * Revision 2.6  91/08/03  18:18:10  jsb
 * 	Added cnpintr.
 * 	[91/07/17  13:59:08  jsb]
 * 
 * Revision 2.5  91/07/31  17:43:17  dbg
 * 	Changed hardclock for stack_switching support.
 * 	[91/07/29            dbg]
 * 
 * Revision 2.4  91/06/18  20:50:26  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  20:07:09  jsb]
 * 
 * Revision 2.3  91/06/06  17:04:53  jsb
 * 	Hang cnppoll off of hardclock (since cnp doesn't get interrupts).
 * 	[91/05/13  17:09:57  jsb]
 * 
 * Revision 2.2  90/12/04  14:47:32  jsb
 * 	First checkin.
 * 	[90/12/04  10:57:23  jsb]
 * 
 */

#include <sys/types.h>
#include <i386/ipl.h>
#include <i386/pic.h>

#undef SCSI_INTR

#include <cnp.h>
#if	NCNP > 0
extern int cnpintr();
#define	ETHER_INTR	cnpintr
#else
#define	ETHER_INTR	intnull
#endif	NCNP > 0

#include <sd.h>
#if	NSD > 0
extern int	sdintr();
#define SCSI_INTR	sdintr
#endif	NSD > 0

#include <esp.h>
#if	NESP > 0
#include <chips/busses.h>
#define SCSI_INTR	scsi_intr

scsi_intr(unit, spllevel)
	int unit;
	int spllevel;
{
	register struct bus_ctlr *master = bus_master_init;

	(*master->intr)(master->unit, spllevel);
}

#endif	NESP > 0

#ifndef SCSI_INTR
#define SCSI_INTR	intnull
#endif	SCSI_INTR

/* These interrupts are always present */
extern intnull(), hardclock();
extern dcm_send_intr(), dcm_recv_intr(), usmintr();

int (*ivect[NINTR])() = {
	/*00*/		intnull,        /* parity error */
	/*01*/		intnull,        /* bus timeout */
	/*02*/ 		hardclock,      /* timer 0 */
	/*03*/		dcm_send_intr,  /* EOD 0, DCM send */
	/*04*/		intnull,        /* EOD 1 not used */
	/*05*/		intnull,        /* DCM error */
	/*06*/		intnull,        /* grounded */
	/*07*/		intnull,        /* slave interrupt */
	/*08*/		dcm_recv_intr,  /* EOD 2, DCM receive */
	/*09*/		intnull,        /* EOD 3, not used */
	/*10*/		intnull,        /* timer 2 */
	/*11*/		intnull,        /* timer 1 */
	/*12*/		usmintr,        /* serial port */
	/*13*/		ETHER_INTR,	/* cnp ethernet */
	/*14*/		SCSI_INTR,	/* SX/SCSI interrupt */
	/*15*/		intnull,	/* ??? */
};

/*
 * NOTE:
 *	The hardclock interrupt priority must intially be zero
 *	It is set to SPLHI by clkstart()
 */
u_char intpri[NINTR] = {
	/* 00 */	0, 	0,	0,	SPL5,
	/* 04 */	0,	0,	0,	0,
	/* 08 */	SPL5,	0,	0,	0,
	/* 12 */	SPLTTY,	SPLHI,	SPL5,	0,
};
