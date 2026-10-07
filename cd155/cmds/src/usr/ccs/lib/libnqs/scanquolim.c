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
 
/*++ scanquolim.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/scanquolim.c,v $
 *
 * DESCRIPTION:
 *
 *	Scan a maximum quota limit and an optional warning quota limit.
 *	This used by qsub and qmgr.
 *
 *	It is assumed that the precision of an unsigned long
 *	is at least 32 bits. Scanquolim() always returns a
 *	coefficient < 2**31.
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	December 30, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:43 $ $State: Exp $)
 * $Log: scanquolim.c,v $
 * Revision 1.3  1994/11/19  02:27:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:39  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:50  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:54:38  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "quolim.h"	/* for struct quotalimit and quota limit units */

extern int secgrfir();			/* Boolean function, 1 if the */
					/* second pair > the first pair */
#define TRUE 1
#define FALSE 0

/*
 *
 *	Token types. Interface between scan1quolim() and scan().
 */
#define	T_EOS		0		/* end of string */
#define	T_ERROR		1		/* bad token (not overflow) */
#define	T_INT32		2		/* an integer < 2**32 */
#define	T_OVERFLOW	3		/* an integer >= 2**32 */
#define	T_PERIOD	4		/* . */
#define T_SHORT		5		/* fractional part: after rounding, */
					/* 0 <= x <= 1000 (not 999) */
#define T_UNITS		6		/* QLM_???, see quolim.h */

/*
 *
 *	Scan quota limit return codes.
 *	Interface between scanquolim() and scan1quolim().
 *	Interface between outside world and scanquolim().
 */
#define	ISUCCESS	0		/* valid quota limit */
#define	ESYNTAX		(-1)		/* syntax error in quota limit */
#define	EOVERFLOW	(-2)		/* syntax ok; overflow or underflow */
#define	EUNDEF		(-3)		/* no quota limit specified */
#define EBADWARN	(-4)		/* warn limit greater than max */
					/* limit, or apples and oranges */

/*
 *
 *	Variables local to this module.
 */
static char *scanptr;			/* next char to be scanned */
static unsigned long int t_int32;	/* scanned integer part */
static short t_short; 			/* scanned 1/1000ths */
static short t_units;			/* scanned units type (QLM_???) */

/*** scanquolim
 *
 *
 *	int scanquolim():
 *	Scan a maximum quota limit and an optional warning quota limit.
 *
 *	Returns:
 *		 0: if successful;
 *		-1: if the specification contains a syntax error;
 *		-2: if the syntax is correct but the limit is invalid
 *		    (i.e. .9 b, 100000000 (too big) b);
 *		-3: if no limit was specified;
 *		-4: if the warning limit was greater than the maximum
 *		    limit.
 *
 *	There are six legal forms for a quota limit:
 *
 *	AA
 *	AA.BB
 *	.BB
 *	AA units
 *	AA.BB units
 *	.BB units
 *
 *	where AA and BB are base 10 digit strings of length
 *	anywhere from one digit to 8 digits.
 *	BB is rounded to the nearest 1/1000th.
 *	If AA is >= 2**21, BB is ignored.
 *	If the "units" are not specified, bytes are assumed.
 *
 *	If BB, when rounded, is neither 0 nor 1000, then
 *	the units may not be bytes or words, for these
 *	cannot be divided.
 *
 *	Legal units are (case insensitive) "b" ,"w" ,"kb" ,"kw" ,
 *	"mb" ,"mw" ,"gb", and "gw", standing for
 *	bytes, words, kilobytes, kilowords, megabytes, megawords,
 *	gigabytes, and gigawords. .5 kilobyte == 512 bytes.
 *	.5 megabyte == 524288 bytes.  .5 gigabyte == 536870912 bytes.
 *
 *	There may be whitespace anywhere except:
 *		within a digit string or
 *		adjacent to a decimal point.
 *
 *	There must be whitespace between a maximum quota limit
 *		and a warning quota limit.
 *
 */
int scanquolim (quolimstr, quolimp, maxonly)
char *quolimstr;			/* what is to be parsed */
struct quotalimit *quolimp;		/* where to put the result */
short maxonly;				/* if non-zero, warn limit illegal */
{
	int trouble;			/* return value of scan1quolim() */
	unsigned long myulong;		/* dummy arg; never looked at */
	short myshort;			/* dummy arg; never looked at */

	quolimp->max_quota = 0;		/* The struct quotalimit is to be */
	quolimp->max_units = 0;		/* zeroed in the event of error */
	quolimp->warn_quota = 0;	/* return.  We'll assume an	*/
	quolimp->warn_units = 0;	/* error return for now.	*/
	
	scanptr = quolimstr;
	
	/*
	 * Below, fill in max_quota and max_units.
	 * If there is an error, max_quota and max_units
	 * will not be touched. Scanptr
	 * is left pointing to the first character that
	 * proves we have reached the end of the first limit.
	 */
	
	trouble = scan1quolim (&quolimp->max_quota, &quolimp->max_units);
	
	if (trouble != ISUCCESS ) return (trouble);
	
	/*
	 * Preset warn limits to max limits
	 * for the case where warn limit is illegal
	 * or no warn limit is specified.
	 */
	 
	quolimp->warn_quota = quolimp->max_quota;
	quolimp->warn_units = quolimp->max_units;
	
	/*
	 * Check if it is OK for a warn limit to appear.
	 * If not, then there had better be nothing left.
	 */
	 
	if (maxonly) {
		trouble = scan1quolim (&myulong, &myshort);
		if (trouble != EUNDEF) {
			quolimp->max_quota = 0;
			quolimp->max_units = 0;
			quolimp->warn_quota = 0;
			quolimp->warn_units = 0;
			return (ESYNTAX);
		}
		return (ISUCCESS);
	}
	
	/*
	 * It is OK for a warn limit to appear.
	 * Below, parse it into warn_quota and warn_units.
	 * If there is an error, warn_quota and warn_units
	 * will not be touched. If the warn limit is
	 * non-null, the scanner's character pointer is 
	 * left pointing to the first character that
	 * proves we have reached the end of the warn limit.
	 */
	 
	trouble = scan1quolim (&quolimp->warn_quota, &quolimp->warn_units);
	
	/*
	 * If there was an error ( other than no warn limit at all),
	 * do not continue.
	 */
	 
	if (trouble != ISUCCESS && trouble != EUNDEF) {
		quolimp->max_quota = 0;
		quolimp->max_units = 0;
		quolimp->warn_quota = 0;
		quolimp->warn_units = 0;
		return (trouble);
	}
	
	/*
	 * Report error if the warning was greater than
	 * the maximum, or they were incomparable.
	 */

	if (secgrfir (quolimp->max_quota, quolimp->max_units,
				quolimp->warn_quota, quolimp->warn_units)) {
		quolimp->max_quota = 0;
		quolimp->max_units = 0;
		quolimp->warn_quota = 0;
		quolimp->warn_units = 0;
		return (EBADWARN);
	}
	
	/*
	 * Below, there had better not be anything left.
	 */
	 
	trouble = scan1quolim (&myulong, &myshort);
	
	if (trouble != EUNDEF) {
		quolimp->max_quota = 0;
		quolimp->max_units = 0;
		quolimp->warn_quota = 0;
		quolimp->warn_units = 0;
		return (ESYNTAX);
	}
	
	return (ISUCCESS);
}				/* end of scanquolim() */

/*** scan1quolim
 *
 *
 *	int scan1quolim():
 *
 *	Assign the quota and units parts of the next quota limit
 *	into the places pointed to by the arguments.
 *
 *	The set of possible return values of scan1quolim() and
 *	the set of possible return values of scanquolim()
 *	are the same, with the exception that scan1quolim()
 *	is unable to detect EBADWARN.
 *	
 *	Scan1quolim() skips over whitespace itself, rather
 *	than having scan() do it. This means that "1.2"
 *	can be three tokens, instead of having to be one token.
 */
static int scan1quolim (ulongp, shortp)
unsigned long *ulongp;
short *shortp;
{
	
	t_int32 = 0;			/* set defaults */
	t_short = 0;
	t_units = QLM_BYTES;
	
	while (*scanptr == ' ' || *scanptr == '\t' || *scanptr == ',') {
		scanptr++;
	}					/* nothing seen yet */
	switch (scan (FALSE)) {
	case T_EOS:
		return (EUNDEF);
	case T_ERROR:
	case T_SHORT:			/* T_SHORT impossible here */
	case T_UNITS:
		return (ESYNTAX);
	case T_INT32:
		if (*scanptr == '.') {
			scanptr++;	/* move scanptr to 1/10ths place */
			if (scan (TRUE) != T_SHORT) {
				return (ESYNTAX);
			}
			if (*scanptr == '.') {	/* a.b. illegal */
				return (ESYNTAX);
			}
			/*
			 *  Below, where necessary, we check we
			 *  can x1024 with no overflow.
			 *  2097151 is the largest integer
			 *  that yields a number less that 2**31
			 *  when multiplied by 1024.
			 */
			if ( t_short != 0 &&
			     t_short != 1000 &&
			     t_int32 > 2097151L
			   ) {
				t_short = 0;	/* ignore fractional part */
			}
				/*
				 * carry if necessary
				 */
			if (t_short == 1000) {
				t_short = 0;
				t_int32++;
			}
				/* move scanptr to first non-white */
			while (
				*scanptr == ' ' ||
				*scanptr == '\t' ||
				*scanptr == ','
	      		) {
				scanptr++;
			}
				/* only scan units if they are there */
			if (
				((*scanptr >= 'a') && (*scanptr <= 'z')) ||
				((*scanptr >= 'A') && (*scanptr <= 'Z'))
			   ) {
				if (scan (FALSE) != T_UNITS) {
					return (ESYNTAX);
				}
				/*
				 * Units must be followed by
				 * whitespace or the end of the string.
				 */
				if (
					*scanptr != ' ' && *scanptr != '\t' &&
					*scanptr != ',' && *scanptr != '\n' &&
					*scanptr != '\0'
				   ) {
					return (ESYNTAX);
				}
			}
			break;
		}
				/* move scanptr to first non-white */
		while (
			*scanptr == ' ' ||
			*scanptr == '\t' ||
			*scanptr == ','
		      ) {
			scanptr++;
		}
				/* only scan units if they are there */
		if (
			((*scanptr >= 'a') && (*scanptr <= 'z')) ||
			((*scanptr >= 'A') && (*scanptr <= 'Z'))
		   ) {
			if (scan (FALSE) != T_UNITS) {
				return (ESYNTAX);
			}
			/*
			 * Units must be followed by
			 * whitespace or the end of the string.
			 */
			if (
				*scanptr != ' ' && *scanptr != '\t' &&
				*scanptr != ',' && *scanptr != '\n' &&
				*scanptr != '\0'
			   ) {
				return (ESYNTAX);
			}
		}
		break;
	case T_OVERFLOW:	/* back to top level switch cases */
		return (EOVERFLOW);
	case T_PERIOD:
		if (scan (TRUE) != T_SHORT) {
			return (ESYNTAX);
		}
			if (*scanptr == '.') {	/* .a. */
				return (ESYNTAX);
			}
			if (t_short == 1000) {	/* carry if necessary */
				t_short = 0;
				t_int32 = 1;
			}
			while (	/* move scanptr to first non-white */
				*scanptr == ' ' ||
				*scanptr == '\t' ||
				*scanptr == ','
		      	) {
				scanptr++;
			}
				/* only scan units if they are there */
			if (
				((*scanptr >= 'a') && (*scanptr <= 'z')) ||
				((*scanptr >= 'A') && (*scanptr <= 'Z'))
			   ) {
				if (scan (FALSE) != T_UNITS) {
					return (ESYNTAX);
				}
				/*
			 	 * Units must be followed by
			 	 * whitespace or the end of the string.
			 	 */
			 	if (
					*scanptr != ' ' && *scanptr != '\t' &&
					*scanptr != ',' && *scanptr != '\n' &&
					*scanptr != '\0'
				   ) {
					return (ESYNTAX);
				}
			}
		break;
	}			/* end of top level switch */
				/* ready to report a quota limit */
	/*
	 * In two of the eight cases below, we are already using
	 * the smallest units.  If there are fractional parts,
	 * we must report underflow.
	 * In the other six cases, we report fractional parts
	 * by switching to a unit 1024 times finer.
	 */
	switch (t_units) {
	case QLM_BYTES:
		if (t_short == 0) {
			*ulongp = t_int32;
			*shortp = QLM_BYTES;
			return (ISUCCESS);
		}
		else {
			return (EOVERFLOW);
		}
	case QLM_WORDS:
		if (t_short == 0) {
			*ulongp = t_int32;
			*shortp = QLM_WORDS;
			return (ISUCCESS);
		}
		else {
			return (EOVERFLOW);
		}
	case QLM_KBYTES:
		if (t_short == 0) {
			*ulongp = t_int32;
			*shortp = QLM_KBYTES;
			return (ISUCCESS);
		}
		else {
			*ulongp = (t_int32 * 1024) + (t_short * 1024) / 1000;
			*shortp = QLM_BYTES;
			return (ISUCCESS);
		}
	case QLM_KWORDS:
		if (t_short == 0) {
			*ulongp = t_int32;
			*shortp = QLM_KWORDS;
			return (ISUCCESS);
		}
		else {
			*ulongp = (t_int32 * 1024) + (t_short * 1024) / 1000;
			*shortp = QLM_WORDS;
			return (ISUCCESS);
		}
	case QLM_MBYTES:
		if (t_short == 0) {
			*ulongp = t_int32;
			*shortp = QLM_MBYTES;
			return (ISUCCESS);
		}
		else {
			*ulongp = (t_int32 * 1024) + (t_short * 1024) / 1000;
			*shortp = QLM_KBYTES;
			return (ISUCCESS);
		}
	case QLM_MWORDS:
		if (t_short == 0) {
			*ulongp = t_int32;
			*shortp = QLM_MWORDS;
			return (ISUCCESS);
		}
		else {
			*ulongp = (t_int32 * 1024) + (t_short * 1024) / 1000;
			*shortp = QLM_KWORDS;
			return (ISUCCESS);
		}
	case QLM_GBYTES:
		if (t_short == 0) {
			*ulongp = t_int32;
			*shortp = QLM_GBYTES;
			return (ISUCCESS);
		}
		else {
			*ulongp = (t_int32 * 1024) + (t_short * 1024) / 1000;
			*shortp = QLM_MBYTES;
			return (ISUCCESS);
		}
	case QLM_GWORDS:
		if (t_short == 0) {
			*ulongp = t_int32;
			*shortp = QLM_GWORDS;
			return (ISUCCESS);
		}
		else {
			*ulongp = (t_int32 * 1024) + (t_short * 1024) / 1000;
			*shortp = QLM_MWORDS;
			return (ISUCCESS);
		}
	}
}				/* end scan1quolim() */


/*** scan
 *
 *
 *	int scan():
 *	Get next token.
 *	See the comment about whitespace above.
 *
 *	Returns: T_<tokentype>.
 */
static int scan (asfracpart)
short asfracpart;			/* Non-zero if the following digits */
					/* are the fractional part */
{
	register int ch;		/* scan character */
	register unsigned long i32value;/* integer part being computed */
	register short svalue;		/* 1/1000ths being computed */

	ch = *scanptr;
	switch (ch) {
	case '\0':
	case '\n':
		return(T_EOS);		/* end of string */
	case '.':
		scanptr++;		/* move scanptr beyond '.' */
		return(T_PERIOD);
	}
	if (ch >= '0' && ch <= '9') {
		/*
		 *  Scan digit string.
		 */
		if (asfracpart) {
			svalue = 100 * (ch - '0');
			ch = *++scanptr;	/* move scanptr to 1/100ths */
			if (ch >= '0' && ch <= '9') {
				svalue += 10 * (ch - '0');
					/* move scanptr to 1/1000ths */
				ch = *++scanptr;
				if (ch >= '0' && ch <= '9') {
					svalue += ch - '0';
					ch = *++scanptr;
					if (ch >= '5' && ch <= '9') {
						svalue++;
					}
					while (ch >= '0' && ch <= '9') {
						ch = *++scanptr;
						/* move scanptr past digits */
					}
				}
			}
			t_short = svalue;
			return (T_SHORT);
		}
		else {			/* an integer, not a fractional part */
			i32value = 0;
			do {
				i32value *= 10;
				i32value += ch - '0';
				ch = *++scanptr;
			} while (
				ch >= '0' &&
				ch <= '9' &&
				i32value < 100000000L
				);		/* end of do while */

			/* 100000000 is the smallest overflow */
			if (i32value > 99999999L) {
				return (T_OVERFLOW);
			}	/* don't have to move scanptr; fatal error */
			t_int32 = i32value;
			return (T_INT32);
		}
	}
	/*
	 * Below, we leave scanptr pointing to the first character
	 * that is not part of the units specification.
	 */
	 if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
		switch (ch) {
		case 'b':
		case 'B':
			scanptr++;
			t_units = QLM_BYTES;
			return (T_UNITS);
		case 'w':
		case 'W':
			scanptr++;
			t_units = QLM_WORDS;
			return (T_UNITS);
		case 'k':
		case 'K':
			scanptr++;
			switch (*scanptr) {
			case 'b':
			case 'B':
				scanptr++;
				t_units = QLM_KBYTES;
				return (T_UNITS);
			case 'w':
			case 'W':
				scanptr++;
				t_units = QLM_KWORDS;
				return (T_UNITS);
			default:
				return (T_ERROR);
			}
		case 'm':
		case 'M':
			scanptr++;
			switch (*scanptr) {
			case 'b':
			case 'B':
				scanptr++;
				t_units = QLM_MBYTES;
				return (T_UNITS);
			case 'w':
			case 'W':
				scanptr++;
				t_units = QLM_MWORDS;
				return (T_UNITS);
			default:
				return (T_ERROR);
			}
		case 'g':
		case 'G':
			scanptr++;
			switch (*scanptr) {
			case 'b':
			case 'B':
				scanptr++;
				t_units = QLM_GBYTES;
				return (T_UNITS);
			case 'w':
			case 'W':
				scanptr++;
				t_units = QLM_GWORDS;
				return (T_UNITS);
			default:
				return (T_ERROR);
			}
		default:
			return (T_ERROR);
		}
	}
	return(T_ERROR);
}					/* end scan() */

