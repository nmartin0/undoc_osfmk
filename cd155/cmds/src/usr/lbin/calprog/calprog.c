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
static char rcsid[] = "@(#)$RCSfile: calprog.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:46:56 $";
#endif

/* /usr/lib/calprog produces an egrep -f file
   that will select today's and tomorrow's
   calendar entries, with special weekend provisions

   used by calendar command
*/

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <nl_types.h>
#include <langinfo.h>

#define	DAY	(3600*24L)

nl_item mon[] = {	/* NL abbreviated months (Jan, etc) */
    ABMON_1, ABMON_2, ABMON_3, ABMON_4, ABMON_5, ABMON_6, ABMON_7,
    ABMON_8, ABMON_9, ABMON_10, ABMON_11, ABMON_12
};

char	separator = '/';

main()
{
	long	t;
	int	month_day;

	(void) setlocale(LC_ALL, "");

	/*
	 * Determine regional month/day order
	 */
	month_day = md_order(nl_langinfo(D_FMT));

	time(&t);
	tprint(t, month_day);

	switch(localtime(&t)->tm_wday) {
	case 5:			/* Fri:  include Sat */
		t += DAY;
		tprint(t, month_day);
	case 6:			/* Fri/Sat:  include Sun */
		t += DAY;
		tprint(t, month_day);
	default:		/* include next day */
		t += DAY;
		tprint(t, month_day);
	}
}


tprint(t, month_day)
long	t;
int	month_day;
{
	struct tm *tm;
	unsigned char month[10];
	
	tm = localtime(&t);
	mk_month(month, nl_langinfo(mon[tm->tm_mon]));

	printf("(^|[^0-9])");

	if (month_day)
		printf("((%s[^ ]* *|0*%d\\%c|\\*(\\%c| ))0*%d)",
		    month, tm->tm_mon + 1, separator, separator, tm->tm_mday);
	else	/* day/month */
		printf("(0*%d( *%s[^ ]*|\\%c0*%d|(\\%c| )\\*))",
		    tm->tm_mday, month, separator, tm->tm_mon + 1, separator);

	printf("([^0-9]|$)\n");
}


int
md_order(fmt)
char *fmt;
{
	int	month_day = 0;

	while (*fmt && !month_day) {
		while (*fmt != '%' && *fmt)
			++fmt;

		if (*fmt == '%' && *++fmt == '%')
			continue;
		
		while (*fmt && !isalpha(*fmt))
			++fmt;

		switch (*fmt) {
			case 'm':	/* decimal month number */
			case 'h':	/* abbr. month (x/open ext.) */
			case 'b':	/* abbr. month name */
			case 'B':	/* full month name */
			case 'D':	/* date (%m/%d/%y) */
				month_day = 2;
				break;
			case 'd':	/* day of month */
				month_day = 1;
				break;
			default:
				;
		}
	}

	while (*fmt && !ispunct(*fmt))	/* try to find date separator */
		++fmt;
	if (*fmt)
		separator = *fmt;

	return(month_day ? month_day-1 : 1);
}


mk_month(mbuf, month)
unsigned char *mbuf;
unsigned char *month;
{
	
	if (isalpha(*month)) {
		*mbuf++ = '[';
		*mbuf++ = toupper(*month);
		*mbuf++ = tolower(*month);
		*mbuf++ = ']';
		strcpy(mbuf, ++month);
	} else
		strcpy(mbuf, month);
}
