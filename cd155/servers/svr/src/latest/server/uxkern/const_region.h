/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright (c) 1994, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: const_region.h,v $
 * Revision 1.2  1994/11/18  20:47:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1994/09/20  18:17:19  yazz
 * Initial revision
 *
 */

#ifndef	_CONST_REGION_H
#define	_CONST_REGION_H
#include <sys/param.h>

/*
 * The macros defined here allow some portions of the server's own
 * data segment to be protected as read-only, to help find bugs that
 * scribble in unintended regions of memory.  This technique is suitable
 * for areas like jump tables that never change.  Each area to be protected
 * must be surrounded by a dummy buffer that's at least as large as the
 * current page size.  (We use NBPG or 8K, whichever is larger.)
 */

#if NBPG > 8*1024
#define CONST_BUFSIZE NBPG
#else
#define CONST_BUFSIZE 8*1024
#endif

#define START_CONST_REGION(x)	char s_##x [CONST_BUFSIZE] = { 0 }
#define END_CONST_REGION(x)	char e_##x [CONST_BUFSIZE] = { 0 }

#define	PROTECT_CONST_REGION(start, end, name)				\
MACRO_BEGIN								\
	kern_return_t	kr;						\
	vm_size_t len = (vm_size_t)((caddr_t)(end) - (caddr_t)(start));	\
	ASSERT(len > 0);						\
	kr = vm_protect(mach_task_self(), (vm_address_t)(start),	\
			len, FALSE, VM_PROT_READ);			\
	if (kr == KERN_SUCCESS) {					\
		printf("Server data region '%s' (0x%x to 0x%x) protected "\
				"read-only.\n", name, (start), (end));	\
	} else {							\
		panic("Kr=0x%x trying to protect %s region, "		\
				"start=0x%x len=%d(0x%x)\n",		\
				kr, name, (start), len, len);		\
	}								\
MACRO_END

#endif	/* ! _CONST_REGION_H */
