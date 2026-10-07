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
 * @OSF_FREE_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 */

/*
 *	kern/ast.h: Definitions for Asynchronous System Traps.
 */

#ifndef	_KERN_AST_H_
#define _KERN_AST_H_

/*
 *	A CPU takes an AST when it is about to return to user code.
 *	Instead of going back to user code, it calls ast_taken.
 *	Machine-dependent code is responsible for maintaining
 *	a set of reasons for an AST, and passing this set to ast_taken.
 */

#include <cpus.h>

#include <kern/cpu_number.h>
#include <kern/macro_help.h>
#include <machine/ast.h>

/*
 *	Bits for reasons
 */

#define	AST_ZILCH	(0)
#define AST_HALT	(1 << 0)
#define AST_TERMINATE	(1 << 1)
#define AST_BLOCK	(1 << 2)
#define AST_NETWORK	(1 << 3)
#define AST_NETIPC	(1 << 4)
#define AST_DIPC_DEALLOC	(1 << 5)
#if	PARAGON860
#ifdef	MCMSG
#define AST_MCMSG	(1 << 7)
#endif	/* MCMSG */
#define AST_NETSEND	(1 << 8)
#define AST_NETRECV	(1 << 9)
#define AST_RPCREQ	(1 << 10)
#define AST_RPCREPLY	(1 << 11)
#define AST_RPCDEPART	(1 << 12)
#define AST_RDMASEND	(1 << 13)
#define AST_RDMARECV	(1 << 14)
#define	AST_RDMATXF	(1 << 15)
#define	AST_RDMARXF	(1 << 16)
#define	AST_SCAN_INPUT	(1 << 17)
#define	AST_SCAN_OUTPUT	(1 << 18)
#define	AST_CONSLOG	(1 << 19)
#endif	PARAGON860

#define	AST_SCHEDULING	(AST_HALT|AST_TERMINATE|AST_BLOCK)

typedef unsigned int ast_t;

extern volatile ast_t need_ast[NCPUS];

#ifdef	MACHINE_AST
/*
 *	machine/ast.h is responsible for defining aston and astoff.
 */
#else	MACHINE_AST

#define aston(mycpu)
#define astoff(mycpu)

#endif	MACHINE_AST

extern void ast_taken();

/*
 * Per-thread ASTs are reset at context-switch time.
 */

#ifndef MACHINE_AST_PER_THREAD
#define MACHINE_AST_PER_THREAD  0
#endif

#define AST_PER_THREAD (AST_HALT|AST_TERMINATE|MACHINE_AST_PER_THREAD)

/*
 *	ast_needed, ast_on, ast_off, ast_context, and ast_propagate
 *	assume splsched.  mycpu is always cpu_number().  It is an
 *	argument in case cpu_number() is expensive.
 */

#define ast_needed(mycpu)		need_ast[mycpu]

#define ast_on(mycpu, reasons)					\
MACRO_BEGIN							\
	{ int s = sploff();					\
	if ((need_ast[mycpu] |= (reasons)) != AST_ZILCH)	\
		aston(mycpu);					\
	splon(s); }						\
MACRO_END

#define ast_off(mycpu, reasons)					\
MACRO_BEGIN							\
	{ int s = sploff();					\
	if ((need_ast[mycpu] &= ~(reasons)) == AST_ZILCH)	\
		astoff(mycpu);					\
	splon(s); }						\
MACRO_END

#define ast_propagate(thread, mycpu)	ast_on((mycpu), (thread)->ast)

#define ast_context(thread, mycpu)					\
MACRO_BEGIN								\
	{ int s = sploff();					\
	if ((need_ast[mycpu] = 						\
                (need_ast[mycpu] & ~AST_PER_THREAD) | (thread)->ast)	\
              != AST_ZILCH) {						\
            aston(mycpu);						\
        } else {							\
            astoff(mycpu);						\
	}								\
	splon(s); }						\
MACRO_END

#define	thread_ast_set(thread, reason)		(thread)->ast |= (reason)
#define thread_ast_clear(thread, reason)	(thread)->ast &= ~(reason)
#define thread_ast_clear_all(thread)		(thread)->ast = AST_ZILCH

/*
 *	NOTE: if thread is the current thread, thread_ast_set should
 *	be followed by ast_propagate().
 */

#endif	_KERN_AST_H_
