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
 *	ldr_types.h
 *	loader defined types
 *	NOTE: other include files needed in order to use this one :
 *		<sys/types.h> <loader.h>
 *
 * OSF/1 Release 1.0
 */

#ifndef _H_LDR_TYPES
#define _H_LDR_TYPES

/* Externally-visible type declarations */

#ifndef _H_LDR_MAIN_TYPES
#include <loader/ldr_main_types.h>
#endif /* _H_LDR_MAIN_TYPES */

typedef	univ_t	ldr_heap_t;		/* Heap data type (opaque) */

typedef	univ_t	ldr_export_list;	/* opaque type for fmt-dep exports */

typedef	enum {				/* kinds of entry points */
	init_routines,			/* initialization routine */
	term_routines			/* termination routine */
}  entry_pt_kind;

typedef univ_t		ldr_switch_t;

#ifndef NULL
#define NULL	0
#endif /* NULL */

extern ldr_heap_t	ldr_process_heap;

/* Machine-dependent type declarations */

#include <ldr_machdep.h>

#endif /* _H_LDR_TYPES */
