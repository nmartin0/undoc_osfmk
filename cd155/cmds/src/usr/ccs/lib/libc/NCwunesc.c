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
static char rcsid[] = "@(#)$RCSfile: NCwunesc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:24 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCwunesc
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
 * NCwunesc.c	1.2  com/lib/c/nls,3.1,8943 9/13/89 16:49:35
 */

#ifdef KJI
#include <NLctype.h>

/*
 * FUNCTION:
 * 	Translate the character escape sequence at src to a single NLchar at dest.
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	Return the number of NLchars translated.
 */


int
NCwunesc(src, dest)
register NLchar *src;
register NLchar *dest;
{
	register int length;
	char carray[5];
	register char *cptr = carray;
	register int i;
	NLchar *j;

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
		/*
		 *  Convert 'length' NLchars to chars before calling 
		 *  _NLunescval. 
		 */
		for (i = 0, j = &src[2]; i < length; i++, j++)
		     cptr += NCenc (j, cptr);
		if (_NLunescval (carray, length, dest) == -1)   {
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
