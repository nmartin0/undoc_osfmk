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
 
/*++ scantime.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/scantime.c,v $
 *
 * DESCRIPTION:
 *
 *	Scan a date and/or time specification with implied defaults.
 *	This module contains the three exported functions:
 *
 *		scnctime(),
 *		scnftime(), and
 *		scnptime()
 *
 *	which parse a time and/or date specification respectively
 *	defaulting unspecified values to their:
 *
 *		current value (scnctime()),
 *		future value (scnftime()), or
 *		past value (scnptime()).
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:45 $ $State: Exp $)
 * $Log: scantime.c,v $
 * Revision 1.4  1994/11/19  02:27:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/19  17:48:37  mwan
 * Before T10 frozen. Fixed several PTS
 *
 * Revision 1.2  1992/10/09  20:18:44  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:51  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:54:50  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)scantime.c	1.2 (scantime.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#if OSF
#include "sys_types_OSF.h"      /* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>
#endif

#if	SGI | SYS52 | UTS | UNICOS | OSF
#include <time.h>
extern long timezone;			/* Timezone difference in seconds */
					/* between GMT and standard time */
extern void tzset();			/* Set local timezone */
extern int daylight;			/* Defined in ctime.c.  Non-zero */
					/* iff the standard U.S.A. daylight */
					/* savings time conversion is */
					/* enabled;  (Parts of?) Michigan */
					/* and Arizona choose to ignore */
					/* daylight savings time. */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>
#else
BAD SYSTEM TYPE
#endif
#endif

extern struct tm *localtime ();		/* Get local time */
extern int strcmp();			/* String comparison */
extern time_t time();			/* Get current time */

/*
 *
 *	Macros.
 */
#define	yearsize(y)	( (y)%4 ? 365 : 366 )	/* Will not work for centur- */
						/* ies not divisible by 400. */

/*
 *
 *	Default modes.
 */
#define	DEF_CURRENT	0		/* Default unspecified date and */
					/* unspecified hour values to the */
					/* current value */
#define	DEF_FUTURE	1		/* Default unspecified date and */
					/* unspecified hour values to the */
					/* current or future value (so long */
					/* as the default value is NOT in the */
					/* temporal past). */
#define	DEF_PAST	2		/* Default unspecified date and */
					/* unspecified hour values to their */
					/* current value or past value (so */
					/* long as the value is NOT in the */
					/* temporary future). */

/*
 *
 *	Token types.
 */
#define	T_DASH		0		/* - */
#define	T_SLASH		1		/* / */
#define	T_INT		2		/* an integer number */
#define	T_COLON		3		/* : */
#define	T_LITERAL	4		/* A string of alphabetic chars */
#define	T_ERROR		5		/* Bad token scanned */
#define	T_EOS		6		/* End of time string found */

/*
 *
 *	Parse function result codes.
 */
#define	SUCCESS		1		/* Rule successfully reduced */
#define	FAILURE		0		/* Rule not found */
#define	ERROR		(-1)		/* Syntax error in scan phase */

/*
 *
 *	Scan date/time specification return codes.
 */
#define	ISUCCESS	0		/* Valid date/time spec */
#define	ESYNTAX		(-1)		/* Syntax error in time/date spec */
#define	EINVALID	(-2)		/* Invalid time and/or date spec */
#define	EUNDEF		(-3)		/* No time/date specified */

/*
 *
 *	Miscellaneous.
 */
#define	ALFA_TERM	0		/* Digit string can be terminated */
					/* with an alphabetic character */
#define	NOALFA_TERM	1		/* Digit string cannot be terminated */
					/* with an alphabetic character */
#define	MAX_LITSIZE	9		/* Big enough to hold "wednesday" */
#define	UNDEF		(-721)		/* Value not specified. */
					/* We pick a token value that is: */
					/*    1. Negative, and */
					/*    2. Not in the range: */
					/*	 [-12*60..12*60] so that */
					/*	 valid timezone */
					/*	 specifications will not */
					/*	 show up as unspecified. */
#define	MERIDIAN_AM	0		/* AM meridian was specified */
#define	MERIDIAN_PM	1		/* PM meridian was specified */
#define	MERIDIAN_M	2		/* M meridian was specified */
#define	YESTERDAY	7		/* Weekday specified = yesterday */
#define	TODAY		8		/* Weekday specified = today */
#define	TOMORROW	9		/* Weekday specified = tomorrow */


/*
 *
 *	External functions.
 */
extern strcmp();			/* String comparison */


/*
 *
 *	Variables local to this module.
 */
static char *scanptr;			/* Scan pointer */
static char t_literal [MAX_LITSIZE+1];	/* <alpha_lit> literal */
static short t_integer;			/* Scanned <nn> integer value */
static short t_token;			/* Current scan token type: T_ */
static short sp_meridian;		/* Meridian value: MERIDIAN_ or UNDEF*/
static short sp_weekday;		/* Weekday specification: */
					/* UNDEF = unspecified; otherwise */
					/* [0..6] maps to [Sunday..Saturday] */
					/* 7=yesterday; 8=today; 9=tomorrow; */
static short sp_month;			/* Month specification: */
					/* UNDEF = unspecified; otherwise */
					/* [0..11] maps to [January..December]*/
static short sp_zone;			/* Timezone specification: */
					/* Minutes west from GMT */
					/* UNDEF = unspecified; */
static short sp_day;			/* Numerical day of month: */
					/* UNDEF = unspecified; */
static short sp_year;			/* Year: */
					/* UNDEF = unspecified; */

/*** scnctime
 *
 *
 *	scnctime():
 *
 *	Scan a date and time specification defaulting unspecified date and
 *	unspecified hour values to the current value.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if the specification contains a syntax error.
 *	       -2: if the specified date and/or time is invalid
 *		   (i.e Jan. 40);
 *	       -3: if no date or time was specified.
 */
int scnctime (dateandtimestr, res_time)
char *dateandtimestr;			/* Time and/or date specification */
time_t *res_time;			/* Where to place the resulting time */
{
	return (scantime (dateandtimestr, res_time, DEF_CURRENT));
}


/*** scnftime
 *
 *
 *	scnftime():
 *
 *	Scan a date and time specification defaulting unspecified date and
 *	unspecified hour values to the current value, or the next temporally
 *	closest value occurring in the future.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if the specification contains a syntax error.
 *	       -2: if the specified date and/or time is invalid
 *		   (i.e Jan. 40);
 *	       -3: if no date or time was specified.
 */
int scnftime (dateandtimestr, res_time)
char *dateandtimestr;			/* Time and/or date specification */
time_t *res_time;			/* Where to place the resulting time */
{
	return (scantime (dateandtimestr, res_time, DEF_FUTURE));
}


/*** scnptime
 *
 *
 *	scnptime():
 *
 *	Scan a date and time specification defaulting unspecified date and
 *	unspecified hour values to the current value, or the temporally
 *	closest value occurring in the past.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if the specification contains a syntax error.
 *	       -2: if the specified date and/or time is invalid
 *		   (i.e Jan. 40);
 *	       -3: if no date or time was specified.
 */
int scnptime (dateandtimestr, res_time)
char *dateandtimestr;			/* Time and/or date specification */
time_t *res_time;			/* Where to place the resulting time */
{
	return (scantime (dateandtimestr, res_time, DEF_PAST));
}


/*** scantime
 *
 *
 *	int scantime():
 *	Scan a date and time specification.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if the specification contains a syntax error.
 *	       -2: if the specified date and/or time is invalid
 *		   (i.e Jan. 40);
 *	       -3: if no date or time was specified.
 *
 *
 *	The syntax acceptable by this function is quite flexible but
 *	there are some restrictions to eliminate possible ambiguities:
 *
 *
 *		A time and date specification to be considered valid
 *		by this function, must contain a time and/or date
 *		specification.  If no date is specified, then the
 *		current day is assumed.  If no time is specified, then
 *		the time of 0:00:00 for the specified timezone is
 *		assumed.  If no time or timezone is specified, then
 *		the time of 0:00:00 for the local time zone is assumed.
 *
 *
 *		A date can be specified as a month and numerical
 *		day (with the optional addition of a numerical year
 *		specification), or alternatively, as the name of a
 *		weekday (i.e. Sunday).
 *
 *
 *		It is illegal to specify a year, WITHOUT also
 *		specifying a month and day.
 *
 *
 *		If it illegal to specify a month and day (with
 *		optional year), IN ADDITION to specifying the name
 *		of a weekday, and vice versa.
 *
 *
 *		It is illegal to specify a month, without also
 *		specifying the numerical day within the month, and
 *		vice versa.
 *
 *
 *		A weekday is specified as any prefix of the name
 *		of a weekday that is three characters or longer,
 *		where the comparison is case insensitive.  An optional
 *		period may follow a weekday name specification.  Thus,
 *
 *			Sun.
 *			sUN
 *			Monda
 *			Tues
 *			tue
 *			tue.
 *
 *		are acceptable weekday name specifications with the
 *		obvious interpretations.  Three additional "weekday
 *		names" are also acceptable.  They are:
 *
 *			today,
 *			tomorrow,	and
 *			yesterday.
 *
 *		These last three "weekday names" are recognized regard-
 *		less of case, however they cannot be abbreviated and
 *		must be completely spelled out to be recognized.
 *
 *
 *		The month portion of a month and day date specification
 *		can be either numerical in the interval [1..12], or
 *		alphabetic, as the name of the month.  Month names are
 *		recognized using rules identical for the recognition of
 *		weekday names.  Thus,
 *
 *			Janu
 *			Jan
 *			January
 *			Jan.
 *			fEBrUAr
 *
 *		are all acceptable as month name specifications with
 *		the obvious interpretations.
 *
 *
 *		The permissable forms for specifying the numerical day
 *		and month are as follows:
 *
 *			MM/DD
 *			Month DD
 *			DD-Month
 *
 *		where "Month" indicates that the name of the month has
 *		been specified (i.e. "Jan").
 *
 *
 *		If a year is specified as part of the time/date
 *		specification, then it must appear adjacent to the
 *		month and day specification, unless the year is
 *		specified as a four-digit number (i.e. 2001), in
 *		which case it can appear in any location within the
 *		time and date specification string, where the
 *		location must not violate any of the rules for a
 *		time of day, or month/day specification.
 *
 *
 *		The permissable forms for specifying a year
 *		adjacent to the numerical date and month are as
 *		follows:
 *
 *			DD-Month-YY
 *			DD-Month-YYYY
 *			DD-Month YYYY
 *			Month DD YYYY
 *			MM/DD YYYY
 *			MM/DD/YY
 *			YYYY-MM-DD
 *			YYYY DD-Month
 *			YYYY Month DD
 *			YYYY MM/DD
 *
 *
 *		A time of day specification can appear anywhere
 *		within the time and date specification string with
 *		the restriction that it cannot appear in places
 *		which would violate the syntax of a month/day
 *		(and optional year) specification.  Thus,
 *
 *			22- 11pm January
 *
 *		is illegal, while:
 *
 *			22-January, 11pm
 *
 *		is perfectly acceptable.
 *
 *
 *		A time of day specification is allowed in the
 *		following forms:
 *
 *			HH <meridian>
 *			HH:MM <meridian>
 *			HH:MM:SS <meridian>
 *			noon
 *			midnight
 *
 *		where:
 *			HH is any one or two digit hour specification
 *			   in the interval [0..24].
 *
 *			MM is any one or two digit minutes specification
 *			   in the interval [0..59].
 *
 *			SS is any one or two digit seconds specification
 *			   in the interval [0..59].
 *
 *			<meridian> (if present) specifies "am", "pm", or
 *				   "m" (meaning noon).  A meridian is
 *				   recognized regardless of upper and/or
 *				   lowercase character usage.  A meridian
 *				   specification can also be optionally
 *				   preceded with a dash.
 *
 *
 *		In the absence of a meridian specification, a 24-hour
 *		clock semantic interpretation is used.
 *
 *
 *		The precise meanings of "am" and "pm" are not always
 *		agreed upon by the general populace when specifying
 *		times like 12am or 12pm.  Rather than cave in to any
 *		individual's favorite "folk-interpretation", we have
 *		chosen to treat everyone like dirt, and implement the
 *		correct definitions of "am" and "pm" (which almost
 *		no one seems to be familiar with).
 *
 *
 *		The interpretation of "am", "pm", and "m" used by this
 *		function is shown in the following table.
 *
 *			Specified time:		24-hour clock interpretation:
 *			---------------		-----------------------------
 *			 0:00:00-11:59:59 AM	 0:00:00-11:59:59
 *			12:00:00	  AM	 0:00:00
 *			12:00:01-12:59:59 AM	 0:00:01- 0:59:59
 *			13:00:00-24:00:00 AM	Invalid
 *			 0:00:00-11:59:59 PM	12:00:00-23:59:59
 *			12:00:00	  PM	24:00:00
 *			12:00:01-12:59:59 PM	12:00:01-12:59:59
 *			13:00:00-24:00:00 PM	Invalid
 *			 0:00:00-11:59:59 M	Invalid
 *			12:00:00-12:59:59 M	12:00:00-12:59:59
 *			13:00:00-24:00:00 M	Invalid
 *
 *
 *		Legal example time of day specifications are as
 *		follows:
 *
 *			11pm
 *			11:30-am
 *			12:00		(noon)
 *			11:59AM
 *			12:00-aM	(0:00:00)
 *			12:00-Pm	(24:00:00)
 *			12m		(noon)
 *			23:00:01	(1 second past 11pm).
 *			9 pm
 *			01:00:01am
 *			noon
 *			midnight
 *
 *
 *		A timezone can also be optionally specified.  If no
 *		time zone is specified, then the local timezone is
 *		assumed.  A timezone name specified as any 3-character
 *		timezone abbreviation can appear anywhere within the
 *		time and date specification string, with the restriction
 *		that it cannot appear in locations which would violate
 *		the syntax of a month/day (and optional year), or time
 *		of day specification.  Thus,
 *
 *			22- EDT January
 *
 *		is illegal, while:
 *
 *			22-January, EDT
 *
 *		is perfectly acceptable.  It is also legal to place an
 *		optional dash in front of any timezone name as in:
 *
 *			29-Feb 1988 12:01-EST
 *
 *		For a list of the recognized time zone names, see the
 *		function: zone() below.
 *
 *
 *		Lastly, all tab, space, and comma characters are
 *		interpreted as whitespace and are therefore, essentially
 *		ignored.  A newline character if encountered, terminates
 *		the time/date specification.  Otherwise the null character
 *		at the end of the string serves to terminate the time/-
 *		date specification.
 *		
 */
static int scantime (dateandtimestr, res_time, default_mode)
char *dateandtimestr;			/* Time and/or date specification */
time_t *res_time;			/* Where to return time in secs */
int default_mode;			/* Default mode = [DEF_CURRENT, */
{					/* DEF_FUTURE, or DEF_PAST]. */
	static monthsize [] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,	/* Non-leap */
		31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31	/* Leap year */
	};

	short daylbegin;		/* Daylight savings time begins */
	short daylend;			/* Daylight savings time ends */
	short sp_hour;			/* Time specification for hours: */
					/* UNDEF = unspecified; */
	short sp_minute;		/* Time specification for minutes: */
	short sp_second;		/* Time specification for seconds: */
	short ch;			/* Scan character */
	short save_int;			/* Save integer token value */
	short local_zone;		/* Timezone in minutes west from GMT */
	time_t timeinsecs;		/* Time in seconds from 0:00:00 */
					/* January 1, 1970-GMT */
	struct tm *tm;			/* Time structure pointer */
	register short i;		/* Index and work var */

	/*
	 *  Get the current time/date and local timezone for later use.
	 */
#if	SGI | SYS52 | UTS | UNICOS | OSF
	time (&timeinsecs);		/* Get current time */
	tzset();			/* Get local timezone */
	local_zone = timezone / 60;	/* Timezone in minutes west from GMT */
#else
#if	BSD42 | BSD43 | ULTRIX
	struct	timeval	tv;
	struct	timezone tz;

	time (&timeinsecs);		/* Get current time */
	gettimeofday (&tv, &tz);	/* Get timezone information */
	local_zone = tz.tz_minuteswest;	/* Timezone in minutes west from GMT */
#else
BAD SYSTEM TYPE
#endif
#endif
	tm = localtime (&timeinsecs);
	/*
	 *  At this point,
	 *	tm->tm_sec:   Seconds [0..59].
	 *	tm->tm_min:   Minutes [0..59].
	 *	tm->tm_hour:  Hours [0..23].
	 *	tm->tm_mday:  Day of month [1..31].
	 *	tm->tm_mon:   Month [0..11].
	 *	tm->tm_year:  Year - 1900.
	 *	tm->tm_wday:  Weekday [0..6].
	 *	tm->tm_yday:  Day of year [0..365].
	 *	tm->tm_isdst: Non-zero if daylight savings time.
	 */
	scanptr = dateandtimestr;	/* Initialize scan pointer */
	sp_hour = UNDEF;		/* Hour unspecified */
	sp_minute = 0;			/* Default is 0 */
	sp_second = 0;			/* Default is 0 */
	sp_meridian = UNDEF;		/* Meridian unspecified */
	sp_weekday = UNDEF;		/* Weekday unspecified */
	sp_month = UNDEF;		/* Month unspecified */
	sp_day = UNDEF;			/* Day unspecified */
	sp_year = UNDEF;		/* Year unspecified */
	sp_zone = UNDEF;		/* Timezone unspecified */
	if (scan (ALFA_TERM) == T_EOS) {/* Get first token */
		return (EUNDEF);	/* No time and/or date specified */
	}
	do {
		switch (t_token) {
		case T_DASH:
			scan (ALFA_TERM);	/* Treat a dash like white- */
			break;			/* space when looking for a */
						/* left-sentential form */
		case T_SLASH:
		case T_COLON:
		case T_ERROR:
			return(ESYNTAX);
		case T_INT:
			/*
			 *  Could be month, day, year, or time specification.
			 */
			ch = *scanptr;	/* Peek at following character */
			if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
				/*
				 *  It better be "am", "pm", or "m" as a
 				 *  time of day specification.
				 */
				if (sp_hour != UNDEF) {
					/*
					 *  The only time an alphabetic
					 *  character can EVER follow a
					 *  digit string is on a time of
					 *  day specification: i.e. 3pm.
					 */
					return (ESYNTAX);
				}
				sp_hour = t_integer;
				/*
				 *  Parse:
				 *	HH [{am,pm,m}]
				 */
				if (scan (ALFA_TERM) != T_LITERAL) {
					return (ESYNTAX);
				}
				if (meridian() != SUCCESS) {
					return (ESYNTAX);
				}
				scan (ALFA_TERM);	/* Scan next token */
			}
			else if (t_integer <= 99) {
				/*
				 *  Could be month, day, year, or time spec.
				 */
				save_int = t_integer;	/* Save value */
				switch (scan (ALFA_TERM)) {
				case T_DASH:
					/*
					 *  Parse:
					 *	DD- month-YY
					 *	DD- month-YYYY
					 *	DD- month YYYY
					 *	DD- month
					 *	HH- {am,pm,m}
					 */
					if (scan (ALFA_TERM) != T_LITERAL) {
						return (ESYNTAX);
					}
					switch (month()) {
					case SUCCESS:
						/*
						 *  We have parsed:
						 *	DD-month
						 */
						if (sp_weekday != UNDEF ||
						    sp_day != UNDEF) {
							return (ESYNTAX);
						}
						sp_day = save_int;
						/*
						 *  The possible forms are:
						 *	DD-month -YY
						 *	DD-month -YYYY
						 *	DD-month  YYYY
						 *	DD-month
						 *
						 *  Check for the form:
						 *	DD-month -YY
						 *
						 *  We will let the form:
						 *	DD-month YYYY
						 *
						 *  be parsed separately as:
						 *	DD-month     and
						 *	YYYY
						 */
						if (scan (ALFA_TERM)==T_DASH) {
							/*
							 *  We have parsed:
							 *	DD-month-
							 */
							scan (NOALFA_TERM);
							if (t_token != T_INT ||
							    sp_year != UNDEF) {
								return(ESYNTAX);
							}
							/*
							 *  We have parsed:
							 *	DD-month-YY or
							 *	DD-month-YYYY
							 */
							sp_year = t_integer;
							scan (ALFA_TERM);
						}
						break;
					case FAILURE:
						if (meridian() != SUCCESS) {
							return (ESYNTAX);
						}
						/*
						 *  We have parsed:
						 *	HH-{am,pm,m}
						 */
						if (sp_hour != UNDEF) {
							return (ESYNTAX);
						}
						sp_hour = save_int;
						scan (ALFA_TERM);
						break;
					case ERROR:
						return (ESYNTAX);
					}
					break;
				case T_SLASH:
					/*
					 *  Process:
					 *	MM/ DD YYYY
					 *	MM/ DD/YY
					 *	MM/ DD/YYYY
					 *	MM/ DD
					 */
					if (sp_month != UNDEF ||
					    sp_weekday != UNDEF) {
						return (ESYNTAX);
					}
					sp_month = t_integer;
					if (scan (NOALFA_TERM) != T_INT) {
						return (ESYNTAX);
					}
					sp_day = t_integer;
					/*
					 *  Check for (and parse if present),
					 *  the forms:
					 *
					 *	MM/DD /YY   and
					 *	MM/DD /YYYY.
					 *
					 *  We let the form: MM/DD YYYY
 					 *
					 *  be recognized as the two rules:
					 *	MM/DD, and
					 *	YYYY
					 */
					if (scan (ALFA_TERM) == T_SLASH) {
						if (scan (NOALFA_TERM) != T_INT
						    || sp_year != UNDEF) {
							return (ESYNTAX);
						}
						sp_year = t_integer;
						scan (ALFA_TERM);
					}
					break;
				case T_COLON:
					/*
					 *  Process:
					 *	HH: MM [:SS] [[-]{am,pm,m}]
					 */
					if (sp_hour != UNDEF) {
						return (ESYNTAX);
					}
					sp_hour = save_int;
					if (scan (ALFA_TERM) != T_INT) {
						return (ESYNTAX);
					}
					sp_minute = t_integer;
					if (scan (ALFA_TERM) == T_COLON) {
						/*
						 *  HH:MM: SS [[-]{am,pm,m}]
						 */
						if (scan (ALFA_TERM) != T_INT) {
							return (ESYNTAX);
						}
						sp_second = t_integer;
						scan (ALFA_TERM);
					}
					/*
					 *  Check for, and skip over any
					 *  leading dash introducing a
					 *  meridian.
					 */
					if (t_token == T_DASH) {
						scan (ALFA_TERM);
					}
					/*
					 *  Check for meridian.
					 */
					if (t_token == T_LITERAL) {
						/*
						 *  Interpret meridian if
						 *  specified.
						 */
						if (meridian() == SUCCESS) {
							/*
							 *  Scan past
							 *  meridian.
							 */
							scan (ALFA_TERM);
						}
					}
					break;
				case T_LITERAL:
					/*
					 *  Possibly: HH {am,pm,m}
					 */
					if (meridian() != SUCCESS ||
					    sp_hour != UNDEF) {
						return (ESYNTAX);
					}
					sp_hour = save_int;
					scan (ALFA_TERM);
					break;
				case T_INT:
				case T_ERROR:
					return (ESYNTAX);
				case T_EOS:
					/*
					 *  Assumed to be a lone hour
					 *  specification.
					 */
					if (sp_hour != UNDEF) {
						return (ESYNTAX);
					}
					sp_hour = save_int;
				}
			}
			else if (t_integer >= 1970 && t_integer <= 2037) {
				/*
				 *  It is definitely specifying a year.
				 *  YYYY
				 */
				if (sp_weekday != UNDEF ||
				    sp_year != UNDEF) {
					return (ESYNTAX);
				}
				sp_year = t_integer;
				if (scan (ALFA_TERM) == T_DASH) {
					/*
					 *  Parse:
					 *	YYYY- MM-DD
					 */
					if (scan (NOALFA_TERM) != T_INT ||
					    sp_month != UNDEF) {
						return (ESYNTAX);
					}
					/*
					 *  Parse:
					 *	YYYY-MM -DD
					 */
					sp_month = t_integer;
					if (scan (ALFA_TERM) != T_DASH) {
						return (ESYNTAX);
					}
					if (scan (NOALFA_TERM) != T_INT) {
						return (ESYNTAX);
					}
					/*
					 *  We have parsed:
					 *	YYYY-MM-DD
					 */
					sp_day = t_integer;
					scan (ALFA_TERM);
				}
			}
			else return (ESYNTAX);
			break;
		case T_LITERAL:
			/*
			 *  Could be a month name, day name, or time zone
			 *  name.
			 */
			switch (day()) {
			case SUCCESS:
				/*
				 *  We have scanned:
				 *	"Yesterday", "Today", "Tomorrow",
				 *	"Sunday", "Monday", "Tuesday",
				 *	"Wednesday", "Thursday", "Friday",
				 *	or "Saturday".
				 */
				if (sp_day != UNDEF ||
				    sp_year != UNDEF) {
					/*
					 *  Giving a day name prohibits any
					 *  other date specification.
					 */
					return (ESYNTAX);
				}
				break;
			case FAILURE:
				switch (month()) {
				case SUCCESS:
					/*
					 *  We have scanned:
					 *	"January", "February",
					 *	"March", "April", "May",
					 *	"June", "July", "August",
					 *	"September", "October",
					 *	"November", or "December".
					 *
					 *  When a month-name is specified as
					 *  the start of a left-sentential
					 *  form, then an integer must
					 *  immediately follow it specifying
					 *  the numerical day date within the
					 *  month.
					 *
					 *  Parse:  Month DD
					 */
					if (scan (NOALFA_TERM) != T_INT ||
					    sp_weekday != UNDEF) {
						return (ESYNTAX);
					}
					sp_day = t_integer;
					break;
				case FAILURE:
					if (!strcmp (t_literal, "midnight")) {
						if (sp_hour != UNDEF) {
							return (ESYNTAX);
						}
						sp_hour = 24;
					}
					else if (!strcmp (t_literal, "noon")) {
						if (sp_hour != UNDEF) {
							return (ESYNTAX);
						}
						sp_hour = 12;
					}
					else if (zone() != SUCCESS) {
						return (ESYNTAX);
					}
					break;
				case ERROR:
					return (ESYNTAX);
				}
				break;
			case ERROR:
				return (ESYNTAX);
			}
			scan (ALFA_TERM);	/* Scan next token */
			break;
		}
	} while (t_token != T_EOS);	/* Keep scanning until end-of-string */
	/*
	 *  Parsing of the time/date specification is now complete.
	 *  Now, interpret the time/date specification.
	 */
	if (sp_year == UNDEF) sp_year = tm->tm_year + 1900;	/* Year */
	else {
		/*
		 *  A year was explicitly specified.
		 */
		if (sp_month == UNDEF) {
			/*
			 *  If you specify a year, then you must also
			 *  specify a month and day.
			 */
			return (ESYNTAX);
		}
		if (sp_year < 99) {
			/*
			 *  Add in the current century.
			 */
			save_int = (tm->tm_year + 1900) / 100;
			sp_year += save_int * 100;
		}
		else if (sp_year < 1970 || sp_year > 2037) {
			/*
			 *  Catch ridiculous year specifications.
			 */
			return (EINVALID);
		}
	}
	if (sp_weekday != UNDEF) {
		/*
		 *  A date specification has been given in the form
		 *  of a weekday name.
		 */
		sp_day = tm->tm_yday;		/* Day of year [0..365] */
		if (sp_weekday == TOMORROW || sp_weekday <= 6) {
			if (sp_weekday == TOMORROW) sp_day += 1;
			else if (sp_weekday < tm->tm_wday) {
				sp_day += 7 - tm->tm_wday + sp_weekday;
			}
			else sp_day += sp_weekday - tm->tm_wday;
			/*
			 *  i will have the number of days to
			 *  advance [0..6].
			 */
			if ((i = sp_day - yearsize(sp_year)) >= 0) {
				sp_year++;
				sp_day = i;
			}
		}
		else if (sp_weekday == YESTERDAY) {
			if (sp_day == 0) {
				sp_year--;
				sp_day = yearsize(sp_year);
			}
			sp_day--;	/* Day of year [0..365] */
		}
	}
	else if (sp_month == UNDEF) {
		/*
		 *  No month/day-of-month (and no weekday) was specified.
		 */
		sp_day = tm->tm_yday;	/* Current day */
	}
	else {
		/*
		 *  An explicit date specification has been given of
		 *  at least the month and month day, and possibly
		 *  the year.
		 */
		if (sp_month == 0 || sp_month > 12) return (EINVALID);
		i = 0;
		sp_month--;
		if (yearsize(sp_year) == 366) {
			i = 12;
			sp_month += i;
		}
		if (monthsize [sp_month] < sp_day) return (EINVALID);
		/*
		 *  Determine the day of the year from the month and day.
		 */
		while (i < sp_month) {
			sp_day += monthsize [i++];
		}
		sp_day--;		/* Map to [0..365] */
	}
	/*
	 *  At this time, the date is completely specified by year, and
	 *  day of year:
	 *
	 *	sp_year:	[1970..2037]
	 *	sp_day:		[0..365]
 	 *
	 *  Now, determine the time of day.
	 */
	if (sp_hour == UNDEF) sp_hour = 0;	/* Default; mins and secs */
						/* were already 0 by default */
	if (sp_hour == 24) {			/* Only allow: 24:00:00 */
		if (sp_minute || sp_second) return (EINVALID);
	}
	else if (sp_hour > 23 || sp_minute > 59 || sp_second > 59) {
		return (EINVALID);
	}
	if (sp_meridian != UNDEF) {		/* Not using 24-hour clock */
		if (sp_hour > 12) return (EINVALID);
		if (sp_hour == 12) {
			if (sp_minute || sp_second) {
				if (sp_meridian != MERIDIAN_M) {
					/*
					 *  12:00:01-12:59:59-am
					 *	maps to:  0:00:01- 0:59:59.
					 *
					 *  12:00:01-12:59:59-pm
					 *	maps to: 12:00:01-12:59:59.
					 *	("PM" times get 12 added to
					 *	 sp_hour below....)
					 */
					sp_hour = 0;
				}
			}
			else if (sp_meridian == MERIDIAN_AM) {
				/*
				 *  12:00:00-am maps to 0:00:00.
				 */
				sp_hour = 0;
			}
			/*
			 *  "PM" times get 12 added to sp_hour below....
			 *  "M" times need no compensation.
			 */
		}
		else if (sp_meridian == MERIDIAN_M) return (EINVALID);
		if (sp_meridian == MERIDIAN_PM) sp_hour += 12;
	}
	/*
	 *  The time and date are now completely specified by:
	 *
	 *	sp_year:	[1970..2037]
	 *	sp_day:		[0..365]
	 *	sp_hour:	[0..23] or 24, (in which case sp_minute
	 *					and sp_second = 0).
	 *	sp_minute:	[0..59]
	 *	sp_second:	[0..59]
	 *
	 *  Now compute the time in seconds since 0:00:00 January 1,
	 *  1970 GMT to the specified time.
	 */
	timeinsecs = 0;
	i = 1970;
	while (i < sp_year) {
		timeinsecs += yearsize(i);	/* Don't say i++ here because */
		i++;				/* yearsize() is a macro! */
	}
	timeinsecs += sp_day;			/* + day of year [0..365] */
	/*
	 *  Determine the day of the week for the specified date.
	 *  January 1, 1970 was a Thurday.
	 */
	sp_weekday = (timeinsecs + 4) % 7;	/* Day of week: used by */
						/* sunday(). */
	timeinsecs *= 24;			/* Convert to hours */
	timeinsecs += sp_hour;			/* + hours [0..24] */
	timeinsecs *= 60;			/* Convert to minutes */
	if (sp_zone == UNDEF) {
		/*
		 *  No timezone was specified.  Use the local time zone.
		 */
		timeinsecs += local_zone;	/* + minutes west from GMT */
#ifdef SDSC
		/* The start of DST (daylbigin) has changes to the beginning 
		 * of April. tm_isdst will do.
		 */
		  
                if (tm->tm_isdst) {
#else

		/*
		 *  Now, adjust for daylight savings time.
		 */
		daylbegin = 119;		/* Last Sunday in April */
		daylend = 303;			/* Last Sunday in October */
		if (sp_year == 1974) {		/* 1974 was wierd */
			daylbegin = 5;		/* January 6... */
			daylend = 333;		/* to last Sunday in Nov. */
		}
		else if (sp_year == 1975) {	/* 1975 was also strange */
			daylbegin = 58;		/* Last Sunday in Feb... */
			daylend = 303;		/* to last Sunday in Oct */
		}
		daylbegin = sunday (daylbegin);
		daylend = sunday (daylend);
		if (
#if	SGI | SYS52 | UTS | UNICOS | OSF
			daylight &&		/* Daylight conversion enabled*/
#else
#if	BSD42 | BSD43 | ULTRIX
			tz.tz_dsttime && 	/* Daylight conversion enabled*/
#else
BAD SYSTEM TYPE
#endif
#endif
		   (sp_day > daylbegin || (sp_day==daylbegin && sp_hour>=2)) &&
		   (sp_day < daylend || (sp_day==daylend && sp_hour < 1))) {
			/*
			 *  Backup time by one hour, since the date is
			 *  specified in a daylight savings time interval.
			 */
#endif SDSC
			timeinsecs -= 60;	/* Less one hour in minutes */
		}
	}
	else timeinsecs += sp_zone;		/* + minutes west from GMT */
	timeinsecs += sp_minute;		/* + minutes [0..59] */
	timeinsecs *= 60;			/* Convert to seconds */
	timeinsecs += sp_second;		/* + seconds [0..59] */
	*res_time = timeinsecs;			/* Set scanned time */
	return (ISUCCESS);			/* Successful completion */
}


/*** sunday
 *
 *
 *	int sunday():
 *
 *	Return the day of year number of the first Sunday on or after the
 *	specified day of year.
 */
static int sunday (d)
register int d;					/* Day of year */
{
	if (d >= 58) {				/* For leap years.... */
		d += yearsize (sp_year) - 365;	/* we add a day past Feb 28 */
	}
	return (d - (d - sp_day + sp_weekday + 700) % 7);
}


/*** scan
 *
 *
 *	int scan():
 *	Get next token.
 *
 *	Returns:
 *		T_<tokentype>.
 */
static int scan (noalpha)
short noalpha;				/* Non-zero if no alpha-termination */
					/* is allowed for digit string */
{
	register int ch;		/* Scan character */
	register int length;		/* Current literal length */
	register int ivalue;		/* Integer value being computed */

	ch = *scanptr;
	while (ch == ' ' || ch == '\t' || ch == ',') {
		/*
		 *  Scan whitespace.
		 */
		ch = *++scanptr;	/* Increment scan; get next char */
	}
	switch (ch) {
	case '\0':
	case '\n':
		t_token = T_EOS;
		return(T_EOS);		/* End of string */
	case '-':
		scanptr++;		/* Increment scan pointer */
		t_token = T_DASH;
		return(T_DASH);
	case '/':
		scanptr++;		/* Increment scan pointer */
		t_token = T_SLASH;
		return(T_SLASH);
	case ':':
		scanptr++;		/* Increment scan pointer */
		t_token = T_COLON;
		return(T_COLON);
	}
	if (ch >= '0' && ch <= '9') {
		/*
		 *  Scan integer.
		 */
		ivalue = 0;		/* Initialize ivalue. */
		do {
			ivalue *= 10;		/* Multiply by 10 */
			ivalue += ch - '0';	/* and add in digit */
			ch = *++scanptr;	/* Get next char */
		} while (ch >= '0' && ch <= '9' && ivalue <= 999);
		if (ch >= '0' && ch <= '9') {	/* Integer is > 9999 */
			t_token = T_ERROR;
			return(T_ERROR);	/* Syntax error */
		}
		if (noalpha &&			/* No alphabetic */
		   (ch >= 'a' && ch <= 'z' ||	/* termination is */
		    ch >= 'A' && ch <= 'Z')) {	/* allowed */
			t_token = T_ERROR;
			return(T_ERROR);	/* Syntax error */
		}
		t_integer = ivalue;	/* Save result */
		t_token = T_INT;
		return(T_INT);		/* Token was an integer */
	}
	if (ch >= 'A' && ch <= 'Z') {	/* Character is uppercase */
		ch += 'a' - 'A';	/* Convert to lowercase */
	}
	if (ch >= 'a' && ch <= 'z') {
		/*
		 *  Scan a literal.
		 */
		length = 0;		/* Current literal length */
		do {
			t_literal [length++] = ch;	/* Store char */
			ch = *++scanptr;		/* Get next char */
			if (ch >= 'A' && ch <= 'Z') {	/* Char is uppercase */
				ch += 'a' - 'A';	/* Convert to lowcase */
			}
		} while (ch >= 'a' && ch <= 'z' && length < MAX_LITSIZE);
		t_literal [length] = '\0';		/* Null terminator */
		if (ch >= 'a' && ch <= 'z') {
			t_token = T_ERROR;
			return(T_ERROR);	/* Token is too long */
		}
		t_token = T_LITERAL;
		return(T_LITERAL);	/* Token was a literal */
	}
	t_token = T_ERROR;
	return(T_ERROR);		/* Bad token */
}


/*** match3
 *
 *
 *	int match3():
 *
 *	Perform string comparison where at least the first three (3)
 *	characters must match.
 *
 *	Returns:
 *		1: if a match is made.
 *		0: if no match.
 */
static int match3 (str1, str2)
register char *str1;			/* String #1 */
register char *str2;			/* String #2 */
{
	if (*str1++ == *str2++) {
		if (*str1++ == *str2++) {
			if (*str1++ == *str2++) {
				/*
				 *  The first 3 characters match.
				 */
				while (*str1 && *str2) {
					if (*str1++ != *str2++) return (0);
				}
				return (1);	/* They match */
			}
		}
	}
	return (0);		/* They do not match */
}


/*** meridian
 *
 *
 *	int meridian():
 *
 *	Rule:
 *		<meridian> := <am_lit>
 *			   |  <pm_lit>
 *			   |  <m_lit>
 *
 *		<am_lit> := Any literal whose case insensitive
 *			    representation is equivalent to the
 *			    literal: "AM".
 *
 *		<pm_lit> := Any literal whose case insensitive
 *			    representation is equivalent to the
 *			    literal: "PM".
 *
 *		<m_lit>  := Any literal whose case insensitive
 *			    representation is equivalent to the
 *			    literal: "M".
 *
 *	Returns:
 *		SUCCESS or FAILURE.  If successful, then the value of
 *		the static short integer:  sp_meridian  is set to either
 *		MERIDIAN_AM or MERIDIAN_PM, as appropriate.
 */
static int meridian()
{
	if (t_token == T_LITERAL) {
		if (strcmp (t_literal, "am") == 0) {
			sp_meridian = MERIDIAN_AM;
			return (SUCCESS);
		}
		else if (strcmp (t_literal, "pm") == 0) {
			sp_meridian = MERIDIAN_PM;
			return (SUCCESS);
		}
		else if (strcmp (t_literal, "m") == 0) {
			sp_meridian = MERIDIAN_M;
			return (SUCCESS);
		}
	}
	return (FAILURE);		/* No AM or PM meridian spec found */
}


/*** day
 *
 *
 *	int day():
 *
 *	Rule:
 *		<day>		:= <day-lit> .
 *				|  <day-lit>
 *				|  <yesterday-lit>
 *				|  <today-lit>
 *				|  <tomorrow-lit>
 *
 *		<day-lit>	:= <sun-lit>
 *				|  <mon-lit>
 *				|  <tue-lit>
 *				|  <wed-lit>
 *				|  <thu-lit>
 *				|  <fri-lit>
 *				|  <sat-lit>
 *
 *		<yesterday-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "YESTERDAY".
 *
 *		<today-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "TODAY".
 *
 *		<tomorrow-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "TOMORROW".
 *
 *		<sun-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "SUNDAY".
 *
 *		<mon-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "MONDAY".
 *
 *		<tue-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "TUESDAY".
 *
 *		<wed-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "WEDNESDAY".
 *
 *		<thu-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "THURSDAY".
 *
 *		<fri-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "FRIDAY".
 *
 *		<sat-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "SATURDAY".
 *
 *	Returns SUCCESS, FAILURE, or ERROR.
 */
static int day()
{
	static char *days [] = {
		"sunday",			/* Day 0 */
		"monday",			/* Day 1 */
		"tuesday",			/* Day 2 */
		"wednesday",			/* Day 3 */
		"thursday",			/* Day 4 */
		"friday",			/* Day 5 */
		"saturday",			/* Day 6 */
		"yesterday",			/* Day 7 #define YESTERDAY 7 */
		"today",			/* Day 8 #define TODAY     8 */
		"tomorrow"			/* Day 9 #define TOMORROW  9 */
	};

	register short i;			/* Day name index */

	i = 0;					/* First day */
	do {
		if (match3 (t_literal, days [i])) {
			if (*scanptr == '.') scanptr++;	/* Scan any '.' */
			if (sp_weekday != UNDEF) {
				/*
				 *  Multiple day name specifications
				 *  were given.
				 */
				return (ERROR);
			}
			sp_weekday = i;		/* Save day [0..6] */
			return (SUCCESS);	/* Success */
		}
		i++;				/* Try the next day */
	} while (i < 10);			/* Until end of list.... */
	return (FAILURE);		/* No <day> specification found */
}


/*** month
 *
 *
 *	int month():
 *
 *	Rule:
 *		<month>		:= <month_name>
 *				|  <month_name> .
 *
 *		<month_name>	:= <jan_lit>
 *				|  <feb_lit>
 *				|  <mar_lit>
 *				|  <apr_lit>
 *				|  <may_lit>
 *				|  <jun_lit>
 *				|  <jul_lit>
 *				|  <aug_lit>
 *				|  <sep_lit>
 *				|  <oct_lit>
 *				|  <nov_lit>
 *				|  <dec_lit>
 *
 *		<jan-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "JANUARY".
 *
 *		<feb-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "FEBRUARY".
 *
 *		<mar-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "MARCH".
 *
 *		<apr-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "APRIL".
 *
 *		<may-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "MAY".
 *
 *		<jun-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "JUNE".
 *
 *		<jul-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "JULY".
 *
 *		<aug-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "AUGUST".
 *
 *		<sep-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "SEPTEMBER".
 *
 *		<oct-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "OCTOBER".
 *
 *		<nov-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "NOVEMBER".
 *
 *		<dec-lit>	:= Any literal containing three (3) or
 *				   more characters whose case insensitive
 *				   representation is equivalent to the
 *				   exact spelling or any prefix of the
 *				   literal: "DECEMBER".
 *
 *	Returns SUCCESS, FAILURE, or ERROR.
 */
static int month()
{
	static char *months [] = {
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
		"december",
	};

	register short i;		/* Month table index */

	i = 0;				/* First month */
	do {
		if (match3 (t_literal, months [i++])) {
			if (*scanptr == '.') scanptr++;	/* Scan any '.' */
			if (sp_month != UNDEF) {
				/*
				 *  Multiple month name specifications
				 *  were given.
				 */
				return (ERROR);
			}
			sp_month = i;		/* Save month [1..12] */
			return (SUCCESS);	/* Success */
		}
	} while (i < 12);		/* Until end of list.... */
	return (FAILURE);		/* No <day> specification found */
}


/*** zone
 *
 *
 *	int zone():
 *
 *	Rule:
 *		<zone>		:= <gmt-lit>
 *				|  <zulu-lit>
 *				|  <nst-lit>
 *				|  <ast-lit>
 *				|  <est-lit>
 *				|  <cst-lit>
 *				|  <mst-lit>
 *				|  <pst-lit>
 *				|  <yst-lit>
 *				|  <hst-lit>
 *				|  <bst-lit>
 *				|  <adt-lit>
 *				|  <edt-lit>
 *				|  <cdt-lit>
 *				|  <mdt-lit>
 *				|  <pdt-lit>
 *				|  <ydt-lit>
 *				|  <hdt-lit>
 *				|  <bdt-lit>
 *
 *		<gmt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "GMT".  (Specifies Greenwich
 *				   Mean Time.)
 *
 *		<zulu-lit>	:= Z	(identical in meaning to GMT.)
 *
 *		<nst-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "NST".  (Specifies
 *				   Newfoundland Standard Time at 3 hours
 *				   and 30 minutes past GMT; note that
 *				   there is no NDT.)
 *
 *		<ast-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "AST".  (Specifies Atlantic
 *				   Standard Time at 4 hours past GMT.)
 *
 *		<est-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "EST".  (Specifies Eastern
 *				   Standard Time at 5 hours past GMT.)
 *
 *		<cst-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "CST".  (Specifies Central
 *				   Standard Time at 6 hours past GMT.)
 *
 *		<mst-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "MST".  (Specifies Mountain
 *				   Standard Time at 7 hours past GMT.)
 *
 *		<pst-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "PST".  (Specifies Pacific
 *				   Standard Time at 8 hours past GMT.)
 *
 *		<yst-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "YST".  (Specifies Yukon
 *				   Standard Time at 9 hours past GMT.)
 *
 *		<hst-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "HST".  (Specifies Hawaii
 *				   Standard Time at 10 hours past GMT.)
 *
 *		<bst-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "BST".  (Specifies Bering
 *				   Standard Time at 11 hours past GMT.)
 *
 *		<adt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "ADT".  (Specifies Atlantic
 *				   Daylight Time at 3 hours past GMT.)
 *
 *		<edt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "EDT".  (Specifies Eastern
 *				   Daylight Time at 4 hours past GMT.)
 *
 *		<cdt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "CDT".  (Specifies Central
 *				   Daylight Time at 5 hours past GMT.)
 *
 *		<mdt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "MDT".  (Specifies Mountain
 *				   Daylight Time at 6 hours past GMT.)
 *
 *		<pdt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "PDT".  (Specifies Pacific
 *				   Daylight Time at 7 hours past GMT.)
 *
 *		<ydt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "YDT".  (Specifies Yukon
 *				   Daylight Time at 8 hours past GMT.)
 *
 *		<hdt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "HDT".  (Specifies Hawaii
 *				   Daylight Time at 9 hours past GMT.)
 *
 *		<bdt-lit>	:= Any literal whose case insensitive
 *				   representation is equivalent to the
 *				   literal: "BDT".  (Specifies Bering
 *				   Daylight Time at 10 hours past GMT.)
 *
 *	Returns SUCCESS, FAILURE, or ERROR.
 */
static int zone()
{
	static struct zone_descr {
		char *zonename;			/* Timezone name */
		short mins_west;		/* Minutes west from GMT */
	} zone_set [] = {
		{ "gmt", 0 },
		{ "z", 0 },
		{ "nst",  3 * 60 + 30 },
		{ "ast",  4 * 60 },
		{ "est",  5 * 60 },
		{ "cst",  6 * 60 },
		{ "mst",  7 * 60 },
		{ "pst",  8 * 60 },
		{ "yst",  9 * 60 },
		{ "hst", 10 * 60 },
		{ "bst", 11 * 60 },
		{ "adt",  3 * 60 },
		{ "edt",  4 * 60 },
		{ "cdt",  5 * 60 },
		{ "mdt",  6 * 60 },
		{ "pdt",  7 * 60 },
		{ "ydt",  8 * 60 },
		{ "hdt",  9 * 60 },
		{ "bdt", 10 * 60 },
		{ (char *)0, 0 }		/* End of list */
	};

	register struct zone_descr *descr;

	descr = zone_set;			/* First element of zone set */
	do {
		if (strcmp (descr->zonename, t_literal) == 0) {
			if (sp_zone != UNDEF) {
				/*
				 * Multiple time zone name
				 * specifications given.
				 */
				return (ERROR);
			}
			sp_zone = descr->mins_west;
			return (SUCCESS);	/* Success */
		}
		descr++;			/* Try the next timezone */
	} while (descr->zonename != (char *)0);	/* Until end of list.... */
	return (FAILURE);			/* Failure */
}
