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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 *
 *
 *      This module contains the emulator pfs functions that are
 *      used to implement PFS iomode communications.
 *
 * HISTORY
 * $Log: pfs_msgutil.c,v $
 * Revision 1.12  1994/11/18  20:24:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/03/29  17:54:15  rlg
 * Merged the changes from 1.6.4.5 in R1.2 into R1.3.
 *
 * Revision 1.10  1994/03/21  18:13:10  rlg
 * Merged changes from the R1.2 branch, 1.6.4.4, into the trunk.
 *
 * Revision 1.9  1994/03/04  22:24:40  dbm
 * Mainline merge for bug 6919, (1.2 rev 1.6.4.3)
 *
 * Revision 1.8  1994/02/16  00:47:53  dbm
 * Merge from 1.2 branch, revision 1.6.4.2
 *
 * Revision 1.7  1994/01/27  22:27:32  brad
 * Merge of revision 1.6.4.1 from the R1.2 branch.
 *
 * Revision 1.6.4.5  1994/03/29  16:18:04  rlg
 * The warning messages from lint were evaluated and corrections made as
 * required.
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  low
 *  Benefit or PTS #:  7719
 *
 * Revision 1.6.4.4  1994/03/21  17:35:59  rlg
 * Added code to check for a MACH_NOTIFY_NO_SENDERS response from
 * calls to  mach_msg().  This change is in conjunction with calls
 * to  mach_port_request_notification()  when the ports are allocated
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  medium
 *  Benefit or PTS #:  8431
 *  Testing:  failing test case; fileio and pfs EATs
 *  Module(s):  emulator/pfs_msgutil.c [rcv_iomode_msg(), rcv_iomode_token(),
 * 				     rcv_global(), rcv_global_vec()],
 * 	     emulator/pfs_iomode.c [pfs_init_co()]
 *
 * Revision 1.6.4.3  1994/03/04  21:44:21  dbm
 * Added logic to allow token msgs in M_SYNC to continue without being
 * interrupted by task_suspend.
 *  Reviewer: Brad Rullman
 *  Risk:M
 *  Benefit or PTS #:6919
 *  Testing: PFS EATS, Overlapping PFS Sats.
 *  Module(s):
 * 	pfs_msgutil.c
 *
 * Revision 1.6.4.2  1994/02/15  23:58:47  dbm
 * Added code to retry the Mach IPC calls if a non terminating signal
 * was sent during an I/O operation.
 *
 *  Reviewer: Charlie Johnson
 *  Risk:L
 *  Benefit or PTS #:6919
 *  Testing: Ran overlapping PFS Sat tests, Ran PFS Eats.
 *  Module(s):
 * 	pfs_msgutil.c
 *
 * Revision 1.6.4.1  1994/01/27  01:40:31  brad
 * Added a workaround for PTS #7082 ... when the -plk NX switch is used,
 * vm_copy() does not preserve the 'wired' state of the VM pages in the
 * user's buffer.  Workaround by not using vm_copy() when -plk used,
 * resulting in possible PFS performance degradation at higher bandwidths.
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 7082
 *  Testing: Verified workaround with test from PTS report, ran PFS EATs
 *     on 64 nodes, ran PFS SAT on 64 nodes.
 *  Module(s): emulator/{pfs2_user_side.c,pfs_msgutil.c,i860/emul_machdep.c}
 *
 * Revision 1.6  1993/07/16  03:03:27  dbm
 * Added token optimization functionality.
 *
 * Revision 1.5  1993/06/16  20:33:57  dbm
 * Changed all references to pfs_iomode to pfs_iomode_info to allow single
 * node applications to obtain the PFS I/O mode info.
 *
 * Revision 1.4  1993/05/25  18:39:46  dbm
 * Added fixes for readv/writev to avoid extra copies.
 *
 * Revision 1.3  1993/04/03  03:18:40  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.8  1993/03/25  01:35:36  dbm
 * Fixed up some incorrect comments and headers on some of the functions.
 *
 * Revision 1.1.2.1.2.7  1993/03/19  01:29:22  dbm
 * Added code to allow i/o mode operations to be interrupted.
 *
 * Revision 1.1.2.1.2.6  1993/02/12  17:14:34  dbm
 * Added M_GLOBAL I/O moded functionality.
 *
 * Revision 1.1.2.1.2.5  1993/01/14  20:41:21  dbm
 * Moved the include reference to pfs_iomode.h to after fdt.h include.
 *
 * Revision 1.1.2.1.2.4  1992/12/14  22:56:42  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.1.2.1.2.3  1992/12/11  21:06:37  dbm
 * Added ifdef's to remove mapped file dependencies on file tokens.
 *
 * Revision 1.1.2.1.2.2  1992/12/03  00:16:29  dbm
 * Updated for pfs i/o mode information in the fdte entry.
 *
 * Revision 1.2  1992/11/30  22:09:10  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/25  02:48:07  dbm
 * Added changes to support mapped files with PFS I/O modes.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:01:26  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/10  16:35:47  cfj
 * Put into NX branch.
 *
 * Revision 1.1  1992/11/05  22:16:10  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.1  1992/10/22  15:17:20  dbm
 * New for PFS functionality.
 *
 * Revision 1.3  1992/08/06  17:59:19  brad
 * Added #ifdef PFS wrapper.
 *
 * Revision 1.2  92/08/04  16:36:42  dbm
 * Added standard headers to all of the functions and also lined up 
 * the comments so that they looked like OSF comments.
 * 
 *
 */

#ifdef PFS
#define EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/std_types.h>
#include <mach/mach_types.h>
#include <sys/estat.h>
#include <sys/errno.h>
#include <sys/uio.h>
#include <uxkern/sthread.h>
#include "emul.h"
#include "fdt.h"
#include "pfs_iomode.h"

/*
 * Iomode message header definitions:
 */
#define	PFS_IOMODE_MSG_ID		86000
#define	PFS_IOMODE_TOKEN_ID		86001
#define PFS_GLOBAL_DATA_ID		86003
#define TMGR_RPLY_MSG_ID		86010
#define TMGR_REQ_MSG_ID			86011

/*
 * Iomode Message data definitions:
 */
typedef struct {
	mach_msg_header_t 	Head;
	mach_msg_type_t 	msg_dataType;
	iomode_msg 		msg_data;
} IOMODE_MSG;

typedef struct {
	mach_msg_header_t 	Head;
	mach_msg_type_t		hdr_dataType;
	global_hdr		hdr_data;
	mach_msg_type_long_t 	msg_dataType;
	char			*msg_data;
} global_msg;

/*
 * Global message structure, sent via Mach IPC:
 */
typedef struct {
	int	op_type;	/* Operation type. */
	int	count;		/* Count. */
	int	retval;		/* Return value from I/O operation. */
	int	error;		/* Error value from I/O operation. */
	int	iovcnt;		/* Vector count. */
} global_vec_hdr;

typedef struct {
	mach_msg_type_long_t	msg_dataType;
	char			*msg_data;
} global_vec_data;

#define	MAX_GLOBAL_VECTORS	16

typedef struct {
	mach_msg_header_t	Head;
	mach_msg_type_t		hdr_dataType;
	global_vec_hdr		hdr_data;
	global_vec_data		msg_data[MAX_GLOBAL_VECTORS];
} global_vec_msg;

/*
 * Global Header:
 */
static mach_msg_type_t msg_hdr = {
	/* msgt_name       */	MACH_MSG_TYPE_INTEGER_32,
	/* msgt_size       */	32,
	/* msgt_number     */  	sizeof(global_hdr) / 4,
	/* msgt_inline     */ 	TRUE,
	/* msgt_longform   */ 	FALSE,
	/* msgt_deallocate */ 	FALSE,
	/* msgt_unused     */ 	0 
};

/*
 * Global Vector Header:
 */
static mach_msg_type_t msg_vec_hdr = {
	/* msgt_name       */	MACH_MSG_TYPE_INTEGER_32,
	/* msgt_size       */	32,
	/* msgt_number     */	sizeof(global_vec_hdr) / 4,
	/* msgt_inline     */	TRUE,
	/* msgt_longform   */	FALSE,
	/* msgt_deallocate */	FALSE,
	/* msgt_unused	   */	0
};


/*
 * Global data:
 */
static mach_msg_type_long_t msg_data = {
	{
		/* msgt_name = */		0,
		/* msgt_size = */		0,
		/* msgt_number = */		0,
		/* msgt_inline = */		FALSE,
		/* msgt_longform = */		TRUE,
		/* msgt_deallocate = */		FALSE,
		/* msgt_unused = */		0
	},
		/* msgtl_name = */	8,
		/* msgtl_size = */	8,
		/* msgtl_number = */	0,
};

/*
 * Token Manager messages:
 */
typedef struct {
	mach_msg_header_t 	Head;
	mach_msg_type_t 	msg_dataType;
	tmgr_req_msg		msg_data;
} TMGR_REQ_MSG;

typedef struct {
	mach_msg_header_t 	Head;
	mach_msg_type_t 	msg_dataType;
	tmgr_rply_msg		msg_data;
} TMGR_RPLY_MSG;

/*
 * External References:
 */
extern boolean_t               suspend_is_exit;
extern boolean_t               must_suspend;

/*
 * NAME:	giomode_op
 *
 *
 * DESCRIPTION:
 * 		This function is used to perform a global iomode operation.
 * 		This function sets up and takes care of all of the necessary 
 * 		communicaton to perform a global operation.  An operation 
 * 		specific function is passed as a parameter and is called
 * 		at various times in the communication.  The actual actions
 *		that are required for each global operations are provided 
 *		by the operation specific function.  The operation specific
 *		function is called in various states of the global operation,
 *		the state is passed as a parameter. 
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table index.
 *
 *		operation	Pointer to function that is called at various
 *				states in the gop protocol.
 *
 *		data		Pointer to data that is passed to the above
 *				function when called.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 */
int
giomode_op(fdte, operation, data)
fdt_entry_t	*fdte;
int		(*operation)();		/* Pointer to operation function. */
char 		*data;			/* Pointer to operation data. */
{
	int		result = ESUCCESS;
	pfs_iomode_t	*iomode_p;
	int		msg_result;
	iomode_msg	out_msg;
	iomode_msg	in_msg;

	iomode_p = fdte->pfs_iomode_info;

	in_msg.error = EIO;
	if (iomode_p->my_node_number == 0 ) {

		/*
		 * Build the starting message, by calling the 
		 * operation specific function:
		 */
		if ((result = (* operation) (fdte, &out_msg, data, IOOP_START))) { 
			return result;
		}
		/*
		 * Send the message to my neighbor:
		 */
		if ((result = snd_iomode_msg( iomode_p->syncout_port, &out_msg))) {
			return result;
		}
		/*
		 * Receive the message from the last node:
		 */
		if (result = rcv_iomode_msg(	iomode_p->syncin_port,
						&in_msg)) {
			return result;
		}
		/*
		 * Handle the operation, by calling the operation
		 * specific message.
		 */
		result = ( *operation )(fdte, &in_msg, data,  IOOP_END);

	} else {
		/*
		 * Non zero nodes: Receive the message:
		 */
		if (result = rcv_iomode_msg(	iomode_p->syncin_port,
						&in_msg)) {
			return result;
		}

		/*
		 * Call the operation specific function:
		 */
		result = ( *operation ) (fdte, &in_msg, data, IOOP_RCV);

		/*
		 * Pass the (operation modified) message on to my neigbor. 	
		 */
	
		if (msg_result = snd_iomode_msg(iomode_p->syncout_port,
						&in_msg)) {
			return msg_result;
		}
	}
	return result;
}


/*
 * NAME:	rcv_iomode_msg
 *
 *
 * DESCRIPTION:
 *		This function is used to receive an iomode message
 *		from the specified port number.
 *
 * PARAMETERS:
 *
 *		port		Mach port to receive the message on.
 *
 *		ret_msg		Pointer to an iomode_msg structure to
 *				which the contents of the message 
 *				will be copied.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
int
rcv_iomode_msg (port, ret_msg)
mach_port_t 	port;		
iomode_msg	*ret_msg;
{
	int			error = ESUCCESS;
	kern_return_t           d_rtn;
	mach_msg_return_t 	msg_ret;
	IOMODE_MSG 		msg;
	transaction_id_t	trans_id;
	boolean_t		interrupt;

	/*
	 * Register the operation so that the callback thread
	 * will call the thread_abort() function.  This will 
 	 * knock the mach_msg() out with the MACH_RCV_INTERRUPTED
	 * error.
	 */
restart:
	isc_register_chk_async(MACH_PORT_NULL, &trans_id);
	/*
	 * Receive the Mach message from the port:
	 */
	msg_ret = mach_msg(&msg.Head,
			   MACH_RCV_MSG | MACH_RCV_INTERRUPT,
			   0,
			   sizeof(IOMODE_MSG), 
			   port, 
			   MACH_MSG_TIMEOUT_NONE,
			   MACH_PORT_NULL);

	isc_deregister(&interrupt);

	if (msg_ret == MACH_MSG_SUCCESS) {
		/*
		 * Make sure the it is the correct type of message.
		 */
		if (msg.Head.msgh_id == PFS_IOMODE_MSG_ID) {
			bcopy(&msg.msg_data, ret_msg, sizeof(iomode_msg));
		} else if (msg.Head.msgh_id == MACH_NOTIFY_NO_SENDERS) {
		        /*
			 * Deallocate the port any future references to it
			 * by the user's application will cause an immediate
			 * error:
			 */
		        d_rtn = mach_port_deallocate(mach_task_self(),
						     port);
		        error = EIO;
			EPRINT(( "rcv_iomode_msg: mach_msg: %s\n",
				mach_error_string(msg_ret)));
		} else {
			error = EIO;
			EPRINT(( "rcv_iomode_msg: wrong message type: %d\n",
				  msg.Head.msgh_id));	
		}

	} else if (msg_ret == MACH_RCV_INTERRUPTED){
		if (!suspend_is_exit) {
			/*
			 * Handle restarting the IPC if suspended:
			 */
			if (must_suspend) {
				syscall_suspend_barrier();
			}
			goto restart;
		} else {
			error = EINTR;
		}
	} else {
		error = EIO;	
		EPRINT(( "rcv_iomode_msg: mach_msg: %s\n",
			mach_error_string(msg_ret)));
	}
	return error;
}


/*
 * NAME:	rcv_iomode_token
 *
 *
 * DESCRIPTION:
 *		This function is used to receive an iomode token
 *		from the specified port number.
 *
 * PARAMETERS:
 *
 *		port		Mach port to receive the message on.
 *
 *		eoffset		Pointer to an esize_t structure to
 *				which the contents of the latest  
 *				file offset will be copied.
 *
 *		elength		Pointer to an esize_t structure
 *				which the contents of the latest 
 *				file length will be copied.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
int
rcv_iomode_token (port, eoffset, elength)
mach_port_t 	port;		
esize_t		*eoffset;
esize_t		*elength;
{
	int			error = ESUCCESS;
	kern_return_t           d_rtn;
	mach_msg_return_t 	msg_ret;
	IOMODE_MSG 		msg;
	transaction_id_t	trans_id;
	boolean_t		interrupt;

	/*
	 * Register the operation so that the callback thread
	 * will call the thread_abort() function.  This will 
 	 * knock the mach_msg() out with the MACH_RCV_INTERRUPTED
	 * error.
	 */
restart:
	isc_register_chk_async(MACH_PORT_NULL, &trans_id);
	/*
	 * Receive the message:
	 */
	msg_ret = mach_msg(&msg.Head,
			   MACH_RCV_MSG | MACH_RCV_INTERRUPT,
			   0,
			   sizeof(msg), 
			   port, 
			   MACH_MSG_TIMEOUT_NONE,
			   MACH_PORT_NULL);

	isc_deregister(&interrupt);

	if (msg_ret == MACH_MSG_SUCCESS) {
		/*
		 * Make sure the it is the correct type of message.
		 */
		if (msg.Head.msgh_id == PFS_IOMODE_TOKEN_ID) {
			eoffset->shigh = msg.msg_data.offset.shigh;
			eoffset->slow  = msg.msg_data.offset.slow;
			elength->shigh = msg.msg_data.length.shigh;
			elength->slow  = msg.msg_data.length.slow;
		} else if (msg.Head.msgh_id == MACH_NOTIFY_NO_SENDERS) {
		        /*
			 * Deallocate the port any future references to it
			 * by the user's application will cause an immediate
			 * error:
			 */
		        d_rtn = mach_port_deallocate(mach_task_self(),
						     port);
		        error = EIO;
			EPRINT(( "rcv_iomode_token: mach_msg: %s\n",
				mach_error_string(msg_ret)));
		} else {
			error = EIO;
			EPRINT(( "rcv_iomode_token: wrong message type: %d\n",
				  msg.Head.msgh_id));	
		}

	} else if (msg_ret == MACH_RCV_INTERRUPTED){
		if (!suspend_is_exit) {
                        /*
                         * This IPC is used only for M_SYNC
                         * when receiving a token message around
                         * the loop.  This operation is considered
                         * to be uninterruptable until after the
                         * whole I/O (all of the nodes) has completed.
                         */
			goto restart;
		} else {
			error = EINTR;
		}
	} else {
		error = EIO;	
		EPRINT(( "rcv_iomode_msg: mach_msg: %s\n",
			mach_error_string(msg_ret)));
	}
	return error;
}



/*
 * NAME:	snd_iomode_msg
 *
 *
 * DESCRIPTION:
 *		This function is used to send an iomode message
 *		to the specified Mach port number.
 *
 * PARAMETERS:
 *
 *		node_port	Mach port to send the message on.
 *
 *		msg_data	Pointer to an iomode_msg structure from
 *				which the contents of the message 
 *				will be copied.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
int
snd_iomode_msg (node_port, msg_data)
	mach_port_t 	node_port;
	iomode_msg 	*msg_data;
{
	int			error = ESUCCESS;
	IOMODE_MSG		msg_out;
	kern_return_t		mach_error;
	transaction_id_t        trans_id;
	boolean_t               interrupt;

	/*
	 * Build the Mach message:
	 */

	msg_out.msg_dataType.msgt_name 		= MACH_MSG_TYPE_INTEGER_32;
	msg_out.msg_dataType.msgt_size 		= 32;
	msg_out.msg_dataType.msgt_number 	= sizeof(iomode_msg) / 4;
	msg_out.msg_dataType.msgt_inline 	= TRUE;
	msg_out.msg_dataType.msgt_longform 	= FALSE;
	msg_out.msg_dataType.msgt_deallocate 	= FALSE;
	msg_out.msg_dataType.msgt_unused 	= 0;

	bcopy(msg_data, &msg_out.msg_data, sizeof(iomode_msg));

	msg_out.Head.msgh_bits 			= MACH_MSGH_BITS(19, 0);
	msg_out.Head.msgh_remote_port 		= node_port;
	msg_out.Head.msgh_local_port 		= MACH_PORT_NULL;
	msg_out.Head.msgh_seqno 		= 0;
	msg_out.Head.msgh_id 			= PFS_IOMODE_MSG_ID;


	/*
	 * Register the operation so that the callback thread
	 * will call the thread_abort() function.  This will
	 * knock the mach_msg() out with the MACH_SEND_INTERRUPTED
	 * error.
	 */
restart:
	isc_register_chk_async(MACH_PORT_NULL, &trans_id);

	mach_error = mach_msg(	&msg_out.Head, 
				(MACH_SEND_MSG | MACH_MSG_OPTION_NONE |
				 MACH_SEND_INTERRUPT),
				sizeof(IOMODE_MSG),
			 	0,
				MACH_PORT_NULL, 
				MACH_MSG_TIMEOUT_NONE, 
				MACH_PORT_NULL);

	isc_deregister(&interrupt);

	if (mach_error == KERN_SUCCESS) {
		return error;
	} else if (mach_error == MACH_SEND_INTERRUPTED) {
		if (!suspend_is_exit) {
			/*
			 * Handle restarting the IPC if suspended:
			 */
			if (must_suspend) {
				syscall_suspend_barrier();
			}
			goto restart;
		} else {
			error = EINTR;
		}
	} else {
		EPRINT(( "snd_iomode_msg: mach_msg: %s\n",
			mach_error_string(mach_error)));
		error = EIO;
	}

	return error;
}



/*
 * NAME:	snd_iomode_token
 *
 *
 * DESCRIPTION:
 *		This function is used to send an iomode token
 *		to the specified port number.
 *
 * PARAMETERS:
 *
 *		node_port	Mach port to send token the on.
 *
 *		offset		Pointer to an esize_t structure from
 *				which the contents of the latest  
 *				file offset will be copied.
 *
 *		length		Pointer to an esize_t structure from
 *				which the contents of the latest  
 *				file length will be copied.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
int
snd_iomode_token (node_port, offset, length)
	mach_port_t 	node_port;
	esize_t 	*offset;
	esize_t 	*length;
{
	int			error = ESUCCESS;
	IOMODE_MSG		msg_out;
	kern_return_t		mach_error;
	transaction_id_t	trans_id;
	boolean_t		interrupt;

	/*
	 * Build the Mach message:
	 */

	msg_out.msg_dataType.msgt_name 		= 2;
	msg_out.msg_dataType.msgt_size 		= 32;
	msg_out.msg_dataType.msgt_number 	= sizeof (iomode_msg) / 4;
	msg_out.msg_dataType.msgt_inline 	= TRUE;
	msg_out.msg_dataType.msgt_longform 	= FALSE;
	msg_out.msg_dataType.msgt_deallocate 	= FALSE;
	msg_out.msg_dataType.msgt_unused 	= 0;

	msg_out.msg_data.op_type 		= 0;
	msg_out.msg_data.error 			= 0;
	msg_out.msg_data.offset.shigh 		= offset->shigh;
	msg_out.msg_data.offset.slow 		= offset->slow;
	msg_out.msg_data.length.shigh 		= length->shigh;
	msg_out.msg_data.length.slow 		= length->slow;

	msg_out.Head.msgh_bits 			= MACH_MSGH_BITS(19, 0);
	/* msgh_size passed as argument */
	msg_out.Head.msgh_remote_port 		= node_port;
	msg_out.Head.msgh_local_port 		= MACH_PORT_NULL;
	msg_out.Head.msgh_seqno 		= 0;
	msg_out.Head.msgh_id 			= PFS_IOMODE_TOKEN_ID;

	/*
	 * Register the operation so that the callback thread
	 * will call the thread_abort() function.  This will
	 * knock the mach_msg() out with the MACH_SEND_INTERRUPTED
	 * error.
	 */
restart:
	isc_register_chk_async(MACH_PORT_NULL, &trans_id);

	mach_error = mach_msg(	&msg_out.Head, 
				(MACH_SEND_MSG | MACH_MSG_OPTION_NONE |
				 MACH_SEND_INTERRUPT),
				sizeof(msg_out),
			 	0,
				MACH_PORT_NULL, 
				MACH_MSG_TIMEOUT_NONE, 
				MACH_PORT_NULL);

	isc_deregister(&interrupt);
	if (mach_error == KERN_SUCCESS) {
		return error;
	} else if (mach_error == MACH_SEND_INTERRUPTED) {
		if (!suspend_is_exit) {
			/*
			 * This IPC is used only for M_SYNC
			 * when sending a token message around
			 * the loop.  This operation is considered
			 * to be uninterruptable until after the 
			 * whole I/O (all of the nodes) has completed.
			 */
			goto restart;
		} else {
			error = EINTR;
		}
	} else {
		EPRINT(( "snd_iomode_token: mach_msg: %s\n",
			mach_error_string(mach_error)));
		error = EIO;
	}

	return error;
}


/*
 * NAME:	snd_global
 *
 *
 * DESCRIPTION:
 *		This function is used to send an global data
 *		message.
 *
 * PARAMETERS:
 *
 *		node_ports	Mach ports to send the message on.
 *
 *		hdr_data	Pointer to an global_hdr structure from
 *				which the contents of the message 
 *				will be copied.
 *
 *		data		Pointer to the data to send.
 *
 *		count		Actual number of bytes of data.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
static int
snd_global (node_ports, hdr_data, data, count)
	mach_port_t 	*node_ports;
	global_hdr 	*hdr_data;
	char		*data;
	int		count;
{

	global_msg		Msg;
	int			error = ESUCCESS;
	kern_return_t		mach_error;
	int			port_num = 0;
	register global_msg	*MsgPtr = &Msg;
	transaction_id_t	trans_id;
	boolean_t		interrupt;

	/*
	 * Fill in the message header first:
	 */

	MsgPtr->hdr_dataType = msg_hdr;
	bcopy(hdr_data, &MsgPtr->hdr_data, sizeof(global_hdr));
	MsgPtr->Head.msgh_bits 	= MACH_MSGH_BITS_COMPLEX |
		MACH_MSGH_BITS(19, 0);
	MsgPtr->Head.msgh_local_port 		= MACH_PORT_NULL;
	MsgPtr->Head.msgh_seqno 		= 0;
	MsgPtr->Head.msgh_id 			= PFS_GLOBAL_DATA_ID;

	/*
	 * Build the message data portion of the message:
	 */
	MsgPtr->msg_dataType = msg_data;
	MsgPtr->msg_data = data;

	if ((hdr_data->op_type == PFS_OP_READ) || 
	    (hdr_data->op_type == PFS_OP_READV)) {
		MsgPtr->msg_dataType.msgtl_number = count;
	} else {
		/*
		 * Don't send any data if write operation.
		 */
		MsgPtr->msg_dataType.msgtl_number = 0;
	}

	/*
	 * Send the global data to all of the destinations:
	 */
	while((!error) && (node_ports[port_num] != MACH_PORT_NULL)) {

	  restart:
		MsgPtr->Head.msgh_remote_port 		= node_ports[port_num];
		isc_register_chk_async(MACH_PORT_NULL, &trans_id);
		mach_error = mach_msg(	&MsgPtr->Head, 
					MACH_SEND_MSG | MACH_SEND_INTERRUPT,
					sizeof(global_msg),
				 	0,
					MACH_PORT_NULL, 
					MACH_MSG_TIMEOUT_NONE, 
					MACH_PORT_NULL);

		isc_deregister(&interrupt);
		if (mach_error == KERN_SUCCESS) {
			port_num++;
		} else if (mach_error == MACH_SEND_INTERRUPTED) {
			if (!suspend_is_exit) {
				/*
				 * Handle restarting the IPC if suspended:
				 */
				if (must_suspend) {
					syscall_suspend_barrier();
				}
				goto restart;
			} else {
				error = EINTR;
			}
		} else {		
			error = EIO;
			EPRINT(( "snd_global_hdr: mach_msg: %s\n",
				mach_error_string(mach_error)));
		}
	}
	return error;
}



/*
 * NAME:	rcv_global
 *
 *
 * DESCRIPTION:
 *		This function is used to receive an global 
 *		message and possibly forward it on to other nodes.
 *
 * PARAMETERS:
 *
 *		in_port		Mach port to receive the message on.
 *
 *		rcv_msg		Pointer to an global_hdr structure to
 *				which the contents of the message 
 *				will be copied.
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
static int
rcv_global(in_port,  rcv_msg)
mach_port_t 	in_port;		
global_msg	*rcv_msg;
{
	kern_return_t           d_rtn;
	int			error = ESUCCESS;
	mach_msg_return_t 	msg_ret;
	transaction_id_t	trans_id;
	boolean_t		interrupt;

	/*
	 * Receive the Mach message from the port:
	 */
restart:
	isc_register_chk_async(MACH_PORT_NULL, &trans_id);
	msg_ret = mach_msg(&rcv_msg->Head,
			   MACH_RCV_MSG | MACH_RCV_INTERRUPT,
			   0,
			   sizeof(global_msg), 
			   in_port, 
			   MACH_MSG_TIMEOUT_NONE,
			   MACH_PORT_NULL);

	isc_deregister(&interrupt);

	if (msg_ret == MACH_MSG_SUCCESS) {
		/*
		 * Make sure the it is the correct type of message.
		 */
		if (rcv_msg->Head.msgh_id == MACH_NOTIFY_NO_SENDERS) {
		        /*
			 * Deallocate the port any future references to it
			 * by the user's application will cause an immediate
			 * error:
			 */
		        d_rtn = mach_port_deallocate(mach_task_self(),
						     in_port);
		        error = EIO;
			EPRINT(( "rcv_global: mach_msg: %s\n",
				mach_error_string(msg_ret)));
		} else if (rcv_msg->Head.msgh_id != PFS_GLOBAL_DATA_ID) {
			error = EIO;
			EPRINT(( "rcv_global_hdr: wrong message type: %d\n",
				  rcv_msg->Head.msgh_id));	
		}
	} else if (msg_ret == MACH_RCV_INTERRUPTED) { 
		if (!suspend_is_exit) {
			/*
			 * Handle restarting the IPC if suspended:
			 */
			if (must_suspend) {
				syscall_suspend_barrier();
			}
			goto restart;
		} else {
			error = EINTR;
		}
	} else {
		error = EIO;
		EPRINT(( "rcv_global: mach_msg: %s\n",
			mach_error_string(msg_ret)));
	}
		
	return error;
}


/*
 * NAME:	snd_global_vec
 *
 *
 * DESCRIPTION:
 *		This function is used to send a global data
 *		message for a vector of data arrays.
 *
 * PARAMETERS:
 *
 *		node_ports	Mach ports to send the message on.
 *
 *		msg		Pointer to an global_vec_msg structure to
 *				be used to send to the nodes.
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
static int
snd_global_vec(node_ports, msg)
	mach_port_t 	*node_ports;
	global_vec_msg 	*msg;
{

	int			error = ESUCCESS;
	kern_return_t		mach_error;
	int			port_num = 0;
	transaction_id_t	trans_id;
	boolean_t		interrupt;


	/*
	 * Send the global data to all of the destinations:
	 */
	while((!error) && (node_ports[port_num] != MACH_PORT_NULL)) {

	  restart:
		msg->Head.msgh_remote_port = node_ports[port_num];
		isc_register_chk_async(MACH_PORT_NULL, &trans_id);
		mach_error = mach_msg(	&msg->Head, 
					MACH_SEND_MSG | MACH_SEND_INTERRUPT,
					sizeof(global_vec_msg),
				 	0,
					MACH_PORT_NULL, 
					MACH_MSG_TIMEOUT_NONE, 
					MACH_PORT_NULL);

		isc_deregister(&interrupt);

		if (mach_error == KERN_SUCCESS) {
			port_num++;
		} else if (mach_error == MACH_SEND_INTERRUPTED) {
			if (!suspend_is_exit) {
				/*
				 * Handle restarting the IPC if suspended:
				 */
				if (must_suspend) {
					syscall_suspend_barrier();
				}
				goto restart;
			} else {
				error = EINTR;
			}
		} else {
			error = EIO;
			EPRINT(( "snd_global_vec_hdr: mach_msg: %s\n",
				mach_error_string(mach_error)));
			break;
		}
	}
	return error;
}


/*
 * NAME:	rcv_global_vec
 *
 *
 * DESCRIPTION:
 *		This function is used to receive an global vector
 *		message and possibly forward it on to other nodes.
 *
 * PARAMETERS:
 *
 *		in_port		Mach port to receive the message on.
 *
 *		msg		Pointer to an global_vec_msg structure to
 *				which the contents of the message 
 *				will be copied.
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
static int
rcv_global_vec(in_port,  msg)
mach_port_t 	in_port;		
global_msg	*msg;
{
	int			error = ESUCCESS;
	kern_return_t           d_rtn;
	mach_msg_return_t 	msg_ret;
	transaction_id_t 	trans_id;
	boolean_t		interrupt;

	/*
	 * Receive the Mach message from the port:
	 */
restart:
	isc_register_chk_async(MACH_PORT_NULL, &trans_id);

	msg_ret = mach_msg(&msg->Head,
			   MACH_RCV_MSG | MACH_RCV_INTERRUPT,
			   0,
			   sizeof(global_vec_msg), 
			   in_port, 
			   MACH_MSG_TIMEOUT_NONE,
			   MACH_PORT_NULL);

	isc_deregister(&interrupt);

	if (msg_ret == MACH_MSG_SUCCESS) {
		/*
		 * Make sure the it is the correct type of message.
		 */
		if (msg->Head.msgh_id == MACH_NOTIFY_NO_SENDERS) {
		        /*
			 * Deallocate the port any future references to it
			 * by the user's application will cause an immediate
			 * error:
			 */
		        d_rtn = mach_port_deallocate(mach_task_self(),
						     in_port);
		        error = EIO;
			EPRINT(( "rcv_global_vec: mach_msg: %s\n",
				mach_error_string(msg_ret)));
		} else if (msg->Head.msgh_id != PFS_GLOBAL_DATA_ID) {
			error = EIO;
			EPRINT(( "rcv_global_hdr: wrong message type: %d\n",
				  msg->Head.msgh_id));	
		}
	} else if (msg_ret == MACH_RCV_INTERRUPTED) {
		if (!suspend_is_exit) {
			/*
			 * Handle restarting the IPC if suspended:
			 */
			if (must_suspend) {
				syscall_suspend_barrier();
			}
			goto restart;
		} else {
			error = EINTR;
		}
	} else {
		error = EIO;
		EPRINT(( "rcv_global: mach_msg: %s\n",
			mach_error_string(msg_ret)));
	}
		
	return error;
}



/*
 * NAME:	pfs_global_snd
 *
 *
 * DESCRIPTION:
 *		This function is used to broadcast file data 
 *		from node zero to the other nodes in the 
 *		application for use by the M_GLOBAL PFS I/O
 *		mode.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry for the 
 *				source file.
 *
 *		op_type		Type of operation being performed, used
 *				for cecking operation consistency.
 *
 *		data		Pointer to the data to send.
 *
 *		count		Number of bytes of data requested.
 *
 *		retval		Return value from the I/O operation.
 *
 *		error		Error from the I/O operation.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
int
pfs_global_snd(fdte, op_type, data, count, retval, error)
fdt_entry_t	*fdte;
int		op_type;
char 		*data;
int		count;
int		retval;
int		error;
{
	global_hdr	msg_hdr;

	/*
	 * Build the message header:
	 */
	msg_hdr.op_type		= op_type;
	msg_hdr.count 		= count;
	msg_hdr.retval		= retval;
	msg_hdr.error		= error;

	/*
	 * Send the message and data to the other nodes in the system
	 * using the Mach ports.
	 */
	error = snd_global(fdte->pfs_iomode_info->dataout_ports,
			   &msg_hdr,
			   data,
			   retval);
	return error;

}


/*
 * NAME:	pfs_global_rcv
 *
 *
 * DESCRIPTION:
 *		This function is used to broadcast file data 
 *		from node zero to the other nodes in the 
 *		application for use by the M_GLOBAL PFS I/O
 *		mode.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry for the 
 *				source file.
 *
 *		op_type		Type of operation being performed, used
 *				for checking operation consistency.
 *
 *		data		Pointer to the data to receive.
 *
 *		count		Number of bytes of data requested.
 *
 *		rval		Pointer to the return value of the
 *				actual I/O operation.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
int
pfs_global_rcv(fdte, op_type, data, count, rval)
fdt_entry_t	*fdte;
int		op_type;
char		*data;
int		count;
int		*rval;
{
	global_msg 	rcv_msg;
	int		error;

	/*
	 * Receive the header:
	 */
	if ((error = rcv_global(fdte->pfs_iomode_info->datain_port,
				&rcv_msg))) {
		return error;
	}

	/*
	 * Check for error condition on message:
	 */
	if (rcv_msg.hdr_data.error) {
		/*
		 * Original operation received an error:
		 */
		error = rcv_msg.hdr_data.error;
	} else {

		/*
	 	 * Check for parameter consistency:
	 	 */

		if (( op_type != rcv_msg.hdr_data.op_type) ||
	    	    ( count   != rcv_msg.hdr_data.count)) {
			error = EMIXIO;
			rcv_msg.hdr_data.error = error;
		}
	}

	*rval = rcv_msg.hdr_data.retval;

	/*
	 * Forward the message to the next nodes:
	 */

	if (error = snd_global(fdte->pfs_iomode_info->dataout_ports, 
			 	&rcv_msg.hdr_data, rcv_msg.msg_data, *rval)) {
		return error;

	} else {
		error = rcv_msg.hdr_data.error;
	}
		
	if ((!error) && (rcv_msg.hdr_data.retval > 0)) {
		if (op_type == PFS_OP_READ) {
			error = pfs_copy(rcv_msg.msg_data, data,
					 rcv_msg.msg_dataType.msgtl_number);
		} else if (op_type == PFS_OP_READV) {
			char **data_adr = (char **)data;
			/*
			 * Just send back the address of the data instead
			 * of the actual data.  This is because readv 
			 * gathers the data into different buffers.
			 */
			*data_adr = (char *)rcv_msg.msg_data;
		}
	}

	/*
	 * Deallocate the data:
	 */
	if ((rcv_msg.hdr_data.retval > 0) && (op_type == PFS_OP_READ)) { 
		vm_deallocate (mach_task_self(), 
			       (vm_address_t)rcv_msg.msg_data,
			       (vm_size_t)rcv_msg.msg_dataType.msgtl_number); 
	}

	return error;
}


/*
 * NAME:	pfs_global_snd_vec
 *
 *
 * DESCRIPTION:
 *		This function is used to broadcast multiple vectors
 *		of file data from node zero to the other nodes in the 
 *		application for use by the M_GLOBAL PFS I/O mode.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry for the 
 *				source file.
 *
 *		op_type		Type of operation being performed, used
 *				for checking operation consistency.
 *
 *		iov		Array of iovec structures.
 *
 *		iovcnt		Number of iovec structures.
 *
 *		count		Total number of bytes of data requested.
 *
 *		retval		Return value from the I/O operation.
 *
 *		error		Error from the I/O operation.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
int
pfs_global_snd_vec(fdte, op_type, iov, iovcnt, count, retval, error)
fdt_entry_t	*fdte;
int		op_type;
struct 	iovec	*iov;
int		iovcnt;
int		count;
int		retval;
int		error;
{
	global_vec_msg	msg;
	int		i;

	/*
	 * Build the message header:
	 */
	msg.hdr_data.op_type		= op_type;
	msg.hdr_data.count 		= count;
	msg.hdr_data.retval		= retval;
	msg.hdr_data.error		= error;
	msg.hdr_data.iovcnt		= iovcnt;

	msg.hdr_dataType 		= msg_vec_hdr;
	msg.Head.msgh_bits  	        = MACH_MSGH_BITS_COMPLEX |
					    MACH_MSGH_BITS(19, 0);
	msg.Head.msgh_local_port	= MACH_PORT_NULL;
	msg.Head.msgh_seqno		= 0;
	msg.Head.msgh_id		= PFS_GLOBAL_DATA_ID;

	/*
	 * Build the message data:
	 */
	
	for(i = 0; i< MAX_GLOBAL_VECTORS; i++) {
		msg.msg_data[i].msg_dataType = msg_data;
		msg.msg_data[i].msg_data = NULL;
		msg.msg_data[i].msg_dataType.msgtl_number = 0;
	}

	for(i = 0; i< iovcnt; i++ ) {
		msg.msg_data[i].msg_dataType = msg_data;
		msg.msg_data[i].msg_data = iov[i].iov_base;
		msg.msg_data[i].msg_dataType.msgtl_number = 
			(((op_type == PFS_OP_READ) ||
			  (op_type == PFS_OP_READV)) ? iov[i].iov_len : 0);
	}

	/*
	 * Send the message and data to the other nodes in the system
	 * using the Mach ports.
	 */
	error = snd_global_vec(	fdte->pfs_iomode_info->dataout_ports, &msg);
	return error;
}


/*
 * NAME:	pfs_global_rcv_vec
 *
 *
 * DESCRIPTION:
 *		This function is used to receive a set of vectors
 *		from other nodes in the system when using the 
 *		M_GLOBAL PFS I/O mode.
 *
 * PARAMETERS:
 *
 *		fdte		File descriptor table entry for the 
 *				source file.
 *
 *		op_type		Type of operation being performed, used
 *				for checking operation consistency.
 *
 *		iov		Array of iovec structures.
 *
 *		iovcnt		Number of iovec structures.
 *
 *		count		Number of bytes of data requested.
 *
 *		retval		Pointer to the return value of the
 *				actual I/O operation.
 *
 * RETURNS:
 *
 *	ESUCCESS        -       if successful
 *	error number    -       if an error occurred.
 *
 */
int
pfs_global_rcv_vec(fdte, op_type, iov, iovcnt, count, rval)
fdt_entry_t	*fdte;
int		op_type;
struct iovec	*iov;
int		iovcnt;
int		count;
int		*rval;
{
	global_vec_msg 	msg;
	int             copy_error;
	int		error;
	int		i;


	/*
	 * Receive the message:
	 */
	if ((error = rcv_global_vec(fdte->pfs_iomode_info->datain_port,
				    &msg))) {
		return error;
	}

	/*
	 * Check for error condition on message:
	 */
	if (msg.hdr_data.error) {
		/*
		 * Original operation received an error:
		 */
		error = msg.hdr_data.error;
	} else {

		/*
	 	 * Check for parameter consistency:
	 	 */

		if (( op_type != msg.hdr_data.op_type) ||
	    	    ( count   != msg.hdr_data.count)) {
			error = EMIXIO;
			msg.hdr_data.error = error;
		}
	}

	*rval = msg.hdr_data.retval;

	/*
	 * Forward the message to the next nodes:
	 */
	msg.hdr_dataType		= msg_vec_hdr;
	msg.Head.msgh_bits		= MACH_MSGH_BITS_COMPLEX |
						MACH_MSGH_BITS(19, 0);
        msg.Head.msgh_local_port	= MACH_PORT_NULL;
        msg.Head.msgh_seqno		= 0;
	msg.Head.msgh_id		= PFS_GLOBAL_DATA_ID;

	if (error = snd_global_vec(fdte->pfs_iomode_info->dataout_ports, &msg)) {
		return error;

	} else {
		error = msg.hdr_data.error;
	}

	/*
	 * Copy the data into the user's buffer.
	 */
		
	if ((!error) && (msg.hdr_data.retval > 0)) {
		for(i=0; i < msg.hdr_data.iovcnt; i++ ) {
			int ccount = msg.msg_data[i].msg_dataType.msgtl_number;

			/*
			 * Check again for parameter consistency:
			 */
			if (ccount  != iov[i].iov_len) {
				error = EMIXIO;
				continue;
			}

			copy_error = pfs_copy(msg.msg_data[i].msg_data,
					      iov[i].iov_base, ccount);
			if ((!error) && (copy_error)) {
			        error = copy_error;
			}

                	vm_deallocate(mach_task_self(),
				      (vm_address_t)msg.msg_data[i].msg_data,
				      (vm_size_t) msg.msg_data[i].msg_dataType.msgtl_number);
		}
	}

	return error;
}
#endif
