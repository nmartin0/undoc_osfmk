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
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: cat.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:00:14 $";
#endif
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* "@(#)cat.c	1.5  com/lib/PW,3.1,8943 9/11/89 09:11:23"; */
/*
 * COMPONENT_NAME: (LIBPW) Programmers Workbench Library
 *
 * FUNCTIONS: cat
 *
 * ORIGINS: 3
 *
 * OBJECT CODE ONLY SOURCE MATERIALS
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 * 
 */

/*
	Concatenate strings.
 
	cat(destination,source1,source2,...,sourcen,0);
 
	returns destination.
*/

#include <stdarg.h>

char *cat(char *dest, ...)
{
	register char *d;
	char *s;
	va_list arg_list;

        va_start(arg_list, dest);

	d = dest;
	while (s = va_arg(arg_list, char *))
		{
		while (*d++ = *s++);
		d--;
		}
	*d = 0;
	va_end(arg_list);

	return(dest);
}
