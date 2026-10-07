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
 * $Log: if_slvar.h,v $
 * Revision 1.5  1994/11/18  20:33:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:07:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:28:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:41  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:20  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:01  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:37  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1992/03/09  14:56:30  durriya
 * 	initial check-in from grV3.5
 * 	Revision 3.0  92/01/02  15:20:19  david
 * 	added for SLIP driver
 *
 * 	Revision 1.7  90/10/07  14:31:59  devrcs
 * 		Added EndLog Marker.
 * 		[90/09/28  11:10:26  gm]
 *
 * 	Revision 1.6  90/07/27  08:59:02  devrcs
 * 		Update to BSD Reno release.
 * 		[90/07/19  16:35:28  tmt]
 *
 * 	Revision 1.5  90/04/27  19:12:24  devrcs
 * 		Updated version with compression. Move ioctl's to ioctl.h.
 * 		[90/04/20  12:26:49  tmt]
 *
 * 	Revision 1.4  90/01/18  08:43:35  gm
 * 		OSF/1 "one" snapshot revision.
 * 		[90/01/02  12:00:00  tmt]
 *
 * 		- Base is BSD 4.4 (Alpha) networking.
 * 		- Encore multiprocessing merged in with some structural
 * 		  modifications to support flexible configuration.
 * 		- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 		  lock testing under Unix, thread or software interrupt netisr's,
 * 		  locking and/or spl synchronization, single or multiple CPUs.
 * 		[89/12/20  12:00:00  tmt]
 *
 * 	Revision 1.3  90/01/02  20:10:50  gm
 * 		Fixes for first snapshot.
 *
 * 	Revision 1.2  89/12/26  09:44:51  gm
 * 		New networking code from BSD.
 * 		[89/12/16            tmt]
 *
 */

/*
 * Definitions for SLIP interface data structures
 * 
 * (this exists so programs like slstats can get at the definition
 *  of sl_softc.)
 *
 *	Base:	if_slvar.h	7.4 (Berkeley) 1/20/90
 *		from if_slvar.h,v 1.3 89/05/31 02:25:18 van Exp
 */

struct sl_softc {
	struct	ifnet sc_if;		/* network-visible interface */
	struct	ifqueue sc_fastq;	/* interactive output queue */
	struct	tty *sc_ttyp;		/* pointer to tty structure */
	u_char	*sc_mp;			/* pointer to next available buf char */
	u_char	*sc_ep;			/* pointer to last available buf char */
	u_char	*sc_buf;		/* input buffer */
	caddr_t	sc_cluster;		/* cluster page holding above */
	u_int	sc_flags;		/* see below */
	u_int	sc_escape;	/* =1 if last char input was FRAME_ESCAPE */
	u_int	sc_bytessent;
	u_int	sc_bytesrcvd;
	long	sc_lasttime;		/* last time a char arrived */
	long	sc_starttime;		/* last time a char arrived */
	long	sc_abortcount;		/* number of abort escape chars */
	struct	slcompress sc_comp;	/* tcp compression data */
};

/* visible flags */
#define	SC_COMPRESS	0x0002		/* compress TCP traffic */
#define	SC_NOICMP	0x0004		/* supress ICMP traffic */
#define	SC_AUTOCOMP	0x0008		/* auto-enable TCP compression */
/* internal flags (should be separate) */
#define	SC_ABORT	0x10000		/* have been sent an abort request */
