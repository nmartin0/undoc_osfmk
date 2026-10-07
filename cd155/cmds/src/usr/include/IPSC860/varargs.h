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
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * $Log: varargs.h,v $
 * Revision 2.4  1994/11/19  02:41:46  mtm
 * Copyright additions/changes
 *
 * Revision 2.3  1992/08/05  15:55:58  shala
 * new version from PGI
 *
 * Revision 2.2  92/04/27  17:29:32  stans
 * "OSF/1.0.4"
 * 
 * Revision 2.2  90/12/04  14:48:35  jsb
 * 	First checkin.
 * 	[90/12/03  21:29:33  jsb]
 * 
 */

/* 
* varargs.h
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

#include <va_list.h>

extern void  __i860_builtin_va_start(va_list *);
extern void *__builtin_va_arg();
#define va_start(ap) __i860_builtin_va_start(&ap)
#define va_arg(ap,type) (*(type *)__builtin_va_arg(&ap, (type *)0))
#define va_alist __builtin_va_alist
#define va_dcl int __builtin_va_alist;

#define va_end(list)

#endif
