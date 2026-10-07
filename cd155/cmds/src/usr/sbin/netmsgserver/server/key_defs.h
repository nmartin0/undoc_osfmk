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
 * key_defs.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/key_defs.h,v 1.2 1994/11/19 03:11:15 mtm Exp $
 *
 */

/*
 * Definitions of encryption keys etc..
 */


#ifndef	_KEY_DEFS_
#define	_KEY_DEFS_

/*
 * An encrytion key.
 */
typedef union {
    unsigned char	key_bytes[16];
    unsigned long	key_longs[4];
} nmkey_t, *key_ptr_t;

#define KEY_EQUAL(key1, key2) 					\
    ((key1.key_longs[0] == key2.key_longs[0])			\
	&& (key1.key_longs[1] == key2.key_longs[1])		\
	&& (key1.key_longs[2] == key2.key_longs[2])		\
	&& (key1.key_longs[3] == key2.key_longs[3]))

#define KEY_IS_NULL(key)					\
    (((key).key_longs[0] == 0) && ((key).key_longs[1] == 0)	\
	&& ((key).key_longs[2] == 0) && ((key).key_longs[3] == 0))


/*
 * Macros to convert keys between network and host byte order.
 */
#define NTOH_KEY(key) {							\
    (key).key_longs[0] = ntohl((key).key_longs[0]);			\
    (key).key_longs[1] = ntohl((key).key_longs[1]);			\
    (key).key_longs[2] = ntohl((key).key_longs[2]);			\
    (key).key_longs[3] = ntohl((key).key_longs[3]);			\
}

#define HTON_KEY(key) {							\
    (key).key_longs[0] = htonl((key).key_longs[0]);			\
    (key).key_longs[1] = htonl((key).key_longs[1]);			\
    (key).key_longs[2] = htonl((key).key_longs[2]);			\
    (key).key_longs[3] = htonl((key).key_longs[3]);			\
}

/*
 * Structure used to transmit or store a token or a key.
 */
typedef union {
    nmkey_t	si_key;
    nmkey_t	si_token;
} secure_info_t, *secure_info_ptr_t;

/*
 * Security Level of ports and messages.
 */
#define PORT_NOT_SECURE		0
#define MESSAGE_NOT_SECURE	0

#endif	_KEY_DEFS_
