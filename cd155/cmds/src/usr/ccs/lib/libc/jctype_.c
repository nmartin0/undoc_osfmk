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
static char rcsid[] = "@(#)$RCSfile: jctype_.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:35 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN/KJI) Standard C Library Conversion Functions
 *
 * FUNCTIONS: none.
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
/* jctype_.c	1.1  com/lib/c/gen/KJI,3.1,9021 10/19/89 17:25:00 */

/*	jctype_ - character classification tables for KJI
 *	Two-level indexing (based on first, second bytes of character)
 *	is used to reduce data requirement.  This scheme relies on the
 *	fact that for most characters, the upper byte determines the
 *	characteristics.
 *						rcd  12-Apr-88
 *      These structures are isolated in include files so that they
 *	can be used by kernel extensions.  Bill Kennedy 19-Oct-89 
 */
#ifdef KJI
#include <NLctype.h>
#include <sys/jctype0.h>
#include <sys/jctype1.h>
#endif KJI
