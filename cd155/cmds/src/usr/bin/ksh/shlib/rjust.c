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
static char sccsid[] = "@(#)rjust.c	1.6  com/cmd/ksh/shlib,3.1,9013 9/10/89 06:35:54";
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *   NAM_RJUST.C
 *
 *   Programmer:  D. G. Korn
 *
 *        Owner:  D. A. Lambeth
 *
 *         Date:  April 17, 1980
 *
 *
 *
 *   NAM_RJUST (STR, SIZE, FILL)
 *
 *      Right-justify STR so that it contains no more than 
 *      SIZE non-blank characters.  If necessary, pad with
 *      the character FILL.
 *
 *
 *
 *   See Also:  
 */

#ifdef KSHELL
#include	"shtype.h"
#else
#include	<ctype.h>
#endif	/* KSHELL */

/*
 *   NAM_RJUST (STR, SIZE, FILL)
 *
 *        char *STR;
 *
 *        int SIZE;
 *
 *        char FILL;
 *
 *   Right-justify STR so that it contains no more than
 *   SIZE characters.  If STR contains fewer than SIZE
 *   characters, left-pad with FILL.  Trailing blanks
 *   in STR will be ignored.
 *
 *   If the leftmost digit in STR is not a digit, FILL
 *   will default to a blank.
 */

void	nam_rjust(str,size,fill)
char *str,fill;
int size;
{
	register int n;
	register char *cp,*sp;
	n = strlen(str);

#if (defined(NLS) | defined(KJI))
	/* trailing blanks have already been stripped; 
	 * the remainder of the routine works for 2-byte characters because
	 * any 2-byte character that may straddle a boundary has already
	 * been replaced with ascii space characters in ja_size()
	 */
	 cp = (n>1) ? str+n-1 : str;
#else
	/* ignore trailing blanks */

	for(cp=str+n;n && *--cp == ' ';n--);
#endif
	if (n == size) return;
	if(n > size)
        {
        	*(str+n) = 0;
        	for (sp = str, cp = str+n-size; sp <= str+size; *sp++ = *cp++);
        	return;
        }
	else *(sp = str+size) = 0;
	if (n == 0)  
        {
        	while (sp > str)
               		*--sp = ' ';
        	return;
        }
	while(n--)
		*--sp = *cp--;
	if(!isdigit(*str))
		fill = ' ';
	while(sp>str)
		*--sp = fill;
	return;
}

