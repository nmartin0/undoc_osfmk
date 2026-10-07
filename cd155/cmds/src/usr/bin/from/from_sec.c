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
 * SecureWare, Inc.
 *
 * Copyright (c) 1990, All rights reserved.
 *
 * From(1) mail handling hook routines
 */

#include <sys/secdefines.h>

#if SEC_MAC

#ident "@(#)from_sec.c	5.1 17:50:43 8/15/90 SecureWare"

#include <sys/types.h>
#include <sys/security.h>
#include <mandatory.h>
#include <prot.h>
#include <errno.h>
#include <stdio.h>

static mand_ir_t	*mailbox_level;

extern int		sec_errno;

/*
 * from_init()-initialization for the hook routines
 */

void
from_init(argc, argv)
	int	argc;
	char	*argv[];
{
	set_auth_parameters(argc, argv);
	initprivs();

	if ((mailbox_level = mand_alloc_ir()) == NULL) {
		fprintf(stderr, "%s: can't initialize for sensitivity labels\n",
			command_name);
		exit(1);
	}
}


/*
 * See if the current process sensitivity level dominates that of the
 * mailbox.  If not, we infer that the user is not logged in at his
 * clearance.
 */

int
from_check_mailbox(file)
	char	*file;
{

	/*
	 * If we are able to retrieve the file's sensitivity label, then
	 * our process level dominates (or equals) the mailbox level, or
	 * we have the MAC override privilege.  Assuming we don't have the
	 * privilege, then if the mailbox is our own and is properly labeled,
	 * that means we are logged in at our clearance.  Conversely, if we
	 * can't get the file's sensitivity label, that means we are logged
	 * in below our clearance and need to be admonished.
	 *
	 * If the mailbox is someone else's, the open in the main program will
	 * fail unless we have the DAC override privilege.
	 */

	if (statslabel(file, mailbox_level) == -1 && errno == EACCES &&
			sec_errno == ESEC_MAC_SDOM)
		return 0;

	return 1;
}

#endif /* SEC_MAC */
