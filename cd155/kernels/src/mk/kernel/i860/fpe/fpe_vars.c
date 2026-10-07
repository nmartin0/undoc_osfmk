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
 * $Id: fpe_vars.c,v 1.4 1994/11/18 20:40:31 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_vars.c,v $
 * Revision 1.4  1994/11/18  20:40:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:29  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_vars.c     1.0"

#include "sys/types.h"
#include "fpe.h"

/*
 * global variables
 */
 
dlong_t SingleDefaultResult = {0xffc00000,0};
dlong_t DoubleDefaultResult = {0,0xfff80000};
dlong_t SinglePosInfinity =   {0x7f800000,0};
dlong_t DoublePosInfinity =   {0,0x7ff00000};
dlong_t SingleNegInfinity =   {0xff800000,0};
dlong_t DoubleNegInfinity =   {0,0xfff00000};
dlong_t SinglePosBiggest =    {0x7f7fffff,0};
dlong_t DoublePosBiggest =    {0xffffffff,0x7fefffff};
dlong_t SingleNegBiggest =    {0xff7fffff,0};
dlong_t DoubleNegBiggest =    {0xffffffff,0xffefffff};
dlong_t SinglePosSmallest =   {0x00800000,0x00000000};

/* Some external definitions */
ulong	SPindefinite[2] = 	{0xffc00000, 0x00000000};
ulong	SPinf[2] =		{0x7f800000, 0x00000000};
ulong	SPlargest[2] =		{0x7f7fffff, 0x00000000};
ulong	SPpow34[2] =		{0x50800000, 0x00000000};
ulong	SPpow32[2] =		{0x4f800000, 0x00000000};
ulong	SPpow4[2] =		{0x41800000, 0x00000000};
ulong	SPpow2[2] =		{0x40800000, 0x00000000};
ulong	SPzero[2] = 		{0x00000000, 0x00000000};
ulong	SPpow_32[2] =		{0x2f800000, 0x00000000};
ulong	SPsmallest[2] =		{0x00800000, 0x00000000};

ulong	DPindefinite[2] = 	{0x00000000, 0xfff80000};
ulong	DPinf[2] =		{0x00000000, 0x7ff00000};
ulong	DPlargest[2] =		{0xffffffff, 0x7fefffff};
ulong	DPpow66[2] =		{0x00000000, 0x44100000};
ulong	DPpow64[2] =		{0x00000000, 0x43f00000};
ulong	DPpow4[2] =		{0x00000000, 0x40300000};
ulong	DPpow2[2] =		{0x00000000, 0x40100000};
ulong	DPzero[2] = 		{0x00000000, 0x00000000};
ulong	DPpow_64[2] =		{0x00000000, 0x3bf00000};
ulong	DPsmallest[2] = 	{0x00000000, 0x00100000};

ulong	INzero[2] = 		{0x00000000, 0x3ff00000};
ulong	INindefinite[2] = 	{0x80000000, 0x3ff00000};
