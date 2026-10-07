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
static char rcsid[] = "@(#)$RCSfile: _NCtolower.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:16 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: _NCtolower
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
 * _NCtolower.c	1.2  com/lib/c/nls,3.1,9013 2/27/90 21:55:38
 */

#include <NLctype.h>

#ifdef _NCtolower
#undef _NCtolower
#endif

int
_NCtolower(int c)
{
#ifdef _KJI
	return( (isascii(c) ? _tolower(c) : _tojlower(c)) );
#else
	return(((_locp->lc_chrtbl)->lc_caseconv+1)[c]);
#endif
}
