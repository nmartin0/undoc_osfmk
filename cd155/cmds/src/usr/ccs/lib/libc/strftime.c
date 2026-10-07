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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: strftime.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:36 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS:  strftime
 *
 * ORIGINS: 27
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

#include <time.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <langinfo.h>
#include <stdlib.h>

/*
 * FUNCTION: Formats the date. The values returned are affected by the
 *           setting of the locale category LC_TIME
 *
 * PARAMETERS: 
 *	     char *s - location of returned string
 *	     size_t maxsize - maximum length of return string
 *	     char *format - format that date is to be printed out
 *           struct tm *timeptr - date to be printed
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns the number of bytes that comprise the return string
 *	     - returns 0 if s is longer than maxsize
 */

#define BADFORMAT  { format=fstart; \
		   bufp = "%"; }

#define BUFSIZE    100
#define WIDTH(a)   (wpflag ? 0  : (a))
#define PUT(c)     (strp <= strend ? *strp++  = (c) : toolong++)

/* codes used by getformat(), doformat() */
#define TWELVEHOUR 1
#define NOYEAR     2
#define NOSECS     3
#define SKIP       for(strp=lastf; (i = *format) && i != '%'; format++ )

static char *gettimezone(struct tm *timeptr);
static char *gettok(char *t, char *s,int n, char *sep);

#ifdef	_THREAD_SAFE
static size_t doformat(char *s, size_t maxsize, char *format,
			 struct tm *timeptr, int code, char *buffer,
			 char **pbp, int *level);
static char *getformat(char *id, struct tm *timeptr, int code,
			 char *buffer, char **pbp, int *level);
static char *getnum(int i, int n, char *buffer);
static char *getshowa(char *id, struct tm *tmdate, char *buffer);

#define	DOFORMAT(a,b,c,d,e)	doformat(a, b, c, d, e, buffer, pbp, level)
#define	GETFORMAT(a,b,c)	getformat(a, b, c, buffer, pbp, level)
#define	GETNUM(a,b)		getnum(a, b, buffer)
#define	GETSHOWA(a,b)		getshowa(a, b, buffer)
#define BUFSIZE_R		512
#define	NL_LANGINFO(a)          (nl_langinfo_r(a, nlbuf, BUFSIZE_R), nlbuf)

#else
static size_t doformat(char *s, size_t maxsize, char *format,
		       struct tm *timeptr, int code);
static char *getformat(char *id, struct tm *timeptr, int code);
static char *getnum( int i, int n);
static char *getshowa(char *id, struct tm *tmdate);

#define	DOFORMAT(a,b,c,d,e)	doformat(a, b, c, d, e)
#define	GETFORMAT(a,b,c)	getformat(a, b, c)
#define	GETNUM(a,b)		getnum(a, b)
#define	GETSHOWA(a,b)		getshowa(a, b)
#define	NL_LANGINFO(a)		nl_langinfo(a)

/* global variables */
static char *bufp;
static char buffer[BUFSIZE];
static int  level = 0;
#endif

static int strftime_day[7] = {DAY_1, DAY_2, DAY_3, DAY_4, DAY_5, DAY_6, DAY_7};
static int strftime_aday[7] = {ABDAY_1, ABDAY_2, ABDAY_3, ABDAY_4, ABDAY_5, ABDAY_6, ABDAY_7};
static int strftime_mon[12] = {MON_1, MON_2, MON_3, MON_4, MON_5, MON_6, MON_7, MON_8, MON_9, MON_10, MON_11, MON_12};
static int strftime_amon[12] = {ABMON_1, ABMON_2, ABMON_3, ABMON_4, ABMON_5, ABMON_6, ABMON_7, ABMON_8, ABMON_9, ABMON_10, ABMON_11, ABMON_12};

extern char *NLgetenv();

size_t 
strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr)
{
#ifdef	_THREAD_SAFE
	char	buffer[BUFSIZE], *bp;
	char	**pbp = &bp;
	int	_level = 0;
	int	*level = &_level;
#endif

	return(DOFORMAT(s, maxsize, (char *)format, (struct tm *) timeptr, 0));
}
char *
NLstrtime(char *s, size_t maxsize, const char *format, const struct tm *timeptr)
{
#ifdef	_THREAD_SAFE
	char	buffer[BUFSIZE], *bp;
	char	**pbp = &bp;
	int	_level = 0;
	int	*level = &_level;
#endif

	(int)DOFORMAT(s, maxsize, (char *)format, (struct tm *)timeptr, 0);
	return(s);
}

static size_t
#ifdef	_THREAD_SAFE
doformat(char *s, size_t maxsize, char *format,
	   struct tm *timeptr, int code, char *buffer, char **pbp, int *level)
#else
doformat(char *s, size_t maxsize, char *format, struct tm *timeptr, int code)
#endif
{
    int i;
    int firstday; /* first day of the year */
    int toolong = 0;
    int weekno, wmod;
    char *strp;  /* pointer into output buffer str */
    char *fstart;         /* where format started in case its invalid */
    char *strend;/* last available byte in output buffer str */
    char *lastf; /* last byte of str made by a format */
    int width;            /* width in current format or 0 */
    int prec;             /* precision in current format or 0 */
    int wpflag;           /* true if width or precision specified */
    char locbuffer[BUFSIZE]; /* local temporary buffer */
#ifdef	_THREAD_SAFE
#define	bufp (*pbp)
    char nlbuf[BUFSIZE_R];	/* buffer for nl_langinfo_r() */
#endif

    lastf = strp = s;
    strend = s+maxsize-1;
    while (i = *format++) {
	if (i != '%')
	    PUT(i);
	else {
	    wpflag = width = prec = 0;
	    fstart = format;
	    /* get field width & precision */
	    i = 0;
	    if (*format == '-') {
		i++;
		format++;
		wpflag++;
	    }
	    while(isdigit(*format)) {
		width *= 10;
		width += *format++ - '0';
		wpflag++;
	    }
	    if (i)
		width = -width;
	    if (*format == '.') {
		wpflag++;
		format++;
		while(isdigit(*format)) {
		    prec *= 10;
		    prec += *format++ - '0';
		}
	    }
	    switch(*format++) {
	    /* X/Open - percent sign */
	    case '%': bufp = "%";
		      break;
	    /* X/Open - newline character */
	    case 'n': bufp = "\n";
		      break;
	    /* X/Open - tab character */
	    case 't': bufp = "\t";
		      break;
	    /* X/Open - the month as a decimal number */
	    case 'm': bufp = GETNUM(timeptr->tm_mon+1,WIDTH(2));
		      break;
	    /* X/Open - the day of the month as a decimal number */
	    case 'd': bufp = GETNUM(timeptr->tm_mday,WIDTH(2));
		      break;
	    /* X/Open - the year without century as a decimal number */
	    case 'y': if (code==NOYEAR)
			     SKIP;
		      else
			     bufp = GETNUM(timeptr->tm_year,WIDTH(2));
		      break;
	    /* X/Open - hour (24-hour clock) as a decimal number */
	    case 'H': i = timeptr->tm_hour;
		      if (i>12 && code==TWELVEHOUR) i -= 12;
		      bufp = GETNUM(i,WIDTH(2));
		      break;
	    /* X/Open - the minute as a decimal number */
	    case 'M': bufp = GETNUM(timeptr->tm_min,WIDTH(2));
		      break;
	    /* X/Open - the second as a decimal number */
	    case 'S': if (code==NOSECS)
			     SKIP;
		      else
			     bufp = GETNUM(timeptr->tm_sec,WIDTH(2));
		      break;
	    /* X/Open - the day of the year as a decimal number */
	    case 'j': bufp = GETNUM(timeptr->tm_yday+1,WIDTH(3));
		      break;
	    /* X/Open - the weekday as a decimal number */
	    case 'w': bufp = GETNUM(timeptr->tm_wday,WIDTH(1));
		      break;
	    /* X/Open - the time in AM/PM notation */
	    case 'r': 
		      DOFORMAT(locbuffer,BUFSIZE,"%H:%M:%S",timeptr,TWELVEHOUR);
		      if (timeptr->tm_hour<12) 
		          strcat(locbuffer," AM");
		      else  
		          strcat(locbuffer," PM");
		      bufp = locbuffer;
		      break;

	    /* X/Open - the time in %H:%M:%S notation */
	    case 'T':
		      DOFORMAT(locbuffer, BUFSIZE, "%H:%M:%S", timeptr, 0);
	      	      strcpy(buffer,locbuffer);
		      bufp = buffer;
		      break;
	    /* X/Open - the locale time notation */
	    case 'X': bufp = GETFORMAT("NLTIME",timeptr,0);
	      	      if (!bufp || !*bufp) {
			  BADFORMAT;
		      }
		      break;
	    /* IBM-long day name, long month name, locale date representation*/
	    case 'l': switch (*format++) {
		      case 'a': bufp=NL_LANGINFO(strftime_day[timeptr->tm_wday]);
				break;
		      case 'h': bufp=NL_LANGINFO(strftime_mon[timeptr->tm_mon]);
				break;
		      case 'D': bufp = GETFORMAT("NLLDATE", timeptr,0);
				break;
		      default : BADFORMAT;
		      }
		      break;
	    /* IBM-hour(12 hour clock), long date w/o year, long time w/o secs*/
	    case 's': switch (*format++) {
		      case 'H': i=timeptr->tm_hour;
				bufp=GETNUM(i>12?i-12:i,WIDTH(2));
				break;
		      case 'D': bufp=GETFORMAT("NLLDATE",timeptr,NOYEAR);
				break;
		      case 'T': bufp = GETFORMAT("NLTIME", timeptr,NOSECS);
				break;
		      default : BADFORMAT;
		      }
		      break;
	    /* X/Open - locale's abbreviated weekday name */
	    case 'a': bufp = NL_LANGINFO(strftime_aday[timeptr->tm_wday]);
		      break;
	    /* X/Open - locale's abbreviated month name */
	    case 'h':
	    case 'b': bufp = NL_LANGINFO(strftime_amon[timeptr->tm_mon]);
		      break;
	    /* X/Open - locale's equivalent AM/PM */
	    case 'p': bufp = NL_LANGINFO((timeptr->tm_hour<12) ? AM_STR : PM_STR);
		      break;
	    /* X/Open - year with century as a decimal number */
	    case 'Y': if (code==NOYEAR)
			     SKIP;
		      else
			     bufp = GETNUM(timeptr->tm_year+1900,WIDTH(4));
		      break;
	    /* IBM - timezone name if it exists */
	    case 'z':
	    /* X/Open - timezone name if it exists */
	    case 'Z': bufp = gettimezone(timeptr);
		      break;
	    /* X/Open - locale's full weekday name */
	    case 'A': bufp = NL_LANGINFO(strftime_day[timeptr->tm_wday]);
		      break;
	    /* X/Open - locale's full month name */
	    case 'B': bufp = NL_LANGINFO(strftime_mon[timeptr->tm_mon]);
		      break;
	    /* X/Open - hour (12-hour clock) as a decimal number */
	    case 'I': i = timeptr->tm_hour;
		      bufp = GETNUM(i>12?i-12:i?i:12,WIDTH(2));
		      break;
	    /* X/Open - date in %m/%d/%y format */
	    case 'D':
		      DOFORMAT(locbuffer, BUFSIZE, "%m/%d/%y", timeptr, 0);
	      	      strcpy(buffer,locbuffer);
		      bufp = buffer;
		      break;
	    /* X/Open - locale's appropriate date representation */
	    case 'x': bufp = GETFORMAT("NLDATE", timeptr,0);
		      break;
	    /* X/Open - locale's appropriate date and time representation */
	    case 'c': bufp = GETFORMAT("NLDATIM", timeptr,0);
		      break;
	    /* X/Open - week number of the year (Sunday as the first day) */
	    case 'U': firstday=(timeptr->tm_wday-(timeptr->tm_yday % 7) + 7 ) % 7;
		      weekno = (timeptr->tm_yday + firstday + 1) / 7;
		      wmod = (timeptr->tm_yday + firstday + 1) % 7;
		      if (wmod)
		      	  bufp = GETNUM(weekno,WIDTH(2));
		      else
		          bufp = GETNUM(weekno-1,WIDTH(2));
		      break;
	    /* X/Open - week number of the year (Monday as the first day) */
	    case 'W': firstday=(timeptr->tm_wday-(timeptr->tm_yday % 7) + 7 ) % 7;
		      weekno = (timeptr->tm_yday + firstday) / 7;
		      wmod = (timeptr->tm_yday + firstday) % 7;
		      if (firstday == 0)
		          if (wmod)
		      	    bufp = GETNUM(weekno+1,WIDTH(2));
		        else
		            bufp = GETNUM(weekno,WIDTH(2));
		      else
		          if (wmod)
		      	    bufp = GETNUM(weekno,WIDTH(2));
		        else
		            bufp = GETNUM(weekno-1,WIDTH(2));
		      break;

		/* This is the additional code to support non-Christian
		   eras. The formatter %Jy will display the relative
		   year from the relevant era entry in NLYEAR, %Js will
		   display the era name.
		 */
	    /* IBM - era and year of the Emperor */
	    case 'J': bufp = GETSHOWA("NLYEAR", timeptr);
		      if (!bufp || (strncmp(bufp,"?",1)==0)) {
			  format=fstart;
			  bufp = "%";
		      }
		      else {
			  if ((bufp = gettok(locbuffer,buffer,0,",")) == (char *) 0) {
			      format=fstart;
			      bufp = "%";
			  }
			  else {
			      switch(*format++) {
			      case 'y': break;
			      case 's': if ((bufp = gettok(locbuffer,buffer,1,",")) == (char *) 0){
					    format=fstart;
					    bufp = "%";
					}
					break;
			      default:  break;
			      }
			 }
		      }
		      break;

	    default:
	    /* badformat */
		      BADFORMAT;
		      break;
	    } /* switch */

	if(!bufp)	/* BADFORMAT safety */
	{
	  BADFORMAT;
	}
	/* output bufp with appropriate padding */
	i = strlen(bufp);
	if (prec && prec<i) {
	    /* truncate on right */
	    * (bufp + prec) = '\0' ;
	    i = prec;
	}
	if (width>0)
	    while(!toolong && i++ < width)
		PUT(' ');
	while(!toolong && *bufp)
		PUT(*bufp++);
	if (width<0)
	    while(!toolong && i++ < -width)
		PUT(' ');
	lastf = strp;
      } /* i == '%' */
      if (toolong)
	break;
    }
    *strp = 0;
    if ((toolong) || (strlen(s)+1 > maxsize))
	return(0);
    else
        return (strlen(s));
}

static char *
#ifdef	_THREAD_SAFE
getnum( int i, int n, char *buffer)
#else
getnum( int i, int n)
#endif
{
	char *s = buffer;
	s += n ? n : 19;
	*s = 0;
	while(s>buffer) {
	    if (i==0 && n==0) break;
	    *--s = (i%10)+'0';
	    i /= 10;
	}
	return s;
}

static char *
#ifdef	_THREAD_SAFE
getformat(char *id, struct tm *timeptr, int code,
	    char *buffer, char **pbp, int *level)
#else
getformat(char *id, struct tm *timeptr, int code)
#endif
{
	char locbuffer[BUFSIZE];
	char *f = NLgetenv(id);

	/* 
	**	Check for recursive format: 
	** 		e.g. "NLDATE=*%x" but also "NLDATE=*%X" 
	*/

	if(!f || !*f) {
	  /*
	   * No value for this locale!  Switch to C locale and try again
	   */
	  char *old = setlocale(LC_TIME, NULL);	/* Gets current settings */

	  setlocale(LC_TIME, "C");		/* Back to default */
	  f = NLgetenv(id);			/* We're in REAL trouble if this fails */
	  setlocale(LC_TIME, old );
	}

#ifdef	_THREAD_SAFE
	if(*level) {
		*pbp = f;
		return 0;
	}
	*level++;
	doformat(locbuffer, BUFSIZE, f, timeptr, code, buffer, pbp, level);
	*level--;
#else
	if (level) {
		bufp = f;
		return 0;
	}
	level++;
	doformat(locbuffer, BUFSIZE, f, timeptr, code);
	level--;
#endif
	strcpy(buffer,locbuffer);
	return buffer;
}

static char *
gettimezone(struct tm *timeptr)
{
	tzset();
	if (daylight && timeptr->tm_isdst)
		return tzname[1];
	else
		return tzname[0];
}


static char *
#ifdef	_THREAD_SAFE
getshowa(char *id, struct tm *tmdate, char *buffer)
#else
getshowa(char *id, struct tm *tmdate)
#endif
{
     /* This routine is used in determining non-Christian eras
        (typically the Emperors Era for Japan.
	This routine examines the contents of the environment 
	variable NLYEAR, which must be of the following format:
	NLYEAR="CCYYMMDD,PERIOD_NAME:CCYYMMDD,PERIOD_NAME:..."
	with the latest period start first. The comma is mandatory,
	but the string can be null.  If an entry is found
	such that the date in tmtime is later than the entry in
	NLYEAR, then the year of that entry will be subtracted from
	the tmtime->tm_year (+ 1). Negative years means B.C.
	For instance: If NLYEAR="19230101,SHOWA" then 1989 will be "67"; 
	if NLYEAR="-07530101,A.U.C." then 1989 will
	be 2743 ab urbe condida, the founding of Rome.
      */
	register int n, n1, n2, n3, i=0;
	char *s1, *s2, *s3;

	s1 = NLgetenv(id);
	if (!s1 || !*s1)
		return("?");
	s3 = (char *) malloc(strlen(s1) + 1);
	s3 = gettok(s3, s1, i++,":");  
	if (s3 == 0)
		s3 = s1;
	do {
	    n3 = 0;
	    s2 = s3;
	    if (strchr(s2, ',') == 0) {
		free (s3);
		return ("?");
	    }
	    if (strncmp(s2, "-", 1) == 0) {
		if (strlen(s2) < 10) {
			free (s3);
			return("?");
		}
		++s2;
		n3 = 1;
    	    } else if (strlen(s2) < 9) {
			free (s3);
			return("?");
		}
	    for (n = 0, n1 = 0; n1 < 4; n1++)
		n = n*10 + *s2++ - '0';
	    if (n3)	n = -n;	
	    n -= 1900;
	    if (n > tmdate->tm_year)
		continue;
	    n2 = tmdate->tm_year - n + 1;
	    if (n2 > 1 ) {
		s2 += 4;
		break;
	    }
	    for (n = 0, n1 = 0; n1 < 2; n1++)
		n = n*10 + *s2++ - '0';
	    if (n < tmdate->tm_mon+1) {
		s2 += 2;
		break;
	    }
	    if (n > tmdate->tm_mon+1) 
		continue;
	    for (n = 0, n1 = 0; n1 < 2; n1++)
		n = n*10 + *s2++ - '0';
	    if (n < tmdate->tm_mday)
		break;
	    if (n > tmdate->tm_mday)
		continue;
	}
	while (s3 = gettok(s3,s1, i++,":"));
	if (s3 == 0) {
		free (s3);
		return("?");
	}
	while (n2 > 0) 
	{
		*--s2 = (n2%10)+'0';
		n2 /= 10;
	}
	free(s3);
	strcpy(buffer, s2);
	return(buffer);
}

static char *
gettok(t,s,n,sep)
char *t;
char *s;
int n;
char *sep;
{
	char *p; /* to hold the beginning address of the t  */
	int i;   /* index */
	p=t;
	/* skip the separator by the offset */
	for (i=0; i<n; i++) {
		while( (*s) && (*s != '\n') ){
			/* End of field is when we see an unescaped separator */
			if( (*s == *sep) && (*(s-1) != '\\'))
				break;	/* Colon separator got */
			s++;
		}
		s++;	/* Get next field */
	}

	/* Beginning of the field at this point */

	 while (*s != '\n' && *s != '\0'){
                if ( *s == *sep) 	
			if ( *(s-1) == '\\') 
				--p;	/* Overwrite escape */
                	else 
                        	break; /* Colon sep got */

                *p++ = *s++;	
        }
	
	*p ='\0';
	return(t);
}
