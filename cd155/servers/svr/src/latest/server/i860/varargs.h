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
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: varargs.h,v $
 * Revision 1.5  1994/11/18  20:32:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:00:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:19:21  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:11:06  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:20  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:27  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:31  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/11/13  12:23:28  rabii
 * 	Initial checkin
 *
 * Revision 2.2  90/12/04  14:48:35  jsb
 * 	First checkin.
 * 	[90/12/03  21:29:33  jsb]
 * 
 */
#ifndef	_VARARGS_H_
#define	_VARARGS_H_ 1

#ifdef	__STDC__
#include <varargs.pgi.h>
#else	/* __STDC__ */
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

#endif	/* __STDC__ */

#endif	/* _VARARGS_H_ */
