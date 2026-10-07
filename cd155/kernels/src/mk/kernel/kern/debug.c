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

/* $Id: debug.c,v 1.12 1995/03/28 03:01:13 stans Exp $ */

#include "mach_kdb.h"
#include "norma_vm.h"
#include "cpus.h"

#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <sys/varargs.h>
#include <kern/thread.h>

extern void Debugger();

extern int cnputc();

int panic_on_assertion_failure = 1;

#if	MACH_KDB
extern int db_breakpoints_inserted;
#endif	MACH_KDB

void Assert(file, line)
	char *file;
	int line;
{
	printf("Assertion failed: file \"%s\", line %d\n", file, line);

	if (panic_on_assertion_failure)
#if	MACH_KDB
		if (db_breakpoints_inserted)
#endif	MACH_KDB
			Debugger("assertion failure");
}

void Debugger(message)
	char *	message;
{
#ifdef	lint
	message++;
#endif	lint

#ifdef	vax
	asm("bpt");
#endif	vax
#ifdef	sun
	current_thread()->pcb->flag |= TRACE_KDB;
	asm("orw  #0x00008000,sr");
#endif	sun
#ifdef	mips 
	gimmeabreak();
#endif	mips
#ifdef  luna88k
	gimmeabreak();
#endif
#ifdef	i386
	asm("int3");
#endif	i386
#ifdef	i860
	gimmeabreak();
#endif	i860
}

char			*panicstr;
decl_simple_lock_data(,	panic_lock)
int			paniccpu;

panic_init()
{
	simple_lock_init(&panic_lock);
}

/*VARARGS1*/
panic(va_alist)
	va_dcl
{
	va_list		listp;
	char		*s;
	int		ipl = splhigh();/* do not let other CPUs interrupt */
#if	NCPUS > 1
	extern int	printf_cpu_number_prefix;
#endif

#if	NORMA_VM
	extern int _node_self;	/* node_self() may not be callable yet */
#endif	NORMA_VM

	SIMPLE_LOCK(&panic_lock);
	if (panicstr) {
	    if (cpu_number() != paniccpu) {
		SIMPLE_UNLOCK(&panic_lock);
		halt_cpu();
		/* NOTREACHED */
	    }
	}
	else {
	    panicstr = s;
	    paniccpu = cpu_number();
	}
	SIMPLE_UNLOCK(&panic_lock);
	printf("panic");
#if	NCPUS > 1
	printf_cpu_number_prefix = FALSE;
#endif
#if	NORMA_VM
	printf(" (node %d)", _node_self);
#endif
	printf(": ");
	va_start(listp);
	s = va_arg(listp, char *);
	_doprnt(s, &listp, cnputc, 0);
	va_end(listp);
	printf("\n");
#if	NCPUS > 1
	printf_cpu_number_prefix = TRUE;
#endif

#if	MACH_KDB
	Debugger("panic");
#else	MACH_KDB
	halt_cpu();
#endif	MACH_KDB
	splx(ipl);
}

/*
 * We'd like to use BSD's log routines here...
 */
/*VARARGS2*/
log(va_alist)
	va_dcl
{
	va_list	listp;
	int	level;
	char	*fmt;

#ifdef lint
	level++;
#endif lint
	va_start(listp);
	level = va_arg(listp, int);
	fmt = va_arg(listp, char *);
	_doprnt(fmt, &listp, cnputc, 0);
	va_end(listp);
}


void abort()
{
	panic("abort");
	/*NOTREACHED*/
}
