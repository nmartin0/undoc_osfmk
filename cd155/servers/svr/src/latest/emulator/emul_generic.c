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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: emul_generic.c,v $
 * Revision 1.9  1995/02/01  21:22:08  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:23:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  17:30:51  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:22:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/27  21:25:06  hobbes
 * added support for libhippi.
 *
 * Revision 1.5  1993/05/06  20:14:47  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:25  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:17:32  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 2.15  93/06/02  12:25:17  yazz
 * For Sys V IPC under TNC add routine and message skeleton for the generic
 * svipc message type.
 * 
 * Revision 2.14  93/03/10  10:44:28  mmp
 * 	Cast fdt_unref_entry calls to (void).
 *
 * Revision 2.13  93/01/06  10:36:42  loverso
 * 	Fix notice.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:56:52  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:51:28  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:08:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:15:37  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.12  92/10/29  10:57:27  loverso
 * 	Deallocate reply ports on error.  (loverso)
 * 
 * Revision 2.11  92/08/26  12:09:14  loverso
 * 	isc_deregister() no longer takes transid.  Add emul_blocking
 * 	calls around PM calls. (loverso)
 * 
 * Revision 2.10  92/05/31  18:55:21  loverso
 * 	Revision 3.1  92/02/28  00:11:06  condict
 * 	Convert bcopy to user_bcopy, for EFAULT recovery.
 * 
 * Revision 2.9  92/03/01  18:48:46  pjg
 * 	Don't use the sigport (loverso).
 * 
 * Revision 2.8  92/01/17  17:17:14  roy
 * 	Pass transaction id through generic fs messages (loverso).
 * 
 * Revision 2.7  92/01/14  10:40:45  roy
 *	92/01/10  22:55:44  noemi
 *	Removed emul_vnode_generic.
 *
 * Revision 2.6  92/01/02  18:40:50  roy
 * 	91/10/14  21:25:34  noemi
 * 	Removed obsolete use of MACH_MSGH_KIND_NORMAL in message headers.
 * 	Changed emul_fs_generic and fs_req_template to use signature ports.
 * 
 * 	91/09/22  22:17:38  noemi
 * 	OSF1/ADFS update
 * 
 * Revision 2.5  91/12/13  10:05:06  roy
 * 	91/10/29  17:53:19  roy
 * 	Changes associated with new file descriptor table.
 * 
 * Revision 2.4  91/10/04  14:41:52  chrisp
 * Get rid of references to msgh_kind (it's now a sequence number).
 * 
 * Revision 2.3  91/09/17  12:11:33  sjs
 * integrate Locus changes	roman
 * Addition of a new routines emul_fs_generic() and
 * emul_vp_generic(), which handle all the generic system calls
 * which have a file descriptor as the first parameter or are
 * directed at a vproc port.
 * 
 * Revision 2.2  91/08/30  16:40:05  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.0  91/01/17  12:05:07  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.5  90/06/02  15:20:25  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  19:26:22  rpd]
 * 
 * Revision 2.4  89/11/29  15:26:09  af
 * 	Added rval2, because some syscalls do not modify it and it
 * 	must be preserved.
 * 	[89/11/20            af]
 * 
 * Revision 2.3  89/10/17  11:24:01  rwd
 * 	Add interrupt return parameter.  Remove special msg_rpc
 * 	redefinition.
 * 	[89/09/21            dbg]
 * 
 * Revision 2.2  89/08/09  14:35:19  rwd
 * 	Added initialization of unused field in msg_hdr.
 * 	[89/08/08            rwd]
 * 
 *
 */
#include <mach/mach.h>
#include <mach/message.h>
#include <mach/msg_type.h>
#include <uxkern/bsd_msg.h>
#include <sys/errno.h>
#include "emul.h"
#include "fdt.h"

/*
 * Generic emulated UX call.
 */
struct bsd_request bsd_req_template = {
    {
	MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE),
					/* msgh_bits */
	0,				/* msgh_size */
	MACH_PORT_NULL,			/* msgh_remote_port */
	MACH_PORT_NULL,			/* msgh_local_port */
	0,				/* msgh_seqno */
	BSD_REQ_MSG_ID			/* msgh_id */
    },
    {
	MACH_MSG_TYPE_INTEGER_32,	/* msgt_name */
	32,				/* msgt_size */
	8,				/* msgt_number */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,0,
    { 0, 0, 0, 0, 0, 0 },
};

int
emul_generic(serv_port, interrupt, syscode, argp, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int	syscode;
	int	* argp;
	int	* rvalp;
{
	register kern_return_t	error;
	register mach_port_t	reply_port;
	union bsd_msg	bsd_msg;

	bsd_msg.req = bsd_req_template;

	reply_port = mig_get_reply_port();
	bsd_msg.req.hdr.msgh_remote_port = serv_port;
	bsd_msg.req.hdr.msgh_local_port  = reply_port;

	bsd_msg.req.syscode	= syscode;
	bsd_msg.req.rval2	= rvalp[1];
	bsd_msg.req.arg[0]	= argp[0];
	bsd_msg.req.arg[1]	= argp[1];
	bsd_msg.req.arg[2]	= argp[2];
	bsd_msg.req.arg[3]	= argp[3];
	bsd_msg.req.arg[4]	= argp[4];
	bsd_msg.req.arg[5]	= argp[5];

	emul_blocking();
	error = mach_msg(&bsd_msg.req.hdr, MACH_SEND_MSG|MACH_RCV_MSG,
			 sizeof bsd_msg.req, sizeof bsd_msg, reply_port,
			 MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	emul_unblocking();

	if (error != MACH_MSG_SUCCESS) {
	    mig_dealloc_reply_port();
	    return (error);
	}

	error = bsd_msg.rep.retcode;
	*interrupt = bsd_msg.rep.interrupt;

	if (error == 0) {

	    register char *		start;
	    char *			end;
	    register mach_msg_type_long_t *	tp;
	    register vm_size_t		size;

	    vm_address_t	user_addr, msg_addr;

	    /*
	     * Pass return values back to caller.
	     */
	    rvalp[0] = bsd_msg.rep.rval[0];
	    rvalp[1] = bsd_msg.rep.rval[1];

	    /*
	     * Scan reply message for data to copy
	     */
	    start = (char *)&bsd_msg + sizeof(struct bsd_reply);
	    end   = (char *)&bsd_msg + bsd_msg.rep.hdr.msgh_size;
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
		    if (!user_bcopy(start, (char *)user_addr, size))
			error = EFAULT;
		    start += size;
		    start = (char *)
				( ((int)start + sizeof(int) - 1)
				  & ~(sizeof(int) - 1) );
		}
		else {
		    msg_addr = *(vm_address_t *)start;
		    start += sizeof(vm_address_t);
		    if (!user_bcopy((char *)msg_addr, (char *)user_addr, size))
			error = EFAULT;
		    (void) vm_deallocate(mach_task_self(), msg_addr, size);
		}
	    }
	}

	return (error);
}

#ifdef	OSF1_ADFS
/*
 * Generic emulated vproc call.
 */
struct bsd_request vproc_req_template = {
    {
	MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE),
					/* msgh_bits */
	0,				/* msgh_size */
	MACH_PORT_NULL,			/* msgh_remote_port */
	MACH_PORT_NULL,			/* msgh_local_port */
	0,				/* msgh_seqno */
	VPROC_REQ_MSG_ID		/* msgh_id */
    },
    {
	MACH_MSG_TYPE_INTEGER_32,	/* msgt_name */
	32,				/* msgt_size */
	8,				/* msgt_number */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,0,
    { 0, 0, 0, 0, 0, 0 },
};

int
emul_vproc_generic(serv_port, interrupt, syscode, argp, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int	syscode;
	int	* argp;
	int	* rvalp;
{
	register kern_return_t	error;
	register mach_port_t	reply_port;
	union bsd_msg	vproc_msg;

	vproc_msg.req = vproc_req_template;

	reply_port = mig_get_reply_port();
	vproc_msg.req.hdr.msgh_remote_port = vproc_port;
	vproc_msg.req.hdr.msgh_local_port  = reply_port;

	vproc_msg.req.syscode	= syscode;
	vproc_msg.req.rval2	= rvalp[1];
	vproc_msg.req.arg[0]	= argp[0];
	vproc_msg.req.arg[1]	= argp[1];
	vproc_msg.req.arg[2]	= argp[2];
	vproc_msg.req.arg[3]	= argp[3];
	vproc_msg.req.arg[4]	= argp[4];
	vproc_msg.req.arg[5]	= argp[5];

	emul_blocking();
	error = mach_msg(&vproc_msg.req.hdr, MACH_SEND_MSG|MACH_RCV_MSG,
			 sizeof vproc_msg.req, sizeof vproc_msg, reply_port,
			 MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	emul_unblocking();

	if (error != MACH_MSG_SUCCESS) {
	    mig_dealloc_reply_port();
	    return (error);
	}

	error = vproc_msg.rep.retcode;
	*interrupt = vproc_msg.rep.interrupt;

	if (error == 0) {

	    register char *		start;
	    char *			end;
	    register mach_msg_type_long_t *	tp;
	    register vm_size_t		size;

	    vm_address_t	user_addr, msg_addr;

	    /*
	     * Pass return values back to caller.
	     */
	    rvalp[0] = vproc_msg.rep.rval[0];
	    rvalp[1] = vproc_msg.rep.rval[1];

	    /*
	     * Scan reply message for data to copy
	     */
	    start = (char *)&vproc_msg + sizeof(struct bsd_reply);
	    end   = (char *)&vproc_msg + vproc_msg.rep.hdr.msgh_size;
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
		    if (!user_bcopy(start, (char *)user_addr, size))
			error = EFAULT;
		    start += size;
		    start = (char *)
				( ((int)start + sizeof(int) - 1)
				  & ~(sizeof(int) - 1) );
		}
		else {
		    msg_addr = *(vm_address_t *)start;
		    start += sizeof(vm_address_t);
		    if (!user_bcopy((char *)msg_addr, (char *)user_addr, size))
			error = EFAULT;
		    (void) vm_deallocate(mach_task_self(), msg_addr, size);
		}
	    }
	}

	return (error);
}


/*
 * Generic emulated file port call.
 */
struct fs_request fs_req_template = {
    {
	MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE) |
	    MACH_MSGH_BITS_COMPLEX,	/* msgh_bits */
	0,				/* msgh_size */
	MACH_PORT_NULL,			/* msgh_remote_port */
	MACH_PORT_NULL,			/* msgh_local_port */
	0,				/* msgh_seqno */
	FS_REQ_MSG_ID			/* msgh_id */
    },
    {
	MACH_MSG_TYPE_COPY_SEND,	/* msgt_name */
	32,				/* msgt_size */
	1,				/* msgt_number  - creds */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,					/* credentials_port */
    {
	MACH_MSG_TYPE_INTEGER_32,	/* msgt_name */
	32,				/* msgt_size */
	9,				/* msgt_number */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,0,0,				/* transid, rval2, syscode */
    { 0, 0, 0, 0, 0, 0 },		/* arg array */
};

int
emul_fs_generic(serv_port, interrupt, syscode, argp, rvalp)
	mach_port_t	serv_port;	/* bsd server port for this proc */
	boolean_t	*interrupt;
	int		syscode;
	int		*argp;
	int		*rvalp;
{
	register kern_return_t	error;
	mach_port_t		reply_port;
	union fs_msg		fs_msg;
        fdt_entry_t             *fdte;
	transaction_id_t	transid;

        if (error = fdt_ref_entry(argp[0], &fdte))
                return(error);

	fs_msg.req = fs_req_template;

	reply_port = mig_get_reply_port();
	fs_msg.req.hdr.msgh_remote_port = fdte->fp;
	fs_msg.req.cred_port = credentials_port;
	fs_msg.req.hdr.msgh_local_port = reply_port;

	isc_register(fdte->fp, &transid);
	fs_msg.req.transid = transid;

	fs_msg.req.syscode	= syscode;
	fs_msg.req.rval2	= rvalp[1];
	fs_msg.req.arg[0]	= argp[1];	/* 1 off since 1st arg=fd */
	fs_msg.req.arg[1]	= argp[2];
	fs_msg.req.arg[2]	= argp[3];
	fs_msg.req.arg[3]	= argp[4];
	fs_msg.req.arg[4]	= argp[5];
	fs_msg.req.arg[5]	= argp[6];

	error = mach_msg(&fs_msg.req.hdr, MACH_SEND_MSG|MACH_RCV_MSG,
			 sizeof fs_msg.req, sizeof fs_msg, reply_port,
			 MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

	isc_deregister(interrupt);

	(void) fdt_unref_entry(fdte);
	if (error != MACH_MSG_SUCCESS) {
	    mig_dealloc_reply_port();
	    return (error);
	}

	error = fs_msg.rep.retcode;
	/* don't use bsd_msg.rep.interrupt, as we got it in isc_deregister */

	if (error == 0) {

	    register char *		start;
	    char *			end;
	    register mach_msg_type_long_t *	tp;
	    register vm_size_t		size;

	    vm_address_t	user_addr, msg_addr;

	    /*
	     * Pass return values back to caller.
	     */
	    rvalp[0] = fs_msg.rep.rval[0];
	    rvalp[1] = fs_msg.rep.rval[1];

	    /*
	     * Scan reply message for data to copy
	     */
	    start = (char *)&fs_msg + sizeof(struct bsd_reply);
	    end   = (char *)&fs_msg + fs_msg.rep.hdr.msgh_size;
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
		    if (!user_bcopy(start, (char *)user_addr, size))
			error = EFAULT;
		    start += size;
		    start = (char *)
				( ((int)start + sizeof(int) - 1)
				  & ~(sizeof(int) - 1) );
		}
		else {
		    msg_addr = *(vm_address_t *)start;
		    start += sizeof(vm_address_t);
		    if (!user_bcopy((char *)msg_addr, (char *)user_addr, size))
			error = EFAULT;
		    (void) vm_deallocate(mach_task_self(), msg_addr, size);
		}
	    }
	}

	return (error);
}


struct fs_request fs_dev_req_template = {
    {
	MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE) |
	    MACH_MSGH_BITS_COMPLEX,	/* msgh_bits */
	0,				/* msgh_size */
	MACH_PORT_NULL,			/* msgh_remote_port */
	MACH_PORT_NULL,			/* msgh_local_port */
	0,				/* msgh_seqno */
	FS_REQ_MSG_ID			/* msgh_id */
    },
    {
	MACH_MSG_TYPE_COPY_SEND,	/* msgt_name */
	32,				/* msgt_size */
	1,				/* msgt_number  - creds */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,					/* credentials_port */
    {
	MACH_MSG_TYPE_INTEGER_32,	/* msgt_name */
	32,				/* msgt_size */
	9,				/* msgt_number */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,0,0,				/* transid, rval2, syscode */
    { 0, 0, 0, 0, 0, 0 },		/* arg array */
};

union fs_dev_msg {
	struct fs_request	req;
	struct dev_port_reply {
		struct bsd_reply	rep;
		struct dev_port {
			mach_msg_type_t	type;
			mach_port_t	port;
		} dev;
	} dev_rep;
};

mach_port_t
emul_fs_get_device_port(serv_port, interrupt, syscode, argp, rvalp)
	mach_port_t	serv_port;	/* bsd server port for this proc */
	boolean_t	*interrupt;
	int		syscode;
	int		*argp;
	int		*rvalp;
{
	register kern_return_t	error;
	mach_port_t		reply_port;
	union fs_dev_msg	fs_dev_msg;
        fdt_entry_t             *fdte;
	transaction_id_t	transid;

        if (error = fdt_ref_entry(argp[0], &fdte))
                return(error);

	fs_dev_msg.req = fs_dev_req_template;	/* structure copy */

	reply_port = mig_get_reply_port();
	fs_dev_msg.req.hdr.msgh_remote_port = fdte->fp;
	fs_dev_msg.req.cred_port = credentials_port;
	fs_dev_msg.req.hdr.msgh_local_port  = reply_port;

	isc_register(fdte->fp, &transid);
	fs_dev_msg.req.transid = transid;

	fs_dev_msg.req.syscode	= syscode;
	fs_dev_msg.req.rval2	= rvalp[1];
	fs_dev_msg.req.arg[0]	= argp[1];
	fs_dev_msg.req.arg[1]	= argp[2];

	error = mach_msg(&fs_dev_msg.req.hdr, MACH_SEND_MSG|MACH_RCV_MSG,
			 sizeof(fs_dev_msg.req), sizeof(fs_dev_msg), reply_port,
			 MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

	isc_deregister(interrupt);

	fdt_unref_entry(fdte);
	if (error != MACH_MSG_SUCCESS) {
	    mig_dealloc_reply_port();
	    return (error);
	}

	error = fs_dev_msg.dev_rep.rep.retcode;
	/* don't use bsd_msg.rep.interrupt, as we got it in isc_deregister */

	if (error == 0) {

	    /*
	     * Pass return values back to caller.
	     */
	    rvalp[0] = fs_dev_msg.dev_rep.dev.port;
	    rvalp[1] = 0;

	    }

	return (error);
}


#ifdef	TNC
/*
 * Generic emulated System V IPC port call under TNC.
 */
struct svipc_request svipc_req_template = {
    {
	MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE) |
	    MACH_MSGH_BITS_COMPLEX,	/* msgh_bits */
	0,				/* msgh_size */
	MACH_PORT_NULL,			/* msgh_remote_port */
	MACH_PORT_NULL,			/* msgh_local_port */
	0,				/* msgh_seqno */
	SVIPC_REQ_MSG_ID		/* msgh_id */
    },
    {
	MACH_MSG_TYPE_COPY_SEND,	/* msgt_name */
	32,				/* msgt_size */
	1,				/* msgt_number  - creds */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,					/* credentials_port */
    {
	MACH_MSG_TYPE_INTEGER_32,	/* msgt_name */
	32,				/* msgt_size */
	9,				/* msgt_number */
	TRUE,				/* msgt_inline */
	FALSE,				/* msgt_longform */
	FALSE,				/* msgt_deallocate */
	0				/* msgt_unused */
    },
    0,0,0,				/* transid, rval2, syscode */
    { 0, 0, 0, 0, 0, 0 },		/* arg array */
};

int
emul_svipc_generic(serv_port, interrupt, syscode, argp, rvalp)
	mach_port_t	serv_port;	/* bsd server port for this proc */
	boolean_t	*interrupt;
	int		syscode;
	int		*argp;
	int		*rvalp;
{
	register kern_return_t	error;
	mach_port_t		reply_port;
	union svipc_msg		svipc_msg;
	transaction_id_t	transid;
	int			dummy_interrupt;

	if (our_svipc_port == MACH_PORT_NULL) {
		error = bsd_get_my_svipc_port(our_bsd_server_port,
				&dummy_interrupt, &our_svipc_port);
		if (error != ESUCCESS)
			panic("emul_svipc_generic: bsd_get_my_svipc_port: %d",
					error);
	}

	svipc_msg.req = svipc_req_template;

	reply_port = mig_get_reply_port();
	svipc_msg.req.hdr.msgh_remote_port = our_svipc_port;
	svipc_msg.req.cred_port = credentials_port;
	svipc_msg.req.hdr.msgh_local_port = reply_port;

	isc_register(our_svipc_port, &transid);
	svipc_msg.req.transid = transid;

	svipc_msg.req.syscode	= syscode;
	svipc_msg.req.rval2	= rvalp[1];
	svipc_msg.req.arg[0]	= argp[0];	/* unlike fs, argp NOT 1 off */
	svipc_msg.req.arg[1]	= argp[1];
	svipc_msg.req.arg[2]	= argp[2];
	svipc_msg.req.arg[3]	= argp[3];
	svipc_msg.req.arg[4]	= argp[4];
	svipc_msg.req.arg[5]	= argp[5];

	error = mach_msg(&svipc_msg.req.hdr, MACH_SEND_MSG|MACH_RCV_MSG,
			 sizeof svipc_msg.req, sizeof svipc_msg, reply_port,
			 MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

	isc_deregister(interrupt);

	if (error != MACH_MSG_SUCCESS) {
	    mig_dealloc_reply_port();
	    return (error);
	}

	error = svipc_msg.rep.retcode;
	/* don't use bsd_msg.rep.interrupt, as we got it in isc_deregister */

	if (error == 0) {

	    register char *		start;
	    char *			end;
	    register mach_msg_type_long_t *	tp;
	    register vm_size_t		size;

	    vm_address_t	user_addr, msg_addr;

	    /*
	     * Pass return values back to caller.
	     */
	    rvalp[0] = svipc_msg.rep.rval[0];
	    rvalp[1] = svipc_msg.rep.rval[1];

	    /*
	     * Scan reply message for data to copy
	     */
	    start = (char *)&svipc_msg + sizeof(struct bsd_reply);
	    end   = (char *)&svipc_msg + svipc_msg.rep.hdr.msgh_size;
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
		    if (!user_bcopy(start, (char *)user_addr, size))
			error = EFAULT;
		    start += size;
		    start = (char *)
				( ((int)start + sizeof(int) - 1)
				  & ~(sizeof(int) - 1) );
		}
		else {
		    msg_addr = *(vm_address_t *)start;
		    start += sizeof(vm_address_t);
		    if (!user_bcopy((char *)msg_addr, (char *)user_addr, size))
			error = EFAULT;
		    (void) vm_deallocate(mach_task_self(), msg_addr, size);
		}
	    }
	}

	return (error);
}
#endif	/* TNC */
#endif	/* OSF1_ADFS */
