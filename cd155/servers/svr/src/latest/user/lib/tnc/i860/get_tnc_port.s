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
 *      Copyright (c) 1992, Intel Corporation.
 *      All rights reserved.
 *
 *         INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *    This software is supplied under the terms of a license
 *    agreement or nondisclosure agreement with Intel Corpo-
 *    ration and may not be copied or disclosed except in
 *    accordance with the terms of that agreement.
 */
/*
 * HISTORY
 * $Log: get_tnc_port.s,v $
// Revision 1.7  1995/02/02  00:10:30  bolsen
//  Reviewer(s): Jerry Toman
//  Risk: Medium (lots of files)
//  Module(s): Too many to list
//  Configurations built: STD, LITE, & RAMDISK
//
//  Added or Updated the Locus Copyright message.
//
// Revision 1.6  1994/11/18  20:56:41  mtm
// Copyright additions/changes
//
// Revision 1.5  1994/01/17  21:17:33  slk
// Fix for #7797, programs that return from main() core dump.
//
//  Reviewer: Chris Peak and Charlie Johnson
//  Risk: Low.
//  Benefit or PTS #: #7797, allows Locus test suites to complete properly.
//  Testing: Ran Locus test suites
//  Module(s): SYS.h
//
// Revision 1.4  1993/07/14  18:52:58  cfj
// OSF/1 AD 1.0.4 code drop from Locus.
//
// Revision 1.3  1993/05/06  19:31:52  stefan
// ad103+tnc merged with Intel code.
//
// Revision 1.1.1.3  1993/07/01  21:20:30  cfj
// Adding new code from vendor
//
// Revision 1.2  1992/11/30  23:03:42  dleslie
// Copy of NX branch back into main trunk
//
// Revision 1.1.2.1  1992/11/05  22:49:54  dleslie
// Local changes for NX through noon, November 5, 1992.
//
 * Revision 4.1  1992/11/04  01:05:15  cfj
 * Bump major revision number.
 *
// Revision 1.1.1.1  1993/05/03  17:58:53  cfj
// Initial 1.0.3 code drop
//
 * Revision 3.1  1992/04/23  17:25:32  klh
 * 	Initial check-in of Intel provided code.
 * 	Fix RCS comment.
 *
 * Revision 1.1  92/04/22  14:41:43  cfj
 * Initial revision
 * 
 * $EndLog$
 */
        .file   "get_tnc_port.s"

#include "SYS.h"

//
// get_tnc_port(load_level_id, node_number, tnc_port)
//	int		load_level_id
//	int		node_number
//	mach_port_t	*tnc_port
//

SYSCALL(get_tnc_port)
	SRET




