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
 
%{
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: att1.y,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:54 $";
#endif
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: 
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
 * att1.y 1.8  com/cmd/oper/cron,3.1,9021 12/22/89 13:04:42 
 */


#include <stdio.h>
#include <time.h>
#include <sys/dir.h>
#include "att1.h"

#include <NLctype.h>

extern	int	gmtflag;
static char * getfield();
int timedon = 0;
int daydon = 0;
int yeartm = 0;
extern	int	mday[];
extern	struct	tm *tp, at, rt;

#include "cron_msg.h"
extern nl_catd catd;
#define	MSGSTR(Num,Str) catgets(catd,MS_CRON,Num,Str)
%}
%token	TIME
%token	NOW
%token	NOON
%token	N
%token	MIDNIGHT
%token	M
%token	MINUTE
%token	HOUR
%token	DAY
%token	WEEK
%token	MONTH
%token	YEAR
%token	UNIT
%token	SUFF
%token	AM
%token	A
%token	PM
%token	P
%token	ZULU
%token	NEXT
%token	NUMB
%token	COLON
%token	COMMA
%token	PLUS
%token	DAYN
%token	YEARN
%token	UNKNOWN
%%

args
	: time date incr {
		if (at.tm_min >= 60 || at.tm_hour >= 24)
			atabort(MSGSTR(MS_BTIME,"bad time"));
		if (at.tm_year >= 100)
			at.tm_year -= 1900;
		if (at.tm_year < 70 || at.tm_year >= 138)    /* need valid year */
			atabort(MSGSTR(MS_BYEAR,"bad year"));
		if (leap(at.tm_year))                        /* to check for leap year */
			mday[1]=29;
		if (at.tm_mon >= 12 || at.tm_mday > mday[at.tm_mon])  /* to check for day */
			atabort(MSGSTR(MS_BDATE,"bad date"));
		return;
	}
	| time date incr UNKNOWN {
		yyerror();
	}
	;

time
	: hour opt_suff {
	timedon = 1;   
	checksuff:
		at.tm_hour = $1;
		switch ($2) {
		case P:
		case PM:
			if (at.tm_hour < 1 || at.tm_hour > 12)
				atabort(MSGSTR(MS_BHOUR,"bad hour"));
			at.tm_hour %= 12;
			at.tm_hour += 12;
			break;
		case A:
		case AM:
			if (at.tm_hour < 1 || at.tm_hour > 12)
				atabort(MSGSTR(MS_BHOUR,"bad hour"));
			at.tm_hour %= 12;
			break;
		case ZULU:
			if (at.tm_hour == 24 && at.tm_min != 0)
				atabort(MSGSTR(MS_BTIME,"bad time"));
			at.tm_hour %= 24;
			gmtflag = 1;
		}
	}
	| hour COLON minute opt_suff {
		torder(&$1, &$3);
		timedon = 1;   
		at.tm_min = $3;
		$3 = $1;
		goto checksuff;
	}
	| hour minute opt_suff {
		torder(&$1, &$2);
		timedon = 1;   
		at.tm_min = $2;
		$2 = $1;
		goto checksuff;
	}
	| TIME {
		timedon = 1;   
		switch ($1) {
		case NOON:
			at.tm_hour = 12;
			break;
		case MIDNIGHT:
			at.tm_hour = 0;
			break;
		case NOW:
			at.tm_hour = tp->tm_hour;
			at.tm_min = tp->tm_min;
			at.tm_sec = tp->tm_sec;
			break;
		}
	}
	;

date
	: /*empty*/ {
		at.tm_mday = tp->tm_mday;
		at.tm_mon = tp->tm_mon;
		at.tm_year = tp->tm_year;
		if ((at.tm_hour < tp->tm_hour)
			|| ((at.tm_hour==tp->tm_hour)&&(at.tm_min<tp->tm_min)))
			rt.tm_mday++;
	}
	| dayn month {
		at.tm_mon = $2;
		at.tm_mday = $1;
		at.tm_year = tp->tm_year;
		if (at.tm_mon < tp->tm_mon)
			at.tm_year++;
	}
	| month dayn {
		at.tm_mon = $1;
		at.tm_mday = $2;
		at.tm_year = tp->tm_year;
		if (at.tm_mon < tp->tm_mon)
			at.tm_year++;
	}
	| dayn month COMMA year {
		at.tm_mon = $2;
		at.tm_mday = $1;
		at.tm_year = $4;
	}
	| month dayn COMMA year {
		at.tm_mon = $1;
		at.tm_mday = $2;
		at.tm_year = $4;
	}
	| DAY {
		at.tm_mon = tp->tm_mon;
		at.tm_mday = tp->tm_mday;
		at.tm_year = tp->tm_year;
		if ($1 < 7) {
			rt.tm_mday = $1 - tp->tm_wday;
			if (rt.tm_mday < 0)
				rt.tm_mday += 7;
		}
		else if ($1 == 8)
			rt.tm_mday += 1;
	}
	;

incr
	: /*empty*/
	| NEXT UNIT	{ addincr:
		switch ($2) {
		case MINUTE:
			rt.tm_min += $1;
			break;
		case HOUR:
			rt.tm_hour += $1;
			break;
		case DAY:
			rt.tm_mday += $1;
			break;
		case WEEK:
			rt.tm_mday += $1 * 7;
			break;
		case MONTH:
			rt.tm_mon += $1;
			break;
		case YEAR:
			rt.tm_year += $1;
			break;
		}
	}
	| PLUS opt_number UNIT { goto addincr; }
	;

dayn
	: DAYN {
			daydon = 1;
			$$ = $1; }
	;

year
	: number	{ $$ = $1; }
	| number number	{ $$ = 100 * $1 + $2; }
	;

hour
	: NUMB		{ $$ = $1; }
	;
minute
	: NUMB		{ 
			timedon = 1;
			$$ = $1; }
month
	: MONTH	        { 
			timedon = 2;  
			$$ = $1; 
			}
	;
opt_number
	: /* empty */	{ $$ = 1; }
	| number	{ $$ = $1; }
	;
number
	: NUMB		{ $$ = $1; }
	| number NUMB	{ $$ = 10 * $1 + $2; }
	;
opt_suff
	: /* empty */	{ $$ = 0; }
	| SUFF		{ $$ = $1; }
	;

%%
torder(h, m)
int *h, *m;
{
	char timeord[20];
 	char *getenv();
	int i, tmp;

	if (getenv("NLTIME") != NULL) {
		strcpy(timeord, getenv("NLTIME"));
		for (i=0; timeord[i] != '\0'; i++) {
			if (timeord[i] == 'h' || timeord[i] == 'H')
				break;
			if (timeord[i] == 'm' || timeord[i] == 'M') {
				tmp = *m;
				*m = *h;
				*h = tmp;
				break;
			}
		}
	}
}


int dayfirst()
{
	char dateord[20];
 	char *getenv();
	int i, tmp;

	if (getenv("NLDATE") != NULL) {
		strcpy(dateord, getenv("NLDATE"));
		for (i=0; dateord[i] != '\0'; i++) {
			if (dateord[i] == 'm' || dateord[i] == 'M') {
				return(0);
			}
			if (dateord[i] == 'd' || dateord[i] == 'D') {
				return(1);
			}
		}
	}
	return(0);
}

#define	LL(t, v)	return(yylval = v, t)
#define EQ(s1, s2)	(strcmp(s1, s2) == 0)
#define EQN(s1, s2, n)	(strncmp(s1, s2, n) == 0)

#undef getc
#define	getc()		(*argp ? *argp++ : EOF)

char	*argp = "";
int i;

char mon[12][8] = {
			"jan",
		   	"feb",
			"mar",
			"apr",
			"may",
			"jun",
			"jul",
			"aug",
			"sep",
			"oct",
			"nov",
			"dec"
			};
char lgmon[12][26] = {
			"january",
			"february",
			"march",
			"april",
			"may",
			"june",
			"july",
			"august",
			"september",
			"october",
			"november",
			"december"
			};
char days[7][8] = {
			"sun",
			"mon",
			"tue",
			"wed",
			"thu",
			"fri",
			"sat"
			};
char lgdays[7][26]  =  {
			"sunday",
			"monday",
			"tuesday",
			"wednesday",
			"thursday",
			"friday",
			"saturday"
			};
char rest[30][26] = {
			"am",
			"pm",
			"zulu",
			"now",
			"yesterday",
			"tomorrow",
			"noon",
			"midnight",
			"next",
			"weekdays",
			"weekend",
			"today",
			"a",
			"p",
			"n",
			"m",
			"minute",
			"minutes",
			"hour",
			"hours",
			"day",
			"days",
			"week",
			"weeks",
			"month",
			"months",
			"year",
			"years",
			"min",
			"mins",
			};


yylex()
{
	int c, j=0;
	char buf[26];
	int i,val;
	

	while ((c = getc()) != EOF) {
		switch (c) {
		case '\t':
		case '\n':
		case ' ':
			break;
		case ':':
		case '.':
			{ LL(COLON, 0); }
			break;
		case ',':
			{ LL(COMMA, 0); }
			yeartm++;
			break;
		case '+':
			{ LL(PLUS, 0); }
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (lookahd() == 2)  /* a number */
				val = (c - '0') * 10 + (getc() - '0'); 
			else
				val = c - '0';
			if (timedon == 2 && daydon != 1 && !dayfirst()) {
				return(yylval = val,DAYN); 
			}
			else {
				if (daydon != 1 && dayfirst()) {
				  char *argpsav;
			
				  argpsav = argp;
				  if (yylex() == MONTH)  {
				    argp = argpsav;
				    yylval = val;
				    return(DAYN);
				  }
				  else {
				    argp = argpsav;
				    yylval = val;
				    return(NUMB);
				  }
				}
				return(yylval=val,NUMB); 
			}
			break;
		default:
			buf[j++] = c;
			if (lookahd() != 1) {
				buf[j] = '\0';
				j = 0;
				return(word(buf));
			}
		}
	}
}


lookahd()
{
	switch (*argp) {
	case '\t':
	case '\n':
	case ' ':
	case ':':
	case '.':
	case ',':
	case '+':
		return(0);
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return(2);
	default:
		return(1);
	}
}


word(bufin)
char *bufin;
{
	int i;
	char buf[30];

	strtolower(buf, bufin);

	for (i = 0; i < 12; i++) {
		if (EQ(mon[i], buf) || EQ(lgmon[i], buf)) {
			if (timedon != 2) 
				LL(MONTH,i);
			else
				LL(NUMB,i);
		}
	}

	for (i = 0; i < 30; i++) {
		if (EQ(rest[i], buf))
			break;
	}

	switch (i) {
	case 11:
		/*  "today"  */
		LL(DAY, 7);
		break;
	case 5:
		/*  "tomorrow"  */
		LL(DAY, 8);
		break;
	case 14:
		/* "n"  */
  		LL(TIME, NOON);
		break;
	case 6:
		/* "noon"  */
  		LL(TIME, NOON);
		break;
	case 15:
		/*   "m"  */
		LL(TIME, MIDNIGHT);
		break;
	case 7:
		/*   "midnight"  */
		LL(TIME, MIDNIGHT);
		break;
	case 3:
		/*  "now"    */
		LL(TIME, NOW);
		break;
	case 12:
		/*  "a"   */
		LL(SUFF, AM);
		break;
	case 0:
		/*  "am"  */
		LL(SUFF, AM);
		break;
	case 13:
		/*   "p"   */
		LL(SUFF, PM);
		break;
	case 1:
		/*   "pm"  */
		LL(SUFF, PM);
		break;
	case 2:
		/*  "zulu"   */
		LL(SUFF, ZULU);
		break;
	case 8:
		/*  "next"   */
		LL(NEXT, 1);
		break;
	case 16:
		/*  "minute"	*/
		LL(UNIT, MINUTE);
		break;
	case 17:
		/*  "minutes"	*/
		LL(UNIT, MINUTE);
		break;
	case 28:
		/*  "min"	*/
		LL(UNIT, MINUTE);
		break;
	case 29:
		/*  "mins"	*/
		LL(UNIT, MINUTE);
		break;
	case 18:
		/*   "hour"   */
		LL(UNIT, HOUR);
		break;
	case 19:
		/*   "hours"   */
		LL(UNIT, HOUR);
		break;
	case 20:
		/*   "day"   */
		LL(UNIT, DAY);
		break;
	case 21:
		/*   "days"   */
		LL(UNIT, DAY);
		break;
	case 22:
		/*   "week"   */
		LL(UNIT, WEEK);
		break;
	case 23:
		/*   "weeks"   */
		LL(UNIT, WEEK);
		break;
	case 24:
		/*   "month"   */
		LL(UNIT, MONTH);
		break;
	case 25:
		/*   "months"   */
		LL(UNIT, MONTH);
		break;
	case 26:
		/*   "year"   */
		LL(UNIT, YEAR);
		break;
	case 27:
		/*   "years"   */
		LL(UNIT, YEAR);
		break;
	default:

		for (i = 0; i < 7; i++)
			if (EQN(days[i],buf,strlen(buf)) || 
			   EQN(lgdays[i],buf,strlen(buf) <= 2 ? 2:strlen(buf))) 
				LL(DAY,i);

		fprintf(stderr, MSGSTR(MS_WORD,"unknown word, %s\n"), buf);
		exit(1);
	}
}


getNLS()
{
	int j;
 	char *getfield(), buf1[26];

	if (*getfield("NLSMONTH", 0) != '?') {
		for (i=0; i < 12; i++) {
			strcpy(buf1, getfield("NLSMONTH", i));
			if (buf1[0] != '?')
				strtolower(mon[i], buf1);
		}
	}
	if (*getfield("NLLMONTH", 0) != '?') {
		for (i=0; i < 12; i++) {
			strcpy(buf1, getfield("NLLMONTH", i));
			if (buf1[0] != '?')
				strtolower(lgmon[i], buf1);
		}
	}
	if (*getfield("NLSDAY", 0) != '?') {
		for (i=0; i < 7; i++) {
			strcpy(buf1, getfield("NLSDAY", i));
			if (buf1[0] != '?')
				strtolower(days[i], buf1);
		}
	}
	if (*getfield("NLLDAY", 0) != '?') {
		for (i=0; i < 7; i++) {
			strcpy(buf1, getfield("NLLDAY", i));
			if (buf1[0] != '?')
				strtolower(lgdays[i], buf1);
		}
	}
	if (*getfield("NLTMISC", 0) != '?') {
		for (i=0; i < 3; i++) {
			strcpy(buf1, getfield("NLTMISC", i+5));
			if (buf1[0] != '?')
				strtolower(rest[i], buf1);
		}
	}
	if (*getfield("NLTSTRS", 0) != '?') {
		for (i=3; i < 16; i++) {
			strcpy(buf1, getfield("NLTSTRS", i-3));
			if (buf1[0] != '?')
				strtolower(rest[i], buf1);
		}
	}
	if (*getfield("NLTUNITS", 0) != '?') {
		for (i=16; i < 30; i++) {
			strcpy(buf1, getfield("NLTUNITS", i-16));
			if (buf1[0] != '?')
				strtolower(rest[i], buf1);
		}
	}
}


#include <NLchar.h>

char *NLgetenv(), *strchr();
char buffer[NAME_MAX+1];

static
char *
getfield(s, n)
char *s;
register int n;
{
	/* copy 'n'th field of value of envt. variable 's' to 'result' */
	/* return a string consisting of a "?" for 'n' less than 0 or */
	/* greater than number of fields in string (zero origin) */

	register char *s1, *s2;

	if (n < 0)
		return("?");
	for (s1 = NLgetenv(s), s2 = s1; ; s1 = s2, --n) {
	    	if (s1 == 0)
			return("?");
	    	s2 = strchr(s1, ':');
	    	if (n <= 0)
			break;
	    	if (s2)
			++s2;
	}
	if (s2 == 0)
		return s1;
	else {
	    	n = s2 - s1;
	    	strncpy(buffer, s1, n);
	    	buffer[n] = 0;
	    	return buffer;
	}
}


strtolower(s1, s2)
char *s1, *s2;
{
	NLchar nlc, nlcl;

	while (*s2 != '\0') {
		s2 += NCdecode(s2, &nlc);
		nlcl = NCtolower(nlc);
		s1 += NCencode(&nlcl, s1);
	}
	*s1 = '\0';
	return;
}
