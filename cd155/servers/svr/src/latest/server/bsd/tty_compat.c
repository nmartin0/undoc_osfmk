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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: tty_compat.c,v $
 * Revision 1.6  1994/11/18  20:28:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/13  17:52:25  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.4  1993/07/14  17:49:55  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:50:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:06:43  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:33  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1992/03/09  14:33:00  durriya
 * 	Revision 3.2  92/02/19  16:23:02  emcmanus
 * 	Conversion between baud rates such as 9600 and codes such as
 * 	B9600 == 13 now done by functions baud_encode and baud_decode.
 *
 * Revision 2.2  91/08/31  13:23:43  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:56:00  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/31  13:50:01  devrcs
 * 	INFO => STATUS, FLUSHO => DISCARD.
 * 	[90/10/14  16:54:18  ers]
 * 
 * Revision 1.6  90/10/07  13:19:54  devrcs
 * 	Implement TCION and TCIOFF for TCXON.
 * 	[90/09/28  20:16:50  brezak]
 * 	Correctly reference data for TCXON and TCFLSH ioctl by using
 * 	mnemonic values instead of constants.
 * 	[90/09/19  21:58:07  brezak]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:01:20  gm]
 * 
 * Revision 1.5  90/09/23  15:43:43  devrcs
 * 	Fixed for ANSI compliance
 * 	[90/09/05  17:12:44  rossi]
 * 
 * Revision 1.4  90/08/09  13:15:23  devrcs
 * 	Fix TCFLSH and TCXONC ioctls to correctly use third argument.
 * 	[90/08/01  11:13:12  tmt]
 * 
 * Revision 1.3  90/06/22  20:07:25  devrcs
 * 	Made minor fixes to the SYID compatibility routines.
 * 	[90/06/14  23:00:15  havens]
 * 
 * 	Added code for SVID compatibility.  Mainly output processing flags plus
 * 	the XCASE stuff. This file now holds the function which does the
 * 	convertion between termio and termios.  Also changed the BSD function
 * 	to get the character size from cflags instead of flags which is
 * 	no longer used.
 * 	[90/06/13  12:48:24  havens]
 * 
 * Revision 1.2  90/02/16  16:48:58  devrcs
 * 	Delete include of <sys/dir.h>
 * 	[90/02/15  16:04:55  ers]
 * 
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:22:14  ers]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)tty_compat.c	7.3 (Berkeley) 11/20/89
 */

/* 
 * mapping routines for old line discipline (yuck)
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/termios.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/conf.h>
#include <sys/dk.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#include <sys/syslog.h>
#include <sys/termio.h>

#include <machine/reg.h>

int ttydebug = 0;

/* XXX - fold these two tables into one */
static struct speedtab compatspeeds[] = {
	38400,	15,
	19200,	14,
	9600,	13,
	4800,	12,
	2400,	11,
	1800,	10,
	1200,	9,
	600,	8,
	300,	7,
	200,	6,
	150,	5,
	134,	4,
	110,	3,
	75,	2,
	50,	1,
	0,	0,
	-1,	-1,
};
static int compatspcodes[16] = { 
	0, 50, 75, 110, 134, 150, 200, 300, 600, 1200,
	1800, 2400, 4800, 9600, 19200, 38400,
};

#if COMPAT_43 || OSF1_SERVER
/* Convert a baud rate such as 9600 to a BSD-style code such as 13.  */
int
baud_encode(baud)
int baud;
{
	if ((unsigned) baud > (unsigned) 15)
		return ttspeedtab(baud, compatspeeds);
	else return baud;
}

/* Convert a BSD-style baud code such as 13 to a baud rate such as 9600. */
int
baud_decode(code)
int code;
{
	if ((unsigned) code <= (unsigned) 16)
		return compatspcodes[code];
	else return code;
}
#endif	/* COMPAT_43 || OSF1_SERVER */

#ifdef COMPAT_43

/*ARGSUSED*/
ttcompat(tp, com, data, flag)
	register struct tty *tp;
	caddr_t data;
{
	switch(com) {
	case TIOCGETP: {
		register struct sgttyb *sg = (struct sgttyb *)data;
		register u_char *cc = tp->t_cc;
		register speed;

		speed = baud_encode(tp->t_ospeed);
		sg->sg_ospeed = (speed == -1) ? 15 : speed;
		if (tp->t_ispeed == 0)
			sg->sg_ispeed = sg->sg_ospeed;
		else {
			speed = baud_encode(tp->t_ispeed);
			sg->sg_ispeed = (speed == -1) ? 15 : speed;
		}
		sg->sg_erase = cc[VERASE];
		sg->sg_kill = cc[VKILL];
		sg->sg_flags = ttcompatgetflags(tp);
		break;
	}

	case TIOCSETP:
	case TIOCSETN: {
		register struct sgttyb *sg = (struct sgttyb *)data;
		struct termios term;

		term = tp->t_termios;
		term.c_ispeed = baud_decode(sg->sg_ispeed);
		term.c_ospeed = baud_decode(sg->sg_ospeed);
		term.c_cc[VERASE] = sg->sg_erase;
		term.c_cc[VKILL] = sg->sg_kill;
		tp->t_flags = (tp->t_flags&0xffff0000) | sg->sg_flags;
		ttcompatsetflags(tp, &term);
		return (ttioctl(tp, com == TIOCSETP ? TIOCSETAF : TIOCSETA, 
			&term, flag));
	}

	case TIOCGETC: {
		struct tchars *tc = (struct tchars *)data;
		register u_char *cc = tp->t_cc;

		tc->t_intrc = cc[VINTR];
		tc->t_quitc = cc[VQUIT];
		tc->t_startc = cc[VSTART];
		tc->t_stopc = cc[VSTOP];
		tc->t_eofc = cc[VEOF];
		tc->t_brkc = cc[VEOL];
		break;
	}
	case TIOCSETC: {
		struct tchars *tc = (struct tchars *)data;
		register u_char *cc = tp->t_cc;

		cc[VINTR] = tc->t_intrc;
		cc[VQUIT] = tc->t_quitc;
		cc[VSTART] = tc->t_startc;
		cc[VSTOP] = tc->t_stopc;
		cc[VEOF] = tc->t_eofc;
		cc[VEOL] = tc->t_brkc;
		if (tc->t_brkc == -1)
			cc[VEOL2] = _POSIX_VDISABLE;
		break;
	}
	case TIOCSLTC: {
		struct ltchars *ltc = (struct ltchars *)data;
		register u_char *cc = tp->t_cc;

		cc[VSUSP] = ltc->t_suspc;
		cc[VDSUSP] = ltc->t_dsuspc;
		cc[VREPRINT] = ltc->t_rprntc;
		cc[VDISCARD] = ltc->t_flushc;
		cc[VWERASE] = ltc->t_werasc;
		cc[VLNEXT] = ltc->t_lnextc;
		break;
	}
	case TIOCGLTC: {
		struct ltchars *ltc = (struct ltchars *)data;
		register u_char *cc = tp->t_cc;

		ltc->t_suspc = cc[VSUSP];
		ltc->t_dsuspc = cc[VDSUSP];
		ltc->t_rprntc = cc[VREPRINT];
		ltc->t_flushc = cc[VDISCARD];
		ltc->t_werasc = cc[VWERASE];
		ltc->t_lnextc = cc[VLNEXT];
		break;
	}
	case TIOCLBIS:
	case TIOCLBIC:
	case TIOCLSET: {
		struct termios term;

		term = tp->t_termios;
		if (com == TIOCLSET)
			tp->t_flags = (tp->t_flags&0xffff) | *(int *)data<<16;
		else {
			tp->t_flags = 
			 (ttcompatgetflags(tp)&0xffff0000)|(tp->t_flags&0xffff);
			if (com == TIOCLBIS)
				tp->t_flags |= *(int *)data<<16;
			else
				tp->t_flags &= ~(*(int *)data<<16);
		}
		ttcompatsetlflags(tp, &term);
		return (ttioctl(tp, TIOCSETA, &term, flag));
	}
	case TIOCLGET:
		*(int *)data = ttcompatgetflags(tp)>>16;
		break;

	case OTIOCGETD:
		*(int *)data = tp->t_line ? tp->t_line : 2;
		break;

	case OTIOCSETD: {
		int ldisczero = 0;

		return(ttioctl(tp, TIOCSETD, 
			*(int *)data == 2 ? (caddr_t)&ldisczero : data, flag));
	}

	default:
		return (-1);
	}
	return(0);
}

ttcompatgetflags(tp)
	register struct tty *tp;
{
	register long iflag = tp->t_iflag;
	register long lflag = tp->t_lflag;
	register long oflag = tp->t_oflag;
	register long cflag = tp->t_cflag;
	register flags = 0;

	if (iflag&IXOFF)
		flags |= TANDEM;
	if (iflag&ICRNL || oflag&ONLCR)
		flags |= CRMOD;
	if (cflag&PARENB) {
		if (iflag&INPCK) {
			if (cflag&PARODD)
				flags |= ODDP;
			else
				flags |= EVENP;
		} else
			flags |= EVENP | ODDP;
	} else {
		if (!(oflag&OPOST))
			flags |= LITOUT;
		if ((tp->t_cflag&CSIZE) == CS8)
			flags |= PASS8;
	}
	
	if ((lflag&ICANON) == 0) {	
		/* fudge */
		if (iflag&IXON || lflag&ISIG || lflag&IEXTEN || cflag&PARENB)
			flags |= CBREAK;
		else
			flags |= RAW;
	}
	if (oflag&OXTABS)
		flags |= XTABS;
	if (lflag&ECHOE)
		flags |= CRTERA|CRTBS;
	if (lflag&ECHOKE)
		flags |= CRTKIL|CRTBS;
	if (lflag&ECHOPRT)
		flags |= PRTERA;
	if (lflag&ECHOCTL)
		flags |= CTLECH;
	if ((iflag&IXANY) == 0)
		flags |= DECCTQ;
	flags |= lflag&(ECHO|MDMBUF|TOSTOP|FLUSHO|NOHANG|PENDIN|NOFLSH);
	return (flags);
}

ttcompatsetflags(tp, t)
	register struct tty *tp;
	register struct termios *t;
{
	register flags = tp->t_flags;
	register long iflag = t->c_iflag;
	register long oflag = t->c_oflag;
	register long lflag = t->c_lflag;
	register long cflag = t->c_cflag;

	if (flags & RAW) {
		iflag &= IXOFF;
		oflag &= ~OPOST;
		lflag &= ~(ECHOCTL|ISIG|ICANON|IEXTEN);
		t->c_cc[VTIME] = 0;
		t->c_cc[VMIN] = 1;
	} else {
		iflag |= BRKINT|IXON|IMAXBEL;
		oflag |= OPOST;
		lflag |= ISIG|IEXTEN;
		if (flags & XTABS)
			oflag |= OXTABS;
		else
			oflag &= ~OXTABS;
		if (flags & CBREAK)
			lflag &= ~ICANON;
		else
			lflag |= ICANON;
		if (flags&CRMOD) {
			iflag |= ICRNL;
			oflag |= ONLCR;
		} else {
			iflag &= ~ICRNL;
			oflag &= ~ONLCR;
		}
	}
	if (flags&ECHO)
		lflag |= ECHO;
	else
		lflag &= ~ECHO;
		
	if (flags&(RAW|LITOUT|PASS8)) {
		cflag &= ~(CSIZE|PARENB);
		cflag |= CS8;
		if ((flags&(RAW|PASS8)) == 0)
			iflag |= ISTRIP;
	} else {
		cflag &= ~CSIZE;
		cflag |= CS7|PARENB;
	}
	if ((flags&(EVENP|ODDP)) == EVENP) {
		iflag |= INPCK;
		cflag &= ~PARODD;
	} else if ((flags&(EVENP|ODDP)) == ODDP) {
		iflag |= INPCK;
		cflag |= PARODD;
	} else 
		iflag &= ~INPCK;
	if (flags&LITOUT)
		oflag &= ~OPOST;	/* move earlier ? */
	if (flags&TANDEM)
		iflag |= IXOFF;
	else
		iflag &= ~IXOFF;
	t->c_iflag = iflag;
	t->c_oflag = oflag;
	t->c_lflag = lflag;
	t->c_cflag = cflag;
}

ttcompatsetlflags(tp, t)
	register struct tty *tp;
	register struct termios *t;
{
	register flags = tp->t_flags;
	register long iflag = t->c_iflag;
	register long oflag = t->c_oflag;
	register long lflag = t->c_lflag;
	register long cflag = t->c_cflag;

	if (flags&CRTERA)
		lflag |= ECHOE;
	else
		lflag &= ~ECHOE;
	if (flags&CRTKIL)
		lflag |= ECHOKE;
	else
		lflag &= ~ECHOKE;
	if (flags&PRTERA)
		lflag |= ECHOPRT;
	else
		lflag &= ~ECHOPRT;
	if (flags&CTLECH)
		lflag |= ECHOCTL;
	else
		lflag &= ~ECHOCTL;
	if ((flags&DECCTQ) == 0)
		lflag |= IXANY;
	else
		lflag &= ~IXANY;
	lflag &= ~(MDMBUF|TOSTOP|FLUSHO|NOHANG|PENDIN|NOFLSH);
	lflag |= flags&(MDMBUF|TOSTOP|FLUSHO|NOHANG|PENDIN|NOFLSH);
	if (flags&(LITOUT|PASS8)) {
		iflag &= ~ISTRIP;
		cflag &= ~(CSIZE|PARENB);
		cflag |= CS8;
		if (flags&LITOUT)
			oflag &= ~OPOST;
		if ((flags&(PASS8|RAW)) == 0)
			iflag |= ISTRIP;
	} else if ((flags&RAW) == 0) {
		cflag &= ~CSIZE;
		cflag |= CS7|PARENB;
		oflag |= OPOST;
	}
	t->c_iflag = iflag;
	t->c_oflag = oflag;
	t->c_lflag = lflag;
	t->c_cflag = cflag;
}
#endif	/* COMPAT_43 */

/* Mapping routines for SVID tty ioctls */
tt_sysv_compat(tp, com, data, flag)
register struct tty *tp;
caddr_t data;
{
	struct termios term;
	struct termio *termio_ptr;
	int cmd;
	int speed;
	int line;
	int i;
        TSPLVAR(s)
	
	termio_ptr = (struct termio *) data;
	
	switch(com) {
	case TCXONC: {
		/* Start/stop control */
                
                switch (*(int *)data) {
                case TCOOFF:
                        return(ttioctl(tp, TIOCSTOP, data, flag));
                        
                case TCOON:
                        return(ttioctl(tp, TIOCSTART, data, flag));

                case TCIOFF:
                        if (tp->t_cc[VSTOP] != _POSIX_VDISABLE &&
                            putc(tp->t_cc[VSTOP], &tp->t_outq) == 0) {
                                TSPLTTY(s);
                                tp->t_state |= TS_TBLOCK;
                                TSPLX(s);
                                ttstart(tp);
                        }
                        return(0);
                                
                case TCION:
                        if (tp->t_cc[VSTART] != _POSIX_VDISABLE &&
                            putc(tp->t_cc[VSTART], &tp->t_outq) == 0) {
                                TSPLTTY(s);
                                tp->t_state &= ~TS_TBLOCK;
                                TSPLX(s);
                                ttstart(tp);
                        }
                        return(0);
                                
                default:
                        return(EINVAL);
                }
                break;
        }
                        
	case TCFLSH: {
		/* Flush queues */
                int flags;
                
                switch (*(int *)data) {
                case TCIFLUSH:
                        flags = FREAD;
                        break;
				
                case TCOFLUSH:
                        flags = FWRITE;
                        break;
 
                case TCIOFLUSH:
                        flags = FREAD | FWRITE;
                        break;
 
		default:
                        return(EINVAL);
                }
						
                ttyflush(tp, flags);
                break;
        }
		
		
	case TCGETA:
                /* Get tty attributes */
		termio_ptr->c_iflag = tp->t_iflag;
		termio_ptr->c_oflag = tp->t_oflag;
		if ((speed = baud_encode(tp->t_ospeed)) == -1)
			speed = B0;
		termio_ptr->c_cflag = tp->t_cflag | speed;
		termio_ptr->c_lflag = tp->t_lflag;
		if (tp->t_lflag & NOFLSH) 
			termio_ptr->c_lflag |= VNOFLSH;
		termio_ptr->c_line = tp->t_line;
		termio_ptr->c_cc[VVINTR] = tp->t_cc[VINTR];
		termio_ptr->c_cc[VVQUIT] = tp->t_cc[VQUIT];
		termio_ptr->c_cc[VVERASE] = tp->t_cc[VERASE];
		termio_ptr->c_cc[VVKILL] = tp->t_cc[VKILL];
		termio_ptr->c_cc[VVEOL2] = tp->t_cc[VEOL2];
		termio_ptr->c_cc[VVSWTCH] = tp->t_cc[VSUSP];
		if (tp->t_lflag & ICANON) {
			termio_ptr->c_cc[VVEOL] = tp->t_cc[VEOL];
			termio_ptr->c_cc[VVEOF] = tp->t_cc[VEOF];
		}
		else {
			termio_ptr->c_cc[VVMIN] = tp->t_cc[VMIN];
			termio_ptr->c_cc[VVTIME] = tp->t_cc[VTIME];
		}
		break;
		
	case TCSETAW:
	case TCSETAF:
	case TCSETA:
		/* Note that since the termio structure uses shorts and */
		/* the termios structure uses longs any flag set in the */
		/* upper half of the word will be cleared. However, this */
		/* is the correct behavior. Note that we have to move */
		/* around the NOFLSH flag. */
		term.c_iflag = termio_ptr->c_iflag;
		term.c_oflag = termio_ptr->c_oflag;
		term.c_cflag = termio_ptr->c_cflag & ~0xf;
		term.c_ispeed = term.c_ospeed =
		    baud_decode(termio_ptr->c_cflag & 0xf);
		term.c_lflag = termio_ptr->c_lflag & ~VNOFLSH;
		if (termio_ptr->c_lflag & VNOFLSH)
			term.c_lflag |= NOFLSH;

		/* Initialize the whole termios cc array since its bigger */
		for (i = 0; i < NCCS; i++) {
			term.c_cc[i] = tp->t_cc[i];
		}
		
		term.c_cc[VINTR] = termio_ptr->c_cc[VVINTR];
		term.c_cc[VQUIT] = termio_ptr->c_cc[VVQUIT];
		term.c_cc[VERASE] = termio_ptr->c_cc[VVERASE];
		term.c_cc[VKILL] = termio_ptr->c_cc[VVKILL];
		term.c_cc[VEOL2] = termio_ptr->c_cc[VVEOL2];
		term.c_cc[VSUSP] = termio_ptr->c_cc[VVSWTCH];
		if (termio_ptr->c_lflag & ICANON) {
			term.c_cc[VEOL] = termio_ptr->c_cc[VVEOL];
			term.c_cc[VEOF] = termio_ptr->c_cc[VVEOF];
		}
		else {
			term.c_cc[VMIN] = termio_ptr->c_cc[VVMIN];
			term.c_cc[VTIME] = termio_ptr->c_cc[VVTIME];
		}
 
		switch(com) {
		case TCSETAW:
			cmd = TIOCSETAW;
			break;
			
		case TCSETAF:
			cmd = TIOCSETAF;
			break;
			
		case TCSETA:
			cmd = TIOCSETA;
			break;
		}
		
                /*
                 * Check to see if the line displine is
                 * changing. If so change it then to the
                 * ioctl.
                 */
		if (termio_ptr->c_line != tp->t_line) {
			int retval;
			line = termio_ptr->c_line;
			if (retval = ttioctl(tp, TIOCSETD, &line, flag))
				return(retval);
		}

		return(ttioctl(tp, cmd, &term, flag));
		break;
 
	default:
		return(EINVAL);
	}
 
	return(0);
}

