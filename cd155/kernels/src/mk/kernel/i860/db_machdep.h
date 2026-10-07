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
 * Copyright (c) 1991 Carnegie Mellon University
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
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: db_machdep.h,v $
 * Revision 2.8  1994/11/18  20:38:45  mtm
 * Copyright additions/changes
 *
 * Revision 2.7  1994/07/12  19:18:26  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 2.6.8.1  1994/01/12  19:43:22  andyp
 * Reduce the size of the debugger history buffer.
 *
 * Revision 2.6  1993/06/30  22:30:31  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.5  1992/09/22  17:15:26  SSD
 * *** empty log message ***
 *
 * Revision 2.5  91/12/10  16:30:36  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:39  jsb]
 * 
 * Revision 2.4  91/06/18  20:50:54  jsb
 * 	New copyright, and support for single stepping, from Intel.
 * 	[91/06/18  18:50:45  jsb]
 * 
 * Revision 2.3  91/06/17  15:44:27  jsb
 * 	Added inst_load, inst_store definitions.
 * 	[91/06/17  10:25:34  jsb]
 * 
 * Revision 2.2  90/12/04  14:48:15  jsb
 * 	First checkin.
 * 	[90/12/03  21:27:35  jsb]
 * 
 */
/*
 * Machine-dependent definitions for kernel debugger.
 * Derived from i386.
 */

#ifndef	_I860_DB_MACHDEP_H_
#define	_I860_DB_MACHDEP_H_

#include <mach/i860/vm_types.h>
#include <mach/i860/vm_param.h>
#include <i860/thread.h>		/* for thread_status */
#include <i860/trap.h>

typedef	vm_offset_t	db_addr_t;	/* address - unsigned */
typedef	int		db_expr_t;	/* expression - signed */

typedef struct i860_saved_state db_regs_t;
db_regs_t	ddb_regs;	/* register state */
#define	DDB_REGS	(&ddb_regs)

#define	PC_REGS(regs)	((db_addr_t)(regs)->pc)

#define	BKPT_INST	0x44000000	/* breakpoint instruction */
#define	BKPT_SIZE	(4)		/* size of breakpoint inst */
#define	BKPT_SET(inst)	(BKPT_INST)

/*#define	FIXUP_PC_AFTER_BREAK*/	/* trap.c does this */

#define	IS_BREAKPOINT_TRAP(type, code)	((type) == T_BPTFLT)
#define IS_WATCHPOINT_TRAP(type, code)	((type) == T_WATCHPOINT)

#define	inst_trap_return(ins)	isa_rei(ins)
#define	inst_return(ins)	isa_ret(ins)
#define	inst_call(ins)		isa_call(ins)
#define inst_branch(ins)	isa_branch(ins)
#define inst_load(ins)		isa_load(ins)
#define inst_store(ins)		isa_store(ins)

#define SOFTWARE_SSTEP	1
#define DB_NO_AOUT	1
#define DB_HISTORY_SIZE	2048

/* access capability and access macros */

#define DB_ACCESS_LEVEL		2	/* access any space */
#define DB_CHECK_ACCESS(addr,size,task)			\
	db_check_access(addr,size,task)
#define DB_PHYS_EQ(task1,addr1,task2,addr2)		\
	db_phys_eq(task1,addr1,task2,addr2)
#define DB_VALID_KERN_ADDR(addr)			\
	((addr) >= VM_MIN_KERNEL_ADDRESS &&		\
	 (addr) < VM_MAX_KERNEL_ADDRESS)
#define DB_VALID_ADDRESS(addr,user)			\
	((!(user) && DB_VALID_KERN_ADDR(addr)) ||	\
	 ((user) && (addr) < VM_MIN_KERNEL_ADDRESS))

boolean_t	db_check_access(/* vm_offset_t, int, task_t */);
boolean_t	db_phys_eq(/* task_t, vm_offset_t, task_t, vm_offset_t */);

/* macros for printing OS server dependent task name */

#define DB_TASK_NAME(task)	db_task_name(task)
#define DB_TASK_NAME_TITLE	"COMMAND                "
#define	DB_TASK_NAME_LEN	23
#define DB_NULL_TASK_NAME	"?                      "

void		db_task_name(/* task_t */);

#endif	_I860_DB_MACHDEP_H_
