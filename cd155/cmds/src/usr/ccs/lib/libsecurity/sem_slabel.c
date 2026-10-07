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
 * All rights reserved.
 */

#ident "@(#)sem_slabel.c	2.1 16:16:48 4/20/90 SecureWare"
#ident "@(#)sem_slabel.c	2.1 11:12:59 1/25/89 SecureWare, Inc."

#include <sys/secdefines.h>

#if SEC_MAC

#include <sys/types.h>
#include <sys/errno.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <sys/secpolicy.h>
#include <mandatory.h>

extern int	errno;
extern int	sec_errno;

sem_statslabel (semid, slabel)
	int	semid;
	mand_ir_t *slabel;
{
	obj_t		obj;
	attr_t		attr;
	int		ret;

	/* Make sure the mand_config structure is initialized */
	if (mand_init () != 0) {
		errno = EINVAL;
		sec_errno = ESEC_MAC_CONFIG_FAILURE;
		return -1;
	}

	attr.ir = (char *) slabel;
	attr.ir_length = mand_bytes();

	obj.o_semid = semid;

	ret = getlabel(mand_config.policy, MAND_OBJ_SL_TAG,
		&attr, OT_SEMAPHORE, &obj);

	if (ret == 0 && attr.code == SEC_WILDCARD_TAG) {
		ret = -1;
		errno = EINVAL;
		sec_errno = ESEC_WILDCARD_TAG;
	}

	return ret;
}


sem_chslabel (semid, slabel)
	int	semid;
	mand_ir_t *slabel;
{
	obj_t		obj;
	attr_t		attr;

	/* Make sure the mand_config structure is initialized */
	if (mand_init () != 0) {
		errno = EINVAL;
		sec_errno = ESEC_MAC_CONFIG_FAILURE;
		return -1;
	}

	if (slabel == (mand_ir_t *) 0) {
		attr.code = SEC_WILDCARD_TAG;
		attr.ir = (char *) 0;
		attr.ir_length = 0;
	} else {
		attr.code = SEC_ACTUAL_TAG;
		attr.ir = (char *) slabel;
		attr.ir_length = mand_bytes();
	}

	obj.o_semid = semid;

	return setlabel(mand_config.policy, MAND_OBJ_SL_TAG,
		&attr, OT_SEMAPHORE, &obj);
}
#endif
