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

extern int
kernel_load(char *file_pathname, ldr_load_flags_t load_flags,
	    ldr_module_t *module);

extern int
kernel_unload(ldr_module_t module);

extern int
kernel_ldr_entry(ldr_module_t module, ldr_entry_pt_t *entry);

extern int
kernel_ldr_lookup(ldr_module_t module, char* symbol_name,
		  void *symbol_addr);

extern int
kernel_ldr_lookup_package(char *package_name, char *symbol_name,
			  void *symbol_addr);

extern int
kernel_next_module(ldr_module_t *module);

extern int
kernel_inq_module(ldr_module_t module, ldr_module_info_t *info,
		  size_t info_size, size_t *ret_size);

extern int
kernel_inq_region(ldr_module_t module, ldr_region_t region,
		  ldr_region_info_t *info, size_t info_size,
		  size_t *ret_size); 
