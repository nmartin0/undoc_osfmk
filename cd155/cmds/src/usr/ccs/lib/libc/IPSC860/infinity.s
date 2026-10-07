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

//      Copyright (c) 1992 Intel Corporation.
//      All rights reserved.
//
//         INTEL CORPORATION PROPRIETARY INFORMATION
//
//    This software is supplied under the terms of a license
//    agreement or nondisclosure agreement with Intel Corpo-
//    ration and may not be copied or disclosed except in
//    accordance with the terms of that agreement.
//

.file   "infinity.s"

.data

.align 16

__inf:
        .long   0x00000000,0x7ff00000           // Infinity 

.text

___infinity__::

.align 8

       orh     ha%__inf,r0,r16   // moved up from below
       fld.d   l%__inf(r16),f8

       bri     r1
          nop

