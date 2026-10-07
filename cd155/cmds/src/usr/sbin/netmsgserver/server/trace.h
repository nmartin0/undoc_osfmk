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
 * trace.h 
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/trace.h,v 1.2 1994/11/19 03:12:23 mtm Exp $
 */

/*
 * General purpose tracing macros - particularly useful when using cthreads.
 */


#ifndef _TRACE_
#define _TRACE_

#ifdef DEBUGOFF

#define BEGIN(_ROUTINE_NAME_)	{
#define END			}
#define RET			return
#define RETURN(_VALUE_)		return(_VALUE_)

#else DEBUGOFF

#include <stdio.h>

extern int	trace_recursion_level;
#ifndef	tracing_on
extern int	tracing_on;
#endif	tracing_on

#ifdef	LOCKTRACING

#ifndef	trace_lock
#include <pthread.h>
extern struct pthread_mutex	*trace_lock
#endif	trace_lock

#define BEGIN(_ROUTINE_NAME_)						\
{									\
	char *_routine_name = _ROUTINE_NAME_;				\
	trace_recursion_level++;					\
	if (tracing_on) {						\
		int i;							\
		pthread_mutex_lock(trace_lock);					\
	    	for (i=1; i<=trace_recursion_level; i++)		\
			fprintf(stdout, " ");				\
	        fprintf(stdout, "Enter %s\n", _routine_name);		\
		(void)fflush(stdout);					\
		pthread_mutex_unlock(trace_lock);				\
	}								\
	{


#define END								\
	}								\
}

#define RET								\
{									\
	if (tracing_on) {						\
		int i;							\
		pthread_mutex_lock(trace_lock);					\
		for (i=1; i<=trace_recursion_level; i++)		\
			fprintf(stdout, " ");				\
		fprintf(stdout, "Exit  %s\n", _routine_name);		\
		(void)fflush(stdout);					\
		pthread_mutex_unlock(trace_lock);				\
	}								\
	trace_recursion_level--;					\
	return;								\
}

#define RETURN(_value_)							\
{									\
	if (tracing_on) {						\
		int i;							\
		pthread_mutex_lock(trace_lock);					\
		for (i=1; i<=trace_recursion_level; i++)		\
			fprintf(stdout, " ");				\
		fprintf(stdout, "Exit  %s\n", _routine_name);		\
		(void)fflush(stdout);					\
		pthread_mutex_unlock(trace_lock);				\
	}								\
	trace_recursion_level--;					\
	return(_value_);						\
}

#else	LOCKTRACING

#define BEGIN(_ROUTINE_NAME_)						\
{									\
	char *_routine_name = _ROUTINE_NAME_;				\
	trace_recursion_level++;					\
	if (tracing_on) {						\
		int i;							\
	    	for (i=1; i<=trace_recursion_level; i++)		\
			fprintf(stdout, " ");				\
	        fprintf(stdout, "Enter %s\n", _routine_name);		\
		(void)fflush(stdout);					\
	}								\
	{


#define END								\
	}								\
}

#define RET								\
{									\
	if (tracing_on) {						\
		int i;							\
		for (i=1; i<=trace_recursion_level; i++)		\
			fprintf(stdout, " ");				\
		fprintf(stdout, "Exit  %s\n", _routine_name);		\
		(void)fflush(stdout);					\
	}								\
	trace_recursion_level--;					\
	return;								\
}

#define RETURN(_value_)							\
{									\
	if (tracing_on) {						\
		int i;							\
		for (i=1; i<=trace_recursion_level; i++)		\
			fprintf(stdout, " ");				\
		fprintf(stdout, "Exit  %s\n", _routine_name);		\
		(void)fflush(stdout);					\
	}								\
	trace_recursion_level--;					\
	return(_value_);						\
}

#endif	LOCKTRACING
#endif	DEBUGOFF


#endif	_TRACE_
