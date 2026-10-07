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
static char sccsid[] = "@(#)util.c	1.4  com/lib/bsd,3.1,8943 9/7/89 16:12:32";
/*
 * COMPONENT_NAME: (LIBBSD)  Berkeley Compatibility Library
 *
 * FUNCTIONS: move, xalloc, fatal, xfree, mcan, itom, mcmp, xtoi, xtom, itox,
 *	      mtox, mfree 
 *
 * ORIGINS: 26 27
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
 * "@(#)util.c 1.2 88/05/16 4.0NFSSRC SMI; from 5.1 (Berkeley) 4/30/85";
 */

char *malloc();
void mfree();
#ifdef lint
int xv_oid;
#endif
#include <stdio.h>
#include "mp.h"


/*
 * Convert hex digit to binary value
 */
static int
xtoi(c)
	char c;
{
	if (c >= '0' && c <= '9') {
		return(c - '0');
	} else if (c >= 'a' && c <= 'f') {
		return(c - 'a' + 10);
	} else {
		return(-1);
	}
}



/*
 * Convert hex key to MINT key
 */
MINT *
xtom(key)
	char *key;
{	
	int digit;
	MINT *m = itom(0);
	MINT *d;
	MINT *sixteen;

	sixteen = itom(16);
	for (; *key; key++) {
		digit = xtoi(*key);
		if (digit < 0) {
			return(NULL);
		}
		d = itom(digit);
		mult(m,sixteen,m);
		madd(m,d,m);
		mfree(d);
	}
	mfree(sixteen);
	return(m);
}

static char
itox(d)
	short d;
{
	d &= 15;
	if (d < 10) {
		return('0' + d);
	} else {
		return('a' - 10 + d);
	}
}

/*
 * Convert MINT key to hex key
 */
char *
mtox(key)
	MINT *key;
{
	MINT *m = itom(0);
	MINT *zero = itom(0);
	short r;
	char *p;
	char c;
	char *s;
	char *hex;
	int size;

#	define BASEBITS	(8*sizeof(short) - 1)

	if (key->len >= 0) {
		size = key->len;
	} else {	
		size = -key->len; 
	}
	hex = malloc((unsigned) ((size * BASEBITS + 3)) / 4 + 1);
	if (hex == NULL) {
		return(NULL);
	}
	move(key,m);
	p = hex;
	do {
		sdiv(m,16,m,&r);
		*p++ = itox(r);
	} while (mcmp(m,zero) != 0);	
	mfree(m);
	mfree(zero);

	*p = 0;
	for (p--, s = hex; s < p; s++, p--) {
		c = *p;
		*p = *s;
		*s = c;
	}
	return(hex);
}

/*
 * Deallocate a multiple precision integer
 */
void
mfree(a)
	MINT *a;
{
	xfree(a);
	free((char *)a);
}
