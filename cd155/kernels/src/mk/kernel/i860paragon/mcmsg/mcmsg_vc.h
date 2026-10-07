/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
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
 *	Copyright 1991  Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_vc.h,v 1.2 1994/11/18 20:44:52 mtm Exp $

/*
 * mcmsg_vc.h
 *
 * Virtual Connections
 */

#ifndef	MCMSG_VC_H
#define	MCMSG_VC_H

#define	VC_INIT		1
#define VC_OPEN_ONE	2
#define VC_CLOSE_ONE	3

#define	VC_MAX	100
#define VC_NREQ	3

typedef struct vc_req {
	unsigned long	buf;
	unsigned long	count:	30;
	unsigned long	active:	1;
	unsigned long	intreq:	1;
} vc_req_t;

#define	VC_STATE_FREE	0
#define	VC_STATE_TAGGED	1
#define	VC_STATE_WAIT	2
#define	VC_STATE_ACTIVE	3

typedef struct vc_entry {
	mcmsg_task_t	*mcmsg_task;
	unsigned long	route;
	unsigned long	remote_index:	16;
	unsigned long	state:		2;

	unsigned long	local_freeze:	1;
	unsigned long	remote_freeze:	1;
	unsigned long	freeze_int:	1;

	unsigned long	send_slot:	2;
	unsigned long	recv_slot:	2;
	unsigned long	srdy_slot:	2;
	unsigned long	rrdy_slot:	2;

	unsigned long	send_req_ready_count;
	
	vc_req_t	send_req[VC_NREQ];
	vc_req_t	recv_req[VC_NREQ];
} vc_entry_t;

vc_entry_t	vctab[VC_MAX];

int	vc_low_free;

#endif MCMSG_VC_H
