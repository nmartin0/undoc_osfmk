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
 * coff_types.h
 *
 * COFF format dependent loader types and data structures
 *
 * OSF/1 Release 1.0
 */

#ifndef __H_COFF_TYPES
#define __H_COFF_TYPES

struct coff_module_handle {
	ldr_file_t	fd;		/* file descriptor for module */
	ldr_window_t	*wp;		/* window file header */
	univ_t		fhd;		/* module file header */
	ldr_window_t	*wpo;		/* window optional header */
	univ_t		ohd;		/* optional file header */
	ldr_entry_pt_t	entry_pt;	/* entry point, if any */
};

typedef struct coff_module_handle  *coff_module_handle_t;

#endif /* __H_COFF_TYPES */
