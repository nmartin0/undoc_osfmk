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
static char rcsid[] = "@(#)$RCSfile: strcoll.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:28 $";
#endif
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: strcoll, wcscoll
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
 *
 * strcoll.c	1.15  com/lib/c/str,3.1,9013 3/15/90 17:15:18
 */
 
#include <sys/types.h>
#include <string.h>
#include <NLchar.h>

/*
 * FUNCTION: Compares the strings pointed to by s1 and s2, returning an
 *	     integer as follows:
 *
 *		Less than 0	If s1 is less than s2
 *		Equal to 0	If s1 is equal to s2
 *		Greater than 0	If s1 is greater than s2.
 *
 *	     The comparison is based on the collating sequence specified
 *	     by the locale category LC_COLLATE affected by the setlocale
 *	     function.
 *
 * NOTES:    The ANSI Programming Language C standard requires this routine.
 *
 * PARAMETERS: (Uses file codes )
 *	     char *s1 - first string
 *	     char *s2 - second string
 *
 * RETURN VALUE DESCRIPTIONS: Returns a negative, zero, or positive value
 *	     as described above.
 */
/*LINTLIBRARY*/

#define	CHNEXT(s)	    (NCisshift(s[0]) ? s+=2, _NCd2(s[-2], s[-1]) : *s++)
#define	EXTCOL(s, ch, cu)   ((cu = NCcoluniq(ch = CHNEXT(s))), NCcollate(ch))


/* The code consists of two loops: the first compares the two strings based
 * on primary weight, the second compares based on secondary weights (if there
 * were any).
 * In each loop we fetch the desired value for a char ( or collation element)
 * from each string, discarding "ignore" characters, and compare them.
 * We exit from the loops if either string terminates (i.e., the other is
 * longer or we hit an inequality.
 * If the first loop is exited with both strings equal and we found a char
 * with secondary weights (NCcollate value different from NCcoluniq value),
 * then we enter the second loop. In this we only compare NCcoluniq values
 * from the two strings if the char has secondary weight.
 * The NLxcolu routine is entered if the primary collation value is -1
 * (extended collation), and returns either a collation value  (n-to-1),
 * or a -1 for replacement string. In the last case, the 'p' parameter
 * now points to the first character of the replacement string.
 */

#ifdef _NO_PROTO
int strcoll(fs1, fs2)
char *fs1;
char *fs2;
#else
int	
strcoll(const char *fs1, const char *fs2)
#endif
{
	int co1, co2;			/* primary collation value */
	wchar_t cu1, cu2;			/* secondary collation value */
	int secflag = 0;

	wchar_t ch1, ch2;
	wchar_t *p1, *p2;		/* pointers for NLxcolu's use */
	wchar_t *x1, *x2;		/* pointers for NLxcolu's use */
	unsigned char *s1 = (void *)fs1;
	unsigned char *s2 = (void *)fs2;

	if (s1 == s2)
		return (0);

	p1 = p2 = 0;

	do {
		co1 = 0;
		while (co1 == 0)	/* loop until non-zero */
		{
					/* The if extracts co and cu and */
					/* evaluates to TRUE only if it is */
					/* 1-to-n mapping */
		    if (p1 != 0 || ((co1 = EXTCOL(s1, ch1, cu1)) < 0 &&
		       (co1 = _NLxcolu(co1, &s1, &p1, &cu1)) == -1)) {
					/* This if is for the case where the */
					/* replacement  char is part of an */
					/* extended collation (except */
					/* replacement, because we check that */
					/* in ctab) */
			    if ((co1 = NCcollate(*p1)) < 0) {
				 p1++;
				 co1 = _NCxcolu(co1, &p1, &x1, &cu1);
			    }else 
				 cu1 = NCcoluniq(*p1++);

			    if (*p1 == '\0')	/* at end of 1-to-n str */
				p1 = 0;
		    }
		}

		co2 = 0;
		while (co2 == 0)
		{
		    if (p2 != 0 || ((co2 = EXTCOL(s2, ch2, cu2)) < 0 &&
		       (co2 = _NLxcolu(co2, &s2, &p2, &cu2)) == -1)) {
			    if ((co2 = NCcollate(*p2)) < 0) {
				 p2++;
				 co2 = _NCxcolu(co2, &p2, &x2, &cu2);
			    }else 
				 cu2 = NCcoluniq(*p2++);
			    if (*p2 == '\0')
				p2 = 0;
		    }
		}
		if ((co1 != cu1) || (co2 != cu2))
			secflag = 1;		/* we found a sec ordering */


	} while (ch1 != '\0' && ch2 != '\0' && co1 == co2);

	if (((ch1 != '\0') || (ch2 != '\0')) || (secflag == 0))
		return (co1 - co2);
	if ( (ch1 == '\0') && (ch2 != '\0') )
			return (-1);
	if ( (ch2 == '\0') && (ch1 != '\0') )
			return (1);

 /* At this point, we have two strings that collate equal on their primary
  * collation values. We now go back to the beginning of the strings and
  * recompare them based on secondary collation values (if any)...
  */


	p1 = p2 = 0;
	s1 = (void *)fs1;
	s2 = (void *)fs2;
	ch1 = ch2 = 1;

	do {
		/* Force loop entry here */
		co1 = 0;
		while ((ch1 != 0) && ((co1 == 0) || (co1 == cu1))) 
		{
		    if (p1 != 0 || ((co1 = EXTCOL(s1, ch1, cu1)) < 0 &&
		       (co1 = _NLxcolu(co1, &s1, &p1, &cu1)) == -1)) {
			    if ((co1 = NCcollate(*p1)) < 0) {
				 p1++;
				 co1 = _NCxcolu(co1, &p1, &x1, &cu1);
			    }else
				    cu1 = NCcoluniq(*p1++);
			    if (*p1 == '\0')
				p1 = 0;
		    }
		}

		co2 = 0;
		while ((ch2 != 0) && ((co2 == 0) || (co2 == cu2)))
		{
		    if (p2 != 0 || ((co2 = EXTCOL(s2, ch2, cu2)) < 0 &&
		       (co2 = _NLxcolu(co2, &s2, &p2, &cu2)) == -1)) {
			    if ((co2 = NCcollate(*p2)) < 0) {
				  p2++;
		       		  co2 = _NCxcolu(co2, &p2, &x2, &cu2);
			    }else
				    cu2 = NCcoluniq(*p2++);

			    if (*p2 == '\0')
				p2 = 0;
		    }
		}

		
	} while (ch1 != '\0' && ch2 != '\0' && cu1 == cu2);

	return (cu1 - cu2);
}

/*
 * FUNCTION: Compares the wchar_t strings pointed to by s1 and s2, returning an
 *	     integer as follows:
 *
 *		Less than 0	If s1 is less than s2
 *		Equal to 0	If s1 is equal to s2
 *		Greater than 0	If s1 is greater than s2.
 *
 *	     The comparison is based on the collating sequence specified
 *	     by the locale category LC_COLLATE affected by the setlocale
 *	     function.
 *
 * NOTES:    The ANSI Programming Language C standard requires this routine.
 *
 * PARAMETERS: (Uses file codes )
 *	     wchar_t *s1 - first string
 *	     wchar_t *s2 - second string
 *
 * RETURN VALUE DESCRIPTIONS: Returns a negative, zero, or positive value
 *	     as described above.
 */
/*LINTLIBRARY*/

#define	EXTCOL1(s, ch, cu)   (cu = NCcoluniq(ch = *s), NCcollate(*s++))


/* The code consists of two loops: the first compares the two strings based
 * on primary weight, the second compares based on secondary weights (if there
 * were any).
 * In each loop we fetch the desired value for a char ( or collation element)
 * from each string, discarding "ignore" characters, and compare them.
 * We exit from the loops if either string terminates (i.e., the other is
 * longer or we hit an inequality.
 * If the first loop is exited with both strings equal and we found a char
 * with secondary weights (NCcollate value different from NCcoluniq value),
 * then we enter the second loop. In this we only compare NCcoluniq values
 * from the two strings if the char has secondary weight.
 * The NLxcolu routine is entered if the primary collation value is -1
 * (extended collation), and returns either a collation value  (n-to-1),
 * or a -1 for replacement string. In the last case, the 'p' parameter
 * now points to the first character of the replacement string.
 */

#ifdef _NO_PROTO
int wcscoll(s1, s2)
wchar_t *s1;
wchar_t *s2;
#else
int	
wcscoll(const wchar_t *s1, wchar_t *s2)
#endif
{
	int co1, co2;			/* primary collation value */
	wchar_t cu1, cu2;			/* secondary collation value */
	int secflag = 0;
	wchar_t *sav1, *sav2;
	wchar_t ch1, ch2;
	wchar_t *p1, *p2;		/* pointers for NLxcolu's use */
	wchar_t *x1, *x2;		/* pointers for NLxcolu's use */

	if (s1 == s2)
		return (0);

	p1 = p2 = 0;
	sav1 = (wchar_t *)s1;		/* save string pointers for possible */
	sav2 = s2;			/* second pass */

	do {
		co1 = 0;			/* so first pass will work */
		while (co1 == 0)	/* loop until non-zero */
		{
					/* The if extracts co and cu and */
					/* evaluates to TRUE only if it is */
					/* 1-to-n mapping */
		    if (p1 != 0 || ((co1 = EXTCOL1(s1, ch1, cu1)) < 0 &&
		       (co1 = _NCxcolu(co1, &s1, &p1, &cu1)) == -1)) {
			    if ((co1 = NCcollate(*p1)) < 0) {
				 p1++;
				 co1 = _NCxcolu(co1, &p1, &x1, &cu1);
			    } else 
				 cu1 = NCcoluniq(*p1++);

			    if (*p1 == '\0')	/* at end of 1-to-n str */
				p1 = 0;
		    }
		}

		co2 = 0;			/* so first pass will work */
		while (co2 == 0)
		{
		    if (p2 != 0 || ((co2 = EXTCOL1(s2, ch2, cu2)) < 0 &&
		       (co2 = _NCxcolu(co2, &s2, &p2, &cu2)) == -1)) {
			    if ((co2 = NCcollate(*p2)) < 0) {
				 p2++;
				 co2 = _NCxcolu(co2, &p2, &x2, &cu2);
			    } else 
			         cu2 = NCcoluniq(*p2++);

			    if (*p2 == '\0')
				p2 = 0;
		    }
		}
		if ((co1 != cu1) || (co2 != cu2))
			secflag = 1;		/* we found a sec ordering */


	} while (ch1 != '\0' && ch2 != '\0' && co1 == co2);

	if (((ch1 != '\0') || (ch2 != '\0')) || (secflag == 0))
		return (co1 - co2);
	if ( (ch1 == '\0') && (ch2 != '\0') )
			return (-1);
	if ( (ch2 == '\0') && (ch1 != '\0') )
			return (1);

 /* At this point, we have two strings that collate equal on their primary
  * collation values. We now go back to the beginning of the strings and
  * recompare them based on secondary collation values (if any)...
  */


	p1 = p2 = 0;
	s1 = sav1;
	s2 = sav2;
	ch1 = *s1;
	ch2 = *s2;

	do {
		co1 = 0;
		while ((ch1 != 0) && ((co1 == 0) || (co1 == cu1)))
		{
		    if (p1 != 0 || ((co1 = EXTCOL1(s1, ch1, cu1)) < 0 &&
		       (co1 = _NCxcolu(co1, &s1, &p1, &cu1)) == -1)) {
			    if ((co1 = NCcollate(*p1)) < 0) {
				 p1++;
				 co1 = _NCxcolu(co1, &p1, &x1, &cu1);
			    } else 
			    	cu1 = NCcoluniq(*p1++);
			    if (*p1 == '\0')
				p1 = 0;
		    }
		}

		co2 = 0;
		while ((ch2 != 0) && ((co2 == 0) || (co2 == cu2)))
		{
		    if (p2 != 0 || ((co2 = EXTCOL1(s2, ch2, cu2)) < 0 &&
		       (co2 = _NCxcolu(co2, &s2, &p2, &cu2)) == -1)) {
			    if ((co2 = NCcollate(*p2)) < 0) {
				 p2++;
				 co2 = _NCxcolu(co2, &p2, &x2, &cu2);
			    } else 
			    	cu2 = NCcoluniq(*p2++);
			    if (*p2 == '\0')
				p2 = 0;
		    }
		}

		
	} while (ch1 != '\0' && ch2 != '\0' && cu1 == cu2);

	return (cu1 - cu2);
}
