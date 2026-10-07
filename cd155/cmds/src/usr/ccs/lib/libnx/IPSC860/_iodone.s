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
 *      Copyright 1992  Intel Corporation.
 *
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/IPSC860/_iodone.s,v 1.4 1994/11/19 02:32:57 mtm Exp $
 *
 * Description of the routine
 *
 *
 */

/*
 * _iodone
 *
 * Description: 	System call wrapper for _iodone.
 *		
 * Parameters:
 *
 * Returns:
 */

        .file "_iodone.s"

#include "SYS.h"

RSYSCALL(_iodone)
