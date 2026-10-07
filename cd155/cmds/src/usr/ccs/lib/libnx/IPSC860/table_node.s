// 
// $Copyright
// Copyright 1993, 1994, 1995  Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
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
 * $Log: table_node.s,v $
// Revision 1.3  1994/11/19  02:33:33  mtm
// Copyright additions/changes
//
// Revision 1.2  1993/04/28  22:45:02  shala
// Support stack frame.
//
// Revision 1.1  1993/02/23  15:44:35  shala
// A new call for TNC.
//
* Revision 1.3  1992/11/30  23:04:07  dleslie
* Copy of NX branch back into main trunk
*
* Revision 1.2.2.1  1992/11/10  17:19:22  cfj
* Put into NX branch.
*
 * Revision 1.2  1992/11/06  20:36:58  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1  1992/11/05  22:50:10  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.0  92/10/28  14:40:38  roman
 * First checkin of file with TNC-specific table_node() system call.
 *
 * Revision 3.1  92/04/23  17:16:40  klh
 * 	Initial check-in of Intel provided code.
 * 	Fix RCS comment.
 * 
 * Revision 1.1  92/04/22  14:42:17  cfj
 * Initial revision
 * 
 * $EndLog$
 */
        .file   "table_node.s"

#include "SYS.h"
//
// table_node(node)
//	int	 node;
//

RSYSCALL(table_node)
