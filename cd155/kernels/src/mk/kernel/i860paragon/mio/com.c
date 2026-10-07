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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * HISTORY
 * $Log: com.c,v $
 * Revision 0.9  1995/03/14  23:43:16  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 * Revision 0.8  1995/02/07  05:29:35  lenb
 *         if (fscan_secondary_console), poll fscan (12322)
 *
 * files: i860paragon/ cons.c model_dep.c fscan.c fscan.h mio/com.c
 *
 * Revision 0.7  1994/11/18  20:45:02  mtm
 * Copyright additions/changes
 *
 * Revision 0.6  1993/06/30  22:39:30  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.5  1993/06/05  00:44:23  arlin
 * changed hippi_bd_present() to expansion_id()
 *
 * Revision 0.4  1993/05/27  22:11:58  arlin
 * Added Hippi Driver Support, ARD
 *
 * Revision 0.3  1992/10/14  17:22:32  andyp
 * Finally discovered the cause of the lost transmit interrupt:
 * the console routines (before the first device_open()) are
 * using the tty structure and don't have it marked open.
 * If the first open occurs while console output is still
 * draining, it reinitialized the uart and disabled transmit
 * interrupts.  Also added some minor cosmetic tweaks.
 *
 * Revision 0.2  1992/07/13  12:10:26  andyp
 * Avoid making the upcall to the generic routines on a receive
 * interrupt when the tty circular buffer has not yet been
 * initialized.
 *
 * Revision 0.1  92/07/09  16:57:41  andyp
 * Initial checkin of Paragon files.
 * 
 *
 */
/*
 *	Author: Andy Pfiffer, SSD Intel
 *
 *	Liberally based on a snapshot of i386at/com.c of NORMA-MK9
 *	vintage with all of the Mach 2.[56] cruft removed,
 *	and a healthy dose of support for chips/serial_console.c
 *	(Hats off to af@cs.cmu.edu, again!)
 *
 *	Eventually, it may be nice to use the chip as a real
 *	i82510; but for now, abusing it as an i8250 is just fine.
 */

#include <com.h>
#if	NCOM > 0

#if	NCOM > 1
	This driver does not support more than one device
#endif	NCOM > 1

#include <sys/types.h>
#include <sys/time.h>
#include <mach/boolean.h>
#include <device/conf.h>
#include <device/errno.h>
#include <device/tty.h>
#include <device/io_req.h>
#include <chips/busses.h>
#include <chips/serial_defs.h>
#include <i860paragon/expansion.h>
#include <i860paragon/mio/i8250.h>
#include <i860paragon/mio/i8250reg.h>
#include <i860paragon/mio/mio.h>
#include <i860paragon/fscan.h>


/*
 *	On a Paragon MIO card, iOUT2 is wired to the i82586 ethernet chip's
 *	reset line.  It would be rude to hang up the ethernet when hanging
 *	up a modem...
 */
#define	COM_MCR_DEFAULT		(iOUT2)


extern void	outb();
extern unsigned char	inb();

/*
 *	Driver software state
 */
struct com_softc {
	caddr_t		com_regs;	/* i/o space address */
	struct tty	*com_tty;	/* tty back pointer */
	int		com_modem;	/* modem control bits */
	int		com_enables;	/* shadow of enabled interrupts */
	int		com_prevenable;	/* saved enable state after polling */
	boolean_t	com_probed;	/* has probed once */
	boolean_t	com_attached;	/* has attached */
	boolean_t	com_polling;	/* in console polling mode */
	boolean_t	com_softCAR;	/* software carrier */
	u_long		com_cnt_rx;	/* rx interrupt count */
	u_long		com_cnt_tx;	/* tx interrupt count */
	u_long		com_cnt_mod;	/* modem interrupt count */
	u_long		com_cnt_bad;	/* unknown interrupt count */
	u_long		com_cnt_err;	/* error interrupt count */
	u_long		com_cnt_or;	/* overruns */
	u_long		com_cnt_brk;	/* breaks */
	u_long		com_cnt_pe;	/* parity errors */
	u_long		com_cnt_fe;	/* framing errors */
} com_softc_data[NCOM];
typedef struct com_softc *com_softc_t;

/*
 *	For autoconfiguration...
 */
int	com_probe(), com_attach();

static struct bus_device	*com_info[NCOM];
static caddr_t		com_std[NCOM] = { (caddr_t) MIO_SERIAL_START };
struct bus_driver	com_driver = {
	com_probe,		/* is the i82510 installed? */
	0,			/* really a probe for slaves... */
	com_attach,		/* setup driver after the probe */
	0,			/* start transfer */
	com_std,		/* standard device csr addresses */
	"com",			/* name of the device */
	com_info,		/* backpointers to init structs */
	0,			/* name of a controller */
	0,			/* backpointers to init structs */
	0			/* flags */
};

#ifndef	B19200
#define B19200	EXTA
#endif	B19200

#ifndef	B38400
#define	B38400	EXTB
#endif	B38400

/*
 *	Rhetorical question: does anyone actually *use* baud
 *	rates below 300 (aside from 0)?
 */
static u_short	divisor[] = {
	   0,	/*  B0   */
	2304,	/*  B50  */
	1536,	/*  B75  */
	1047,	/*  B110 */
	 857,	/*  B134 */
	 768,	/*  B150 */
	 576,	/*  B200 */
	 384,	/*  B300 */
	 192,	/*  B600 */
	  96,	/* B1200 */
	  64,	/* B1800 */
	  48,	/* B2400 */
	  24,	/* B4800 */
	  12,	/* B9600 */
	   6,	/* EXTA ( 19200) */
	   3,	/* EXTB ( 38400) */
	   2,	/*   ?  ( 57600) */
	   1	/*   ?	(115200) */
};


/*
 *	Return non-zero if the device is present.
 */
com_probe(ignored, device)
	caddr_t 		ignored;
        struct bus_device	*device;
{
	int		comunit = device->unit;
	com_softc_t	com;
	extern int expansion_id(); 

	if ((comunit < 0) || (comunit >= NCOM)) {
		return 0;
	}

	com = &com_softc_data[comunit];
	if (com->com_probed) {
		return 1;
	}

         /* check for a MIO expansion board type */
	 if (expansion_id() != EXP_ID_MIO)
		 return 0;

	/*
	 * XXX	Should poke at the chip to see if it is
	 * XXX	really there...
	 */
	com_reset(com_std[comunit]);

	/*
	 *	The generic console code doesn't call the
	 *	attach routine...more is done here than
	 *	really ought to be...
	 */
	com->com_probed = TRUE;
	com->com_modem = FALSE;
	com->com_polling = FALSE;
	com->com_regs = com_std[comunit];
	com->com_tty = console_tty[comunit];
	com->com_tty->t_addr = com->com_regs;

	return 1;
}


/*
 *	Reset the chip and drain out any driver-confusing sludge.
 */
com_reset(reg)
	caddr_t	reg;
{
	caddr_t	ier, mcr, iir, lsr, rbr, msr;

	ier = INTR_ENAB(reg);
	mcr = MODEM_CTL(reg);
	iir = INTR_ID(reg);
	lsr = LINE_STAT(reg);
	rbr = TXRX(reg);
	msr = MODEM_STAT(reg);

	outb(ier, 0);
	outb(mcr, COM_MCR_DEFAULT);
	while (! (inb(iir) & 1)) {
		inb(lsr);
		inb(rbr);
		inb(msr);
	}
}


/*
 *	Finalize driver installation.
 */
com_attach(device)
	struct bus_device	*device;
{
	com_softc_t	com;

	com = &com_softc_data[device->unit];
	ttychars(com->com_tty);
	com->com_attached = TRUE;
}


/*
 *	Enable/disable polling mode on a line.
 *	Remember the state of enabled interrupts.
 */
com_pollc(unit, on)
	int		unit;
	boolean_t	on;
{
	com_softc_t	com;
	caddr_t		ier;

	com = &com_softc_data[unit];
	ier = INTR_ENAB(com->com_regs);
	if (on) {
		if (com->com_polling == FALSE) {
			com->com_prevenable = com->com_enables;
			com->com_enables = 0;
			outb(ier, com->com_enables);
		}
	} else {
		com->com_enables = com->com_prevenable;
		com->com_prevenable = 0;
		outb(ier, com->com_enables);
	}
	com->com_polling = on;
}


/*
 *	Modem signal state change (eg, carrier has dropped)
 *	XXX (perhaps RTS/CTS flow control?)
 */
com_modemint(com, iid)
	com_softc_t	com;
	unsigned char	iid;
{
	com->com_cnt_mod++;
}


/*
 *	disable transmit interrupts
 */
com_txdis(com)
	com_softc_t	com;
{
	caddr_t	ier;

	ier = INTR_ENAB(com->com_regs);
	com->com_enables &= ~iTX_ENAB;
	outb(ier, com->com_enables);
}


/*
 *	enable transmit interrupts
 */
com_txena(com)
	com_softc_t	com;
{
	caddr_t	ier;

	ier = INTR_ENAB(com->com_regs);
	com->com_enables |= iTX_ENAB;
	outb(ier, com->com_enables);
}


/*
 *	Transmit holding register is empty.
 */
com_txint(com, iid)
	com_softc_t	com;
	unsigned char	iid;
{
	struct tty *tp;
	caddr_t thr;
	int     c;

	com->com_cnt_tx++;

	tp = com->com_tty;
	simple_lock(&tp->t_lock);

	c = cons_simple_tint(0);
	if (c < 0) {
		com_txdis(com);
	} else {
		thr = TXRX(com->com_regs);
		outb(thr, c);
        }

	simple_unlock(&tp->t_lock);
}


/*
 *	Receive holding register has a character.
 */
com_rxint(com, iid)
	com_softc_t	com;
	unsigned char	iid;
{
	caddr_t	rbr;
	int	c;

	com->com_cnt_rx++;

	rbr = TXRX(com->com_regs);
	c = inb(rbr);
	if (com->com_attached) {
		cons_simple_rint(0, 0, c, 0);
	}
}


/*
 *	Parity error, framing error, overrun error.
 */
com_errint(com, iid)
	com_softc_t	com;
	unsigned char	iid;
{
	struct tty	*tp = com->com_tty;
	caddr_t		regs;
	int		err;
	unsigned char	s;

	com->com_cnt_err++;

	err = 0;
	regs = com->com_regs;
	s = inb(LINE_STAT(regs));
	if (s & iFE) {
		com->com_cnt_fe++;
		err |= CONS_ERR_BREAK;
	}
	if (s & iBRKINTR) {
		com->com_cnt_brk++;
		err |= CONS_ERR_BREAK;
	}
	if (s & iPE) {
		com->com_cnt_pe++;
		err |= CONS_ERR_PARITY;
	}
	if (s & iOR) {
		com->com_cnt_or++;
		err |= CONS_ERR_OVERRUN;
	}

	if (err == 0) {
		printf("com err int, s=0x%x\n", s);
	}

	cons_simple_rint(0, 0, inb(TXRX(regs)), err);
}


/*
 *	Unknown (and as yet unsupported) interrupt.
 */
com_unknownint(com, iid)
	com_softc_t	com;
	unsigned char	iid;
{
	com->com_cnt_bad++;
	printf("com unknown interrupt, iid=0x%x\n", iid);
}


/*
 *	Determine the nature of the interrupt and dispatch it.
 */
com_intr(unit)
	int	unit;
{
	com_softc_t	com;
	struct tty	*tp;
	caddr_t		reg, iir;
	unsigned char	iid;

	com = &com_softc_data[unit];
	tp  = com->com_tty;
	reg = com->com_regs;
	iir = INTR_ID(reg);

	/*
	 *	Drain each pending interrupt.
	 */
	while (! ((iid = inb(iir)) & 1)) {
		switch (iid) { 
		case MODi:
			com_modemint(com, iid);
			break;
		case TRAi:
			com_txint(com, iid);
			break;
		case RECi:
			com_rxint(com, iid);
			break;
		case LINi:
			com_errint(com, iid);
			break;
		default:
			com_unknownint(com, iid);
			break;
		}
	}
}


/*
 *	Receive a character via polling.
 *	Optionally return early if no character
 *	is available.
 */
com_getc(unit, line, wait, raw)
	boolean_t	wait;
	boolean_t	raw;
{
	caddr_t		reg, lsr, rbr;
	int		c, s;

	reg = com_std[unit];
	lsr = LINE_STAT(reg);
	rbr = TXRX(reg);

	s = sploff();
	if ((wait == FALSE) && ((inb(lsr) & iDR) == 0)) {
		splon(s);
		return -1;
	}

	while ((inb(lsr) & iDR) == 0)
	{
		extern int fscan_secondary_console;

		/*
		 * If FSCAN is providing a "secondary" console,
		 * then we'll need to allow it to answer while
		 * we're here polling for tty input.
		 */
		if (fscan_secondary_console)
		{
			extern void fscan_provide_state();

			(void) fscan_provide_state();
		}
	}
	c = inb(rbr);
	if (c == '\r') {
		c = '\n';
	}

	splon(s);

	return c;
}


/*
 *	Output a single character via polling.
 *	Wait for the transmit holding register to be empty,
 *	output the character, and wait for it to drain.
 */
com_putc(unit, line, c)
	char	c;
{
	com_softc_t	com;
	caddr_t		reg, lsr, thr, ier;
	int		i, s, txe, max;

	com = &com_softc_data[unit];
	reg = com_std[unit];
	lsr = LINE_STAT(reg);
	thr = TXRX(reg);
	ier = INTR_ENAB(reg);

	s = sploff();
	txe = (com->com_enables & iTX_ENAB) == iTX_ENAB;
	if (txe) {
		com_txdis(com);
	}

	max = 10000;
	for (i = 0; i < max; i++) {
		if (inb(lsr) & iTHRE)
			break;
		delay(1);
	}

	outb(thr, c);

	max = 10000;
	for (i = 0; i < max; i++) {
		if (inb(lsr) & iTHRE)
			break;
		delay(1);
	}

	if (txe) {
		com_txena(com);
	}

	splon(s);
}


/*
 *	Enable transmit interrupts.
 */
com_start(tp)
	struct tty	*tp;
{
	com_softc_t	com;

	com = &com_softc_data[minor(tp->t_dev)];
	if ((com->com_enables & iTX_ENAB) == 0)
		com_txena(com);
}


/*
 *	Configure "line" as per "tp".
 *	(ie, translate tty flags and baud rates to chip-specific bits)
 */
com_param(tp, line)
	struct tty	*tp;
{
	com_softc_t	com;
	int		unit, mode;
	caddr_t		reg, mcr, lcr, bal, bah, ier;

	unit = minor(tp->t_dev);
	com = &com_softc_data[unit];

	reg = tp->t_addr;
	mcr = MODEM_CTL(reg);
	lcr = LINE_CTL(reg);
	bal = BAUD_LSB(reg);
	bah = BAUD_MSB(reg);
	ier = INTR_ENAB(reg);

	if (tp->t_ispeed == B0) {
		tp->t_state |= TS_HUPCLS;
		com->com_modem = 0;
		/* should probably do:
		com_set_modem_state(com);
		*/
		outb(mcr, COM_MCR_DEFAULT);
		return;
	}

	outb(lcr, iDLAB);
#if	0
	outb(bal, divisor[tp->t_ispeed] & 0xff);
	outb(bah, divisor[tp->t_ispeed] >> 8);
#endif	0
	outb(bal, divisor[B19200] & 0xff);
	outb(bah, divisor[B19200] >> 8);

	if (tp->t_flags & (RAW|LITOUT|PASS8))
		mode = i8BITS;
	else
		mode = i7BITS | iPEN;
	if (tp->t_flags & EVENP)
		mode |= iEPS;
	if (tp->t_ispeed == B110)
		/*
		 * 110 baud uses two stop bits -
		 * all other speeds use one
		 */
		mode |= iSTB;

	/*com->com_enables = iRX_ENAB | iERROR_ENAB | iMODEM_ENAB;*/
	/*
	 * disable modem control interrupts for now -- the cable on
	 * my card is the tried and true 3-wire kind. :^)
	 */
	com->com_enables = iRX_ENAB | iERROR_ENAB;
	if ((tp->t_state & TS_BUSY) == TS_BUSY)
		com->com_enables |= iTX_ENAB;
#if	0
	com->com_enables |= iMODEM_ENAB;
#endif	0

mode = i8BITS;
	outb(lcr, mode);
	outb(ier, com->com_enables);
	outb(mcr, iDTR|iRTS|COM_MCR_DEFAULT);
	delay(10000);	/* XXX 50MHz w/ caches is too fast for the chip */

	com->com_tty->t_state |= TS_CARR_ON;
	com->com_modem |= (TM_DTR|TM_RTS);

	/*
	 *	modem control signals (based on com->com_modem)
	 *	should be translated to chip-specific stuff here.
	com_set_modem_state(com);
	 */

	mio_interrupt_enable(MIO_INTENABLE_SERIAL);
}


/*
 *	af's code has a big #if 0 around this routine.
 *	I guess it should call com_set_modem_state() or something
 *	like it.  Perhaps com_set_modem_state() should be implemented
 *	in terms of com_mctl()?
 */
com_mctl(dev, bits, how)
	int	dev, bits, how;
{
}


/*
 *	Raise or lower carrier.
 */
com_softCAR(unit, line, on_or_off)
{
	com_softc_t	com;
	struct tty	*tp;

	com = &com_softc_data[unit];
	com->com_softCAR = on_or_off;
	tp = com->com_tty;
	if (on_or_off) {
		tp->t_state |= TS_CARR_ON;
		/* might want to raise DTR and RTS here */
	} else {
		tp->t_state &= ~TS_CARR_ON;
		/* might want to drop DTR and RTS here */
	}
}


/*
 *	Back-door routines for triggering a reset of the i82586
 *	ethernet chip.  It's reset line is wired up to iOUT2.
 */
com_drop_out2(unit)
	int	unit;
{
	com_softc_t	com;
	caddr_t		mcr;

	com = &com_softc_data[unit];
	mcr = MODEM_CTL(com->com_regs);
	outb(mcr, inb(mcr) & ~iOUT2);
}


com_raise_out2(unit)
	int	unit;
{
	com_softc_t	com;
	caddr_t		mcr;

	com = &com_softc_data[unit];
	mcr = MODEM_CTL(com->com_regs);
	outb(mcr, inb(mcr) | iOUT2);
}


/*
 *	Return TRUE if the com device should be used as the
 *	console.
 */
boolean_t com_cons_autoconf()
{
	extern int expansion_id(); 

         /* check for a MIO expansion board type */
	 if (expansion_id() != EXP_ID_MIO)
		return FALSE;
#if	0
	if (com_cons_override())
		return FALSE;
#endif	0

	return TRUE;
}


/*
 *	Return TRUE if the com device should be used as the
 *	console.
 */
boolean_t
com_cons_autoconf_firstnode()
{
	char	*s;
	int	firstnode;
	extern	char *getbootenv();
	char	c;

	firstnode = 0;
	if ((s = getbootenv("BOOT_FIRST_NODE")) != 0) {
		firstnode = atoi(s);
	}
	if ((s = getbootenv("BOOT_CONSOLE")) != 0) {
		while (c = *s++)  {
			switch (c) {

				/*
				 * Special Case: FSCAN on the boot node.
				 * Use the serial driver for primary console
				 * and FSCAN for secondary console.
				 *
				 */
			case 'f':
			case 'F':
				/*
				 * FSCAN_PROTOCOL allows new kernels
				 * to run with the old DS fscan application.
				 */
				if (!getbootint("FSCAN_PROTOCOL", TRUE))
					return FALSE;

				if (node_self() != firstnode)
					return FALSE;
				fscan_secondary_console = 1;
				goto do_com;

			case 'c':
				if (node_self() != firstnode) {
					return FALSE;
				}
				goto do_com;
			case 'C':
				goto do_com;
			}
		}
		return FALSE;
	}

do_com:
	return com_cons_autoconf();
}


#endif	NCOM > 0
