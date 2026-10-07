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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_ipc.h,v 1.7 1994/11/18 20:41:36 mtm Exp $
 */

/*
 * mcmsg_ipc.h
 *
 * Data structures for Mach IPC
 */

#if	PARAGON860
#define MAXIPCREQ	1
#endif	PARAGON860

#if	iPSC860
#define MAXIPCREQ	10
#endif	iPSC860

#define MAXBV		3
#define INVALID_NODE	65535

typedef
struct ipcreq {
	struct ipcreq	*link;
	unsigned long	total;
	unsigned short	node;
	unsigned short	count;
	struct netvec	bv[MAXBV];
} ipcreq_t;
