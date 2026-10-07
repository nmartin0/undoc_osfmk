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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libc/IPSC860/alloca.s,v 1.2 1994/11/19 02:10:27 mtm Exp $
 *
 */

        .file   "alloca.s"
        .globl _alloca
        .align 8
        .text
_alloca:
        adds 15,r16,r16 // round to 16 byte boundary
        andnot 15,r16,r16
        subs sp,r16,sp  // bump stack pointer
        bri r1
        mov sp,r16
