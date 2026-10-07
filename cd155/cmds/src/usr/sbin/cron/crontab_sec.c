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
 * Copyright (c) 1988-1990, SecureWare, Inc.  All rights reserved.
 */
#ident "@(#)crontab_sec.c	4.1 10:11:07 7/12/90 SecureWare"
/*
 * Based on:	@(#)crontab_sc.c	2.8 16:01:05 10/19/89
 */

#include <sys/secdefines.h>

#if SEC_BASE
#include <sys/security.h>
#include <prot.h>
#include <fcntl.h>

#if SEC_MAC || SEC_NCAV

#include <stdio.h>

extern char *malloc();

#endif /* SEC_MAC || SEC_NCAV */

/*
 * Create the crontab so that it cannot be tampered by other than the
 * cron utilities.
 */
int
crontab_secure_create(file_name)
	char	*file_name;
{
	int	cfs_status;

	cfs_status = create_file_securely(file_name, AUTH_SILENT,
					  "create crontab file");
	if (cfs_status != CFS_GOOD_RETURN)
		return -1;
	return open(file_name, O_WRONLY);
}


#if SEC_MAC || SEC_NCAV
/*
 * Set up the message buffer to send to the cron daemon.  Include
 * the security level of the sender, obtaining that information from
 * getslabel().
 */
char *
crontab_set_message(base_size)
	int base_size;
{
	char *message;
	int msg_size = base_size;

#if SEC_MAC
	if (mand_init()) {
		fprintf(stderr, "Cannot initialize for sensitivity labels.\n");
		exit(1);
	}
	msg_size += mand_bytes();
#endif
#if SEC_NCAV
	if (ncav_init()) {
		fprintf(stderr, "Cannot initialize for nationality caveats.\n");
		exit(1);
	}
	msg_size += sizeof(ncav_ir_t);
#endif

	message = malloc(msg_size);
	if (message == (char *) 0) {
		fprintf(stderr, "Cannot allocate cron message\n");
		exit(1);
	}
	msg_size = base_size;

#if SEC_MAC
	if (getslabel((mand_ir_t *) (message + msg_size)) < 0) {
		fprintf(stderr, "Cannot get your sensitivity level\n");
		exit(1);
	}
	msg_size += mand_bytes();
#endif
#if SEC_NCAV
	if (getncav((ncav_ir_t *) (message + msg_size)) < 0) {
		fprintf(stderr, "Cannot get your nationality caveats\n");
		exit(1);
	}
#endif

	return message;
}


/*
 * Write the message to the cron daemon, including the security level.
 * Also, free the space used for the message.
 */
int
crontab_write_message(fd, message, base_size)
	int fd;
	char *message;
	int base_size;
{
	int status;

#if SEC_MAC
	base_size += mand_bytes();
#endif
#if SEC_NCAV
	base_size += sizeof(ncav_ir_t);
#endif
	status = (write(fd, message, base_size) == base_size);

	free(message);

	return status;
}
#endif /* SEC_MAC || SEC_NCAV */

#endif /* SEC_BASE */
