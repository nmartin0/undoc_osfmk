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
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: db_task_thread.h,v $
 * Revision 1.5  1994/11/18  20:29:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:20:58  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:19:06  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:51  dleslie
 * First R1_0 release
 *
 * Revision 2.2  91/10/09  16:03:18  af
 * 	 Revision 2.1.3.1  91/10/05  13:08:07  jeffreyh
 * 	 	Created for task/thread handling.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.1.3.1  91/10/05  13:08:07  jeffreyh
 * 	Created for task/thread handling.
 * 	[91/08/29            tak]
 * 
 */

#ifndef _DDB_DB_TASK_THREAD_H_
#define _DDB_DB_TASK_THREAD_H_

#include <kern/task.h>
#include <kern/thread.h>

#define db_current_task()						\
		((current_thread())? current_thread()->task: TASK_NULL)
#define db_target_space(thread, user_space)				\
		((!(user_space))? TASK_NULL:				\
		(thread)? (thread)->task: db_current_task())
#define db_is_current_task(task) 					\
		((task) == TASK_NULL || (task) == db_current_task())

extern task_t	db_default_task;		/* default target task */
extern thread_t	db_default_thread;		/* default target thread */

extern int		db_lookup_task(/* task_t */);
extern int		db_lookup_thread(/* thread_t */);
extern int		db_lookup_task_thread(/* task_t, thread_t */);
extern boolean_t	db_check_thread_address_valid(/* db_expr_t */);
extern boolean_t	db_get_next_thread(/* thread_t *, int */);
extern void		db_init_default_thread();

#endif  _DDB_DB_TASK_THREAD_H_
