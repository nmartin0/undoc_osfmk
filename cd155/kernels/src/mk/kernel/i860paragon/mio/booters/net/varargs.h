/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: varargs.h,v $
 * Revision 0.3  1994/11/18  20:46:10  mtm
 * Copyright additions/changes
 *
 * Revision 0.2  1993/06/30  22:40:37  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.1  1992/07/09  16:59:18  andyp
 * Initial checkin of Paragon files.
 *
 * Revision 2.2  91/12/10  16:31:36  jsb
 * 	New files from Intel
 * 	[91/12/10  16:12:01  jsb]
 * 
 */ 

#ifndef	_VARARGS_H_
#define	_VARARGS_H_ 1

#ifdef	PGI

/*
 * Portland Group, Inc. i860 compiler.
 */

#include <i860/varargs.pgi.h>

#else	/* PGI */

/*
 * GreenHills i860 compiler
 */
/*
 *      Copyright (c) 1989, Intel Corporation.
 *
 *        INTEL CORPORATION PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license
 * agreement or nondisclosure agreement with Intel Corp.
 * and may not be copied or disclosed except in accordance
 * with the terms of that agreement.
 */

typedef struct {
        char *   stkaddr;
        int      argnumint;
        int      argnumdbl;
        int *    iregs;
        double * dregs;
} va_list;

#define va_dcl      va_type va_alist;

#define va_start(ap)  ( (ap).argnumint=(ap).argnumdbl=va_argnum(va_alist), \
                            (ap).stkaddr= (char*)&(va_alist), \
                            (ap).iregs = va_intreg-1, \
                            (ap).dregs = va_dblreg-1 )

/*
 * va_arg must decide whether an argument of type 'type' can ever be passed
 * in a register - if it can, proceed to __va_reg_OK, if not,  
 * immediately go to __va_stack, which fetches from the extended parameter
 * block
 */
#define va_arg(ap,type) \
  ((va_regtyp(type))?(__va_reg_OK(ap,type)):(__va_stack(ap,type)))

/*
 * the parameter may be in a register - see whether it is an int analog or
 * a double
 */
#define __va_reg_OK(ap,type) \
  ((sizeof(type)==sizeof(int))?(__va_int(ap,type)):(__va_dbl(ap,type)))

/*
 * the parameter is an int analog - fetch from the parameter registers if
 * there have been fewer than twelve int analog arguments retrieved - note
 * that 'argnumint' starts at '1'.  If the twelve int analogs have already
 * been retrieved, go to the extended parameter block
 */
#define __va_int(ap,type) \
  ((++(ap).argnumint<=12)?(*((type *)((ap).iregs+(ap).argnumint))):(__va_stack(ap,type)))

/*
 * the parameter is an double - fetch from the parameter registers if
 * there have been fewer than four double arguments retrieved - note
 * that 'argnumdbl' starts at '1'.  If the four register doubles have already
 * been retrieved, go to the extended parameter block
 */
#define __va_dbl(ap,type) \
  ((++(ap).argnumdbl<=4)?(*((type *)((ap).dregs+(ap).argnumdbl))):(__va_stack(ap,type)))

/*
 * retrieve a parameter of type 'type' from the extended parameter block,
 * and advance the 'stkaddr' pointer to the next parameter in the extended
 * parameter block
 */
#define __va_stack(ap,type) \
  (((type *)((ap).stkaddr=((char*)(((int)(ap).stkaddr+va_align(type)-1)&-va_align(type)))+sizeof(type)))[-1])

#define va_end(list)

#endif	/* !PGI */

#endif	/* _VARARGS_H_ */
