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

