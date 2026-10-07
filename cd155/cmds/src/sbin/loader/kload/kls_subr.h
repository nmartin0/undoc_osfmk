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

extern vm_prot_t
convert_ldr_prot_to_vm_prot(ldr_prot_t ldr_prot);

extern int
convert_kls_vm_status_to_ldr_status(int kls_vm_status);

extern int
convert_mach_error_to_unix_errno(kern_return_t mach_error);
