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
 * $Log: stream.h,v $
 * Revision 1.4  1994/11/18  20:41:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:28:10  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:28  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:06  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:40  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:42  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/07/29  08:22:43  rabii
 * 	Include stdarg.h instead of varags.h for ANSI-C compilers (pjg).
 *
 * Revision 2.3  91/10/17  11:19:01  sjs
 * fixed RCS messages
 * 
 * Revision 2.2  91/10/14  13:08:55  sjs
 * Check in
 * 
 * Revision 3.0  91/09/27  11:52:30  emcmanus
 * Installed for streams.
 * 
 * Revision 1.8.4.2  91/03/15  17:51:35  tmt
 * 	Fix spelling of NDELON mode.
 * 	Rearrange struct stradm declaration for prototypes.
 * 	[91/03/13  19:18:40  tmt]
 * 
 * Revision 1.8  90/10/31  14:07:01  devrcs
 * 	Post-file-rearrangement pathnames.
 * 	[90/10/11  08:34:04  tmt]
 * 
 * 	Add phony u_error for SVR3.2 compat. Add canenable(q).
 * 	Cleanup includes, re-defines, etc. Checkin prior to file rearrangement.
 * 	[90/10/08  10:03:50  tmt]
 * 
 * Revision 1.7  90/10/07  14:54:22  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:45:28  gm]
 * 
 * 	Rearrange str_config structure. Make sa_flags visible to cfgmgr.
 * 	Add extern for clone device major/minor.
 * 	[90/09/26  10:00:04  tmt]
 * 
 * 	undef cpp tokens before defining them to eliminate complaints during
 * 	build process.
 * 	[90/09/23  17:01:51  jeffc]
 * 
 * Revision 1.6  90/09/23  16:00:18  devrcs
 * 	Added strmod_del() stud.
 * 	Removed reference to defunct include file sys/configure.h
 * 	Moved str_config_t out of ifdef KERNEL
 * 	[90/09/17  14:08:17  knight]
 * 
 * 	Dymanic configuration integration
 * 	[90/09/13  14:24:41  knight]
 * 
 * 	Delete inappropriate bcopy, bzero, timeout declarations.
 * 	Delete inappropriate (and wrong) varargs.
 * 	[90/09/05  17:35:23  tmt]
 * 
 * Revision 1.5  90/08/09  13:29:06  devrcs
 * 	Fix the strcmp, strcpy problem.
 * 	[90/08/04  10:18:57  tmt]
 * 
 * 	Rearrange q_other for SVID compliance.
 * 	Move msgb.b_attr to MSG_KERNEL_FIELDS.
 * 	[90/07/26  10:50:50  tmt]
 * 
 * Revision 1.4  90/07/17  11:42:32  devrcs
 * 	Add definition of msgb under SEC_BASE.
 * 	[90/07/10  22:03:30  seiden]
 * 
 * 	Replaced LONG_SIGN_BIT definition with machine-independent one.
 * 	Revisited Message types during upgrade to V.3.2 STREAMS.
 * 	Unified M_IOCTL, M_COPYIN, M_COPYOUT, and M_IOCDATA message type.
 * 	[90/07/05  17:14:30  nolting]
 * 
 * 	Replaced LONG_SIGN_BIT definition with machine-independent one.
 * 	Revisited Message types during upgrade to V.3.2 STREAMS.
 * 	Unified M_IOCTL, M_COPYIN, M_COPYOUT, and M_IOCDATA message type.
 * 	[90/07/05  17:14:30  nolting]
 * 
 * Revision 1.3  90/07/05  23:14:38  devrcs
 * 	... and made it valid only in _KERNEL space.
 * 	[90/06/28  16:25:14  nolting]
 * 
 * 	Moved string substitute definitions from pse/str_stream.h to here,
 * 	as part of the MENTAT_SPECIAL group.
 * 	[90/06/28  16:23:14  nolting]
 * 
 * Revision 1.2  90/06/29  13:54:01  devrcs
 * 	Added comment to queue_conflict (usage in pse/str_stream.h).
 * 	[90/06/25  09:31:05  nolting]
 * 
 * 	Introduced standard STREAMS include file.
 * 	[90/06/25  07:35:50  nolting]
 * 
 * $EndLog$
 */
/** Copyright (c) 1988  Mentat Inc. */

#ifndef	_SYS_STREAM_H
#define	_SYS_STREAM_H

/*
 *	In order to reduce include file problems to a minimum, we include
 *	the necessary set here. I.e. the porting effort should consist of
 *	removing all includes from outside the streams area.
 */

#include <sys/param.h>
#include <sys/errno.h>
#include <sys/ioctl.h>

#ifdef	_KERNEL
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#ifdef __STDC__
#include <machine/stdarg.h>
#else
#include <varargs.h>
#endif

/*
 *	Resolution of name conflicts...
 *
 *	The method is to redefine names with "streams_"+"name".
 *	We can neither change the AT&T STREAMS spec, nor do we want to
 *	change the OS environment. So we have to intercept those occurrences
 *	using the C preprocessor. Because of this, it is imperative that
 *	STREAMS header files be included LAST in files that care.
 *
 *	Sometimes, we need this just for the purpose of solving name conflicts,
 *	sometimes to provide the correct functionality, sometimes to make
 *	things MP-safe (all of these are not mutually exclusive). In any
 *	case, however, these definitions are intentionally not depending
 *	on preprocessor variables (like MULTI_CPU) in order to maintain
 *	binary compatibility for modules and drivers.
 *
 *	The routines for this are found in str_env.c.
 */

#undef queue
#undef queue_t
#undef time
#undef lbolt
#undef timeout
#undef untimeout
#undef sleep
#undef malloc
#undef mfree

#define queue   	streams_queue
#define queue_t 	streams_queue_t
extern	time_t		streams_time();
#define time		streams_time()	
extern	time_t		streams_lbolt();
#define	lbolt		streams_lbolt()
#define timeout		streams_timeout
#define untimeout	streams_untimeout
#define sleep		streams_sleep

#define	mfree		streams_mfree
#define	malloc		streams_malloc

/*
 *	End of name conflicts
 */

#endif	/* _KERNEL */

/* Glue for Mentat specials */
#define LONG_SIGN_BIT		((long) ~((~((unsigned long)0)) >> 1))

#define	MS_TO_TICKS(ms)		((ms*CLOCKS_PER_SEC)/1000)
#define	TICKS_TO_MS(ticks)	(((ticks)*1000)/CLOCKS_PER_SEC)

typedef	char		* USERP;
#define	nilp(t)		((t *)0)
#define	nil(t)		((t)0)
typedef	int		(*pfi_t)();
typedef	pfi_t		(*pfpfi_t)();
#define reg		register

#define	noop
#define	fallthru

#define	U8(x)		((unsigned char)(x))

/* XXX This is a job for sys/types.h! */
typedef short		i16;
typedef int		i32;
typedef	unsigned char	u8;
typedef	unsigned short	u16;
typedef unsigned int	u32;

#define	OK_32PTR(addr)	TRUE

#define LE16_TO_U16(p)		(*((u16 *) (p)))
#define LE32_TO_U32(p)		(*((u32 *) (p)))
#define U16_TO_LE16(u,p)	(*((u16 *) (p)) = (u16) (u))
#define U32_TO_LE32(u,p)	(*((u32 *) (p)) = (u32) (u))
/* End glue */


struct streamtab {
	struct qinit	* st_rdinit;	/* defines read QUEUE */
	struct qinit	* st_wrinit;	/* defines write QUEUE */
	struct qinit	* st_muxrinit;	/* for multiplexing drivers only */
	struct qinit	* st_muxwinit;	/* ditto */
};

#ifndef	QUEUE_KERNEL_FIELDS
#define QUEUE_KERNEL_FIELDS
#endif

struct queue {
	struct qinit *	q_qinfo;	/* procedures and limits for queue */
	struct msgb *	q_first;	/* head of message queue */
	struct msgb *	q_last;		/* tail of message queue */
	struct queue *	q_next;		/* next QUEUE in Stream */
	struct queue *	q_link;		/* link to scheduling queue */
	caddr_t		q_ptr;		/* to private data structure */
	ushort		q_count;	/* weighted count of characters on q */
	ushort		q_flag;		/* QUEUE state */
	short		q_minpsz;	/* min packet size accepted */
	short		q_maxpsz;	/* max packet size accepted */
	ushort		q_hiwat;	/* high water mark, for flow control */
	ushort		q_lowat;	/* low water mark */
	struct queue *	q_other;	/* pointer to other Q in queue pair */
	QUEUE_KERNEL_FIELDS
};

typedef	struct	queue	queue_t;

/* Q state defines */
#define	F_Q_IS_WRITE_Q		0x01
#define	F_Q_DISABLED		0x02
#define	F_Q_FULL		0x04
#define	F_Q_TO_SCHEDULE		0x08
#define	F_Q_PUT_STOPPED		0x10
#define	F_Q_WELDED		0x20
#define	F_Q_SEQUENT_SYNCH	0x40

struct	qinit {
	int	(*qi_putp)();		/* put procedure */
	int	(*qi_srvp)();		/* service procedure */
	int	(*qi_qopen)();		/* called on each open or a push */
	int	(*qi_qclose)();		/* called on last close or a pop */
	int	(*qi_qadmin)();		/* reserved for future use */
	struct module_info * qi_minfo;	/* information structure */
	struct module_stat * qi_mstat;	/* statistics structure - optional */
};

struct	module_info {
	unsigned short	mi_idnum;	/* module ID number */
	char	* 	mi_idname;	/* module name */
	short		mi_minpsz;	/* min packet size, for developer use */
	short		mi_maxpsz;	/* max packet size, for developer use */
	short		mi_hiwat;	/* hi-water mark, for flow control */
	short		mi_lowat;	/* lo-water mark, for flow control */
};

#ifndef MSG_KERNEL_FIELDS
#define MSG_KERNEL_FIELDS
#endif

/* message block */
struct	msgb {
	struct msgb *	b_next;		/* next message on queue */
	struct msgb *	b_prev;		/* previous message on queue */
	struct msgb *	b_cont;		/* next message block of message */
	unsigned char *	b_rptr;		/* first unread data byte in buffer */
	unsigned char *	b_wptr;		/* first unwritten data byte */
	struct datab *	b_datap;	/* data block */
	MSG_KERNEL_FIELDS
};
typedef	struct msgb	mblk_t;

/* data descriptor */
struct	datab {
	struct datab *	db_freep;	/* used internally */
	unsigned char *	db_base;	/* first byte of buffer */
	unsigned char *	db_lim;		/* last byte+1 of buffer */
	unsigned char	db_ref;		/* count of messages pointing to block*/
	unsigned char	db_type;	/* message type */
	unsigned char	db_class;	/* used internally */
	unsigned char	db_pad[1];	/* Take to a quad word boundary */
};
typedef	struct datab	dblk_t;

/* Used in M_IOCTL mblks to muxes (ioc_cmd I_LINK) */
struct	linkblk {
	queue_t	* l_qtop;	/* lowest level write queue of upper stream */
	queue_t	* l_qbot;	/* highest level write queue of lower stream */
	int	l_index;	/* system-unique index for lower stream */
};


/*
 * Structure of M_IOCTL, M_COPYIN, M_COPYOUT and M_IOCDATA messages
 *
 * Note that these structures must be interchangeable!
 * That means: sizes must be identical, and the first fields,
 * as well as the final private pointer must match.
 *
 * As defined below, that works as long as sizeof(caddr_t) == sizeof(long)
 * XXX see sys/types.h comment above.
 */

struct	iocblk {
	int		ioc_cmd;	/* ioctl command type */
	uid_t	 	ioc_uid;	/* effective uid of user */
	gid_t		ioc_gid;	/* effective gid of user */
	unsigned int	ioc_id;		/* ioctl id */

	ulong		ioc_count;	/* count of bytes in data field */
	int		ioc_error;	/* error code */
	int		ioc_rval;	/* return value */
	caddr_t		ioc_pad1;	/* padding to maintain size */
};

struct copyreq {
	int		oq_cmd;		/* command type == ioc_cmd */
	uid_t		oq_uid;		/* effective uid == ioc_uid */
	gid_t		oq_gid;		/* effective gid == ioc_gid */
	uint		oq_id;		/* ioctl id == ioc_id */

	caddr_t		oq_addr;	/* address to cpy data to/from */
	uint		oq_size;	/* number of bytes to copy */
	int		oq_flag;	/* reserved */
	mblk_t *	oq_private;	/* module's private state info */
};

struct copyresp {
	int		cp_cmd;		/* command type == ioc_cmd */
	uid_t		cp_uid;		/* effective uid == ioc_uid */
	gid_t		cp_gid;		/* effective gid == ioc_gid */
	uint		cp_id;		/* ioctl id == ioc_id */

	long		cp_rval;	/* 0 = success */
	uint		cp_pad1;	/* reserved */
	uint		cp_pad2;	/* reserved */
	mblk_t *	cp_private;	/* module's private state info */
};

#define TRANSPARENT	0xffff8000	/* special value for ioc_count! */

/* Message types */
#define	QNORM		0
#define	M_DATA		0x0000	/* Ordinary data */
#define	M_PROTO		0x0001	/* Internal control info and data */
#define	M_BREAK		0x0010	/* Request a driver to send a break */
#define	M_PASSFP	0x0011	/* Used to pass a file pointer */
#define	M_SIG		0x0013	/* Requests a signal to be sent */
#define	M_DELAY		0x0014	/* Request a real-time delay */
#define	M_CTL		0x0015	/* For inter-module communication */
#define	M_IOCTL		0x0016	/* Used internally for I_STR requests */
#define	M_SETOPTS	0x0020	/* Alters characteristics of stream head */

/* Priority messages types */
#define	QPCTL		0x0080
#define	M_IOCACK	0x0081	/* Positive ack of previous M_IOCTL */
#define	M_IOCNAK	0x0082	/* Previous M_IOCTL failed */
#define	M_PCPROTO	0x0083	/* Same as M_PROTO except for priority*/
#define	M_PCSIG		0x0084	/* Priority signal */
#define	M_FLUSH		0x0086	/* Requests modules to flush queues */
#define	M_STOP		0x0087	/* Request drivers to stop output */
#define	M_START		0x0088	/* Request drivers to start output */
#define	M_HANGUP	0x0089	/* Driver can no longer produce data */
#define	M_ERROR		0x008a	/* Reports downstream error condition */
#define	M_READ          0x008b  /* Reports client read at stream head */
#define	M_HPDATA        0x008c  /* PSE-private: high priority data */
#define M_COPYIN	0x008d	/* module's request to perform copyin */
#define M_COPYOUT	0x008e	/* module's request to perform copyout */
#define M_IOCDATA	0x008f	/* response to M_COPYIN, M_COPYOUT */
#define	FLUSHALL	0x0001
#define	FLUSHDATA	0x0000
#define	FLUSH_CAN_CLOSE	0x8000


/* structure contained in an M_SETOPTS message block */
struct	stroptions {
	short		so_flags;	/* options to set */
	short		so_readopt;	/* read option */
	unsigned short	so_wroff;	/* write offset */
	short		so_minpsz;	/* minimum read packet size */
	short		so_maxpsz;	/* maximum read packet size */
	unsigned short	so_hiwat;	/* read queue high-water mark */
	unsigned short	so_lowat;	/* read queue low-water mark */
};

/* definitions for so_flags field */
#define	SO_ALL		0xffff	/* Update all options */
#define	SO_READOPT	0x0001	/* Set the read mode */
#define	SO_WROFF	0x0002	/* Insert an offset in write M_DATA mblks */
#define	SO_MINPSZ	0x0004	/* Change the min packet size on sth rq */
#define	SO_MAXPSZ	0x0008	/* Change the max packet size on sth rq */
#define	SO_HIWAT	0x0010	/* Change the high water mark on sth rq */
#define	SO_LOWAT	0x0020	/* Change the low water mark */
#define	SO_MREADON      0x0040  /* Request M_READ messages */
#define	SO_MREADOFF     0x0080  /* Don't gen M_READ messages */
#define	SO_ISTTY	0x0100	/* Act as controlling tty */
#define SO_ISNTTY	0x0200	/* Don't act as controlling tty */
#define SO_NDELON	0x0400	/* Apply TTY semantics on ONDELAY */
#define SO_NDELOFF	0x0800	/* Apply STREAMS semantics on ONDELAY */

/* Buffer Allocation Priority */
#define	BPRI_LO		1
#define	BPRI_MED	2
#define	BPRI_HI		3

#ifndef	INFPSZ
#define	INFPSZ		-1
#endif

/** Test whether message is a data message */
#define	datamsg(type)	((type) == M_DATA || (type) == M_PROTO || (type) == M_PCPROTO)

/** Re-allow a queue to be scheduled for service */
#define	enableok(q)	((q)->q_flag &= ~F_Q_DISABLED)

/** Prevent a queue from being scheduled */
#define	noenable(q)	((q)->q_flag |= F_Q_DISABLED)

/** Can a queue be enabled? */
#define	canenable(q)	(((q)->q_flag & F_Q_DISABLED) == 0)

/** Get pointer to the mate queue */
#define	OTHERQ(q)	((q)->q_other)

/** Get pointer to the read queue, assumes 'q' is a write queue ptr */
#define	RD(q)	((q)->q_other)

/** Get pointer to the write queue, assumes 'q' is a read queue ptr */
#define	WR(q)	((q)->q_other)

#if	defined(_KERNEL) && !defined(SEC_BASE)
/** XXX provide u.u_error for SVR3.2 drivers */
/** XXX note u_spare[0] used by pse_ioctl */
#define u_error		u_spare[1]
#endif

#define	OPENFAIL	(-1)
#define	CLONEOPEN	0x2
#define	MODOPEN		0x1

#define	NSTREVENT	20
#define	STRMSGSZ	4096
#define	STRCTLSZ	1024
#define STRLOFRAC	80
#define	STRMEDFRAC	90

#ifndef MAXBSIZE
#define	MAXBSIZE	4096
#endif

#define FMNAMESZ		8

/*
 * STREAMS configuration entry point in/out data structures
 */
#define OSF_STREAMS_CONFIG_10   0x04026019      /* OSF/1 str_config_t version */

typedef struct str_config {
        uint    sc_version;
        uint    sc_sa_flags;
        char    sc_sa_name[FMNAMESZ+1];
        dev_t   sc_devnum;
} str_config_t;

/*
 * Values for sa_flags (str_config and streamadm)
 */
#define	STR_TYPE_MASK	0x00000003
#define	STR_IS_DEVICE	0x00000001	/* device */
#define	STR_IS_MODULE	0x00000002	/* module */

/*
 *	Definitions for initialization
 */

#define	OSF_STREAMS_10	0x04026019	/* magic number for OSF/1.0 version */

/*
 * Synchronization level codes.
 * These are supplied to fmodsw_install and dmodsw_install and stored
 * in the appropriate tables.  sth_osr_open and sth_ipush use these to
 * set up synch queue subordination for new devices and modules.
 */

struct streamadm {
	uint	sa_version;
	uint	sa_flags;
	char	sa_name[FMNAMESZ+1];
	caddr_t	sa_ttys;
	uint	sa_sync_level;
	caddr_t	sa_sync_info;
};

#define	SQLVL_DEFAULT		0
#define	SQLVL_GLOBAL		1
#define	SQLVL_ELSEWHERE		2
#define	SQLVL_MODULE		3
#define	SQLVL_QUEUEPAIR		4
#define	SQLVL_QUEUE		5
#define	SQLVL_SEQUENT		6

#ifdef	_KERNEL

extern dev_t	clonedev;	/* Clone device major(/minor) */

#ifdef _NO_PROTO

extern	mblk_t *	allocb();
extern	mblk_t *	allocbi();	/* OSF add-on */
extern	queue_t *	allocq();
extern	int		adjmsg();
extern	queue_t *	backq();
extern	int		bufcall();
extern	int		canput();
extern	mblk_t *	copyb();
extern	mblk_t *	copymsg();
extern	mblk_t *	dupb();
extern	mblk_t *	dupmsg();
extern	int		flushq();
extern	int		freeb();
extern	int		freemsg();
extern	int		freeq();
extern	mblk_t *	getq();
extern	int		insq();
extern	int		linkb();
extern	int		msgdsize();
extern	int		pullupmsg();
extern	int		putbq();
extern	int		putctl();
extern	int		putctl1();
extern	void		putnext();
extern	int		putq();
extern	int		qenable();
extern	void		qreply();
extern	int		qsize();
extern	mblk_t *	rmvb();
extern	int		rmvq();
extern	int		strlog();
extern	dev_t		strmod_add();	/* OSF add-on for configuration */
extern	int		strmod_del();	/* OSF add-on for configuration */
extern	int		testb();
extern	mblk_t *	unlinkb();

#else	/* _NO_PROTO */

extern	mblk_t *	allocb(int size, int pri);
extern	mblk_t *	allocbi(int, int, pfi_t, char *, uchar *); /* OSF */
extern	queue_t *	allocq(void);
extern	int		adjmsg(mblk_t * mp, int len_param);
extern	queue_t *	backq(queue_t * q);
extern	int		bufcall(int size, int pri, int (* f)(), long arg);
extern	int		canput(queue_t * q);
extern	mblk_t *	copyb(mblk_t * mp);
extern	mblk_t *	copymsg(mblk_t * mp);
extern	mblk_t *	dupb(mblk_t * bp);
extern	mblk_t *	dupmsg(mblk_t * mp);
extern	int		flushq(queue_t * q, int flag);
extern	int		freeb(mblk_t * bp);
extern	int		freemsg(mblk_t * mp);
extern	int		freeq(queue_t * q);
extern	mblk_t *	getq(queue_t * q);
extern	int		insq(queue_t * q, mblk_t * emp, mblk_t * nmp);
extern	int		linkb(mblk_t * mp1, mblk_t * mp2);
extern	int		msgdsize(mblk_t * mp);
extern	int		pullupmsg(mblk_t * mp, int len);
extern	int		putbq(queue_t * q, mblk_t * mp);
extern	int		putctl(queue_t * q, int type);
extern	int		putctl1(queue_t * q, int type, int c);
extern	void		putnext(queue_t *, mblk_t *);
extern	int		putq(queue_t * q, mblk_t * mp);
extern	int		qenable(queue_t * q);
extern	void		qreply(queue_t * q, mblk_t * mp);
extern	int		qsize(queue_t * q);
extern	mblk_t *	rmvb(mblk_t * mp, mblk_t * bp);
extern	int		rmvq(queue_t * q, mblk_t * mp);
extern	int		strlog(short sid, short mid, char level, short aflags, ...);
extern	dev_t		strmod_add(dev_t, struct streamtab *, struct streamadm *);
extern	int		strmod_del(dev_t, struct streamtab *, struct streamadm *);
extern	int		testb(int size, int pri);
extern	mblk_t *	unlinkb(mblk_t * mp);

#endif	/* _NO_PROTO */

#endif	/* _KERNEL */

#endif	/* _SYS_STREAM_H */
