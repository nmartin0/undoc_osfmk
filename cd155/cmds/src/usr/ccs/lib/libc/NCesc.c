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
static char rcsid[] = "@(#)$RCSfile: NCesc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:04 $";
#endif

/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCesc
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
 * sccsid[] = "NCesc.c	1.3  com/lib/c/nls,3.1,9021 3/23/90 16:43:35";
 */

#include <sys/types.h>
#include <NLctype.h>

#ifndef KJI
static int 
__NCesc(NLchar *nlc, char *c)
{
	return(NCesc(nlc, c));
}

#ifdef NCesc
#undef NCesc
#endif

int
NCesc(NLchar *nlc, char *c) 
{
    	int retcode = 0;
	/* if NLchar value is less than 0x80 then return ascii character */
	if (*nlc < 0x80) { 
	    c[0] = *nlc;
	    retcode++;
	}
	else {
	    c[0] = '\\';
	    c[1] = '<';
	    if (_NLesctsize <= (unsigned)*(nlc)-0x80)  {
	        c[2] = '?';
	        c[3] = '?';
		retcode = 4;
	    }
	    else {
	        c[2] = _NLesctab[(int)(*nlc - 0x80)][0];
		c[3] = _NLesctab[(int)(*nlc - 0x80)][1];
		if (c[3]) {
		    c[4] = '>';
		    retcode = 5;
		}
		else {
		    c[3] = '>';
		    retcode = 4;
		}
	    }
	}
	return(retcode);
}

#else

int
NCesc(src, dest)
register NLchar *src;
register char *dest;
{
	register int length;

	/* the character does not need to be escaped
	 */
	if ((unsigned) *src < 0x80)   {
		dest[0] = src[0];
		return (1);
	}

	/* Try to find the escape sequence in NLesctab
	 */
	if (*src >= MINESCVAL && *src <= MAXESCVAL)
		return (_NLescval (src, dest));

	/* return a hex escape string
	 */
	NCeschex (src, dest);
	return (7);
}
#endif /* KJI */
