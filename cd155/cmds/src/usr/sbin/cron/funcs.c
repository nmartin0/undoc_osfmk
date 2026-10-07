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
static char rcsid[] = "@(#)$RCSfile: funcs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:05:11 $";
#endif
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: funcs
 *
 * ORIGINS: 3, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 */

/*
 * module used for crontab, cron, and at
 */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <NLctype.h>
#define ISDIGIT NCisdigit


/*****************/
time_t num(ptr)
/*****************/
char **ptr;
{
	time_t n=0;
	while (ISDIGIT(**ptr)) {
		n = n*10 + (**ptr - '0');
		*ptr += 1; }
	return(n);
}


int dom[12]={31,28,31,30,31,30,31,31,30,31,30,31};

/*****************/
days_btwn(m1,d1,y1,m2,d2,y2)
/*****************/
int m1,d1,y1,m2,d2,y2;
{
	/* calculate the number of "full" days in between m1/d1/y1 and m2/d2/y2.
	   NOTE: there should not be more than a year separation in the dates.
		 also, m should be in 0 to 11, and d should be in 1 to 31.    */
	
	int days,m;

	if ((m1==m2) && (d1==d2) && (y1==y2)) return(0);
	if ((m1==m2) && (d1<d2)) return(d2-d1-1);
	/* the remaining dates are on different months */
	days = (days_in_mon(m1,y1)-d1) + (d2-1);
	m = (m1 + 1) % 12;
	while (m != m2) {
		if (m==0) y1++;
		days += days_in_mon(m,y1);
		m = (m + 1) % 12; }
	return(days);
}


/*****************/
days_in_mon(m,y)
/*****************/
int m,y;
{
	/* returns the number of days in month m of year y
	   NOTE: m should be in the range 0 to 11	*/

	return( dom[m] + (((m==1)&&((y%4)==0)) ? 1:0 ));
}

/*****************/
char *xmalloc(size)
/*****************/
int size;
{
	char *p;

	if((p=(char *) malloc((size_t)size)) == NULL) {
		printf("cannot allocate %d bytes of space\n",size);
		fflush(stdout);
		exit(55);
	}
	return p;
}

/*****************/
char *xrealloc(p, size)
/*****************/
char *p;
int size;
{

	if((p= (char *) realloc((void *)p, (size_t)size)) == NULL) {
		printf("cannot allocate %d bype of space\n",size);
		fflush(stdout);
		exit(55);
	}
	return p;
}

/*****************/
char *cat2(s1, s2)
/*****************/
char *s1, *s2;
{
    return (strcat(strcpy(xmalloc((int)(strlen(s1)+strlen(s2)+1)), s1), s2));
}

/*****************/
static int sameenv(s, t)
/*****************/
char *s, *t;
{
    while (*s != '\0' && *s != '=')
	if (*s++ != *t++) return (0);
    return (*t == '\0' || *t == '=');
}

/*****************/
/* void putenv(s)		NOT USED */
/*****************/
/* char *s;
{
    extern char **environ;
    static char **envmalloced = NULL;
    static char *nullenv[1] = { NULL };
    register char **ep;
    if (environ == NULL) environ = nullenv;
    ep = environ;
    for (ep = environ; *ep != NULL && !sameenv(*ep, s); ++ep);
    if (*ep == NULL) {
	register char **newenv;
	if (environ == envmalloced) {
	    newenv = (char **)xrealloc((char *)environ,
		    (ep-environ+2)*sizeof(char *));
	} else {
	    newenv = (char **)memcpy((void *)xmalloc((ep-environ+2)*sizeof(char *)), (void *)environ, (size_t)((ep-environ)*sizeof(char *)));
	}
	ep = newenv + (ep - environ);
	envmalloced = environ = newenv;
	ep[1] = NULL;
    }
    *ep = s;
}
*/
