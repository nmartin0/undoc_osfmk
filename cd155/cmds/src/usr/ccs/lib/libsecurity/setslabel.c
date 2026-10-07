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
 * Copyright (c) 1988 SecureWare, Inc.
 * All Rights Reserved.
 */

#ident "@(#)setslabel.c	2.1 16:16:57 4/20/90 SecureWare"
#ident "@(#)setslabel.c	2.1 11:13:08 1/25/89 SecureWare, Inc."

#include <sys/secdefines.h>

#if SEC_BASE
#if SEC_MAC

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/security.h>
#include <sys/audit.h>
#include <sys/secpolicy.h>
#include <mandatory.h>

extern int errno;
extern int sec_errno;

/* Return values:
 *   -1 and errno = EPERM  means process sensitivity label already set
 *			   or no privilege or clearance not set yet or
 *			   changing to specified label would violate
 *			   a constraint
 *    0 and errno = 0      means success
 */

setslabel (ir)
mand_ir_t *ir;
{
	attr_t	attr;
	obj_t	obj;

	/* Make sure mand_config structure is initialized */
	if (mand_init() != 0) {
		errno = EINVAL;
		sec_errno = ESEC_MAC_CONFIG_FAILURE;
		return (-1);
	}
	
	attr.code = SEC_ACTUAL_TAG;
	attr.ir = (char *) ir;
	attr.ir_length = mand_bytes();

	obj.o_pid = 0;

	return setlabel(mand_config.policy, MAND_SUBJ_SL_TAG,
		&attr, OT_PROCESS, &obj);
}
#endif
#endif
