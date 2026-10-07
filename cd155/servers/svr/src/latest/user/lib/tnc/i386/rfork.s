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
 * $Log: rfork.s,v $
// Revision 1.6  1995/02/02  00:07:01  bolsen
//  Reviewer(s): Jerry Toman
//  Risk: Medium (lots of files)
//  Module(s): Too many to list
//  Configurations built: STD, LITE, & RAMDISK
//
//  Added or Updated the Locus Copyright message.
//
// Revision 1.5  1994/11/18  20:56:31  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/07/14  18:52:34  cfj
// OSF/1 AD 1.0.4 code drop from Locus.
//
// Revision 1.3  1993/05/06  19:31:17  stefan
// ad103+tnc merged with Intel code.
//
// Revision 1.1.1.3  1993/07/01  21:19:44  cfj
// Adding new code from vendor
//
// Revision 1.2  1992/11/30  23:03:17  dleslie
// Copy of NX branch back into main trunk
//
// Revision 1.1.2.1  1992/11/05  22:49:06  dleslie
// Local changes for NX up to noon, November 5, 1992.
//
 * Revision 4.1  1992/11/04  01:04:56  cfj
 * Bump major revision number.
 *
// Revision 1.1.1.1  1993/05/03  17:58:37  cfj
// Initial 1.0.3 code drop
//
 * Revision 3.1  1992/04/20  21:35:24  klh
 * 	Fix RCS comments
 *
 * Revision 3.0  92/04/16  21:03:13  yazz
 *	i386 specific
 * 
 * Revision 3.1  92/04/14  15:01:27  klh
 *	Fix RCS Log.
 * 
 * $EndLog$
 */
#include <sys/syscall.h>

	.globl	_rfork
_rfork:
	movl	$SYS_rfork,%eax
/*	lcall	$0x7,$0 */
	.byte	0x9a,0x00,0x00,0x00,0x00,0x07,0x00
	jc	_cerror		/* eax has errno in it */
	orl	%edx,%edx	/* if edx 0, eax = child pid & we are parent */
	je	rfork1
	xorl	%eax,%eax	/* else we are child and WE zero eax for ret */
rfork1:
	ret
