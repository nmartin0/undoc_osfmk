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
/* ldr_auxv.h
 * declarations for loader auxiliary vector abstraction
 * Depends on "standards.h"
 *
 * OSF/1 Release 1.0
 */

#ifndef	_H_LDR_AUXV
#define	_H_LDR_AUXV


extern int
ldr_auxv_init __((void));

extern int
ldr_auxv_get_exec_filename __((char **));

extern int
ldr_auxv_get_exec_loader_filename __((char **));

extern int
ldr_auxv_get_exec_loader_flags __((int *));

extern int
ldr_auxv_get_pagesz __((int *));

#endif	/* _H_LDR_AUXV */
