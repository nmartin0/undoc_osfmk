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
static char rcsid[] = "@(#)$RCSfile: NCislower.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:13 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCislower
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
 * NCislower.c	1.2  com/lib/c/nls,3.1,9013 2/27/90 21:32:23
 */

#include <sys/types.h>
#include <NLctype.h>

#ifdef NCislower
#undef NCislower
#endif

int
#ifdef _NO_PROTO
NCislower(c)
int c;
#else
NCislower(int c)
#endif
{
#ifdef _KJI
	return ( (_jctype1_[_jctype0_[(unsigned short)(c)>>8]][(c)&0xff])
				& (_Ja) );
#else
	return( ((_locp->lc_chrtbl)->lc_ctype+1)[c] & (_L) );
#endif
}
