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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_xmsg.h,v 1.11 1994/11/18 20:42:03 mtm Exp $
 */

/*
 * mcmsg_xmsg.h
 *
 * User message buffer format
 */

#ifndef MCMSG_XMSG_H
#define MCMSG_XMSG_H

#define XMSG_FREE	0
#define	XMSG_EMPTY	1
#define XMSG_FULL	2
#define XMSG_TRASH	3
#define XMSG_STOP	4
#define XMSG_CONT	5

typedef
struct xmsg {
	unsigned short	state;
	unsigned short	source_node;
	struct xmsg	*link;
	unsigned long	size;
	unsigned long	length;
	struct select_item *si;
	long		dest_ptype;
	long		source_ptype;
	long		msg_type;
} xmsg_t;

/*
 *	xmsg_offset		the offset into the user buffer for this xmsg
 *	xmsg_data		the current offset into the xmsg
 *	xmsg_stop		the amount of valid data in the xmsg
 */
#define xmsg_offset	msg_type
#define xmsg_data	source_ptype
#define xmsg_stop	dest_ptype

/* macros to provide necessary information for performance monitoring */

#ifndef NO_PERFMON

extern unsigned long *_nx_perfmon_msginfo;
extern xmsg_t        *_nx_perfmon_xmsgt;

    /* fake up a label for a specified point in a source */
#define __PERFMON_LABEL__(label) \
  do { \
    asm (".nx$perfmon_" #label ":"); \
    asm (".def nx$perfmon_" #label "; .val .nx$perfmon_" #label "; .scl 6; .type 0; .endef"); \
    asm ("nop"); \
    asm ("nop"); \
  } while (0)

#define PERFMON_IDLE_START(proc)     __PERFMON_LABEL__(idle##proc##_start)
#define PERFMON_IDLE_END(proc)       __PERFMON_LABEL__(idle##proc##_end)
#define PERFMON_RECV_INFO(proc,info) \
  do {\
    _nx_perfmon_msginfo = info; \
    __PERFMON_LABEL__(msginfo##proc); \
  } while (0)
#define PERFMON_XMSGT_INFO(proc,info) \
  do {\
    _nx_perfmon_xmsgt = info; \
    __PERFMON_LABEL__(xmsgt##proc); \
  } while (0)
#define PERFMON_INTERRUPT_START()    __PERFMON_LABEL__(interrupt_start)
#define PERFMON_INTERRUPT_END()      __PERFMON_LABEL__(interrupt_end)
#define PERFMON_USER_HANDLER_START() __PERFMON_LABEL__(user_handler_start)
#define PERFMON_USER_HANDLER_END()   __PERFMON_LABEL__(user_handler_end)

#else

#define __PERFMON_LABEL__(label)
#define PERFMON_IDLE_START(proc)
#define PERFMON_IDLE_END(proc)
#define PERFMON_RECV_INFO(proc,info)
#define PERFMON_XMSGT_INFO(proc,info)
#define PERFMON_INTERRUPT_START()
#define PERFMON_INTERRUPT_END()
#define PERFMON_USER_HANDLER_START()
#define PERFMON_USER_HANDLER_END()

#endif /* !NO_PERFMON */

#endif MCMSG_XMSG_H
