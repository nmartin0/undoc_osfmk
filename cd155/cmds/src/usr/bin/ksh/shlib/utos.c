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
static char sccsid[] = "@(#)utos.c	1.6  com/cmd/ksh/shlib,3.1,9013 9/10/89 06:41:37";
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
 *   UTOS.C
 *
 *   Programmer:  D. G. Korn
 *
 *         Date:  May 3, 1982
 *
 *
 *
 *   LTOS (SINT, BASE)
 *
 *        Return a pointer to a string denoting the value of
 *        the signed long integer SINT in base BASE.
 *
 *   UTOS (USINT, BASE)
 *
 *        Return a pointer to a string denoting the value of
 *        the unsigned long integer USINT in base BASE.
 *
 *
 *
 *   See Also:  arith(III)
 */


#define BASEMAX	 (4+16*sizeof(int))
static char hexstr[BASEMAX];
extern char e_hdigits[]; /*NOTX*/
char *utos();
char *ltos();
#ifdef FLOAT
char *ftos();
char *etos();
#endif /* FLOAT */

/*
 *   LTOS (SINT, BASE)
 *
 *        long USINT;
 *
 *        int BASE;
 *
 *   Return a pointer to a string denoting the value of SINT 
 *   in base BASE.  The string will be stored within HEXSTR.
 *   It will begin with the base followed by a single '#'.
 *   A minus sign will be prepended for negative numbers
 *
 */


char *ltos(sint,base)
long sint;
int base;
{
	register char *sp;
	register long l = (sint>=0?sint:-sint);
#ifdef pdp11
	sp = utos(l,base);
#else
	sp = utos((unsigned long)l,base);
#endif /* pdp11 */
	if(sint<0)
		*--sp = '-';
	return(sp);
}

/*
 *   UTOS (USINT, BASE)
 *
 *        unsigned USINT;
 *
 *        int BASE;
 *
 *   Return a pointer to a string denoting the value of USINT 
 *   in base BASE.  The string will be stored within HEXSTR.
 *   It will begin with the base followed by a single '#'.
 *
 */



char *utos(usint,base)
register int base;
#ifdef pdp11
 /* unsigned longs are not supported on pdp11 */
long usint;
{
	long l = usint;
#else
unsigned long usint;
{
	register unsigned long l = usint;
#endif	/* pdp11 */
	register char *cp = hexstr+(BASEMAX-1);
	if(base < 2 || base > BASEMAX)
		return(cp);
	for(*cp = 0;cp > hexstr && l;l /= base)
		*--cp = e_hdigits[(l%base)<<1]; /*NOTX*/
	if(usint==0)
		*--cp = '0';
	if(base==10)
		return(cp);
	*--cp = '#';
	*--cp = e_hdigits[(base%10)<<1]; /*NOTX*/
	if(base /= 10)
		*--cp = e_hdigits[(base%10)<<1]; /*NOTX*/
	return(cp);	
}

#ifdef FLOAT
/*
 *   FTOS (VAL, PLACES)
 *
 *        double VAL;
 *
 *        int PLACES;
 *
 *   Return a pointer to a string denoting the value of VAL 
 *   with PLACES places after the decimal point.  The string
 *   will be stored within HEXSTR.
 *
 */

char *ftos(val,places)
double val;
int places;
{
	register char *sp=hexstr;
	register char *cp;
	extern char *fcvt();
	int decpt,sign;
	cp = fcvt(val,places,&decpt,&sign);
	if(sign)
		*sp++ = '-';
	while(decpt-- > 0)
		*sp++ = *cp++;
	*sp++ = '.';
	while(++decpt < 0)
		*sp++  = '0';
	while(*sp++ = *cp++);
	return(hexstr);
}

char *etos(val,places)
double val;
int places;
{
	extern char *gcvt();
	return(gcvt(val,places,hexstr));
}
#endif /* FLOAT */
