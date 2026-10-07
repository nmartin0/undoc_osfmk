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
static char rcsid[] = "@(#)$RCSfile: strtod.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:42 $";
#endif
/*
 * COMPONENT_NAME: LIBCCNV strtod
 *
 * FUNCTIONS: strtod
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * strtod.c	1.4  com/lib/c/cnv,3.1,8943 9/13/89 09:28:35
 */

/*
 * NAME: strtod
 *                                                                    
 * FUNCTION: convert character string to double
 *                                                                    
 * NOTES:
 *
 *      C library - string to double (strtod)
 *
 *      This version compiles strtod because STRTOD is set for atof.c.
 *      The only difference between atof and strtod is the storage
 *      of a pointer to the character which terminated the conversion.
 *
 * RETURNS: a double value
 */

#ifndef STRTOD
#define STRTOD	1
#endif
#include "atof.c"
