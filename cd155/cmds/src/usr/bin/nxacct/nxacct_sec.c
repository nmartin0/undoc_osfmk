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
 * Copyright (c) 1988-1990 SecureWare, Inc.  All rights reserved.
 */
/*
 * OSF/1 Release 1.0
 */

#include <sys/secdefines.h>

#if SEC_BASE

/*
 * Based on:	@(#)newgrp_sec.c	2.1 11:11:03 1/25/89
 */

#include <sys/security.h>
#include <prot.h>

/*
 * Prompt for and encrypt a password and return 1 if the password matches
 * the supplied cipher text, and return 0 if not.  Erase the cleartext
 * buffer before returning.
 */
int
nxacct_good_password(cipher, message)
	char *cipher;
	char *message;
{
	register int is_good;

	is_good = (strcmp(bigcrypt(getpasswd(message, AUTH_MAX_PASSWD_LENGTH),
				   cipher),
			  cipher) == 0);

	/*
	 * Clear the cleartext left in the static buffer in getpasswd().
	 */
	(void) getpasswd((char *) 0, AUTH_MAX_PASSWD_LENGTH);

	return is_good;
}
#endif
