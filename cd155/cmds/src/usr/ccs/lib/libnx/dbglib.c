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
 
#define DEBUG_IPD_MSG 0
/****************************************************************************
 **                                                                        **
 **                (C) Copyright 1990 Intel Corporation                    **
 **                        All rights reserved                             **
 **                                                                        **
 **              INTEL CORPORATION PROPRIETARY INORMATION                 **
 **                                                                        **
 **  This software is supplied under the terms of a license agreement or   **
 **  nondisclosure agreement with Intel Corporation and may not be copied  **  
 **  or disclosed except in accordance with the terms of that agreement.   **
 **                                                                        **
 **                                                                        **
 ****************************************************************************/

/***************************************************************************
 *    
 *    Title:
 *        $Id: dbglib.c,v 1.19 1995/01/17 01:12:32 tnt Exp $
 *    
 *    Description:
 *        System calls that support monitoring of NX applications.
 *
 *    Revision History:
 *        $Log: dbglib.c,v $
 * Revision 1.19  1995/01/17  01:12:32  tnt
 * PTS #: 12126
 * Mandatory?:  No (M*).
 * Description: This module contains functions used by IPD.  In the dbglib.o
 *              module the debugging printfs were left enabled.  The debugging
 *              code has been disabled so that IPD is free to print error
 *              messages based on the returned error codes.
 * Reviewer(s): Chuck, Terry
 * Risk:        Low
 * Testing:     Message Passing, Control-c, and Misc. EATs, IPD tests.
 * Module(s):   usr/ccs/lib/libnx/dbglib.c
 *
 * Revision 1.18  1994/11/19  02:29:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/08/31  20:22:37  bradf
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.15.2.1  1994/08/10  16:38:55  tnt
 *  PTS #: 10446
 *  Mandatory?:  Yes
 *  Description: IPD EATs were failing due to segmentation violations by TAM.
 *               The problem was incorrect read lengths passed into vm_read().
 *               Problem was fixed in nx_ptypes_by_pid() and other IPD support
 *               functions.
 *  Reviewer(s): Karla Callaghan
 *  Risk:        Medium
 *  Module(s):   usr/ccs/lib/libnx/dbglib.c
 *  Testing:     Message passing EATs.  Karla ran IPD EATs.  PTS 9901 test case.
 *               Misc EATs. Control-C EATs.
 *
 * Corrected 'size' argument on vm_read() calls.
 *
 * Revision 1.15  1993/12/17  20:14:50  tnt
 *  Reviewer: Terry Prickett, Mike Andrews
 *  Risk: Low
 *  Benefit or PTS #: 7506; Mask off global send bit of ptype in message list
 *                    structures returned to ipd.
 *  Testing: Message Passing EATs, TAMU
 *  Modules: dbglib.c
 *
 * Merging R1.2 revision 1.14.4.1 into the main-line.
 *
 * Revision 1.14  1993/07/06  21:51:51  prp
 * New message queue algorithm.
 *
 * Revision 1.13  1993/05/12  23:14:39  andrews
 * Masked off global send bit in current_ptype (nx_ptypes_by_pid).
 *
 * Revision 1.12  1993/02/18  01:12:41  andrews
 * Fixed bug in nx_msgq_get(), where the pointer to msg_data was set incorrectly.
 *
 * Revision 1.11  1993/01/28  20:39:14  shala
 * Changed PORT_NULL to MACH_PORT_NULL .
 *
 * Revision 1.10  1993/01/20  01:03:09  andrews
 * Fixed bug #3983 (errno now set on error conditions).  Also added pid
 * validation with error setting.
 *
 * Revision 1.9  1993/01/15  18:13:47  andrews
 * Added sanity check for last_entry count in nx_ptypes_by_pid().
 *
 * Revision 1.8  1992/12/29  17:07:16  andrews
 * Fixed nx_ptypes_by_pid() when called for control process.
 *
 * Revision 1.7  1992/12/22  01:16:02  andrews
 * Added nx_traced_delete_mem() call.
 *
 * Revision 1.6  1992/12/15  18:36:39  andrews
 * Fixed bug 3813.
 *
 * Revision 1.5  1992/11/04  17:45:39  andrews
 * Fixed bug in nx_ptypes_by_pid(), where len_ptypes value was off by 1.
 *
 * Revision 1.4  1992/10/16  20:32:18  andrews
 * Fixed include file problem.
 *
 * Revision 1.3  1992/10/15  23:01:49  andrews
 * Fixed include file pathname problem.
 *
 * Revision 1.2  1992/10/15  22:48:02  andrews
 * Removed nx_ptypes_by_pid_old
 *
 *    
 **************************************************************************/

#include <errno.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/syslimits.h>
#include <sys/table.h>
#include <stdio.h>
#include <dbglib.h>

/***************************************************************************
 *    
 *    Name:
 *        nx_attach_pgrp()
 *    
 *    Description:
 *        Associate the indicated process group to the caller for tracing.
 *        The processes in the process group must be traced. The caller is
 *        sent a SIGCHLD whenever a new (traced) process is created within the
 *        NX application, and any further trace events will cause the caller
 *        to be signaled as well.
 *    
 *    Parameters:
 *        pgrp:             - Process group ID to be associated to the caller.
 *    
 *    Returns:
 *        0 on success, -1 on error (on error, errno is set to an appropriate 
 *        value).
 *    
 **************************************************************************/

int 
nx_attach_pgrp( pid_t pgrp ) 
{ 
    return( syscall( 303, pgrp )); 
}

/***************************************************************************
 *    
 *    Name:
 *        nx_detach_pgrp()
 *    
 *    Description:
 *      Disassociate the indicated process group from the calling TAM.
 *    
 *    Parameters:
 *        pgrp:             - Process group ID to be disassociated
 *    
 *    Returns:
 *        0 on success, -1 on error (on error, errno is set to an appropriate 
 *        value).
 *    
 **************************************************************************/

int 
nx_detach_pgrp( pid_t pgrp ) 
{ 
    return( syscall( 304, pgrp )); 
}

/***************************************************************************
 *    
 *    Name:
 *        nx_join()
 *    
 *    Description:
 *        Make the calling process part of the NX application indicated by
 *        the specified process group ID. The calling process MUST already
 *        reside on a node that belongs to the partition where the NX 
 *        application is running. Typically this means that you can only join 
 *        on the node where the NX controlling process is running.
 *    
 *    Parameters:
 *        pgrp:             - Process group ID of NX application to join.
 *    
 *    Returns:
 *        0 on success, -1 on error (on error, errno is set to an appropriate 
 *        value).
 *    
 **************************************************************************/

int 
nx_join( pid_t pgrp ) 
{   
    return( syscall( 305, pgrp )); 
}


/***************************************************************************
 *    
 *    Name:
 *        nx_wait()
 *    
 *    Description:
 *      Return the pid of a process in an NX application which is a
 *      member of the designated process group and who is stopped.  
 *      If there are no processes which qualify then the call will block,
 *      waiting for an event. This call should only be called
 *      when a TAM receives a SIGCHLD.
 *
 *      The WNOWAIT option causes wait to NOT clean up a ZOMBIE
 *      process so that it's exit status is still available to
 *      the waited on by the controlling process.  The sibling
 *      TAMs must use WNOWAIT otherwise the TAM will clean up
 *      the exiting process and the controlling process will get
 *      an MIG error from wait. 
 *
 *      The "mother" TAM should NOT use WNOWAIT because it has
 *      the tradiitional releationship between it and the process under
 *      debug.
 *
 *      The value for WNOWAIT is:
 *      #define WNOWAIT         0x4     Get status but leave the process such
 *                                      that wait can be called again and
 *                                      receive the same status.
 *
 *      There is no plan to export this to the users.
 *
 *      When the TAM detects the process under debug has exited, it must
 *      send a SIGCHLD to the controling process.  If it did not, nx_waitall()
 *      will never return.  OSF/1 AD does not signal both the TAM and the
 *      controling process because it cannot guarantee which process runs
 *      first and the TAM needs to be able to collect the status of the 
 *      process.
 *
 *    
 *    Parameters:
 *              pid_t pgrp;          The process group interested in.
 *              int   *status        Status of the process under debug.
 *              int    option        Either WNOHANG, WUNTRACED or WNOWAIT.
 *              pid_t pid;           -1 = error, otherwise pid of 
 *                                   a process under debug which caused
 *                                   a SIGCHLD.
 *
 *    
 *    Returns:
 *        0 on success, -1 on error (on error, errno is set to an appropriate 
 *        value).
 *    
 **************************************************************************/

int 
nx_wait( pid_t pgrp, int *status, int option ) 
{ 
    return( syscall( 307, pgrp, status, option )); 
}


/***************************************************************************
 *    
 *    Name:
 *        nx_ptrace()
 *    
 *    Description:
 *      The function of nx_ptrace() mirrors that of ptrace() EXCEPT
 *      that it allows the tam to trace any process that is in the
 *      process group passed in when the tam called nx_attach_pgrp().
 *      This function can be called by TAM only and will return an
 *      EQTAM error if the process making the call had not previously
 *      called nx_attach_pgrp().
 *    
 *    Parameters:
 *              int    req;          ptrace request
 *              pid_t  pid;          pid to trace.
 *              int   *addr;         Address
 *              int    data;         Data 
 *    
 *    Returns:
 *        0 on success, -1 on error (on error, errno is set to an appropriate 
 *        value).
 *    
 **************************************************************************/

int 
nx_ptrace( int req, pid_t pid, int *addr, int data ) 
{ 
    return( syscall( 308, req, pid, addr, data )); 
}


/***************************************************************************
 *
 *    Name:
 *      nx_traced_add_mem()
 *
 *    Description:
 *      This function allocates memory for the process that is being traced.
 *
 *    Parameters:
 *              pid_t    pid;          pid of process to add memory to.
 *              int    * addr;         in/out address of where memory is to be
 *                                     allocated.
 *              size_t * len;          in/out length of memory to allocate.
 *
 *    Returns:
 *        0 on success, -1 on error (on error, errno is set to an appropriate
 *        value).
 *
 **************************************************************************/

int
nx_traced_add_mem(pid_t Pid, int * naddr, size_t * len)
{
    /* XXX There is a bug: the length is "in" only at pressent. */
    return syscall(309, Pid, naddr, *len);
}


/***************************************************************************
 *
 *    Name:
 *      nx_traced_delete()
 *
 *    Description:
 *      This function deletes memory for the process that is being traced.
 *
 *    Parameters:
 *              pid_t    pid;          pid of process to add memory to.
 *              int      addr;         in/out address of where memory is to be
 *                                     allocated.
 *              size_t   len;          in/out length of memory to allocate.
 *
 *    Returns:
 *        0 on success, -1 on error (on error, errno is set to an appropriate
 *        value).
 *
 **************************************************************************/

int
nx_traced_delete_mem(pid_t Pid, int naddr, size_t len)
{
    /* XXX There is a bug: the length is "in" only at pressent. */
    return syscall(310, Pid, naddr, len);
}


/***************************************************************************
 * 
 *  nx_msgq_free - free (deallocate) a linked list of received message data
 *  previously created by nx_msgq_get().
 *
 *  This routine is intended to be called by the TAM after it is done using
 *  the data in the associated message queue list.  All elements in the
 *  list are deallocated.
 *
 *  Parameters:
 *
 *  msgq_head   = a pointer to the head of the message data list, obtained
 *                from a previous nx_msgq_get() call.
 *
 *  Returns:
 *
 *  no return value
 *
 *
 *  Note:
 *
 *  A call with msgq_head == NULL is permitted, and no error condition results.
 *
 **************************************************************************/

#ifdef __STDC__
void nx_msgq_free(msgq_data_t *msgq_head)
#else
void nx_msgq_free(msgq_head)
msgq_data_t *msgq_head;
#endif
{
msgq_data_t *next;

	while (msgq_head != NULL) {
		next = msgq_head->next;
		free(msgq_head);
		msgq_head = next;
	}
}


/***************************************************************************
 * 
 *  nx_msgq_get - create a linked list of queued message data elements.
 *
 *  This routine creates a static copy of the queued message data for
 *  a particular process.  The format of the message data is the
 *  msgq_data_t structure declared in ipd_msg.h.
 *
 *  This routine dynamically allocates memory to create the list.  When
 *  the list is no longer needed, nx_msgq_free() should be called to
 *  deallocate this memory.
 *
 *
 *  Parameters:
 *
 *  unix_pid    = process ID denoting the process whose message queue
 *                is to be examined.
 *
 *  msgq_head   = the address of (pointer to) a pointer to the head of the
 *                message data list.  The pointer to the head of the list
 *                is filled in by this routine.
 *
 *  num_msgs    = a pointer to a variable containing the number of elements
 *                in the message queue list.  The value of this variable
 *                is filled in by this routine.
 *
 *  Returns:
 *
 *  0 if successful
 * -1 on any failure
 *
 *
 *  Note:
 *
 *    
 **************************************************************************/

#ifdef __STDC__
int nx_msgq_get(pid_t unix_pid, msgq_data_t **msgq_head, long *num_msgs)
#else
int nx_msgq_get(unix_pid, msgq_head, num_msgs)
pid_t	unix_pid;
msgq_data_t **msgq_head;
long	*num_msgs;
#endif
{
ipd_msg_info_t	*ipd_msg_info_p;
ipd_msg_info_t	*ipd_msg_info;
long	buffer_left;
xmsg_t	*xmsg_data;
xmsg_t	*msg;
msgq_data_t **last_link;
long	*app_id_p;
long	*app_id;
long	status;
int	task;
vm_size_t ipd_msg_info_size, xmsg_data_size, app_id_size;
kern_return_t k_status;

	if (msgq_head == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), NULL msgq_head\n",
		       unix_pid);
#endif
	
		errno = EINVAL;	
		return -1;
	}

	if (num_msgs == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), NULL num_msgs\n",
		       unix_pid);
#endif
		
		errno = EINVAL;	
		return -1;
	}

	task = task_by_pid(unix_pid);
	if (task == MACH_PORT_NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), cannot get task token for pid\n",
		       unix_pid);
#endif

		errno = ESRCH;	
		return -1;
	}

	*num_msgs = 0;
	*msgq_head = NULL;
	last_link = msgq_head;
	status = table(TBL_IPD_MSG_INFO, (int)unix_pid, &ipd_msg_info_p,
	               1, sizeof(ipd_msg_info_p));
	if (status != 1) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), table() status = %d\n",
		       unix_pid,status);
#endif

		errno = ESRCH;	
		return -1;
	}

	if (ipd_msg_info_p == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), NULL ipd_msg_info_p\n",
		       unix_pid);
#endif

		errno = EINTERNAL;	
		return -1;
	}

	if ((k_status = vm_read(task, ipd_msg_info_p, sizeof(ipd_msg_info_t),
	                        &ipd_msg_info, &ipd_msg_info_size)) !=
		 KERN_SUCCESS) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), cannot read ipd_msg_info, status = %d\n",
		       unix_pid, k_status);
#endif

		errno = EINTERNAL;	
		return -1;
	}

	app_id_p = ipd_msg_info->app_id;
	if (app_id_p == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), NULL ipd_msg_info.app_id\n",
		       unix_pid);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		errno = EINTERNAL;	
		return -1;
	}

	if ((k_status = vm_read(task, app_id_p, sizeof(long),
	                        &app_id, &app_id_size)) != KERN_SUCCESS) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), cannot read app_id, status = %d\n",
		       unix_pid, k_status);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		errno = EINTERNAL;	
		return -1;
	}

	msg = ipd_msg_info->buffer_p;
	buffer_left = ipd_msg_info->buffer_size;
	if (msg == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_msgq_get(pid=%d), NULL ipd_msg_info.buffer_p\n",
		       unix_pid);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		vm_deallocate(mach_task_self(), app_id, app_id_size);
		errno = EINTERNAL;	
		return -1;
	}

	while (buffer_left > 0) {
		if ((k_status = vm_read(task, msg, sizeof(xmsg_t),
		                        &xmsg_data, &xmsg_data_size)) !=
			KERN_SUCCESS) {

#if DEBUG_IPD_MSG
			printf("ERROR: nx_msgq_get(pid=%d), cannot read xmsg_data, status = %d\n",
			       unix_pid, k_status);
#endif

			vm_deallocate(mach_task_self(), ipd_msg_info,
			              ipd_msg_info_size);
			vm_deallocate(mach_task_self(), app_id, app_id_size);
			errno = EINTERNAL;	
			return -1;
		}

		if (xmsg_data->state == XMSG_FULL) {
			*last_link = (msgq_data_t *)malloc(sizeof(msgq_data_t));
			if (*last_link == NULL) {

#if DEBUG_IPD_MSG
				printf("ERROR: nx_msgq_get(pid=%d), insufficient memory\n",
				       unix_pid);
#endif

				vm_deallocate(mach_task_self(), ipd_msg_info,
				              ipd_msg_info_size);
				vm_deallocate(mach_task_self(), app_id,
				              app_id_size);
				vm_deallocate(mach_task_self(), xmsg_data,
				              xmsg_data_size);
				nx_msgq_free(*msgq_head);
				errno = ENOMEM;	
				return -1;
			}
			(*last_link)->src_node = xmsg_data->source_node;
			(*last_link)->src_ptype = xmsg_data->source_ptype & ~GLOBAL_BIT;
			/* Note: the source application ID cannot be
			   determined at this point; for non-cross
			   application messages, the source and
			   destination application IDs will differ. */
			(*last_link)->src_app = 0;
			/* Note: the following is not always correct;
			   the destination node may be either the
			   application's mynode() or -1 (for message
			   broadcast, and there is no way to distinguish
			   between the two. */
			(*last_link)->dest_node = ipd_msg_info->nodenum;
			(*last_link)->dest_ptype = xmsg_data->dest_ptype;
			(*last_link)->dest_app = *app_id;
			(*last_link)->msg_type = xmsg_data->msg_type;
			(*last_link)->msg_length = xmsg_data->length;
			(*last_link)->msg_data = (char *)(msg+1);
			(*last_link)->next = NULL;
			last_link = &((*last_link)->next);
			(*num_msgs)++;
		}
		msg += (xmsg_data->size/sizeof(xmsg_t)) + 1;
		buffer_left -= xmsg_data->size + sizeof(xmsg_t);
		vm_deallocate(mach_task_self(), xmsg_data, xmsg_data_size);
	}
	vm_deallocate(mach_task_self(), ipd_msg_info, ipd_msg_info_size);
	vm_deallocate(mach_task_self(), app_id, app_id_size);
	return 0;
}


/***************************************************************************
 * 
 *  nx_recvq_free - free (deallocate) a linked list of message receive
 *                  request data previously created by nx_recvq_get().
 *
 *  This routine is intended to be called by the TAM after it is done using
 *  the data in the associated request queue list.  All elements in the
 *  list are deallocated.
 *
 *  Parameters:
 *
 *  recvq_head  = a pointer to the head of the request data list, obtained
 *                from a previous nx_recvq_get() call.
 *
 *  Returns:
 *
 *  no return value
 *
 *
 *  Note:
 *
 *  A call with recvq_head == NULL is permitted, and no error condition results.
 *
 **************************************************************************/

#ifdef __STDC__
void nx_recvq_free(recvq_data_t *recvq_head)
#else
void nx_recvq_free(recvq_head)
recvq_data_t *recvq_head;
#endif
{
recvq_data_t *next;

	while (recvq_head != NULL) {
		next = recvq_head->next;
		free(recvq_head);
		recvq_head = next;
	}
}


/***************************************************************************
 * 
 *  nx_recvq_get - create a linked list of queued message request data.
 *
 *  This routine creates a static copy of the queued message request data for
 *  a particular process.  The format of the request data is the
 *  recvq_data_t structure declared in ipd_msg.h.
 *
 *  This routine dynamically allocates memory to create the list.  When
 *  the list is no longer needed, nx_recvq_free() should be called to
 *  deallocate this memory.
 *
 *
 *  Parameters:
 *
 *  unix_pid    = process ID denoting the process whose message queue
 *                is to be examined.
 *
 *  recvq_head  = the address of (pointer to) a pointer to the head of the
 *                request data list.  The pointer to the head of the list
 *                is filled in by this routine.
 *
 *  num_reqs    = a pointer to a variable containing the number of elements
 *                in the request queue list.  The value of this variable
 *                is filled in by this routine.
 *
 *  Returns:
 *
 *  0 if successful
 * -1 on any failure
 *
 *
 *  Note:
 *
 *
 **************************************************************************/

#ifdef __STDC__
int nx_recvq_get(pid_t unix_pid, recvq_data_t **recvq_head, long *num_reqs)
#else
int nx_recvq_get(unix_pid, recvq_head, num_reqs)
pid_t	unix_pid;
recvq_data_t **recvq_head;
long	*num_reqs;
#endif
{
ipd_msg_info_t	*ipd_msg_info_p;
ipd_msg_info_t	*ipd_msg_info;
recvq_data_t **last_link;
nxreq_t	*nxreq;
nxreq_t	*nxreq_data;
long	nxreq_size;
ipd_msg_aux_t *ipd_msg_aux;
ipd_msg_aux_t *ipd_msg_aux_data;
long	i;
long	status;
int	task;
vm_size_t ipd_msg_info_size, nxreq_data_size, ipd_msg_aux_data_size;
kern_return_t k_status;

	if (recvq_head == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), NULL recvq_head\n",
		       unix_pid);
#endif
		
		errno = EINVAL;	
		return -1;
	}

	if (num_reqs == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), NULL num_reqs\n",
		       unix_pid);
#endif
		
		errno = EINVAL;	
		return -1;
	}

	task = task_by_pid(unix_pid);
	if (task == MACH_PORT_NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), cannot get task token for pid\n",
		       unix_pid);
#endif

		errno = ESRCH;	
		return -1;
	}

	*num_reqs = 0;
	*recvq_head = NULL;
	last_link = recvq_head;
	status = table(TBL_IPD_MSG_INFO, (int)unix_pid, &ipd_msg_info_p,
	               1, sizeof(ipd_msg_info_p));
	if (status != 1) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), table() status = %d\n",
		       unix_pid, status);
#endif

		errno = ESRCH;	
		return -1;
	}

	if (ipd_msg_info_p == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), NULL ipd_msg_info_p\n",
		       unix_pid);
#endif

		errno = EINTERNAL;	
		return -1;
	}

	if ((k_status = vm_read(task, ipd_msg_info_p, sizeof(ipd_msg_info_t),
	                        &ipd_msg_info, &ipd_msg_info_size)) !=
		 KERN_SUCCESS) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), cannot read ipd_msg_info, status = %d\n",
		       unix_pid, k_status);
#endif

		errno = EINTERNAL;	
		return -1;
	}
	
	nxreq = ipd_msg_info->nxreq;
	if (nxreq == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), NULL ipd_msg_info.nxreq\n",
		       unix_pid);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		errno = EINTERNAL;	
		return -1;
	}
	nxreq_size = ipd_msg_info->nxreq_size;

	if ((k_status = vm_read(task, nxreq, nxreq_size*sizeof(nxreq_t),
	                        &nxreq_data, &nxreq_data_size)) !=
		 KERN_SUCCESS) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), cannot read nxreq[], status = %d\n",
		       unix_pid, k_status);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		errno = EINTERNAL;	
		return -1;
	}

	ipd_msg_aux = ipd_msg_info->ipd_msg_aux;
	if (ipd_msg_aux == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), NULL ipd_msg_info.ipd_msg_aux\n",
		       unix_pid);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info, ipd_msg_info_size);
		vm_deallocate(mach_task_self(), nxreq_data, nxreq_data_size);
		errno = EINTERNAL;	
		return -1;
	}

	if ((k_status = vm_read(task, ipd_msg_aux,
	                        nxreq_size*sizeof(ipd_msg_aux_t),
	                        &ipd_msg_aux_data, &ipd_msg_aux_data_size)) !=
		KERN_SUCCESS) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_recvq_get(pid=%d), cannot read ipd_msg_aux[], status = %d\n",
		       unix_pid, k_status);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info, ipd_msg_info_size);
		vm_deallocate(mach_task_self(), nxreq_data, nxreq_data_size);
		errno = EINTERNAL;	
		return -1;
	}

	for (i = 0; i < nxreq_size; i++)
		if ((nxreq_data[i].state == NX_ACTIVE) &&
		    (nxreq_data[i].req == NX_RECV_REQ)) {
			*last_link = (recvq_data_t *)malloc(sizeof(recvq_data_t));
			if (*last_link == NULL) {

#if DEBUG_IPD_MSG
				printf("ERROR: nx_recvq_get(pid=%d), insufficient memory\n",
				       unix_pid);
#endif

				vm_deallocate(mach_task_self(), ipd_msg_info,
				              ipd_msg_info_size);
				vm_deallocate(mach_task_self(), nxreq_data,
				              nxreq_data_size);
				vm_deallocate(mach_task_self(),
				              ipd_msg_aux_data,
				              ipd_msg_aux_data_size);
				nx_recvq_free(*recvq_head);
				errno = ENOMEM;	
				return -1;
			}
			(*last_link)->src_node = nxreq_data[i].node;
			(*last_link)->src_ptype = nxreq_data[i].ptype;
			(*last_link)->recv_ptype = ipd_msg_aux_data[i].ptype;
			(*last_link)->recv_type = nxreq_data[i].type;
			(*last_link)->recv_length = nxreq_data[i].bcount *
			                            nxreq_data[i].bsize;
			(*last_link)->call_type = ipd_msg_aux_data[i].call_type;
			(*last_link)->handler = (void *)nxreq_data[i].handler;
			(*last_link)->next = NULL;
			last_link = &((*last_link)->next);
			(*num_reqs)++;
		}
	vm_deallocate(mach_task_self(), ipd_msg_info, ipd_msg_info_size);
	vm_deallocate(mach_task_self(), nxreq_data, nxreq_data_size);
	vm_deallocate(mach_task_self(), ipd_msg_aux_data,
	              ipd_msg_aux_data_size);
	return 0;
}

/***************************************************************************
 *
 *  nx_ptypes_by_pid - create a list of ptypes associated with a particular
 *                     process.
 *
 *
 *  Parameters:
 *
 *  unix_pid    = the process ID whose ptypes are to be listed.
 *
 *  ptypes      = pointer to an array which is to hold the list of ptype
 *                ranges.  Even indicies contain low value of each range,
 *                odd indicies contain high value.  This array is filled
 *                in by this routine.
 *
 *  len_ptypes  = number of valid entries in ptypes array.  This value is
 *                filled in by this routine.
 *
 *  curr_ptype =  the current ptype.  This value is filled in by this
 *                routine.
 *
 *
 *  Returns:
 *
 *  non-negative value = actual number of ptypes
 *  -1 on any error.
 *
 *
 *  Note:
 *
 *
 **************************************************************************/

#ifdef __STDC__
long nx_ptypes_by_pid(pid_t unix_pid, long *ptypes,
                      long *len_ptypes, long *curr_ptype)
#else
long nx_ptypes_by_pid(unix_pid, ptypes, len_ptypes, curr_ptype)
pid_t	unix_pid;
long	*ptypes;
long	*len_ptypes;
long	*curr_ptype;
#endif
{
ipd_msg_info_t	*ipd_msg_info_p;
ipd_msg_info_t	*ipd_msg_info;
ipd_ptype_list_t *plist_p;
ipd_ptype_list_t *plist_data;
long	*current_ptype_p;
long	*current_ptype;
long	i;
long	limit;
long	status;
int	task;
long	num_ptypes;
vm_size_t ipd_msg_info_size, plist_data_size, current_ptype_size;
kern_return_t k_status;

	if (ptypes == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), NULL ptypes\n",
		       unix_pid);
#endif
		
		errno = EINVAL;	
		return -1;
	}

	if (len_ptypes == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), NULL len_ptypes\n",
		       unix_pid);
#endif
		
		errno = EINVAL;	
		return -1;
	}

	if (curr_ptype == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), NULL current_ptype\n",
		       unix_pid);
#endif
		
		errno = EINVAL;	
		return -1;
	}

	task = task_by_pid(unix_pid);
	if (task == MACH_PORT_NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), cannot get task token for pid\n",
		       unix_pid);
#endif

		errno = ESRCH;	
		return -1;
	}

	status = table(TBL_IPD_MSG_INFO, (int)unix_pid, &ipd_msg_info_p,
	               1, sizeof(ipd_msg_info_p));
	if (status != 1) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), table() status = %d\n",
		       unix_pid, status);
#endif

		errno = ESRCH;	
		return -1;
	}

	if (ipd_msg_info_p == NULL) {
		/* Application must by the control process, and has
		   not called setptype() */
		*len_ptypes = 0;
		*curr_ptype = INVALID_PTYPE;
		return 0;
	}

	if ((k_status = vm_read(task, ipd_msg_info_p, sizeof(ipd_msg_info_t),
	                        &ipd_msg_info, &ipd_msg_info_size)) !=
		KERN_SUCCESS) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), cannot read ipd_msg_info, status = %d\n",
		       unix_pid, k_status);
#endif

		errno = EINTERNAL;	
		return -1;
	}

	plist_p = ipd_msg_info->ipd_plist;
	if (plist_p == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), NULL ipd_msg_info.ipd_plist\n",
		       unix_pid);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		errno = EINTERNAL;	
		return -1;
	}

	current_ptype_p = ipd_msg_info->current_ptype;
	if (current_ptype_p == NULL) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), NULL ipd_msg_info.curent_ptype\n",
		       unix_pid);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		errno = EINTERNAL;	
		return -1;
	}

	if ((k_status = vm_read(task, current_ptype_p, sizeof(long),
	                        &current_ptype, &current_ptype_size)) !=
		KERN_SUCCESS) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), cannot read ipd_msg_info.current_ptype, status = %d\n",
		       unix_pid, k_status);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		errno = EINTERNAL;	
		return -1;
	}

	if ((k_status = vm_read(task, plist_p, sizeof(ipd_ptype_list_t),
	                        &plist_data, &plist_data_size)) !=
		KERN_SUCCESS) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), cannot read plist[], status = %d\n",
		       unix_pid, k_status);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		vm_deallocate(mach_task_self(), current_ptype,
		              current_ptype_size);
		errno = EINTERNAL;	
		return -1;
	}

	/* Sanity check here; ensure that the last_entry is either 0 or odd,
	   and that we're not overwriting the end of the ptypes array
	   (although this should never happen). */
	if ((plist_data->last_entry != 0) && !(plist_data->last_entry & 1)) {

#if DEBUG_IPD_MSG
		printf("ERROR: nx_ptypes_by_pid(pid=%d), bad last_entry value = %d\n",
		       unix_pid, plist_data->last_entry);
#endif

		vm_deallocate(mach_task_self(), ipd_msg_info,
		              ipd_msg_info_size);
		vm_deallocate(mach_task_self(), current_ptype,
		              current_ptype_size);
		vm_deallocate(mach_task_self(), plist_data, plist_data_size);
		errno = EINTERNAL;	
		return -1;
	}
	limit = plist_data->last_entry;
	if (limit > MAX_PTYPE_ENTRIES)
		limit = MAX_PTYPE_ENTRIES;

	for (i = 0; i < limit; i += 2) {
		ptypes[i] = plist_data->ptype_entry[i].ptype;
		ptypes[i+1] = plist_data->ptype_entry[i+1].ptype - 1;
	}

	*len_ptypes = plist_data->last_entry+1;
	*curr_ptype = *current_ptype & ~GLOBAL_BIT;
	num_ptypes = ipd_msg_info->num_ptypes;

	vm_deallocate(mach_task_self(), ipd_msg_info, ipd_msg_info_size);
	vm_deallocate(mach_task_self(), current_ptype, current_ptype_size);
	vm_deallocate(mach_task_self(), plist_data, plist_data_size);

	return num_ptypes;
}
