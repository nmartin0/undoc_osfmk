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
 * netmsg.h 
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/netmsg.h,v 1.2 1994/11/19 03:11:31 mtm Exp $
 */

/*
 * This file contains definitions of macros that will be used throughout the
 * network server.  It should be included by every *.c file.  Macros defined
 * here include a standard set of debugging macros, and others. 
 */


#ifndef _NETMSG_
#define _NETMSG_

#include "config.h"
#include "debug.h"

/*
 * Function argument order is IN, INOUT, OUT, REF. These psuedo-keywords are
 * used to label the arguments. For example: 
 *
 * Char FunReturningChar(   IN arg1, arg2, INOUT arg3, OUT arg4, arg5, REF arg6) 
 *
 * IN    indicates arg value is read, and not written. All non-pointer arguments
 * will be IN. 
 *
 * INOUT indicates arg value may be both read and written. 
 *
 * OUT   indicates arg value is only written. INOUT and OUT apply to values
 * referenced by pointers. 
 *
 * REF   indicates arg is a pointer to shared data.  Data may be read or
 * written, but may not meaningfully be copied. 
 *
 * Note: 
 *
 * Functions for which all arguments are IN need not use any of these keywords. 
 *
 */

#define IN
#define INOUT
#define OUT
#define REF

#define EXPORT
#define PUBLIC

#include "debug.h"

#if	NET_TRACE

#define LOCKTRACING	1
#define PRIVATE
#define tracing_on	debug.tracing
#define trace_lock	log_lock
#include "trace.h"

#else	NET_TRACE

#define DEBUGOFF	1
#define LOCKTRACING	1
#define tracing_on	debug.tracing
#define trace_lock	log_lock
#define PRIVATE static
#include "trace.h"

#endif	NET_TRACE

#endif _NETMSG_
