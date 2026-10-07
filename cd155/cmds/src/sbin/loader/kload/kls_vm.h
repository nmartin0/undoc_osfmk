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
 * kls_vm_init() - perform any initialization necessary for
 *                 manipulation of the kernel's address space
 */
extern int
kls_vm_init(void);

/*
 * kls_vm_allocate() - allocate paged virtual memory in the kernel's
 *                     address space
 */
extern int
kls_vm_allocate(vm_address_t *address, vm_size_t size,
		boolean_t anywhere);

/*
 * kls_vm_deallocate() - deallocate virtual memory in the kernel's
 *                       address space
 */
extern int
kls_vm_deallocate(vm_address_t address, vm_size_t size);

/*
 * kls_vm_read() - read virtual memory from the kernel's address
 *                 space
 */
extern int
kls_vm_read(vm_address_t address, vm_size_t size, pointer_t *data,
	    int *data_count);

/*
 * kls_vm_write() - write virtual memory to the kernel's address
 *                  space
 */
extern int
kls_vm_write(vm_address_t address, pointer_t data, int data_count);

/*
 * kls_vm_protect() - change protection of virtual memory in the
 *                    kernel's address space
 */
extern int
kls_vm_protect(vm_address_t address, vm_size_t size,
	       boolean_t set_maximum, vm_prot_t new_protection);

/*
 * kls_vm_allocate_wired() - allocate wired virtual memory in the
 *                           kernel's address space
 *
 * Wiring at time of allocation is only available via kloadcall(2).
 */
extern int
kls_vm_allocate_wired(vm_address_t *address, vm_size_t size,
		      vm_prot_t prot, boolean_t anywhere);

/*
 * kls_vm_wire() - wired virtual memory in the kernel's address space
 *
 * This call, used to wire memory, sometime after its allocation, is
 * only provided via the Mach VM primitives.
 */
extern int
kls_vm_wire(vm_address_t address, vm_size_t size, vm_prot_t prot);
