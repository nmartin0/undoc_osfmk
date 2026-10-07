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
static char rcsid[] = "@(#)$RCSfile: isj1kana.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:19 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library National Language Support
 *
 * FUNCTIONS: isj1bytekana
 *
 * ORIGINS: 10
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
/* isj1kana.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:41:48 */
#ifdef KJI
#include <NLctype.h>

static int
__isj1bytekana(int nlc)
{
	return(isj1bytekana(nlc));
}

#ifdef isj1bytekana
#undef isj1bytekana
#endif

int
isj1bytekana(int nlc)
{
	return(__isj1bytekana(nlc));
}
#endif KJI
