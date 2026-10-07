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
/* Copyright (c) 1988-90 SecureWare, Inc.
 * All rights reserved.
 */

#ident "@(#)sem_acl.c	2.1 16:15:51 4/20/90 SecureWare"
#ident "@(#)sem_acl.c	2.1 11:13:35 1/25/89 SecureWare, Inc."

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
 * Get the ACL associated with a semaphore
 */

sem_statacl (semid, acl, size)
	int	semid;
	acle_t	*acl;
	int	size;
{
	int	rval;
	obj_t	obj;
	attr_t	attr;

	/* Make sure acl_config structure is initialized */
	if (acl_init () == -1) {
		errno = EINVAL;
		sec_errno = ESEC_ACL_CONFIG_FAILURE;
		return (-1);
	}

	attr.ir = (char *) acl;
	attr.ir_length = size * sizeof (acle_t);
	obj.o_semid = semid;
	rval = getlabel(acl_config.policy, ACL_OBJ_TAG, &attr,
		OT_SEMAPHORE, &obj);
	if (rval == 0)
		switch (attr.code) {
		case SEC_WILDCARD_TAG:
			rval = -1;
			errno = EINVAL;
			sec_errno = ESEC_WILDCARD_TAG;
			break;
		case SEC_ACTUAL_TAG:
			rval = attr.ir_length / sizeof(acle_t);
			break;
		}
	return rval;
}

/*
 * Change the ACL associated with a semaphore
 */

sem_chacl (semid, acl, size)
	int	semid;
	acle_t	*acl;
	int	size;
{
	obj_t	obj;
	attr_t	attr;

	/* Make sure the acl_config structure is initialized */
	if (acl_init () == -1) {
		errno = EINVAL;
		sec_errno = ESEC_ACL_CONFIG_FAILURE;
		return (-1);
	}

	if (acl == ACL_DELETE) {
		attr.code = SEC_WILDCARD_TAG;
		attr.ir = (char *) 0;
		attr.ir_length = 0;
	} else {
		attr.code = SEC_ACTUAL_TAG;
		attr.ir = (char *) acl;
		attr.ir_length = size * sizeof (acle_t);
	}

	obj.o_semid = semid;

	return setlabel(acl_config.policy, ACL_OBJ_TAG,
		&attr, OT_SEMAPHORE, &obj);
}
#endif
#endif
