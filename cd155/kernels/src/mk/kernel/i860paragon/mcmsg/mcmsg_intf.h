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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_intf.h,v 1.9 1994/11/18 20:44:20 mtm Exp $
 */

/*
 * mcmsg_intf.h
 *
 * User message interface structure format
 */

#ifndef MCMSG_INTF_H
#define MCMSG_INTF_H

#define INVALID_PTYPE	-1

typedef
struct {
	unsigned long	mynode;
	unsigned long	numnodes;
	long		app;
	long		pid;
	long		ptype;

	unsigned long	pkt_size;
	unsigned long	memory_each;
	unsigned long	send_threshold;
	unsigned long	send_count;
	unsigned long	give_threshold;
	long		process_lock;

	unsigned long	addrspace;
	unsigned long	addrsize;

} mcmsg_intf_t;

typedef
struct {
	unsigned long	mynode;
	unsigned long	numnodes;
	APPLINFO_T	applinfo;
} nodeinfo_t;

#endif MCMSG_INTF_H
