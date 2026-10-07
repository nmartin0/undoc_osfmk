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
static char rcsid[] = "@(#)$RCSfile: NCunesc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:23 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCunesc
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * NCunesc.c	1.2  com/lib/c/nls,3.1,9013 3/10/90 12:59:41
 */

#include <sys/types.h>
#include <NLctype.h>

#ifndef KJI
static int
__NCunesc(char * c, NLchar *nlc)
{
	return(NCunesc(c, nlc));
}

#ifdef NCunesc
#undef NCunesc
#endif

int
NCunesc(char * c, NLchar *nlc)
{
/*  Translate char escape string at c to single NLchar at nlc.
 */
	if (c[0] != '\\' || c[1] != '<' || 
	    _NLunescval(&c[2],c[3] == '>' ? 1 : 2, nlc) < 0) { 
	    nlc[0] = c[0];
	}
	else {
	    if (c[3] == '>') return (4);
	    if (c[4] == '>') return (5);
	    nlc[0] = c[0];
	}
 	return(1);
}

#else

int
NCunesc(src, dest)
register char *src;
register NLchar *dest;
{
	register int length;

	/* return the character if not the start of an escape sequence
	 */
	if (src[0] != '\\' || src[1] != '<')  {
		dest[0] = src[0];	
		return (1);
	}
	
	if (src[4] == '>')
		length = 2;
	else if (src[5] == '>')
		length = 3;
	else if (src[6] == '>')
		length = 4;
	else  {
		dest[0] = src[0];	
		return (1);
	}

	/* start of a mneumonic in unesctab 
	 */
	if (src[2] == 'j')  {
		if (_NLunescval (&src[2], length, dest) == -1)   {
			dest[0] = src[0];
			return (1);
		}
		else
			return (length + 3);
	}

	if (ishexesc(&src[2]) == -1)  {
		dest[0] = src[0];
		return (1);
	}
	else  {
		NCuneschex(src, dest);
		return (length + 3);
	}
}
#endif /* KJI */
