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
static char rcsid[] = "@(#)$RCSfile: ungetwc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:23 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: ungetwc 
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
 * sccsid[] = "ungetwc.c 1.8  com/lib/c/io,3.1,9021 2/7/90 15:08:48";
 */

#include <stdio.h>
#include <NLchar.h>

/*
 * Unget a wide char by ungetting either one or two bytes.
 * If this is a two byte char unget the lower byte first.
 */

int
ungetwc(c, iop)
register int	c;
register FILE  *iop;
{
	char	wc[2];
	NLchar tc = c;

	if(c == EOF)
		return(EOF);
        if (NCencode(&tc, wc) == 1)
		/* one byte character */
                return (ungetc(c, iop));

	/* two byte character */
        if (ungetc((int)wc[1], iop) == EOF)
                return (EOF);
        if (ungetc((int)wc[0], iop) == EOF)
                return (EOF);
        return (c);
}
