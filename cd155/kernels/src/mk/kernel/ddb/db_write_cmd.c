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
 * $Log: db_write_cmd.c,v $
 * Revision 1.5  1994/11/18  20:30:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:21:07  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:19:19  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:19:05  dleslie
 * First R1_0 release
 *
 * Revision 2.6  91/10/09  16:05:06  af
 * 	 Revision 2.5.3.1  91/10/05  13:09:25  jeffreyh
 * 		Added user space write support including inactive task.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.5.3.1  91/10/05  13:09:25  jeffreyh
 * 	Added user space write support including inactive task.
 * 	[91/08/29            tak]
 * 
 * Revision 2.5  91/05/14  15:38:04  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:07:35  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:20:19  mrt]
 * 
 * Revision 2.3  90/10/25  14:44:26  rwd
 * 	Changed db_write_cmd to print unsigned.
 * 	[90/10/19            rpd]
 * 
 * Revision 2.2  90/08/27  21:53:54  dbg
 * 	Set db_prev and db_next instead of explicitly advancing dot.
 * 	[90/08/22            dbg]
 * 	Reflected changes in db_printsym()'s calling seq.
 * 	[90/08/20            af]
 * 	Warn user if nothing was written.
 * 	[90/08/07            dbg]
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/*
 *	Author: David B. Golub,  Carnegie Mellon University
 *	Date:	7/90
 */

#include <mach/boolean.h>
#include <kern/task.h>
#include <kern/thread.h>

#include <machine/db_machdep.h>

#include <ddb/db_lex.h>
#include <ddb/db_access.h>
#include <ddb/db_command.h>
#include <ddb/db_sym.h>
#include <ddb/db_task_thread.h>

/*
 * Write to file.
 */
/*ARGSUSED*/
void
db_write_cmd(address, have_addr, count, modif)
	db_expr_t	address;
	boolean_t	have_addr;
	db_expr_t	count;
	char *		modif;
{
	register db_addr_t	addr;
	register db_expr_t	old_value;
	db_expr_t	new_value;
	register int	size;
	boolean_t	wrote_one = FALSE;
	boolean_t	t_opt, u_opt;
	thread_t	thread;
	task_t		task;
	register	char *p;
	extern task_t	db_target_task(/* db_addr_t, thread_t */);

	addr = (db_addr_t) address;

	size = db_size_option(modif, &u_opt, &t_opt);
	task = TASK_NULL;
	if (t_opt) {
	    if (!db_get_next_thread(&thread, 0))
		return;
	    if (!DB_VALID_ADDRESS(addr, u_opt)) {
		db_printf("Bad address 0x%x\n", addr);
		return;
	    }
	    task = db_target_space(thread, u_opt);
	}

	while (db_expression(&new_value)) {
	    old_value = db_get_task_value(addr, size, FALSE, task);
	    db_task_printsym(addr, DB_STGY_ANY, task);
	    db_printf("\t\t%#8n\t=\t%#8n\n", old_value, new_value);
	    db_put_task_value(addr, size, new_value, task);
	    addr += size;

	    wrote_one = TRUE;
	}

	if (!wrote_one)
	    db_error("Nothing written.\n");

	db_next = addr;
	db_prev = addr - size;
}
