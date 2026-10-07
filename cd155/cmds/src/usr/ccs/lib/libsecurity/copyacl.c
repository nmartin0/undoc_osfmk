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
 * (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * OSF/1 Release 1.0.4
 */

/*
 * Copyright (c) 1991, SecureWare, Inc.		All rights reserved.
 * 
 * copyacl.c - ACL support routines for applications
 *
 * copyacl(3)-routine designed to transfer an ACL from the specified source
 * file or directory to a target file or directory. Setting the ACL on an
 * object requires ownership of the object as well as write permission to the
 * object.
 */

/*
 * Based on:
 *	 "@(#)copyacl.c	3.1 08:34:32 5/16/91 SecureWare, Inc."
 */

#include <sys/secdefines.h>

#if SEC_ACL_SWARE

#include <sys/types.h>
#include <sys/security.h>

#include <acl.h>
#include <errno.h>

/* Static acl buffer for retrieval/setting */

static acle_t *acl_buffer;
static int acl_buffer_size;


/*
 * Retrieve the acl from the source (if one) and place it on the target file.
 * If raise is set, raise the OWNER and ALLOWDACACCESS overrides to permit
 * the operation to succeed for privileged programs without regard to the
 * attributes of the target file/directory.
 */

int
copyacl(source, target, raise)
char *source, *target;
int raise;
{
	privvec_t savep;
	int ret, acl_size;
	int new_size;

	/* Raise privileges if requested to do so */

	if (raise)
		forceprivs(privvec(SEC_OWNER, SEC_ALLOWDACACCESS, -1), savep);

	/* Determine the required buffer size for the ACL */

	if ((acl_size = statacl(source, (acle_t *) 0, 0)) == -1) {

		/* Determine if a WILDCARD ACL or an error occurred */

		if (errno == EINVAL) {

			/* set WILDCARD ACL on the target */

			ret = chacl(target, ACL_DELETE, -1);
			goto leave;
		}

		ret = -1;
		goto leave;
	}

	/* An ACL size of 0 indicates a Null ACL - handle specially */

	if (!acl_size) {

		/* Set the ACL on the target to a Null ACL */

		ret = chacl(target, (acle_t *) 0, 0);
		goto leave;
	}

	/* The source file has an ACL of one or more entries */

	if ((new_size = (acl_size * sizeof(acle_t))) > acl_buffer_size) {

		/* Increase the buffer size if one exists already */

		if (acl_buffer)
			acl_buffer = (acle_t *) realloc(acl_buffer, new_size);
		else
			acl_buffer = (acle_t *) malloc(new_size);

		if (!acl_buffer) {
			ret = -1;
			goto leave;
		}

		acl_buffer_size = new_size;
	}

	/* Retrieve the ACL entries from the source file */

	if ((ret = statacl(source, acl_buffer, acl_size)) == -1)
		goto leave;

	/* Set the ACL onto the target file */

	ret = chacl(target, acl_buffer, acl_size);

leave:

	/* Restore the caller's privileges (if required) and return status */

	if (raise)
		seteffprivs(savep, (priv_t *) 0);

	return(ret);
}

#endif /* SEC_ACL_SWARE */
