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
 * $Log: ast_check.c,v $
 * Revision 1.7  1994/11/18  20:38:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/15  19:40:34  lenb
 *         ast_send[] is debug only
 *
 * Revision 1.5  1994/05/12  19:37:51  lenb
 *         use CPU_INT_REASON_SET macro
 *
 * Revision 1.4  1993/10/07  14:51:55  stans
 *    Added spl verification assert.
 *
 */

#include <mach_assert.h>
#include <cpus.h>

#if	NCPUS > 1

/*
 * Handle signaling ASTs on other processors.
 *
 */

#include <i860paragon/mp.h>
#include <i860paragon/spl.h>
#include <kern/processor.h>
#include <kern/assert.h>

#if	MACH_ASSERT
int ast_send[NCPUS];
#endif	MACH_ASSERT

/*
 * Initialize for remote invocation of ast_check.
 */
init_ast_check(processor)
	processor_t	processor;
{
#ifdef lint
	processor++;
#endif lint
}

/*
 * Cause remote invocation of ast_check.  Caller is at splsched().
 */
cause_ast_check(processor)
	processor_t	processor;
{
        register int target_cpu = processor->slot_num;
#if	MACH_ASSERT
	extern int	get_spl();

	assert( get_spl() == SPLSCHED );
        ast_send[cpu_number()]++;
#endif	MACH_ASSERT

	CPU_INT_REASON_SET(MP_AST, target_cpu);
	cpu_interrupt(target_cpu);
}

#endif	/* NCPUS > 1 */
