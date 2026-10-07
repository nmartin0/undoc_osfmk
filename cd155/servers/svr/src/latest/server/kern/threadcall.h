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
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: threadcall.h,v $
 * Revision 1.5  1994/11/18  20:32:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:02:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:22:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:17:29  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:04  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:26  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/12/16  13:10:33  roy
 * 	Initial check-in.
 *
 * Revision 3.1  91/10/29  16:11:45  barbou
 * Restored unexpanded copyright and revision history.
 * 
 * Revision 1.3  90/10/07  13:57:22  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  10:00:09  gm]
 * 
 * Revision 1.2  90/08/24  12:04:47  devrcs
 * 	Initial version.
 * 	[90/08/19  14:08:21  jeffc]
 * 
 * $EndLog$
 */
/*
 * Header file for general-purpose thread callout routines.
 * (see threadcall.c)
 */
#ifndef _KERN_THREADCALL_H_
#define _KERN_THREADCALL_H_

#include <kern/lock.h>
#include <kern/zalloc.h>

struct thread_call {
	struct thread_call *tc_next;
	void	(*tc_func)();
	void	*tc_arg;
};
typedef struct thread_call thread_call_t;

struct thread_callq {
	decl_simple_lock_data(,tcq_lock)
	thread_call_t *tcq_head;
	thread_call_t *tcq_tail;
	zone_t	tcq_zone;
	int	tcq_zone_size;
	int	tcq_threadcall_size;
};
typedef struct thread_callq thread_callq_t;

extern boolean_t thread_call();
extern boolean_t thread_call_one();
extern void thread_call_create();
extern void thread_call_add();
extern void thread_call_alloc();

#endif	/* _KERN_THREADCALL_H_ */
