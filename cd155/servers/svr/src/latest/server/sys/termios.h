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
 * $Log: termios.h,v $
 * Revision 1.4  1994/11/18  20:41:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:29:15  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:26  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:39  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:17  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:16  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:14:20  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:46:00  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10  90/10/31  14:07:15  devrcs
 * 	Change VINFO to VSTATUS and VFLUSHO to VDISCARD to match Berkeley.
 * 	[90/10/12  19:59:34  ers]
 * 
 * 	Changed the definition of VTDLY and friends to be different from FFDLY.
 * 	[90/10/08  17:12:10  collins]
 * 
 * Revision 1.9  90/10/07  14:55:08  devrcs
 * 	changed USE_OLD_TTY to _USE_OLD_TTY, USE_NEW_TTY to _USE_NEW_TTY
 * 	[90/10/04  09:39:19  rabin]
 * 
 * 	cleaned up name-space pollution reported by VSX in POSIX-Only mode
 * 	except for USE_NEW_TTY
 * 	[90/10/04  07:35:26  rabin]
 * 
 * 	protected #define _POSIX_VDISABLE for kernel use
 * 	[90/10/02  17:21:30  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:46:43  gm]
 * 
 * 	fix previous change to eliminate compiler warning
 * 	[90/09/21  09:51:21  garyf]
 * 
 * 	Fixed bug number 966.  The definition of tab3 was incorrect. Changed it
 * 	from 0x0800 to 0x0c00.
 * 	[90/09/19  18:09:59  havens]
 * 
 * Revision 1.8  90/08/09  13:29:12  devrcs
 * 	Added nokerninfo flag which inhibits the generation of process status
 * 	information when the info character is typed.
 * 	[90/07/24  14:02:54  havens]
 * 
 * Revision 1.7  90/07/05  23:14:42  devrcs
 * 	Added in ifdef's and undef's to force the new speed values to be defined
 * 	if this file is used.  Programs which only use ioctl.h or termio.h use
 * 	the old speed values.  If the POSIX functions are used and this file is
 * 	included, then the old compatiblity structures such as sgtty and termio
 * 	cannot be used.
 * 	[90/06/28  14:34:45  havens]
 * 
 * 	Reset the NOFLSH flag to its old value for BSD compatibility.
 * 	[90/06/14  23:04:07  havens]
 * 
 * 	Assigned values to the undefined flags used for SVID support.  Since
 * 	all of the SVID flages must be in the lower 16 bits, the value of
 * 	OXTABS and ONOEOT was changed.
 * 	[90/06/13  12:58:56  havens]
 * 
 * Revision 1.6  90/06/22  20:54:23  devrcs
 * 	Reset the NOFLSH flag to its old value for BSD compatibility.
 * 	[90/06/14  23:04:07  havens]
 * 
 * 	Assigned values to the undefined flags used for SVID support.  Since
 * 	all of the SVID flages must be in the lower 16 bits, the value of
 * 	OXTABS and ONOEOT was changed.
 * 	[90/06/13  12:58:56  havens]
 * 
 * Revision 1.5  90/05/13  20:33:56  devrcs
 * 	Changes for gcc for osc.10
 * 	[90/05/02  19:43:23  brezak]
 * 
 * 	Changes for gcc
 * 	[90/05/01  21:49:54  brezak]
 * 
 * Revision 1.4  90/04/27  19:27:22  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/18  11:47:31  tom]
 * 
 * Revision 1.3  90/03/13  20:50:02  mbrown
 * 	Added dummy value for IUCLC to allow libcur.a to compile.
 * 	[90/03/12  14:42:34  tom]
 * 
 * 	Corrected spelling of TCSAFLUSH.
 * 	[90/03/08  13:57:57  tom]
 * 
 * 	Put DELAYS's (under OSF_SOURCE) back into ioctl_compat.h,
 * 	they don't need to be here, and some things will want them in ioctl.
 * 	[90/02/27  15:00:22  tom]
 * 
 * 	changed cc_t to unsigned char to agree w/ kernel.
 * 	[90/02/23  13:14:00  tom]
 * 
 * 	added correct values for NL, BS, CR, VT delay.
 * 	[90/02/22  16:55:16  tom]
 * 
 * 	AIX merge - Make this file really conform to POSIX and X/OPEN
 * 	[90/02/21  15:27:49  tom]
 * 
 * Revision 1.2  90/02/16  16:52:46  devrcs
 * 	Placed new B<speed> defines under !USE_OLD_TTY.
 * 	[90/02/15  15:32:37  ers]
 * 
 * 	POSIX tty support.
 * 	[90/02/15  15:05:14  ers]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)termios.h	7.6 (Berkeley) 11/20/89
 */

/*
 *  termios structure
 */
#ifndef _SYS_TERMIOS_H_
#define _SYS_TERMIOS_H_

#include <standards.h>

#ifdef _POSIX_SOURCE

typedef unsigned int    tcflag_t;
typedef unsigned char   cc_t;
typedef int    speed_t;


/* 
 * Special Control Characters 
 *
 * Index into c_cc[] character array.
 *
 *	Name	     Subscript	Enabled by 
 */
#define	VEOF		0	/* ICANON */
#define	VEOL		1	/* ICANON */

#ifdef _OSF_SOURCE
#define	VEOL2		2	/* ICANON */
#endif

#define	VERASE		3	/* ICANON */

#ifdef _OSF_SOURCE
#define VWERASE 	4	/* ICANON */
#endif

#define VKILL		5	/* ICANON */

#ifdef _OSF_SOURCE
#define	VREPRINT 	6	/* ICANON */
#endif

/*			7	   spare 1 */

#define VINTR		8	/* ISIG */
#define VQUIT		9	/* ISIG */
#define VSUSP		10	/* ISIG */

#ifdef _OSF_SOURCE
#define VDSUSP		11	/* ISIG */
#endif

#define VSTART		12	/* IXON, IXOFF */
#define VSTOP		13	/* IXON, IXOFF */

#ifdef _OSF_SOURCE
#define	VLNEXT		14	/* IEXTEN */
#define	VDISCARD	15	/* IEXTEN */
#define	VFLUSH		VDISCARD /* for sun */
#endif

#define VMIN		16	/* !ICANON */
#define VTIME		17	/* !ICANON */

#ifdef _OSF_SOURCE
#define VSTATUS		18	/* ISIG */
#endif

/*			19	   spare 2 */

#define	NCCS		20

/*
 * Ioctl control packet
 */
struct termios {
	tcflag_t	c_iflag;	/* input flags */
	tcflag_t	c_oflag;	/* output flags */
	tcflag_t	c_cflag;	/* control flags */
	tcflag_t	c_lflag;	/* local flags */
	cc_t		c_cc[NCCS];	/* control chars */
	long		c_ispeed;	/* input speed */
	long		c_ospeed;	/* output speed */
};

#ifdef _KERNEL
#ifndef _POSIX_VDISABLE
#define _POSIX_VDISABLE	((unsigned char)'\377')
#endif	/* _POSIX_VDISABLE */
#endif	/* _KERNEL */

#ifdef _NO_PROTO
extern speed_t cfgetospeed();
extern speed_t cfsetospeed();
extern speed_t cfgetispeed();
extern speed_t cfsetispeed();
extern int tcgetattr();
extern int tcsetattr();
extern int tcsendbreak();
extern int tcdrain();
extern int tcflush();
extern int tcflow();

#else                           /* use POSIX required prototypes */

extern speed_t cfgetospeed(struct termios *);
extern speed_t cfsetospeed(struct termios *, speed_t);
extern speed_t cfgetispeed(struct termios *);
extern speed_t cfsetispeed(struct termios *, speed_t);
extern int tcgetattr(int , struct termios *);
extern int tcsetattr(int , int , struct termios *);
extern int tcsendbreak(int , int );
extern int tcdrain(int );
extern int tcflush(int , int );
extern int tcflow(int , int );
#endif /* _NO_PROTO */

#ifdef _KERNEL
#define CCEQ(val, c)	(c == val ? val != _POSIX_VDISABLE : 0)
#endif

/*
 * Input flags - software input processing
 */
#define	IGNBRK		0x00000001	/* ignore BREAK condition */
#define	BRKINT		0x00000002	/* map BREAK to SIGINTR */
#define	IGNPAR		0x00000004	/* ignore (discard) parity errors */
#define	PARMRK		0x00000008	/* mark parity and framing errors */
#define	INPCK		0x00000010	/* disable checking of parity errors */
#define	ISTRIP		0x00000020	/* strip 8th bit off chars */
#define	INLCR		0x00000040	/* map NL into CR */
#define	IGNCR		0x00000080	/* ignore CR */
#define	ICRNL		0x00000100	/* map CR to NL (ala CRMOD) */
#define	IXON		0x00000200	/* enable output flow control */
#define	IXOFF		0x00000400	/* enable input flow control */
#ifdef _XOPEN_SOURCE
#define	IXANY		0x00000800	/* any char will restart after stop */
#define IUCLC		0x00001000	/* DUMMY VALUE Map upper to lower */
					/* case on input */
#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE
#define	IFLOW		IXON		/* enable output flow control */
#define	ITANDEM		IXOFF		/* enable input flow control */
#define IMAXBEL		0x00002000	/* ring bell on input queue full */
#endif /* _OSF_SOURCE */

/*
 * Output flags - software output processing
 */
#define	OPOST		0x00000001	/* enable following output processing */

#ifdef _XOPEN_SOURCE

#define ONLCR		0x00000002	/* map NL to CR-NL (ala CRMOD) */
#define OLCUC           0x00000004	/* Map lower case to upper on output */
#define OCRNL           0x00000008	/* Map CR to NL on output */
#define ONOCR           0x00000010	/* No CR output at column 0 */
#define ONLRET          0x00000020	/* NL performs CR function */
#define OFILL           0x00000040	/* Use fill characters for delay */
#define OFDEL           0x00000080	/* fill is DEL, else NUL */

#define		NLDLY		0x00000300	/* \n delay */
#define			NL0	0x00000000
#define			NL1	0x00000100	/* tty 37 */
#define			NL2	0x00000200	/* vt05 */
#define			NL3	0x00000300
#define		TABDLY		0x00000c00	/* horizontal tab delay */
#define			TAB0	0x00000000
#define			TAB1	0x00000400	/* tty 37 */
#define			TAB2	0x00000800
#define			TAB3	0x00000C00	/* expand tabs on output */
#define		CRDLY		0x00003000	/* \r delay */
#define			CR0	0x00000000
#define			CR1	0x00001000	/* tn 300 */
#define			CR2	0x00002000	/* tty 37 */
#define			CR3	0x00003000	/* concept 100 */
#define 	FFDLY           0x00004000	/* Form feed delay */
#define 		FF0	0x00000000
#define			FF1	0x00004000
#define		BSDLY		0x00008000	/* \b delay */
#define			BS0	0x00000000
#define			BS1	0x00008000
#define		VTDLY		0x00010000	/* vertical tab delay */
#define			VT0	0x00000000
#define			VT1	0x00010000

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE
#define ONLCRNL		ONLCR
#define OXTABS		0x00040000	/* expand tabs to spaces */
#define ONOEOT		0x00080000	/* discard EOT's (^D) on output) */
#endif /* _OSF_SOURCE */

/*
 * Control flags - hardware control of terminal
 */
#define CSIZE		0x00000300	/* character size mask */
#define     CS5		    0x00000000	    /* 5 bits (pseudo) */
#define     CS6		    0x00000100	    /* 6 bits */
#define     CS7		    0x00000200	    /* 7 bits */
#define     CS8		    0x00000300	    /* 8 bits */
#define CSTOPB		0x00000400	/* send 2 stop bits */
#define CREAD		0x00000800	/* enable receiver */
#define PARENB		0x00001000	/* parity enable */
#define PARODD		0x00002000	/* odd parity, else even */
#define HUPCL		0x00004000	/* hang up on last close */
#define CLOCAL		0x00008000	/* ignore modem status lines */

#ifdef _OSF_SOURCE
#define CRTSCTS		0x00010000	/* RTS/CTS flow control */
#endif /* _OSF_SOURCE */


/* 
 * "Local" flags - dumping ground for other state
 *
 * Warning: some flags in this structure begin with
 * the letter "I" and look like they belong in the
 * input flag.
 */

#define	ECHOE		0x00000002	/* visually erase chars */
#define	ECHOK		0x00000004	/* echo NL after line kill */
#define ECHO		0x00000008	/* enable echoing */
#define	ECHONL		0x00000010	/* echo NL even if ECHO is off */
#define	ISIG		0x00000080	/* enable signals INTR, QUIT, [D]SUSP */
#define	ICANON		0x00000100	/* canonicalize input lines */
#define	IEXTEN		0x00000400	/* enable FLUSHO and LNEXT */
#define	NOFLSH		0x80000000	/* don't flush after interrupt */
#define TOSTOP		0x00400000	/* stop background jobs from output */

#ifdef _XOPEN_SOURCE
#define XCASE		0x00004000	/* Cononical upper/lower presentation*/
#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE
#define	ECHOKE		0x00000001	/* visual erase for line kill */
#define	ECHOPRT		0x00000020	/* visual erase mode for hardcopy */
#define ECHOCTL  	0x00000040	/* echo control chars as ^(Char) */
#define ALTWERASE	0x00000200	/* use alternate WERASE algorithm */
#define	MDMBUF		0x00100000	/* flow control output via Carrier */
#define FLUSHO		0x00800000	/* output being flushed (state) */
#define	NOHANG		0x01000000	/* XXX this should go away */
#define PENDIN		0x20000000	/* retype pending input (state) */
#define NOKERNINFO      0x40000000      /* Disable printing kernel info */
#endif /* _OSF_SOURCE */


/* 
 * Commands passed to tcsetattr() for setting the termios structure.
 */
#define	TCSANOW		0		/* make change immediate */
#define	TCSADRAIN	1		/* drain output, then change */
#define	TCSAFLUSH	2		/* drain output, flush input */

/* values for the queue_selector argument to tcflush() */

#define TCIFLUSH        0
#define TCOFLUSH        1
#define TCIOFLUSH       2

/* values for the action argument to tcflow() */

#define TCOOFF          0
#define TCOON           1
#define TCIOFF          2
#define TCION           3

#if !defined(_SYS_TERMIO_H_)
/*
 * If termios functionality is used and termio is not used then always
 * specify _USE_NEW_TTY. This is a bit of a hack, but ioctl.h defines 
 * _USE_OLD_TTY by default which defines the wrong speed values.
 */
#undef _USE_OLD_TTY
#define _USE_NEW_TTY

#undef B0
#undef B50
#undef B75
#undef B110
#undef B134
#undef B150
#undef B200
#undef B300
#undef B600
#undef B1200
#undef B1800
#undef B2400
#undef B4800
#undef B9600
#undef B19200
#undef B38400
#undef EXTA
#undef EXTB
#endif	/* !defined(_SYS_TERMIO_H_) */

#ifndef	_USE_OLD_TTY
/*
 * Standard speeds
 */
#define B0	0
#define B50	50
#define B75	75
#define B110	110
#define B134	134
#define B150	150
#define B200	200
#define B300	300
#define B600	600
#define B1200	1200
#define	B1800	1800
#define B2400	2400
#define B4800	4800
#define B9600	9600
#define B19200	19200
#define B38400	38400
#define EXTA	19200
#define EXTB	38400
#endif	/* ! _USE_OLD_TTY */
/*
 * END OF PROTECTED INCLUDE.
 */


#endif /* _POSIX_SOURCE */

#endif /* _SYS_TERMIOS_H_ */

#ifdef _OSF_SOURCE
#include <sys/ttydefaults.h>
#endif
