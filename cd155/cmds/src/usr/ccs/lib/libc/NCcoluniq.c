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
static char rcsid[] = "@(#)$RCSfile: NCcoluniq.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:01:56 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NCcoluniq
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
 * NCcoluniq.c	1.2  com/lib/c/nls,3.1,9013 2/27/90 21:27:42
 */

/* This function returns the secondary, or unique, collation value for 
 * the wchar_t argument. The returned value can be either 
 *	positive -	the real secondary (lc_coluniq) value 
 *	zero     -	the wchar_t is non-collating (ignore)
 *
 * Note that the coluniq value returned only applies to the "single
 * char"; to retrieve the coluniq value for multi-character collating
 * elements, use the NLxcolu function.
 */

#include <sys/types.h>
#include <NLchar.h>

#ifdef NCcoluniq
#undef NCcoluniq
#endif


int
#ifdef _NO_PROTO
NCcoluniq(nlc)
wchar_t nlc;
#else
NCcoluniq(wchar_t nlc)
#endif
{
	if ( (_NCmap(nlc)) < (((_locp->lc_coltbl)->len_coluniq) >> 1) )
		return( (((_locp->lc_coltbl)->lc_coluniq+1)[_NCmap(nlc)]) );
	else	return( nlc );
}
