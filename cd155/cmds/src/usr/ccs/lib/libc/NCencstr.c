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
static char rcsid[] = "@(#)$RCSfile: NCencstr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:02 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCencstr
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * NCencstr.c	1.11  com/lib/c/nls,3.1,9013 1/18/90 09:43:47
 */

#include <sys/types.h>
#include <NLchar.h>

/*
 * NAME: NCencstr
 *
 * FUNCTION: Convert a string of NLchars to chars.
 *
 * NOTE:     Macro NLchrlen returns the length of NLchar.
 *           Macro NCenc does the same as NCencode().
 *
 * RETURN VALUE DESCRIPTION: Return the length of string converted.
 */

/*
 * Convert a string of NLchars to chars; return length of string produced.
 */

int
NCencstr(nlc, c, len)
register NLchar *nlc;
register unsigned char *c;
register int len;	/* the length of a char string */
{
	register NLchar lc;	/* the next NLchar position */
	register int clen, lclen;   /* clen - the length of NLchar
                                     * lclen- a temporary storage for clen
                    		     */ 
	unsigned char *oc = c;	/* The next char position */

	/*  Always NUL-terminate output string, if any; but never count
	 *  NUL as part of length.
	 */
	if (0 < len)
		for (; ; lclen = clen) {
			lc = *nlc;
			if ((len -= (clen = NCchrlen(lc))) < 0)
				break;
			(void)NCenc(nlc, c);	/* encode to char */
			++nlc;
			if (!lc)
				break;
			c += clen;
		}
	if (oc < c && len < 0)
		*(c -= lclen) = 0;
	return (c - oc);
}

#ifdef _KJI
#include	<sys/types.h>

char *
wstrtos (c, nlc)
char *c;
wchar_t *nlc;
{
	/* NCencstr returns number of chars converted, but wstrtos doesn't 
	 * care.  wstrtos is susposed to return NULL on failure, but it
	 * cannot fail.
	 */
    	NCencstr (nlc, c, (2*NCstrlen (nlc))+1);
    	return (c);
}
#endif
