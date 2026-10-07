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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: NCgetbuf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:05 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCgetbuf
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
 * NCgetbuf.c	1.6  com/lib/c/nls,3.1,8943 9/13/89 16:49:09
 */

#include <sys/types.h>
#include <NLchar.h>
#include <stdlib.h>

/*
 * NAME: _NCgetbuf
 *
 * FUNCTION: Malloc a buffer of dlen NLchar's and decode string src into it.
 *
 * RETURN VALUE DESCRIPTION: NLchar ptr to the decoded string.
 */
/* 
 *  Malloc a buffer of dlen NLchar's and decode string src into it.
 */

NLchar *
_NCgetbuf(src, dlen)
char *src;       	/* a char string */
register int dlen;	/* the length of converted NLchar string */
{
	register NLchar *dest = (NLchar *)
		malloc((size_t)(dlen = (strlen(src) + 2) * sizeof (NLchar)));
			/* get a buffer for NLchar string */
	dest[0] = 1;
	(void)NCdecstr(src, &dest[1], (dlen / sizeof (NLchar)) - 1);
			/* decode to NLchar string */
	return (&dest[1]);
}
