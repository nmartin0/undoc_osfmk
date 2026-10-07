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
 
/*++ scancpulim.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/scancpulim.c,v $
 *
 * DESCRIPTION:
 *
 *	Scan a maximum cpu time limit and
 *	an optional warning cpu time limit.
 *	This used by qsub and qmod.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:42 $ $State: Exp $)
 * $Log: scancpulim.c,v $
 * Revision 1.3  1994/11/19  02:27:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:37  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:49  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:54:32  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "quolim.h"			/* for struct cpulimit */

#define TRUE 1
#define FALSE 0

/*
 *
 *	Token types. Interface between scan1cpulim() and scan().
 */
#define	T_COLON		0		/* : */
#define	T_EOS		1		/* End of string found */
#define	T_ERROR		2		/* Bad token (not overflow) */
#define	T_INT32		3		/* an integer < 2**32 */
#define	T_OVERFLOW	4		/* an integer >= 2**32 */
#define	T_PERIOD	5		/* . */
#define T_SHORT		6		/* 0 <= x <= 1000 (not 999) */
					/* "1000" means "0, carry 1" */
/*
 *
 *	Scan cpu limit return codes.
 *	Interface between scancpulim() and scan1cpulim().
 *	Interface between outside world and scancpulim().
 */
#define	ISUCCESS	0		/* Valid cpu limit */
#define	ESYNTAX		(-1)		/* Syntax error in cpu limit */
#define	EOVERFLOW	(-2)		/* Syntax ok; semantic (e.g. 11:61) */
					/* or overflow error in cpu limit */
#define	EUNDEF		(-3)		/* No cpu limit specified */
#define EBADWARN	(-4)		/* Warning greater than max limit */

/*
 *
 *	Variables local to this module.
 */
static char *scanptr;			/* Next char to be scanned */
static unsigned long int t_int32;	/* Scanned seconds value */
static short t_short;			/* Scanned milliseconds value */

/*** scancpulim
 *
 *
 *	int scancpulim():
 *	Scan a maximum cpu limit and an optional warning cpu limit.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if the specification contains a syntax error;
 *	       -2: if the syntax is correct but the limit is invalid
 *		   (i.e 11:61, 4294967296 (==2**32));
 *	       -3: if no limit was specified;
 *	       -4: if the warning limit was greater than the maximum limit.
 *
 *	There are seven legal formats for a cpu limit:
 *		
 *		HH:MM:SS.MS
 *		HH:MM:SS
 *		MM:SS.MS
 *		MM:SS
 *		SS.MS
 *		SS
 *		.MS
 *		
 *		where HH, MM, SS, and MS are base 10 digit strings
 *		of length anywhere from one digit to 10 digits.
 *		HH stands for hours, MM for minutes, SS for seconds,
 *		and MS for milliseconds.
 * 
 *	HH, MM, and SS, summed as seconds, must fit in an unsigned
 *		32 bit integer.  Furthermore, if hours are indicated,
 *		minutes must be <= 59, and if minutes are indicated,
 *		seconds must be <= 59. Otherwise, minutes or seconds
 *		may exceed 59.
 *
 *	MS will be interpreted as the fractional part of a base 10 number
 *		(i.e., 1.01 means one second and TEN milliseconds,
 *		not one second and ONE millisecond).
 *		If MS is longer than three digits, it will be rounded
 *		to the closest millisecond.
 *
 *	There may be whitespace anywhere except:
 *		within a digit string or
 *		adjacent to a decimal point.
 * 
 *	There must be whitespace between a maximum cpu limit
 *		and a warning cpu limit.
 *
 */
int scancpulim (cpulimstr, cpulimp, maxonly)
char *cpulimstr;			/* what is to be parsed */
struct cpulimit *cpulimp;		/* where to put the result */
short maxonly;				/* if non-zero, warn limit illegal */
{
	int trouble;			/* return value of scan1cpulim() */
	unsigned long myulong;		/* dummy arg; never looked at */
	short myshort;			/* dummy arg; never looked at */

	cpulimp->max_seconds = 0;	/* The struct cpulimit is to be */
	cpulimp->max_ms = 0;		/* zeroed in the event of error */
	cpulimp->warn_seconds = 0;	/* return.  We'll assume an	*/
	cpulimp->warn_ms = 0;		/* error return for now.	*/
	
	scanptr = cpulimstr;
	
	/*
	 * Below, fill in max_seconds and max_ms.
	 * If there is an error, max_seconds and max_ms
	 * will not be touched. Scanptr
	 * is left pointing to the first character that
	 * proves we have reached the end of the first limit.
	 */
	
	trouble = scan1cpulim (&cpulimp->max_seconds, &cpulimp->max_ms);
	
	if (trouble != ISUCCESS ) return (trouble);
	
	/*
	 * Preset warn limits to max limits
	 * for the case where no warn limit is specified.
	 */
	 
	cpulimp->warn_seconds = cpulimp->max_seconds;
	cpulimp->warn_ms = cpulimp->max_ms;
	
	/*
	 * Check if it is OK for a warn limit to appear.
	 * If not, then there had better be nothing left.
	 */
	 
	if (maxonly) {
		trouble = scan1cpulim (&myulong, &myshort);
		if (trouble != EUNDEF) {
			cpulimp->max_seconds = 0;
			cpulimp->max_ms = 0;
			cpulimp->warn_seconds = 0;
			cpulimp->warn_ms = 0;
			return (ESYNTAX);
		}
		return (ISUCCESS);
	}
	
	/*
	 * It is OK for a warn limit to appear.
	 * Below, parse it into warn_seconds and warn_ms.
	 * If there is an error, warn_seconds and warn_ms
	 * will not be touched. If the warn limit is
	 * non-null, the scanner's character pointer is 
	 * left pointing to the first character that
	 * proves we have reached the end of the warn limit.
	 */
	 
	trouble = scan1cpulim (&cpulimp->warn_seconds, &cpulimp->warn_ms);
	
	/*
	 * If there was an error ( other than no warn limit at all),
	 * do not continue.
	 */
	 
	if (trouble != ISUCCESS && trouble != EUNDEF) {
		cpulimp->max_seconds = 0;
		cpulimp->max_ms = 0;
		cpulimp->warn_seconds = 0;
		cpulimp->warn_ms = 0;
		return (trouble);
	}
	
	/*
	 * Report error if the warning was greater than the max.
	 */

	if (
		(cpulimp->warn_seconds > cpulimp->max_seconds) ||
		((cpulimp->warn_seconds == cpulimp->max_seconds) &&
		 (cpulimp->warn_ms > cpulimp->max_ms))
	   ) {
		cpulimp->max_seconds = 0;
		cpulimp->max_ms = 0;
		cpulimp->warn_seconds = 0;
		cpulimp->warn_ms = 0;
		return (EBADWARN);
	}
	
	/*
	 * Below, there had better not be anything left.
	 */
	 
	trouble = scan1cpulim (&myulong, &myshort);
	
	if (trouble != EUNDEF) {
		cpulimp->max_seconds = 0;
		cpulimp->max_ms = 0;
		cpulimp->warn_seconds = 0;
		cpulimp->warn_ms = 0;
		return (ESYNTAX);
	}
	
	return (ISUCCESS);
}				/* end of scancpulim() */


/*** scan1cpulim
 *
 *
 *	int scan1cpulim():
 *	Assign the seconds and milliseconds parts of the next cpu limit
 *	into the places pointed to by the arguments.
 *
 *	The set of possible return values of scan1cpulim() and
 *	the set of possible return values of scancpulim()
 *	are the same, with the exception that scan1cpulim()
 *	is unable to detect EBADWARN.
 *	
 *	Scan1cpulim() skips over whitespace itself, rather
 *	than having scan() do it. This means that "1.2"
 *	can be three tokens, instead of having to be one token.
 *
 */
static int scan1cpulim (ulongp, shortp)
unsigned long *ulongp;
short *shortp;
{
	unsigned long seconds;			/* running count */
	
	while (*scanptr == ' ' || *scanptr == '\t' || *scanptr == ',') {
		scanptr++;
	}					/* nothing seen yet */
	switch (scan (FALSE)) {
	case T_COLON:
	case T_ERROR:
	case T_SHORT:			/* T_SHORT impossible here */
		return (ESYNTAX);
	case T_EOS:
		return (EUNDEF);
	case T_INT32:			/* seen HH, MM, or SS */
		seconds = t_int32;
		if (*scanptr == '.') {
			scanptr++;	/* move scanptr to 1/10ths place */
			if (scan (TRUE) != T_SHORT) {
				return (ESYNTAX);
			}
			else {
				if (*scanptr == '.') {	/* a.b. illegal */
					return (ESYNTAX);
				}
				if ( t_short == 1000) {	/* carry */
					if (seconds == 4294967295) {
						return (EOVERFLOW);
					}
					else {
						t_short = 0;
						seconds++;
					}
				}
				*ulongp = seconds;
				*shortp = t_short;
				return (ISUCCESS);
			}
		}
		while (			/* move scanptr to first non-white */
			*scanptr == ' ' ||
			*scanptr == '\t' ||
			*scanptr == ','
		      ) {
			scanptr++;
		}
		if (*scanptr != ':') {
			*ulongp = seconds;
			*shortp = 0;
			return (ISUCCESS);
		}
		scanptr++;		/* move scanptr past colon */
					/* seen HH: or MM: */
		while (			/* move scanptr to first non-white */
			*scanptr == ' ' ||
			*scanptr == '\t' ||
			*scanptr == ','
		      ) {
			scanptr++;
		}
		switch (scan (FALSE)) {
		case T_COLON:
		case T_EOS:
		case T_ERROR:
		case T_PERIOD:
		case T_SHORT:			/* T_SHORT impossible here */
			return (ESYNTAX);
		case T_INT32:			/* seen HH:MM or MM:SS */
			if (	/* overflow check */
				/* (71582788+16/60)*60 is smallest overflow */
				(t_int32 > 59) ||
				(seconds > 71582788L) ||
				((seconds == 71582788L) && (t_int32 >15))
			   ) {
				return (EOVERFLOW);
			}
			seconds *= 60;
			seconds += t_int32;
			if (*scanptr == '.') {
				scanptr++;	/* move scanptr to 1/10ths */
				if (scan (TRUE) != T_SHORT) {
					return (ESYNTAX);
				}
				else {
					if (*scanptr == '.') { /* a:b.c. */
						return (ESYNTAX);
					}
					if ( t_short == 1000) {	/* carry */
						/* 4294967295 = 2**32-1 */
						if (seconds == 4294967295) {
							return (EOVERFLOW);
						}
						else {
							t_short = 0;
							seconds++;
						}
					}
					*ulongp = seconds;
					*shortp = t_short;
					return (ISUCCESS);
				}
			}
			break;
		case T_OVERFLOW:
			return (EOVERFLOW);
		}
		while (			/* move scanptr to first non-white */
			*scanptr == ' ' ||
			*scanptr == '\t' ||
			*scanptr == ','
		      ) {
			scanptr++;
		}
		if (*scanptr != ':') {
			*ulongp = seconds;
			*shortp = 0;
			return (ISUCCESS);
		}
		scanptr++;		/* move scanptr past colon */
					/* seen HH:MM: */
		while (			/* move scanptr to first non-white */
			*scanptr == ' ' ||
			*scanptr == '\t' ||
			*scanptr == ','
		      ) {
			scanptr++;
		}
		switch (scan (FALSE)) {
		case T_COLON:
		case T_EOS:
		case T_ERROR:
		case T_PERIOD:
		case T_SHORT:		/* T_SHORT impossible here */
			return (ESYNTAX);
		case T_INT32:			/* seen HH:MM:SS */
			if (	/* overflow check */
				/* (71582788+16/60)*60 is smallest overflow */
				(t_int32 > 59) ||
				(seconds > 71582788L) ||
				((seconds == 71582788L) &&
				(t_int32 >15))
			   ) {
				return (EOVERFLOW);
			}
			seconds *= 60;
			seconds += t_int32;
			if (*scanptr == '.') {
				scanptr++;	/* move scanptr to 1/10ths */
				if (scan (TRUE) != T_SHORT) {
					return (ESYNTAX);
				}
				else {
					if (*scanptr == '.') {
						/* a:b:c.d. */
						return (ESYNTAX);
					}
					if ( t_short == 1000) {	/* carry */
						/* 4294967295 = 2**32-1 */
						if (seconds == 4294967295) {
							return (EOVERFLOW);
						}
						else {
							t_short = 0;
							seconds++;
						}
					}
					*ulongp = seconds;
					*shortp = t_short;
					return (ISUCCESS);
				}
			}
			*ulongp = seconds;
			*shortp = 0;
			return (ISUCCESS);
		case T_OVERFLOW:
			return (EOVERFLOW);
		}
	case T_OVERFLOW:		/* back to top level switch here */
		return (EOVERFLOW);
	case T_PERIOD:			/* .MS case */
		if (scan (TRUE) != T_SHORT) {
			return (ESYNTAX);
		}
		else {
			if (*scanptr == '.') {	/* .a. */
				return (ESYNTAX);
			}
			if (t_short == 1000) {	/* carry */
				*ulongp = 1;
				*shortp = 0;
			}
			else {
				*ulongp = 0;
				*shortp = t_short;
			}
			return (ISUCCESS);
		}
	}
}				/* end scan1cpulim() */


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
	register unsigned long i32value;/* seconds being computed */
	register short svalue;		/* milliseconds being computed */

	ch = *scanptr;
	switch (ch) {
	case '\0':
	case '\n':
		return(T_EOS);		/* end of string */
	case ':':
		scanptr++;		/* move scanptr beyond ':' */
		return(T_COLON);
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
				ch = *++scanptr;	/* move to 1/1000ths */
				if (ch >= '0' && ch <= '9') {
					svalue += ch - '0';
					ch = *++scanptr;  /* to 1/10000ths */
					if (ch >= '5' && ch <= '9') {
						svalue++;
					}
				}
				while (ch >= '0' && ch <= '9') {
					ch = *++scanptr;
					/* move scanptr past digit string */
				}
			}
			t_short = svalue;
			return (T_SHORT);
		}
		else {			/* HH, MM, or SS */
			i32value = 0;
			do {
				i32value *= 10;
				i32value += ch - '0';
				ch = *++scanptr;
			} while (
				ch >= '0' &&
				ch <= '9' &&
				i32value <= 429496728L
				);		/* end of do while */
				/* 429496728*10+6 is smallest overflow */
			if (i32value == 429496729L && ch >= '0' && ch <= '5') {
				i32value *= 10;
				i32value += ch - '0';
				ch = *++scanptr;
			}	
			if (ch >= '0' && ch <= '9') {
				return (T_OVERFLOW);
			}	/* don't have to move scanptr; fatal error */
			t_int32 = i32value;
			return (T_INT32);
		}
	}
	return(T_ERROR);		/* Bad token */
}					/* end scan() */
