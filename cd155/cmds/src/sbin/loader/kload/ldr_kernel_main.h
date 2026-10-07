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
 * ldr_kernel_main.h
 *
 * OSF/1 Release 1.0
 */

extern ldr_context_t ldr_kernel_context;  /* the kernel context */

/* Bootstrap the loader system.  Includes building the loader process
 * context, containing the module and region records and exported symbol
 * list for the loader itself.  The loader_name argument is the name of
 * the loader's object module, for building the module record.
 */

extern int ldr_kernel_bootstrap(char *);

/* Absolute region allocator for regions to be loaded into a process.  This is
 * the "default" absolute region allocator used in initializing the process
 * loader context.
 */

extern int alloc_abs_kernel_region __((univ_t vaddr, size_t size,
				       ldr_prot_t prot, univ_t *baseaddr));

/* Relocatable region allocator for regions to be loaded into a process.  This is
 * the "default" relocatable region allocator used in initializing the process
 * loader context.
 */

extern int alloc_rel_kernel_region __((size_t size, ldr_prot_t prot,
				       univ_t *vaddr, univ_t *baseaddr));

/* Region deallocator for regions loaded into a process.  This is
 * the "default" region deallocator used in initializing the process loader
 * context.
 */

extern int dealloc_kernel_region __((univ_t vaddr, univ_t mapaddr, size_t size));
