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
 * $Log: msg.h,v $
 * Revision 1.5  1994/11/18  20:40:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:26:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:19:36  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:42:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:25  cfj
 * Bump major revision number.
 * Revision 1.1.1.1  1993/05/03  17:41:58  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  93/06/02  09:54:58  yazz
 * For Sys V IPC under TNC change the pid fields from ushort to pid_t.
 * 
 * Revision 2.2  91/08/31  14:08:51  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:43:55  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:52:24  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:42:32  gm]
 * 
 * Revision 1.6  90/08/09  13:28:53  devrcs
 * 	Make msgbuf user visible.
 * 	[90/08/01  16:41:21  seiden]
 * 
 * 	Removed  extern u_int msg_mark;
 * 	[90/07/31  10:47:49  bet]
 * 
 * Revision 1.5  90/07/27  09:07:28  devrcs
 * 	Added external declarations.
 * 	Define kernel structures for _KERNEL only.
 * 	[90/07/23  12:48:43  bet]
 * 
 * Revision 1.4  90/06/22  20:53:29  devrcs
 * 	Changed lspid and lrpid fields of struct msgid_ds to ushort's,
 * 	as per SVID.
 * 	[90/06/06  13:05:09  ers]
 * 
 * Revision 1.3  90/04/27  19:26:35  devrcs
 * 	Integrated with AIX header.
 * 	[90/04/20  10:51:11  bet]
 * 
 * Revision 1.2  90/02/23  00:32:56  devrcs
 * 	Initial version of System V IPC message header file.
 * 	[90/02/20  11:22:21  bet]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)msg.h	1.18  com/inc/sys,3.1,8943 9/7/89 08:16:53 */

#ifndef _SYS_MSG_H_
#define _SYS_MSG_H_

#include <standards.h>
#include <sys/ipc.h>

#ifdef _XOPEN_SOURCE

/*
 *	Message Operation Flags.
 */

#define	MSG_NOERROR	010000	/* no error if big message */

/*
 *	Structure Definitions
 */

/*
 *	There is one msg queue id data structure for each q in the system.
 */

struct msqid_ds {
	struct ipc_perm	msg_perm;	/* operation permission struct */
	struct msg	*msg_first;	/* ptr to first message on q */
	struct msg	*msg_last;	/* ptr to last message on q */
	u_short		msg_cbytes;	/* current # bytes on q */
	u_short		msg_qnum;	/* # of messages on q */
	u_short		msg_qbytes;	/* max # of bytes on q */
	pid_t		msg_lspid;	/* pid of last msgsnd */
	pid_t		msg_lrpid;	/* pid of last msgrcv */
	time_t		msg_stime;	/* last msgsnd time */
	time_t		msg_rtime;	/* last msgrcv time */
	time_t		msg_ctime;	/* last change time */
};

#ifndef _KERNEL
#ifdef _NO_PROTO
extern int msgctl();
extern int msgget();
extern int msgrcv();
extern int msgsnd();
#else
extern int msgget(key_t, int);
extern int msgrcv(int, void *, size_t, long, int);
extern int msgsnd(int, void *, size_t, int);
extern int msgctl(int, int, struct msqid_ds *);
#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif /* _XOPEN_SOURCE */

#ifdef	_OSF_SOURCE
/*
 * 	Implementation Constants. 
 */

#define	PMSG	(PZERO + 2)	/* message facility sleep priority */

/*
 * 	Permission Definitions. 
 */
#define	MSG_R		IPC_R	/* read permission */
#define	MSG_W		IPC_W	/* write permission */

/*
 *	ipc_perm Mode Definitions.
 */
#define	MSG_RWAIT	01000	/* a reader is waiting for a message */
#define	MSG_WWAIT	02000	/* a writer is waiting to send */

/*
 *	There is one msg structure for each message that may be in the system.
 */

#ifdef _KERNEL

struct msg {
	struct msg     *msg_next;	/* ptr to next message on q */
	long            msg_type;	/* message type */
	long		msg_ts; 	/* message text size */
	caddr_t         msg_addr;	/* message text address */
};
#endif _KERNEL


/*
 *	User message buffer template for msgsnd and msgrcv system calls.
 */

struct msgbuf {
	mtyp_t	mtype;		/* message type */
	char	mtext[1];	/* message text */
};


#ifdef _KERNEL
/*
 *	Message information structure.
 */

struct msginfo {
	int	msgmax,	/* max message size */
		msgmnb,	/* max # bytes on queue */
		msgmni,	/* # of message queue identifiers */
	   	msgtql;	/* # of system message headers */
};

#endif  /* _KERNEL */

#endif	/* _OSF_SOURCE */

extern struct msqid_ds msgque[];
extern struct msginfo msginfo;

#endif  /* _SYS_MSG_H_ */

