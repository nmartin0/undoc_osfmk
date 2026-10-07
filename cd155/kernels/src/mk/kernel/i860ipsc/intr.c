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
 * $Log: intr.c,v $
 * Revision 1.3  1994/11/18  20:40:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/30  22:34:54  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/09/22  18:12:00  regnier
 * Initial revision
 *
 * Revision 2.6.2.2  92/04/08  15:44:00  jeffreyh
 * 	Reorder interrupt checks; add a history buffer; dont let serial
 * 	interrupts starve; add a hook for checking at thread_block()
 * 	[92/04/08            andyp]
 * 
 * Revision 2.6.2.1  92/03/28  10:08:53  jeffreyh
 * 	Change arguments to usmintr to include regs
 * 	[92/03/20            sjs]
 * 
 * Revision 2.6  91/12/10  16:31:52  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:29  jsb]
 * 
 * Revision 2.5  91/08/28  11:12:59  jsb
 * 	From Intel SSD: fixed intr_poll() and serialint().
 * 	[91/08/26  15:33:05  jsb]
 * 
 * Revision 2.4  91/06/18  20:52:30  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  19:02:30  jsb]
 * 
 * Revision 2.3  91/06/17  15:45:45  jsb
 * 	From Paul Pierce: replaced old dcm intr callouts with new.
 * 	[91/06/17  13:29:23  jsb]
 * 
 * Revision 2.2  90/12/04  14:50:14  jsb
 * 	First checkin.
 * 	[90/12/03  21:49:43  jsb]
 * 
 */

#include <cpus.h>
#include <sys/types.h>
#include <i860/thread.h>
#include <i860ipsc/ctlreg.h>
#include <i860ipsc/nodehw.h>
#include <i860/psl.h>
#include <mach_kdb.h>


/* These interrupts are always present */

extern hardclock();
extern usmintr();

unsigned long dcm_send_count;
unsigned long dcm_recv_count;

extern unsigned long	control_shadow;		/* defined in i860_init.c */
extern int	db_active;

int	intr_debug = 0;
unsigned long	unknown_interrupts = 0;
unsigned long	spurious_serial = 0;
unsigned long	last_unknown_status = 0;

#if	MACH_KDB
#if	DB_MACHINE_COMMANDS
#define	INTERRUPT_HISTORY	1

#if	INTERRUPT_HISTORY

#include <kern/thread.h>

#define	INTERRUPT_HISTLEN	64

struct interrupt_history {
	unsigned long	ih_intrn;	/* interrupt number */
	unsigned long	ih_regs;	/* pointer to saved state */
	unsigned long	ih_psr;		/* psr at time of interrupt */
	unsigned long	ih_pc;		/* pc at time of interrupt */
	unsigned long	ih_sp;		/* sp at time of interrupt */
	unsigned long	ih_fp;		/* fp at time of interrupt */
	unsigned long	ih_thread;	/* thread that was interrupted */
	unsigned long	ih_status;	/* status register at interrupt */
	unsigned long	ih_shadow;	/* control shadow at interrupt */
	unsigned long	ih_spl;		/* spl at time of interrupt */
	unsigned long	ih_timein;	/* hi-res clock at entry */
	unsigned long	ih_timeout;	/* hi-res clock at exit */
};

struct interrupt_history interrupt_history[INTERRUPT_HISTLEN];
int interrupt_history_next;
static unsigned long total_interrupt_count;
static void	ih_complete_timestamp();
#endif	INTERRUPT_HISTORY
#endif	DB_MACHINE_COMMANDS
#endif	MACH_KDB

int	interrupt_service_in_progress;

/*
 * the IM bit of the PSR is off at this point, and the ipl
 * still needs to be set.
 */
interrupt(regs)
struct i860_saved_state	*regs;
{
	u_long	sreg, status;
	int	s = -1;
#if	INTERRUPT_HISTORY
	struct interrupt_history *ih;
#endif	INTERRUPT_HISTORY

netipc_called_here(__FILE__, __LINE__, "(enter) interrupt");
	assert(interrupt_service_in_progress >= 0);
	interrupt_service_in_progress++;

	status = STATUS_REG;
	sreg = status &
	    (control_shadow & (RECV_INT | SEND_INT | SERIAL_INT) |
	     (PARITY_INT | BUSTOUT_INT));

#if	INTERRUPT_HISTORY
	{
		ih = &interrupt_history[interrupt_history_next++];
		ih->ih_intrn = total_interrupt_count++;
		if (interrupt_history_next >= INTERRUPT_HISTLEN) {
			interrupt_history_next = 0;
		}
		ih->ih_regs = (unsigned long) regs;
		ih->ih_psr = regs->psr;
		ih->ih_pc = regs->pc;
		ih->ih_sp = regs->sp;
		ih->ih_fp = regs->fp;
		ih->ih_thread = (unsigned long) current_thread();
		ih->ih_status = status;
		ih->ih_shadow = control_shadow;
		ih->ih_spl = get_spl();
		ih->ih_timein = (unsigned long) inb(COUNTER_PORT);
		ih->ih_timeout = 0;
	}
#endif	INTERRUPT_HISTORY

	/*
	 * interrupts worth dying for
	 */
	if (sreg & PARITY_INT) {
		CLR_CONTROL(CLEAR_PARITY);
		SET_CONTROL(CLEAR_PARITY);
		printf("Parity error! unreliable pc=%x, psr=%x\n", regs->pc, regs->psr);
	}
	if (sreg & BUSTOUT_INT) {
		CLR_CONTROL(CLEAR_BUS);
		SET_CONTROL(CLEAR_BUS);
		printf("Bus timeout! unreliable pc=%x, psr=%x\n", regs->pc, regs->psr);
	}


	/*
	 * Serial chip (includes clock interrupts)
	 */
	if (sreg & SERIAL_INT) {
		if ((s = serialint(regs)) == -1) {
			unknown_interrupts++;
			spurious_serial++;
		}
	}

	/*
	 * DCM Send interrupt
	 */
	else if (sreg & SEND_INT) {
		s = spldcm_noi();
		dcm_send_count++;
		dcm_send_intr();
	}

	/*
	 * DCM Receive interrupt
	 */
	else if (sreg & RECV_INT) {
		s = spldcm_noi();
		dcm_recv_count++;
		dcm_recv_intr();
	}

	/*
	 * unknown
	 */
	else {
		unknown_interrupts++;
		last_unknown_status = status;
	}

#if	INTERRUPT_HISTORY
	ih_complete_timestamp(ih);
#endif	INTERRUPT_HISTORY

	assert(interrupt_service_in_progress > 0);
	interrupt_service_in_progress--;

netipc_called_here(__FILE__, __LINE__, "(leave) interrupt");

	if (s != -1) {
		splx_noi(s);
	}

	return 0;
}


#if	INTERRUPT_HISTORY
static void ih_complete_timestamp(ih)
struct interrupt_history *ih;
{
	if ((ih < &interrupt_history[0]) ||
	    (ih > &interrupt_history[INTERRUPT_HISTLEN-1])) {
		panic("ih_complete_timestamp");
	}

	if (ih->ih_timeout == 0) {
		ih->ih_timeout = (unsigned long) inb(COUNTER_PORT);
	}
}
#endif	INTERRUPT_HISTORY


serialint(regs)
struct i860_saved_state *regs;
{
	int	c1, c2, c3, s, n;

	s = -1;

	c1 = inb(UART_GIR) & 0xFF;
	if ((c1 & 0x01) != 0) {
		return s;
	}

	n = ((c1 & 0xf) >> 1);

	switch (n & 7) {
	case 0:	/* modem */
		c2 = inb(UART_MSR) & 0xFF;
		if (c2 & 0x08) {	/* DCM interrupt */
			panic("DCM error");
		} else if (c2 & 0x01) { /* USM interrupt */
			printf("USM interrupt\n");
			c3 = inb(UART_MSR); /* For both transitions */
#if	MACH_KDB
			gimmeabreak();
#endif	MACH_KDB
		} else {
			printf("Diagnostic line error, c1=0x%x, c2=0x%x\n", c1, c2);
		}
		break;

	case 4:
	case 1: /* transmit */
		printf("%s, line %d:serialint: type 1 or 4\n", __FILE__, __LINE__);
		break;

	case 2:	/* receive */
		s = spltty_noi();
		usmintr(0, regs);
		break;

	case 3:	/* rx error */
		(void) inb(UART_LSR);
		printf("%s, line %d:serialint: rx error int\n", __FILE__, __LINE__);
		break;

	case 5: /* timer */
		s = splclock_noi();
		update_leds();
		hardclock(0, s, regs);
		break;

	default:
		printf("%s, line %d:serialint: id=%d\n", __FILE__, __LINE__, ((c1 & 0x0f) >> 1));
		panic("serialint: unknown interrupt");
		break;
	}

	return s;
}

update_leds()
{
	extern unsigned long	dcm_send_count;
	static unsigned long	dcm_send_shadow;
	extern unsigned long	dcm_recv_count;
	static unsigned long	dcm_recv_shadow;
	extern unsigned long	previous_user_count;
	static unsigned long	previous_user_shadow;

	if (dcm_send_count != dcm_send_shadow) {
		led_red_on();
		dcm_send_shadow = dcm_send_count;
	} else {
		led_red_off();
	}
	if (dcm_recv_count != dcm_recv_shadow) {
		led_yellow_on();
		dcm_recv_shadow = dcm_recv_count;
	} else {
		led_yellow_off();
	}
	if (previous_user_count != previous_user_shadow) {
		led_green_on();
		previous_user_shadow = previous_user_count;
	} else {
		led_green_off();
	}
}


#if	INTERRUPT_HISTORY
void db_show_intr_hist()
{
	int	i, j;
	struct interrupt_history *ih;
	unsigned long	status, shadow, delta, us, tenths;
	char	*dash = " - ";
	char	*s = "%s ";

	db_printf("Nth int      regs=     thread     us spl   shadow   status (status decode)\n");

	j = interrupt_history_next - 1;
	for (i = 0; i < INTERRUPT_HISTLEN; i++) {

		if (j < 0)
			j = INTERRUPT_HISTLEN - 1;

		ih = &interrupt_history[j];
		status = ih->ih_status;
		shadow = ih->ih_shadow;
		if (ih->ih_timeout != 0) {
			delta = (ih->ih_timeout - ih->ih_timein);
			us = delta / 10;
			tenths = delta - (us * 10);
		} else {
			us = 0;
			tenths = 0;
		}

		db_printf("%7u 0x%08x 0x%08x %4u.%1u %3d %8x %8x ",
			ih->ih_intrn,
			ih->ih_regs,
			ih->ih_thread,
			us,
			tenths,
			ih->ih_spl,
			shadow,
			status);

		db_printf(s, (status & RECV_INT)    ? "drx" : dash);
		db_printf(s, (status & SEND_INT)    ? "dtx" : dash);
		db_printf(s, (status & SERIAL_INT)  ? "ser" : dash);
		db_printf(s, (status & PARITY_INT)  ? "PAR" : dash);
		db_printf(s, (status & BUSTOUT_INT) ? "BUS" : dash);
		db_printf("\n");

		j--;
	}
}
#endif	INTERRUPT_HISTORY

