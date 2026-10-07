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
// Revision 1.3  1995/02/02  00:09:48  bolsen
//  Reviewer(s): Jerry Toman
//  Risk: Medium (lots of files)
//  Module(s): Too many to list
//  Configurations built: STD, LITE, & RAMDISK
//
//  Added or Updated the Locus Copyright message.
//
// Revision 1.2  1994/11/18  20:56:41  mtm
// Copyright additions/changes
//
// Revision 1.1  1994/03/14  17:49:19  slk
// Checkpoint Restart Code Drop
//  Reviewer: Chris Peak, chrisp@locus.com
//  Risk: Low
//  Benefit or PTS #: Enhancement
//  Testing: Locus VSTNC, individual checkpoint restart by hand
//  Module(s):
//
 * Revision 2.2  93/11/10  12:10:13  slk
 * 	Merge into main tree.
 * 
 * Revision 2.1.1.2  93/06/22  12:18:07  chrisp
 * 	Fix-up truncated revision history comments.
 * 
 * Revision 2.1.1.1  93/06/10  11:55:03  chrisp
 * 	Revision 3.1  93/05/25  09:46:16  chrisp
 * 	Eliminate chkpnt_async() trap.
 * 
 * 	Revision 3.0  93/05/17  09:30:34  chrisp
 * 	First cut.
 * 
 * 	$EndLog$
 * 
 */
#include "SYS.h"

//
// forkfamily(
//	int	*nproc,
//	pid_t	pid_list[])
//
SYSCALL(forkfamily)
        bte     r0,r17,.parent
        mov     r0,r16
.parent:
        SRET

//
// on_checkpoint(
//	caddr_t	user_routine)
//
SYSCALL(on_checkpoint)
	SRET

//
// on_restart(
//	caddr_t	user_routine)
//
SYSCALL(on_restart)
	SRET

//
// exec_restart(
//	char	*chkpnt_prefix,
//	int	options)
//
SYSCALL(exec_restart)
	SRET
