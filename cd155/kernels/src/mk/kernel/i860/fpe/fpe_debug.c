/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*      Copyright (c) 1989,1990 Intel Corporation.         */
/*      All rights reserved.                               */
/*                                                         */
/*        INTEL CORPORATION PROPRIETARY INFORMATION        */
/*                                                         */
/* This software is supplied under the terms of a license  */
/* agreement or nondisclosure agreement with Intel Corp.   */
/* and may not be copied or disclosed except in accordance */
/* with the terms of that agreement.                       */

/*
 * $Id: fpe_debug.c,v 1.4 1994/11/18 20:40:20 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_debug.c,v $
 * Revision 1.4  1994/11/18  20:40:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:09  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_debug.c    1.0"

#include "sys/types.h"
#include "fpe.h"

#if DEBUG
int fpe_debug = 0;

print_i860type(i860_ptr)
fp_t *i860_ptr;
{
    printf("high32  %x  low32  %x\n",i860_ptr->dlong.hi,i860_ptr->dlong.lo);
}
#endif

#define NUM_OF_FPE_TBUF 	10
struct {
    ulong inst;
    ulong psr;
    ulong fsr;
} fpe_trace_buf[NUM_OF_FPE_TBUF];
int fpe_tbp = 0;
int fpe_bug_count = 0;
int bug_print = 0;

fpe_trace(fir,psr,fsr)
ulong *fir;
ulong fsr;
ulong psr;
{
    fpe_trace_buf[fpe_tbp].inst = *fir;
    fpe_trace_buf[fpe_tbp].psr = psr;
    fpe_trace_buf[fpe_tbp].fsr = fsr;
    fpe_bug_count++;
    if (++fpe_tbp == NUM_OF_FPE_TBUF)
        fpe_tbp = 0;
}
