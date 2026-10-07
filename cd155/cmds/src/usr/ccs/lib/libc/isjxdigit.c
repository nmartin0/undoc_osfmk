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
static char rcsid[] = "@(#)$RCSfile: isjxdigit.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:30 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: isjxdigit
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
 */
/* isjxdigit.c	1.1  com/lib/c/gen/KJI,3.1,9021 2/27/90 20:59:57 */

#ifdef KJI
#include <NLctype.h>

#ifdef isjxdigit
#undef isjxdigit
#endif

int
#ifdef _NO_PROTO
isjxdigit(c)
int c;
#else
isjxdigit(int c)
#endif
{
	return( ((unsigned)(c) > 0xff) 
		? ( (_jctype1_[_jctype0_[(unsigned short)(c)>>8]][(c)&0xff])
				& (_JX ))
		: 0 );
}
#endif
