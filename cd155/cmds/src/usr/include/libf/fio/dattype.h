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
 
/*
 *	Copyright 1991, The Portland Group, Incorporated.
 *	All rights reserved.
 *
 *	THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with The Portland Group and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */
/* dattype.h - definitions of symbols for data types. */

#define TY_WORD 1
#define TY_DWORD 2
#define TY_HOLL 3
#define TY_BINT 4
#define TY_SINT 5
#define TY_INT 6
#define TY_REAL 7
#define TY_DBLE 8
#define TY_QUAD 9
#define TY_CMPLX 10
#define TY_DCMPLX 11
#define TY_BLOG 12
#define TY_SLOG 13
#define TY_LOG 14
#define TY_CHAR 15
#define TY_NCHAR 16

#define Is_complex(parm) ((parm) == TY_CMPLX || (parm) == TY_DCMPLX)
