/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: second_cons.c,v $
 * Revision 1.5  1994/11/18  20:49:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:43:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:05:35  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:32:32  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:56:03  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:55:14  cfj
 * Bump major revision number.
 *
 * Revision 2.6  1992/03/09  13:38:28  durriya
 * 	92/02/25  17:48:46  condict
 * 	Wire the console-reader thread (second server), so that all special-purpose
 * 	threads are wired (a policy that ux_server_loop depends on).
 * 
 * 	92/02/19  16:23:23  emcmanus
 * 	Decode baud rates using baud_decode function.  BSD-style tty flags
 * 	set in second_tty_open are now reflected in SYSV-style equivalents
 * 	via call to ttcompatsetflags.
 * 
 * 	92/01/20  10:46:01  bernadat
 * 	Added a call to xpr_dump() if BELL key is typed in. (Configurable)
 * 	Revision 3.10  91/12/13  13:04:03  sp
 * 	Remove CMUCS conditionals
 * 
 * Revision 2.5  91/12/17  09:43:15  roy
 * 	91/12/13  13:04:03  sp
 * 	Remove CMUCS conditionals
 * 
 * 	91/11/13  14:56:09  barbou
 * 	Added code to restore the console state when halting a second server.
 * 
 * Revision 2.4  91/11/13  15:49:13  rabii
 * 	Must pass third arg 'flag' to line discipline open 
 * 	in second_tty_open(). 
 * 
 * Revision 2.3  91/10/14  13:24:35  sjs
 * 	91/10/01  14:12:53  condict
 * 	Remove useless spl calls.
 * 
 * Revision 2.2  91/08/31  14:27:35  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.7  91/07/29  16:33:00  barbou
 * Modified to decide at run-time if running as a second server.
 * 
 * Revision 3.6  91/07/04  18:02:54  barbou
 * Added console locking. Modified the way of sleeping.
 * 
 * Revision 3.5  91/07/02  14:47:33  condict
 * Add static tty struct (cons_tty) for console.
 * 
 * Revision 3.4  91/06/25  17:13:31  condict
 * Fix initialization/allocation of tty struct for the console, and fix
 * initial state on open (use OSF/1 defaults).
 * 
 * Revision 3.3  91/06/25  15:11:38  jose
 * Corrected first console_open bug
 * 
 * Revision 3.2  91/06/12  09:40:15  condict
 * Finish adapting to the OSF/1 select syscall data structures.
 * 
 * Revision 3.1  91/05/29  16:21:46  condict
 * Add queue_init call to the init of a new tty struct.
 * 
 * Revision 3.0  91/05/07  15:11:34  condict
 * A modified verions of uxkern/cons.c that diverts console output to the
 * stdout and stderr of the server, when it is running as a second server.
 * 
 * Revision 1.1  90/11/20  07:41:29  bernadat
 * Initial revision
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/ttyloc.h>
#include <sys/tty.h>
#include <sys/conf.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/user.h>
#include <sys/synch.h>
#include <sys/ttydefaults.h>
#include <xpr_debug.h>

#include <uxkern/device_utils.h>

extern struct tty	*cons_tp;	/* Points to cons_tty (see conf.c) */

dev_t		cons_dev_number;

extern struct tty *tty_find_tty();


#ifdef	SECOND_SERVER

extern	int	second_server;

second_cons_open(dev, flag)
	dev_t	dev;
	int	flag;
{
	register int error;
	register int major_num;
	if (cons_dev_number == NODEV) {
	    /*
	     * Look for console.
	     */
	    for (major_num = 0; major_num < nchrdev; major_num++) {
		if (!strcmp(cdevsw[major_num].d_name, "console")) {
		    cons_dev_number = makedev(major_num, 0);
		    break;
		}
	    }
	    if (cons_dev_number == NODEV) {
		panic("no console configured");
	    }
	}
	error = second_tty_open(cons_dev_number, flag);

	return (error);
}

mach_port_t
second_cons_port(dev)
	dev_t	dev;
{
	panic("second_cons_port");
}

second_tty_stop()
{
}


int	second_tty_start();
int	second_cons_intr();

/* Save the console state to restore on exit() as a civilized application */
static 	struct sgttyb second_cons_tty;
static 	int second_cons_tty_saved = 0;

second_cons_restore()
{
	if (second_cons_tty_saved) 
		second_ioctl(0, TIOCSETP, &second_cons_tty);
}

second_tty_open(dev, flag)
	dev_t	dev;
	int	flag;
{
	register struct tty *tp;
	static int first_console_open = 1;
	int error;

	/*
	 * Check whether tty is already open.
	 */
	
	tp = &cdevsw[major(dev)].d_ttys[minor(dev)];
	TTY_LOCK(tp);

	tp->t_oproc = second_tty_start;

	if ((tp->t_state & TS_ISOPEN) == 0) {
	    struct	sgttyb ttyb;

	    queue_init(&tp->t_selq);

	    /*
	     * Set initial characters
	     */
	    ttychars(tp);

	    /*
	     * Get configuration parameters from device, put in tty structure
	     */

	    second_ioctl(0, TIOCGETP, &ttyb);

	    if (!second_cons_tty_saved) {
		    second_cons_tty = ttyb;
		    second_cons_tty_saved = 1;
	    }

	    /* From OSF/1: */
	    tp->t_iflag = TTYDEF_IFLAG;
	    tp->t_oflag = TTYDEF_OFLAG;
	    tp->t_lflag = TTYDEF_LFLAG;
	    tp->t_cflag = CS8|CREAD;
	    tp->t_line = 0;
	    tp->t_state = TS_ISOPEN|TS_CARR_ON;

	    tp->t_ispeed = baud_decode(ttyb.sg_ispeed);
	    tp->t_ospeed = baud_decode(ttyb.sg_ospeed);
	    ttsetwater(tp);
	    if (ttyb.sg_flags & EVENP)
		tp->t_flags |= EVENP;
	    if (ttyb.sg_flags & ODDP)
		tp->t_flags |= ODDP;
	    if (ttyb.sg_flags & ECHO)
		tp->t_flags |= ECHO;
	    if (ttyb.sg_flags & CRMOD)
		tp->t_flags |= CRMOD;
	    if (ttyb.sg_flags & XTABS)
		tp->t_flags |= XTABS;
	    ttcompatsetflags(tp, &tp->t_termios);

	    ttyb.sg_flags |= RAW;
	    ttyb.sg_flags &= ~ECHO;
	    second_ioctl(0, TIOCSETP, &ttyb);
	    /*
	     * Pretend that carrier is always on, until I figure out
	     * how to do it right.
	     */
	    tp->t_state |= TS_CARR_ON; /* should get from TTY_STATUS */
	}
	else if (tp->t_state & TS_XCLUDE && u.u_uid != 0) {
		error = EBUSY;
		goto out;
	}

	if ((flag & FREAD) && first_console_open) {
		/* create thread to read on standard input */	
		ux_create_thread(second_cons_intr);

		first_console_open = 0;
	  	tp->t_state |= TS_RQUEUED;
	}
	/*
	 * Wait for CARR_ON
	 */
	if (flag & O_NDELAY) {
	    tp->t_state |= TS_ONDELAY;
	}
	else {
	    while ((tp->t_state & TS_CARR_ON) == 0) {
		tp->t_state |= TS_WOPEN;
		if (error = ttysleep(tp, (caddr_t)&tp->t_rawq, TTIPRI | PCATCH,
				     ttopen))
			goto out;
		/*
		 * some devices sleep on t_state...	XXX
		 */
	    }
	}

	error = (*linesw[tp->t_line].l_open)(dev, tp, flag);
 out:
	TTY_UNLOCK(tp);
	return (error);
}

#if   XPR_DEBUG
int show_xpr_on_bell = 0;
#endif	/* XPR_DEBUG */

second_cons_intr()
{
	char c;
	struct tty *tp = tty_find_tty(cons_dev_number);

	cthread_set_name(cthread_self(), "2nd cons intr");

  	while (1) if (second_read(0, &c, 1) == 1) {
#if   XPR_DEBUG
		if ( c == CBELL && show_xpr_on_bell) { /* Bell CTRL G */
			xpr_dump(0);
			continue;
	     	}
#endif	/* XPR_DEBUG */
		TTY_LOCK(tp);
		interrupt_enter(SPLTTY);
		(*linesw[tp->t_line].l_rint)(c, tp);
		interrupt_exit(SPLTTY);
		TTY_UNLOCK(tp);
        } else
	  	printf("second_read failed\n");
}

second_tty_start(tp)
	register struct tty *tp;
{
	int	cc, s;

	if ((tp->t_state & (TS_TIMEOUT|TS_BUSY|TS_TTSTOP)) == 0) {
	    if (tp->t_outq.c_cc <= tp->t_lowat) {
		if (tp->t_state & TS_ASLEEP) {
		    tp->t_state &= ~TS_ASLEEP;
		    wakeup((caddr_t)&tp->t_outq);
		}
		select_wakeup(&tp->t_selq);
	    }

	    /* get characters from tp->t_outq,
	       send to device */
	    while (tp->t_outq.c_cc != 0) {
		cc = ndqb(&tp->t_outq, 0);	
		second_write(1, tp->t_outq.c_cf, cc);
		ndflush(&tp->t_outq, cc);
	    }
	}
}

second_cnputc(c)
	int	c;
{
    	second_write(2, &c, 1);
	if (c == '\n')
	    cnputc('\r');
}

#endif	/* SECOND_SERVER */
