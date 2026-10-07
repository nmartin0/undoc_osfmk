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
static char rcsid[] = "@(#)$RCSfile: NCisspace.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:18 $";
#endif

/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCisspace
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
 * sccsid[] = "NCisspace.c       1.3  com/lib/c/nls,3.1,9021 3/22/90 11:52:33";
 */

#include <sys/types.h>
#include <NLctype.h>

#ifdef NCisspace
#undef NCisspace
#endif

int
#ifdef _NO_PROTO
NCisspace(c)
int c;
#else
NCisspace(int c)
#endif
{
#ifdef KJI
	return( ((unsigned short)c <= 0x7f) ? 
		(((_locp->lc_chrtbl)->lc_ctype+1)[(unsigned short)(c)] & (_S|_B) ) : ((c)==0x8140) );
#else
	return( ((_locp->lc_chrtbl)->lc_ctype+1)[c] & (_S|_B) );
#endif
}
