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
 *        $Id: dbglib.h,v 1.6 1994/11/18 20:40:12 mtm Exp $
 *    
 *    Description:
 *        System calls and definitions that support monitoring of NX
 *        applications.
 *    
 **************************************************************************/

#ifndef __DBGLIB_H__
#define __DBGLIB_H__


#ifdef _KERNEL
#include <i860paragon/mcmsg/mcmsg_xmsg.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#else
#include <mcmsg/mcmsg_xmsg.h>
#include <mcmsg/mcmsg_nx.h>
#include <mcmsg/mcmsg_appl.h>
#include <mcmsg/mcmsg_intf.h>
#endif

/* extra option for nx_wait() */
#define WNOWAIT         0x4  /* Get status but leave the process such
                                that wait can be called again and
                                receive the same status. */


/* Note -- the following define is a duplicate of the a define in the
   kernel file kern/i860ipsc/mcmsg/mcmsg_task.h */
#define PTYPE_ENTRIES	31  /* maximum number of ptype ranges supported by OS */
#define MAX_PTYPE_ENTRIES PTYPE_ENTRIES-1

#define IPD_UNKNOWN_TYPE 	-1
#define IPD_CSEND_TYPE		1
#define IPD_CRECV_TYPE		2
#define IPD_CSENDRECV_TYPE	3
#define IPD_ISEND_TYPE		4
#define IPD_IRECV_TYPE		5
#define IPD_ISENDRECV_TYPE	6
#define IPD_HSEND_TYPE		7
#define IPD_HRECV_TYPE		8
#define IPD_HSENDRECV_TYPE	9
#define IPD_CRECVX_TYPE		10
#define IPD_IRECVX_TYPE		11
#define IPD_HSENDX_TYPE		12
#define IPD_HRECVX_TYPE		13
#define IPD_CSENDXS_TYPE	14
#define IPD_CRECVXS_TYPE	15
#define IPD_ISENDXS_TYPE	16
#define IPD_IRECVXS_TYPE	17
#define IPD_HSENDXS_TYPE	18
#define IPD_HRECVXS_TYPE	19
#define IPD_CSENDXV_TYPE	20
#define IPD_CRECVXV_TYPE	21
#define IPD_ISENDXV_TYPE	22
#define IPD_IRECVXV_TYPE	23
#define IPD_HSENDXV_TYPE	24
#define IPD_HRECVXV_TYPE	25
#define IPD_IPROBEX_TYPE	26


typedef struct msgq_data {
	long	src_node;	/* sender's node number */
	long	src_ptype;	/* sender's ptype */
	long	src_app;	/* sender's application ID */
	long	dest_node;	/* destination node number */
	long	dest_ptype;	/* destination ptype */
	long	dest_app;	/* destination application ID */
	long	msg_type;	/* message type */
	long	msg_length;	/* message length */
	void	*msg_data;	/* pointer to message data */
	struct msgq_data *next; /* pointer to next msgq_data in linked list */
} msgq_data_t;

typedef struct recvq_data {
	long	src_node;	/* sender's node */
	long	src_ptype;	/* sender's ptype */
	long	recv_ptype;	/* receiver's ptype */
	long	recv_type;	/* message type to be received */
	long	recv_length;	/* length of receive buffer */
	long	call_type;	/* call that sent the message */
	void	(*handler)();	/* handler for hrecv */
	struct recvq_data *next; /* pointer to next recvq_data in list */
} recvq_data_t;

typedef
struct ipd_ptype_list {
	long		method;
	unsigned long	last_entry;
	long		app;
	unsigned long	numnodes;
	unsigned long	*phys_node_list;
	struct ptype_list *node_ptype_list;
	unsigned long	ref_count;
	unsigned long	fill7;
	struct ptype_entry {
		long		ptype;
		long		item;
		long		offset;
	} ptype_entry[PTYPE_ENTRIES];
} ipd_ptype_list_t;

typedef struct ipd_msg_aux {
	long	call_type;
	long	ptype;
} ipd_msg_aux_t;

typedef struct ipd_msg_info {
	xmsg_t	**buffer_p;
	unsigned long buffer_size;
	nxreq_t	*nxreq;
	long	nxreq_size;
	ipd_ptype_list_t	*ipd_plist;
	long	num_ptypes;
	long	*current_ptype;
	long	*app_id;
	long	nodenum;
	ipd_msg_aux_t *ipd_msg_aux;
} ipd_msg_info_t;


#ifndef _KERNEL
#ifdef __STDC__
void nx_msgq_free(msgq_data_t *msgq_head);
#else
void nx_msgq_free();
#endif

#ifdef __STDC__
int nx_msgq_get(pid_t unix_pid, msgq_data_t **msgq_head, long *num_msgs);
#else
int nx_msgq_get();
#endif

#ifdef __STDC__
void nx_recvq_free(recvq_data_t *recvq_head);
#else
void nx_recvq_free();
#endif

#ifdef __STDC__
int nx_recvq_get(pid_t unix_pid, recvq_data_t **recvq_head, long *num_reqs);
#else
int nx_recvq_get();
#endif

#ifdef __STDC__
long nx_ptypes_by_pid(pid_t unix_pid, long *ptypes,
                      long *len_ptypes, long *current_ptype);
#else
long nx_ptypes_by_pid();
#endif

#ifdef __STDC__
int  nx_join( pid_t pgrp );
#else
int  nx_join();
#endif

#ifdef __STDC__
int  nx_attach_pgrp( pid_t pgrp );
#else
int  nx_attach_pgrp();
#endif

#ifdef __STDC__
int  nx_detach_pgrp( pid_t pgrp );
#else
int  nx_detach_pgrp();
#endif

#ifdef __STDC__
int  nx_wait( pid_t pgrp, int *status, int option );
#else
int  nx_wait();
#endif

#ifdef __STDC__
int  nx_ptrace( int req, pid_t pid, int *addr, int data );
#else
int  nx_ptrace();
#endif

#ifdef __STDC__
int  nx_traced_add_mem(pid_t Pid, int * naddr, size_t * len);
#else
int  nx_traced_add_mem();
#endif

#ifdef __STDC__
int  nx_traced_delete_mem(pid_t Pid, int naddr, size_t len);
#else
int  nx_traced_delete_mem();
#endif
#endif	/* _KERNEL */

#endif /* __DBGLIB_H__ */
