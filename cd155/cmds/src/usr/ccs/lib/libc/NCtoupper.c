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
static char rcsid[] = "@(#)$RCSfile: NCtoupper.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:22 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: NCtoupper
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
 * NCtoupper.c	1.5  com/lib/c/gen,3.1,8943 9/7/89 17:33:51
 */

#include <NLctype.h>

/*LINTLIBRARY*/
/*
 *
 *  If arg is lower-case, return upper-case, else return the arg.
 *  This version uses _NLctab to classify c according to the current
 *  NLS environment.
 */

int
NCtoupper(c)
register int c;
{
	return ((NCislower(c)) ? _NCtoupper(c) : c);
}
