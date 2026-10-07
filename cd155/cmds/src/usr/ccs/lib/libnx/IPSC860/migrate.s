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
 * $Log: migrate.s,v $
// Revision 3.3  1994/11/19  02:33:19  mtm
// Copyright additions/changes
//
// Revision 3.2  1993/04/28  22:44:52  shala
// Support stack frame.
//
Revision 3.1  1992/06/23  12:09:51  stans
*** empty log message ***

 * Revision 3.1  92/04/23  17:16:40  klh
 * 	Initial check-in of Intel provided code.
 * 	Fix RCS comment.
 * 
 * Revision 1.1  92/04/22  14:42:17  cfj
 * Initial revision
 * 
 * $EndLog$
 */
        .file   "migrate.s"

#include "SYS.h"
//
// migrate(node)
//	int	 node;
//

RSYSCALL(migrate)
