/*
 * $Copyright
 * Copyright 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 * $Id: baton.h,v 1.7 1995/04/04 20:55:38 lenb Exp $
 */

#ifndef	_BATON_H_
#define	_BATON_H_ 1

#include <cpus.h>
#include <mach_assert.h>

#if	ASMP

#define	BATON_FREE	NCPUS	/* an invalid cpu_number() */
#define	BATON_DEBUG	1	/* set me to MACH_ASSERT */

#define	BATON_CPU	0	/* kernel_baton.cpu */
#define	BATON_LEVEL	4	/* kernel_baton.level */

#ifndef	ASSEMBLER
/*
 * The Kernel Baton implements the ASMP microkernel.
 *
 * "cpu" records which CPU owns the baton, else BATON_FREE.
 * "level" records current owner's recursion level.
 */

struct baton
{
	int	cpu;
	int	level;
};
extern struct baton	kernel_baton;
extern void		baton_enter();
extern void		baton_exit();
extern void		db_show_baton();
extern void		baton_assert_owner();
extern void		baton_assert_not_owner();
#endif	/* !ASSEMBLER */

#if	MACH_ASSERT || BATON_DEBUG
#define	ASSERT_BATON_OWNER()		baton_assert_owner()
#define	ASSERT_BATON_NOT_OWNER()	baton_assert_not_owner()
#else	/* !MACH_ASSERT || BATON_DEBUG */
#define	ASSERT_BATON_OWNER()
#define	ASSERT_BATON_NOT_OWNER()
#endif	/* !MACH_ASSERT || BATON_DEBUG */

#else	/* !ASMP */

#define	baton_enter()
#define	baton_exit()
#define	ASSERT_BATON_OWNER()
#define	ASSERT_BATON_NOT_OWNER()
#endif	/* !ASMP */

#endif	/* _BATON_H_ */
