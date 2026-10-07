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
 * $Log: db_machdep.h,v $
 * Revision 1.5  1994/11/18  20:32:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:23:28  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:22:05  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:21:39  dleslie
 * First R1_0 release
 *
 * Revision 2.7.3.1  92/03/03  16:14:36  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  11:05:44  jeffreyh]
 * 
 * Revision 2.8  92/02/19  15:07:56  elf
 * 	Added db_thread_fp_used.
 * 	[92/02/19            rpd]
 * 
 * Revision 2.7  91/10/09  16:06:28  af
 * 	 Revision 2.6.3.1  91/10/05  13:10:32  jeffreyh
 * 	 	Added access and task name macros.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.6.3.1  91/10/05  13:10:32  jeffreyh
 * 	Added access and task name macros.
 * 	[91/08/29            tak]
 * 
 * Revision 2.6  91/05/14  16:05:58  mrt
 * 	Correcting copyright
 *
 * Revision 2.5  91/02/05  17:11:17  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:31:24  mrt]
 * 
 * Revision 2.4  91/01/08  15:10:16  rpd
 * 	Added dummy inst_load/inst_store macros.
 * 	[90/12/11            rpd]
 * 
 * Revision 2.3  90/10/25  14:44:49  rwd
 * 	Added watchpoint support.
 * 	[90/10/18            rpd]
 * 
 * Revision 2.2  90/08/27  21:56:15  dbg
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */

#ifndef	_I386_DB_MACHDEP_H_
#define	_I386_DB_MACHDEP_H_

/*
 * Machine-dependent defines for new kernel debugger.
 */

#include <mach/i386/vm_types.h>
#include <mach/i386/vm_param.h>
#include <i386/thread.h>		/* for thread_status */
#include <i386/eflags.h>
#include <i386/trap.h>

typedef	vm_offset_t	db_addr_t;	/* address - unsigned */
typedef	int		db_expr_t;	/* expression - signed */

typedef struct i386_saved_state db_regs_t;
db_regs_t	ddb_regs;	/* register state */
#define	DDB_REGS	(&ddb_regs)

#define	PC_REGS(regs)	((db_addr_t)(regs)->eip)

#define	BKPT_INST	0xcc		/* breakpoint instruction */
#define	BKPT_SIZE	(1)		/* size of breakpoint inst */
#define	BKPT_SET(inst)	(BKPT_INST)

#define	FIXUP_PC_AFTER_BREAK	ddb_regs.eip -= 1;

#define	db_clear_single_step(regs)	((regs)->efl &= ~EFL_TF)
#define	db_set_single_step(regs)	((regs)->efl |=  EFL_TF)

#define	IS_BREAKPOINT_TRAP(type, code)	((type) == T_INT3)
#define IS_WATCHPOINT_TRAP(type, code)	((type) == T_WATCHPOINT)

#define	I_CALL		0xe8
#define	I_CALLI		0xff
#define	I_RET		0xc3
#define	I_IRET		0xcf

#define	inst_trap_return(ins)	(((ins)&0xff) == I_IRET)
#define	inst_return(ins)	(((ins)&0xff) == I_RET)
#define	inst_call(ins)		(((ins)&0xff) == I_CALL || \
				 (((ins)&0xff) == I_CALLI && \
				  ((ins)&0x3800) == 0x1000))
#define inst_load(ins)		0
#define inst_store(ins)		0

/* access capability and access macros */

#define DB_ACCESS_LEVEL		2	/* access any space */
#define DB_CHECK_ACCESS(addr,size,task)				\
	db_check_access(addr,size,task)
#define DB_PHYS_EQ(task1,addr1,task2,addr2)			\
	db_phys_eq(task1,addr1,task2,addr2)
#define DB_VALID_KERN_ADDR(addr)				\
	((addr) >= VM_MIN_KERNEL_ADDRESS && 			\
	 (addr) < VM_MAX_KERNEL_ADDRESS)
#define DB_VALID_ADDRESS(addr,user)				\
	((!(user) && DB_VALID_KERN_ADDR(addr)) ||		\
	 ((user) && (addr) < VM_MIN_KERNEL_ADDRESS))

boolean_t 	db_check_access(/* vm_offset_t, int, task_t */);
boolean_t	db_phys_eq(/* task_t, vm_offset_t, task_t, vm_offset_t */);

/* macros for printing OS server dependent task name */

#define DB_TASK_NAME(task)	db_task_name(task)
#define DB_TASK_NAME_TITLE	"COMMAND                "
#define DB_TASK_NAME_LEN	23
#define DB_NULL_TASK_NAME	"?                      "

void		db_task_name(/* task_t */);

/* macro for checking if a thread has used floating-point */

#define db_thread_fp_used(thread)	((thread)->pcb->ims.ifps != 0)

#endif	/* _I386_DB_MACHDEP_H_ */
