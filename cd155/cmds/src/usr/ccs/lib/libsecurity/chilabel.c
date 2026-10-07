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

#ident "@(#)chilabel.c	2.1 16:16:05 4/20/90 SecureWare"
#ident "@(#)chilabel.c	2.2 08:22:30 11/3/89 SecureWare, Inc."

#include <sys/secdefines.h>

#if SEC_BASE
#if SEC_ILB
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/security.h>
#include <sys/audit.h>
#include <sys/secpolicy.h>
#include <mandatory.h>

extern int sec_errno;

/* Return values:
 *   -1 and errno = EIO    means policy database could not be initialized
 *   -1 and errno = EINVAL means illegal ir or SL is wildcard.
 *    0 and errno = 0      means success
 */

chilabel (path, ir)
char	*path;
ilb_ir_t *ir;
{
	attr_t	attr;
	obj_t	obj;
	extern int errno;

	/* Make sure mand_config structure is initialized */
	if (mand_init() == -1) {
		errno = EINVAL;
		sec_errno = ESEC_MAC_CONFIG_FAILURE;
		return (-1);
	}
	
	if (ir == (ilb_ir_t *) 0) {
		attr.code = SEC_WILDCARD_TAG;
		attr.ir = (char *) 0;
		attr.ir_length = 0;
	} else {
		attr.code = SEC_ACTUAL_TAG;
		attr.ir = (char *) ir;
		attr.ir_length = ilb_bytes();
	}

	obj.o_file = path;

	return setlabel(mand_config.policy, MAND_OBJ_IL_TAG,
			&attr, OT_REGULAR, &obj);
}
#endif
#endif
