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
 * crypt.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/crypt.h,v 1.2 1994/11/19 03:10:34 mtm Exp $
 *
 */

/*
 * External definitions for network server link-level encryption.
 */


#ifndef	_CRYPT_
#define	_CRYPT_

/*
 * Security heade for packets.
 */
typedef struct crypt_header {
    unsigned long	ch_crypt_level;	/* This must not be encrypted. */
    unsigned short	ch_checksum;	/* Everything hereon can be encrypted. */
    unsigned short	ch_data_size;
} crypt_header_t, *crypt_header_ptr_t;

#define CRYPT_HEADER_SIZE	(sizeof(short) + sizeof(short) + sizeof(long))

/*
 * Encryption return codes.
 */
#define CRYPT_SUCCESS			0
#define CRYPT_FAILURE			-1
#define CRYPT_REMOTE_FAILURE		-2
#define CRYPT_CHECKSUM_FAILURE		-3

/*
 * Encryption algorithms.
 */
#define CRYPT_NULL	0
#define	CRYPT_XOR	1
#define	CRYPT_NEWDES	2
#define	CRYPT_MULTPERM	3
#define CRYPT_DES	4
#define CRYPT_MAX	4

typedef struct {
    int		(*encrypt)();
    int		(*decrypt)();
} crypt_function_t;


#define CHECK_ENCRYPT_ALGORITHM(algorithm)				\
	(((algorithm) >= CRYPT_NULL) && ((algorithm) <= CRYPT_MAX)	\
		&& (crypt_functions[(algorithm)].encrypt))

#define CHECK_DECRYPT_ALGORITHM(algorithm)				\
	(((algorithm) >= CRYPT_NULL) && ((algorithm) <= CRYPT_MAX)	\
		&& (crypt_functions[(algorithm)].decrypt))




#ifdef _NOCRYPT


#define encrypt_xor		crypt_null
#define decrypt_xor		crypt_null

#define encrypt_newdes		crypt_null
#define decrypt_newdes		crypt_null

#define encrypt_multperm	crypt_null
#define decrypt_multperm	crypt_null

#define encrypt_des		crypt_null
#define decrypt_des		crypt_null

#define crypt_encrypt_packet	crypt_null
#define crypt_decrypt_packet	crypt_null

#define CRYPT_DONT_ENCRYPT		1
#define CRYPT_ENCRYPT			0
/*
crypt_function_t crypt_functions[] = {
		{crypt_null, crypt_null},
		{crypt_null, crypt_null},
		{crypt_null, crypt_null},
		{crypt_null, crypt_null},
		{crypt_null, crypt_null},
		};
*/
#else /* _NOCRYPT */
/*
 * Encryption levels for data sent over the network.
 */
#define CRYPT_DONT_ENCRYPT		0
#define CRYPT_ENCRYPT			1

#endif /* _NOCRYPT */

extern crypt_function_t		crypt_functions[];

#endif	_CRYPT_
