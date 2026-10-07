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
static char rcsid[] = "@(#)$RCSfile: NLflatstr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:38 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NLflatstr
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
 * sccsid[] = "NLflatstr.c       1.10  com/lib/c/nls,3.1,9021 1/18/90 09:45:37";
 */

#include <NLctype.h>

/*
 * NAME: NLflatstr
 *
 * FUNCTION: Translate each NLS code point in src into the single 
 *           standard ASCII char most nearly matching the code 
 *           point's appearance.
 *
 * RETURN VALUE DESCRIPTION: The length of the converted char.
 */
/*
 *
 *  Translate each NLS code point in src into the single standard ASCII
 *  character most nearly matching the code point's appearance.
 */
#ifndef _KJI   	/*  This function not included on JLS platform. */
int
NLflatstr(src, dest, dlen)
register unsigned char *src, *dest;
register int dlen;	/* the length of a dest string */
{
	register int n;	/* the length of NLS code */
	unsigned char *odest = dest;	/* the next char position */

	/*  Always NUL-terminate output string, if any; but never count
	 *  NUL as part of length.
	 */
	for ( ; dlen && *src; --dlen)
		if (n = NLisNLcp(src)) {		/* NLS ? */
			*dest++ = NCflatchr(NCdechr(src));
				/* decode to NLchar and convert to ASCII */
			src += n;
		} else
			*dest++ = *src++;
	if (odest < dest) {	/* more char? */
		if (!dlen)
			--dest;
		*dest = '\0';
	}
	return (dest - odest);
}
#endif /* ndef _KJI */
