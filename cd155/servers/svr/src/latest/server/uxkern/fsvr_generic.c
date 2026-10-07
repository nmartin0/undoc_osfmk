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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was modified and extended by the Center for High 
 * Performance Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: fsvr_generic.c,v $
 * Revision 1.6  1995/02/01  22:19:56  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:47:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:41:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:01:28  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:27:39  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:54:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:52:17  cfj
 * Bump major revision number.
 *
 * Revision 2.7  1992/05/31  18:59:42  loverso
 *
 * Revision 2.10  93/06/19  15:42:47  yazz
 * [ ad1.04 merge ]
 * 	Guard System V IPC-specific routine with #ifdef TNC so non-TNC
 * 	configurations will build.
 * 
 * Revision 2.9  93/06/02  12:35:00  yazz
 * For Sys V IPC under TNC add the generic svipc message handling routine.
 * 
 * Revision 2.8  93/01/25  22:56:28  durriya
 * 	get rid of start_port and root_port args in struct vnode_request
 * 	and as args to fsvr_vnode_forw                         (durriya)
 *
 * Revision 2.7  92/05/31  18:59:42  loverso
 * 	Remove duplicated include of parallel.h (pjg).
 * 
 * Revision 2.6  92/03/15  14:33:39  roy
 * 	Call ux_server_thread_[un]blocking before and after call to
 * 	mach_msg() (srl).
 * 	Change copyright (pjg).
 * 
 * Revision 2.5  92/03/01  18:32:35  pjg
 * 	Remove signature ports and interrupt variable.  The interrupt return
 * 	of std "bsd" reply message is unused in fsvr reply messages.  (loverso)
 * 
 * Revision 2.4  92/01/17  17:27:00  roy
 * 	Extra arg to start ops for int. system calls (loverso).
 * 
 * Revision 2.3  92/01/14  11:26:41  roy
 * 	92/01/10  22:01:20  noemi
 * 	Removed fsvr_vnode_generic_server.  Fixed a bug in fsvr_vnode_forw.
 * 
 * Revision 2.2  92/01/05  20:21:38  roy
 * 	92/01/02  14:31:49  roy
 * 	Set forwarding ports to correct values.  Remove syscall tracing.
 * 
 * 	1991/12/16  22:17:49  noemi
 * 	Initial revision.  
 * 
 */

#include <uxkern/import_mach.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/systm.h>

#include <uxkern/proc_to_task.h>
#include <uxkern/syscall_subr.h>
#include <uxkern/bsd_msg.h>
#include <uxkern/syscalltrace.h>
#include <uxkern/fsvr_generic.h>
#include <uxkern/sthread.h>

extern struct sysent    sysent[];
int                     nsysent;

/*
 * Generic forwarding interface for fileserver ports.
 * Basically the same as the interface in the emulation library.
 * Used to forward a message to a vnode_port.
 */
/*
 * Generic forwarded file port call.
 */
struct vnode_request vnode_req_template = {
    {
	MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE) |
	    MACH_MSGH_BITS_COMPLEX,	/* msgh_bits */
	0,				/* msgh_size */
	MACH_PORT_NULL,			/* msgh_remote_port */
	MACH_PORT_NULL,			/* msgh_local_port */
	0,				/* msgh_seqno */
	VNODE_REQ_MSG_ID		/* msgh_id */
    },
    {
	MACH_MSG_TYPE_COPY_SEND,	/* msgt_name */
	32,				/* msgt_size */
	1,				/* msgt_number (of ports) */
					/* creds */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,				/* PORTS: creds */
    {
	MACH_MSG_TYPE_INTEGER_32,	/* msgt_name */
	32,				/* msgt_size */
	9,				/* msgt_number */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,0,0,				/* transid, rval2, code */
    { 0, 0, 0, 0, 0, 0 },		/* arg array */
};

int
fsvr_vnode_forw(request_port, cred_port, transid, code, argp, rvalp)
	mach_port_t		request_port;
	mach_port_t		cred_port;
	transaction_id_t	transid;
	int			*argp;
	int			*rvalp;
{
	register kern_return_t	error;
	mach_port_t	reply_port;
	union vnode_msg		vnode_msg;

	vnode_msg.req = vnode_req_template;

	/* Special case for remote ioctl messages */
	if (code == SPEC_IOCTL)
		vnode_msg.req.hdr.msgh_id = FORW_REQ_MSG_ID;

	reply_port = mig_get_reply_port();
	vnode_msg.req.hdr.msgh_remote_port = request_port;
	vnode_msg.req.cred_port = cred_port;
	vnode_msg.req.transid = transid;
	vnode_msg.req.hdr.msgh_local_port  = reply_port;

	vnode_msg.req.syscode	= code;
	vnode_msg.req.rval2	= rvalp[1];
	vnode_msg.req.arg[0]	= argp[0];
	vnode_msg.req.arg[1]	= argp[1];
	vnode_msg.req.arg[2]	= argp[2];
	vnode_msg.req.arg[3]	= argp[3];
	vnode_msg.req.arg[4]	= argp[4];
	vnode_msg.req.arg[5]	= argp[5];

	ux_server_thread_blocking();
	error = mach_msg(&vnode_msg.req.hdr, MACH_SEND_MSG|MACH_RCV_MSG,
			 sizeof vnode_msg.req, sizeof vnode_msg, reply_port,
			 MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	ux_server_thread_unblocking();

	if (error != MACH_MSG_SUCCESS) {
	    return (error);
	}

	error = vnode_msg.rep.retcode;

	if (error == 0) {

	    register char *		start;
	    char *			end;
	    register mach_msg_type_long_t *	tp;
	    register vm_size_t		size;

	    vm_address_t	user_addr, msg_addr;

	    /*
	     * Pass return values back to caller.
	     */
	    rvalp[0] = vnode_msg.rep.rval[0];
	    rvalp[1] = vnode_msg.rep.rval[1];

	    /*
	     * Scan reply message for data to copy
	     */
	    start = (char *)&vnode_msg + sizeof(struct bsd_reply);
	    end   = (char *)&vnode_msg + vnode_msg.rep.hdr.msgh_size;
	    while (end > start) {

		/*
		 * Descriptor for address
		 */
		start += sizeof(mach_msg_type_t);

		/*
		 * Address
		 */
		user_addr = *(vm_address_t *)start;
		start += sizeof(vm_address_t);

		/*
		 * Data - size is in bytes
		 */
		tp = (mach_msg_type_long_t *)start;
		if (tp->msgtl_header.msgt_longform) {
		    size = tp->msgtl_number;
		    start += sizeof(mach_msg_type_long_t);
		}
		else {
		    size = tp->msgtl_header.msgt_number;
		    start += sizeof(mach_msg_type_t);
		}

		if (tp->msgtl_header.msgt_inline) {
		    bcopy(start, (char *)user_addr, size);
		    start += size;
		    start = (char *)
				( ((int)start + sizeof(int) - 1)
				  & ~(sizeof(int) - 1) );
		}
		else {
		    msg_addr = *(vm_address_t *)start;
		    start += sizeof(vm_address_t);
		    bcopy((char *)msg_addr, (char *)user_addr, size);
		    (void) vm_deallocate(mach_task_self(), msg_addr, size);
		}
	    }
	}

	return (error);
}


/*
 * Generic file port message handler.
 */
boolean_t
fsvr_fs_generic_server(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
{
	register struct uthread		*uth = &u;
	register struct fs_request	*req = (struct fs_request *)InHeadP;
	register struct bsd_reply	*rep = (struct bsd_reply *)OutHeadP;
	register struct sysent		*callp;
	register int			syscode = req->syscode;
	int 				error, retval[2];
	struct	file			*fp;

	/*
	 * Fix the reply message.
	 */
	static mach_msg_type_t bsd_rep_int_type = {
	    /* msgt_name */		MACH_MSG_TYPE_INTEGER_32,
	    /* msgt_size */		32,
	    /* msgt_number */		4,
	    /* msgt_inline */		TRUE,
	    /* msgt_longform */		FALSE,
	    /* msgt_deallocate */	FALSE,
	    /* msgt_unused */		0
	};

	if (InHeadP->msgh_id != FS_REQ_MSG_ID)
	    return (FALSE);

	/*
	 * Set up standard reply.
	 */
	rep->hdr.msgh_bits =
		MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(InHeadP->msgh_bits), 0);
	rep->hdr.msgh_remote_port = InHeadP->msgh_remote_port;
	rep->hdr.msgh_local_port = MACH_PORT_NULL;
	rep->hdr.msgh_id = InHeadP->msgh_id + 100;
	rep->int_type = bsd_rep_int_type;
	rep->hdr.msgh_size = sizeof(struct bsd_reply);

	/*
	 * Find the system call table descriptor for this call.
	 */
	if (syscode < 0)
	    callp = &sysent[0];
	else {
	    if (syscode >= nsysent)
		callp = &sysent[0];
	    else
		callp = &sysent[syscode];
	}

	/*
	 * Set up server thread to handle process
	 */
	rep->retcode = start_fileserver_op(&fp, req->hdr.msgh_local_port,
				req->cred_port, req->transid,
				req->syscode, !callp->sy_parallel);
	if (rep->retcode != KERN_SUCCESS)
		return (TRUE);
	/*
	 * Save the reply msg and initialize current_output.
	 * The user_copy/user_reply_msg code uses them for copyout.
	 */
	uth->uu_reply_msg = &rep->hdr;
	uth->uu_current_size = 0;

	retval[0] = 0;
	retval[1] = req->rval2;

	error = (*callp->sy_call)(fp, req->arg, retval);

#if     MAP_UAREA
	if (uth->uu_share_lock_count) {
	    panic("Share lock still held", uth->uu_share_lock_count);
	    uth->uu_share_lock_count = 0;
	}
#endif  MAP_UAREA

	rep->retcode = end_fileserver_op(fp, error, !callp->sy_parallel);
	rep->rval[0] = retval[0];
	rep->rval[1] = retval[1];

	/*
	 * Wrap up any trailing data in the reply message.
	 */
	finish_reply_msg();

	return (TRUE);
}


/*
 * Generic UX file server message handler.
 * Generic file system system call.
 */
boolean_t
fsvr_generic_server(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
{
	register struct uthread		*uth = &u;
	register struct forw_request	*req = (struct forw_request *)InHeadP;
	register struct bsd_reply	*rep = (struct bsd_reply *)OutHeadP;
	register int			code = req->code;
	int 				error, retval[2];
	struct nameidata 		*ndp = &uth->uu_nd;


	/*
	 * Fix the reply message.
	 */
	static mach_msg_type_t bsd_rep_int_type = {
	    /* msgt_name */		MACH_MSG_TYPE_INTEGER_32,
	    /* msgt_size */		32,
	    /* msgt_number */		4,
	    /* msgt_inline */		TRUE,
	    /* msgt_longform */		FALSE,
	    /* msgt_deallocate */	FALSE,
	    /* msgt_unused */		0
	};

	if (InHeadP->msgh_id != FORW_REQ_MSG_ID)
	    return (FALSE);

	/*
	 * Set up standard reply.
	 */
	rep->hdr.msgh_bits =
		MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(InHeadP->msgh_bits), 0);
	rep->hdr.msgh_remote_port = InHeadP->msgh_remote_port;
	rep->hdr.msgh_local_port = MACH_PORT_NULL;
	rep->hdr.msgh_id = InHeadP->msgh_id + 100;
	rep->int_type = bsd_rep_int_type;
	rep->hdr.msgh_size = sizeof(struct bsd_reply);

	/*
	 * Save the reply msg and initialize current_output.
	 * The user_copy/user_reply_msg code uses them for copyout.
	 */
	uth->uu_reply_msg = &rep->hdr;
	uth->uu_current_size = 0;

	retval[0] = 0;
	retval[1] = req->rval2;

	/*
	 * Do the forwarded operation.
	 */

	 switch (code) {

	 case	SPEC_IOCTL:
		rep->retcode = start_vnodeserver_op(req->hdr.msgh_local_port,
 					MACH_PORT_NULL, MACH_PORT_NULL,
 					req->cred_port, req->transid,
					NULL, 0, 0, 0);
 		if (rep->retcode != KERN_SUCCESS)
			return (TRUE);
		/*
		 * start_vnodeserver_op left the vnode in ndp->ni_cdir);
		 */
		error = spec_ioctl(ndp->ni_cdir, req->arg[0], req->arg[1],
			req->arg[2], (req->cred_port == MACH_PORT_NULL) ?
			NOCRED : ndp->ni_cred);

		rep->retcode = end_vnodeserver_op(req->hdr.msgh_local_port,
			MACH_PORT_NULL, MACH_PORT_NULL, error, 0);
		break;

	 default:
		return(FALSE);
	}

	/*
	 * Wrap up any trailing data in the reply message.
	 */
	finish_reply_msg();

	return (TRUE);
}


#ifdef TNC
/*
 * Generic System V IPC port message handler.
 */
boolean_t
svipc_generic_server(InHeadP, OutHeadP)
	mach_msg_header_t *InHeadP, *OutHeadP;
{
	register struct uthread		*uth = &u;
	register struct svipc_request	*req = (struct svipc_request *)InHeadP;
	register struct bsd_reply	*rep = (struct bsd_reply *)OutHeadP;
	register struct sysent		*callp;
	register int			syscode = req->syscode;
	int 				error, retval[2];
	struct svipc			*svp;

	/*
	 * Fix the reply message.
	 */
	static mach_msg_type_t bsd_rep_int_type = {
	    /* msgt_name */		MACH_MSG_TYPE_INTEGER_32,
	    /* msgt_size */		32,
	    /* msgt_number */		4,
	    /* msgt_inline */		TRUE,
	    /* msgt_longform */		FALSE,
	    /* msgt_deallocate */	FALSE,
	    /* msgt_unused */		0
	};

	if (InHeadP->msgh_id != SVIPC_REQ_MSG_ID)
	    return (FALSE);

	/*
	 * Set up standard reply.
	 */
	rep->hdr.msgh_bits =
		MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(InHeadP->msgh_bits), 0);
	rep->hdr.msgh_remote_port = InHeadP->msgh_remote_port;
	rep->hdr.msgh_local_port = MACH_PORT_NULL;
	rep->hdr.msgh_id = InHeadP->msgh_id + 100;
	rep->int_type = bsd_rep_int_type;
	rep->hdr.msgh_size = sizeof(struct bsd_reply);

	/*
	 * Find the system call table descriptor for this call.
	 */
	if (syscode < 0)
	    callp = &sysent[0];
	else {
	    if (syscode >= nsysent)
		callp = &sysent[0];
	    else
		callp = &sysent[syscode];
	}

	/*
	 * Set up server thread to handle process
	 */
	rep->retcode = start_svipcserver_op(&svp, req->hdr.msgh_local_port,
				req->cred_port, req->transid,
				req->syscode, !callp->sy_parallel);
	if (rep->retcode != KERN_SUCCESS)
		return (TRUE);
	/*
	 * Save the reply msg and initialize current_output.
	 * The user_copy/user_reply_msg code uses them for copyout.
	 */
	uth->uu_reply_msg = &rep->hdr;
	uth->uu_current_size = 0;

	retval[0] = 0;
	retval[1] = req->rval2;

	error = (*callp->sy_call)(svp, req->arg, retval);

#if     MAP_UAREA
	if (uth->uu_share_lock_count) {
	    panic("Share lock still held", uth->uu_share_lock_count);
	    uth->uu_share_lock_count = 0;
	}
#endif  MAP_UAREA

	rep->retcode = end_svipcserver_op(svp, error, !callp->sy_parallel);
	rep->rval[0] = retval[0];
	rep->rval[1] = retval[1];

	/*
	 * Wrap up any trailing data in the reply message.
	 */
	finish_reply_msg();

	return (TRUE);
}
#endif	/* TNC */
