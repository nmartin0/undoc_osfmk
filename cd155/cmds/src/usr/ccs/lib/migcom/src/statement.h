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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: statement.h,v $
 * Revision 1.6  1994/11/19  02:39:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/11/18  21:02:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:00:15  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:54:17  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:00:10  dleslie
 * First R1_0 release
 *
 * Revision 2.3  91/02/05  17:55:47  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:55:51  mrt]
 * 
 * Revision 2.2  90/06/02  15:05:41  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:13:34  rpd]
 * 
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 * 27-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

#ifndef	_STATEMENT_H
#define	_STATEMENT_H

#include "routine.h"

typedef enum statement_kind
{
    skRoutine,
    skImport,
    skUImport,
    skSImport,
    skRCSDecl,
} statement_kind_t;

typedef struct statement
{
    statement_kind_t stKind;
    struct statement *stNext;
    union
    {
	/* when stKind == skRoutine */
	routine_t *_stRoutine;
	/* when stKind == skImport, skUImport, skSImport */
	string_t _stFileName;
    } data;
} statement_t;

#define	stRoutine	data._stRoutine
#define	stFileName	data._stFileName

#define stNULL		((statement_t *) 0)

/* stNext will be initialized to put the statement in the list */
extern statement_t *stAlloc();

/* list of statements, in order they occur in the .defs file */
extern statement_t *stats;

#endif	_STATEMENT_H
