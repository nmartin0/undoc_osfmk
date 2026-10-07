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
 * $Log: db_access.c,v $
 * Revision 1.6  1994/11/18  20:28:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/02/01  01:54:18  raya
 * Revision 1.4.6.1  1994/02/01  01:50:14  raya
 * Added db_check_task_addr() to check to see that page is present in
 * memory.  This allows the trace command to check a page before trying
 * to read it so that the t/uT command can continue with the next thread
 * when it encounters a thread that is partially paged out.
 *
 *  Reviewer:              Don B.
 *  Risk:                  Low
 *  Benefit or PTS #:      7993
 *  Testing:               Limited test of the t/uT command.
 *  Module(s):             i860/db_trace.c ddb/db_access.c
 *
 * Revision 1.4  1993/06/30  22:20:16  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:18:17  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:17:51  dleslie
 * First R1_0 release
 *
 * Revision 2.5  91/10/09  15:56:44  af
 * 	 Revision 2.4.3.1  91/10/05  13:03:41  jeffreyh
 * 	 	Added db_{get,put}_task_value, and changed db_{get,put}_value
 * 	 	  to call them.  db_{get,put}_value are left for compatibility
 * 	 	  reason.
 * 	 	Added "task" parameter to specifiy target task space.
 * 	 	Added db_access_level to indicate implementation dependent
 * 	 	  access capability.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.4.3.1  91/10/05  13:03:41  jeffreyh
 * 	Added db_{get,put}_task_value, and changed db_{get,put}_value
 * 	  to call them.  db_{get,put}_value are left for compatibility
 * 	  reason.
 * 	Added "task" parameter to specifiy target task space.
 * 	Added db_access_level to indicate implementation dependent
 * 	  access capability.
 * 	[91/08/29            tak]
 * 
 * Revision 2.4  91/05/14  15:31:33  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:05:44  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:16:22  mrt]
 * 
 * Revision 2.2  90/08/27  21:48:20  dbg
 * 	Fix type declarations.
 * 	[90/08/07            dbg]
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */
#include <mach/boolean.h>
#include <machine/db_machdep.h>		/* type definitions */
#include <machine/setjmp.h>
#include <kern/task.h>
#include <ddb/db_access.h>

/*
 * Access unaligned data items on aligned (longword)
 * boundaries.
 */

extern void	db_read_bytes();	/* machine-dependent */
extern void	db_write_bytes();	/* machine-dependent */

int db_access_level = DB_ACCESS_LEVEL;

int db_extend[] = {	/* table for sign-extending */
	0,
	0xFFFFFF80,
	0xFFFF8000,
	0xFF800000
};

db_expr_t
db_get_task_value(addr, size, is_signed, task)
	db_addr_t	addr;
	register int	size;
	boolean_t	is_signed;
	task_t		task;
{
	char		data[sizeof(int)];
	register db_expr_t value;
	register int	i;

	db_read_bytes(addr, size, data, task);

	value = 0;
#if	BYTE_MSF
	for (i = 0; i < size; i++)
#else	/* BYTE_LSF */
	for (i = size - 1; i >= 0; i--)
#endif
	{
	    value = (value << 8) + (data[i] & 0xFF);
	}
	    
	if (size < 4) {
	    if (is_signed && (value & db_extend[size]) != 0)
		value |= db_extend[size];
	}
	return (value);
}

void
db_put_task_value(addr, size, value, task)
	db_addr_t	addr;
	register int	size;
	register db_expr_t value;
	task_t		task;
{
	char		data[sizeof(int)];
	register int	i;

#if	BYTE_MSF
	for (i = size - 1; i >= 0; i--)
#else	/* BYTE_LSF */
	for (i = 0; i < size; i++)
#endif
	{
	    data[i] = value & 0xFF;
	    value >>= 8;
	}

	db_write_bytes(addr, size, data, task);
}

db_expr_t
db_get_value(addr, size, is_signed)
	db_addr_t	addr;
	int		size;
	boolean_t	is_signed;
{
	return(db_get_task_value(addr, size, is_signed, TASK_NULL));
}

void
db_put_value(addr, size, value)
	db_addr_t	addr;
	int		size;
	db_expr_t	value;
{
	db_put_task_value(addr, size, value, TASK_NULL);
}

int
db_check_task_addr(addr, task, flag)
        db_addr_t       addr;
        task_t          task;
        int             flag;
{
        db_addr_t       kaddr;
        if( db_user_to_kernel_address(task, addr, &kaddr, 0) < 0 ) {
            if ( flag ) {
                db_printf("\nno memory is assigned to address %08x\n", addr);
            }
            return(-1);
        }
        return(0);
}

