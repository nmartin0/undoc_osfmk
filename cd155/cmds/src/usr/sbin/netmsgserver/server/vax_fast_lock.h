/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * vax_fast_lock.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/vax_fast_lock.h,v 1.2 1994/11/19 03:12:31 mtm Exp $
 *
 */

/*
 * Fast inline locking routines for Vax.
 */


#ifndef _VAX_FAST_LOCK_
#define _VAX_FAST_LOCK_ 1

#ifndef	vax
	Sorry, this only works on a Vax.
#endif	vax

#undef spin_lock
#define spin_lock(p_crit) {					\
	extern	int	cthread_critical;			\
	register int not_done = 1;				\
	while (not_done) {					\
		asm("7:	bbssi	$0,_cthread_critical,7b");	\
		if (*(int *)(p_crit) == 0) {			\
			*(int *)(p_crit) = 1; not_done = 0; 	\
		asm("8:	bbcci	$0,_cthread_critical,8b");	\
		} else {					\
		asm("9:	bbcci	$0,_cthread_critical,9b"); }	\
	}							\
}

#undef spin_unlock
#define	spin_unlock(p_crit) {				\
	extern	int	cthread_critical;		\
	asm("7:	bbssi	$0,_cthread_critical,7b");	\
	*(int *)(p_crit) = 0;				\
	asm("8:	bbcci	$0,_cthread_critical,8b");	\
}

#undef mutex_lock
#define mutex_lock(m_crit) {					\
	extern	int	cthread_critical;			\
	extern 	int *	swtch_cnt_ptr; 				\
	register int 	not_done = 1;				\
	while (not_done) {					\
		asm("7:	bbssi	$0,_cthread_critical,7b");	\
		if (*(int *)(m_crit) == 0) {			\
			*(int *)(m_crit) = 1; not_done = 0; 	\
		asm("8:	bbcci	$0,_cthread_critical,8b");	\
		} else {					\
		asm("9:	bbcci	$0,_cthread_critical,9b"); }	\
		if (not_done) { (*swtch_cnt_ptr)++; swtch_pri(127); }	\
	}							\
}

#undef mutex_unlock
#define	mutex_unlock(m_crit) {				\
	extern	int	cthread_critical;		\
	asm("7:	bbssi	$0,_cthread_critical,7b");	\
	*(int *)(m_crit) = 0;				\
	asm("8:	bbcci	$0,_cthread_critical,8b");	\
}


#endif _VAX_FAST_LOCK_
