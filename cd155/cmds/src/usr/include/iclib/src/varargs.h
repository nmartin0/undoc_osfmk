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
 *
 *      Copyright 1990, The Portland Group, Incorporated.
 *      All rights reserved.
 *
 *        THE PORTLAND GROUP, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with The Portland Group and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

#ifndef _VARARGS_H
#define _VARARGS_H

#ifndef _VA_LIST
#define _VA_LIST
typedef struct {
	unsigned ireg_used;	/* How many int regs consumed 'til now? */
	unsigned freg_used;	/* How many flt regs consumed 'til now? */
	long *reg_base;		/* Address of where we stored the regs. */
	long *mem_ptr;		/* Address of memory args area. */
} va_list;
#endif

extern void  __i860_builtin_va_start(va_list *);
extern void *__builtin_va_arg();
#define va_start(ap) __i860_builtin_va_start(&ap)
#define va_arg(ap,type) (*(type *)__builtin_va_arg(&ap, (type *)0))
#define va_alist __builtin_va_alist
#define va_dcl int __builtin_va_alist;

#define va_end(list)

#endif
