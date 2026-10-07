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
/* static char rcsid[] = "@(#)$RCSfile: doprnt.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 02:04:18 $"; */
static char rcsid[] = "@(#)doprnt.c    1.555";
#endif

/*
 * COMPONENT_NAME: (LIBCPRNT) Standard C Library Print Functions 
 *
 * FUNCTIONS: _doprnt 
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
 * static char sccsid[] = "@(#)doprnt.c	1.55  com/lib/c/prnt,3.1,9021 5/11/90 12:32:41";
 */

#include <stdio.h>
#ifdef  _THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <NLctype.h>
#include <NLchar.h>
#include <langinfo.h>					/* p30097 */
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <values.h>
#include <string.h>
#include <fp.h>
#include "print.h"	/* parameters & macros for doprnt */

/*
 * Definitions of argument handling types and macros.
 * This should be in print.h, and make doscan.c include <print.h> as well.
 */
#define RFREE(s)	if (s) free((void *)s)
#define UNSET	(-1)
#define get_arg(va,da,type) (reorderflag == FALSE ? \
			va_arg(va,type) : (((type *)&((da)++->da_arg))[0]))

/*
 * argument list in canonical form
 */
typedef struct _arglist arglist;

typedef union _arg {
	arglist	*au_arg;
	int	au_int;
	long	au_long;
	unsigned au_unsigned;
	void	*au_pointer;
	double	au_double;
} arg;

struct _arglist {
	arg	da_arg;
	int	da_type;
};

#define	DA_T_UNDEF	0
#define	DA_T_INT	1
#define	DA_T_LONG	2
#define	DA_T_UNSIGNED	3
#define	DA_T_POINTER	4
#define	DA_T_DOUBLE	5

static int reorder();

extern char *malloc();
								/* p30098+ */
/* all putc's in doprnt now call the macro 'PUTC', so that, we can break */
/* from doprnt if it interrupted. */
#ifdef _THREAD_SAFE
#define PUTC(A1,A2)	if ( unlocked_putc(A1,A2) < 0 )  { \
				RFREE(nargs); \
				RFREE(nformat); \
				return(-1); }
#else
#define PUTC(A1,A2)	if ( putc(A1,A2) < 0 )  { \
				RFREE(nargs); \
				RFREE(nformat); \
				return(-1); }
#endif /* _THREAD_SAFE */

#ifdef _THREAD_SAFE
#define FWRITE(A1,A2,A3,A4) {if( unlocked_fwrite(A1,A2,A3,A4) == 0 ) { \
			RFREE(nargs); \
			RFREE(nformat); \
			return(-1); } }
#else
#define FWRITE(A1,A2,A3,A4) {if( fwrite(A1,A2,A3,A4) == 0 ) { \
			RFREE(nargs); \
			RFREE(nformat); \
			return(-1); } }
#endif /* _THREAD_SAFE */

#define PUT(p, n)	{if (n == 1) { PUTC((int)*p, iop); } \
			else if (iop->_flag & _IONOFD) \
			iop->_ptr = (unsigned char *) \
			 memcpy((void *)iop->_ptr, (void *)p, (size_t)n) +n; \
			else FWRITE((void *)p, (size_t)1, (size_t)n, iop);}
								/* p30098. */
#ifdef KJI
/* Put out SJIS double wide blank and zero if J-flag used. */
/* Note hard-coded values used. */
#define PUTBLANK	if (jflag) {PUTC(0x81, iop); \
			     PUTC(0x40, iop);} \
			     else PUTC(' ', iop)
#define PUTZERO		if (jflag) {PUTC(0x82, iop); \
			     PUTC(0x4f, iop);} \
			     else PUTC('0', iop)
#endif

/*
 *	C-Library routines for floating conversion
 */
#ifdef  _THREAD_SAFE
extern char *fcvt_r(), *ecvt_r();
#else
extern char *fcvt(), *ecvt();
#endif

/*                                                                    
 * FUNCTION: _doprnt: common code for printf, fprintf, sprintf
 *	modifications to support ieee NaNs and +-infinity +-zero
 *	no longer calls KillNaN
 *
 *	Originaly derived from merge of NLdoprnt 8.9 and doprnt 1.11 then
 *	changes from 1.12 com/lib/c/prnt,3.1,8910" added
 */

int
_doprnt(format, args, iop)
register char	*format;
va_list	args;
register FILE	*iop;
{
	/* This variable counts output characters. */
	int	count = 0;
	char		*nformat;	/* temp. format pointer if reordering */
	arglist		*nargs;		/* list of reordered arguments */
	int	reorderflag = UNSET;	/* reordering flag */
        int	ndigits;		/* number of signif float digits */

	/* character pointers used in 'S' format */
	NLchar	*Nbp, *Np, Nc;

	/* Starting and ending points for value to be printed */
	register char	*bp, *p;

	/* character pointer to the radix character  p30097 */
	register char *radchr;

	/* Field width and precision */
	int	width, prec;

	/* Format code */
	/* Changing this declaration from register to volatile is just
	   a workaround until the compiler bug is found and fixed.
	 */
	register int	fcode;

	/* int indicating leading zeros format */
	int	leadz;

	/* Number of padding zeroes required on the left and right */
	int	lzero, rzero;

	/* Flags - nonzero if corresponding character is in format */
	int	length;		/* l */
	int	fplus;		/* + */
	int	fminus;		/* - */
	int	fblank;		/* blank */
	int	fsharp;		/* # */
	int	bflag;		/* e.g. %B.1s */
	int	nflag;		/* e.g. %Ns */
	int	hflag;		/* p29789 */
#ifdef KJI
	int	jflag;		/* e.g. %Jd */
#endif

	char	sjbuf[2];	/* to hold bytes of SJIS character */
	int     wlflag;         /* e.g. %ws or %wc or %ls or %lc */

	/* Values are developed in this buffer */
	char	buf[max(MAXDIGS, 1+max(MAXFCVT+MAXEXP, MAXECVT))];

	/* Pointer to sign, "0x", "0X", or empty */
	char	*prefix;

	/* Exponent or empty or blank */
	char	*suffix;

	/* Buffer to create exponent */
	char	expbuf[MAXESIZ + 1];

	/* The value being converted, if integer */
	long	val;

	/* The value being converted, if real */
	double	dval;

#ifdef  _THREAD_SAFE
	/* cvt buffer plus extra space */
	char    cvtbuf[NDIG];
#endif

	/* Output values from fcvt and ecvt */
	int	decpt, sign;

	/* Pointer to a translate table for digits of whatever radix */
	char	*tab;

	/* Work variables */
	int	k, n, hradix, lowbit;
	int	i, nb;

	expbuf[MAXESIZ] = '\0';

	nformat = NULL;
	nargs = NULL;

	/*
	 *	The main loop -- this loop goes through one iteration
	 *	for each string of ordinary characters or format specification.
	 */
	for ( ; ; ) {
		bp = format;
		while ((fcode = *format) != '\0' && fcode != '%' ) {
			format += NLchrlen(format);
			count++;
		}
		if (n = format - bp) /* ordinary non-% characters */
			PUT(bp, n);

		if (fcode == '\0') /* end of format; normal return */
		{
			RFREE(nargs);
			RFREE(nformat);
			return (ferror(iop) ? EOF : count);
		}
		/*
		 *	% has been found.
		 *	First, parse the format specification.
		 */
		fplus = fminus = fblank = fsharp = leadz = 0;
		bflag = nflag = hflag = 0;	/* p29789 */
#ifdef KJI
		jflag = 0;
#endif
		wlflag = 0;

		if (reorderflag == UNSET && format[1] != '%') {
			if (isdigit(format[1])) {
				char *p = format + 1;
				while (*p && isdigit(*p))
					p++;
				if (*p == '$') {
					reorderflag = TRUE;
					if (reorder(wlflag,format,args,
							&nformat,&nargs))
						return(EOF);
					format = nformat;
				}
			}
			if (reorderflag == UNSET) {
				reorderflag = FALSE;
			}
		}

		for ( ; ; ) { /* Scan the <flags> */
			switch (fcode = *++format) {
			case '+':
				fplus++;
				continue;
			case '-':
				fminus++;
				continue;
			case ' ':
				fblank++;
				continue;
			case '#':
				fsharp++;
				continue;
			case 'B':
				bflag++;
				continue;
			case 'N':
				nflag++;
				continue;
#ifdef KJI
			case 'J':
				/* use double-wide chars. in numeric conversions */
				jflag++;
				continue;
#endif /* KJI */
                                /* input wide chars */
			case 'w':
                                wlflag++;
                                continue;
			case 'h':			/* p29789 */
				hflag++;		/* p29789 */
				continue;		/* p29789 */
                        }
			break;
		}

		/* Scan the field width */
		if (fcode == '*') {
			width = get_arg(args, nargs, int);
			if (width < 0) {
				width = -width;
				fminus++;
			}
			format++;
		} else {
			if (fcode == '0') {
				/* ignore if '-' also specified */
				if (!fminus)
					leadz = 1;
				fcode = *++format;
			}
			if (fcode == '*') {
				width = get_arg(args, nargs, int);
				if (width < 0) {
					width = -width;
					fminus++;
				}
				format++;
			}
			else for (width = 0; isdigit(fcode = *format); format++)
				width = width * 10 + tonumber(fcode);
		}

		/* Scan the precision */
		if (*format != '.')
			prec = -1;
		else if (*++format == '*') { /* '*' instead of digits? */
			prec = get_arg(args, nargs, int);
			format++;
		} else
			for (prec = 0; isdigit(fcode = *format); format++)
				prec = prec * 10 + tonumber(fcode);

		/* Scan the length modifier */
		length = 0;
		switch (*format) {
		case 'w':
                        wlflag++;
                        format++;
                        break;
                case 'l':
			length++;
			/* No break */
		case 'L': /* long double == double in this implementation */
		case 'h':
			format++;
		}

		/*
		 *	The character addressed by format must be
		 *	the format letter -- there is nothing
		 *	left for it to be.
		 *
		 *	The status of the +, -, #, and blank
		 *	flags are reflected in the variables
		 *	"fplus", "fminus", "fsharp", and
		 *	"fblank".  "width" and "prec" contain
		 *	numbers corresponding to the digit
		 *	strings before and after the decimal
		 *	point, respectively. If there was no
		 *	decimal point, "prec" is -1.
		 *
		 *	The NLS flag B is reflected in the variable
		 *	"bflag".
		 *
		 *	The following switch sets things up
		 *	for printing.  What ultimately gets
		 *	printed will be padding blanks, a
		 *	prefix, left padding zeroes, a value,
		 *	right padding zeroes, a suffix, and
		 *	more padding blanks.  Padding blanks
		 *	will not appear simultaneously on both
		 *	the left and the right.	 Each case in
		 *	this switch will compute the value, and
		 *	leave in several variables the informa-
		 *	tion necessary to construct what is to
		 *	be printed.
		 *
		 *	The prefix is a sign, a blank, "0x",
		 *	"0X", or null, and is addressed by
		 *	"prefix".
		 *
		 *	The suffix is either null or an
		 *	exponent, and is addressed by "suffix".
		 *
		 *	The value to be printed starts at "bp"
		 *	and continues up to and not including
		 *	"p".
		 *
		 *	"lzero" and "rzero" will contain the
		 *	number of padding zeroes required on
		 *	the left and right, respectively.  If
		 *	either of these variables is negative,
		 *	it will be treated as if it were zero.
		 *
		 *	The number of padding blanks, and
		 *	whether they go on the left or the
		 *	right, will be computed on exit from
		 *	the switch.
		 */

		prefix = "";
		suffix = &expbuf[MAXESIZ];
		lzero = rzero = 0;

		switch (fcode = *format++) {

		/*
		 *	fixed point representations
		 *
		 *	"hradix" is half the radix for the
		 *	conversion. Conversion is unsigned
		 *	unless fcode is 'd'. HIBITL is 100...000
		 *	binary, and is equal to the maximum
		 *	negative number.
		 *	We assume a 2's complement machine
		 */
		case 'n':
			if (!hflag)				   /* p29789 */
				*get_arg(args, nargs, long *) =	   /* p29789 */
						count;		   /* p29789 */
			else					   /* p29789 */
				*get_arg(args, nargs, short *) =   /* p29789 */
						count;		   /* p29789 */
			continue;

		case 'i':
		case 'd':
		case 'u':
			hradix = 10/2;
			goto fixed;

		case 'o':
			hradix = 8/2;
			goto fixed;

		case 'X':
		case 'x':
		case 'p':
			hradix = 16/2;

		fixed:
			/* Establish default precision */
			if (prec < 0)
			  prec = 1;
			else
			  /*
			   * There was a specified precision.  ANSI C says
			   * ignore any leading zeros
			   */
			  leadz = 0;

			/* Fetch the argument to be printed */

			if(length)
				val = get_arg(args, nargs, long);
			else if(fcode == 'd' || fcode == 'i')
				val = get_arg(args, nargs, int);
			else
				val = get_arg(args, nargs, unsigned);

			/* If signed conversion, make sign */
			if ((fcode == 'd') || (fcode == 'i')) {
				if(val < 0) {
					prefix = "-";
					/*
					 * Negate, checking in
					 * advance for possible
					 * overflow.
					 */
					if(val != HIBITL)
						val = -val;
				} else if (fplus)
					prefix = "+";
				else if (fblank)
					prefix = " ";
			}

			/* Set translate table for digits */
			tab = (fcode == 'X') ?
			    "0123456789ABCDEF" : "0123456789abcdef";

			/* Develop the digits of the value.  Special-case
			   0 value with 0 precision to print 0 characters. */
			p = bp = buf + MAXDIGS;
			if (val || prec) do {
				lowbit = val & 1;
				val = (val >> 1) & ~HIBITL;
				*--bp = tab[val % hradix * 2 + lowbit];
				val /= hradix;
			} while (val != 0);

			/* Handle the # flag for hex specifier */
			if (fsharp && bp != p)
				switch (fcode) {
				case 'x':
					prefix = "0x";
					break;
				case 'X':
					prefix = "0X";
					break;
				}

			/* Calculate padding zero requirement */ /* p40768+ */
			if (leadz)
			  lzero = bp - p + max(prec, (width -
			  (prefix[0] == '\0' ? 0 : prefix[1] == '\0' ? 1 : 2)));
			else if ((lzero = bp - p + prec) <= 0)  /* precision */
				lzero = 0;

			/* Handle the # flag for the octal specifier */
			if (fsharp && (fcode == 'o') && (lzero <= 0) )
				lzero = 1;
								/* p40768. */

			break;

		case 'E':
		case 'e':
			/*
			 * E-format.  The general strategy
			 * here is fairly easy: we take
			 * what ecvt gives us and re-format it.
			 */

			/* Establish default precision */
			if (prec < 0)
				prec = 6;

			/* Fetch the value */
			dval = get_arg(args, nargs, double);

			/* Develop the mantissa */
#ifdef _THREAD_SAFE
			ecvt_r(dval, min(prec + 1, MAXECVT), &decpt, &sign,
								cvtbuf, NDIG);
			bp = cvtbuf;
#else
			bp = ecvt(dval, min(prec + 1, MAXECVT), &decpt, &sign);
#endif

			/* Determine the prefix */
		e_merge:
			if (sign)
				prefix = "-";
			else if (fplus)
				prefix = "+";
			else if (fblank)
				prefix = " ";

			/* Place the first digit in the buffer*/
			p = &buf[0];

			if ( !FINITE(dval) ) {
				/* INF, NAN() */
				while (*p = *bp++){
					p++;
				}
				bp = &buf[0];
				break;
			}

			k = 0;					/* p29848 */
			*p++ = (*bp != '\0') ? (k++,*bp++) : (k++,'0');	
								/* p29848 */

			/* Put in a decimal point if needed */
			if (prec != 0 || fsharp) {
				radchr = nl_langinfo(RADIXCHAR); /* p30097 */
				if(radchr[0] != '\0')		/* p30097 */
					*p++ = radchr[0];	/* p30097 */
				else				/* p30097 */
					*p++ = '.';
			}

			/* Create the rest of the mantissa */
			for (rzero = prec; rzero > 0 && *bp != '\0'; --rzero) {
				*p++ = *bp++;
				k++;				/* p29848 */
			}
			bp = &buf[0];

			/* Create the exponent */
			if (dval != 0) {
				n = decpt - 1;
				if (n < 0)
				    n = -n;
				for ( ; n != 0; n /= 10) {
					*--suffix = todigit(n % 10);
					k++; 			/* p29848 */
				}
			}

			/* Prepend leading zeroes to the exponent */
			while (suffix > &expbuf[MAXESIZ - 2]) {
				*--suffix = '0';
				k++; 				/* p29848 */
			}

			/* Put in the exponent sign */
			*--suffix = (decpt > 0 || dval == 0) ? (k++,
					'+') : (k++,'-'); 	/* p29848 */

			/* Put in the e */
			*--suffix = isupper(fcode) ? (k++,'E')  : (k++,'e');
								/* p29848 */

			if ( leadz == 1 )			/* p29848 */
				if (fsharp || prec > 0)		/* p29848 */
					lzero = width - (k+1);	/* p29848 */
				else				/* p29848 */
					lzero = width - k;	/* p29848 */

			/* embed prefix within the field width */ /* p40768+ */
			lzero -= (prefix[0] == '\0' ? 0 : 1);	  /* p40768. */

			break;

		case 'f':
			/*
			 * F-format floating point.  This is a
			 * good deal less simple than E-format.
			 * The overall strategy will be to call
			 * fcvt, reformat its result into buf,
			 * and calculate how many trailing
			 * zeroes will be required.  There will
			 * never be any leading zeroes needed.
			 *
			 * 8/24/88 apar 1890:
			 * leading zeros are a valid option for
			 * floating point output.
			 */

			/* Establish default precision */
			if (prec < 0)
				prec = 6;

			/* Fetch the value */
			dval = get_arg(args, nargs, double);

			/* Do the conversion */
#ifdef _THREAD_SAFE
			fcvt_r(dval, min(prec, MAXFCVT), &decpt, &sign,
								cvtbuf, NDIG);
			bp = cvtbuf;
#else
			bp = fcvt(dval, min(prec, MAXFCVT), &decpt, &sign);
#endif

			/* Determine the prefix */
		f_merge:
			if (sign)    		/* p40510 */
				prefix = "-";
			else if (fplus)
				prefix = "+";
			else if (fblank)
				prefix = " ";

			/* Initialize buffer pointer */
			p = &buf[0];

			if ( !FINITE(dval) ) {
				/* INF, NAN() */
				if (sign)
					prefix = "-";
				else if (fplus)
					prefix = "+";
				else if (fblank)
					prefix = " ";
				while (*p = *bp++){
					p++;
				}
				bp = &buf[0];
				break;
			}

			/* Emit the digits before the decimal point */
			n = decpt;
			k = 0;
			ndigits = 0;
			do {
				*p++ = (n <= 0 || *bp == '\0' || ndigits >= MAXFSIG) ?
				    (k++, '0') : (ndigits++, k++, *bp++);
			} while (--n > 0);

			/* Decide whether we need a decimal point */
			if (fsharp || prec > 0) {
				radchr = nl_langinfo(RADIXCHAR); /* p30097 */
				if(radchr[0] != '\0')		/* p30097 */
					*p++ = radchr[0];	/* p30097 */
				else				/* p30097 */
					*p++ = '.';
			}

			/* Digits (if any) after the decimal point */
			n = min(prec, MAXFCVT);
			rzero = prec - n;
			while (--n >= 0)
				*p++ = (++decpt <= 0 || *bp == '\0' ||
				    ndigits >= MAXFSIG) ? (k++,'0') : (ndigits++, k++, *bp++);

			/* determine if leading zeros are requested and if
			   so, how many. reference apar 1890 */
			if ( leadz == 1 )
				if (fsharp || prec > 0)
					lzero = width - (k+1);
				else
					lzero = width - k;

			/* embed prefix within the field width */ /* p40768+ */
			lzero -= (prefix[0] == '\0' ? 0 : 1);	  /* p40768. */


			bp = &buf[0];

			break;

		case 'G':
		case 'g':
			/*
			 * g-format.  We play around a bit
			 * and then jump into e or f, as needed.
			 */

			/* Establish default precision */
			if (prec < 0)
				prec = 6;

			/* If prec is 0, 1 is assumed - ANSI */
			if (prec == 0)
				prec = 1;

			/* Fetch the value */
			dval = get_arg(args, nargs, double);

			/* Do the conversion */
#ifdef _THREAD_SAFE
			ecvt_r(dval, min(prec, MAXECVT), &decpt, &sign,
								cvtbuf, NDIG);
			bp = cvtbuf;
#else
			bp = ecvt(dval, min(prec, MAXECVT), &decpt, &sign);
#endif

			if ( !FINITE(dval) ) {
				/* INF, NAN() */
				goto e_merge;
			}

			if (dval == 0)
				decpt = 1;

			k = prec;
			if (!fsharp) {
				n = strlen(bp);
				if (n < k)
					k = n;
				while (k >= 1 && bp[k-1] == '0')
					--k;
			}

			if (decpt < -3 || decpt > prec) {
				prec = k - 1;
				goto e_merge;
			}
			prec = k - decpt;
			goto f_merge;

	/*	case '%':	*/
		default:
			buf[0] = fcode;
			goto c_merge;

		case 'c':
			if (! wlflag) { /* note: if wlflag, treate c as C */
                                       /* by falling through to next case */
				buf[0] = get_arg(args, nargs, int);
		c_merge:
				p = (bp = &buf[0]) + 1;
				break;
			} /* ! wlflag */
#ifdef KJI

				/* Specification to print out a
				 * single NLchar value.
				 */
		case 'C':
			Nc = get_arg(args, nargs, int);
			buf[0] = fcode;
			bp = &buf[0];
			n =  1;
			nb = NCenc (&Nc, buf);
			if (bflag)
			    width -= nb - 1;
			p = buf + nb;
			break;
#else /* KJI */
		case 'C':		/* p27147, handle 1 NLchar	*/
			*((unsigned short *)buf) = get_arg(args, nargs, int);
			p = (bp = &buf[0]) + 1; /* p27632		*/
			break;		/* cast NLchar into a short	*/
#endif /* KJI */

		case 's':
		    if (! wlflag) { /* note: if wlflag, treate s as S */
                                    /* by falling through to next case */
			p = bp = get_arg(args, nargs, char *);
			if(!p) p = bp = "(null)";	/* Be tolerant of NULL pointers */
			if (nflag) {
				/* compute length of escape'd ASCII string */
				for(n=0;*p;) {
					p += NCdec(p,&Nc);
					n += NCesc(&Nc,buf);
				}
				if (prec>=0 && prec<n)
					n = prec;
			}
			else if (prec < 0) {
#ifdef KJI
				n = (bflag ? strlen(bp) : NLcplen(bp));
#else
				n = (bflag ? strlen(bp) : NLstrdlen(bp));
#endif
				p += strlen (bp);
			}
			else {
#ifdef KJI
				n = 0;
				while ( *p && n < prec) {
				    if (bflag) {
					if (NCisshift (*p))
					    if (n + 1 >= prec || *(p+1) == '\0') {
						*suffix-- = '\0';
						*suffix = ' ';
						break;
					    } else {
						p++;
						n++;
					    }
					p++;
				    } else
					p += NLchrlen (p);
				    n++;
				};
#else
				for (n=0; *p && n<prec; n++)
					p += bflag ? 1 : NLchrlen(p);
				if (p > bp && NCisshift(p[-1])) {
					*suffix-- = '\0';
					*suffix = ' ';
					p--;
				}
#endif
			}
			break;
		    } /* ! wlflag */

		case 'S':
			Np = Nbp = get_arg(args, nargs, NLchar *);
			if(!Np) Np=Nbp= (void *)"(null)";
			if (nflag) {
				for(n=0; *Np != 0; Np++)
					n += NCesc(Np,buf);
				if (prec>=0 && prec<n) n = prec;
			}
			else if (prec < 0) {
				n = NCstrlen(Nbp);
				if (bflag)
				    for(; *Np != 0; Np++) {
					int i = NCchrlen(*Np);
					width -= i-1;
				    }
			}
			else {
				for(; *Np != 0 && prec > 0; Np++, p++)
					if (bflag) {
						int i = NCchrlen(*Np);
						width -= i-1;
						prec -= i;
					} else
						prec--;
				n = Np-Nbp;
				if ( prec<0 ) {
					n--;
					*suffix-- = '\0';
					*suffix = ' ';
				}
			}
			break;

		case '\0': /* unexpected end of format; return error */
			RFREE(nargs);
			RFREE(nformat);
			return (EOF);

		}

		/* Calculate number of padding blanks */
		if (lzero < 0)
			lzero = 0;
		if (rzero < 0)
			rzero = 0;
		/* nb = number of bytes in buffer.  Not used if */
		/* nflag asserted or fcode = 'S' */
		nb = p - bp;
		/* nb also not used if fcode == 'C' */
		if (!((fcode == 'S') || (fcode == 's') || (fcode == 'C') ||
                    (wlflag && (fcode == 'c'))))
			n = nb;
			/* otherwise n, # chars, has already been computed */
		k = n + lzero + rzero +
		    (prefix[0] == '\0' ? 0 : prefix[1] == '\0' ? 1 : 2) +
		    (&expbuf[MAXESIZ] - suffix);
		count += (width > k) ? width : k;

		/* Blanks on left if required */
		if (!fminus)
			while (--width >= k)
#ifdef KJI
				PUTBLANK;
#else
				PUTC(' ', iop);
#endif

		/* Prefix, if any */
		while (*prefix != '\0') {
#ifdef KJI
			if (jflag) {
				i = NCenc (&(_atojis (*prefix)), sjbuf);
				PUT (sjbuf, i);
				prefix++;
			} else
#endif
				PUTC(*prefix++, iop);
			rzero--;
		}

		/* Zeroes on the left */
		while (--lzero >= 0)
#ifdef KJI
			PUTZERO;
#else
			PUTC('0', iop);
#endif

		/* The value itself */
		if (nflag)
			while(n) {
			      if (fcode=='S' || wlflag && fcode=='s')
				      Nc = *Nbp++;
			      else
				      bp += NCdec(bp,&Nc);
			      i = NCesc(&Nc,buf);
			      if (i>n) i=n;
			      PUT(buf,i);
			      n -= i;
		       }
		else if (fcode=='S' || wlflag && fcode=='s')
			while(n--) {
			    i = NCenc(Nbp,buf);
			    PUT(buf, i);
			    Nbp++;  /* don't combine w/macro above */
			    /*	NOTE: Keep this loop's statement ordering
			     *	to foil compiler optimization,
			     *	which would currently be incorrect.
			     */
			}
		else if (fcode=='C' || wlflag && fcode=='c')
		       {PUT(buf, 2);} /* buf is char array, NL char 2 bytes */
		else if (nb>0)
#ifdef KJI
			if (jflag && fcode != '%' ) {
/* Removed the following line since %Jc, %JC, and %Js are valid formats
** P27563
			  && fcode != 'c' && fcode != 'C' && fcode != 's' ) {
} */
			    while (nb--) {
				i = NCenc (&(_atojis (*bp)), sjbuf);
				PUT(sjbuf, i);
				bp++;
			    }
			} else
#endif
			    PUT(bp, nb);

		/* Zeroes on the right */
		while (--rzero >= 0)
#ifdef KJI
			PUTZERO;
#else
			PUTC('0', iop);
#endif

		/* The suffix */
		while (*suffix != '\0')
#ifdef KJI
		    if (jflag) {
			i = NCenc (&(_atojis (*suffix)), sjbuf);
			PUT (sjbuf, i);
			suffix++;
		    } else
#endif
			PUTC(*suffix++, iop);

		/* Blanks on the right if required */
		while (--width >= k)
#ifdef KJI
			PUTBLANK;
#else
			PUTC(' ', iop);
#endif
	}
}


	/* Last_valloc is the last alloced index for the vlist array */
static	int	last_valloc;

	/* Vlist is the array of arguments corresponding to each format code */
static	arglist *vlist;

	/* Last_alloc is the last alloced index for the alist array */
static	int	last_alloc;

	/* Alist is the array of arguments in the original order */
static	arglist *alist;

	/* arg_count is the total number of arguments in format string */
static	int	arg_count;

	/* max_arg is highest numbered argument */
static	int	max_arg;

/*	argnum: common code for reorder, width.
 *		Originally design for PTM p36289
 *		to support XPG3 standards.
 */

static int
argnum(prt, rcode)
char	**prt;
int	*rcode;
{
	int	arg_index, i;
	char	fcode;

	for (arg_index = 0; isdigit(fcode = *((*prt)++)); )
		arg_index = arg_index * 10 + tonumber(fcode);
	if (arg_index > max_arg)
		max_arg = arg_index;
	if (arg_index >= last_alloc) {
		i = last_alloc;
		last_alloc += 10;
		alist = (arglist *)
			realloc (alist, last_alloc * sizeof (*alist));
		if (alist == 0) {
			*rcode = -1;
			return (-1);
		}

		/* Make sure no holes are in format string 
		   (checked later) */
		for (; i < last_alloc; i++)
			alist[i].da_type = DA_T_UNDEF;
	}
	arg_index--;	/* the arrays are zero based */
	return(arg_index);
}

/*	width: Used for reorder to handle width and precision.
 *		Originally design for PTM p36289 to support
 *		XPG3 standards.
 */

static
width(prt, new_prt, rc)
char	**prt, **new_prt;
int	*rc;
{
	char	*sav_prt;
	int	index, i;
	register arglist *ap, *vp;

	if (**prt == '*' && isdigit( *(*prt+1))){
		*(++(*new_prt)) = *((*prt)++);
		sav_prt = *prt;
		while( isdigit(*((*prt)++)));
		*((*prt)--);
		if ( **prt == '$') {
			*prt = sav_prt;
			index = argnum(prt, rc);
			if (index == -1)
				return;
			ap = &alist[index];
			if (arg_count >= last_valloc) {
				i = last_valloc;
				last_valloc += 10;
				vlist = (arglist *)
					realloc (vlist,
						last_valloc * sizeof (*vlist));
				if (vlist == 0) {
					*rc = -1;
					return;
				}
			}
			vp = &vlist[arg_count];
			vp->da_arg.au_arg = ap;
			ap->da_type = DA_T_INT;
			arg_count++;
		}
		else	*rc = -1;
	}
	else {
		if ( **prt == '*')
			*rc = -1;
		while ( isdigit( **prt))
			*(++(*new_prt)) = *((*prt)++);
	}
}

/*
 *	Reorder goes through a format statement containing variably
 *	ordered arguments and reorders the arguments accordingly,
 *	stripping out the x$'s.	 Upon success, reorder() returns 0;
 *	the new format string is returned via the argument "bp_new_fmt",
 *	and the new arg pointer is returned via the argument "bp_new_args".
 *	Upon failure, reorder() returns -1; *bp_new_fmt = *bp_new_args = 0.
 */

static int
reorder(wlflag, format, args, bp_new_fmt, bp_new_args)
int	wlflag;
char	*format;
va_list args;
char		**bp_new_fmt;	/* new format */
arglist		**bp_new_args;	/* new argument list */
{
	/* Arg_index counts number of args (i.e. % occurrences) */
	int	arg_index;

	/* new_fmt is the working pointer in the new format string*/
	char	*new_fmt;

	/* Format code */
	register int	fcode;

	/* Return code */
	int	rcode;

	/* Flags - nonzero if corresponding character is in format */
	int	length;		/* l */

	/* work variable(s) */
	int	i;

	register arglist *ap, *vp;

	/*
	 *	The format loop interogates all conversion characters to
	 *	determine the address and type of each argument, building the
	 *	alist and vlist arrays and filling in new_fmt.
	 */
	arg_count = max_arg = 0;
	alist = vlist = 0;
	*bp_new_fmt = 0;
	*bp_new_args = 0;
	rcode = 0;

	*bp_new_fmt = new_fmt = malloc (strlen (format)+1);

	last_alloc = 10;
	alist = (arglist *) malloc (10 * sizeof (*alist));

	last_valloc = 10;
	vlist = (arglist *) malloc (10 * sizeof (*vlist));

	if (new_fmt == 0 || alist == 0 || vlist == 0) {
		rcode = -1;
		goto ret;
	}

	for (i = strlen (format); i; i--)
		*(new_fmt+i) = '\0';

	/* Make sure no holes are in format string (checked later) */
	for (i = 0; i < last_alloc; i++)
		alist[i].da_type = DA_T_UNDEF;

	new_fmt--;
	for ( ; ; ) {
		while ((*++new_fmt = *format) != '\0') {
			if (*format == '%') {
				if (*(format + 1) == '%') {
					*++new_fmt = *++format;
				}
				else
					break;
			}
			format++;
		}

		fcode = *format;
		if (fcode == '\0') { /* end of format; normal return */
			break;	  /* Now do the get_args loop */
		}
		/*
		 *	% has been found.
		 *	First extract digit$ from format and compute arg_index.
		 *	Next parse the format specification.
		 */
		format++;
		arg_index = argnum(&format, &rcode);
		if (rcode == -1) goto ret;
		ap = &alist[arg_index];

		for ( ; ; ) { /* Scan the <flags> */
			switch (fcode = *++new_fmt = *(format++)) {
			case '+':
			case '-':
			case ' ':
			case '#':
			case 'B':
			case 'N':
			case 'J':
				continue;
			}
			*new_fmt-- = '\0';
			format--;
			break;
		}

		/* Scan the field width */
		width(&format, &new_fmt, &rcode);
		if (rcode == -1 ) goto ret;

		/* Scan the precision */
		if (*format == '.') {
			*++new_fmt = *format++;
			width(&format, &new_fmt, &rcode);
			if (rcode == -1 ) goto ret;
		}

		/* Now store pointer to this arg into arg-count-based
		 * list.  Must defer this till after we have scanned
		 * width and precision above!
		 */

		if (arg_count >= last_valloc) {
			i = last_valloc;
			last_valloc += 10;
			vlist = (arglist *)
				realloc (vlist, last_valloc * sizeof (*vlist));
			if (vlist == 0) {
				rcode = -1;
				goto ret;
			}
		}
		vp = &vlist[arg_count];
		vp->da_arg.au_arg = ap;
		arg_count++;

		/* Scan the length modifier */
		length = 0;
		switch (*format) {
		case 'l':
			length++;
			/* No break */
		case 'L': /* long double == double in this implementation */
		case 'h':
			*++new_fmt = *format++;
		}

		switch (fcode = *++new_fmt = *format++) {

		case 'i':
		case 'd':
		case 'u':
		case 'o':
		case 'X':
		case 'x':
		case 'p':				/* p36961 */
		case 'n':				/* p36963 */
			/* Fetch the argument type */
			if(length)
				ap->da_type = DA_T_LONG;
			else if(fcode == 'd' || fcode == 'i')
				ap->da_type = DA_T_INT;
			else
				ap->da_type = DA_T_UNSIGNED;
			break;

		case 'E':
		case 'e':
		case 'f':
		case 'G':
		case 'g':
			/* Fetch the type */
			ap->da_type = DA_T_DOUBLE;
			break;

		/* case '%':   */
		default:
			break;

		case 'c':
			if (wlflag)
                            break; /* note: no case for C in this case */
			ap->da_type = DA_T_INT;
			break;

		case 's':
		case 'S':
			ap->da_type = DA_T_POINTER;
			break;

		case '\0': { /* unexpected end of format; return error */
			rcode = -1;
			goto ret;
			}

		}
	}

	/* Get ARGS Loop:  fill in the alist array...pointers to args. */
	for (i=0, ap = alist; i < max_arg; i++, ap++) {
		switch (ap->da_type) {
		case DA_T_UNDEF:
			/* Make sure no holes are in format string */
			rcode = -1;
			goto ret;
		case DA_T_INT:
			ap->da_arg.au_int = va_arg(args,int);
			break;
		case DA_T_LONG:
			ap->da_arg.au_long = va_arg(args,long);
			break;
		case DA_T_UNSIGNED:
			ap->da_arg.au_unsigned = va_arg(args,unsigned);
			break;
		case DA_T_POINTER:
			ap->da_arg.au_pointer = va_arg(args,void *);
			break;
		case DA_T_DOUBLE:
			ap->da_arg.au_double = va_arg(args,double);
			break;
		}
	}

	/* Reorder ARGS Loop:  loop through vlist array to gather
			       argument addresses and types */
	for (i = 0, vp = vlist; i < arg_count; i++, vp++) {
		ap = vp->da_arg.au_arg;
		vp->da_arg = ap->da_arg;
		vp->da_type = ap->da_type;
	}
	*bp_new_args = vlist;
	vlist = 0;	/* don't free our result on exit */
	
ret:
	RFREE (alist);
	RFREE (vlist);
	return (rcode);
}
