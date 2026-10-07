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
 * OSF/1 Release 1.0.3
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: strxfrm.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:08:46 $";
#endif
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: strxfrm
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
 * strxfrm.c	1.13  com/lib/c/str,3.1,9013 3/15/90 17:18:19
 */

#include <sys/types.h>
#include <string.h>
#include <NLchar.h>

/*
 * FUNCTION: Transforms the string pointed to by s2 and places it in the
 *	     string pointed to by s1 according to the locale category
 *	     LC_COLLATE affected by the setlocale function.  This string
 *	     is of a collation weight such that a strcmp of two strings
 *	     produced by strxfrm is equivalent to a strcoll of the two
 *	     original strings.  No more than n characters are transformed.
 *
 * NOTES:    The ANSI Programming Language C standard requires this routine.
 *
 * PARAMETERS:  (file code)
 *	     char *s1 - transformed string
 *	     char *s2 - string to be transformed
 *	     size_t n - number of characters to transform
 *
 * RETURN VALUE DESCRIPTIONS: Returns the count of the characters
 *	     transformed, not including the terminating null character.
 */
/*LINTLIBRARY*/

#define	CHNEXT(s)	   (NCisshift(s[0]) ? s+=2, _NCd2(s[-2], s[-1]) : *s++)
#define	EXTCOL(s, ch, cu)  ((cu = NCcoluniq(ch = CHNEXT(s))), NCcollate(ch))

/*
 * This routine accepts a string to be transformed, and creates a string
 * such that "if the strcmp function is applied to two transformed strings,
 * it returns a value greater than, equal to, or less than zero, corresponding
 * to the result of the strcoll function applied to the same two original
 * strings." What we create is a string containing primary collation weights
 * for the characters in the string, followed, if needed, by a spearator and the
 * secondary collation weights for characters with secondary ordering.
 * This string is terminated by a NULL character.
 * Secondary ordering is indicated by a coluniq value different from a
 * collate value.
 * A zero collating value means "ignore this char for collation purposes". All
 * collating (and coluniq) values are guaranteed to have both bytes non-zero
 * so we store values as the high-order byte of the value, followed by
 * the low-order byte.
 */

#ifdef _NO_PROTO
size_t strxfrm(s1, s2, n)
char *s1;
char *s2;
size_t n;
#else
size_t 	
strxfrm(char *s1, const char *s2, size_t n)
#endif
{
    size_t i;
    int len;

    len = strlen(s2);

    /**********
      if s1 is null or n is 0, just return the length
    **********/
    if (s1==NULL || n==0)
        return len;

    /**********
      copy n-1 bytes from s2 to s1 and null terminate s1
    **********/
    n--;
    strncpy(s1, s2, n);
    s1[n] = '\0';

    return len;

}
