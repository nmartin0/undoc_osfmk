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
/* hash_string.c
 * Function to hash a string into a small integer
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#ifndef _NO_PROTO
#include <loader.h>
#endif

#include <ldr_main_types.h>

#include "ldr_types.h"
#include "ldr_hash.h"
#include "hash_rbytes.h"

unsigned
#if __STDC__
hash_string(const char *string, unsigned modulus)
#else
hash_string(string, modulus)
char *string;
unsigned modulus;
#endif

/* Hash a string into an unsigned integer that is modulus the
 * specified modulus value.
 */
{
	unsigned	hash;
	unsigned long	i, byte;

	i = 0;
	hash = 0;

	while ((byte = (unsigned long)*string++) != '\0')
		hash = hash + rbytes[i = (i + byte)&(HASH_RBYTES_LEN - 1)];
	return (hash % modulus);
}


