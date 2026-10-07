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
 * $Log: spl.c,v $
 * Revision 1.3  1994/11/18  20:40:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/30  22:35:01  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/09/22  18:13:30  regnier
 * Initial revision
 *
 * Revision 2.5  91/12/10  16:32:02  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:34  jsb]
 * 
 * Revision 2.4  91/08/28  11:13:04  jsb
 * 	From Intel SSD: overhauled spl implementation.
 * 	[91/08/26  15:34:58  jsb]
 * 
 * Revision 2.3  91/06/18  20:52:36  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  19:03:22  jsb]
 * 
 * Revision 2.2  90/12/04  14:50:24  jsb
 * 	First checkin.
 * 	[90/12/03  21:51:11  jsb]
 * 
 */

#include <i860ipsc/nodehw.h>
#include <i860ipsc/spl.h>
#include <i860/psl.h>

/******************************************************************************
 *
 *  May 10, 1991
 *
 *  Summary:
 *	We've explored a few of the simple options available with
 *	respect to the spl code for the ipsc860.
 *
 *	We covered three plans in the lunchroom, and one more in the
 *	hall.
 *
 *  Breakdown:
 *
 *	Given:
 *
 *	+ a write-only control register w/ 6 bits for interrupt masks
 *	  in bits d58 through d63
 *
 *		d58: hard error (parity error or bus timeout)
 *		d59: EOD in Rx FIFO
 *		d60: dcm Rx
 *		d61: dcm Tx
 *		d62: expansion port
 *		d63: serial chip (82510 w/ clock)
 *
 *	+ a shadow of the control register.
 *
 *	+ a need to map Mach 3.0 spl() calls onto the hardware.
 *
 *	+ an understanding that at any given splx(N), devices that
 *	  interrupt at or below N are disabled, and devices that
 *	  interrupt at N+1 or above are enabled.
 *
 *  Plan Green:
 *
 *	+ construct a table of interrupts to *disable* at each spl() level:
 *
 *		int disable[8] = {
 *							    0,	// 0 //
 *							    0,	// 1 //
 *							    0,	// 2 //
 *			       			          EXP,	// 3 //
 *			                         DCM_TX | EXP,  // 4 //
 *			                DCM_RX | DCM_TX | EXP,	// 5 //
 *			       SERIAL | DCM_RX | DCM_TX | EXP,	// 6 //
 *			HARD | SERIAL | DCM_RX | DCM_TX | EXP	// 7 //
 *		};
 *
 *  Plan Blue:
 *
 *	+ construct a table of interrupts to *enable* at each spl() level:
 *
 *		int enable[8] = {
 *			HARD | SERIAL | DCM_RX | DCM_TX | EXP,	// 0 //
 *			HARD | SERIAL | DCM_RX | DCM_TX | EXP,	// 1 //
 *			HARD | SERIAL | DCM_RX | DCM_TX | EXP,	// 2 //
 *			HARD | SERIAL | DCM_RX | DCM_TX,	// 3 //
 *			HARD | SERIAL | DCM_RX,			// 4 //
 *			HARD | SERIAL,				// 5 //
 *			HARD,					// 6 //
 *			0					// 7 //
 *		};
 *
 *
 *  Plan Red:
 *
 *	+ construct a "configuration" table that is scanned at initialization
 *	  and is used to construct a table as in Plan Green or Plan Blue:
 *
 *		int intertab[8] = {
 *			0,		// 0 //
 *			0,		// 1 //
 *			0,		// 2 //
 *			EXP,		// 3 //
 *			DCM_TX,		// 4 //
 *			DCM_RX,		// 5 //
 *			SERIAL,		// 6 //
 *			HARD		// 7 //
 *		};
 *
 *  Plan Pierce:
 *
 *	+ don't use a table at all.
 *
 *		spl(0) == splon()
 *		spl(1) == sploff()
 *		spl(2) == sploff()
 *		spl(3) == sploff()
 *		spl(4) == sploff()
 *		spl(5) == sploff()
 *		spl(6) == sploff()
 *		spl(7) == sploff()
 *
 *  Observations:
 *
 *	+ it is not clear how much of this code can or will be salvaged for
 *	  the Sigma node.
 *
 *	+ tables provide a certain degree of flexibility.
 *
 *	+ we've thought about this for far too long.
 *
 *****************************************************************************/

extern long control_shadow;
extern int sploff(), splon();
extern int	db_active;

#define led_debug(x)
#define SPL_LIMIT	8

int	soft_pic = 0;
int	current_int_level = 0;
int	dotimein = 0;

/*
 *  The disable_table defines which interrupts at each spl() level should be
 *  turned off.
 */
int	disable_table[SPL_LIMIT] = {
	0,							     /* SPL0 */
	SERIAL_INT_MASK|EOD_IN_INT_MASK|RECV_INT_MASK|SEND_INT_MASK, /* SPL1 */
	SERIAL_INT_MASK|EOD_IN_INT_MASK|RECV_INT_MASK|SEND_INT_MASK, /* SPL2 */
	SERIAL_INT_MASK|EOD_IN_INT_MASK|RECV_INT_MASK|SEND_INT_MASK, /* SPL3 */
	SERIAL_INT_MASK|EOD_IN_INT_MASK|RECV_INT_MASK|SEND_INT_MASK, /* SPL4 */
	SERIAL_INT_MASK|EOD_IN_INT_MASK|RECV_INT_MASK|SEND_INT_MASK, /* SPL5 */
	SERIAL_INT_MASK|EOD_IN_INT_MASK|RECV_INT_MASK|SEND_INT_MASK, /* SPL6 */
	SERIAL_INT_MASK|EOD_IN_INT_MASK|RECV_INT_MASK|SEND_INT_MASK, /* SPL7 */
};


/*
 * Enables a bit in the software similated pic
 */
int
soft_pic_enable(intr_bit)
int	intr_bit;
{
	int	s;

	s = sploff();
	soft_pic |= intr_bit;
	splon(s);
}


/*
 * Disable a bit in the software similated pic
 */
int
soft_pic_disable(intr_bit)
int	intr_bit;
{
	int	s;

	s = sploff();
	soft_pic &= ~intr_bit;
	splon(s);
}


spl0() { return set_spl(SPL0); }
spl1() { return set_spl(SPL1); }
spl2() { return set_spl(SPL2); }
spl3() { return set_spl(SPL3); }
spl4() { return set_spl(SPL4); }
spl5() { return set_spl(SPL5); }
spl6() { return set_spl(SPL6); }
spl7() { return set_spl(SPL7); }

splsoftclock() { return spl1(); }
splnet() { return spl4(); }
splhdw() { return spl4(); }
splbio() { return spl5(); }

int	max_spl = SPL6;
spldcm()	{ return spl5(); }
spltty()	{ return set_spl(max_spl); }
splclock()	{ return set_spl(max_spl); }
splimp()	{ return set_spl(max_spl); }
splvm()		{ return set_spl(max_spl); }
splsched()	{ return set_spl(max_spl); }
splhigh()	{ return set_spl(max_spl); }
splhi()		{ return set_spl(max_spl); }

splclock_noi()	{ int s = set_spl_noi(max_spl); splon(PSR_IM); return s; }
spldcm_noi()	{ int s = set_spl_noi(max_spl); splon(PSR_IM); return s; }
spltty_noi()	{ int s = set_spl_noi(max_spl); splon(PSR_IM); return s; }
splsched_noi()	{ int s = set_spl_noi(max_spl); splon(PSR_IM); return s; }

/*
 * return to the previous ipl and disable interrupts in the psr.
 * called only from intr.c
 */
splx_noi(prev)
int	prev;
{
	(void) sploff();
	(void) set_spl_noi(prev);
}

/*
 * set_spl_noi: set the current interrupt level.
 *		Must be called PSR_IM == 0 in the psr and will preserve
 *		that state.
 */
set_spl_noi(new)
int	new;
{
	int	old;

#if 0
	if (get_psr() & PSR_IM) {
		led_printf("set_spl_noi: IM bit is ON, psr=%x\n", get_psr());
		Debugger("set_spl_noi");
	}
#endif 0

	old = current_int_level;
	current_int_level = new;
	control_shadow = (control_shadow | soft_pic) & ~disable_table[new];
	CONTROL_REG = control_shadow;

	if (new == SPL0 && dotimein && !db_active) {
		dosoftclock();
	}

	return old;
}


dosoftclock()
{
	dotimein = 0;
	splon(PSR_IM);		/* so we can set_spl() by the rules */
	(void) splsoftclock();
	softclock();
	spl0();
	(void) sploff();
}


/*
 * set_spl: Set the current interrupt level to the proper value
 *	    taking into account possible softclock interrupts and
 *	    disabling interrupts when at spl7.
 */
int set_spl(new)
int new;
{
	int	old, s;

	s = sploff();

#if 0
	if ((s & PSR_IM) == 0) {
		led_printf("set_spl: IM bit is OFF, s=%x\n", s);
		Debugger("set_spl");
	}
#endif 0

	old = set_spl_noi(new);

	/*
	 * make sure interrupts are really enabled when
	 * returning to spl0()
	 */
	if (new == SPL0)
		s = PSR_IM;
	splon(s);

	return (old);
}


/*
 *	splx: This routine is used to set the ipl BACK to a level it was at
 *	before spln() was called.
 */
splx(previous_level)
int	previous_level;
{
	if (previous_level < 0 || previous_level > SPLHI) {
		panic("splx(new %x, curr %x): error in spl.c",
			previous_level, get_spl());
	}
	set_spl(previous_level);
}


/*
 *	setsoftclock: arrange for softclock to be called next time
 *	priority level is set to 0.
 */
setsoftclock()
{
	dotimein = 1;
}


get_spl()
{
	return current_int_level;
}


