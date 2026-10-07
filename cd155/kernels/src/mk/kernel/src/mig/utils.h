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
 * $Log: utils.h,v $
 * Revision 1.6  1994/11/19  02:39:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/11/18  21:03:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:00:30  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:54:37  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:00:36  dleslie
 * First R1_0 release
 *
 * Revision 2.4  91/06/25  10:32:47  rpd
 * 	Changed WriteVarDecl to WriteUserVarDecl.
 * 	Added WriteServerVarDecl.
 * 	[91/05/23            rpd]
 * 
 * Revision 2.3  91/02/05  17:56:33  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:56:48  mrt]
 * 
 * Revision 2.2  90/06/02  15:06:16  rpd
 * 	Created for new IPC.
 * 	[90/03/26  21:15:06  rpd]
 * 
 * 07-Apr-89  Richard Draves (rpd) at Carnegie-Mellon University
 *	Extensive revamping.  Added polymorphic arguments.
 *	Allow multiple variable-sized inline arguments in messages.
 *
 * 28-May-87  Richard Draves (rpd) at Carnegie-Mellon University
 *	Created.
 */

#ifndef	_UTILS_H
#define	_UTILS_H

/* stuff used by more than one of header.c, user.c, server.c */

extern void WriteImport(/* FILE *file, string_t filename */);
extern void WriteRCSDecl(/* FILE *file, identifier_t name, string_t rcs */);
extern void WriteBogusDefines(/* FILE *file */);

extern void WriteList(/* FILE *file, argument_t *args,
			 void (*func)(FILE *file, argument_t *arg),
			 u_int mask, char *between, char *after */);

extern void WriteReverseList(/* FILE *file, argument_t *args,
				void (*func)(FILE *file, argument_t *arg),
				u_int mask, char *between, char *after */);

/* good as arguments to WriteList */
extern void WriteNameDecl(/* FILE *file, argument_t *arg */);
extern void WriteUserVarDecl(/* FILE *file, argument_t *arg */);
extern void WriteServerVarDecl(/* FILE *file, argument_t *arg */);
extern void WriteTypeDeclIn(/* FILE *file, argument_t *arg */);
extern void WriteTypeDeclOut(/* FILE *file, argument_t *arg */);
extern void WriteCheckDecl(/* FILE *file, argument_t *arg */);

extern char *ReturnTypeStr(/* routine_t *rt */);

extern char *FetchUserType(/* ipc_type_t *it */);
extern char *FetchServerType(/* ipc_type_t *it */);
extern void WriteFieldDeclPrim(/* FILE *file, argument_t *arg,
				  char *(*tfunc)(ipc_type_t *it) */);

extern void WriteStructDecl(/* FILE *file, argument_t *args,
			       void (*func)(FILE *file, argument_t *arg),
			       u_int mask, char *name */);

extern void WriteStaticDecl(/* FILE *file, ipc_type_t *it,
			       boolean_t dealloc, boolean_t longform,
			       boolean_t inname, identifier_t name */);

extern void WriteCopyType(/* FILE *file, ipc_type_t *it,
			     char *left, char *right, ... */);

extern void WritePackMsgType(/* FILE *file, ipc_type_t *it,
				boolean_t dealloc, boolean_t longform,
				boolean_t inname, char *left, char *right,
				... */);

#endif	_UTILS_H
