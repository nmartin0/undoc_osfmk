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
 * $Log: db_break.h,v $
 * Revision 1.5  1994/11/18  20:29:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:20:23  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:18:26  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:01  dleslie
 * First R1_0 release
 *
 * Revision 2.6  91/10/09  15:58:03  af
 * 	 Revision 2.5.3.1  91/10/05  13:05:04  jeffreyh
 * 	 	Added db_thread_breakpoint structure, and added task and threads
 * 	 	field to db_breakpoint structure.  Some status flags were also
 * 	 	added to keep track user space break point correctly.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.5.3.1  91/10/05  13:05:04  jeffreyh
 * 	Added db_thread_breakpoint structure, and added task and threads
 * 	field to db_breakpoint structure.  Some status flags were also
 * 	added to keep track user space break point correctly.
 * 	[91/08/29            tak]
 * 
 * Revision 2.5  91/05/14  15:32:35  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:06:06  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:17:10  mrt]
 * 
 * Revision 2.3  90/10/25  14:43:40  rwd
 * 	Added map field to breakpoints.
 * 	[90/10/18            rpd]
 * 
 * Revision 2.2  90/08/27  21:50:00  dbg
 * 	Modularized typedef names.
 * 	[90/08/20            af]
 * 	Add external defintions.
 * 	[90/08/07            dbg]
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */
#ifndef	_DDB_DB_BREAK_H_
#define	_DDB_DB_BREAK_H_

#include <machine/db_machdep.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <mach/boolean.h>

/*
 * thread list at the same breakpoint address
 */
struct db_thread_breakpoint {
	boolean_t tb_is_task;			/* task qualified */
	unsigned tb_task_thd;			/* target task or thread */
	short	 tb_number;			/* breakpoint number */
	short	 tb_init_count;			/* skip count(initial value) */
	short	 tb_count;			/* current skip count */
	short	 tb_cond;			/* break condition */
	struct	 db_thread_breakpoint *tb_next;	/* next chain */
};

typedef struct db_thread_breakpoint *db_thread_breakpoint_t;

/*
 * Breakpoint.
 */

struct db_breakpoint {
	task_t	  task;			/* target task */
	db_addr_t address;		/* set here */
	db_thread_breakpoint_t threads; /* thread */
	int	flags;			/* flags: */
#define	BKPT_SINGLE_STEP	0x2	/* to simulate single step */
#define	BKPT_TEMP		0x4	/* temporary */
#define BKPT_USR_GLOBAL		0x8	/* global user space break point */
#define BKPT_SET_IN_MEM		0x10	/* break point is set in memory */
#define BKPT_1ST_SET		0x20	/* 1st time set of user global bkpt */
	int	bkpt_inst;		/* saved instruction at bkpt */
	struct db_breakpoint *link;	/* link in in-use or free chain */
};

typedef struct db_breakpoint *db_breakpoint_t;

extern db_breakpoint_t	db_find_breakpoint(/* task_t, db_addr_t */);
extern boolean_t	db_find_breakpoint_here(/* task_t, db_addr_t */);
extern void		db_set_breakpoints();
extern void		db_clear_breakpoints();
extern db_thread_breakpoint_t	db_find_thread_breakpoint_here
					(/* db_breakpoint_t, thread_t */);
extern db_thread_breakpoint_t	db_find_breakpoint_number(/* int */);

extern db_breakpoint_t	db_set_temp_breakpoint(/* task_t, db_addr_t */);
extern void		db_delete_temp_breakpoint(/* task_t,db_breakpoint_t */);

#endif	_DDB_DB_BREAK_H_
