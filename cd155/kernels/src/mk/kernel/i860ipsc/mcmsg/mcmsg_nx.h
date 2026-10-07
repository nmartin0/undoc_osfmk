/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_nx.h,v 1.9 1994/11/18 20:41:47 mtm Exp $
 */

/*
 * mcmsg_nx.h
 *
 * External declarations
 */

#ifndef MCMSG_NX_H
#define MCMSG_NX_H

/*
 * OR'ed into Source Ptype For Global Send
 */
#define	GLOBAL_BIT	0x40000000
#define MAX_GLOBAL	20

/*
 * Base type of reserved system messages
 */
#define MSG_BASE_TYPE 1000000000
#define RESERVED_BASE_TYPE MSG_BASE_TYPE

#define FORCE_FLAG	0x40000000

#define NX_FREE		0
#define NX_ACTIVE	1
#define NX_COMPLETE	2
#define NX_BUFFERED	3

#define	NX_SEND_REQ	0
#define NX_RECV_REQ	1

typedef
struct nxreq {
	long		state;
	unsigned long	*info;
	unsigned long	link;
	xmsg_t		*xmsg;
	long		req;
	long		type;
	unsigned long	node;
	long		ptype;
	unsigned long	bcount;
	unsigned long	buf;
	unsigned long	bsize;
	unsigned long	boffset;
	xmsg_t		*incoming;
	unsigned long	update;
	unsigned long	handler;	/* for hsend/hrecv */
	unsigned long	hparam;		/* for hsendx/hrecvx */
	long		localinfo[8];
} nxreq_t;


/*
 *  NX Kernel Port Request Structure.
 */
typedef struct  mcmsg_nxport_req {
	unsigned long	task;
	unsigned long	op;
	nxreq_t			*nxreq;
	unsigned long	fill1;
} mcmsg_nxport_req_t;

/*
 *	Possible values for op field of mcmsg_nxport_req_t.
 */
#define NXPORT_OP_INVOKE_HANDLER    1
#define NXPORT_OP_MISSING_PAGE      2

#endif MCMSG_NX_H
