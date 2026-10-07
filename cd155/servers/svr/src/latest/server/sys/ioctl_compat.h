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
 * $Log: ioctl_compat.h,v $
 * Revision 1.5  1994/11/18  20:40:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:25:08  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:19:19  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:42:15  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:47  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:42  cfj
 * Bump major revision number.
 * Revision 1.1.1.1  1993/05/03  17:41:34  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  14:06:56  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:43:10  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/31  14:06:35  devrcs
 * 	Changed the definition of VTDLY and friends to be different from FFDLY.
 * 	[90/10/08  17:12:04  collins]
 * 
 * Revision 1.6  90/10/07  14:51:03  devrcs
 * 	changed USE_OLD_TTY to _USE_OLD_TTY, USE_NEW_TTY to _USE_NEW_TTY
 * 	[90/10/04  09:39:07  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:40:24  gm]
 * 
 * 	Fixed TAB3 value to be unique.
 * 	[90/09/20  13:00:11  havens]
 * 
 * Revision 1.5  90/06/22  20:53:05  devrcs
 * 	No changes necessary.
 * 	[90/06/14  23:53:54  havens]
 * 
 * Revision 1.4  90/05/13  20:33:38  devrcs
 * 	Changes for gcc
 * 	[90/05/01  22:12:42  brezak]
 * 
 * 	gcc changes for osc.9
 * 	[90/03/30  16:18:45  duthie]
 * 
 * 	changes for gcc build
 * 	[90/03/28  13:59:57  duthie]
 * 
 * Revision 1.3  90/03/13  20:49:23  mbrown
 * 	Put all the rest of the delays (NL, TAB, CR, BS, VT) back
 * 	in under conditional to support older apps.
 * 	[90/03/12  13:28:33  tom]
 * 
 * 	Put some DELAY's back here which don't need to be in termios.h.
 * 	[90/02/27  15:00:12  tom]
 * 
 * 	removed NL, TAB, CR, BS, and VT delays,
 * 	moved them into termios.h as per XOPEN
 * 	[90/02/22  16:54:13  tom]
 * 
 * Revision 1.2  90/02/16  16:52:21  devrcs
 * 	Merged Robert Coren's and Rich Morris's changes
 * 	[90/02/15  14:10:37  ers]
 * 
 * $EndLog$
 */
#include <sys/ttychars.h>
#include <sys/ttydev.h>


struct tchars {
	char	t_intrc;	/* interrupt */
	char	t_quitc;	/* quit */
	char	t_startc;	/* start output */
	char	t_stopc;	/* stop output */
	char	t_eofc;		/* end-of-file */
	char	t_brkc;		/* input delimiter (like nl) */
};

struct ltchars {
	char	t_suspc;	/* stop process signal */
	char	t_dsuspc;	/* delayed stop process signal */
	char	t_rprntc;	/* reprint line */
	char	t_flushc;	/* flush output (toggles) */
	char	t_werasc;	/* word erase */
	char	t_lnextc;	/* literal next character */
};

/*
 * Structure for TIOCGETP and TIOCSETP ioctls.
 */
#ifndef _SGTTYB_
#define	_SGTTYB_
struct sgttyb {
	char	sg_ispeed;		/* input speed */
	char	sg_ospeed;		/* output speed */
	char	sg_erase;		/* erase character */
	char	sg_kill;		/* kill character */
	short	sg_flags;		/* mode flags */
};
#endif

#ifdef _USE_OLD_TTY
# undef  TIOCGETD
# define TIOCGETD	_IOR('t', 0, int)	/* get line discipline */
# undef  TIOCSETD
# define TIOCSETD	_IOW('t', 1, int)	/* set line discipline */
#else
# define OTIOCGETD	_IOR('t', 0, int)	/* get line discipline */
# define OTIOCSETD	_IOW('t', 1, int)	/* set line discipline */
#endif /* _USE_OLD_TTY */

#define	TIOCHPCL	_IO('t', 2)		/* hang up on last close */
#define	TIOCGETP	_IOR('t', 8,struct sgttyb)/* get parameters -- gtty */
#define	TIOCSETP	_IOW('t', 9,struct sgttyb)/* set parameters -- stty */
#define	TIOCSETN	_IOW('t',10,struct sgttyb)/* as above, but no flushtty*/
#define	TIOCSETC	_IOW('t',17,struct tchars)/* set special characters */
#define	TIOCGETC	_IOR('t',18,struct tchars)/* get special characters */
#define		TANDEM		0x00000001	/* send stopc on out q full */
#define		CBREAK		0x00000002	/* half-cooked mode */
#define		LCASE		0x00000004	/* simulate lower case */
#define		ECHO		0x00000008	/* echo input */
#define		CRMOD		0x00000010	/* map \r to \r\n on output */
#define		RAW		0x00000020	/* no i/o processing */
#define		ODDP		0x00000040	/* get/send odd parity */
#define		EVENP		0x00000080	/* get/send even parity */
#define		ANYP		0x000000c0	/* get any parity/send none */
#define		NLDELAY		0x00000300	/* \n delay */
#define		TBDELAY		0x00000c00	/* horizontal tab delay */
#define		XTABS		0x00000c00	/* expand tabs on output */
#define		CRDELAY		0x00003000	/* \r delay */
#define		VTDELAY		0x00004000	/* vertical tab delay */
#define		BSDELAY		0x00008000	/* \b delay */
#define		ALLDELAY	(NLDELAY|TBDELAY|CRDELAY|VTDELAY|BSDELAY)

#ifndef NL0		/* Compatability , these are in termios.h */
#define			NL0	0x00000000
#define			NL1	0x00000100	/* tty 37 */
#define			NL2	0x00000200	/* vt05 */
#define			NL3	0x00000300
#define			TAB0	0x00000000
#define			TAB1	0x00000400	/* tty 37 */
#define			TAB2	0x00000800
#define			TAB3	0x00000C00	/* expand tabs on output */
#define			CR0	0x00000000
#define			CR1	0x00001000	/* tn 300 */
#define			CR2	0x00002000	/* tty 37 */
#define			CR3	0x00003000	/* concept 100 */
#define			FF0	0x00000000
#define			FF1	0x00004000
#define			BS0	0x00000000
#define			BS1	0x00008000
#define			VT0	0x00000000
#define			VT1	0x00010000
#endif	/* NL0 */

#define		CRTBS		0x00010000	/* do backspacing for crt */
#define		PRTERA		0x00020000	/* \ ... / erase */
#define		CRTERA		0x00040000	/* " \b " to wipe out char */
#define		TILDE		0x00080000	/* hazeltine tilde kludge */
#define		MDMBUF		0x00100000	/*start/stop output on carrier*/
#define		LITOUT		0x00200000	/* literal output */
#define		TOSTOP		0x00400000	/*SIGSTOP on background output*/
#define		FLUSHO		0x00800000	/* flush output to terminal */
#define		NOHANG		0x01000000	/* no SIGHUP on carrier drop */
#define		L001000		0x02000000
#define		CRTKIL		0x04000000	/* kill line with " \b " */
#define		PASS8		0x08000000
#define		CTLECH		0x10000000	/* echo control chars as ^X */
#define		PENDIN		0x20000000	/* tp->t_rawq needs reread */
#define		DECCTQ		0x40000000	/* only ^Q starts after ^S */
#define		NOFLSH		0x80000000	/* no output flush on signal */
#define	TIOCLBIS	_IOW('t', 127, int)	/* bis local mode bits */
#define	TIOCLBIC	_IOW('t', 126, int)	/* bic local mode bits */
#define	TIOCLSET	_IOW('t', 125, int)	/* set entire local mode word */
#define	TIOCLGET	_IOR('t', 124, int)	/* get local modes */
#define		LCRTBS		(CRTBS>>16)
#define		LPRTERA		(PRTERA>>16)
#define		LCRTERA		(CRTERA>>16)
#define		LTILDE		(TILDE>>16)
#define		LMDMBUF		(MDMBUF>>16)
#define		LLITOUT		(LITOUT>>16)
#define		LTOSTOP		(TOSTOP>>16)
#define		LFLUSHO		(FLUSHO>>16)
#define		LNOHANG		(NOHANG>>16)
#define		LCRTKIL		(CRTKIL>>16)
#define		LPASS8		(PASS8>>16)
#define		LCTLECH		(CTLECH>>16)
#define		LPENDIN		(PENDIN>>16)
#define		LDECCTQ		(DECCTQ>>16)
#define		LNOFLSH		(NOFLSH>>16)
#define	TIOCSLTC	_IOW('t',117,struct ltchars)/* set local special chars*/
#define	TIOCGLTC	_IOR('t',116,struct ltchars)/* get local special chars*/
#define	OTTYDISC	0
#define	NETLDISC	1
#define	NTTYDISC	2
