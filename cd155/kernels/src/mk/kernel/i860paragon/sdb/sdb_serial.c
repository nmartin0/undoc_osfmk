/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * $Log: sdb_serial.c,v $
 * Revision 1.2.8.1  1995/06/11  18:33:44  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.2  1995/03/14  23:43:57  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	sdb_serial.c
 *	Author:	Jerrie Coffman
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	4/94
 *
 *	SCSI-16 daughter board serial driver
 *
 *	Based on the MIO board com.c driver written by Andy Pfiffer
 */


#include <sdbs.h>
#if	NSDBS > 0

#if	NSDBS > 1
	This driver does not support more than one device
#endif	NSDBS > 1

#include <sys/types.h>
#include <chips/busses.h>
#include <chips/serial_defs.h>
#include <device/tty.h>

#include <i860paragon/fscan.h>
#include <i860paragon/expansion.h>
#include <i860paragon/sdb/sdb.h>
#include <i860paragon/sdb/i8250.h>

/*
 * This define allows iOUT1 and/or iOUT2 to default values to be set.
 * These bits may be wired to control other hardware.  It would may be
 * rude to change these bits when hanging up a modem...
 */
#define	COM_MCR_DEFAULT		(0)

/*
 * Definition of the driver for the auto-configuration program
 */
int	sdbs_probe(), sdbs_attach();

static caddr_t			sdbs_std[NSDBS] = { (caddr_t)SDB_SERIAL_START };
static struct bus_device	*sdbs_dinfo[NSDBS];
struct bus_driver		sdbs_driver = {
	/*
	 *	  structure		   structure    field
	 *	initialization		    field    description
	 */
	sdbs_probe,		/* probe  - is the i82510 installed ?	 */
	0,			/* slave  - really a probe for slaves... */
	sdbs_attach,		/* attach - setup driver after the probe */
	0,			/* go	  - start transfer		 */
	sdbs_std,		/* addr	  - device csr addresses	 */
	"sdbs",			/* dname  - device driver name		 */
	sdbs_dinfo,		/* dinfo  - bus_device backpointers	 */
	0,			/* mname  - name of controller		 */
	0,			/* minfo  - bus_ctlr backpointers	 */
	0			/* flags  - flags			 */
};

/*
 * Driver software state
 */
struct sdbs_softc {
	caddr_t		com_regs;	/* i/o space address */
	struct tty	*com_tty;	/* tty back pointer */
	int		com_modem;	/* modem control bits */
	int		com_enables;	/* shadow of enabled interrupts */
	int		com_prevenable;	/* saved enable state after polling */
	boolean_t	com_probed;	/* has probed once */
	boolean_t	com_attached;	/* has attached */
	boolean_t	com_polling;	/* in console polling mode */
	boolean_t	com_softCAR;	/* software carrier */
	unsigned long	com_cnt_rx;	/* rx interrupt count */
	unsigned long	com_cnt_tx;	/* tx interrupt count */
	unsigned long	com_cnt_mod;	/* modem interrupt count */
	unsigned long	com_cnt_bad;	/* unknown interrupt count */
	unsigned long	com_cnt_err;	/* error interrupt count */
	unsigned long	com_cnt_or;	/* overruns */
	unsigned long	com_cnt_brk;	/* breaks */
	unsigned long	com_cnt_pe;	/* parity errors */
	unsigned long	com_cnt_fe;	/* framing errors */
} sdbs_softc_data[NSDBS];
typedef struct sdbs_softc *sdbs_softc_t;

#ifndef	B19200
#define B19200	EXTA
#endif	B19200

#ifndef	B38400
#define	B38400	EXTB
#endif	B38400

/*
 * Rhetorical question:
 *	Does anyone actually *use* baud	rates below 300 (aside from 0)?
 */
static unsigned short	divisor[] = {
	   0,	/* B0		 */
	2304,	/* B50		 */
	1536,	/* B75		 */
	1047,	/* B110		 */
	 857,	/* B134		 */
	 768,	/* B150		 */
	 576,	/* B200		 */
	 384,	/* B300		 */
	 192,	/* B600		 */
	  96,	/* B1200	 */
	  64,	/* B1800	 */
	  48,	/* B2400	 */
	  24,	/* B4800	 */
	  12,	/* B9600	 */
	   6,	/* EXTA ( 19200) */
	   3,	/* EXTB ( 38400) */
	   2,	/*   ?  ( 57600) */
	   1	/*   ?	(115200) */
};

/*
 * Return non-zero if the device is present
 */
sdbs_probe(ignored, device)
	caddr_t 		ignored;
        struct bus_device	*device;
{
	int		comunit = device->unit;
	sdbs_softc_t	com;
	extern int expansion_id(); 

	if ((comunit < 0) || (comunit >= NSDBS)) {
		return 0;
	}

	com = &sdbs_softc_data[comunit];
	if (com->com_probed) {
		return 1;
	}

         /* check for a SCSI-16 expansion board type */
	 if (expansion_id() != EXP_ID_SCSI)
		 return 0;

	/*
	 * XXX Should poke at the chip to see if it is really there...
	 */
	sdbs_reset(comunit);

	/*
	 * The generic console code doesn't call the
	 * attach routine... more is done here than
	 * really ought to be...
	 */
	com->com_probed = TRUE;
	com->com_modem = FALSE;
	com->com_polling = FALSE;
	com->com_regs = sdbs_std[comunit];
	com->com_tty = console_tty[comunit];
	com->com_tty->t_addr = com->com_regs;

	return 1;
}

/*
 * Reset the chip and drain out any driver-confusing sludge
 */
sdbs_reset(comunit)
	int	comunit;
{
	caddr_t	reg;
	caddr_t	ier, mcr, iir, lsr, rbr, msr;

	reg = sdbs_std[comunit];

	sdb_serial_reset(comunit);
	delay(1);

	ier = INTR_ENAB(reg);
	mcr = MODEM_CTL(reg);
	iir = INTR_ID(reg);
	lsr = LINE_STAT(reg);
	rbr = TXRX(reg);
	msr = MODEM_STAT(reg);

	outb(ier, 0);
	outb(mcr, COM_MCR_DEFAULT);
	while (!(inb(iir) & 1)) {
		inb(lsr);
		inb(rbr);
		inb(msr);
	}
}

/*
 * Finalize driver installation
 */
sdbs_attach(device)
	struct bus_device	*device;
{
	sdbs_softc_t	com;

	com = &sdbs_softc_data[device->unit];
	ttychars(com->com_tty);
	com->com_attached = TRUE;
}

/*
 * Enable/disable polling mode on a line.
 * Remember the state of enabled interrupts.
 */
sdbs_pollc(unit, on)
	int		unit;
	boolean_t	on;
{
	sdbs_softc_t	com;
	caddr_t		ier;

	com = &sdbs_softc_data[unit];
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
 * Modem signal state change (eg, carrier has dropped)
 * XXX (perhaps RTS/CTS flow control?)
 */
sdbs_modemint(com, iid)
	sdbs_softc_t	com;
	unsigned char	iid;
{
	com->com_cnt_mod++;
}

/*
 * Disable transmit interrupts
 */
sdbs_txdis(com)
	sdbs_softc_t	com;
{
	caddr_t	ier;

	ier = INTR_ENAB(com->com_regs);
	com->com_enables &= ~iTX_ENAB;
	outb(ier, com->com_enables);
}

/*
 * Enable transmit interrupts
 */
sdbs_txena(com)
	sdbs_softc_t	com;
{
	caddr_t	ier;

	ier = INTR_ENAB(com->com_regs);
	com->com_enables |= iTX_ENAB;
	outb(ier, com->com_enables);
}


/*
 * Transmit holding register is empty
 */
sdbs_txint(com, iid)
	sdbs_softc_t	com;
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
		sdbs_txdis(com);
	} else {
		thr = TXRX(com->com_regs);
		outb(thr, c);
        }

	simple_unlock(&tp->t_lock);
}

/*
 * Receive holding register has a character
 */
sdbs_rxint(com, iid)
	sdbs_softc_t	com;
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
 * Parity error, framing error, overrun error, etc.
 */
sdbs_errint(com, iid)
	sdbs_softc_t	com;
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
		printf("sdbs error, status = 0x%x\n", s);
	}

	cons_simple_rint(0, 0, inb(TXRX(regs)), err);
}

/*
 * Unknown (and as yet unsupported) interrupt
 */
sdbs_unknownint(com, iid)
	sdbs_softc_t	com;
	unsigned char	iid;
{
	com->com_cnt_bad++;
	printf("sdbs unknown interrupt, iid = 0x%x\n", iid);
}

/*
 * Determine the nature of the interrupt and dispatch it
 */
sdbs_intr(unit)
	int	unit;
{
	sdbs_softc_t	com;
	struct tty	*tp;
	caddr_t		reg, iir;
	unsigned char	iid;

	com = &sdbs_softc_data[unit];
	tp  = com->com_tty;
	reg = com->com_regs;
	iir = INTR_ID(reg);

	/*
	 *	Drain each pending interrupt.
	 */
	while (! ((iid = inb(iir)) & 1)) {
		switch (iid) { 
		case MODi:
			sdbs_modemint(com, iid);
			break;
		case TRAi:
			sdbs_txint(com, iid);
			break;
		case RECi:
			sdbs_rxint(com, iid);
			break;
		case LINi:
			sdbs_errint(com, iid);
			break;
		default:
			sdbs_unknownint(com, iid);
			break;
		}
	}
}

/*
 * Receive a character via polling.
 * Optionally return early if no character is available.
 */
sdbs_getc(unit, line, wait, raw)
	boolean_t	wait;
	boolean_t	raw;
{
	caddr_t		reg, lsr, rbr;
	int		c, s;

	reg = sdbs_std[unit];
	lsr = LINE_STAT(reg);
	rbr = TXRX(reg);

	s = sploff();
	if ((wait == FALSE) && ((inb(lsr) & iDR) == 0)) {
		splon(s);
		return -1;
	}

	while ((inb(lsr) & iDR) == 0)
			;
	c = inb(rbr);
	if (c == '\r') {
		c = '\n';
	}

	splon(s);

	return c;
}

/*
 * Output a single character via polling.
 * Wait for the transmit holding register to be empty,
 * output the character, and wait for it to drain.
 */
sdbs_putc(unit, line, c)
	char	c;
{
	sdbs_softc_t	com;
	caddr_t		reg, lsr, thr, ier;
	int		i, s, txe, max;

	com = &sdbs_softc_data[unit];
	reg = sdbs_std[unit];
	lsr = LINE_STAT(reg);
	thr = TXRX(reg);
	ier = INTR_ENAB(reg);

	s = sploff();
	txe = (com->com_enables & iTX_ENAB) == iTX_ENAB;
	if (txe) {
		sdbs_txdis(com);
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
		sdbs_txena(com);
	}

	splon(s);
}

/*
 * Enable transmit interrupts
 */
sdbs_start(tp)
	struct tty	*tp;
{
	sdbs_softc_t	com;

	com = &sdbs_softc_data[minor(tp->t_dev)];
	if ((com->com_enables & iTX_ENAB) == 0)
		sdbs_txena(com);
}

/*
 * Configure "line" as per "tp".
 * (ie, translate tty flags and baud rates to chip-specific bits)
 */
sdbs_param(tp, line)
	struct tty	*tp;
{
	sdbs_softc_t	com;
	int		unit, mode;
	caddr_t		reg, mcr, lcr, bal, bah, ier;

	unit = minor(tp->t_dev);
	com = &sdbs_softc_data[unit];

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
		sdbs_set_modem_state(com);
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
	 * my card is the tried and true 3-wire kind. :-)
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
	delay(10000);	/* XXX 50MHz w/ caches is too fast for the chip XXX */

	com->com_tty->t_state |= TS_CARR_ON;
	com->com_modem |= (TM_DTR|TM_RTS);

	/*
	 * XXX
	 * modem control signals (based on com->com_modem)
	 * should be translated to chip-specific stuff here.
	 */
	/* sdbs_set_modem_state(com); */

	sdb_interrupt_enable(SDB_IMASK_SERIAL);
}

/*
 * af's code has a big #if 0 around this routine.
 * I guess it should call sdbs_set_modem_state() or something like it.
 * Perhaps sdbs_set_modem_state() should be implemented in terms of sdbs_mctl()?
 */
sdbs_mctl(dev, bits, how)
	int	dev, bits, how;
{
}

/*
 * Raise or lower carrier
 */
sdbs_softCAR(unit, line, on_or_off)
{
	sdbs_softc_t	com;
	struct tty	*tp;

	com = &sdbs_softc_data[unit];
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
 * Return TRUE if the sdbs device should be used as the console
 */
boolean_t
sdbs_cons_autoconf()
{
	extern int expansion_id(); 

         /* check for a SCSI-16 expansion board type */
	 if (expansion_id() != EXP_ID_SCSI)
		return FALSE;

#if	0
	if (sdbs_cons_override())
		return FALSE;
#endif	0

	return TRUE;
}

/*
 * Return TRUE if the sdbs device should be used as the console
 */
boolean_t
sdbs_cons_autoconf_firstnode()
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
				goto do_sdbs;

			case 'c':
				if (node_self() != firstnode) {
					return FALSE;
				}
				goto do_sdbs;
			case 'C':
				goto do_sdbs;
			}
		}
		return FALSE;
	}

do_sdbs:
	return sdbs_cons_autoconf();
}

#endif	NSDBS > 0
