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
 * ipc_swap.h 
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/ipc_swap.h,v 1.2 1994/11/19 03:11:14 mtm Exp $ 
 *
 */

/*
 * Definitions and macros for byte-swapping operations in the IPC module. 
 */


#ifndef	_IPC_SWAP_
#define	_IPC_SWAP_

#include	"config.h"

/*
 * TypeType seen in reverse for byte-swapping. 
 */
#if	VaxOrder
typedef struct {
	unsigned int
		msg_type_name:8,
		msg_type_size:8,
		msg_type_numhigh:8,
		msg_type_dummy:1,
		msg_type_deallocate:1,
		msg_type_longform:1,
		msg_type_inline:1,
		msg_type_numlow:4;
} swap_msg_type_t;
#else	VaxOrder
typedef struct {
	unsigned int
		msg_type_name:8,
		msg_type_size:8,
		msg_type_numlow:8,
		msg_type_dummy:1,
		msg_type_deallocate:1,
		msg_type_longform:1,
		msg_type_inline:1,
		msg_type_numhigh:4;
} swap_msg_type_t;
#endif	VaxOrder

/*
 * Declarations needed for the following macros to work. 
 */
#define	SWAP_DECLS	register unsigned long swap_tmp

/*
 * Swap and copy a long value, and return the swapped value. 
 */
#define	SWAP_LONG(f,t) (						\
	swap_tmp = f,							\
	t = ((swap_tmp >> 24) | ((swap_tmp >> 8) & 0xff00)		\
		| ((swap_tmp << 8) & 0xff0000) | (swap_tmp << 24))	\
)

/*
 * Swap and copy a short value, and return the swapped value. 
 */
#define	SWAP_SHORT(f,t) (						\
	swap_tmp = f,							\
	t = ((swap_tmp >> 8) | ((swap_tmp << 8) & 0xff00))		\
)

/*
 * Swap in place an array of longs. 
 */
#define	SWAP_LONG_ARRAY(s,n) {						\
	register long	*p = (long *)s;					\
									\
	for (; n; n--, p++) SWAP_LONG(*p,*p);				\
}

/*
 * Swap in place an array of shorts. 
 */
#define	SWAP_SHORT_ARRAY(s,n) {						\
	register short	*p = (short *)s;				\
									\
	for (; n; n--, p++) SWAP_SHORT(*p,*p);				\
}

/*
 * External definitions for functions implemented by ipc_swap.c
 */

extern void swap_long_sbuf();
/*
sbuf_ptr_t	sb_ptr;
sbuf_pos_t	*pos_ptr;
long		**to_ptr;
int		count;
*/

extern void swap_short_sbuf();
/*
sbuf_ptr_t	sb_ptr;
sbuf_pos_t	*pos_ptr;
short		**to_ptr;
int		count;
*/

#endif	_IPC_SWAP_
