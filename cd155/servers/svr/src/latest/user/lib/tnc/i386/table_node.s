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
 * $Log: table_node.s,v $
// Revision 1.7  1995/02/02  00:09:06  bolsen
//  Reviewer(s): Jerry Toman
//  Risk: Medium (lots of files)
//  Module(s): Too many to list
//  Configurations built: STD, LITE, & RAMDISK
//
//  Added or Updated the Locus Copyright message.
//
// Revision 1.6  1994/11/18  20:56:36  mtm
// Copyright additions/changes
//
// Revision 1.5  1993/07/14  18:52:47  cfj
// OSF/1 AD 1.0.4 code drop from Locus.
//
// Revision 1.4  1993/05/06  19:31:30  stefan
// ad103+tnc merged with Intel code.
//
// Revision 1.1.1.4  1993/07/01  21:20:03  cfj
// Adding new code from vendor
//
// Revision 1.3  1992/12/11  03:09:10  cfj
// Merged 12-1-92 bug drop from Locus.
//
 * Revision 1.2  1992/11/06  20:36:47  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1  1992/11/05  22:49:14  dleslie
 * Local changes for NX up to noon, November 5, 1992.
 *
// Revision 1.1.1.2  1993/05/03  17:58:46  cfj
// Initial 1.0.3 code drop
//
 * Revision 3.0  92/10/28  14:40:06  roman
 * First checkin of file with TNC-specific table_node() system call.
 *
 * Revision 3.1  92/04/20  21:24:24  klh
 * 	Fix RCS comment
 * 
 * Revision 3.0  92/04/16  21:02:37  yazz
 * 	i386 specific
 * 
 * Revision 3.2  92/04/15  10:38:58  chrisp
 * 	Correct typo: $_kill3 nows read $SYS_kill3.
 * 
 * Revision 3.1  92/04/14  14:57:44  klh
 * 	Fix RCS Log
 * 
 * $EndLog$
 */
#include <sys/syscall.h>

	.globl	_table_node
_table_node:
	movl	$SYS_table_node,%eax
/*	lcall	$0x7,$0 */
	.byte	0x9a,0x00,0x00,0x00,0x00,0x07,0x00
	jc	_cerror		/* eax has errno in it */
	ret
