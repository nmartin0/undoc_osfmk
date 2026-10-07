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
static char rcsid[] = "@(#)$RCSfile: NCiscntrl.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:09 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCiscntrl
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
 * NCiscntrl.c	1.2  com/lib/c/nls,3.1,9013 2/27/90 21:31:08
 */

#include <sys/types.h>
#include <NLctype.h>

#ifdef NCiscntrl
#undef NCiscntrl
#endif

int
#ifdef _NO_PROTO
NCiscntrl(c)
int c;
#else
NCiscntrl(int c)
#endif
{
#ifdef KJI
	return(((unsigned short)(c) <= 0x7f) ? (((_locp->lc_chrtbl)->lc_ctype+1)[(unsigned short)(c)] & (_C)) : 0 );
#else
	return( ((_locp->lc_chrtbl)->lc_ctype+1)[c] & (_C) );
#endif
}
