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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_trace.h,v 1.13 1994/11/18 20:42:02 mtm Exp $
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

#define MAXLOOP	10000

#define DANGEROUS 0
#define NODUPS	1

#if MACH_ASSERT
#define MSG_TRACE 1
#endif MACH_ASSERT

#if	MSG_TRACE

#define	mcmsg_trace_recv(h1,h2,h3,x,x1,x2) mcmsg_trace_recv_(h1,h2,h3,x,x1,x2)
#define mcmsg_trace_send(h1,h2,h3,x,x1,x2) mcmsg_trace_send_(h1,h2,h3,x,x1,x2)
#define mcmsg_trace_provide(xmsg,xp)	mcmsg_trace_provide_(xmsg,xp)
#define mcmsg_trace_drop(s,v)		mcmsg_trace_drop_(s,v)
#define mcmsg_trace_debug(s,n,v1,v2,v3,v4) mcmsg_trace_debug_(s,n,v1,v2,v3,v4)
#define mcmsg_trace_time(s) mcmsg_trace_time_(s)

#else	MSG_TRACE

#define	mcmsg_trace_recv(h1,h2,h3,x,x1,x2)
#define mcmsg_trace_send(h1,h2,h3,x,x1,x2)
#define mcmsg_trace_provide(xmsg,xp)
#define mcmsg_trace_drop(s,v)
#define mcmsg_trace_debug(s,n,v1,v2,v3,v4)
#define mcmsg_trace_time(s)

#endif	MSG_TRACE

#endif MCMSG_TRACE_H
