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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libc/IPSC860/fabs.c,v 1.2 1994/11/19 02:10:57 mtm Exp $
 *
 * 
 */

typedef  union {
	struct {
		unsigned  lo	: 32 ;
		unsigned  hi	: 20 ;
		unsigned  exp	: 11 ;
		unsigned  sign	: 1 ;
	} fparts ;
	struct {
		unsigned  lo	: 32 ;
		unsigned  hi	: 19 ;
		unsigned  qnan_bit	: 1 ;
		unsigned  exp	: 11 ;
		unsigned  sign	: 1 ;
	} nparts ;
	struct {
		unsigned  lo	: 32 ;
		unsigned  hi	: 32 ;
	} fwords ;
	double	d ;
} _dval ;
 
typedef  union {
	struct {
		unsigned fract	: 23 ;
		unsigned exp	: 8 ;
		unsigned sign	: 1 ;
	} fparts ;
	struct {
		unsigned fract	: 22 ;
		unsigned qnan_bit	: 1 ;
		unsigned exp	: 8 ;
		unsigned sign	: 1 ;
	} nparts ;
	unsigned long	fword ;
	float	f ;
} _fval ;
 
double
fabs ( double x )
{
	( ( ( _dval * ) & ( x ) ) -> fparts . sign ) = 0 ;
	return x ;
}
 
