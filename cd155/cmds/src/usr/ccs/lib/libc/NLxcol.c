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
static char rcsid[] = "@(#)$RCSfile: NLxcol.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:12 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: NLxcol, NCxcol, NLxcolu, NCxcolu
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * NLxcol.c	1.12  com/lib/c/nls,3.1,9013 2/27/90 21:36:10
 */


#include <sys/types.h>
#include <NLctype.h>

/*
 *
 *  Perform extended collation, according to the lc_coldesc element
 *  indexed by "desc". Passed parameters are: "desc" (index to 
 *  coldesc element), str (pointer to input +1), rstr (pointer to
 *  replacement).  
 *  Elements in lc_coldesc contain four fields: 
 *	cd_stroff	the offset from the start of the lc_strings array 
 *			of a collating string,
 *	cd_repoff	the offset from the start of the lc_strings array
 *			of a replacement string
 *	cd_cval		a collate value
 *	cd_cuniq	a coluniq value
 *
 *  The fields are interpreted as follows:
 *      cd_stroff  cd_repoff  cd_cval   cd_cuniq
 *      ---------  ---------  -------   --------
 *       "from"      "to"       0        uniq      n-to-n mapping
 *       "from"        0       coll      uniq      multi-char coll symbol
 *         0           0       coll      uniq      default for char
 *     
 *  IF the "cd_stroff" field is non-zero and the cd_cval is zero,
 *  then the entry defines a many-to-many replacement mapping.
 *  If the input matches the "cd_stroff" string, then it is  
 *  replaced by the replacement string. The input pointer is modified
 *  to point past string to be replaced, the replacement pointer is
 *  set to the replacement string, and -1 is returned.
 *  
 *  If the "repoff" field in zero, and "stroff" is non-zero, then 
 *  the entry defines a multi-character collating element (e.g. Spanish
 *  'ch'); input pointer is set past element, and collation value
 *  is returned.
 *  
 *  If "cd_stroff" and "cd_repoff" are zero, then the entry defines the
 *  collating (and coluniq) value for the character itself; and collation
 *  value is returned.
 *
 * Change Activity:
 *   P24692 05/08/87 Return -1 if 1-to-n collation is used.
 */

#define ONE_TO_N -1

/*
 * NAME: _NLxcol
 *
 * FUNCTION: Find  collating value or replacement string for character
 *           beginning at str.                                  
 *
 * RETURN VALUE DESCRIPTION: -1 if 1-to-n collation is used. Otherwise,
 *                           return collating value. 
 */
/*  Find collating value or replacement string for character beginning
 *  at str.  If no replacement string exists, return collating value and
 *  set rstr to NULL.  Else return -1 and set rstr to point to
 *  replacement string.
 */
#ifdef _NO_PROTO
int _NLxcol(descptr, str, rstr)
int descptr;
unsigned char **str;
wchar_t **rstr;
#else
int
_NLxcol(int descptr, unsigned char **str, wchar_t **rstr)
/* descptr - coldesc rel. ptr */
/* **str - input string */
/* **rstr - replacement string */
#endif
{
	register unsigned char *is;
	register wchar_t *ds;
	register coldesc_t *desc;
	int rval;
	wchar_t ic;
	register coldesc_t *coldescp = (_locp->lc_coltbl)->lc_coldesc;
	register wchar_t *stringsp = (_locp->lc_coltbl)->lc_strings;

	if (rstr != 0)
		*rstr = 0;
	rval = descptr;
	if (descptr < 0) {
		descptr = -descptr - 1;
		for (desc = &coldescp[descptr]; ; ++desc) {
			rval = desc->cd_cval;
			if (desc->cd_stroff == 0)	/* default for */
				break;			/* char */
			ds = stringsp + desc->cd_stroff;
			ds++;
			is = *str;
			do {
				if (*ds == '\0') {
					*str = is;
					if (desc->cd_cval == 0) {	
					    if (rstr != 0)
						*rstr = stringsp+desc->cd_repoff;
						rval = ONE_TO_N;   /* p24692 */
					}
					return (rval);
				}
				is += NCdec(is, &ic);
			} while (ic == *ds++);
		}
	}
	return (rval);
}

/*
 * NAME: _NCxcol
 *
 * FUNCTION: Find  collating value or replacement character for character
 *           beginning at str.                                  
 *
 * NOTE: Like _NLxcol, but interprets an input string of type wchar_t
 *       instead of char.
 * 
 * RETURN VALUE DESCRIPTION: -1 if 1-to-n collation is used. Otherwise,
 *                           return collating value. 
 */
#ifdef _NO_PROTO
int _NCxcol(descptr, str, rstr)
int descptr;
wchar_t **str;
wchar_t **rstr;
#else
int
_NCxcol(int descptr, wchar_t **str, wchar_t **rstr)
#endif
{
	register wchar_t *ds, *is;
	register coldesc_t *desc;
	int rval;
	register coldesc_t *coldescp = (_locp->lc_coltbl)->lc_coldesc;
	register wchar_t *stringsp = (_locp->lc_coltbl)->lc_strings;

	if (rstr != 0)
		*rstr = 0;
	rval = descptr;
	if (descptr < 0) {
		descptr = -descptr - 1;
		for (desc = &coldescp[descptr]; ; ++desc) {
			rval = desc->cd_cval;
			if (desc->cd_stroff == 0)
				break;
			ds = stringsp + desc->cd_stroff;
			ds++;
			is = *str;
			do {
				if (*ds == '\0') {
					*str = is;
					if (desc->cd_cval == 0) {	
					    if (rstr != 0)
						*rstr = stringsp+desc->cd_repoff;
						rval = ONE_TO_N;    /* P24692 */
					}
					return (rval);
				}
			} while (*is++ == *ds++);
		}
	}
	return (rval);
}

/*
 * NAME: _NLxcolu
 *
 * FUNCTION: Find  collation value and unique collation value for character 
 * beginning at str.
 *
 * RETURN VALUE DESCRIPTION: -1 if 1-to-n collation is used. Otherwise,
 *                           return colluniq value. 
 */
/*  Find collating value and unique collation value or replacement string 
 *  for character beginning at str.
 *  If no replacement string exists, return collation value and
 *  set rstr to NULL.  Else return -1 and set rstr to point to
 *  replacement string.
 */
#ifdef _NO_PROTO
int _NLxcolu(descptr, str, rstr, uniq)
int descptr;
unsigned char **str;
wchar_t **rstr;
wchar_t *uniq;
#else
int
_NLxcolu(int descptr, unsigned char **str, wchar_t **rstr, wchar_t *uniq)
/* descptr - coldesc rel. ptr */
/* **str - input string */
/* **rstr - replacement string */
/* *uniq - unique collation value */
#endif
{
	register unsigned char *is;
	register wchar_t *ds;
	register coldesc_t *desc;
	int rval;
	wchar_t ic;
	register coldesc_t *coldescp = (_locp->lc_coltbl)->lc_coldesc;
	register wchar_t *stringsp = (_locp->lc_coltbl)->lc_strings;

	if (rstr != 0)
		*rstr = 0;
	*uniq = 0;
	rval = descptr;
	if (descptr < 0) {
		descptr = -descptr - 1;
		for (desc = &coldescp[descptr]; ; ++desc) {
			rval = desc->cd_cval;
			*uniq = desc->cd_cuniq;
			if (desc->cd_stroff == 0)	/* default for */
				break;			/* char */
			ds = stringsp + desc->cd_stroff;
			ds++;
			is = *str;
			do {
				if (*ds == '\0') {
					*str = is;
					if (desc->cd_cval == 0) {	
					    if (rstr != 0)
						*rstr = stringsp+desc->cd_repoff;
						rval = ONE_TO_N;   /* p24692 */
						*uniq = desc->cd_cuniq;
					}
					return (rval);
				}
				is += NCdec(is, &ic);
			} while (ic == *ds++);
		}
	}
	return (rval);

}

/*
 * NAME: _NCxcolu
 *
 * FUNCTION: Find collating value and unique collation value, or replacement 
 * character for character beginning at str.                                  
 *
 * NOTE: Like _NLxcolu, but interprets an input string of type wchar_t
 *       instead of char.
 * 
 * RETURN VALUE DESCRIPTION: -1 if 1-to-n collation is used. Otherwise,
 *                           return collation value. 
 */
#ifdef _NO_PROTO
int _NCxcolu(descptr, str, rstr, uniq)
int descptr;
wchar_t **str;
wchar_t **rstr;
wchar_t *uniq;
#else
int
_NCxcolu(int descptr, wchar_t **str, wchar_t **rstr, wchar_t *uniq)
#endif
{
/* descptr - coldesc rel. ptr */
/* **str - input string */
/* **rstr - replacement string */
/* *uniq - unique collation value */
	register wchar_t *ds, *is;
	register coldesc_t *desc;
	int rval;
	register coldesc_t *coldescp = (_locp->lc_coltbl)->lc_coldesc;
	register wchar_t *stringsp = (_locp->lc_coltbl)->lc_strings;

	if (rstr != 0)
		*rstr = 0;
        *uniq = 0;
	rval = descptr;
	if (descptr < 0) {
		descptr = -descptr - 1;
		for (desc = &coldescp[descptr]; ; ++desc) {
			rval = desc->cd_cval;
			*uniq = desc->cd_cuniq;
			if (desc->cd_stroff == 0)
				break;
			ds = stringsp + desc->cd_stroff;
			ds++;
			is = *str;
			do {
				if (*ds == '\0') {
					*str = is;
					if (desc->cd_cval == 0) {	
					    if (rstr != 0)
						*rstr = stringsp+desc->cd_repoff;
						rval = ONE_TO_N;    /* P24692 */
						*uniq = desc->cd_cuniq;
					}
					return (rval);
				}
			} while (*is++ == *ds++);
		}
	}
	return (rval);
}
