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
 * $Log: stropts.h,v $
 * Revision 1.4  1994/11/18  20:41:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:28:23  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:11  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:48  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:48  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/17  11:19:06  sjs
 * fixed RCS messages
 *
 * Revision 2.2  91/10/14  13:09:15  sjs
 * Check in
 * 
 * Revision 3.0  91/09/27  11:52:37  emcmanus
 * Installed for streams.
 * 
 * Revision 1.6  90/10/31  14:07:08  devrcs
 * 	Post-file-rearrangement pathnames.
 * 	[90/10/11  08:34:31  tmt]
 * 
 * 	Add recursive include protection. Checkin prior to file rearrangement.
 * 	[90/10/08  10:04:19  tmt]
 * 
 * Revision 1.5  90/10/07  14:54:27  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:45:36  gm]
 * 
 * Revision 1.4  90/07/27  09:08:06  devrcs
 * 	Fix character declaration.
 * 	[90/07/12  17:45:03  seiden]
 * 
 * Revision 1.3  90/07/17  11:42:36  devrcs
 * 	Added security STREAMS ioctl structures.
 * 	[90/07/10  22:03:36  seiden]
 * 
 * Revision 1.2  90/06/29  13:54:05  devrcs
 * 	Introduced standard STREAMS include file
 * 	(moved from /usr/include to here)
 * 	[90/06/25  07:36:52  nolting]
 * 
 * Revision 1.4  90/02/23  00:31:14  devrcs
 * 	Changed first _IO parameter to match new macros in sys/ioctl.h.
 * 	[90/02/18  14:05:47  nolting]
 * 
 * 	Integrated new PSE release from Mentat.
 * 	[90/02/14  21:02:27  nolting]
 * 
 * 	Integrated new PSE release from Mentat.
 * 	[90/02/05  07:56:50  nolting]
 * 
 * Revision 1.3  90/01/02  20:22:52  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:28:19  gm
 * 	Added streams support.
 * 	[89/12/25            gm]
 * 
 * $EndLog$
 */
/** Copyright (c) 1989  Mentat Inc.
 ** stropts.h 1.5, last change 10/17/89
 **/

#ifndef	_SYS_STROPTS_H
#define	_SYS_STROPTS_H

#include <sys/secdefines.h>
#include <sys/ioctl.h>

/* priority message request on putmsg() or strpeek */
#define	RS_HIPRI	0x1

/* return values from getmsg(), 0 indicates all ok */
#define	MORECTL		0x1	/* more control info available */
#define	MOREDATA	0x2	/* more data available */

/* flush requests */
#define	FLUSHR		1	/* Flush the read queue */
#define	FLUSHW		2	/* Flush the write queue */
#define	FLUSHRW		3	/* Flush both */

/* signal event masks */
#define	S_INPUT		0x1	/* non-priority message has arrived */
#define	S_HIPRI		0x2	/* priority message on stream head read q */
#define	S_OUTPUT	0x4	/* write queue is no longer full */
#define	S_MSG		0x8	/* signal message at front of read queue */

/* read modes for I_S|GRDOPT */
#define	RNORM		0	/* byte-stream mode, default */
#define	RMSGD		1	/* message-discard mode */
#define	RMSGN		2	/* message-nondiscard mode */
#define	RFILL		3	/* fill read buffer mode (PSE private) */

struct	strbuf {
	int	maxlen;		/* max buffer length */
	int	len;		/* length of data */
	char *	buf;		/* pointer to buffer */
};

/* structure of ioctl data on I_FDINSERT */
struct	strfdinsert {
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	long		flags;	/* type of message, 0 or RS_HIPRI */
	int		fildes;	/* fd of other stream */
	int		offset;	/* where to put other stream read qp */
};

/* I_PEEK structure */
struct	strpeek {
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	long		flags;	/* if RS_HIPRI, get priority messages only */
};

/* structure of ioctl data on I_RECVFD */
struct	strrecvfd {
	int		fd;	/* new file descriptor */
/* XXX uid_t, gid_t? */
	unsigned short	uid;	/* user id of sending stream */
	unsigned short	gid;
	char		fill[8];
};

/* structure of ioctl data on I_STR */
struct	strioctl {
	int	ic_cmd;		/* downstream command */
	int	ic_timout;	/* ACK/NAK timeout */
	int	ic_len;		/* length of data arg */
	char *	ic_dp;		/* ptr to data arg */
};
#if SEC_BASE

/* structure of ioctl data for I_FDINSERT_ATTR */
struct strfdinsert_attr {
        struct strbuf   ctlbuf;
        struct strbuf   databuf;
        long            flags;  /* type of message, 0 or RS_HIPRI */
        int             fildes; /* fd of other stream */
        int             offset; /* where to put other stream read qp */
        struct strbuf   attrbuf;
};

/* structure of ioctl data for I_PEEK_ATTR */
struct strpeek_attr {
        struct strbuf   ctlbuf;
        struct strbuf   databuf;
        long            flags;  /* if RS_HIPRI, get priority messages only */
        struct strbuf   attrbuf;
};

/* structure of ioctl data for I_RECVFD_ATTR */
struct strrecvfd_attr {
        int             fd;     /* new file descriptor */
/* XXX uid_t, gid_t? */
        unsigned short  uid;    /* user id of sending stream */
        unsigned short  gid;
        struct strbuf   attrbuf;
};

/* structure of ioctl data for I_SENDFD_ATTR */
struct strsendfd_attr {
        int             fd;     /* file descriptor to be passed */
        struct strbuf   attrbuf;
};

/* structure of ioctl data for I_STR_ATTR */
struct strioctl_attr {
        int     ic_cmd;         /* downstream command */
        int     ic_timout;      /* ACK/NAK timeout */
        int     ic_len;         /* length of data arg */
        char *  ic_dp;          /* ptr to data arg */
        struct strbuf   attrbuf;
};
#endif /* SEC_BASE */
#endif /* _SYS_STROPTS_H */
