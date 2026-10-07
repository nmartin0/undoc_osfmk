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
 * crypt_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/crypt_defs.h,v 1.2 1994/11/19 03:10:35 mtm Exp $
 *
 */

/*
 * Internal definitions for crypt module.
 */


#ifndef	_CRYPT_DEFS_
#define	_CRYPT_DEFS_


/*
 * External definitions of functions.
 */

extern void encrypt_des();
/*
nmkey_t		key;
pointer_t	data_ptr;
int		data_size;
*/

extern void decrypt_des();
/*
nmkey_t		key;
pointer_t	data_ptr;
int		data_size;
*/


extern void encrypt_multperm();
/*
nmkey_t		key;
pointer_t	data_ptr;
int		data_size;
*/

extern void decrypt_multperm();
/*
nmkey_t		key;
pointer_t	data_ptr;
int		data_size;
*/


extern void encrypt_newdes();
/*
nmkey_t		key;
pointer_t	data_ptr;
int		data_size;
*/

extern void decrypt_newdes();
/*
nmkey_t		key;
pointer_t	data_ptr;
int		data_size;
*/


extern void encrypt_xor();
/*
nmkey_t		key;
pointer_t	data_ptr;
int		data_size;
*/

extern void decrypt_xor();
/*
nmkey_t		key;
pointer_t	data_ptr;
int		data_size;
*/

#endif	_CRYPT_DEFS_
