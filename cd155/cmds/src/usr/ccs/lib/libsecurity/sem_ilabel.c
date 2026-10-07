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
 * All rights reserved.
 */

#ident "@(#)sem_ilabel.c	2.1 16:16:43 4/20/90 SecureWare"
#ident "@(#)sem_ilabel.c	2.2 08:01:20 11/3/89 SecureWare, Inc."

#include <sys/secdefines.h>

#if SEC_ILB

#include <sys/types.h>
#include <sys/errno.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <sys/secpolicy.h>
#include <mandatory.h>

extern int	errno;
extern int	sec_errno;

sem_statilabel (semid, ilabel)
	int	semid;
	ilb_ir_t *ilabel;
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

	attr.ir = (char *) ilabel;
	attr.ir_length = ilb_bytes();

	obj.o_semid = semid;

	ret = getlabel(mand_config.policy, MAND_OBJ_IL_TAG, &attr,
			OT_SEMAPHORE, &obj);

	if (ret == 0 && attr.code == SEC_WILDCARD_TAG) {
		ret = -1;
		errno = EINVAL;
		sec_errno = ESEC_WILDCARD_TAG;
	}

	return ret;
}


sem_chilabel (semid, ilabel)
	int	semid;
	ilb_ir_t *ilabel;
{
	obj_t		obj;
	attr_t		attr;

	/* Make sure the mand_config structure is initialized */
	if (mand_init () != 0) {
		errno = EINVAL;
		sec_errno = ESEC_MAC_CONFIG_FAILURE;
		return -1;
	}

	if (ilabel == (ilb_ir_t *) 0) {
		attr.code = SEC_WILDCARD_TAG;
		attr.ir = (char *) 0;
		attr.ir_length = 0;
	} else {
		attr.code = SEC_ACTUAL_TAG;
		attr.ir = (char *) ilabel;
		attr.ir_length = ilb_bytes();
	}

	obj.o_semid = semid;

	return setlabel(mand_config.policy, MAND_OBJ_IL_TAG, &attr,
			OT_SEMAPHORE, &obj);
}
#endif
