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
static char rcsid[] = "@(#)$RCSfile: doscan.c,v $ $Revision: 1.7 $ (OSF) $Date: 1994/12/06 19:49:38 $";
#endif

/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: _doscan, nf, NLnan_doscan, getcc, ungetcc 
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
 * "doscan.c	1.49  com/lib/c/io,3.1,9021 5/4/90 19:06:00";
 */

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef  _THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <stdlib.h>


#include <string.h>
#include <ctype.h>
#include <varargs.h>
#include <values.h>
#include <memory.h>
#include <NLctype.h>
#include <fp.h>
#include <nl_types.h>
#include <langinfo.h>

#define NCHARS	(1 << BITSPERBYTE)

static int incount;    	/* count of chars read from input stream */
static int inpeof;	/* EOF on input reached */
static int number();
static unsigned char *setup();
static int NLunescgetc();
static int string();

static wchar_t Lch;

static char Escseqflg;

#ifdef KJI
#define UNGETC(C,P)	ungetwc(C,P)
#ifdef  _THREAD_SAFE
#define GETC(iop)      (((iop->_flag&_IONOFD)&&iop->_cnt<=0)? EOF: unlocked_getwc(iop))
#else
#define GETC(iop)      (((iop->_flag&_IONOFD)&&iop->_cnt<=0)? EOF: getwc(iop))
#endif
#else
#define GETC(iop)       (((iop->_flag&_IONOFD)&&iop->_cnt<=0)? EOF: getcc(iop))
#define UNGETC(C,P)	ungetcc(C,P)
#endif /* KJI */

#define RFREE(s)	if(s) free((void *)s)
static int reorder();


/*                                                                    
 * FUNCTION: _doscan: common code for scanf, sscanf, fscanf
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	     - returns number of matches
 *	     - EOF on failure
 */

int
_doscan(iop, ofmt, ova_alist)
FILE *iop;
unsigned char *ofmt;
va_list ova_alist;
{
	unsigned char *fmt;	  /* (possibly new) format string */
	va_list			rva_alist; /* (possibly reordered) arg. list */
	unsigned char	       *nfmt;	  /* temp. format ptr. if reordering */
	va_list			nva_alist; /* temp. args ptr. if reordering */

	extern unsigned char *setup();
	char tab[NCHARS];
	int ch;

	/* nmatch is the number of matches found between a format
	 * specification and the input stream scanned.
	 */
	int nmatch = 0, len, inchar, stow, size;



	/* The "N" flag will indicate if the "N" qualifier for
	 * the string descriptors ("s" or "S") has been read.
	 * This qualifier indicates that the text string contains
	 * NLS ASCII escape sequences that are to be converted
	 * to NLS code points.
	 * The "B" qualifier indicates that we wish to count raw
	 * bytes rather than code points, the latter being done
	 * by default.  A "B" flag is supplied for this, too.
	 */
	
	char Nflag = 0, Bflag = 0;
	char wlflag = 0;

	unsigned char *bp;
	char	*pattern="%1$"; /* pattern indicating variable arg. ordering */

	/* The 'C' specification, new with KJI, indicates that a one or two
	 * byte character is to read into an NLchar value.
	 */

	incount = 0;
	inpeof = 0;


	rva_alist = ova_alist;
	nfmt = 0;
#ifdef	i860
	/* not ALL va_list's are char * !! */
	bzero( &nva_alist, sizeof(nva_alist) );
#else	/* i860 */
	nva_alist = 0;
#endif	/* i860 */
	fmt = ofmt;
	bp = fmt;
	len = strlen(pattern);
	while (*bp) {
		if (strncmp((const char *)pattern, (const char *)bp, 
							(size_t)len) == 0) {
			/*
			 *  Reorder argument list if variable ordering used.
			 */
			if (reorder (ofmt, ova_alist, &nfmt, &nva_alist) < 0)
				return (EOF);
			rva_alist = nva_alist;
			fmt = nfmt;
			break;
		}
		++bp;
	}


	/* Main Loop:  Read the format specifications for values to
	 * to be scanned one at a time.  For each specification read
	 * from the input stream the expected type of value.
	 */

	for( ; ; ) {
		if((ch = *fmt++) == '\0')
		{
#ifdef	i860
			RFREE(nva_alist.reg_base);
#else	/* i860 */
			RFREE(nva_alist);
#endif	/* i860 */
			RFREE(nfmt);
			return(nmatch); /* end of format */
		}

		if(isspace(ch)) {
#ifdef KJI
			while(inchar = GETC(iop), NCisspace(inchar))
				;
#else
			while(isspace(inchar = GETC(iop)))
				;
#endif

/* If the end of the input string is reached, note this by incrementing 
   inpeof. Loop around again because there may be futher format 
   specifiers  */
			if (UNGETC(inchar, iop) == EOF)
				inpeof++;
                        continue;
                 
		}

		if (!Bflag && !Nflag && !wlflag)
			if(ch != '%' || (ch = *fmt++) == '%') {
				inchar = GETC(iop) ;
				if(inchar == ch)
					continue;
				if(UNGETC(inchar, iop) != EOF)

				{
#ifdef	i860
					RFREE(nva_alist.reg_base);
#else	/* i860 */
					RFREE(nva_alist);
#endif	/* i860 */
					RFREE(nfmt);
					return(nmatch); /* failed to match input */
				}
				break;
			}

		/* When stow is 0, the input is read but not written. */
		if(ch == '*') {
			stow = 0;
			ch = *fmt++;
		} else
			stow = 1;

		/*  If Nflag is not already set, check to see if current
		 *  character is a N.  Set Nflag accordingly and continue
		 *  processing.
		 */

		if (!Nflag && (Nflag = (ch == 'N')))
			continue;

		/*  If Bflag is not already set, check to see if current
		 *  character is a N.  Set Bflag accordingly and continue
		 *  processing.
		 */

		if (!Bflag && (Bflag = (ch == 'B')))
			continue;

		/*  If wlflag is not already set, check to see if current
		 *  character is a l or w.  Set wlflag accordingly and continue
		 *  processing.
		 */

		if (!wlflag)
			if (ch == 'l')
				wlflag = 'l';
			else if (ch == 'w') {
				wlflag = 'w';
				continue;
			}

		for(len = 0; isdigit(ch); ch = *fmt++)
			len = len * 10 + ch - '0';
		if(len == 0)
			len = MAXINT;

		/* Check for syntax "%[*][w][B][N][<length>]<fmt_desc>"
		 */
		if (ch == 'N' || ch == 'B' || ch == 'w')
			break;

		/* long double == double in this implementation */
		if (ch == 'L')
			ch = 'l';
		if((size = ch) == 'l' || size == 'h')
			ch = *fmt++;

		if (ch == 'n') {
		        if (!stow) continue;

			switch(size) {
			case 'h':
				*va_arg(rva_alist, short *) = incount;
				break;
			case 'l':
				*va_arg(rva_alist, long *) = incount;
				break;
			default:
				*va_arg(rva_alist, int *) = incount;
			}
			continue;
		}

		if (inpeof)
			break; 

		/* A call to setup defines scansets. */
		if(ch == '\0' ||
		    ch == '[' && (fmt = setup(fmt, tab)) == NULL)

		{
#ifdef	i860
			RFREE(nva_alist.reg_base);
#else	/* i860 */
			RFREE(nva_alist);
#endif	/* i860 */
			RFREE(nfmt);
			return(EOF); /* unexpected end of format */
		}

		if ((ch != 'N') && (ch != 'B') && 
		    ! ((wlflag && (ch == 's' || ch == 'c' )) 
		      || ch == 'S' || ch == 'C') && isupper(ch)) {
			/* no longer documented */
			ch = _tolower(ch);
		}
		if(ch != 'c' && ch != 'C' && ch != '[') {
#ifdef KJI
			while(inchar = GETC(iop), NCisspace(inchar))
				;
#else
			while(isspace(inchar = GETC(iop)))
				;
#endif
			if(UNGETC(inchar, iop) == EOF)
				break;
		}


		/* if Bflag or Nflag are true, current character must be an 
		 * "s" or  "S".  If it is not, a format error is reached and
		 * we should stop processing.
		 */
		if ((Bflag || Nflag) && (ch != 's') && (ch != 'S'))
			break;

		/* if wlflag is true, current character must be set listed
		 * below.  If it is not, a format error is reached and
		 * we should stop processing.
		 */
		if (wlflag) {
			int	breaker = 0;

			switch(ch) {
			case 's':
			case 'c':
			case 'd':
			case 'i':
			case 'o':
			case 'u':
			case 'X':
			case 'x':
			case 'p':
			case 'E':
			case 'e':
			case 'f':
			case 'G':
			case 'g':
				break;
			default:
				breaker++;
			}
			if (breaker)
				break;
		}

		/* If a character string is specified, then call the
		 * string function; otherwise, call the number function.
		 * Note that "number" is not restricted to handling
		 * digit strings, it must also recognize special IEEE
		 * values (INF, NaNQ, NaNS).
		 */

		if((size = (ch == 'S' || ch == 'C' || ch == 'c' || ch == 's' || ch == '[') ?
		    string(wlflag,Nflag,Bflag,stow,ch,len,tab,iop, &rva_alist) :
		    number(wlflag, stow, ch, len, size, iop, &rva_alist)) != 0)
			nmatch += stow;

		/* 'N' and 'B' qualifiers should affect only the current
		 * descriptor, so we turn them off here.
		 */

		Bflag = Nflag = 0;
		wlflag = 0;

		if(size == 0)
			/* failed to match input */

		{
#ifdef	i860
			RFREE(nva_alist.reg_base);
#else	/* i860 */
			RFREE(nva_alist);
#endif	/* i860 */
			RFREE(nfmt);
			return(nmatch != 0 ? nmatch : inpeof ? EOF : 0);
		}
	}

#ifdef	i860
	RFREE(nva_alist.reg_base);
#else	/* i860 */
	RFREE(nva_alist);
#endif	/* i860 */
	RFREE(nfmt);

	return(nmatch != 0 ? nmatch : EOF); /* end of input */
}


/* Functions to read the input stream in an attempt to match incoming
 * data to the current specification from the main loop of _doscan().
 */

static int
number(wlflag, stow, type, len, size, iop, listp)
char wlflag;
int stow, type, len, size;
/* The arguments are defined as follows:
 *    stow - if set the value being scanned is written to listp
 *    type - the scan conversion character
 *    len - the length of the string to be scanned (MAXINT default)
 *    size - equal to type unless a long (l or h) conversion
 *    iop - pointer to input being scanned
 *    listp - list of pointers where values should be stored
 */

FILE *iop;
va_list *listp;
{
	char numbuf[256];
	char *np = numbuf;
	int c, base;
	int digitseen = 0, dotseen = 0, expseen = 0, floater = 0, negflg = 0;
	int zero = 0;
	long lcval = 0;
	int t;
	int nanret = 0;
	int pointer = 0;
	int radix = 0;
	char *radixv = nl_langinfo(RADIXCHAR);

	/* If there is no radix character defined for this locale, use '.' */
	if (radixv[0] == '\0')
		radix = '.';
	else
		radix = radixv[0];
	switch(type) {
	case 'E':
	case 'e':
	case 'f':
	case 'G':
	case 'g':
		floater++;
	case 'd':
	case 'u':
		base = 10;
		break;
	case 'o':
		base = 8;
		break;
	case 'p':
		pointer = 1;
		/*
			fall through
		*/
	case 'X':
	case 'x':
		base = 16;
		break;
	case 'i':
		base = -1;
		break;
	default:
		return(0); /* unrecognized conversion character */
	}
#ifdef KJI
	switch(c = GETC(iop), c = isascii(c) ? c : _jistoa(c))
#else
	switch(c = GETC(iop))
#endif
	{
	case '-':
		negflg++;
	case '+': /* fall-through */
		len--;

		c = GETC(iop);
#ifdef KJI
		c = isascii(c) ? c : _jistoa(c);
#endif
	}
 	/* set base if %i format specified */
 	while (base == -1)
 		switch(c) {
 		case '0':
			/***** XPG3 Bogus: This means '0012' isn't octal!
 			**if (zero) {
 			**	base = 10;
 			**	break;
 			**}
			**/
 			zero = c;
 			len--;

 			c = GETC(iop);
#ifdef KJI
			c = isascii(c) ? c : _jistoa(c);
#endif
 			break;
 		case 'p':
			pointer = 1;
			/*
				fall through
			*/
 		case 'x':
 		case 'X':
 			base = 16;
 			len--;

 			c = GETC(iop);
#ifdef KJI
			c = isascii(c) ? c : _jistoa(c);
#endif
 			break;
 		default:
 			if (zero) {
			  	UNGETC(c, iop);
 				len++;
 				c = zero;
 				base = 8;
 			}
 			else
 				base = 10;
			break;
 		}

	/* First determine which case - either a number or one of the
	 * three special IEEE numbers which begin with either a I, Q
	 * or a S (P34273)
	 */
	if (wlflag && c == 's')
	    t='S';
	else
	    t=c;

	switch(t) {
	default:
		/* Store the number in numbuf as it's read in.  A blank will
		 * cause a break in the loop.
		 */
		for( ; --len >= 0; *np++ = c, c = GETC(iop))
		{
			if (c == EOF) {
				break;
			}
#ifdef KJI
			c = isascii(c) ? c : _jistoa(c);
#endif
			if(!digitseen && base == 16 && c=='0') {
			    /*
			     * Look for 0x999 or 0X999 patterns
			     */
			    int	rc;

			    rc = GETC(iop);

			    if( rc=='x' || rc=='X' ) {
				c = GETC(iop);

				if(!isxdigit(c)) { 	/* Something wrong! */
				    UNGETC(c, iop);	/* Push back and treat as */
				    UNGETC(rc, iop); 	/* a zero followed by char */
				    c = '0';
				} else {
				    /*
				     * Found 0x followed by valid digits.  Just fall
				     * thru to next test for isdigit.....
				     */
				}
			    } else			/* Not 'x' */
			      UNGETC(rc, iop);
			}			/* Finish looking for 0x999 */


			if(isdigit(c) || base == 16 && isxdigit(c)) {
				int digit = c - (isdigit(c) ? '0' :
				    isupper(c) ? 'A' - 10 : 'a' - 10);
				if(digit >= base)
					break;
				if(stow && !floater)
					lcval = base * lcval + digit;
				digitseen++;
				continue;
			}
			if(!floater)
				break;
			if(c == radix && !dotseen++)
				continue;
			if((c == 'e' || c == 'E') && digitseen && !expseen++) {
				*np++ = c;

				c = GETC(iop);
				--len;
#ifdef KJI
				c = isascii(c) ? c : _jistoa(c);
#endif
				if(isdigit(c) || c == '+' || c == '-')
					continue;
				else {	/*** Bogus Exp,  back up ***/
					if (UNGETC(c,iop) == EOF)
						inpeof++;
					c = *--np;
					*np = '\0';
				}
			}
			break;
		}
		if(stow && digitseen)
			if(floater) {
				*np = '\0';
				if (size == 'l') {
					double dval;

					dval = atof(numbuf);
					if(negflg)
						dval = -dval;
					*va_arg(*listp, double *) = dval;
				}
				else {
					float dval;

					dval = atof(numbuf);
					if(negflg)
						dval = -dval;
					*va_arg(*listp, float *) = dval;
				}
			} else {
				/* 
				 suppress possible overflow on 2's-comp 
				 negation
				 */
				if(negflg && lcval != HIBITL)
					lcval = -lcval;
				if (!pointer)
					if(size == 'l')
						*va_arg(*listp, long *) = lcval;
					else if(size == 'h')
						*va_arg(*listp, short *) =
						(short)lcval;
					else
						*va_arg(*listp, int *) =
						(int)lcval;
				else
				{
					pointer = 0;
					*va_arg(*listp, void **) =
					(void *) lcval;
				}
			}
		if(UNGETC(c, iop) == EOF)
			inpeof++; /* end of input */
		break;
	/* Check for special IEEE values INF, NaNQ, and NaNS.
	 * When any of these strings are found the next character
	 * is read to be consistent with the for loop that reads
	 * the numbers, digitseen is also incremented because these
	 * are considered legitimate matches.
	 */
	case 'i':
	case 'I':
	  if (floater)
	    {
	      if (nf(iop)) {
		     /* An IEEE infinite has been scanned. */
		     digitseen++;
		     if (stow) {
			/* Explicitly set the INF bit string for double or
			 * float types.
			 */
			if (size == 'l') {
			    double ival;
			    if (negflg) {
			       DBL(ival, 0xfff00000, 0x00000000);
			    }
			    else
			       DBL(ival, 0x7ff00000, 0x00000000);
			    *va_arg(*listp, double *) = ival;
			}
			else {
			    unsigned long fival;
			    if (negflg)
			       fival = 0xff800000;
			    else
			       fival = 0x7f800000;
			    *va_arg(*listp, unsigned long *) = fival;
			}
		     }
	      }
	    }
	    else
		if(UNGETC(c, iop) == EOF)
			inpeof++; /* end of input */
		break;
	case 'n':
	case 'N':
	  if (floater)
	  {

	   nanret = NLnan_doscan(iop);
	   if (nanret == 1) {
	      digitseen++;

	      if (stow) {

	      /* A NaNQ has been scanned and needs to be stored.  Negative
	       * signs must be given in the actual assignments to take
	       * effect.  Printf of float negative NaNQs yields a NaNQ.
	       */

		 if (size == 'l') {
		     double qval;
		     if (negflg) {
			DBL(qval, 0xfff80000, 0x00000000);
		     }
		     else
			DBL(qval, 0x7ff80000, 0x00000000);
		     *va_arg(*listp, double *) = qval;
		 }
		 else {
		     unsigned long fqval;
		     if (negflg)
			fqval = 0xffc00000;
		     else
			fqval = 0x7fc00000;
		     *va_arg(*listp, unsigned long *) = fqval;
		 }
	      }
	   }
	   else if (nanret == 2) {
	      digitseen++;

	      if (stow) {

	      /* A NaNS has been scanned and needs to be stored.
	       */


		 if (size == 'l') {
		     double sval;
		     if (negflg) {
			DBL(sval, 0xfff55555, 0x55555555);
		     }
		     else
			DBL(sval, 0x7ff55555, 0x55555555);
		     *va_arg(*listp, double *) = sval;
		 }
		 else {
		     unsigned long fsval;
		     if (negflg)
			fsval = 0xff855555;
		     else
			fsval = 0x7f855555;
		     *va_arg(*listp, unsigned long *) = fsval;
		 }
	      }
	   }
	  }
	  else
		if(UNGETC(c, iop) == EOF)
			inpeof++; /* end of input */
		break;
	}

	return(digitseen); /* successful match if non-zero */
}

static int
string(wlflag, Nflag, Bflag, stow, type, len, tab, iop, listp)
char wlflag, Nflag, Bflag;
int stow, type, len;
char *tab;
FILE *iop;
va_list *listp;
{
	char *ptr;
	NLchar Nch;
	char *start;
	char cpsize, cpbuf[2];
	char i;
	int lowlim = 0;
	char Shiftread = 0;
	NLchar *NLstart;
	NLchar *NLptr;
	int val;

	if((wlflag=='l') && ((type=='c') || (type == 's')))
		wlflag='\0';

	if (type == 'S' || type == 'C' ||(wlflag &&(type == 's' || type == 'c')))
		NLstart = NLptr = stow ? va_arg(*listp, NLchar *) : NULL;
	else
		start = ptr = stow ? va_arg(*listp, char *) : NULL;
	if((type == 'c' || type == 'C') && len == MAXINT)
		len = 1;

	if (! ((wlflag && (type == 's' || type == 'c' ))
	      || type == 'S' || type == 'C'))
	    while((val= NLunescgetc(Nflag, iop),Nch=val,val != EOF) && 
	        !(type == 's' && NCisspace(Nch) || type == '[' && tab[Nch])) {

		    cpsize = NCencode(&Nch, cpbuf);

		    /* Bflag on:  bytes have to be counted. */
		    if (Bflag)
			    lowlim += cpsize - 1;

		    /* Bflag off:  make sure we don't count shift
		     * characters read in one at a time from the
		     * input stream.
		     */
		    if (!Bflag && cpsize == 1 && NCisshift(cpbuf[0]))
			    lowlim--;

		    if (stow)
			    for (i=0; i<cpsize; i++)
				    *ptr++ = cpbuf[i];
		    else
			    ptr += cpsize;
    
	    	    if(--len <= lowlim)
	    		    break;
	    }
	else
	    while((val = NLunescgetc(Nflag, iop),Nch=val, val != EOF) 
		   && (!NCisspace(Nch) || 
		   (((wlflag && type == 'c') || type == 'C') &&
		   NCisspace(Nch)))) {
		    /* When 'S' is used, the 'B' flag really doesn't carry
		     * any meaning, since 'B' refers to the STORAGE length,
		     * not the input length.  We thus do no checks for it.
		     */

#ifdef KJI	
		    /* NLunescgetc now returns an already decoded NLchar, which
		     * can be placed directly into the NLchar array.
		     */
#else
		    /* We can place an NLS character read from an ASCII
		     * escape sequence directly into the NLchar array.
		     * When there was no escape sequence, though, we check
		     * for two byte NLS characters and decode them if necessary.
		     * Note that a shift byte followed by a non-NLS byte
		     * is ignored.
		     */
		    if (!Escseqflg) 
			    if (NCisshift(Nch)) {
				    Shiftread++;
				    cpbuf[0] = Nch;
				    continue;
			    }
			    else if (Shiftread && NCisNLchar(Nch)) {
				    cpbuf[1] = Nch;
				    NCdecode(cpbuf, &Nch);
			    }
		    Shiftread = 0;
#endif
		    if (stow)
			    *NLptr = Nch;
		    NLptr++;
    
		    if(--len <= 0)
			    break;
	    }

	if(val == EOF || len > lowlim && UNGETC(Lch, iop) == EOF)
		inpeof++; /* end of input */

	if (! ((wlflag && (type == 's' || type == 'c' ))
	      || type == 'S' || type == 'C')) {
		if(ptr == start)
			return(0); /* no match */
		if(stow && type != 'c') /* note wc excluded above */
			*ptr = '\0';
	}
	else {
		if(NLptr == NLstart)
			return(0); /* no match */
		if(stow && ! (type == 'C' || (wlflag && type == 'c')))
			*NLptr = '\0';
	}
	return(1); /* successful match */
}

static unsigned char *
setup(fmt, tab)
unsigned char *fmt;
char *tab;
{
	int b, c, d, t = 0;

	if(*fmt == '^') {
		t++;
		fmt++;
	}
	(void)memset((void *)tab, !t, (size_t)NCHARS);
	if((c = *fmt) == ']' || c == '-') { /* first char is special */
		tab[c] = t;
		fmt++;
	}
	while((c = *fmt++) != ']') {
		if(c == '\0')
			return(NULL); /* unexpected end of format */

		/* This is where the scanset range is specified,
		 * b represents the first and d the last character
		 * included in the scanset.
		 */
		if(c == '-' && (d = *fmt) != ']' && (b = fmt[-2]) < d) {
			(void)memset((void *)&tab[b], t, (size_t)(d - b + 1));
			fmt++;
		} else
			tab[c] = t;
	}
	return(fmt);
}

/* Check for nf */
int nf(iop)
FILE *iop;
{
        char c[2];

	/* Backup if !nf
         */

	if (((c[0] = GETC(iop)) == 'n') || (c[0] == 'N'))
	{	if (((c[1] = GETC(iop)) == 'f') || (c[1] == 'F'))
		{	
			return 1;
		}
		UNGETC(c[1], iop);
	}
	UNGETC(c[0], iop);

	return (0);
}

/* Check for aNQ or aNS */
int NLnan_doscan(iop)
FILE *iop;
{
	char c[3];

	/* Backup if ! aNQ or aNS
	 */

	if (((c[0] = GETC(iop)) == 'a') || (c[0] == 'A'))
	{	if (((c[1] = GETC(iop)) == 'n') || (c[1] == 'N'))
		{	if (((c[2] = GETC(iop)) == 'q') || (c[2] == 'Q')) {
				/* NanQ */
				return 1;
			}
			/* anything else is treated as a NaNS */
			return 2;
		}
		UNGETC(c[1], iop);
	}
	UNGETC(c[0], iop);

	return(0);
}

/* We assume that at no point in the NLS world, an ASCII escape sequence
#ifdef KJI
   could contain more than 20 bytes.  Current max = 7.
#else
   could contain more than 20 bytes.  Current max = 6.
#endif
*/

#define _ESCMAXBUF	20

static int
NLunescgetc(Nflag, iop)
char Nflag;
FILE *iop;
{
#ifdef KJI
	NLchar seq[_ESCMAXBUF+1];
#else
	char seq[_ESCMAXBUF+1];
#endif
	short index = -1;
	int   val;
	NLchar result;

	/* Initialize escape sequence flag */
	Escseqflg = 0;

	/* Do the escape processing only when the Nflag is on. */
	if (!Nflag) {
		Lch = val = GETC(iop);
		return (val);
	}
	/* Check if first char is a backslash */

	if ((Lch = seq[++index] = val = GETC(iop)) == '\\') {

		/* If first character is a backslash, now check if
		 * next is a <.  
		 */
		if ((Lch = seq[++index] = val = GETC(iop)) == '<') {

			/* Read from input stream until reaching
			 * closing >, EOF, or maximum buffer size.
			 * If EOF is reached, the user has misconstructed
			 * an escape sequence.  No attempt is made to 
			 * recover the bytes in the stream since the first 
			 * "\<."
			 */
#ifdef KJI
		        /* getcc ok here since escape sequence should be ASCII */
#endif
			while((++index < _ESCMAXBUF && 
			      (Lch = seq[index] = val = GETC(iop)) != '>'))
				if (val == EOF)
					return(EOF);

			/* Convert valid sequence. */

			seq[++index] = '\0';
#ifdef KJI
			NCwunesc(seq, &result);	/* takes NLchars */
#else
			NCunesc(seq, &result);
#endif
			Escseqflg++;
			return (result);
		}
		/* If character following backslash was not a <, put
		 * it back in the stream and return the backslash.
		 */
		else {
			if (val != EOF)
				UNGETC(seq[1], iop);

			return (Lch = '\\');
		}
	}
	else
		/* If first character is not a backslash, just return
		 * it.
		 */
		return(val);
}


/*
 *	Reorder goes through a format statement containing variably
 *      ordered arguments and reorders the arguments accordingly,
 *      stripping out the x$'s.  Upon success, reorder() returns 0;
 *	the new format string is returned via the argument "bp_new_fmt",
 *	and the new arg pointer is returned via the argument "bp_new_args".
 *	Upon failure, reorder() returns -1; *bp_new_fmt = *bp_new_args = 0.
 */

/* Convert a digit character to the corresponding number */
#define tonumber(x) ((x)-'0')

static
reorder (format, args, bp_new_fmt, bp_new_args)
char   *format;
va_list args;
char		**bp_new_fmt;	/* new format */
va_list		*bp_new_args;	/* new argument list */
{
	/* Order array keeps track of arg variable order ..
            e.g. arg 2 comes before arg 1 */
	int	*order;

	/* New_args is the new args va_list returned to caller */
	int    **new_args;
	int    **temp_hold;

	/* Arg_index counts number of args (i.e. % occurrences) */
	int     arg_index;

	/* Last_alloc is the last alloced index for the size&order arrays */
	int     last_alloc;

	/* size of pointer ... usually four */
	int	ptr_size;	    

	/* Tot_size counts the actual size of all arguments in bytes */
	int     tot_size;

	/* arg_count is the total number of arguments in format string */
	int	arg_count;

	/* new_fmt is the working pointer in the new format string*/
	/* bp is the beginning pointer to original format string  */
	char   *new_fmt, *bp;

	/* Format code */
	int	fcode;

	/* Return code */
	int	rcode;

	/* Flags - nonzero if corresponding character is in format */
	int     length;         /* l */

	/* work variable(s) */
	int 	i;
	bp = format;
	tot_size = arg_count = 0;
#ifdef	i860
	/* not ALL va_list's are char * !! */
	bzero( bp_new_args, sizeof(va_list) );
	*bp_new_fmt = 0;
#else	/* i860 */
	*bp_new_fmt = *bp_new_args = 0;
#endif	/* i860 */
	rcode = 0;

	/*
	 *      The format loop interogates all conversion characters to
	 *      determine the order and size of each argument, 
	 *      calculating the tot_size, and filling in new_fmt.
	 */

	*bp_new_fmt = new_fmt = (char *)malloc(strlen(format));
	last_alloc = 10;
	order = (int *)malloc((size_t)(10 * sizeof(int)));
	if (new_fmt == 0 || order == 0) {
		rcode = -1;
		goto ret;
	}

	for ( ; ; ) {
		while ((*new_fmt = *format) != '\0' && *new_fmt != '%' ) {
			new_fmt++;
			format++;
		}

		fcode = *format;
		if (fcode == '\0') { /* end of format; normal return */
			break;    /* Now do the get_args loop */
			}
		/*
		 *	% has been found.
		 *	First extract digit$ from format and compute arg_index.
		 *	Next parse the format specification.
		 *	Skip, if * is encountered.
		 */

		if ((fcode = *++format) == '*') {
			++new_fmt;
			continue; 
		} else
			if(fcode == '%')
				{ /* Assign it to newformat and go get next char*/
				*++new_fmt=fcode;
				new_fmt++;
				format++;
				continue;
				}
			else
				--format;

		for (arg_index = 0; isdigit(fcode = *++format); )
			arg_index = arg_index * 10 + tonumber(fcode);
		if (arg_index > last_alloc) {
			last_alloc += 10;
			order= (int *)realloc((void *)order, (size_t)(last_alloc * sizeof(int)));
			if (order == 0) {
				rcode = -1;
				goto ret;
			}
		}
		arg_index--;    /* the arrays are zero based */
		order[arg_count] = arg_index;

		for ( ; ; ) { /* Scan the <flags> */
			switch (fcode = *++new_fmt = *++format) {
			case ' ':
			case 'B':
			case 'N':
				continue;
			}
			break;
		}

		/* Scan the field width */
		while (isdigit (fcode = *format)) 
			*new_fmt++ = *format++;

		/* Scan the length modifier */
		length = 0;
		switch (*format) {
		case 'l':
			length++;
			/* No break */
		case 'h':
			*new_fmt++ = *format++;
		}

		switch (fcode = *new_fmt++ = *format++) {

		case 'd':
		case 'u':
		case 'o':
		case 'X':
		case 'x':
		case 'E':
		case 'p':
		case 'e':
		case 'F':
		case 'f':
		case 'G':
		case 'g':
		case 'c':
		case 's':
		case 'S':
			break;

		case '[': {
		        /* a string of characters follows ending with a ']' */
			int loop_cnt;
			while (*format != '\0' && *format != ']') {
				loop_cnt = NLchrlen (format);
				for (i=0; i < loop_cnt; i++)
					*new_fmt++ = *format++;
			}
			fcode = *new_fmt = *format;
			if (*format == ']') {
				new_fmt++;
				format++;
			}
			break;
		}

		/* case '%':   */
		default:
			break;

		case '\0': { /* unexpected end of format; return error */
			rcode = -1;
			goto ret;
			}

		}
		arg_count++;
	}

	/* Reorder ARGS Loop:  allocate char *new_args;
                               loop through order array to index into
			       size and args and copy XX size bytes
			       from *args[i] to new_args */
	{
	    int     arg_no;         /* argument order used to index in size */
				    /* and pargs                            */

	    ptr_size = sizeof (int *);
	    tot_size = ptr_size * arg_count;
#ifdef	i860

         /* Get extra room to store fregs addresses first
            (8 of them at 4 bytes each */ 

            new_args = (int**)malloc((size_t)tot_size + 8*ptr_size);

            bp_new_args->mem_ptr =  (long *)new_args;
            bp_new_args->reg_base = (long *)new_args;
            bp_new_args->ireg_used = 0;
            bp_new_args->freg_used = 0;

#else	/* i860 */
	    *bp_new_args = new_args = malloc((size_t)tot_size);
#endif	/* i860 */

	    if (new_args == 0) {
		    rcode = -1;
		    goto ret;
	    }

#ifdef	i860
            if (arg_count > 12)
              { 
                 /* If all regs don't fit in registers, va_arg 
                    uses mem_ptr. Room exists for 20 addresses.   */

                 bp_new_args->mem_ptr =  bp_new_args->mem_ptr + 20; 
              }
                 
            if (  (temp_hold = (int**)malloc((size_t)tot_size)) != NULL)
              {

               for (arg_no = 0; arg_no < arg_count; arg_no++)
                 {
                    temp_hold[arg_no] = (int *) va_arg(args,int *); 
                 }
               for (arg_no = 0; arg_no < arg_count; arg_no++)
                 {

                  /* space for fregs first, so offset by 8 32 bit regs */

                     new_args[arg_no + 8] = temp_hold[order[arg_no]];
                 }
               free ((void *)temp_hold);
              }
            else 
              {
               rcode = -1;
              } 

#else	/* i860 */
	    for (arg_no = 0; arg_no < arg_count; arg_no++) {
		    memcpy ((void *)new_args, (void *)(args+(order[arg_no]*                                 ptr_size)), (size_t)ptr_size);
		    new_args += ptr_size;
	    }
#endif	/* i860 */

ret:
	    if (order != 0)
		    free ((void *)order);
	    if (*bp_new_fmt != 0)
		    free ((void *)*bp_new_fmt);
	    return(rcode);
	}
}


/* Routines to keep track of chars read from input stream  */
getcc(iop)
FILE	*iop;
{
	int c;
	
#ifdef  _THREAD_SAFE
        if ((c = unlocked_getc(iop)) != EOF) incount++;
#else
        if ((c = getc(iop)) != EOF) incount++;
#endif
	return(c);
}

ungetcc(c, iop)
int	c;
FILE	*iop;
{
	if (c != EOF) incount--;
	return(ungetc(c, iop));
}




