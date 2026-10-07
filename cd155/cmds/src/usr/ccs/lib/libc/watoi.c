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
static char rcsid[] = "@(#)$RCSfile: watoi.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:32 $";
#endif
/*
 * LIBCGEN: watoi.c
 *
 * FUNCTIONS: watoi
 *
 * ORIGINS: 10, 27
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
/* watoi.c	1.2  com/lib/c/cnv/KJI,3.1,9021 12/14/89 13:08:46 */

/*
 * NAME: watoi
 *                                                                    
 * FUNCTION: 	SHIFT-JIS TO INTEGER
 *                                                                    
 * RETURNS:
 *	Converts the string 'str' to integer.
 */ 
#include <NLctype.h>

#ifdef KJI
int
watoi(NLchar *p)
{
	register int n, f;

	int ascp;
	n = 0;
	f = 0;
	for(;;p++) {
		switch(ascp = (isascii(*p)? *p: _jistoa(*p))) {
		case ' ':
		case '\t':
			continue;
		case '-':
			f++;
		case '+':
			p++;
			ascp = isascii(*p)? *p: _jistoa(*p);
		}
		break;
	}
	while(ascp >= '0' && ascp <= '9') {
		n = n*10 + ascp - '0';
		ascp = isascii(*(++p))? *p: _jistoa(*p);
	}
	return(f? -n: n);
}
#else /* non KJI stub */
int
watoi(register NLchar *p)
{
}
#endif /* KJI */
