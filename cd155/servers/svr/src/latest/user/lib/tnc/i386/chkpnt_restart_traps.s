// 
// $Copyright
// Copyright 1993, 1994 , 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/* 
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: chkpnt_restart_traps.s,v $
// Revision 1.3  1995/02/02  00:02:20  bolsen
//  Reviewer(s): Jerry Toman
//  Risk: Medium (lots of files)
//  Module(s): Too many to list
//  Configurations built: STD, LITE, & RAMDISK
//
//  Added or Updated the Locus Copyright message.
//
// Revision 1.2  1994/11/18  20:56:23  mtm
// Copyright additions/changes
//
// Revision 1.1  1994/03/14  17:48:48  slk
// Checkpoint Restart Code Drop
//  Reviewer: Chris Peak, chrisp@locus.com
//  Risk: Low
//  Benefit or PTS #: Enhancement
//  Testing: Locus VSTNC, individual checkpoint restart by hand
//  Module(s):
//
 * Revision 2.2  93/11/10  12:10:04  slk
 * 	Merge into main tree.
 * 
 * Revision 2.1.1.2  93/06/22  12:17:37  chrisp
 * 	Fix-up truncated revision history comments.
 * 
 * Revision 2.1.1.1  93/06/10  11:54:55  chrisp
 * 	Revision 3.1  93/05/25  09:45:51  chrisp
 * 	Eliminate chkpnt_async() trap.
 * 
 * 	Revision 3.0  93/04/17  13:07:00  chrisp
 * 	This module contains all trap routines for checkpoint/restart system
 * 	calls.
 * 
 * 	$EndLog$
 * 
 */
#include <sys/syscall.h>
#include <i386/asm.h>

#define	SYSCALL(name) 			\
	ENTRY(name);			\
	movl	$SYS_/**/name,%eax;	\
	SVC;				\
	jc	_cerror

#define	END_SYSCALL	ret
	
	SYSCALL(forkfamily)
	orl	%edx,%edx	/* if edx 0, eax = child pid & we are parent */
	je	forkfamily1
	xorl	%eax,%eax	/* else we are child and WE zero eax for ret */
forkfamily1:
	END_SYSCALL

	SYSCALL(on_checkpoint)
	END_SYSCALL

	SYSCALL(on_restart)
	END_SYSCALL

	SYSCALL(exec_restart)
	END_SYSCALL
