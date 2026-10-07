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
static char rcsid[] = "@(#)$RCSfile: ctermid.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:08 $";
#endif
/*
 * FUNCTIONS: ctermid 
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
 * static char sccsid[] = "%Z%%M%	%I%  %W% %G% %U%";
 */

/*LINTLIBRARY*/

#include <stdio.h>

extern char *strcpy();

#ifndef _THREAD_SAFE
static char res[L_ctermid];
#endif

/*
* Thread-safe ctermid 
*/
#ifdef _THREAD_SAFE
char *
ctermid(char *s)
{
	if (s != NULL)
		return(strcpy(s,"/dev/tty"));
	else
		return(NULL);
}
#else /* _THREAD_SAFE */
char *
ctermid(char *s)
{
	return (strcpy(s != NULL ? s : res, "/dev/tty"));
}
#endif /* _THREAD_SAFE */
