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
static char rcsid[] = "@(#) $RCSfile: od.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:32:42 $";
#endif

/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */
#ifndef lint
static char sccsid[] = "od.c	1.12  com/cmd/scan,3.1,9021 4/3/90 22:49:06";
#endif /* lint */

/*
 * od -- octal, hex, decimal, character dump of data in a file.
 *
 * usage:  od [-abBCcdDefFhHiIlLopPs[n]vw[n]xX] [file] [[+]offset[.][b] [label]]
 *
 * where the option flags have the following meaning:
 *   character	object	radix	signed?
 *	a	byte	(10)	(n.a.)	ASCII named byte stream
 *	b	byte	  8	 no	byte octal
 *	c	byte	 (8)	(no)	character with octal non-graphic bytes
 *	d	short	 10	 no
 *	D	long	 10	 no
 *	e,F	double	(10)		double precision floating pt.
 *	f	float	(10)		single precision floating pt.
 *	h,x	short	 16	 no
 *	H,X	long	 16	 no
 *	i	short	 10	yes
 *	I,l,L	long	 10	yes
 *	o,B	short	  8	 no	(default conversion)
 *	O	long	  8	 no
 *	s[n]	string	 (8)		ASCII graphic strings
 *
 *	p				indicate EVEN parity on 'a' conversion
 *	P				indicate ODD parity on 'a' conversion
 *	v				show all data - don't skip like lines.
 *	w[n]				bytes per display line
 *
 * More than one format character may be given.
 * If {file} is not specified, standard input is read.
 * If {file} is not specified, then {offset} must start with '+'.
 * {Offset} may be HEX (0xnnn), OCTAL (0nn), or decimal (nnn.).
 * The default is octal. The same radix will be used to display the address.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <NLctype.h>
#include <NLchar.h>
#include <locale.h>

#include <nl_types.h>
#include "od_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_OD,Num,Str)

#define DBUF_SIZE	BUFSIZ
#define BIG_DBUF	32
#define NO		0
#define YES		1
#define EVEN	       -1
#define ODD		1
#define UNSIGNED	0
#define SIGNED		1
#define PADDR		1
#define MIN_SLEN	3

static int	a_put();
static int	b_put();
static int	c_put();
static int	C_put();
static int	do_C_put();
static int	s_put();
static int	us_put();
static int	l_put();
static int	f_put();
static int	d_put();
static int	st_put();
static int	parity();
static int	canseek();
static int	put_sbuf();
static void	dumbseek();
static void	offset();
static void	pr_sbuf();
static void	put_addr();
static void	line();

struct dfmt {
	int	df_field;	/* external field required for object */
	int	df_size;	/* size (bytes) of object */
	int	df_radix;	/* conversion radix */
	int	df_signed;	/* signed? flag */
	int	df_paddr;	/* "put address on each line?" flag */
	int	(*df_put)();	/* function to output object */
	char	*df_fmt;	/* output string format */
} *conv_vec[32];		/* vector of conversions to be done */

struct dfmt	ascii	= { 3, sizeof (char),   10,        0, PADDR,  a_put, 0};
struct dfmt	byte	= { 3, sizeof (char),    8, UNSIGNED, PADDR,  b_put, 0};
struct dfmt	cchar	= { 3, sizeof (char),    8, UNSIGNED, PADDR,  c_put, 0};
struct dfmt	Cchar	= { 3, sizeof (char),    8, UNSIGNED, PADDR,  C_put, 0};
struct dfmt	u_s_oct	= { 6, sizeof (short),   8, UNSIGNED, PADDR, us_put, 0};
struct dfmt	u_s_dec	= { 5, sizeof (short),  10, UNSIGNED, PADDR, us_put, 0};
struct dfmt	u_s_hex	= { 4, sizeof (short),  16, UNSIGNED, PADDR, us_put, 0};
struct dfmt	u_l_oct	= {11, sizeof (long),    8, UNSIGNED, PADDR,  l_put, 0};
struct dfmt	u_l_dec	= {10, sizeof (long),   10, UNSIGNED, PADDR,  l_put, 0};
struct dfmt	u_l_hex	= { 8, sizeof (long),   16, UNSIGNED, PADDR,  l_put, 0};
struct dfmt	s_s_dec	= { 6, sizeof (short),  10,   SIGNED, PADDR,  s_put, 0};
struct dfmt	s_l_dec	= {11, sizeof (long),   10,   SIGNED, PADDR,  l_put, 0};
struct dfmt	flt	= {14, sizeof (float),  10,   SIGNED, PADDR,  f_put, 0};
struct dfmt	dble	= {21, sizeof (double), 10,   SIGNED, PADDR,  d_put, 0};
struct dfmt	string	= { 0,               0,  8,        0,    NO, st_put, 0};


char	dbuf[DBUF_SIZE];		/* input buffer */
char	mbuf[DBUF_SIZE];		/* buffer used if 2-byte crosses line*/
char	lastdbuf[DBUF_SIZE];
int	addr_base	= 8;		/* default address base is OCTAL */
long	addr		= 0L;		/* current file offset */
long	label		= -1L;		/* current label; -1 is "off" */
int	dbuf_size	= 16;		/* file bytes / display line */
int	_parity		= NO;		/* show parity on ascii bytes */
char	fmt[]	= "            %s";	/* 12 blanks */
/* we must save globle values and reset them for each call to double byte
 * routines.  For example if you call -CCCCCC, you want print the same line
 * six times.
 */
int straggle = 0;			/* Did word cross line boundary?*/
int straggle_save = 0;
int nls_shift_save = 0;			/* last character was a shift char */
int nls_skip_save = 0;			/* need to skip this byte          */
int already_read = 0;			/* next buffer already read in.    */
int nls_skip = 0;
int nls_shift= 0;
static char	*icvt();
static char	*scvt();
static char	*underline();		/* underline because of parity    */
static long	get_addr();


/*
 * special form of _ctype
 */

#define A	01
#define G	02
#define D	04
#define P	010
#define X	020
#undef	isdigit
#undef	isprint
#undef	isascii
#define isdigit(c)	NCisdigit(c)
#define isgraphic(c)	NCisgraph(c)
#define isprint(c)	NCisprint(c)
#define isascii(c)	NCisalnum(c)
#define ishex(c)	NCisxdigit(c)

char *progname;
int bytes;

main(argc, argv)
int	argc;
char	**argv;
{
	register char *p;
	register char *l;
	register  same;
	struct dfmt	*d;
	struct dfmt	**cv = conv_vec;
	int	showall = NO;
	int	field, llen, nelm;
	int	max_llen = 0;

	(void) setlocale (LC_ALL,"");
	progname = argv[0];
	catd = catopen(MF_OD,0);

	argv++;
	argc--;

	if(argc > 0)
	{
		p = *argv;
		if(*p == '-')
		{
			while(*++p != '\0')
			{
				switch(*p)
				{
				case 'a':
					d = &ascii;
					break;
				case 'b':
					d = &byte;
					break;
				case 'C':
					d = &Cchar;
					break;
				case 'c':
					d = &cchar;
					break;
				case 'd':
					d = &u_s_dec;
					break;
				case 'D':
					d = &u_l_dec;
					break;
				case 'e':
				case 'F':
					d = &dble;
					break;
				case 'f':
					d = &flt;
					break;
				case 'h':
				case 'x':
					d = &u_s_hex;
					break;
				case 'H':
				case 'X':
					d = &u_l_hex;
					break;
				case 'i':
					d = &s_s_dec;
					break;
				case 'I':
				case 'l':
				case 'L':
					d = &s_l_dec;
					break;
				case 'o':
				case 'B':
					d = &u_s_oct;
					break;
				case 'O':
					d = &u_l_oct;
					break;
				case 'p':
					_parity = EVEN;
					continue;
				case 'P':
					_parity = ODD;
					continue;
				case 's':
					d = &string;
					*(cv++) = d;
					while (isdigit(p[1]))
						d->df_size = (10 * d->df_size) + (*++p - '0');
					if (d->df_size <= 0)
						d->df_size = MIN_SLEN;
					showall = YES;
					continue;
				case 'w':
					dbuf_size = 0;
					while (isdigit(p[1]))
						dbuf_size = (10 * dbuf_size) + (*++p - '0');
					if (dbuf_size == 0)
						dbuf_size = BIG_DBUF;
					continue;
				case 'v':
					showall = YES;
					continue;
				default:
					(void)fprintf(stderr, MSGSTR(BADFLAG, "%s: bad flag -%c\n"), progname, *p);
					(void)fprintf(stderr, MSGSTR(USAGE,"Usage: %s [-aBbCcDdeFfHhIiLlOoPpswvXx] [file] [[+]offset[.][b] [label]]\n"),progname);
					exit(1);
				}
				*(cv++) = d;
			}
			argc--;
			argv++;
		}
	}

	/*
	 * if nothing spec'd, setup default conversion.
	 */
	if(cv == conv_vec)
		*(cv++) = &u_s_oct;

	*cv = (struct dfmt *)0;

	/*
	 * calculate display parameters
	 */
	for (cv = conv_vec; d = *cv; cv++)
	{
		nelm = (dbuf_size + d->df_size - 1) / d->df_size;
		llen = nelm * (d->df_field + 1);
		if (llen > max_llen)
			max_llen = llen;
	}

	/*
	 * setup df_fmt to point to uniform output fields.
	 */
	for (cv = conv_vec; d = *cv; cv++)
	{
		if (d->df_field)	/* only if external field is known */
		{
			nelm = (dbuf_size + d->df_size - 1) / d->df_size;
			field = max_llen / nelm;
			d->df_fmt = fmt + 12 - (field - d->df_field);
		}
	}

	/*
	 * input file specified ?
	 */
	if(argc > 0 && **argv != '+')
	{
		if (freopen(*argv, "r", stdin) == NULL)
		{
			perror(*argv);
			exit(1);
		}
		argv++;
		argc--;
	}

	/*
	 * check for possible offset [label]
	 */
	if (argc > 0)
	{
		addr = get_addr(*argv);
		offset(addr);
		argv++;
		argc--;

		if (argc > 0)
			label = get_addr(*argv);
	}

	/*
	 * main dump loop
	 */
	same = -1;
	while ((already_read || (bytes = fread(dbuf, (size_t)1, (size_t)dbuf_size, stdin)) > 0))
	{
		if (already_read)
		{
			already_read = 0;
			(void)memcpy (dbuf,mbuf,dbuf_size);
		}
		if (same>=0 && bcmp(dbuf, lastdbuf, dbuf_size) == 0 && !showall)
		{
			if (same==0)
			{
				(void)printf("*\n");
				same = 1;
			}
		}
		else
		{
			line(bytes);
			same = 0;
			p = dbuf;
			l = lastdbuf;
			for (nelm = 0; nelm < dbuf_size; nelm++)
			{
				*l++ = *p;
				*p++ = '\0';
			}
		}
		addr += bytes;
		if (label >= 0)
			label += bytes;
	}

	/*
	 * Some conversions require "flushing".
	 */
	bytes = 0;
	for (cv = conv_vec; *cv; cv++)
	{
		if ((*cv)->df_paddr)
		{
			if (bytes++ == 0)
				put_addr(addr, label, '\n');
		}
		else
			(*((*cv)->df_put))(0, *cv);
	}
	return(0);
}

/*
 * NAME: put_addr
 *                                                                    
 * FUNCTION:  Print out the current file address.
 *                                                                    
 */  

static void
put_addr(a, l, c)
long	a;
long	l;
char	c;
{
	(void)fputs(icvt(a, addr_base, UNSIGNED, 7), stdout);
	if (l >= 0)
		(void)printf(" (%s)", icvt(l, addr_base, UNSIGNED, 7));
	putchar(c);
}

/*
 * NAME: line
 *                                                                    
 * FUNCTION:  When line is called we have determined the line is different
 *		from the previous line.  We then print it out in all the
 *		formats called for in the command line.
 *                                                                    
 */  
static void
line(n)
int	n;
{
	register i, first;
	register struct dfmt *c;
	register struct dfmt **cv = conv_vec;
	int 	newstraggle = 0,
		newnls_skip = 0,
		newnls_shift =0;

	straggle_save = straggle;
	nls_shift_save = nls_shift;
	nls_skip_save = nls_skip;
	first = YES;
	while (c = *cv++)
	{
		straggle = straggle_save;
		nls_shift = nls_shift_save;
		nls_skip = nls_skip_save;
		if (c->df_paddr)
		{
			if (first)
			{
				put_addr(addr, label, ' ');
				first = NO;
			}
			else
			{
				putchar('\t');
				if (label >= 0)
					(void)fputs("\t  ", stdout);
			}
		}
		i = 0;
		while (i < n)
			i += (*(c->df_put))(dbuf+i, c);

		newstraggle |= straggle;
		newnls_shift |= nls_shift;
		newnls_skip |= nls_skip;

		if (c->df_paddr)
			putchar('\n');
	}
	straggle = newstraggle;
	nls_skip = newnls_skip;
}

/*
 * NAME: s_put
 *                                                                    
 * FUNCTION: Print out a signed short.
 *                                                                    
 * RETURN VALUE DESCRIPTION:  size of a short.
 *			    
 */  

static
s_put(n, d)
short	*n;
struct dfmt	*d;
{
	(void)printf(d->df_fmt, icvt((long)*n, d->df_radix, d->df_signed, d->df_field));
	return(d->df_size);
}

/*
 * NAME: us_put
 *                                                                    
 * FUNCTION: Print out an unsigned short in the "d" base.  hex, oct, dec.
 *                                                                    
 * RETURN VALUE DESCRIPTION: size of a short
 *			    
 */  
static
us_put(n, d)
unsigned short	*n;
struct dfmt	*d;
{
	(void)printf(d->df_fmt, icvt((long)*n, d->df_radix, d->df_signed, d->df_field));
	return(d->df_size);
}

/*
 * NAME: l_put
 *                                                                    
 * FUNCTION:  print out an long.  -D -H -X -O options
 *                                                                    
 * RETURN VALUE DESCRIPTION:  size of a long.
 *			    
 */  
static
l_put(n, d)
long	*n;
struct dfmt	*d;
{
	(void)printf(d->df_fmt, icvt(*n, d->df_radix, d->df_signed, d->df_field));
	return(d->df_size);
}

/*
 * NAME: d_put
 *                                                                    
 * FUNCTION: print out a double  -E -F option.
 *                                                                    
 * RETURN VALUE DESCRIPTION: size of a double
 *			    
 */  
static
d_put(f, d)
double	*f;
struct dfmt *d;
{
	char fbuf[24];
	struct l { long n[2]; };

#if	vax
	if ((((struct l *)f)->n[0] & 0xff00) == 0x8000)	/* Vax illegal f.p. */
		(void)sprintf(fbuf, "    %08x %08x",
			((struct l *)f)->n[0], ((struct l *)f)->n[1]);
	else
#endif

		(void)sprintf(fbuf, "%21.14e", *f);
	(void)printf(d->df_fmt, fbuf);
	return(d->df_size);
}

/*
 * NAME: f_put
 *                                                                    
 * FUNCTION: print out a floating point. in D format.
 *                                                                    
 * RETURN VALUE DESCRIPTION: size of a float.
 *			    
 */  
static
f_put(f, d)
float	*f;
struct dfmt *d;
{
	char fbuf[16];

#if	vax
	if ((*(long *)f & 0xff00) == 0x8000)	/* Vax illegal f.p. form */
		(void)sprintf(fbuf, "      %08x", *(long *)f);
	else
#endif
		(void)sprintf(fbuf, "%14.7e", *f);
	(void)printf(d->df_fmt, fbuf);
	return(d->df_size);
}


char	asc_name[34][4] = {
/* 000 */	"nul",	"soh",	"stx",	"etx",	"eot",	"enq",	"ack",	"bel",
/* 010 */	" bs",	" ht",	" nl",	" vt",	" ff",	" cr",	" so",	" si",
/* 020 */	"dle",	"dc1",	"dc2",	"dc3",	"dc4",	"nak",	"syn",	"etb",
/* 030 */	"can",	" em",	"sub",	"esc",	" fs",	" gs",	" rs",	" us",
/* 040 */	" sp",	"del"
};

/*
 * NAME: a_put
 *                                                                    
 * FUNCTION:  print out value in asci, using known values for unprintables.
 *                                                                    
 * RETURN VALUE:  1
 *			    
 */  
static
a_put(cc, d)
char	*cc;
struct dfmt *d;
{
	int c = *cc;
	register char *s = "   ";
	register pbit = parity((int)c & 0377);

	c &= 0177;
	if (isgraphic(c))
	{
		s[2] = c;
		if (pbit == _parity)
			(void)printf(d->df_fmt, underline(s));
		else
			(void)printf(d->df_fmt, s);
	}
	else
	{
		if (c == 0177)
			c = ' ' + 1;
		if (pbit == _parity)
			(void)printf(d->df_fmt, underline(asc_name[c]));
		else
			(void)printf(d->df_fmt, asc_name[c]);
	}
	return(1);
}

/*
 * NAME: parity
 *                                                                    
 * FUNCTION: return the parity of a given word.
 *                                                                    
 * RETURN VALUE:  	ODD - Odd parity
 *			EVEN - Even parity
 */  
static
parity(word)
int	word;
{
	register int p = 0;
	register int w = word;
	register int hw;

	if (w)
		do
		{
			p ^= 1;
			hw = (~(-w));	/* lint complains about undefined   */
		} while(w &= hw);	/* evaluation order for w &=(~(-w)) */
	return (p? ODD:EVEN);
}

/*
 * NAME: underline
 *                                                                    
 * FUNCTION: underline a given string.
 *                                                                    
 * RETURN VALUE:  return a pointer to the underlined string.
 *			    
 */  
static char *
underline(s)
char	*s;
{
	static char ulbuf[16];
	register char *u = ulbuf;

	while (*s)
	{
		if (*s != ' ')
		{
			*u++ = '_';
			*u++ = '\b';
		}
		*u++ = *s++;
	}
	*u = '\0';
	return(ulbuf);
}

/*
 * NAME: b_put
 *                                                                    
 * FUNCTION:  print out a byte in octal
 *                                                                    
 * RETURN VALUE:  1
 *			    
 */  
static int
b_put(b, d)
char	*b;
struct dfmt *d;
{
	(void)printf(d->df_fmt, icvt((long)*b & 0377, d->df_radix, d->df_signed, d->df_field));
	return(1);
}

/*
 * NAME: C_put
 *                                                                    
 * FUNCTION: print out a NLS character in asci escape sequences.
 *                                                                    
 * RETURN VALUE:  1
 *			    
 */  

static int 
C_put(cc, d)
char	*cc;
struct dfmt *d;
{
	return (do_C_put (cc,d,1));
}

/*
 * NAME: do_C_put
 *                                                                    
 * FUNCTION: print out a value in asci or asci escape sequences.
 *                                                                    
 * RETURN VALUE:  1
 *			    
 */  
static int 
do_C_put(cc, d,doesq)
unsigned char	*cc;
struct dfmt *d;
int doesq;
{
	register char	*s;
	register int	n;
	register int	c = *cc;
	char buffer[8];
	if (!nls_shift  && (NCisshift (c)))
	{
		char two_char[3];
		NLchar nls;

		nls_shift++;
		nls_skip++;
		two_char[0] = c;

		if (cc[1] == '\0')
		{
			straggle++;
			if (!already_read)
			{
				if ((bytes = fread(mbuf, (size_t)1, (size_t)dbuf_size, stdin)) <= 0)
				{
				/* Only half of a double byte charater found. */
					straggle = 0;
					already_read = 0;
					two_char[1] = 0;
				}
				else
				{
					already_read++;
					two_char[1] = mbuf[0];
				}
			}
			else
				two_char[1] = mbuf[0];
		}
		else
			two_char[1] = cc[1];

		two_char[2] = NULL;
		if (NCdecode (two_char,&nls) != 2)
		{
		  /* This means there was only a partial shift character */
		  ;
		}
		if (doesq)
		{
			bzero(buffer, sizeof(buffer));
			NCesc (&nls,buffer);
			s = buffer;
		}
		else
			s = two_char;

	}
	else if (nls_skip)
	{
		nls_skip = 0;
		nls_shift = 0;
		if (straggle || !doesq) 
			(void)fputs ("     ",stdout);
		else
			(void)fputs ("  ",stdout);

		straggle = 0;
		return (1);
	}
	else
	{
		s = scvt(c, d);
	}
	for (n = d->df_field - NLstrlen(s); n > 0; n--)
		putchar(' ');
	(void)printf(d->df_fmt, s);
	return(1);
}
/*
 * NAME: c_put
 *                                                                    
 * FUNCTION: display value as character.
 *                                                                    
 * RETURN VALUE: 1
 *			    
 */  
static
c_put(cc, d)
char	*cc;
struct dfmt *d;
{
	return (do_C_put(cc,d,0));
}

/*
 * NAME: scvt
 *                                                                    
 * FUNCTION:  convert the character to a representable string.
 *                                                                    
 * RETURN VALUE:  A pointer to the string.
 *			    
 */  
static char *scvt(c, d)
int	c;
struct dfmt	*d;
{
	static char s[2];

	switch(c)
	{
		case '\0':
			return("\\0");

		case '\b':
			return("\\b");

		case '\f':
			return("\\f");

		case '\n':
			return("\\n");

		case '\r':
			return("\\r");

		case '\t':
			return("\\t");

		default:
			if (isprint(c))
			{
				s[0] = c; /* Depends on "s" being STATIC initialized to zero */
				return(s);
			}
			return(icvt((long)c, d->df_radix, d->df_signed, d->df_field));
	}
}

/*
 * Look for strings.
 * A string contains bytes > 037 && < 177, and ends with a null.
 * The minimum length is given in the dfmt structure.
 */

#define CNULL		'\0'
#define S_EMPTY	0
#define S_FILL	1
#define	S_CONT	2
#define SBUFSIZE	1024

static char	str_buf[SBUFSIZE];
static int	str_mode = S_EMPTY;
static char	*str_ptr;
static long	str_addr;
static long	str_label;

/*
 * NAME: st_put
 *                                                                    
 * FUNCTION:  print a string if it is at least d characters long.
 *                                                                    
 * RETURN VALUE:  1
 *			    
 */  
static 
st_put(cc, d)
char	*cc;
struct dfmt	*d;
{
	register int	c;

	if (cc == 0)
	{
		pr_sbuf(d, YES);
		return(1);
	}

	c = *(unsigned char *)cc;

	if (str_mode & S_FILL)
	{
		if (isascii(c))
			put_sbuf(c, d);
		else
		{
			*str_ptr = CNULL;
			if (c == NULL)
				pr_sbuf(d, YES);
			str_mode = S_EMPTY;
		}
	}
	else if (isascii(c))
	{
		str_mode = S_FILL;
		str_addr = addr + (cc - dbuf);	  /* ugly */
		if ((str_label = label) >= 0)
			str_label += (cc - dbuf); /*  ''  */
		str_ptr = str_buf;
		put_sbuf(c, d);
	}

	return(1);
}

static
put_sbuf(c, d)
int	c;
struct dfmt	*d;
{
	*str_ptr++ = c;
	if (str_ptr >= (str_buf + SBUFSIZE))
	{
		pr_sbuf(d, NO);
		str_ptr = str_buf;
		str_mode |= S_CONT;
	}
}

/*
 * NAME: pr_sbuf
 *                                                                    
 * FUNCTION: print out the string buffer.
 */  

static void
pr_sbuf(d, end)
struct dfmt	*d;
int	end;
{
	register char	*p = str_buf;

	if (str_mode == S_EMPTY
	    || (!(str_mode & S_CONT) && (str_ptr - str_buf) < d->df_size))
		return;

	if (!(str_mode & S_CONT))
		put_addr(str_addr, str_label, ' ');

	while (p < str_ptr)
		(void)fputs(scvt(*p++, d), stdout);

	if (end)
		putchar('\n');
}

/*
 * integer to ascii conversion
 *
 * This code has been rearranged to produce optimized runtime code.
 */

#define MAXINTLENGTH	32
static char	_digit[] = "0123456789abcdef";
static char	_icv_buf[MAXINTLENGTH+1];
static long	_mask = 0x7fffffff;

/*
 * NAME: icvt
 *                                                                    
 * FUNCTION: return from a given stream the first value.
 *                                                                    
 * RETURN VALUE:  the value is an ascii stream printed in the RADIX given.
 */  
static char *
icvt (value, radix, issigned, ndigits)
long	value;
int	radix;
int	issigned;
int	ndigits;
{
	register long	val = value;
	register long	rad = radix;
	register char	*b = &_icv_buf[MAXINTLENGTH];
	register char	*d = _digit;
	register long	tmp1;
	register long	tmp2;
	long	rem;
	long	kludge;
	int	sign;

	if (val == 0)
	{
		*--b = '0';
		sign = 0;
		goto done; /*return(b);*/
	}

	if (issigned && (sign = (val < 0)))	/* signed conversion */
	{
		/*
		 * It is necessary to do the first divide
		 * before the absolute value, for the case -2^31
		 *
		 * This is actually what is being done...
		 * tmp1 = (int)(val % rad);
		 * val /= rad;
		 * val = -val
		 * *--b = d[-tmp1];
		 */
		tmp1 = val / rad;
		*--b = d[(tmp1 * rad) - val];
		val = -tmp1;
	}
	else				/* unsigned conversion */
	{
		sign = 0;
		if (val < 0)
		{	/* ALL THIS IS TO SIMULATE UNSIGNED LONG MOD & DIV */
			kludge = _mask - (rad - 1);
			val &= _mask;
			/*
			 * This is really what's being done...
			 * rem = (kludge % rad) + (val % rad);
			 * val = (kludge / rad) + (val / rad) + (rem / rad) + 1;
			 * *--b = d[rem % rad];
			 */
			tmp1 = kludge / rad;
			tmp2 = val / rad;
			rem = (kludge - (tmp1 * rad)) + (val - (tmp2 * rad));
			val = ++tmp1 + tmp2;
			tmp1 = rem / rad;
			val += tmp1;
			*--b = d[rem - (tmp1 * rad)];
		}
	}

	while (val)
	{
		/*
		 * This is really what's being done ...
		 * *--b = d[val % rad];
		 * val /= rad;
		 */
		tmp1 = val / rad;
		*--b = d[val - (tmp1 * rad)];
		val = tmp1;
	}

done:
	if (sign)
		*--b = '-';

	tmp1 = ndigits - (&_icv_buf[MAXINTLENGTH] - b);
	tmp2 = issigned? ' ':'0';
	while (tmp1 > 0)
	{
		*--b = tmp2;
		tmp1--;
	}

	return(b);
}

/*
 * NAME: get_addr
 *                                                                    
 * FUNCTION: return the address of the given string in the file.
 */  

static long get_addr(s)
register char *s;
{
	register char *p;
	register long a;
	register int d;

	if (*s=='+')
		s++;
	if (*s=='x')
	{
		s++;
		addr_base = 16;
	}
	else if (*s=='0' && s[1]=='x')
	{
		s += 2;
		addr_base = 16;
	}
	else if (*s == '0')
		addr_base = 8;
	p = s;
	while(*p)
	{
		if (*p++=='.')
			addr_base = 10;
	}
	for (a=0; *s; s++)
	{
		d = *s;
		if(isdigit(d))
			a = a*addr_base + d - '0';
		else if (ishex(d) && addr_base==16)
			a = a*addr_base + d + 10 - 'a';
		else
			break;
	}

	if (*s == '.')
		s++;
	if(*s=='b')
		a *= 512;
	if(*s=='B')
		a *= 1024;

	return(a);
}

/*
 * NAME: offset
 *                                                                    
 * FUNCTION:  seek to the appropriat starting place.
 *                                                                    
 */  

static void
offset(a)
long	a;
{
	if (canseek(stdin))
	{
		/*
		 * in case we're accessing a raw disk,
		 * we have to seek in multiples of a physical block.
		 */
		(void)fseek(stdin, a & 0xfffffe00L, 0);
		a &= 0x1ffL;
	}
	dumbseek(stdin, a);
}

/*
 * NAME: dumbseek
 *                                                                    
 * FUNCTION:  just read from stdin until the appropriate spot is reached.
 */  

static void
dumbseek(s, s_offset)
FILE	*s;
long	s_offset;
{
	char	buf[BUFSIZ];
	int	n;
	int	nr;

	while (s_offset > 0)
	{
		nr = (s_offset > BUFSIZ) ? BUFSIZ : (int)s_offset;
		if ((n = fread(buf, (size_t)1, (size_t)nr, s)) != nr)
		{
			(void)fprintf(stderr, "EOF\n");
			exit(1);
		}
		s_offset -= n;
	}
}


/*
 * NAME: canseek
 *                                                                    
 * FUNCTION: check to see if we are reading from a pipe, file, or stdin.
 *                                                                    
 * RETURN VALUE:	1 if file 
 *			0 if pipe or stdin			    
 */  
static
canseek(f)
FILE	*f;
{
	struct stat statb;

	return( (fstat((int)fileno(f),&statb)==0) &&
		(statb.st_nlink > 0) &&		/*!pipe*/
		(!isatty(fileno(f))) );
}
