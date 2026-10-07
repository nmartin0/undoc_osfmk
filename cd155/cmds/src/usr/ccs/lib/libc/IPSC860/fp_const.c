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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)fp_const.c	1.6  com/lib/c/cnv,3.1,8943 10/27/89 16:02:27";
/*
 * COMPONENT_NAME: LIBCCNV floating point constants
 *
 * FUNCTIONS: NONE
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
 */

#include <fp.h>

/*
 * NAME: fp constants
 *                                                                    
 * FUNCTION: define several floating point constants
 *                                                                    
 * NOTES:
 *
 *      This file contains the external definitions for various
 *      values declared in float.h and math.h. This sort of definition
 *      is allowed in ANSI C.
 *
 *      For FORTRAN programs these external variables can be
 *      referenced directly as external variables. Thus the names
 *      below must be FORTRAN acceptable labels.
 *
 *
 */

/*
 *      Doubles
 */


	unsigned int DEPSILON[2]    = { INTS2DBL(0x3cb00000, 0x00000000) };
	unsigned int DFPMIN[2]      = { INTS2DBL(0x00100000, 0x00000000) };
	unsigned int DFPMAX[2]      = { INTS2DBL(0x7fefffff, 0xffffffff) };
	unsigned int DINFINITY[2]   = { INTS2DBL(0x7ff00000, 0x00000000) };
	unsigned int DQNAN[2]       = { INTS2DBL(0x7ff80000, 0x00000000) };
	unsigned int DSNAN[2]       = { INTS2DBL(0x7ff55555, 0x55555555) };

/*
 *      Floats (SINGLES)
 */

	unsigned int SEPSILON  = 0x34000000;
	unsigned int SFPMIN    = 0x00800000;
	unsigned int SFPMAX    = 0x7f7fffff;
	unsigned int SINFINITY = 0x7f800000;
	unsigned int SQNAN     = 0x7fc00000;
	unsigned int SSNAN     = 0x7f855555;
