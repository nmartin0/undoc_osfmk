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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: stdarg.h,v $
 * Revision 1.4  1994/11/18  20:31:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:57:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:16:11  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:21:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:47  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1992/07/29  08:23:35  rabii
 * 	Included in the AD source tree (pjg).
 *
 * Revision 1.4.4.2  91/03/11  14:40:08  sgrainge
 * 	Moved the typedef for va_list outside of
 * 	ifdef for ANSI compliance.
 * 	[91/03/11  14:37:44  sgrainge]
 * 
 * Revision 1.4  90/10/31  15:14:43  devrcs
 * 	Take out MIPS-centric stack alignment code....
 * 	[90/10/19  18:59:56  lehotsky]
 * 
 * Revision 1.3  90/10/07  20:20:19  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:34:49  gm]
 * 
 * Revision 1.2  90/06/22  21:51:06  devrcs
 * 	Addition for SS4
 * 	[90/06/14  22:42:10  kevins]
 * 
 * Revision 1.3  90/04/27  23:06:05  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:45:56  tom]
 * 
 * Revision 1.2  90/03/13  21:21:09  mbrown
 * 	put va_list in types.h for ANSI purposes
 * 	[90/02/22  11:07:03  mbrown]
 * 
 * 	Initial drop of machine-specific stdarg.h
 * 	[90/02/21  17:21:13  mbrown]
 * 
 * 	     Make stdargs.h work correctly on MIPS
 * 	[90/02/21  13:04:47  meissner]
 * 
 * $EndLog$
 */

#ifndef _STDARG_H
#define _STDARG_H

#include <standards.h>

#ifdef _ANSI_C_SOURCE


#ifndef _VA_LIST
#define _VA_LIST
#endif

typedef char*  va_list;

/* Amount of space required in an argument list for an arg of type TYPE.
   TYPE may alternatively be an expression whose type is used.  */

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(AP, LASTARG) 						\
 (AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))

void va_end (va_list);		/* Defined in gnulib */
#define va_end(AP)

#define va_arg(AP, mode)					\
		  (AP += __va_rounded_size (mode),		\
		   *((mode *) (AP - __va_rounded_size (mode))))
	     

#endif /* _ANSI_C_SOURCE */
#endif /* _STDARG_H */

