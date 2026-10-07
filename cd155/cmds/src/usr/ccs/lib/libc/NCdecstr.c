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
static char rcsid[] = "@(#)$RCSfile: NCdecstr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:00 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCdecstr
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
 * NCdecstr.c	1.12  com/lib/c/nls,3.1,9013 2/11/90 17:08:48
 */

#include <sys/types.h>
#include <NLchar.h>

/*
 * NAME: NCdecstr
 *
 * FUNCTION: Convert a string of chars to wchar_ts.
 *
 * RETURN VALUE DESCRIPTION: Return the length of string converted.
 */

/*
 * Convert a string of chars to wchar_ts; return length of string produced.
 */

int
NCdecstr(c, nlc, len)
register unsigned char *c;       /* char string */
register wchar_t *nlc;   /* wchar_t string */
register int len;     	/* the length of char */
{
	register unsigned char lc;      /* temporary storage for c */
	wchar_t *onlc = nlc;    /* the next wchar_t position */

	/*  Always NUL-terminate output string, if any; but never count
	 *  NUL as part of length.
	 */
	while (0 <= --len) {
		lc = *c;
		c += NCdec(c, nlc);  /* decode char to wchar_t */
		if (!lc)
			break;
		++nlc;
	}
	if (onlc < nlc && len < 0)
		*--nlc = 0;
	return (nlc - onlc);
}

#ifdef _KJI

#include <sys/types.h>

wchar_t *
strtows (nlc, c)
wchar_t *nlc;
char *c;
{
	NCdecstr (c, nlc, strlen (c) + 1);
	return (nlc);
}
#endif
