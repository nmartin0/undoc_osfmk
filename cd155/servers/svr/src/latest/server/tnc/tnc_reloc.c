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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: tnc_reloc.c,v $
 * Revision 1.13  1995/02/01  21:55:56  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.12  1994/11/18  20:44:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/06/18  00:47:53  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.10  1993/10/28  03:10:52  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.9  1993/07/14  18:35:00  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:48:35  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/05/06  19:26:21  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:29  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.7  1993/04/03  03:09:41  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.6  1993/03/29  18:26:27  cfj
 * Merge with T9.
 *
 * Revision 1.5.4.1  1993/03/29  18:17:18  cfj
 * More ux_server_thread_block/unblocking from Locus.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:26  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.5  1993/01/22  01:17:04  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 1.4  1993/01/15  02:03:05  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:17  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.12  93/06/24  12:44:03  mjl
 * [LCC bug 0301] Add missing ux_server_thread_{un,}blocking() calls around
 * mach_msg() call in tnc_ool_reloc() .
 * 
 * Revision 3.11  93/03/27  17:04:23  yazz
 * Added some ux_server_thread_blocking/unblocking() calls.
 *
 * Revision 3.10  93/01/12  13:52:53  mjl
 * Use an "extra syscall" number to allow syscall tracing with relocation ops.
 * 
 * Revision 3.9  92/12/10  17:13:00  mjl
 * Use new debug macros.  Add datagram socket arrival routines to dispatch table.
 * 
 * Revision 3.8  92/11/18  12:45:37  mjl
 * Forward declarations for i860 compiler happiness.  Also, clear the thread
 * area's "opening file pointer" data in arrival_dispatch()---it will always
 * be bogus on arrival; forcing it to NULL means VSOP_CREATE() won't fail.
 * 
 * Revision 3.7  92/10/08  17:16:20  chrisp
 * Cast added in vm_allocate() for address parameter.
 * 
 * Revision 3.6  92/09/28  13:27:17  klh
 * Turn off debugging printfs.  Add casts to pelase the ANSI compiler (klh for mjl)
 * 
 * Revision 3.5  92/08/17  13:11:39  mjl
 * Added new arrival functions for FIFO relocation.  Make server_port be first
 * arg to relocation RPCs, as is traditional.
 * 
 * Revision 3.4  92/08/08  01:47:46  jdh
 * support for send rights relocation;  server port is now
 * passed in, and also passed on to the arrival dispatch routine
 * added unix domain stream socket connect arrival routine -- jdh
 * 
 * Revision 3.3  92/07/08  09:13:52  roman
 * Change node numbers to type node_t.
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.2  92/06/26  18:02:29  mjl
 * Rename ust_XXX to un_pp_XXX.  New naming convention for the AF_UNIX
 * domain:  un_pp/pipe, un_st/stream socket, un_dg/datagram socket,
 * un_ff/FIFO, un/common routine.
 * 
 * Revision 3.1  92/06/15  17:34:57  mjl
 * Fix bogus comment in svr_tnc_ool_reloc() cleanup code.
 * 
 * Revision 3.0  92/06/11  16:03:28  mjl
 * Underlying RPC transport for relocating sockets, pipes, FIFOs.
 * 
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/errno.h>
/* #include <net/net_malloc.h> */

#include <tnc/un_debug.h>
#include <tnc/sgd.h>
#include <tnc/reloc.h>
#include <uxkern/bsd_msg.h>

extern node_t		this_node;
extern mach_port_t	mig_get_reply_port();
extern void		mig_dealloc_reply_port();

#ifdef	RELOC_DEBUG
int	force_reloc_error	= ESUCCESS;
#endif	/* RELOC_DEBUG */

/* Forward Declarations */
int		tnc_inl_reloc(mach_port_t,
			      mach_port_t,
			      sgd_t *,
			      portv_t *,
			      portv_t *);
int		tnc_ool_reloc(mach_port_t,
			      mach_port_t,
			      sgd_t *,
			      lmv_t *,
			      portv_t *,
			      portv_t *);
kern_return_t	svr_tnc_ool_reloc(struct reloc_request *, int *);


/*
 *  This file contains the common mechanism for relocating socket and
 *  other data described by a socket graph descriptor (sgd).
 *
 *  Three vectors are involved.  First, the sgd itself is a vector of
 *  entries describing the types and locations of various socket-related
 *  data structures (sockets themselves, mbufs, etc.).  The there is
 *  a large mbuf vector (lmv), which is a uio-like description of the
 *  buffers used by all the large mbufs mentioned in the sgd.  Finally
 *  there is a vector of Mach port Rcv rts that are also to be moved to
 *  the new node.  The relative order and type of entries in the sgd
 *  determines the associations between data structures, large mbuf
 *  buffers, and Rcv rts.
 *
 *  Note RELOC_PORT_ARRAY_SIZE is a hard upper limit on the number of
 *  Rcv rts, and hence the length of the portv_t, that can be
 *  relocated.  (The lengths of the sgd and lmv are theoretically
 *  unlimited.)
 */



/*
 *  Start data structure relocation operation.
 */
static int
start_relocation_op(
	mach_port_t	creds_port)
{
	uthread_t	uth;
	int		error;
	extern int	fsvr_thread_initialize(uthread_t, mach_port_t);
	extern int	nsysent;

	UNDEBUG(U_RPC, ("start_relocation_op: creds 0x%x\n", creds_port));

	uth = current_thread();

	uth->uu_syscode = 2011;
	if (error = fsvr_thread_initialize(uth, creds_port))
		return error;

	/*
	 *  Just initialize the oip areas in the proc structure.  We
	 *  use a null creds port because we don't expect any
	 *  interrupt deliveries.  Relocation ops should never have to
	 *  deal with intr_delivery() messages, because these messages
	 *  are received on file ports and during relocation no one is
	 *  doing any receiving on the relocating file ports.  Any
	 *  intr_delivery() messages are queued in the ports.
	 */
	oip_register(uth, MACH_PORT_NULL/*credsport*/, 0 /*transid*/);

	return KERN_SUCCESS;
}


static int
end_relocation_op(
	int	error)
{
	uthread_t	uth;
	extern void	fsvr_thread_terminate(uthread_t, int);

	UNDEBUG(U_RPC, ("end_relocation_op: errno %d\n", error));

	uth = current_thread();
	oip_deregister(uth);
	fsvr_thread_terminate(uth, error);
	return error;
}


/*
 *  The arrival function table.
 *
 *  Socket family, type, and protocol numbers are not sufficient to
 *  determine the sort of thing being relocated, because a particular
 *  protocol might want to relocate a socket or sockets in several
 *  different states.  For example, the AF_UNIX SOCK_STREAM protocol
 *  will want to relocate connected socket pairs such as pipes, but
 *  also unconnected sockets in various states.  In addition, FIFO
 *  relocation involves AF_UNIX sockets but the sgd also contains
 *  references to vnode structures, which pipe unmarshalling code
 *  is unprepared to handle.  Rather than examining the sgd in a
 *  heuristic fashion to see what it really represents, an "arrival
 *  code" stored in the sgd header is used to dispatch to the
 *  appropriate unmarshalling routine.
 */

extern int	un_arrival();
extern int	un_ff_arrival();
extern int	un_ff_rejoin();
extern int	un_vsock_recvconnect();
extern int	un_dg_arrival();
extern int	un_dg_pkt_arrival();

int
arrival_noop ( )
{
	return EOPNOTSUPP;
}

typedef int (*func_t)();

func_t	tnc_arrival_funcs[] = {
	un_arrival,			/* SGD_TYPE_UN_PAIR */
	un_ff_arrival,			/* SGD_TYPE_FIFO */
	un_dg_arrival,			/* SGD_TYPE_UN_DG */
	arrival_noop,			/* SGD_TYPE_NETWORK */
	un_ff_rejoin,			/* SGD_TYPE_FIFO_REJOIN */
	un_vsock_recvconnect,		/* SGD_TYPE_VSOCKCONN */
	un_dg_pkt_arrival		/* SGD_TYPE_UN_DG_PKT */
};


kern_return_t
arrival_dispatch(
	mach_port_t	server_port,
	mach_port_t	creds_port,
	sgd_t		*sgdp,
	caddr_t		data,
	lmv_t		*lmvp,
	portv_t		*rrights,
	portv_t		*srights,
	int		*error)
{
	int		arrival_code;
	kern_return_t	kr;
	uthread_t	uth;

	*error = ESUCCESS;
	arrival_code = SGD_HEADER(sgdp)->sgd_type;

	kr = start_relocation_op(creds_port);
	if (kr != KERN_SUCCESS)
		return kr;

	/*
	 *  Arrival routines typically call VSOP_CREATE(), which looks
	 *  at uth->uu_opn_filep for the opening file struct/port.
	 *  But uth->uu_opn_filep is never valid when sockets arrive
	 *  on a new node, so clear out any stale data.
	 */
	uth = current_thread();
	uth->uu_opn_filep = MACH_PORT_NULL;

	/*
	 *  Dispatch to proper arrival routine based on code number.
	 */
	if (arrival_code > SGD_MAX_TYPE || ! arrival_code) {
		UNDEBUG(U_RPC, ("arrival_dispatch: bad arrival code %d\n",
				arrival_code));
		*error = EPROGUNAVAIL;
	} else {
		*error = (*tnc_arrival_funcs[arrival_code-1])
				( (server_port), (sgdp), (data), (lmvp), 
				(rrights), (srights));
	}

	end_relocation_op(*error);
	return KERN_SUCCESS;
}


/*
 *  This is the main routine for controlling marshalling and choice of
 *  underlying Mach RPC.  The general rule is, if all the data,
 *  including the sgd itself, occupies less than MAX_BLOB_SIZE bytes,
 *  then all data is sent inline via the tnc_relocate_inline() RPC.
 *  If there are any large mbufs or if there is more than
 *  MAX_BLOB_SIZE bytes of data, then all data is sent as a series of
 *  out-of-line regions, one for the sgd, one for an amorphous blob of
 *  ``non-large-mbuf'' data, and one for each large mbuf buffer
 *  described by the lmv.
 *
#ifdef	NOTYET
 *  In the future we may try to pack lmv entries into the blob as long
 *  as the total data size remains less than MAX_BLOB_SIZE; for now, the
 *  presence of any large mbuf data at all causes the relocation to be
 *  done with out-of-line regions.
#endif
 */

int
tnc_relocate(
	mach_port_t	server_port,
	sgd_t		*sgdp,
	lmv_t		*lmvp,
	portv_t		*rrights,
	portv_t		*srights)
{
	mach_port_t	creds_port;
	uthread_t	uth;
	int		error;
	int		outofline = 0;
	portv_t		*sdummy = NULL, *rdummy = NULL;

	/*
	 *  Allow NULL port vector arguments.
	 */
	if ( rrights == NULL )
		if ( (rrights = rdummy = PV_ALLOC()) == NULL )
			panic("tnc_relocate: cannot alloc dummy rrights\n");
	if ( srights == NULL )
		if ( (srights = sdummy = PV_ALLOC()) == NULL )
			panic("tnc_relocate: cannot alloc dummy srights\n");

	/*
	 *  Get credentials port.
	 *
	 *  NB this is not necessarily the creds port of the process
	 *  that created the relocating object, and even if it is,
	 *  there is no guarantee that the process has not changed its
	 *  effective uid or gid in the time since the object was
	 *  created.  To ensure that the object is recreated on the
	 *  new node with the same ucred info as when it was
	 *  originally created, a ucred structure must be shipped in
	 *  the sgd and installed in the dummy proc structure on the
	 *  new node.  We only pass the creds port at all because we
	 *  rely on side effects of doing so; in particular, we want
	 *  fsvr_thread_initialize() to set up the "u.u_procp->p_rcred"
	 *  macro to point to the ucred in our dummy proc structure,
	 *  which we intend to immediately replace with the ucred from
	 *  the sgd.  Otherwise, things like falloc() will fail.
	 */
	uth = current_thread();
	creds_port = uth->uu_procp->p_cred;
	ASSERT(creds_port != MACH_PORT_NULL);

	/*
	 *  If no large mbufs and data fits in under MAX_BLOB_SIZE
	 *  bytes, go the inline route...
	 */
	if ( VECTOR_LEN(lmvp) == 0 &&
	     (VECTOR_GATHER_SIZE(sgdp) +
	      SGD_HEADER(sgdp)->sgd_inlsize) <= MAX_BLOB_SIZE ) {
		error = tnc_inl_reloc(server_port, creds_port,
				      sgdp, rrights, srights);
	} else {
		/*
		 *  Large mbufs present or message length > MAX_BLOB_SIZE.
		 *  Send everything to remote server as out-of-line regions.
		 */
		error = tnc_ool_reloc(server_port, creds_port,
				      sgdp, lmvp, rrights, srights);
		outofline++;
	}

	/*
	 *  Clean up dummy port vectors.
	 */
	if ( rdummy )
		PV_DEALLOC(rdummy);
	if ( sdummy )
		PV_DEALLOC(sdummy);

	if ( error != ESUCCESS ) {
		printf("tnc_relocate: %s relocation failed, errno %d\n",
		       ( outofline ? "out-of-line" : "inline" ), error);
	}
	return error;
}


/*
 *  This routine is called when IPC communication with the new node
 *  was successful, but a non-IPC error occurred remotely.  In such
 *  cases, the Rcv rts are returned to the original node.  The names
 *  of these port rights may have changed as a result of their round
 *  trip to the new node and back, so here we welcome them with open
 *  arms and call them by their old names again.  (No name change
 *  should occur for a port if at least one of its Snd rts remained
 *  on this node.)
 *
 *	"But the father said to his servants, 'Quick! ... Bring the
 *	 fattened calf and kill it.  Let's have a feast and celebrate.
 *	 For these Mach ports of mine ... were lost and are found.'"
 *					-- Luke 15:22-24, sort of...
 *
 *  NB there is no need to call this routine for send-rights, since
 *  their names don't correspond to client-side data structures.
 */
static void
kill_fattened_calf(
	portv_t		*pvp,
	mach_port_t	*prodigal_ports,
	int		prodigal_count)
{
	int i;
	mach_port_t	local_name;
	kern_return_t	kr;
#ifdef	UN_DEBUG
	int		ports_were_renamed = 0;
#endif

	ASSERT(VECTOR_LEN(pvp) == prodigal_count);

	PV_RESET(pvp);
	for ( i = 0; i < prodigal_count; i++ ) {
	    local_name = * PV_NEXT(pvp, 1);
	    if ( local_name != prodigal_ports[i] ) {
#ifdef	UN_DEBUG
		ports_were_renamed++;
		UNDEBUG(U_RPC, ("kill_fattened_calf: port 0x%x renamed 0x%x\n",
				prodigal_ports[i], local_name));
#endif
		kr = mach_port_rename(mach_task_self(),
				      prodigal_ports[i],
				      local_name);
		if ( kr != KERN_SUCCESS ) {
			panic("kill_fattened_calf: port rename failed "
					"kr=0x%x oldname=0x%x newname=0x%x",
					kr, prodigal_ports[i], local_name);
		}
	    }
	}

	UNDEBUG(U_RPC, ("kill_fattened_calf: %d ports renamed\n",
			ports_were_renamed));
}


int
tnc_inl_reloc(
	mach_port_t	server_port,
	mach_port_t	creds_port,
	sgd_t		*sgdp,
	portv_t		*rrights,
	portv_t		*srights)
{
	kern_return_t	kr;
	caddr_t 	start_of_raw_data;
	int		blob_size;
	int		sgd_size;
	int		status;
	caddr_t		blob;
	char		buffer[MAX_BLOB_SIZE + sizeof(long)];
	mach_port_t	ret_rcv_rts[RELOC_PORT_ARRAY_SIZE];
	int		ret_rcv_count = RELOC_PORT_ARRAY_SIZE;
	mach_port_t	ret_snd_rts[RELOC_PORT_ARRAY_SIZE];
	int		ret_snd_count = RELOC_PORT_ARRAY_SIZE;

	ASSERT(VECTOR_LEN(rrights) <= RELOC_PORT_ARRAY_SIZE &&
	       VECTOR_LEN(srights) <= RELOC_PORT_ARRAY_SIZE);

	/*
	 *  Allocate space for the blob (Big Lump Of Bytes).
	 *
	 *  XXX Note we are going to marshal into the blob and then
	 *  MIG is going to copy the blob to it's Request structure---
	 *  very inefficient.  The whole thing cries out for IDL-style
	 *  scatter/gather....
	 */

	blob = (caddr_t)roundup((int)buffer, sizeof(long));
	sgd_size = SGD_GATHER_SIZE(sgdp);
	blob_size = sgd_size + SGD_HEADER(sgdp)->sgd_inlsize;
	ASSERT(blob_size <= MAX_BLOB_SIZE);
	start_of_raw_data = blob + sgd_size;

	/*
	 *  Marshalling.  First the data, then the sgd itself.
	 */
	sgd_data_marshal(sgdp, start_of_raw_data);
	SGD_GATHER(sgdp, blob);

	ux_server_thread_blocking();
	kr = cli_tnc_relocate_inline(server_port,
				     creds_port,
				     (mach_port_t *)VECTOR_FIRST(rrights),
				     VECTOR_LEN(rrights),
				     (mach_port_t *)VECTOR_FIRST(srights),
				     VECTOR_LEN(srights),
				     blob,
				     blob_size,
				     &status,
				     ret_rcv_rts,
				     &ret_rcv_count,
				     ret_snd_rts,
				     &ret_snd_count);
	ux_server_thread_unblocking();
	if ( kr != KERN_SUCCESS ) {
		UNDEBUG(U_RPC, ("tnc_inl_reloc: RPC failed, kr 0x%x\n", kr));
		return EBADRPC;
	}

	if ( status != ESUCCESS ) {
		kill_fattened_calf(rrights, ret_rcv_rts, ret_rcv_count);
		kill_fattened_calf(srights, ret_snd_rts, ret_snd_count);
	}

	return status;
}


/*
 *  Called on new node to install arriving sgd's sent inline.
 *  We simply cast vectors from the marshalled data and call
 *  the appropriate arrival routine.
 */
kern_return_t
svr_tnc_relocate_inline(
	mach_port_t	server_port,
	mach_port_t	creds_port,
	mach_port_t	*rcv_rts,
	int		rcv_count,
	mach_port_t	*snd_rts,
	int		snd_count,
	caddr_t		blob,
	int		blob_size,
	int		*status,
	mach_port_t	*ret_rcv_rts,
	int		*ret_rcv_count,
	mach_port_t	*ret_snd_rts,
	int		*ret_snd_count)
{
	sgd_t		*sgdp;
	portv_t		*rrights, *srights;
	caddr_t		start_of_raw_data;
	kern_return_t	kr;

	sgdp = SGD_CAST(blob);
	start_of_raw_data = blob + SGD_GATHER_SIZE(sgdp);

	rrights = PV_CAST(rcv_rts, rcv_count);
	srights = PV_CAST(snd_rts, snd_count);

#ifdef	RELOC_DEBUG
	if ( (*status = force_reloc_error) == ESUCCESS )
#endif
	{
	kr = arrival_dispatch(server_port, creds_port, sgdp, start_of_raw_data,
			      NULL, rrights, srights, status);
	if ( kr != KERN_SUCCESS )
	    panic("svr_tnc_relocate_inline: dispatch failed, kr 0x%x\n", kr);
	}

	if ( *status != ESUCCESS ) {
		/*
		 *  Send the send- and receive-rts back to original node.
		 */
		bcopy(rcv_rts, ret_rcv_rts, rcv_count * sizeof(mach_port_t));
		*ret_rcv_count = rcv_count;
		bcopy(snd_rts, ret_snd_rts, snd_count * sizeof(mach_port_t));
		*ret_snd_count = snd_count;
	} else {
		*ret_rcv_count = *ret_snd_count = 0;
	}

	SGD_DEALLOC(sgdp);
	PV_DEALLOC(rrights);
	PV_DEALLOC(srights);

	return KERN_SUCCESS;
}


/*
 *  Static message type initializers for out-of-line relocation RPC
 */

static mach_msg_type_t rcvrts_type_template = {
	/* msgt_name = */		MACH_MSG_TYPE_MOVE_RECEIVE,
	/* msgt_size = */		32,
	/* msgt_number = */		RELOC_PORT_ARRAY_SIZE,
	/* msgt_inline = */		TRUE,
	/* msgt_longform = */		FALSE,
	/* msgt_deallocate = */		FALSE,
	/* msgt_unused = */		0
};

static mach_msg_type_t sndrts_type_template = {
	/* msgt_name = */		MACH_MSG_TYPE_MOVE_SEND,
	/* msgt_size = */		32,
	/* msgt_number = */		RELOC_PORT_ARRAY_SIZE,
	/* msgt_inline = */		TRUE,
	/* msgt_longform = */		FALSE,
	/* msgt_deallocate = */		FALSE,
	/* msgt_unused = */		0
};

static mach_msg_type_t count_type_template = {
	/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
	/* msgt_size = */		32,
	/* msgt_number = */		1,
	/* msgt_inline = */		TRUE,
	/* msgt_longform = */		FALSE,
	/* msgt_deallocate = */		FALSE,
	/* msgt_unused = */		0
};

static mach_msg_type_long_t region_type_template = {
{
	/* msgt_name = */		0,
	/* msgt_size = */		0,
	/* msgt_number = */		0,
	/* msgt_inline = */		FALSE,
	/* msgt_longform = */		TRUE,
	/* msgt_deallocate = */		FALSE,
	/* msgt_unused = */		0
},
	/* msgtl_name = */	MACH_MSG_TYPE_BYTE,
	/* msgtl_size = */	8,
	/* msgtl_number = */	0,
};

/*
 *  This routine relocates a socket graph using a varying number of
 *  Mach message out-of-line regions.
 *
 *  The first out-of-line region always contains the sgd itself.
 *  Sometimes it also contains the marshalled data that the sgd
 *  describes.  If the sgd is not a "simple" vector (i.e. its entries
 *  are contained in multiple, non-contiguous segments), then it must
 *  itself be marshalled.  In that case, since its data must also be
 *  marshalled, we allocate enough space to marshal both, and send
 *  both the sgd and its data in one out-of-line segment.
 *
 *  A simple sgd lies in a single contiguous area of memory, so in
 *  that case it is easier to use one out-of-line region for the sgd
 *  and marshal its data into a seperate buffer, to be sent as the
 *  second out-of-line region in the message.  The server routine can
 *  determine which of these situations apply by comparing the size of
 *  the first region with the "gather size" of the sgd (see
 *  svr_tnc_ool_reloc() ).
 */
int
tnc_ool_reloc(
	mach_port_t	server_port,
	mach_port_t	creds_port,
	sgd_t		*sgdp,
	lmv_t		*lmvp,
	portv_t		*rrights,
	portv_t		*srights)
{
	int	ret;
	int	simple;
	int	port_count;
	int	delta;
	caddr_t	blob;
	int	blob_size;
	kern_return_t		kr;
	mach_port_t		*portp;
	lmve_t			*lmve;
	struct reloc_request	*InP;
	struct reloc_reply	*OutP;
	struct outofline_region	*RegionP;
	union reloc_msg		message;
	mach_msg_size_t		msgh_size;
	int			msgh_simple;

	/*
	 *  Allocate space for the marshalled sgd data, and for the
	 *  sgd itself if it requires marshalling (i.e. if it is not
	 *  "simple").
	 */

	simple = ( VECTOR_IS_SIMPLE(sgdp) ? 1 : 0 );

	ASSERT(SGD_HEADER(sgdp)->sgd_outlcnt + simple < RELOC_MAXOUTOFLINE);
	ASSERT(SGD_HEADER(sgdp)->sgd_outlcnt == VECTOR_LEN(lmvp));

	blob_size = SGD_HEADER(sgdp)->sgd_inlsize;
	if ( ! simple )
		blob_size += SGD_GATHER_SIZE(sgdp);
			
	kr = vm_allocate(mach_task_self(), (vm_address_t *)&blob,
			 (vm_size_t)blob_size, 1 /* anywhere */);
	if ( kr != KERN_SUCCESS ) {
		UNDEBUG(U_RPC,
			("tnc_ool_reloc: cannot get %d byte blob, kr 0x%x\n",
			 blob_size, kr));
		return ENOMEM;
	}

	/*
	 *  Marshalling.  First the data, then the sgd itself.
	 */
	if ( simple ) {
		sgd_data_marshal(sgdp, blob);
	} else {
		sgd_data_marshal(sgdp, blob + SGD_GATHER_SIZE(sgdp));
		SGD_GATHER(sgdp, blob);
	}

	/*
	 *  Prepare to fill in the request message.
	 */
	InP = (struct reloc_request *)&message.req;
	OutP = (struct reloc_reply *)&message.rep;

	/*
	 *  First argument is the creds port.
	 */
	InP->creds_type = sndrts_type_template;
	InP->creds_type.msgt_name = MACH_MSG_TYPE_COPY_SEND;/*XXX move send?*/
	InP->creds_type.msgt_number = 1;
	InP->creds_port = creds_port;

	/*
	 *  For variable size arrays such as those we use to pass
	 *  port vectors,
	 *
	 *	type rright_array_t = array [*:RELOC_PORT_ARRAY_SIZE]
	 *					of mach_port_move_receive_t;
	 *	type sright_array_t = array [*:RELOC_PORT_ARRAY_SIZE]
	 *					of mach_port_move_send_t;
	 *
	 *  we do the same sneaky thing MIG does: if there are less than
	 *  RELOC_PORT_ARRAY_SIZE ports, we temporarily back up the InP
	 *  request pointer so that the next mach_msg_type_t immediately
	 *  follows the last port name we marshalled.
	 */

	port_count = VECTOR_LEN(rrights);
	ASSERT(port_count <= RELOC_PORT_ARRAY_SIZE);
	InP->rcvrts_type = rcvrts_type_template;
	InP->rcvrts_type.msgt_number = port_count;
	PV_GATHER(rrights, (caddr_t)&InP->rcvrts[0]);

	delta = (port_count - RELOC_PORT_ARRAY_SIZE) * sizeof(mach_port_t);
	InP = (struct reloc_request *) ((char *)InP + delta);
			/* This temporarily adjusts InP so that
			 * sndrts_type will immediately follow
			 * the last port in rcvrts[]
			 */

	/*
	 *  Likewise for the send-rights.
	 */
	port_count = VECTOR_LEN(srights);
	ASSERT(port_count <= RELOC_PORT_ARRAY_SIZE);
	InP->sndrts_type = sndrts_type_template;
	InP->sndrts_type.msgt_number = port_count;
	PV_GATHER(srights, (caddr_t)&InP->sndrts[0]);

	delta = (port_count - RELOC_PORT_ARRAY_SIZE) * sizeof(mach_port_t);
	InP = (struct reloc_request *) ((char *)InP + delta);
			/* This temporarily adjusts InP so that
			 * region_count_type will immediately follow
			 * the last port in sndrts[]
			 */

	/*
	 *  Now fill in the out-of-line regions and their count.
	 */
	InP->region_count_type = count_type_template;
	InP->region_count = simple + 1 /*blob*/ + VECTOR_LEN(lmvp);

	RegionP = &InP->regions[0];
	if ( simple ) {
		RegionP->oolr_type = region_type_template;
		RegionP->oolr_offset = (vm_offset_t) VECTOR_FIRST(sgdp);
		RegionP->oolr_type.msgtl_number =
			VECTOR_LEN(sgdp) * sizeof(sgde_t);
		RegionP++;
	}
	RegionP->oolr_type = region_type_template;
	RegionP->oolr_offset = (vm_offset_t) blob;
	RegionP->oolr_type.msgtl_number = blob_size;
	RegionP++;

	LMV_RESET(lmvp);
	while ( (lmve = LMV_NEXT(lmvp, 1)) != NULL ) {
		RegionP->oolr_type = region_type_template;
		RegionP->oolr_offset = (vm_offset_t) lmve->iov_base;
		RegionP->oolr_type.msgtl_number = lmve->iov_len;
		RegionP++;
	}

	/*
	 *  Send the reloc_request RPC.
	 */
	InP = (struct reloc_request *)&message.req;
	InP->hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX |
			     MACH_MSGH_BITS( MACH_MSG_TYPE_COPY_SEND,
					     MACH_MSG_TYPE_MAKE_SEND_ONCE );
	InP->hdr.msgh_remote_port = server_port;
	InP->hdr.msgh_local_port = mig_get_reply_port();
	InP->hdr.msgh_seqno = 0;
	InP->hdr.msgh_id = RELOC_OOL_REQ_MSG_ID;

	ux_server_thread_blocking();
	kr = mach_msg(&InP->hdr,
		      MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE,
		      (int) RegionP - (int) InP, /* msgh_size */	
		      sizeof(struct reloc_reply),
		      InP->hdr.msgh_local_port,
		      MACH_MSG_TIMEOUT_NONE,
		      MACH_PORT_NULL);
	ux_server_thread_unblocking();

	if ( kr != MACH_MSG_SUCCESS ) {
		if ( (kr == MACH_SEND_INVALID_REPLY) ||
		     (kr == MACH_RCV_INVALID_NAME) )
			mig_dealloc_reply_port();
		UNDEBUG(U_RPC, ("tnc_ool_reloc: mach_msg: kr 0x%x\n", kr));
		ret = EBADRPC;
		goto out;
 	}

	/*
	 *  Perform sanity checking on the reply.
	 */
	if ( OutP->hdr.msgh_id != RELOC_OOL_REQ_MSG_ID + 100 ) {
		if ( OutP->hdr.msgh_id == MACH_NOTIFY_SEND_ONCE ) {
			UNDEBUG((~0),("tnc_ool_reloc: server died\n"));
		} else {
			UNDEBUG((~0),("tnc_ool_reloc: reply id mismatch: %d\n",
				      OutP->hdr.msgh_id));
		}
		ret = EBADRPC;
		goto out;
	}

#if	defined(TypeCheck) || defined(UN_DEBUG)
	msgh_size = OutP->hdr.msgh_size;
	msgh_simple = !(OutP->hdr.msgh_bits & MACH_MSGH_BITS_COMPLEX);

	/*
	 *  IF reply size is the min reply size
	 *	THEN expect a simple reply indicating complete success
	 *  IF reply size is greater than the min reply size
	 *	THEN expect a complex error reply containing port rights
	 *  IF reply size is less than the min reply size
	 *	THEN expect a simple mig_reply_header_t with error retcode
	 */
	if (((msgh_size == MIN_RELOC_REPLY_SIZE) &&
	     (!msgh_simple ||
	      OutP->retcode != KERN_SUCCESS ||
	      OutP->status != ESUCCESS)) ||
	    ((msgh_size > MIN_RELOC_REPLY_SIZE) &&
	     (msgh_simple ||
	      OutP->retcode != KERN_SUCCESS ||
	      OutP->status == ESUCCESS)) ||
	    ((msgh_size < MIN_RELOC_REPLY_SIZE) &&
	     (!msgh_simple || msgh_size != sizeof(mig_reply_header_t)))) {
		UNDEBUG(U_RPC, ("tnc_ool_reloc: MIG_TYPE_ERROR\n"));
		ret = EBADRPC;
		goto out;
	}

	/* If we got a MIG reply header, it better contain an error. */
	ASSERT(!(msgh_size == sizeof(mig_reply_header_t)
		 && OutP->retcode == KERN_SUCCESS));

	/* XXX Check type descriptors... */
#endif	/* UN_DEBUG || TypeCheck */

	if (OutP->retcode != KERN_SUCCESS) {
		UNDEBUG(U_RPC, ("tnc_ool_reloc: RPC return code 0x%x\n",
				OutP->retcode));
		ret = EBADRPC;
		goto out;
	}

	/*
	 *  Return status.  If unsuccessful, the TNC server at the
	 *  new node should have returned our Rcv rts---give them
	 *  a hearty welcome.
	 */
	ret = OutP->status;
	if ( ret != ESUCCESS ) {
		ASSERT( ! msgh_simple );
		ASSERT( OutP->ret_rcvrts_type.msgt_number == port_count );
		kill_fattened_calf(rrights,
				   OutP->ret_rcvrts,
				   OutP->ret_rcvrts_type.msgt_number);
		kill_fattened_calf(srights,
				   OutP->ret_sndrts,
				   OutP->ret_sndrts_type.msgt_number);
	}

out:
	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t)blob, (vm_size_t)blob_size);
	return ret;
}


boolean_t
tnc_ool_server(
	mach_msg_header_t	*InHeadP,
	mach_msg_header_t	*OutHeadP)
{
	struct reloc_request	*req = (struct reloc_request *)InHeadP;
	struct reloc_reply	*rep = (struct reloc_reply *)OutHeadP;

	if ( InHeadP->msgh_id != RELOC_OOL_REQ_MSG_ID )
		return FALSE;

	/*
	 *  Set up standard reply.
	 */
	rep->hdr.msgh_bits =
		MACH_MSGH_BITS(MACH_MSGH_BITS_REMOTE(InHeadP->msgh_bits), 0);
	rep->hdr.msgh_remote_port = InHeadP->msgh_remote_port;
	rep->hdr.msgh_local_port = MACH_PORT_NULL;
	rep->hdr.msgh_id = InHeadP->msgh_id + 100;
	rep->int_type = count_type_template;
	rep->int_type.msgt_number = 2;
	rep->hdr.msgh_size = MIN_RELOC_REPLY_SIZE;

	rep->retcode = svr_tnc_ool_reloc((struct reloc_request *)InHeadP,
					 &rep->status );

	/*
	 *  Send back the send- and receive-rights if the server
	 *  routine failed.
	 */
	if ( rep->retcode == KERN_SUCCESS && rep->status != ESUCCESS ) {
		int i;

		/* Receive rights... */
		i = req->rcvrts_type.msgt_number;
		rep->ret_rcvrts_type = rcvrts_type_template;
		bcopy(&req->rcvrts[0],
		      &rep->ret_rcvrts[0],
		      i * sizeof(mach_port_t));
		rep->ret_rcvrts_type.msgt_number = i;
		rep->hdr.msgh_size += i * sizeof(mach_port_t);

		/* Send rights... */
		i = req->sndrts_type.msgt_number;
		rep->ret_sndrts_type = sndrts_type_template;
		bcopy(&req->sndrts[0],
		      &rep->ret_sndrts[0],
		      i * sizeof(mach_port_t));
		rep->ret_sndrts_type.msgt_number = i;
		rep->hdr.msgh_size += i * sizeof(mach_port_t);
	}

	return TRUE;
}


kern_return_t
svr_tnc_ool_reloc(
	struct reloc_request	*req,
	int			*status)
{
	sgd_t		*sgdp;
	lmv_t		*lmvp;
	lmve_t		*lmve, *sgd_region, *blob_region;
	portv_t		*rrights, *srights;
	caddr_t		start_of_raw_data;
	struct reloc_request *InP = req;
	kern_return_t	kr;
	int		delta, port_count;

	/*
	 *  Create receive-right port vector.
	 */
	port_count = req->rcvrts_type.msgt_number;
	rrights = PV_CAST(&req->rcvrts[0], port_count);

	delta = (port_count - RELOC_PORT_ARRAY_SIZE) * sizeof(mach_port_t);
	InP = (struct reloc_request *) ((char *)InP + delta);
			/* This temporarily adjusts InP so that
			 * sndrts_type will immediately follow
			 * the last port in rcvrts[]
			 */

	/*
	 *  Create send-right port vector.
	 */
	port_count = InP->sndrts_type.msgt_number;
	srights = PV_CAST(&InP->sndrts[0], port_count);

	delta = (port_count - RELOC_PORT_ARRAY_SIZE) * sizeof(mach_port_t);
	InP = (struct reloc_request *) ((char *)InP + delta);
			/* This temporarily adjusts InP so that
			 * region_count_type will immediately follow
			 * the last port in sndrts[]
			 */

	/*
	 *  Here we take advantage of defining an `lmve_t' to look
	 *  just like a `struct outofline_region'.  We now use our
	 *  LMV_xxx() vector macros to examine the out-of-line data.
	 */
	lmvp = LMV_CAST(&InP->regions[0], InP->region_count);
	lmve = LMV_NEXT(lmvp, 1);
	sgd_region = lmve;
	sgdp = SGD_CAST( lmve->iov_base );
	if ( lmve->iov_len > SGD_GATHER_SIZE(sgdp) ) {
		start_of_raw_data =
			lmve->iov_base + SGD_GATHER_SIZE(sgdp);
		blob_region = NULL;
	} else {
		lmve = LMV_NEXT(lmvp, 1);
		start_of_raw_data = lmve->iov_base;
		blob_region = lmve;
	}
	lmve = LMV_NEXT(lmvp, 1);
	LMV_DEALLOC(lmvp);
	lmvp = LMV_CAST(lmve, InP->region_count - ( blob_region ? 2 : 1 ) );

#ifdef	RELOC_DEBUG
	if ( (*status = force_reloc_error) == ESUCCESS )
#endif
	{
	kr = arrival_dispatch(req->hdr.msgh_local_port, req->creds_port, sgdp, 
				start_of_raw_data, lmvp, rrights, srights, 
				status);
	if ( kr != KERN_SUCCESS )
	    panic("svr_tnc_relocate_inline: dispatch failed, kr 0x%x\n", kr);
	}

	/*
	 *  Deallocate out-of-line regions.  If the arrival function
	 *  succeeded, then there is no need to deallocate regions
	 *  corresponding to large mbuf buffers---these regions will
	 *  be deallocated by tnc_lg_mbuf_deallc() via m_free().
	 */
	(void)vm_deallocate(mach_task_self(),
			    (vm_address_t) sgd_region->iov_base,
			    (vm_size_t)    sgd_region->iov_len);
	if ( blob_region )
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) blob_region->iov_base,
				     (vm_size_t)    blob_region->iov_len);
	if ( *status != ESUCCESS ) {
		/*
		 *  Server side cleanup after error.
		 *  Free VM regions received in the message.
		 *  File ports are returned to the client by
		 *  tnc_ool_server() above.
		 */
		LMV_RESET(lmvp);
		while ( (lmve = LMV_NEXT(lmvp, 1)) != NULL ) {
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) lmve->iov_base,
					     (vm_size_t)    lmve->iov_len);
		}
	}

	SGD_DEALLOC(sgdp);
	LMV_DEALLOC(lmvp);
	PV_DEALLOC(rrights);
	PV_DEALLOC(srights);

	return KERN_SUCCESS;
}


/*
 *  Common mbuf buffer deallocation routine for large mbufs whose
 *  data buffer is contained in vm_allocate()'ed memory from a
 *  relocation IPC.
 */
/*ARGSUSED*/
int
tnc_lg_mbuf_dealloc(
	caddr_t	buffer,
	int	len,
	caddr_t	arg)
{
	kern_return_t kr;

	kr = vm_deallocate(mach_task_self(),
			   (vm_address_t)buffer, (vm_size_t)len);
	if ( kr != KERN_SUCCESS ) {
		UNDEBUG(U_RPC,
			("tnc_lg_mbuf_dealloc: vm_deallocate: kr 0x%x\n", kr));
		return (EINVAL);
	}
	return (ESUCCESS);
}

