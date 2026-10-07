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
static char rcsid[] = "@(#)$RCSfile: convert.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:22 $";
#endif
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
 *   CONVERT.C
 *
 *
 *   LTOU (STR1, STR2)
 *        Copy STR1 to STR2, changing lower case to upper case.
 *
 *   UTOL (STR1, STR2)
 *        Copy STR1 to STR2, changing upper case to lower case.
 */

#ifdef KSHELL
#include	"shtype.h"
#else
#include	<ctype.h>
#endif	/* KSHELL */

#if (defined(NLS) || defined(KJI))
#	include <NLchar.h>
#endif /* NLS|KJI */

/* 
 *   LTOU (STR1, STR2)
 *        char *STR1;
 *        char *STR2;
 *
 *   Copy STR1 to STR2, converting uppercase alphabetics to
 *   lowercase.  STR2 should be big enough to hold STR1.
 *
 *   STR1 and STR2 may point to the same place.
 *
 */

void ltou(str1,str2)
register char *str1,*str2;
{
#if (defined(NLS) || defined(KJI))
	wchar_t c;		/* scratch NLchar */
	int len1, len2;	/* length of char at *str1, *str2 */

	for(; len1 = NCdec(str1, &c), c > (wchar_t)0; str1 += len1, str2 += len2) 
	{
		if(NCislower(c))
			c = NCtoupper(c);
		len2 = NCenc(&c, str2);
	}
#else
	for(; *str1; str1++,str2++)
	{
		if(islower(*str1))
			*str2 = toupper(*str1);
		else
			*str2 = *str1;
	}
#endif	/* NLS|KJI */
	*str2 = 0;
}


/*
 *   UTOL (STR1, STR2)
 *        char *STR1;
 *        char *STR2;
 *
 *   Copy STR1 to STR2, converting lowercase alphabetics to
 *   uppercase.  STR2 should be big enough to hold STR1.
 *
 *   STR1 and STR2 may point to the same place.
 *
 */

void utol(str1,str2)
register char *str1,*str2;
{
#if (defined(NLS) || defined(KJI))
	wchar_t c;		/* scratch NLchar */
	int len1, len2;	/* length of char at *str1, *str2 */

	for(; len1 = NCdec(str1, &c), c > (wchar_t)0; str1 += len1, str2 += len2)
	{
		if(NCisupper(c))
			c = NCtolower(c);
		len2 = NCenc(&c, str2);
	}
#else
	for(; *str1; str1++,str2++)
	{
		if(isupper(*str1))
			*str2 = tolower(*str1);
		else
			*str2 = *str1;
	}
#endif /* NLS|KJI */
	*str2 = 0;
}

