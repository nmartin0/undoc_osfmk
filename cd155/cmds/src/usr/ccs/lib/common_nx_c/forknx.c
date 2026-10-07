/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *
 *      Copyright 1993  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/common_nx_c/forknx.c,v 1.9 1994/12/07 17:18:31 wds Exp $
 */

#include <nx.h>
#ifdef NX
#include <dbglib.h>
#include <sys/table.h>
#include <mach.h>
#endif /* NX */

extern long	mcmsg_ptype;
#ifdef NX
extern ipd_msg_info_t ipd_msg_info;
extern ipd_msg_info_t *ipd_msg_info_p;
extern vm_address_t	_nxport_thread_stack;
extern vm_address_t	_nxvmport_thread_stack;
extern long	_nxport_thread_stack_size;
#endif /* NX */

void
_nx_invalidate_ptype()
{
	mcmsg_ptype = INVALID_PTYPE;

#ifdef NX
	/* The parent is an NX process.  Invalidate the IPD NX information
	 * table() entry for the newly forked process.  If the process
	 * calls setptype() the entry will be filled in.
	 */

	ipd_msg_info_p = (ipd_msg_info_t *) 0;
	table(TBL_IPD_MSG_INFO, getpid(), &ipd_msg_info_p,
			-1, sizeof(ipd_msg_info_p));


#endif /* NX */
}

void
forknx()
{
        /*
         * Invalidate the ipd_msg_info pointer where IPD obtains the
         * process's ptype.
         */
        _nx_invalidate_ptype();

#ifdef NX
	/* If the NX thread stacks are allocated the space must be freed.
	 * If the process calls setptype(), new NX threads will be
	 * created along with their stacks.
	 */
	if (_nxport_thread_stack_size != -1) {
		vm_deallocate(mach_task_self(), _nxport_thread_stack,
			_nxport_thread_stack_size);
		vm_deallocate(mach_task_self(), _nxvmport_thread_stack,
			_nxport_thread_stack_size);
		_nxport_thread_stack_size = -1;
	}
#endif /* NX */
}
