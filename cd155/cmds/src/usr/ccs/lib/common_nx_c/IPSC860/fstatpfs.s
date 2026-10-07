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
 *
 * HISTORY
 * $Log: fstatpfs.s,v $
// Revision 1.2  1994/11/19  01:56:43  mtm
// Copyright additions/changes
//
// Revision 1.1  1993/09/28  01:17:52  brad
// Initial revision
//
// Revision 1.2  1993/04/28  22:44:48  shala
// Support stack frame.
//
// Revision 1.1  1993/01/12  00:42:14  brad
// Initial revision
//
 *
 */

/*
 * fstatpfs
 *
 * Description: 	System call wrapper for fstatpfs.
 *		
 * Parameters:
 *
 * Returns:
 */

#include "SYS.h"

RSYSCALL(fstatpfs)
