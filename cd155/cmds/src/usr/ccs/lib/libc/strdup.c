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
static char rcsid[] = "@(#)$RCSfile: strdup.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:32 $";
#endif
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: strdup
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 *	strdup.c	1.2  com/lib/c/str,3.1,9013 2/3/90 09:32:46
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
 * FUNCTION: Copy string argument to a new malloc'ed string.
 *
 * PARAMETERS: 
 *	     char *s1 - string to copy
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer to new string
 */
/*LINTLIBRARY*/

#include	<sys/types.h>	/* for NULL */
#include	<strings.h>

extern char *strcpy();

char  *
strdup(char *s1)
{
	char *s2;

	if (s1 == NULL || (s2 = (char *)malloc(strlen(s1)+1)) == NULL)
		return(NULL);
	return(strcpy(s2, s1));
}
