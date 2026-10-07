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
/* ldr_macro_help.h
 * Macros to assist in writing complex macros
 *
 * OSF/1 Release 1.0
 */

#ifndef _H_LDR_MACRO_HELP
#define _H_LDR_MACRO_HELP

/* A multi-statement macro should always start with a MACRO_BEGIN and
 * end with a MACRO_END, to avoid possible dangling-else problems with
 * using the macro in "if" statements.
 */

#define		MACRO_BEGIN	do {
#define		MACRO_END	} while (0)

#endif /* _H_LDR_MACRO_HELP */
