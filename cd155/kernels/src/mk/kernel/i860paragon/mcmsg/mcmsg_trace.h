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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_trace.h,v 1.18 1994/11/18 20:44:50 mtm Exp $
 */

/*
 * mcmsg_trace.h
 *
 * Tracing support for debugging
 */

#ifndef MCMSG_TRACE_H
#define MCMSG_TRACE_H

#if		PARAGON860
#define	NTRACE	10000
#else
#define NTRACE 1000
#endif	PARAGON860

#define NIC_TRACE	MACH_ASSERT

#define MAXLOOP	300000

#define DANGEROUS 0
#define NODUPS	1

#define MSG_TRACE MACH_ASSERT || MSGTRACE

#if	MSG_TRACE

#define	mcmsg_trace_recv(h1,h2,h3,x,x1,x2) \
	mcmsg_trace_recv_(MCMSG_MODULE,h1,h2,h3,x,x1,x2)
#define mcmsg_trace_send(h1,h2,h3,x,x1,x2) \
	mcmsg_trace_send_(MCMSG_MODULE,h1,h2,h3,x,x1,x2)
#define mcmsg_trace_provide(xmsg,xp)	\
	mcmsg_trace_provide_(MCMSG_MODULE,xmsg,xp)
#define mcmsg_trace_drop(s,v)		\
	mcmsg_trace_drop_(MCMSG_MODULE,s,v)
#define mcmsg_trace_debug(s,n,v1,v2,v3,v4) \
	mcmsg_trace_debug_(MCMSG_MODULE,s,n,v1,v2,v3,v4)
#define mcmsg_trace_time(s) \
	mcmsg_trace_time_(MCMSG_MODULE,s)

#if	MCMSG_TRACE_MODULE

struct {
	unsigned char	t_id;
	unsigned char	t_module;
	unsigned char	extra;
	unsigned char	stack;
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	hdr3;
	unsigned long	extra1;
	unsigned long	extra2;
	unsigned long	hwtime[2];
} mcmsg_trace_buf[NTRACE+10];	/* + 10 to keep CPU0 from overshooting end */

#define t_hdr1		hdr1
#define t_hdr2		hdr2
#define t_hdr3		hdr3
#define t_control	hdr1 & 0xFFFF
#define t_give		hdr1 >> 16  << 5
#define t_length	hdr2 & 0xFFFF
#define t_sequence	hdr2 >> 16
#define t_source_pid	hdr3

#define TRACE_SEND	1
#define	TRACE_RECV	2
#define TRACE_PROVIDE	3
#define TRACE_DROP	4
#define TRACE_DEBUG	5
#define TRACE_TIME	6
#define TRACE_CALL	7
#define TRACE_CALLX	8

unsigned long	mcmsg_trace_index;
unsigned long	mcmsg_trace_count;
unsigned long	mcmsg_trace_module_mask;

#endif	MCMSG_TRACE_MODULE

#else	MSG_TRACE

#define	mcmsg_trace_recv(h1,h2,h3,x,x1,x2)
#define mcmsg_trace_send(h1,h2,h3,x,x1,x2)
#define mcmsg_trace_provide(xmsg,xp)
#define mcmsg_trace_drop(s,v)
#define mcmsg_trace_debug(s,n,v1,v2,v3,v4)
#define mcmsg_trace_time(s)

#endif	MSG_TRACE

#endif MCMSG_TRACE_H
