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
/* Copyright (c) 1988-90, SecureWare, Inc.
 *   All rights reserved.
 *
 * Routine that hides getlabel from access control list users.
 */

#ident "@(#)statacl.c	2.1 16:15:56 4/20/90 SecureWare"
#ident "@(#)statacl.c	2.1 11:13:41 1/25/89 SecureWare, Inc."

#include <sys/secdefines.h>

#if SEC_BASE
#if SEC_ACL_SWARE

#include <sys/types.h>
#include <sys/errno.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <sys/secpolicy.h>
#include <acl.h>

extern int errno;
extern int sec_errno;

/*
 * Get the ACL associated with a file.
 * Returns number of entries in the ACL on success,
 * -1 and an errno on failure.  EINVAL indicates that
 * the file has no ACL (a wildcard).
 */

statacl (path, acl, size)
char	*path;	/* pathname of file */
acle_t	*acl;	/* array of ACL entries */
int	size;	/* elements of ACL array */
{
	obj_t	obj;
	attr_t	attr;
	int	ret;
	extern int errno;

	if (acl_init() == -1) {
		errno = EINVAL;
		sec_errno = ESEC_ACL_CONFIG_FAILURE;
		return (-1);
	}

	attr.ir_length = size * sizeof (acle_t);
	attr.ir = (char *) acl;
	obj.o_file = path;
	ret = getlabel(acl_config.policy, ACL_OBJ_TAG,
		&attr, OT_REGULAR, &obj);
	if (ret == 0)
		switch (attr.code) {
		case SEC_WILDCARD_TAG:
			errno = EINVAL;
			sec_errno = ESEC_WILDCARD_TAG;
			ret = -1;
			break;
		case SEC_ACTUAL_TAG:
			ret = attr.ir_length / sizeof (acle_t);
			break;
		}
	return (ret);
}
#endif
#endif
