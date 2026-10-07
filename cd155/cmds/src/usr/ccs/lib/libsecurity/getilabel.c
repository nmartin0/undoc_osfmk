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
 * Copyright (c) 1989 SecureWare, Inc.
 * All Rights Reserved.
 */

#ident "@(#)getilabel.c	2.1 16:16:15 4/20/90 SecureWare"
#ident "@(#)getilabel.c	2.2 09:53:35 11/3/89 SecureWare, Inc."

#include <sys/secdefines.h>

#ifdef SEC_BASE
#ifdef SEC_ILB

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/security.h>
#include <sys/audit.h>
#include <sys/secpolicy.h>
#include <mandatory.h>

extern int errno;
extern int sec_errno;

/* Return values:
 *   -1 and errno = EINVAL means no process label yet.
 *    0 and errno = 0      means actual IR returned.
 */

getilabel (ir)
ilb_ir_t *ir;
{
	attr_t	attr;
	obj_t	obj;
	int	ret;

	/* Make sure mand_config structure is initialized */
	if (mand_init() != 0) {
		errno = EINVAL;
		sec_errno = ESEC_MAC_CONFIG_FAILURE;
		return -1;
	}
	
	attr.ir = (char *) ir;
	attr.ir_length = ilb_bytes();

	obj.o_pid = 0;

	ret = getlabel(mand_config.policy, MAND_SUBJ_IL_TAG, &attr,
			OT_PROCESS, &obj);
	
	if (ret == 0 && attr.code == SEC_WILDCARD_TAG) {
		errno = EINVAL;
		sec_errno = ESEC_WILDCARD_TAG;
		ret = -1;
	}

	return ret;
}
#endif
#endif
