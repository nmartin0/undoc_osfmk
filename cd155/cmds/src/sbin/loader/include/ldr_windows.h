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
 *	loader window interfaces for loader modules.
 *
 * OSF/1 Release 1.0
 */

#ifndef _H_LDR_WINDOW
#define _H_LDR_WINDOW

#include "ldr_macro_help.h"

struct ldr_window_t	{
	univ_t		map;		/* window refers to this mapping */
	ldr_file_t	fd;		/* file being windowed */
	off_t		start;		/* start pt. in file being windowed */
	size_t		length;		/* window size in bytes */
};

typedef struct ldr_window_t ldr_window_t;

extern ldr_window_t *
ldr_init_window __((ldr_file_t fd));

extern univ_t
ldr_file_window __((int start, size_t len, ldr_window_t *wp));

/*
 *	macro ldr_unwindow: get rid of window mapping
 */

#ifndef FAILURE
#define FAILURE 	0
#endif

#ifndef INVALID_FD
#define INVALID_FD  	-1
#endif

#define ldr_unwindow(wp) \
	MACRO_BEGIN \
	    if (dec_map_refcount((wp)->map) != FAILURE) { \
		(wp)->map = NULL; \
		(wp)->fd = INVALID_FD; \
		(wp)->start = 0; \
		(wp)->length = 0; \
		} \
		MACRO_END

extern int
ldr_flush_mappings __((ldr_file_t fd));

extern int
ldr_flush_lru_maps __(());

extern int 
dec_map_refcount __((univ_t map));  /* was ldr_mapping_t* */

#endif /* _H_LDR_WINDOW */







