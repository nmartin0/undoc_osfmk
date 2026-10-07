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
 * Generic Loader Subroutines
 */

#include <sys/types.h>
#include <loader.h>
#include <mach.h>
#include <errno.h>

#include "kls_subr.h"

/*
 * convert_ldr_prot_to_vm_prot()
 */
vm_prot_t
convert_ldr_prot_to_vm_prot(ldr_prot_t ldr_prot)
{
	vm_prot_t vm_prot;

	vm_prot = VM_PROT_NONE;
	if (ldr_prot & LDR_R)
		vm_prot |= VM_PROT_READ;
	if (ldr_prot & LDR_W)
		vm_prot |= VM_PROT_WRITE;
	if (ldr_prot & LDR_X)
		vm_prot |= VM_PROT_EXECUTE;
	return(vm_prot);
}

/*
 * convert_kls_vm_status_to_ldr_status()
 *
 * The general loader status strategy is to return LDR_SUCCESS (i.e.
 * 0) upon success and on failure, a negative errno value to indicate
 * the reason for the failure.  The kls_vm calls return zero for
 * success.  A non-zero return value implies failure.  A non-zero
 * return value that is equal to -1 implies a UNIX system call
 * failure, with errno set to indicate the reason for the failure.
 * Non-zero return values that are positive are Mach error numbers
 * and we simply convert them to a UNIX errno and set errno.
 */
int
convert_kls_vm_status_to_ldr_status(int kls_vm_status)
{
	if (kls_vm_status == 0)
		return(0);
	if (kls_vm_status != -1)
		errno = convert_mach_error_to_unix_errno((kern_return_t)kls_vm_status);
	return(-errno);
}

/*
 * convert_mach_error_to_unix_errno()
 */
int
convert_mach_error_to_unix_errno(kern_return_t mach_error)
{
	int unix_errno;

	switch (mach_error) {

	default:
	case KERN_INVALID_ARGUMENT:
		unix_errno = EINVAL;
		break;

	case KERN_SUCCESS:
		unix_errno = 0;
		break;

	case KERN_INVALID_ADDRESS:
	case KERN_PROTECTION_FAILURE:
		unix_errno = EFAULT;
		break;

	case KERN_NO_SPACE:
	case KERN_RESOURCE_SHORTAGE:
		unix_errno = ENOMEM;
		break;

	case KERN_NOT_RECEIVER:
	case KERN_NO_ACCESS:
		unix_errno = EACCES;
		break;

	case KERN_FAILURE:
	case KERN_MEMORY_FAILURE:
	case KERN_MEMORY_ERROR:
	case KERN_ABORTED:
		unix_errno = EIO;
		break;

	case KERN_ALREADY_IN_SET:
	case KERN_NAME_EXISTS:
		unix_errno = EEXIST;
		break;

	case KERN_NOT_IN_SET:
		unix_errno = ENOENT;
		break;
	}

	return(unix_errno);
}
