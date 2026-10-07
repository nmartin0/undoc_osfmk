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
 * OSF/1 Release 1.0.1
 */
/*	varargs.h	4.1	83/05/03	*/

#ifndef _VARARGS_H_
#define _VARARGS_H_


#include <standards.h>
#include <sys/types.h>

#ifndef _VA_LIST
#define _VA_LIST
#endif

typedef char* va_list;

/* These macros implement traditional (non-ANSI) varargs for GNU C.  */
/* This file will work for most stack-based machines. */

#define va_alist  __builtin_va_alist
#define va_dcl    int __builtin_va_alist;

#define va_start(AP)  AP=(char *) &__builtin_va_alist
#define va_end(AP)

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_arg(AP, TYPE)						\
 (AP += __va_rounded_size (TYPE),					\
  *((TYPE *) (AP - __va_rounded_size (TYPE))))


#endif /* _VARARGS_H_ */
