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
static char rcsid[] = "@(#)$RCSfile: strmatch.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:37 $";
#endif
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
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

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 * D. G. Korn
 * G. S. Fowler
 * AT&T Bell Laboratories
 *
 * match shell file patterns -- derived from Bourne and Korn shell gmatch()
 *
 *	sh pattern	egrep RE	description
 *	----------	--------	-----------
 *	*		.*		0 or more chars
 *	?		.		any single char
 *	[.]		[.]		char class
 *	[!.]		[^.]		negated char class
 * #ifdef KSH_88D
 *      *(.)            (.)*            0 or more of
 *      +(.)            (.)+            1 or more of
 *      ?(.)            (.)?            0 or 1 of
 *      (.)             (.)             1 of
 *      @(.)            (.)             1 of
 *      a|b             a|b             a or b
 *      a&b                             a and b
 *      !(.)                            none of
 * #else
 *	*(.|.)		(.|.)*		0 or more of
 *	+(.|.)		(.|.)+		1 or more of
 *	?(.|.)		(.|.)?		0 or 1 of
 *	@(.|.)		(.|.)		1 of
 *	!(.|.)				none of
 * #endif KSH_88D
 *
 * \ used to escape metacharacters
 *
 *	*, ?, (, |, ), [, \ must be \'d outside of [...]
 *	only ] must be \'d inside [...]
 * #ifdef KSH_88D
 *
 * BUG: unbalanced ) terminates top level pattern
 * #endif KSH_88D
 */

#if (defined(KJI) || defined(NLS))

#include <NLchar.h>
#define REGISTER
wchar_t kchar;
#define getchar(x)	(x += NCdec(x,&kchar), kchar)

#else
#ifdef MULTIBYTE

#include "national.h"

#define REGISTER

#define C_MASK		(3<<(7*ESS_MAXCHAR))	/* character classes	*/
#define getchar(x)	mb_getchar((unsigned char**)(&(x)))

static int		mb_getchar();

#else	/* MULTIBYTE */


#define REGISTER	register
#define getchar(x)	(*x++)

#endif  /* MULTIBYTE */

#endif	/* KJI | NLS */

#define getsource(s,e)	(((s)>=(e))?0:getchar(s))

#if defined(KJI) || defined(NLS)

#include <NLctype.h>
#if (defined(KJI) || defined(NLS))
ALNUM(c) {return(NCisNLchar(c) && NCisalnum(c));}
ALPHA(c) {return(NCisNLchar(c) && NCisalpha(c));}
CNTRL(c) {return(NCisNLchar(c) && NCiscntrl(c));}
DIGIT(c) {return(NCisNLchar(c) && NCisdigit(c));}
GRAPH(c) {return(NCisNLchar(c) && NCisgraph(c));}
LOWER(c) {return(NCisNLchar(c) && NCislower(c));}
PRINT(c) {return(NCisNLchar(c) && NCisprint(c));}
PUNCT(c) {return(NCisNLchar(c) && NCispunct(c));}
SPACE(c) {return(NCisNLchar(c) && NCisspace(c));}
UPPER(c) {return(NCisNLchar(c) && NCisupper(c));}
XDIGIT(c) {return(NCisNLchar(c) && NCisxdigit(c));}
#else
ALPHA(c) {return(isascii(c) && isalpha(c));}
UPPER(c) {return(isascii(c) && isupper(c));}
LOWER(c) {return(isascii(c) && islower(c));}
DIGIT(c) {return(isascii(c) && isdigit(c));}
ALNUM(c) {return(isascii(c) && isalnum(c));}
SPACE(c) {return(isascii(c) && isspace(c));}
PRINT(c) {return(isascii(c) && isprint(c));}
PUNCT(c) {return(isascii(c) && ispunct(c));}
XDIGIT(c) {return(isascii(c) && isxdigit(c));}
#endif
#  ifdef KJI
JALPHA(c) {return(isjalpha(c));}
JDIGIT(c) {return(isjdigit(c));}
JSPACE(c) {return(isjspace(c));}
JPUNCT(c) {return(isjpunct(c));}
JPAREN(c) {return(isjparen(c));}
JKANJI(c) {return(isjkanji(c));}
JHIRA(c) {return(isjhira(c));}
JKATA(c) {return(isjkata(c));}
JXDIGIT(c) {return(isjxdigit(c));}
JGRAPH(c) {return(isjgraph(c));}
#  endif
struct isarray {
	char *isstr;
	int (*isfunc)();
} istab[] = {
        { "[:alnum:]", ALNUM },
	{ "[:alpha:]", ALPHA },
        { "[:cntrl:]", XDIGIT },
	{ "[:digit:]", DIGIT },
        { "[:graph:]", XDIGIT },
        { "[:lower:]", LOWER },
        { "[:space:]", SPACE },
        { "[:upper:]", UPPER },
	{ "[:print:]", PRINT },
	{ "[:punct:]", PUNCT },
	{ "[:xdigit:]", XDIGIT },
#  ifdef KJI
	{ "[:jalpha:]", JALPHA },
	{ "[:jdigit:]", JDIGIT },
	{ "[:jspace:]", JSPACE },
	{ "[:jpunct:]", JPUNCT },
	{ "[:jparen:]", JPAREN },
	{ "[:jkanji:]", JKANJI },
	{ "[:jhira:]", JHIRA },
	{ "[:jkata:]", JKATA },
        { "[:jxdigit:]", JXDIGIT },
        { "[:jgraph:]", JGRAPH }
#  endif

#define NISTAB (sizeof(istab) / sizeof(struct isarray))
};

#endif

#ifdef  KSH_88D
static  char            *endmatch;
static  int             minmatch;
static  int             grpmatch ();
#endif /* KSH_88D */

static char*		gobble();
static int		onematch();

/*
 * strmatch compares the string s with the shell pattern p
 * returns 1 for match 0 otherwise
 */

#ifdef  KSH_88D
int
strmatch(s, p)
register char*  s;
char*           p;
{
        minmatch = 0;
        return(grpmatch(s, p, s + strlen(s), (char*)0));
}

/*
 * leading substring match
 * first char after end of substring returned
 * 0 returned if no match
 * m: (0-min, 1-max) match
 */

char*
submatch(s, p, m)
register char*  s;
char*           p;
int             m;
{
        endmatch = 0;
        minmatch = !m;
        (void)grpmatch(s, p, s + strlen(s), (char*)0);
        return(endmatch);
}

/*
 * match any pattern in a group
 * | and & subgroups are parsed here
 */

static int
grpmatch(s, p, e, g)
char*           s;
register char*  p;
char*           e;
char*           g;
{
        register char*  a;

        do
        {
                a = p;
                do
                {
                        if (!onematch(s, a, e, g)) break;
                } while (a = gobble(a, '&'));
                if (!a) return(1);
        } while (p = gobble(p, '|'));
        return(0);
}
#else
int
strmatch(s, p)
register char*	s;
register char*	p;
{
	return(submatch(s, p, s + strlen(s), (char*)0));
}

/*
 * match any pattern in a | separated group
 */

int
submatch(s, p, e, g)
char*		s;
register char*	p;
char*		e;
char*		g;
{
	do
	{
		if (onematch(s, p, e, g)) return(1);
	} while (p = gobble(p, 1));
	return(0);
}
#endif /* KSH_88D */

/*
 * match a single pattern
 * e is the end (0) of the substring in s
 * g marks the start of a repeated subgroup pattern
 */

static int
onematch(s, p, e, g)
char*		s;
REGISTER char*	p;
char*		e;
char*		g;
{
	REGISTER int 	pc;
	REGISTER int 	sc;
	REGISTER int	n;
	char*		olds;
	char*		oldp;

#if defined(KJI) || defined(NLS)
	char ifbuf[16], *ib, *eb, *pp;
	int ccc;
#endif

	do
	{
		olds = s;
		sc = getsource(s, e);
		switch (pc = getchar(p))
		{
		case '[':
			{
				int	ok = 0;
				int	invert;

				n = 0;
				if (invert = *p == '!') p++;
#ifdef  KSH_88D
                                for (;;) {
                                        if (!(pc = getchar (p))) return (0);
#else
                                while (pc = getchar(p))
                                {
#endif /* KSH_88D */
					if (pc == ']' && n)
					{
						if (ok != invert) break;
						return(0);
					}
					else if (pc == '-' && n && *p != ']')
					{
						if (!(pc = getchar(p)) || pc == '\\' && !(pc = getchar(p))) return(0);
#ifdef MULTIBYTE
						/*
						 * must be in same char set
						 */

						if ((n & C_MASK) != (pc & C_MASK))
						{
							if (sc == pc) ok = 1;
						}
						else
						if (sc >= n && sc <= pc || sc == pc) ok = 1;
#endif
#if defined(KJI) || defined (NLS)
						{
						    wchar_t tsc, tn, tpc, tco;
						    char b[3], *bp = b;
						
						    b[0] = b[1] = b[2] = '\0';
						    _NCe2(n,b[0],b[1]);
						    tn = NCcoluniq(n);
						    if (((tco = NCcollate(n)) < 0) &&
						        (tco = _NLxcolu(tco, &bp, 0 , &tn))) ;
						    tpc = NCcoluniq(pc);
						    if (((tco = NCcollate(pc)) < 0) &&
						        (tco = _NLxcolu(tco, &pc, 0 , &tpc))) ;
						    tsc = NCcoluniq(sc);
						    if (((tco = NCcollate(sc)) < 0) &&
						        (tco = _NLxcolu(tco, &sc, 0 , &tsc))) ;
							 
						    /* compare coluniq vals if valid */
						    if ( tsc && tn && tpc )
						        	ok |= 
							  	((tsc >= tn) && 
							  	(tsc <= tpc));
						    else
								ok |= 
								((n == sc) || 
								(sc == pc));
						}
#endif
					}
					else if (pc == '\\' && !(pc = getchar(p))) return(0);

#if defined(KJI) || defined(NLS)
					/* Need to check for character class. */
					else if ((pc == '[') && (*p == ':'))
					{
						pp = p;
						ib = ifbuf;
						eb = &ifbuf[15];
						/* save string "[:..." in ifbuf
						 * up to ifbuflen or "]" or "-"
						 */
						do
						{
						    if(pc == '\0' || pc == '\n')
							break;
						    *ib++ = pc;
						} while(((pc = *pp++) != ']') && (pc != '-') && (pc != '[') && (ib < eb));
						/* set n to last char */
						n = pc;

						if (pc == ']')
						    *ib++ = pc;
						*ib = '\0';
						/* check table for entry      */
						for (ccc = 0; ccc < NISTAB; ccc++)
						    if((strcmp(ifbuf,istab[ccc].isstr))==0) {
							ok |= istab[ccc].isfunc(sc);
							p += strlen(ifbuf) - 1;
							break;
						    }
					}
#endif

					else
					{
						if (sc == pc) ok = 1;
						n = pc;
					}
				}
			}
			break;
		case '\\':
			if (!(pc = getchar(p))) return(0);
			/*FALLTHROUGH*/
		default:
			if (pc != sc) return(0);
			break;
		case '(':
		case '*':
		case '?':
		case '+':
		case '@':
		case '!':
			if (pc == '(' || *p == '(')
			{
				char*	subp;

				s = olds;
				oldp = p - 1;
#ifdef  KSH_88D
                                subp = p + (pc != '(');
#else
                                subp = p;
                                if (pc == '(') pc = '@';
                                else subp++;
#endif /* KSH_88D */
				if (!(p = gobble(subp, 0))) return(0);
				if ((pc == '*' || pc == '?' || pc == '+' && oldp == g))
				{
					if (onematch(s, p, e, (char*)0)) return(1);
#ifdef  KSH_88D
                                        if (!sc || !getsource(s, e)) return(0);
#else
                                        if (!getsource(s, e)) return(0);
#endif /* KSH_88D */
				}
				if (pc == '*' || pc == '+') p = oldp;
#ifdef  KSH_88D
                                pc = (pc == '!');
#endif /* KSH_88D */
				do
				{
#ifdef  KSH_88D
                                        if (grpmatch(olds, subp, s, (char*)0) == pc && onematch(s, p, e, oldp)) return(1);
#else
                                        if (submatch(olds, subp, s, (char*)0) == (pc != '!') && onematch(s, p, e, oldp)) return(1);
#endif /* KSH_88D */
				} while (s < e && getchar(s));
				return(0);
			}
			else switch (pc)
			{
			case '*':
				/*
				 * several stars are the same as one
				 */

				while (*p == '*')
					if (*(p + 1) == '(') break;
					else p++;
				oldp = p;
				switch (pc = getchar(p))
				{
				case '@':
				case '!':
				case '+':
					n = *p == '(';
					break;
				case '(':
				case '[':
				case '?':
				case '*':
					n = 1;
					break;
				case '\\':
				if (!(pc = getchar(p))) return(0);
					/*FALLTHROUGH*/
				default:
					n = 0;
					break;
                                case 0:
#ifdef  KSH_88D
                                        endmatch = minmatch ? olds :e;
                                        /* FALLTHROUGH */
                                case '&':
#endif /* KSH_88D */
				case '|':
				case ')':
					return(1);
				}
				p = oldp;
				do
				{
					if ((n || pc == sc) && onematch(olds, p, e, (char*)0)) return(1);
					olds = s;
				} while (sc && (sc = getsource(s, e)));
				return(0);
			case '?':
				break;
			default:
				if (pc != sc) return(0);
				break;
			}
			break;
		case '|':
		case ')':
		case 0:
			return(!sc);
		}
	} while (sc);
	return(0);
}

/*
 * gobble chars up to [ | ] ) keeping track of (...) nesting
 * 0 returned if s runs out
 */

static char*
gobble(s, sub)
REGISTER
char*	s;
register int	sub;
{
#ifdef  KSH_88D
        register int    p = 0;
        register char*  b = 0;

        for (;;) switch (getchar(s))
        {
        case '\\':
                if (getchar(s)) break;
                /*FALLTHROUGH*/
        case 0:
                return(0);
        case '[':
                if (!b) b = s;
                break;
        case ']':
                if (b && b != (s - 1)) b = 0;
                break;
        case '(':
                if (!b) p++;
                break;
        case ')':
                if (!b && p-- <= 0) return(sub ? 0 : s);
                break;
        case '&':
                if (!b && !p && sub == '&') return(s);
                break;
        case '|':
                if (!b && !p)
                {
                        if (sub == '|') return(s);
                        else if (sub == '&') return(0);
                }
#else
	register int	n;

	n = 0;
	for (;;) switch (getchar(s))
	{
	case '\\':
		if (getchar(s)) break;
		/*FALLTHROUGH*/
	case 0:
		return(0);
	case '(':
		n++;
		break;
	case ')':
		if (n-- <= 0) return(sub ? 0 : s);
		break;
	case '|':
		if (sub && !n) return(s);
#endif /* KSH_88D */
		break;
	}
}

#ifdef MULTIBYTE

/*
 * return the next char in (*address) which may be from one to three bytes
 * the character set designation is in the bits defined by C_MASK
 */

static int
mb_getchar(address)
unsigned char**	address;
{
	register unsigned char*	cp = *(unsigned char**)address;
	register int		c = *cp++;
	register int		size;
	int			d;

	if (size = echarset(c))
	{
		d = (size == 1 ? c : 0);
		c = size;
		size = in_csize(c);
		c <<= 7 * (ESS_MAXCHAR - size);
		if (d)
		{
			size--;
			c = (c << 7) | (d & ~HIGHBIT);
		}
		while (size-- > 0)
			c = (c << 7) | ((*cp++) & ~HIGHBIT);
	}
	*address = cp;
	return(c);
}

#endif
