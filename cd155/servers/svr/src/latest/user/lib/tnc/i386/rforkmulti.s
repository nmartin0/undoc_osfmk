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
 * $Log: rforkmulti.s,v $
// Revision 1.7  1995/02/02  00:07:45  bolsen
//  Reviewer(s): Jerry Toman
//  Risk: Medium (lots of files)
//  Module(s): Too many to list
//  Configurations built: STD, LITE, & RAMDISK
//
//  Added or Updated the Locus Copyright message.
//
// Revision 1.6  1994/11/18  20:56:33  mtm
// Copyright additions/changes
//
// Revision 1.5  1994/06/02  22:37:08  chrisp
// Trap code added for waitmulti().
//
//  Reviewer: cfj
//  Risk: M
//  Benefit or PTS #: 6463
//  Testing:
//  Modules: rforkmulti.s
//
// Revision 1.4  1993/07/14  18:52:36  cfj
// OSF/1 AD 1.0.4 code drop from Locus.
//
// Revision 1.3  1993/05/06  19:31:20  stefan
// ad103+tnc merged with Intel code.
//
// Revision 1.1.1.3  1993/07/01  21:19:49  cfj
// Adding new code from vendor
//
// Revision 1.2  1992/11/30  23:03:21  dleslie
// Copy of NX branch back into main trunk
//
// Revision 1.1.2.1  1992/11/05  22:49:08  dleslie
// Local changes for NX up to noon, November 5, 1992.
//
 * Revision 4.1  1992/11/04  01:04:59  cfj
 * Bump major revision number.
 *
// Revision 1.1.1.1  1993/05/03  17:58:38  cfj
// Initial 1.0.3 code drop
//
 * Revision 3.1  1992/04/20  22:03:06  klh
 * 	Fix RCS comments.
 *
 * Revision 3.0  92/04/16  21:03:20  yazz
 *	i386 specific
 * 
 * Revision 3.1  92/04/14  15:01:52  klh
 *	Fix RCS Log.
 * 
 * $EndLog$
 */
#include <sys/syscall.h>

	.globl	_rforkmulti
_rforkmulti:
	movl	$SYS_rforkmulti,%eax
/*	lcall	$0x7,$0 */
	.byte	0x9a,0x00,0x00,0x00,0x00,0x07,0x00
	jc	_cerror		/* eax has errno in it */
	orl	%edx,%edx	/* if edx 0, eax = ret status & we are parent */
	je	rforkm1
	xorl	%eax,%eax	/* else we are child and WE zero eax for ret */
rforkm1:
	ret

	.globl	_waitmulti
_waitmulti:
	movl	$SYS_waitmulti,%eax
/*	lcall	$0x7,$0 */
	.byte	0x9a,0x00,0x00,0x00,0x00,0x07,0x00
	jc	_cerror		/* eax has errno in it */
	ret
