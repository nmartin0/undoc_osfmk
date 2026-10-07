/*
 * 
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

/*
 * Convert a kmsg to/from network representation:
 *
 *	Local IPC format: mach_port_t == ipc_port_t
 *
 *	Network format: mach_port_t == dipc_uid_t
 *
 * $Id: kmsg_convert.c,v 1.4 1995/02/24 17:56:00 andyp Exp $
 *
 * HISTORY:
 */

#include "cpus.h"
#include "norma_ipc.h"
#include "mach_kdb.h"
#include "mach_assert.h"

#include <mach/mach_types.h>
#include <mach/boolean.h>
#include <mach/vm_param.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>

#include <kern/queue.h>

#include <ipc/ipc_kmsg.h>

#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>

#include <norma2/meta_kmsg.h>
#include <norma2/kmsg_parser.h>
#include <norma2/dipc_uid.h>
#include <norma2/dipc_migrate.h>
#include <norma2/dipc_special.h>
#include <norma2/dipc_port.h>
#include <norma2/norma_log.h>
#include <norma2/kmsg_convert.h>

/*
 * Statistics
 */
dipc_nkmsg_convert_stats_t	dipc_nkmsg_convert_stats;


/*
 * Kmsg conversion, parser functions
 */
kern_return_t	cvt_net_inline_ports(
				mach_msg_type_long_t *,
				vm_size_t,
				dipc_uid_t *,
				ipc_kmsg_t),
		cvt_net_ool_ports(
				mach_msg_type_long_t *,
				vm_size_t,
				dipc_uid_t **,
				ipc_kmsg_t);

kmsg_parse_tbl_t
cvt_network_2_local_ptable = {
	cvt_net_inline_ports,
	cvt_net_ool_ports,
	0,
	0, };

/*
 * convert some number of uid's into local IPC port pointers.
 *
 * inputs:
 *	type	Mach message type descriptor ptr.
 *	uidp	pointer to a vector of uid's
 *	count	# of uid to convert.
 *
 * outputs:
 *	none.
 *
 * side effects:
 *	uidp (uid's) are replaced with IPC port pointers (ipc_port_t's).
 */

void
cvt_uid_2_port(
	mach_msg_type_long_t	*type,
	dipc_uid_t		*uidp,
	int			count )
{
	dipc_uid_t		uid;
	mach_msg_type_name_t	type_name;

	kcnvrt_entry3(cvt_uid_2_port, type, uidp, count);

	for(; count > 0; --count, uidp++ ) {

		uid = *uidp;

		CONVERT_NKMSG_STATS(uids++);

		/*
		 *	Deal with different possible types of UID's converting
		 *	them to the proper local port representation.
		 */
		switch (*uidp) {
		case DIPC_UID_NULL:
			*uidp = (dipc_uid_t)IP_NULL;
			CONVERT_NKMSG_STATS(uid_null++);
			kcnvrt_log3(1,"%s: NULL uid @ %x\n",__FUNC__,uidp);
			break;

		case DIPC_UID_DEAD:
			*uidp = (dipc_uid_t)IP_DEAD;
			CONVERT_NKMSG_STATS(uid_dead++);
			kcnvrt_log3(1,"%s: DEAD uid @ %x\n",__FUNC__,uidp);
			break;

		default:
			if ( type->msgtl_header.msgt_longform )
				type_name = type->msgtl_name;
			else
				type_name = type->msgtl_header.msgt_name;

			kcnvrt_log4(3, "%s: uid %x name %d\n",
					__FUNC__, uid, type_name);
			/*
			 *	Replace the uid with a local IPC (ipc_port_t)
			 *	pointer, which could be IP_NULL.
			 */
			*uidp = (dipc_uid_t)dipc_receive_port(*uidp, type_name);
		}
	}
}

/*
 * convert network format inline port(s) to local IPC format (ipc_port_t)
 *
 * inputs:
 *
 * outputs:
 */

kern_return_t
cvt_net_inline_ports(
	mach_msg_type_long_t	*type,
	vm_size_t		bytes,
	dipc_uid_t		*uidp,
	ipc_kmsg_t		kmsg )
{
	kern_return_t		kr = KERN_SUCCESS;

	kcnvrt_entry4(cvt_net_inline_ports,type,bytes,uidp,kmsg);

	CONVERT_NKMSG_STATS(inline_ports++);

	if ( (long)bytes <= 0 || (uidp == (dipc_uid_t *)0) ) {
		return kr;
	}

	cvt_uid_2_port( type, uidp, (bytes / sizeof(dipc_uid_t)) );

	return	kr;
}

/*
 * convert network format OOL port(s) (dipc_uid_t) to local IPC format
 * (ipc_port_t).
 *
 * inputs:
 *
 * outputs:
 */

kern_return_t
cvt_net_ool_ports(
	mach_msg_type_long_t	*type,
	vm_size_t		bytes,
	dipc_uid_t		**ool_uids,
	ipc_kmsg_t		kmsg )
{
	kern_return_t		kr = KERN_SUCCESS;

	kcnvrt_entry4(cvt_net_ool_ports,type,bytes,ool_uids,kmsg);

	CONVERT_NKMSG_STATS(ool_ports++);

	if ( (long)bytes <= 0 || (*ool_uids == (dipc_uid_t *)0) )
		return	kr;

	cvt_uid_2_port( type, *ool_uids, (bytes / sizeof(dipc_uid_t)) );

	return	kr;
}

/*
 * convert network format kmsg to local IPC format kmsg. Called when receiving
 * a kmsg message.
 *
 * implicit inputs:
 *	user thread context, current_space() etc.
 *
 * inputs:
 *	kmsg	network format kmsg (mach_port_t's == dipc_uid_t's).
 *
 * outputs:
 *	kern_return_t's
 *
 * side effects:
 *	kmsg modified to local IPC format (mach_port_t's == ipc_port_t's).
 */

kern_return_t
convert_net_kmsg_2_kmsg( ipc_kmsg_t kmsg )
{
	kern_return_t		kr, dest_kr;
	dipc_uid_t		uid;
	mach_msg_header_t       *msgh;
	boolean_t		dest_dead = FALSE;

	kcnvrt_entry1(convert_net_kmsg_2_kmsg,kmsg);

	CONVERT_NKMSG_STATS(nkmsg++);

	kr = KERN_SUCCESS;
	msgh = (mach_msg_header_t *) &kmsg->ikm_header;

	/* fetch destination uid */
	uid = (dipc_uid_t) kmsg->ikm_header.msgh_remote_port;

	kcnvrt_log4(3,"%s: kmsg %x uid %x\n", __FUNC__, kmsg, uid);

	/*
	 * Receive/process the destination port. 
	 * On successful completion of dipc_receive_dest() the kmsg
	 * destination port 'msgh_remote_port' will be replaced with a valid
	 * local IPC port pointer (ipc_port_t).
	 */

	if ( (dest_kr = dipc_receive_dest( kmsg )) != KERN_SUCCESS ) {
		/*
		 *	Destination port has died; that's okay, but the
		 *	rest of the kmsg must be converted to local form
		 *	otherwise we might have a half-baked kmsg (that
		 *	might still have uid's rather than ipc_port_t's
		 *	in the body).
		 */
		CONVERT_NKMSG_STATS(dest_dead++);
		kcnvrt_log5(0, "%s: dest dead: kmsg %x uid %x dest_kr 0x%x\n",
			__FUNC__, kmsg, uid, dest_kr);
		dest_dead = TRUE;
	}

	kcnvrt_log5(3, "%s: dest uid %x maps to port 0x%x, hbits 0x%x\n",
		__FUNC__, uid, kmsg->ikm_header.msgh_remote_port,
		kmsg->ikm_header.msgh_bits);

	/*
	 * Receive/process the optional reply port. 
	 */
	if ( msgh->msgh_local_port ) {
		register mach_msg_bits_t	mbits = msgh->msgh_bits;
		register dipc_uid_t		reply_uid;

		assert(MACH_MSGH_BITS_LOCAL(mbits) != MACH_MSG_TYPE_PORT_RECEIVE);
		reply_uid = msgh->msgh_local_port;

		msgh->msgh_local_port = (mach_port_t)
					dipc_receive_port(
						reply_uid,
						MACH_MSGH_BITS_LOCAL(mbits));

		CONVERT_NKMSG_STATS(reply++);

		kcnvrt_log4(3, "%s: recv reply port %x uid %x\n", __FUNC__,
				msgh->msgh_local_port,
				((ipc_port_t)msgh->msgh_local_port)->dipc_uid);
	}

	/*
	 * process the message body if it's tagged complex (body contains ports
	 * and/or OOL data).
	 */
	if ( kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX ) {
		int		count;
		vm_size_t	body_size;

		CONVERT_NKMSG_STATS(complex++);

		kcnvrt_log4(3,"%s: parse complex kmsg %x uid %x.\n",
				__FUNC__, kmsg, uid);

		count = -1;
		body_size = kmsg->ikm_header.msgh_size
				- sizeof(mach_msg_header_t);

		(void)norma_parse_kmsg( &count, (mach_msg_type_long_t*)(kmsg+1),
					&body_size,
					&cvt_network_2_local_ptable,
					&kr,
					(void *) kmsg);
	}

	/* kmsg is now of local flavor */
	kmsg->ikm_kmsg_type = IKM_KMSG_TYPE_LOCAL;

	if ( dest_dead )
		kr = dest_kr;

	return	kr;
}
