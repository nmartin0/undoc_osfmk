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
 * OSF/1 Release 1.0
 */
/*
 * Copyright (c) 1988-1990 SecureWare, Inc.  All rights reserved.
 */
#if SEC_BASE

#ident "@(#)at_secure.c	4.2 15:18:08 7/11/90 SecureWare"
/*
 * Based on:	@(#)at_secure.c	2.9 14:36:07 11/1/89
 */

#include <sys/security.h>

#if SEC_MAC || SEC_NCAV
#include <stdio.h>

extern char *malloc();

#if SEC_MAC
#include <mandatory.h>
#endif
#if SEC_NCAV
#include <ncav.h>
#endif
#endif /* SEC_MAC || SEC_NCAV */

/*
 * Check user's authorization to view and delete jobs belonging
 * to other users.
 */
at_authorized()
{
	static int	is_authorized = -1;

	if (is_authorized == -1)
		is_authorized = authorized_user("cron");
	return is_authorized;
}

#if SEC_MAC || SEC_NCAV
/*
 * Allocate a message buffer for communicating with the daemon.
 * Include space for a sensitivity label.
 */
char *
at_alloc_message(base_size)
	int	base_size;
{
	static char	*message = (char *) 0;
	int		msg_size = base_size;

	/*
	 * If message already allocated, just return it.
	 */
	if (message == (char *) 0) {
#if SEC_MAC
		if (mand_init()) {
			fprintf(stderr, 
				"Cannot initialize for sensitivity labels.\n");
			exit(1);
		}
		msg_size += mand_bytes();
#endif
#if SEC_NCAV
		msg_size += sizeof(ncav_ir_t);
#endif

		message = malloc(msg_size);
		if (message == (char *) 0) {
			fprintf(stderr,
				"Cannot allocate space for daemon message.\n");
			exit(1);
		}
		msg_size = base_size;

#if SEC_MAC
		if (getslabel((mand_ir_t *) &message[msg_size]) < 0) {
			fprintf(stderr, "Cannot get your sensitivity label.\n");
			exit(1);
		}
		msg_size += mand_bytes();
#endif
#if SEC_NCAV
		if (getncav((ncav_ir_t *) &message[msg_size]) < 0) {
			fprintf(stderr,
				"Cannot get your nationality caveats.\n");
			exit(1);
		}
#endif
	}

	return message;
}


/*
 * Write a message to the cron daemon, including label information.
 */
at_send_message(fd, message, msg_size)
	int	fd;
	char	*message;
	int	msg_size;
{

#if SEC_MAC
	msg_size += mand_bytes();
#endif
#if SEC_NCAV
	msg_size += sizeof(ncav_ir_t);
#endif

	return write(fd, message, msg_size) == msg_size;
}
#endif /* SEC_MAC || SEC_NCAV */

#endif /* SEC_BASE */
