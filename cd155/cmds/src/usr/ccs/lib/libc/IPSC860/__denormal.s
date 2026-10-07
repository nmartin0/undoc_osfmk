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
 
//
//              INTEL CORPORATION PROPRIETARY INFORMATION
//
//  This software is supplied under the terms of a license
//  agreement or nondisclosure agreement with Intel Corporation
//  and may not be copied or disclosed except in accordance
//  with the terms of that agreement.
//
//
//      Copyright 1992  Intel Corporation.
//
//
//
        .file "__denormal.s"

        .text

//      For links using the -Knoieee switch, this file is
//      explicitly linked in.  It's function is to force
//      denormals to be forced to zero on creation.

___denormal::
        ld.c    fsr,r31
        or      0x1,r31,r31
        bri     r1
        st.c    r31,fsr

