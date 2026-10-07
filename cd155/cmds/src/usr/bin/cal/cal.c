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
static char rcsid[] = "@(#)$RCSfile: cal.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:19:51 $";
#endif
/*
 * COMPONENT_NAME: (CMDMISC) miscellaneous commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * cal.c	1.10  com/cmd/misc,3.1,9008 1/22/90 11:27:50
 */

#include <locale.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <NLchar.h>
#include <time.h>
#include "cal_msg.h"

#define MOWIDTH		24		/* dimensions of printed calendars */ 
#define MODEPTH		6
#define TTLWIDTH	20
#define LNWIDTH		MOWIDTH * 3
#define STRSIZE		LNWIDTH * MODEPTH
#define MAXYR		9999

void cal(), pstr();
void filarray(), dayarray();  /* NLS support routines */
char *center();
extern char *NLgetenv();

char	dayw[] = {
	" S  M Tu  W Th  F  S"
};

char    *lmon[12]= {
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};

char	mon[13] = {               /* month ending days */
	0,
	31, 29, 31, 30,
	31, 30, 31, 31,
	30, 31, 30, 31,
};

char *days[] = {
	"Sun","Mon","Tue","Wed",
	"Thu","Fri","Sat"
};

char	string[STRSIZE];  /* days of the calendar(s) */
char    mhdr[MOWIDTH+1];  /* month title for single month calendar */

#define MSGSTR(Num, Str) NLgetamsg(MF_CAL, MS_CAL, Num, Str)
nl_catd catd;


/*
 * NAME: main
 *                                                                    
 * FUNCTION: The cal command does one of three things when invoked:
 *           - prints the current months calendar is called with no
 *	       arguments
 *	     - prints a full year calendar if called with one argument
 *	     - prints a single month calendar if called with two 
 *	       arguments
 *
 * RETURN VALUE DESCRIPTION:  0  successful completion of any of the above 
 *			     -1  bad input detected 
 */  
main(argc, argv)
int argc;
char *argv[];
{
	register int y, i, j;
	char *nlstr;
	int m;
	struct tm *ts;
	time_t now;
  
        catd = catopen(MF_CAL, 0);
        (void)setlocale(LC_ALL, ""); 
   
	/* print this months calendar when invoked without additional args */
	/* (CASE 1) */
	if(argc > 3) {
		fprintf(stderr,MSGSTR(USAGE, "usage: cal [[month] year]\n"));
		exit(-1);
	}

	/* load title arrays with alternative language captions if specified */
	/* NLLMONTH and NLSDAY will always be set by setlocate() */
	if ((nlstr = NLgetenv("NLLMONTH")) != NULL && *nlstr)
	    filarray(nlstr, lmon, 12);
	if ((nlstr = NLgetenv("NLSDAY")) != NULL && *nlstr)
	     dayarray(nlstr);

/*
 *	print out this month
 */
	if (argc == 1) {
	   now = time(0);
	   ts = localtime(&now);
	   y = ts->tm_year + 1900;
	   m = ts->tm_mon + 1;
	   sprintf(mhdr, "%s %u",lmon[m-1], y);
	   printf("%s\n", center(mhdr, TTLWIDTH));
	   printf("%s\n", dayw);

	   cal(m, y, string, MOWIDTH);
	   for(i=0; i<MODEPTH*MOWIDTH; i+=MOWIDTH)
		pstr(string+i, MOWIDTH);
	   exit(0);
	}

/*	
 *	print out complete year
 *	(CASE 2)
 */

	if(argc == 2) {
	   y = number(argv[1]);
	   if(y<1 || y>MAXYR)
		{
			fprintf(stderr,MSGSTR(BADYEARARG, "Bad argument\n"));
			exit(-1);
		}
	   printf("\n\n\n");
           printf("				%u\n", y);
           printf("\n");
	   for(i=0; i<12; i+=3) {
		for(j=0; j<STRSIZE; j++)
			string[j] = '\0';

                /* print the month names, three across each line */
		printf("%s   ", center(lmon[i],TTLWIDTH));
		printf("%s   ", center(lmon[i+1],TTLWIDTH));
		printf("%s\n", center(lmon[i+2],TTLWIDTH));
		printf("%s   %s   %s\n", dayw, dayw, dayw);

		cal(i+1, y, string, LNWIDTH);
		cal(i+2, y, string+MOWIDTH-1, LNWIDTH);
		cal(i+3, y, string+(MOWIDTH-1)*2, LNWIDTH);

		/* print out the rows and columns of month days */
		for(j=0; j<STRSIZE; j+=LNWIDTH)
			pstr(string+j, LNWIDTH);
	   }
           printf("\n\n\n");
	   exit(0);
	}
/*	   
 *	   print out just month
 *	   (CASE 3)
 */
	   m = number(argv[1]);
	   if(m<1 || m>12)
		{
			fprintf(stderr,MSGSTR(BADMONARG, "Bad argument\n"));
			exit(-1);
		}
	   y = number(argv[2]);
	   if(y<1 || y>MAXYR)
		{
			fprintf(stderr,MSGSTR(BADYEARARG, "Bad argument\n"));
			exit(-1);
		}
	   sprintf(mhdr, "%s %u",lmon[m-1], y);
	   printf("%s\n", center(mhdr, TTLWIDTH));
	   printf("%s\n", dayw);

	   cal(m, y, string, MOWIDTH);
	   for(i=0; i<MODEPTH*MOWIDTH; i+=MOWIDTH)
		pstr(string+i, MOWIDTH);
	   exit(0);
}  

 

/*
 * NAME: number
 *                                                                    
 * FUNCTION: converts a string of numbers to its numeric value
 *
 * RETURN VALUE DESCRIPTION:  the numeric value of the string if its characters
 *			      are 0 - 9, zero otherwise
 */  
int number(str)
char *str;
{
	register int n, c;
	register char *s;

	n = 0;
	s = str;
	while(c = *s++) {
		if(c<'0' || c>'9')
			return(0);
		n = n*10 + c-'0';
	}
	return(n);
}


/*
 * NAME: pstr
 *                                                                    
 * FUNCTION:  prints the given string to stdout, after replacing nulls with
 *            blanks
 *                                                                    
 * RETURN VALUE DESCRIPTION: NONE
 *
 */  
void  pstr(str, n)           /* void * declaration causes clash with str */
char *str;
int n;
{
	register int i;
	register char *s;

	s = str;
	i = n;
	while(i--)
		if(*s++ == '\0')
			s[-1] = ' ';         /* replace the null */
	i = n+1;
	while(i--)
		if(*--s != ' ')
			break;
	s[1] = '\0';
	printf("%s\n", str);
}




/*
 * NAME: cal
 *                                                                    
 * FUNCTION: fills an array with the days of the given month 
 *                                                                    
 * RETURN VALUE DESCRIPTION: NONE 
 *			     
 */  
void cal(m, y, p, w)        
int m,y,w;      /* month, year, offset to the next week of current month */
char *p;	/* start addr of the array to be loaded with calendar days */ 
{
	register int d, i; /* day of the week, counter */
	register char *s;  /* current pointer position in the array of days */

	s = p;
	d = jan1(y);
	mon[2] = 29;
	mon[9] = 30;

	switch((jan1(y+1)+7-d)%7) {

	/*
	 *	non-leap year
	 */
	case 1:
		mon[2] = 28;
		break;

	/*
	 *	1752
	 */
	default:
		mon[9] = 19;
		break;

	/*
	 *	leap year
	 */
	case 2:
		;
	}
	for(i=1; i<m; i++) {
		d += mon[i];    
		}
	d %= 7;		
	s += 3*d;
	for(i=1; i<=mon[m]; i++) {
		if(i==3 && mon[m]==19) {
			i += 11;
			mon[m] += 11;
		}
		if(i > 9)
			*s = i/10+'0';   /* load the 10's digit */
		s++;
		*s++ = i%10+'0';         /* load the  1's digit */
		s++;
		if(++d == 7) {
			d = 0;
			s = p+w;   /* advance to load the next week */         
			p = s;
		}
	}
}


/*
 * NAME: jan1
 *                                                                    
 * FUNCTION: returns the day of the week of Jan 1 of the given year
 *                                                                    
 * RETURN VALUE DESCRIPTION:  0 through 6 correspond to Sunday through Saturday 
 *			     
 */  

int jan1(yr)
int yr;
{
	register int y, d;

/*
 *	normal gregorian calendar has
 *	one extra day per four years
 */

	y = yr;
	d = 4+y+(y+3)/4;

/*
 *	julian calendar is the
 *	regular gregorian
 *	less three days per 400
 */

	if(y > 1800) {
		d -= (y-1701)/100;
		d += (y-1601)/400;
	}

/*
 *	great calendar changeover instant
 */

	if(y > 1752)
		d += 3;

	return(d%7);
}



/*
 * NAME: filarray
 *                                                                    
 * FUNCTION: Sets up array t to reference the contents of s2
 *
 * RETURN VALUE DESCRIPTION:  NONE
 *			     
 */  
void filarray(s2,t,n)
char *s2, **t;   /* source string, destination array */                   
register int n;  /* number of sub-arrays in t */
{
	register char *s1;
	register int i;

	if ((s1 = (char *)malloc(strlen(s2)+1)) != NULL) {
		(void)strcpy(s1, s2);
		for (i = 0; i < n; ++i) {
			t[i] = s1;
			s2 = strchr(s1, ':');
			if (s2 != NULL) {
				*s2++ = '\0';
				s1 = s2;
			}
		}
	}
}

/*
 * NAME: dayarray
 *                                                                    
 * FUNCTION:  replaces day of the week captions with alternative language
 *            text
 *                                                                    
 * RETURN VALUE DESCRIPTION: NONE
 *			     
 */  
void dayarray(s)
char *s;     /* address of the alternative language caption text for 
                day of the week */
{
	register int i;

	filarray(s, days, 7);
	for(i = 0; i < 7; i++) {
		if (strlen(days[i]) == 1 ||
		(!NCisshift(days[i][0]) && NCisshift(days[i][1]))) {
			dayw[(i*3)]=' ';
			dayw[(i*3)+1]=days[i][0];
		}
		else {
			dayw[(i*3)]=days[i][0];
			dayw[(i*3)+1]=days[i][1];
		}
	} /* end of for loop */
}


/*
 * NAME: center
 *
 * FUNCTION: centers string s in a field of length len
 *
 * RETURN VALUE DESCRIPTION: NONE 
 *
 */
char *center(s, len)
   char *s; int len; 
{
	int i, slen;
	static char buf[MOWIDTH+1];

	slen = strlen(s);
	if (slen > len)
		NLstrncpy((unsigned char *)buf, (unsigned char *)s, len);
	else {
		for(i=0;i<((len-slen)/2)+((len-slen)%2);i++) buf[i]=' ';
		NLstrcpy(&buf[i], s);
		for(i=strlen(buf);i<len;i++) buf[i]=' ';
	}
	buf[strlen(buf)]='\0';
	return(buf);
}
