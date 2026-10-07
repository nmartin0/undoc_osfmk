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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1993  Intel Corporation.
 *
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libc/IPSC860/joinrootpart.s,v 1.2 1994/11/19 02:12:13 mtm Exp $
 *
 *
 */

/*
 * join_root_part()
 *
 * Description: 	System call wrapper for join_root_part.
 *		
 * Parameters:
 *
 * Returns:
 */

        .file "join_root_part.s"

#include "SYS.h"

RSYSCALL(join_root_part)
