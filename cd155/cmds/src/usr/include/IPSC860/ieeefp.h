/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*	Copyright (c) 1992 Intel Corporation.
 *	All rights reserved.
 *
 *         INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *    This software is supplied under the terms of a license
 *    agreement or nondisclosure agreement with Intel Corpo-
 *    ration and may not be copied or disclosed except in
 *    accordance with the terms of that agreement.
 */

#ifndef __IEEEFP_H__
#define __IEEEFP_H__
typedef enum {  FP_RN = 0x0, /* round nearest (default) */
		FP_RM = 0x1, /* round to minus infinity */
		FP_RP = 0x2, /* round to plus infinity  */
		FP_RZ = 0x3 /* round to zero           */
		} fp_rnd;

typedef enum {    FP_X_INV= 0x01,  /* invalid operation */
		  FP_X_DZ=  0x02,  /* divide by zero    */
		  FP_X_OFL= 0x04,  /* overflow          */
		  FP_X_UFL= 0x08,  /* underflow         */
		  FP_X_IMP= 0x10   /* inexact           */
		  } fp_except;

extern fp_rnd fpgetround(void);
extern fp_rnd fpsetround(fp_rnd);

extern fp_except fpgetmask(void);
extern fp_except fpsetmask(fp_except);
extern fp_except fpgetsticky(void);
extern fp_except fpsetsticky(fp_except);

extern int isnan(double);
extern int isnand(double);
extern int isnanf(float);

#endif /* __IEEEFP_H__ */
