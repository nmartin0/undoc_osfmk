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
 * $Log: svipc_msg.c,v $
 * Revision 1.5  1994/11/18  20:27:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:49:25  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:49:49  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:05:48  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:25:21  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/12/16  13:06:52  roy
 * 	Initial check-in.
 *
 * Revision 3.0  91/10/31  12:27:04  david
 * ported from 1.0.2
 * 
 * Revision 1.10.7.2  91/06/20  09:06:28  watkins
 * 	Use msghdr_wait count to prevent doing a wakeup() when no one
 * 	is sleeping. Rearrange locking to serialize access to queue's.
 * 	[91/06/20  09:03:58  watkins]
 * 
 * Revision 1.10.4.2  91/03/11  14:53:06  seiden
 * 	Return EACCESS, instead of 'break', when sec_ipcaccess fails in msgctl()
 * 	[91/03/09  16:58:52  seiden]
 * 
 * Revision 1.10.4.2  91/03/11  14:53:06  seiden
 * 	Return EACCESS, instead of 'break', when sec_ipcaccess fails in msgctl()
 * 	[91/03/09  16:58:52  seiden]
 * 
 * Revision 1.10  90/10/31  13:49:33  devrcs
 * 	Added check for negative message id.
 * 	[90/10/11  10:33:01  bet]
 * 
 * Revision 1.9  90/10/07  13:19:12  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:59:37  gm]
 * 
 * 	Change msgsz to an int, which needs to be signed.
 * 	Fixed locking with msgrcv_lock.
 * 	[90/09/26  10:11:22  bet]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:34:04  jeffc]
 * 
 * Revision 1.8  90/08/24  11:18:29  devrcs
 * 	changed sleep to tsleep
 * 	[90/08/20  01:35:14  gmf]
 * 
 * 	New system call interface
 * 	[90/08/17  17:39:04  nags]
 * 
 * 	use new sec_svipc_object_create() to reduce number of
 * 	places SIP locking and audit hooks required
 * 	[90/08/14  15:02:36  hosking]
 * 
 * Revision 1.7  90/08/09  13:14:49  devrcs
 * 	outdate previous revision.
 * 	[90/07/30  15:42:36  bet]
 * 	Revision 1.4  90/06/22  20:06:58  devrcs
 * 	nags merge
 * 	[90/06/12  19:07:39  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:40:35  seiden]
 * 
 * 	Remove pageable flag from zinit argument list.
 * 	[90/06/07  15:20:40  jvs]
 * 
 * 	Condensed history:
 * 	     Suser interface for OSF/1.				nags@encore.com
 * 	     Removed sysv_ipc conditional			bet@osf.org
 * 	     Removed #include <sys/dir.h>.			bet@osf.org
 * 	     Initial version of System V IPC message facility.	bet@osf.org
 * 	[90/06/07  16:14:46  jvs]
 * 
 * $EndLog$
 */
/* @(#)svipc_msg.c	4.1 20:54:30 7/9/90 SecureWare */

/* @(#)msg.c	6.2 */
/*
 *	Inter-Process Communication Message Facility. 
 */

#include <sys/secdefines.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/systm.h>
#include <sys/uio.h>
#include <kern/kalloc.h>
#include <kern/zalloc.h>
#include <kern/macro_help.h>
#include <kern/lock.h>
#if	SEC_BASE
#include <sys/security.h>
#endif	
#if	SEC_ARCH
#include <sys/secpolicy.h>
#endif

int		msghdr_wait;		/* wait for headers event */
struct zone    *msghdr_zone;		/* zone for message headers */
struct msqid_ds msgque[MSGMNI];		/* msg queue headers */
struct msginfo  msginfo = {		/* message parameters */
			   MSGMAX,
			   MSGMNB,
			   MSGMNI,
			   MSGTQL
};
#if	SEC_ARCH
/*
 * Allocate space for the message queue tag pools. On systems that
 * allocate the message queue structures dynamically, the tag pools
 * should also be dynamically allocated at the same time as the
 * queues.
 */
tag_t		msgtag[MSGMNI * SEC_TAG_COUNT];
#endif	

lock_data_t msg_lock;		/* protect queue's on snd, rcv, ctl */

int             cur_msghdrs;	/* current number of message headers */

#ifndef OSF1_SERVER
extern time_t   time;		/* system idea of date */
#endif	


/*
 * free the message header, wakeup waiting procs, and decrement hdr count 
 */
#define FREE_MSGHDR(mp)				\
MACRO_BEGIN					\
    zfree(msghdr_zone, (vm_offset_t) mp);	\
    if (msghdr_wait) {				\
  	msghdr_wait = 0;			\
	wakeup(&msghdr_wait);			\
    }						\
    cur_msghdrs--;				\
MACRO_END

/*
 *	msgconv - Convert a user supplied message queue id into a ptr to a
 *		  msqid_ds structure.
 */

struct msqid_ds *
msgconv(id)
register int    id;	/* message queue id */
{
	register struct msqid_ds 	*qp;	/* ptr to associated q slot */

	if (id < 0)
		return (NULL);

	qp = &msgque[id % msginfo.msgmni];

	if ((qp->msg_perm.mode & IPC_ALLOC) == 0 ||
	    id / msginfo.msgmni != qp->msg_perm.seq)
		return (NULL);
#if	SEC_ARCH
	audstub_levels(MSGTAG(qp, 0));
#endif	
	return (qp);
}

/*
 *	msgctl - msgctl system call: perform  message control operations
 */

/* ARGSUSED */
msgctl(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int             msqid, 
		                cmd;
		struct msqid_ds *buf;
	}	*uap = (struct args *)args;
	register struct msqid_ds 	*qp;	/* ptr to associated q */
	struct msqid_ds 		ds;	/* queue work area */
	int				error;
#if	SEC_ARCH
	dac_t	dac;
	int	ret;
#endif	

	if ((qp = msgconv(uap->msqid)) == NULL)
		return(EINVAL);

	*retval = 0;
#if	SEC_BASE
	/*
	 * Set audit event type based on function code
	 */
	audstub_ipc1(uap->cmd);
#endif

	switch (uap->cmd) {
	case IPC_RMID:

#if	SEC_BASE
		if (!sec_owner(qp->msg_perm.uid, qp->msg_perm.cuid))
			return(EPERM);
#else
		if (u.u_uid != qp->msg_perm.uid && u.u_uid != qp->msg_perm.cuid
		    && (error = suser(u.u_cred, &u.u_acflag)))
			return(error);
#endif	

#if	SEC_ARCH
		if (sec_ipcaccess(&qp->msg_perm, MSGTAG(qp, 0), SP_DELETEACC))
			return(EACCES);
#endif

		/*
	 	 * Don't have to take the lock until here, since this
		 * system call is not parallel. Makes sure we don't 
		 * destroy a queue while a snd or rcv is in progress.
		 */
		lock_write(&msg_lock);

                /* make sure the queue has not been removed */
                if ((qp = msgconv(uap->msqid)) == NULL) {
                        lock_done(&msg_lock);
                        return(EIDRM);
                }

		while (qp->msg_first)
			msgfree(qp, NULL, qp->msg_first);

		qp->msg_cbytes = 0;

		if (uap->msqid + msginfo.msgmni < 0)
			qp->msg_perm.seq = 0;
		else
			qp->msg_perm.seq++;

		if (qp->msg_perm.mode & MSG_RWAIT)
			wakeup(&qp->msg_qnum);

		if (qp->msg_perm.mode & MSG_WWAIT)
			wakeup(qp);

		qp->msg_perm.mode = 0;

		lock_done(&msg_lock);

		return(0);

	case IPC_SET:

#if	SEC_BASE
		if (!sec_owner(qp->msg_perm.uid, qp->msg_perm.cuid))
			return(EPERM);
#else
		if (u.u_uid != qp->msg_perm.uid && u.u_uid != qp->msg_perm.cuid
		    && (error = suser(u.u_cred, &u.u_acflag)))
			return(error);
#endif	

#if	SEC_ARCH
		if (sec_ipcaccess(&qp->msg_perm, MSGTAG(qp, 0), SP_SETATTRACC))
			return(EACCES);
#endif

		/* get the queue id data struct into a work area */
		if (copyin(uap->buf, &ds, sizeof(ds)))
			return(EFAULT);

#if	SEC_BASE
		if (!sec_owner_change_permitted(qp->msg_perm.uid,
		    qp->msg_perm.gid, ds.msg_perm.uid, ds.msg_perm.gid))
			return(EPERM);
		audstub_dac(0, qp->msg_perm.uid, qp->msg_perm.gid,
				qp->msg_perm.mode);
		if (ds.msg_qbytes > qp->msg_qbytes &&
		    !privileged(SEC_LIMIT, EPERM))
			return(EPERM);
#else
		if (ds.msg_qbytes > qp->msg_qbytes
		    && (error = suser(u.u_cred, &u.u_acflag)))
			return(error);
#endif	

		qp->msg_perm.uid = ds.msg_perm.uid;
		qp->msg_perm.gid = ds.msg_perm.gid;
		qp->msg_perm.mode = (qp->msg_perm.mode & ~0777) |
				    (ds.msg_perm.mode & 0777);
		qp->msg_qbytes = ds.msg_qbytes;
#if	SEC_ARCH
		dac.uid = qp->msg_perm.uid;
		dac.gid = qp->msg_perm.gid;
		dac.mode = qp->msg_perm.mode;
		ret = SP_CHANGE_OBJECT(MSGTAG(qp, 0), &dac,
				SEC_NEW_UID|SEC_NEW_GID|SEC_NEW_MODE);
		if (ret) {
			if (ret & SEC_NEW_UID)
				qp->msg_perm.uid = dac.uid;
			if (ret & SEC_NEW_GID)
				qp->msg_perm.gid = dac.gid;
			if (ret & SEC_NEW_MODE)
				qp->msg_perm.mode = (qp->msg_perm.mode & ~0777)
						  | (dac.mode & 0777);
		}
#endif
#if	SEC_BASE
		audstub_dac(1, qp->msg_perm.uid, qp->msg_perm.gid,
				qp->msg_perm.mode);
#endif
#ifndef OSF1_SERVER
		qp->msg_ctime = time;
#else
		TIME_READ_LOCK();
		qp->msg_ctime = time.tv_sec;
		TIME_READ_UNLOCK();
#endif /* OSF1_SERVER */

		return(0);

	case IPC_STAT:

#if	SEC_ARCH
		if (sec_ipcaccess(&qp->msg_perm, MSGTAG(qp, 0), SP_READACC))
#else
		if (ipcaccess(&qp->msg_perm, MSG_R))
#endif	
			return(EACCES);

		if (copyout(qp, uap->buf, sizeof(*qp)))
			return(EFAULT);

		return(0);

	default:
		return(EINVAL);
	}
	return(0);
}

/*
 *	msgfree - Free up space and message header, relink pointers on q,
 *	and wakeup anyone waiting for resources.
 */

msgfree(qp, pre_mp, mp)
register struct msqid_ds 	*qp; 		/* ptr to q of mesg being freed */
register struct msg 		*mp; 		/* ptr to msg being freed */
register struct msg 		*pre_mp;	/* ptr to mp's predecessor */
{
	/* unlink message from the queue */
	if (pre_mp == NULL)
		qp->msg_first = mp->msg_next;
	else
		pre_mp->msg_next = mp->msg_next;

	if (mp->msg_next == NULL)
		qp->msg_last = pre_mp;

	qp->msg_qnum--;

	/* wakeup processes waiting for space on the queue */
	if (qp->msg_perm.mode & MSG_WWAIT) {
		qp->msg_perm.mode &= ~MSG_WWAIT;
		wakeup(qp);
	}

	/* free message text */
	if (mp->msg_ts)
		kfree(mp->msg_addr, mp->msg_ts);

	FREE_MSGHDR(mp);
}

/*
 *	msgget - msgget system call to get a message queue.
 */

/* ARGSUSED */
msgget(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		key_t           key;
		int             msgflg;
	}	*uap = (struct args *) args;
	struct msqid_ds *qp;	/* ptr to associated q */
	int             s;	/* ipcget status return */
	int		error;

	if (error = ipcget(uap->key, uap->msgflg, msgque, msginfo.msgmni,
			   sizeof(*qp), &s, (struct ipc_perm **)&qp))
		return(error);

#if	SEC_BASE
	/*
	 * Set the audit event type based on whether or not the queue
	 * already existed or is being created.
	 */
	audstub_ipc2(s, 0);
#endif	

	if (s) {
		/* initialize the new queue */
		qp->msg_first = qp->msg_last = NULL;
		qp->msg_qnum = 0;
		qp->msg_qbytes = msginfo.msgmnb;
		qp->msg_lspid = qp->msg_lrpid = 0;
		qp->msg_stime = qp->msg_rtime = 0;
#ifndef OSF1_SERVER
		qp->msg_ctime = time;
#else
		TIME_READ_LOCK();
		qp->msg_ctime = time.tv_sec;
		TIME_READ_UNLOCK();
#endif /* OSF1_SERVER */
#if	SEC_ARCH
		sec_svipc_object_create(MSGTAG(qp, 0));
#endif
#if	SEC_ILB
		sp_init_obj_bits(MSGTAG(qp, 0));
#endif
	}

	*retval = qp->msg_perm.seq * msginfo.msgmni + (qp - msgque);
	return(0);
}

/*
 *	msginit - Called by main(main.c) to initialize message queues.
 */

msginit()
{
	/* initialize a zone for message headers */
	msghdr_zone = zinit(sizeof(struct msg), msginfo.msgtql * sizeof(struct msg),
			    10 * sizeof(struct msg), "msghdr zone");

	msghdr_wait = 0;
	cur_msghdrs = 0;
	lock_init(&msg_lock, TRUE);
}

/*
 *	msgrcv - msgrcv system call to receive a message.
 */

/* ARGSUSED */
msgrcv(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int             msqid;
		struct msgbuf	*msgp;
		int		msgsz;
		long            msgtyp;
		int             msgflg;
	}	*uap = (struct args *) args;
	register struct msg 		*best_mp;	/* ptr to best msg on q */
	register struct msg 		*mp;		/* ptr to msg on q */
	register struct msg 		*pre_mp;	/* ptr to mp's predecessor */
	register struct msg 		*pre_best_mp;	/* ptr to best_mp's pred. */
	register struct msqid_ds 	*qp;		/* ptr to associated q */
	register int    		size;		/* transfer byte count */
	struct uio      		suio;		/* I/O info */
	struct iovec    		siov;		/* I/O vectors */
	int				error;
	int				firstpass;
#if	SEC_ILB
	tag_t				ntag;
#endif	


	lock_write(&msg_lock);
	firstpass = 1;

	for (;;) {

		/* make sure the queue has not been removed */
		if ((qp = msgconv(uap->msqid)) == NULL) {
			lock_done(&msg_lock);
			return(firstpass?EINVAL:EIDRM);
		}
		firstpass = 0;

#if	SEC_ARCH
		if (sec_ipcaccess(&qp->msg_perm, MSGTAG(qp, 0), SP_READACC)) 
#else
		if (ipcaccess(&qp->msg_perm, MSG_R)) 
#endif
		{
			lock_done(&msg_lock);
			return(EACCES);
		}

		if (uap->msgsz < 0) {
			lock_done(&msg_lock);
			return(EINVAL);
		}

		best_mp = pre_best_mp = NULL;

		mp = qp->msg_first;
		pre_mp = NULL;

		if (uap->msgtyp == 0)
			/* get the first message on the list */
			best_mp = mp;
		else {
		        /* 
			 * prevent a message being removed while another
			 * proc is searching the list
			 */

			for (; mp; pre_mp = mp, mp = mp->msg_next) {
				if (uap->msgtyp > 0) {
					/* return first message of this type */
					if (uap->msgtyp != mp->msg_type)
						continue;

					best_mp = mp;
					pre_best_mp = pre_mp;

					break;
				}

				if (mp->msg_type <= -uap->msgtyp) {
					/*
					 * return message with lowest type of
					 * all on the queue 
					 */
					if (best_mp && best_mp->msg_type <= mp->msg_type)
						continue;

					best_mp = mp;
					pre_best_mp = pre_mp;
				}
			}
			
		}

		if (best_mp) {

			/* found the requested message */
			if (uap->msgsz < best_mp->msg_ts)
				if (!(uap->msgflg & MSG_NOERROR)) {
					lock_done(&msg_lock);
					return(E2BIG);
				}
				else
					size = uap->msgsz;
			else
				size = best_mp->msg_ts;

#if	SEC_ILB
			if (SP_CHECK_FLOAT(UIO_READ, MSGTAG(qp, 0), &ntag)) {
				lock_done(&msg_lock); 
				error = u.u_error;
				return(error);
			}
#endif	
			/* store the message type */
			if (error = copyout(&best_mp->msg_type, uap->msgp,
					    sizeof(best_mp->msg_type))) {
				lock_done(&msg_lock);
				return(error);
			}

			if (size) {
				suio.uio_iov = &siov;
				suio.uio_iovcnt = 1;
				suio.uio_offset = 0;
				suio.uio_segflg = UIO_USERSPACE;
				suio.uio_resid = siov.iov_len = size;
				siov.iov_base = (caddr_t) uap->msgp + sizeof(best_mp->msg_type);

				/* read message text */
				suio.uio_rw = UIO_READ;
				if (error = uiomove(best_mp->msg_addr, size, 
						    &suio)) {
					lock_done(&msg_lock);
					return(error);
				}
			}

			*retval = size;

			qp->msg_cbytes -= best_mp->msg_ts;
			qp->msg_lrpid = u.u_procp->p_pid;
#ifndef OSF1_SERVER
			qp->msg_rtime = time;
#else
			TIME_READ_LOCK();
			qp->msg_rtime = time.tv_sec;
			TIME_READ_UNLOCK();
#endif /* OSF1_SERVER */
#if	SEC_ILB
			SP_DO_FLOAT(UIO_READ, MSGTAG(qp, 0), &ntag);
			if (qp->msg_qnum == 0)
				SP_EMPTY_OBJECT(MSGTAG(qp, 0));
#endif	

			/*
			 * Sys V raises the priority of this process to
			 * assure that it isn't preempted by the process
			 * doing the sleep. 
			 * curpri = PMSG; 
			 */

			/* free message from the linked list */
			msgfree(qp, pre_best_mp, best_mp);
			lock_done(&msg_lock);
			return(0);
		}  /* if (best_mp) */

		/* no message on the queue matches the receive request */
		if (uap->msgflg & IPC_NOWAIT) {
			lock_done(&msg_lock);
			return(ENOMSG);
		}

		/* wait for more messages on the queue */
		qp->msg_perm.mode |= MSG_RWAIT;

		if (mpsleep(&qp->msg_qnum, PMSG | PCATCH, "sv_msg_rcv", 0,
		            &msg_lock, MS_LOCK_WRITE))
			return(EINTR);

	}  /* for (;;) */

	lock_done(&msg_lock);

	return(0);
}

/*
 *	msgsnd - msgsnd system call to send a message. 
 */

/* ARGSUSED */
msgsnd(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int             msqid;
		struct msgbuf	*msgp;
		int		msgsz;
		int             msgflg;
	}	*uap = (struct args *) args;
	register struct msqid_ds 	*qp;	/* ptr to associated q */
	register struct msg 		*mp;	/* ptr to allocated msg hdr */
	register int    		cnt;	/* byte count */
	register caddr_t 		msgaddr;/* msg allocation address */
	struct uio      		suio;	/* I/O info */
	struct iovec    		siov;	/* I/O vectors */
	long            		type;	/* msg type */
	int				error;
	int				firstpass;
#if	SEC_ILB
	tag_t   			ntag;
#endif	

	/* first get the lock on msg queue's */
	lock_write(&msg_lock);
	firstpass = 1;

	for (;;) {

                /* make sure the queue has not been removed */
		if ((qp = msgconv(uap->msqid)) == NULL) {
			lock_done(&msg_lock);
			return(firstpass?EINVAL:EIDRM);
		}

		firstpass = 0;

#if	SEC_ARCH
		if (sec_ipcaccess(&qp->msg_perm, MSGTAG(qp, 0), SP_WRITEACC)) 
#else	
		if (ipcaccess(&qp->msg_perm, MSG_W)) 
#endif
		{
			lock_done(&msg_lock);
			return(EACCES);
		}

		if ((cnt = uap->msgsz) < 0 || cnt > msginfo.msgmax) {
			lock_done(&msg_lock);
			return(EINVAL);
		}

		/* get the message type */
		if (error = copyin(uap->msgp, &type, sizeof(type))) {
			lock_done(&msg_lock);
			return(error);
		}

		if (type < 1) {
			lock_done(&msg_lock);
			return(EINVAL);
		}


		/*
		 * if bytes for message exceed maximum number of bytes on 
		 * the queue, wait for more space on the queue 
		 */
		if (cnt + qp->msg_cbytes > qp->msg_qbytes) {
			if (uap->msgflg & IPC_NOWAIT) {
				lock_done(&msg_lock);
				return(EAGAIN);
			}

			qp->msg_perm.mode |= MSG_WWAIT;

			if (mpsleep(qp, PMSG | PCATCH, "sv_msg_snd", 0,
			            &msg_lock, MS_LOCK_WRITE)) {
				qp->msg_perm.mode &= ~MSG_WWAIT;
				wakeup(qp);
				return(EINTR);
			}
			continue;
		}

		/*
		 * if the max number of message headers have been allocated, 
		 * wait for some to be freed 
		 */
		if (cur_msghdrs >= msginfo.msgtql) {
			if (uap->msgflg & IPC_NOWAIT) {
				lock_done(&msg_lock);
				return(EAGAIN);
			}

			msghdr_wait++;
			if (mpsleep(&msghdr_wait, PMSG | PCATCH, "sv_msg_snd", 
			            0, &msg_lock, MS_LOCK_WRITE))
				return(EINTR);

			continue;
		}

		break;
	}  /* for (;;) */

	/* everything is available; allocate a message header */
	mp = (struct msg *) zalloc(msghdr_zone);
	cur_msghdrs++;

	if (cnt) {
		/* allocate the message text and fill it in */
		msgaddr = kalloc(cnt);

		suio.uio_iov = &siov;
		suio.uio_iovcnt = 1;
		suio.uio_offset = 0;
		suio.uio_segflg = UIO_USERSPACE;
		suio.uio_resid = siov.iov_len = cnt;
		siov.iov_base = (caddr_t) uap->msgp + sizeof(type);

		suio.uio_rw = UIO_WRITE;
		if (error = uiomove(msgaddr, cnt, &suio)) {
			kfree(msgaddr, cnt);
			FREE_MSGHDR(mp);
			lock_done(&msg_lock);
			return(error);
		}
#if	SEC_ILB
		if (SP_CHECK_FLOAT(UIO_WRITE, MSGTAG(qp, 0), &ntag)) {
			kfree(msgaddr, cnt);
			FREE_MSGHDR(mp);
			lock_done(&msg_lock);
			return(0);
		}
		SP_DO_FLOAT(UIO_WRITE, MSGTAG(qp, 0), &ntag);
#endif	
	}

	/* put the message on the queue */
	qp->msg_qnum++;
	qp->msg_cbytes += cnt;
	qp->msg_lspid = u.u_procp->p_pid;
#ifndef OSF1_SERVER
	qp->msg_stime = time;
#else
	TIME_READ_LOCK();
	qp->msg_stime = time.tv_sec;
	TIME_READ_UNLOCK();
#endif /* OSF1_SERVER */

	mp->msg_next = NULL;
	mp->msg_type = type;
	mp->msg_ts = cnt;
	mp->msg_addr = msgaddr;

	if (qp->msg_last == NULL)
		qp->msg_first = qp->msg_last = mp;
	else {
		qp->msg_last->msg_next = mp;
		qp->msg_last = mp;
	}

	if (qp->msg_perm.mode & MSG_RWAIT) {
		qp->msg_perm.mode &= ~MSG_RWAIT;

		/*
		 * Sys V raises the priority of this process to assure that
		 * it isn't preempted by the process doing the sleep.      
		 * curpri = PMSG; 
		 */
		wakeup(&qp->msg_qnum);
	}

	*retval = 0;
	lock_done(&msg_lock);
	return(0);
}
