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
static char rcsid[] = "@(#)$RCSfile: mbspbrk.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:07 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: mbspbrk
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * mbspbrk.c	1.4  com/lib/c/nls,3.1,9013 2/27/90 21:41:12
 */

#include <sys/types.h>
#include <NLchar.h>

/*
 * NAME: mbspbrk
 *
 * FUNCTION: Locate the first occurrence of multibyte characters (code 
 *           points) in a string
 *
 * PARAMETERS: 
 *	     char *string - compared string
 *	     char *brkset - comparing multibyte character set
 *
 * RETURN VALUE DESCRIPTION: 
 *  Return ptr to first occurrence of any character from `brkset'
 *  in the character string `string'; NULL if none exists.
 */

#ifdef NULL
#undef NULL
#endif
#define	NULL	0

char *
#ifdef _NO_PROTO
mbspbrk(string, brkset)
char *string, *brkset;
#else
mbspbrk(char *string, char *brkset)
#endif
{
	register wchar_t *p;
	wchar_t buf[NLCSETMAX], sc;
	wchar_t *nlbrkset = (wchar_t *)_NCbufstr(brkset, buf, NLCSETMAX);
	register int i;

	for (; ; ) {
		if (!*string)
			break;
		i = NCdec(string, &sc);
		for(p = nlbrkset; *p != 0 && *p != sc; ++p)
			;
		if(*p != 0) {
			_NCfreebuf(nlbrkset);
			return(string);
		}
		string += i;
	}
	_NCfreebuf(nlbrkset);
	return(NULL);
}
