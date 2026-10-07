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
static char rcsid[] = "@(#)$RCSfile: mbs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:01 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: mbsncat, mbsncpy, mbsrchr, mbstoint, mbschr
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * mbs.c	1.7  com/lib/c/nls,3.1,9013 12/7/89 22:18:09
 */
/*LINTLIBRARY*/

#include	<sys/types.h>

#define INTSIZE	sizeof(int)
#define MBMAXLEN	2
static  size_t mlentoblen();	/* converting multibyte length to byte length */
static int mbcmp();
static void itombc();

/*
 * NAME: mbsncat
 *
 * FUNCTION: Append a specific number of multibyte characters (code points) 
 *  from one multibyte character string to another multibyte character string.
 *
 * PARAMETERS:
 *	char *s1	-	the multibyte character string
 *	char *s2	-	the multibyte character string
 *	size_t    n	-	the number of multibyte characters
 *
 * RETURN VALUE DESCRIPTION: 
 *	char *s1	-	the multibyte character string
 */

char *mbsncat(char *s1, char *s2, size_t n)
{
    size_t slen;	/* number of bytes in s2 */
    char *os1;		/* pointer to s1 */

    if (!s2)
        return(s1);

    if (!s1)
        return(NULL);

    if (!(slen = mlentoblen (s2, n)))	/* error occurs in the s2, stop concatenation */
	return(s1);

	os1 = s1;
	while(*s1++)
		;
	--s1;
	while(*s1++ = *s2++)
		if(!(slen--)) {
			*--s1 = '\0';
			break;
		}
	return(os1);
}

/*
 * NAME: mlentoblen
 *
 * FUNCTION: converting the number of multibyte characters to the number
 *  of bytes.
 *
 * PARAMETERS:
 *	char  *s	-	the multibyte character string
 *	size_t    n	-	the number of multibyte characters
 *
 * RETURN VALUE DESCRIPTION: 
 *	the number of bytes in a multibyte character string
 */

static size_t mlentoblen(s,n)
char *s;
size_t n;
{
    size_t l=0;	/* the number of bytes in a multibyte character */
    size_t cnt=0;	/* the number of bytes */

    while(n-- && *s)
    {
        l=mblen(s,MBMAXLEN);
        if(l == -1)
	    return(0);		/* return 0 if error occurs		*/
 
	s += l;
	cnt += l;
    }
    return(cnt);
}

/*
 * NAME: mbslen
 *
 * FUNCTION: counting the number of multibyte characters in a multibyte 
 *  character string.
 *
 * PARAMETERS:
 *	char  *s	-	the multibyte character string
 *
 * RETURN VALUE DESCRIPTION: 
 *	the number of multibyte characters in a multibyte character string
 */

size_t mbslen(const char *s0)
{
    char *s = (char *)s0;
    size_t slen=strlen(s);	/* the number of bytes in the s */
    size_t tlen=0;	/* the number of multibyte characters in the s */
    size_t cnt;	/* the number of a multibyte character */

    while(slen > 0)
    {
        if((cnt=mblen(s,MBMAXLEN)) != -1)
        {
	    s += cnt;
	    slen -= cnt;
	    tlen++;
        }
	else
	    return(0);		/* return 0 if error occurs		*/
    }
    return(tlen);
}

/*
 * NAME: mbsncpy
 *
 * FUNCTION: Copy a specific number of characters (code points) from one 
 *  multibyte character string to another multibyte character string.
 *
 * PARAMETERS:
 *	char  *s1	-	the multibyte character string
 *	char  *s2	-	the multibyte character string
 *	int    n	-	the number of multibyte characters
 *
 * RETURN VALUE DESCRIPTION: 
 *	pointer		to the multibyte character string s1.
 */

char *mbsncpy(char *s1, char *s2, size_t n)
{
    register size_t slen;	/* the number of bytes in the s */
    register char *os1;		/* pointer to a multibyte character string */
    register size_t s2len=mbslen(s2);	/* the number of multibyte characters in the s2 */

    if (!s2)
    {
        *s1='\0';
        return(s1);
    }

    if (!s1)
        return(NULL);

    if (!(slen = mlentoblen (s2, n)))	/* error occurs in the s2, stop copy */
	return(NULL);

	os1 = s1;

	while(*s1++ = *s2++)
		if(!(slen--)) {
			s1--;
			s2len = ( n < s2len ) ? s2len - n : 1;
			while(s2len-- > 0)
			    *s1++ = '\0';
			break;
		}
    return(os1);
}

/*
 * NAME: mbsrchr
 *
 * FUNCTION: Locate a character (multibyte character) in a multibyte character string.
 *  This function locates the last occurrence of mb in the string pointed to
 *  by mbs.  The terminating null character is considered to be part of the
 *  string.
 *
 * PARAMETERS:
 *	char  *mbs	-	the multibyte character string
 *	int    mbc	-	the multibyte character (code point)
 *
 * RETURN VALUE DESCRIPTION: 
 *	pointer		to mb within the multibyte character string.
 *	null pointer	if mb does not occur in the string.
 */

char *mbsrchr(char *mbs, int mbc)
{
    register size_t mbslen=strlen(mbs);	/* the number of bytes in the s */
    register size_t mbclen;	/* number of bytes of a multibyte char*/
    register char *ombs=mbs;		/* pointer to a multibyte character */
    register char *pmbs=mbs+mbslen;	/* pointer to end of a multibyte char */
    register char *match=(char *)0;	/* pointer to a multibyte character */
    register size_t ombclen=0;	/* the number of bytes in a multibyte char */
    void itombc();	/* function converting integer to multibyte */
    char s2[INTSIZE + 1];		/* multibyte character */

    if ( mbclen == -1 || !mbs || !*mbs)
        return(NULL);

    if (!mbc)
        return(pmbs);

    itombc(mbc,s2);
    mbclen=mblen(s2,MBMAXLEN);		/* number of bytes of a multibyte char*/

    for (; ombs < pmbs && ombclen != -1; ombs+=ombclen)
    {
        ombclen = mblen(ombs,MBMAXLEN);
	if (ombclen == mbclen && mbcmp(ombs, s2, mbclen))
	    match=ombs;
    }

    return(match);
}

/*
 * NAME: mbcmp
 *
 * FUNCTION:  compare multibyte characters (code points) in one string to 
 * 	another string.
 *
 * PARAMETERS:
 *	char  *s1	-	the multibyte character string
 *	char  *s2	-	the multibyte character string
 *	int    n	-	the number of bytes
 *
 * RETURN VALUE DESCRIPTION:
 * 	an integer greater than zero if s1 is greater than s2; zero if 
 *	the strings are equivalent; and an integer less than zero if s1 
 *	is less than s2.
 */

static int mbcmp(s1,s2,n)
register char *s1;
register char *s2;
register int n;
{
register int i;		/* the counter for a for loop */

    for (i = 0 ; i < n; i++)
	if (s1[i] != s2[i])
	{
    	    break;
	}
    return(i == n);
}

/*
 * NAME: mbstoint
 *
 * FUNCTION: Extract a multibyte character from a multibyte character string.
 *
 * PARAMETERS:
 *	char  *mbs	-	the multibyte character
 *
 * RETURN VALUE DESCRIPTION: 
 *    0  if an invalid multibyte character is encountered
 *    n  code point of the multibyte character
 */

int mbstoint(char *mbs)
{
    register size_t mbslen=mblen(mbs,MBMAXLEN); /* the number of bytes */
    register size_t mbint=0; /* the multibyte character (code point) */
    if ( mbslen == -1 || mbslen == 0)
	return(0);

    for (; mbslen>0; mbslen--)
	mbint = ((unsigned)mbint<<8) + *(unsigned char *)mbs++;

    return(mbint);
}
/*
 * NAME: mbschr
 *
 * FUNCTION: Locate a character (code point) in a multibyte string.
 *		(The terminating NULL character is consiered to be part
 *		  of the string)
 *
 * PARAMETERS: 
 *		char    *s1    - the multibyte char string
 *		int	mbc    - the code point of a multibyte char
 *
 * RETURN VALUE DESCRIPTIONS:
 *	NULL	if the mbc does not occur in the string.
 *	ptr	points to the mbc within the multibyte character string
 *
*/

char	*mbschr(const char *s1, const int mbc)
{
    char s2[INTSIZE + 1];		/* multibyte character */
    register char *p;	/* pointer to a multibyte character string */
    register char *q;	/* pointer to a multibyte character string */
    register char *r;	/* pointer to a multibyte character string */
    void itombc();	/* function converting integer to multibyte */
    register size_t qlen=1;	/* the number of bytes in a multibyte char */

    if( !mbc )
        return(NULL);
    itombc(mbc,s2);

    for(q=(char *)s1; *q ; q+=qlen) 
    {
        for(r=q, p=s2; *r && *p ; r++, p++)
        {
            if( *p != *r )
	        break;
        }
        if( !(*p) )
            break;
        qlen=mblen(q,MBMAXLEN);
        if(qlen == -1)
	    return(NULL);
    }
    if (*q)
        return(q);
    return(NULL);
}

/*
 * NAME: itombc
 *
 * FUNCTION: convert a multibyte char (code point) into a multibyte character.
 *
 * PARAMETERS: 
 *		int	   n    - the code point of a multibyte char
 *		char    *mbc    - the multibyte char
 *
 * RETURN VALUE DESCRIPTIONS: none
 *
*/

static void itombc(n,mbc)
int n;
char *mbc;
{
    register int cnt=0;		/* counter */
    register int i=0;		/* counter */
    register char *mtmp=(char *)&n;	/* pointer to a character string */

    for(;!mtmp[i] && i<INTSIZE;i++);

    for(; i < INTSIZE; i++)
        if(mtmp[i] || cnt)
            mbc[cnt++] = mtmp[i];
    mbc[cnt] = '\0';
}
