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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 *
 * relocate.h
 *
 * AT386 (intel 386) machine dependent Mach-O format dependent
 * loader interfaces.
 *
 * OSF/1 Release 1.0.1
 */

#ifndef _MO_RELOCATE

#define _MO_RELOCATE

extern int
patch_reloc_addr(reloc_info_t *reloc_p, univ_t vaddr, univ_t mapaddr,
		 mo_long_t relocation);

#endif /* _MO_RELOCATE */
