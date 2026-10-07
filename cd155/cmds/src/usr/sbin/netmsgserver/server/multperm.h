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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * multperm.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/multperm.h,v 1.2 1994/11/19 03:11:28 mtm Exp $
 *
 */

/*
 * External definitions for multperm.c
 */


#ifndef	_MULTPERM_
#define	_MULTPERM_


typedef struct mp_block {
    unsigned int	mp_block_high;
    unsigned int	mp_block_low;
} mp_block_t, *mp_block_ptr_t;


extern unsigned int mult();
/*
unsigned int	a,b;
*/

extern void permute();
/*
unsigned int	*out, *out1, in0, in1;
*/


extern void multcrypt();
/*
key_t		key;
mp_block_ptr_t	text;
*/

extern void multdecrypt();
/*
key_t		key;
mp_block_ptr_t	text;
*/

extern unsigned int minverse();
/*
unsigned int	test_key;
*/

extern void invert_key();
/*
key_t		*key_ptr;
*/

#endif	_MULTPERM_
