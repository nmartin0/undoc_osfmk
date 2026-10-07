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
static char rcsid[] = "@(#)$RCSfile: expand.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:04 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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

#include	"defs.h"
#include	<sys/limits.h>
#include	<sys/types.h>
#include	<memory.h>
#include	<sys/param.h>
#include	<dirent.h>
#include        <NLchar.h>

#if defined(NLS) || defined(KJI)
static uchar_t             entry[3*NAME_MAX+2];
			/* room for possible 3 bytes/char. when encoded,
			   1 for header, 1 for null at end */
#else
static uchar_t             entry[NAME_MAX+1];
#endif

#if defined(KJI) || defined(NLS)
#include <NLctype.h>
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
	{ "[:alpha:]", ALPHA }, 
	{ "[:upper:]", UPPER }, 
	{ "[:lower:]", LOWER },
	{ "[:digit:]", DIGIT },
	{ "[:alnum:]", ALNUM }, 
	{ "[:space:]", SPACE }, 
	{ "[:print:]", PRINT },
	{ "[:punct:]", PUNCT },
	{ "[:xdigit:]", XDIGIT },
        { "[:graph:]", GRAPH },
        { "[:cntrl:]", CNTRL },
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
/*
 * globals (file name generation)
 *
 * "*" in params matches r.e ".*"
 * "?" in params matches r.e. "."
 * "[...]" in params matches uchar_tacter class
 * "[...a-z...]" in params matches a through z.
 *
 */
static int	addg();


expand(as, rcnt)
	uchar_t	*as;
{
	int	count;
	DIR	*dirf;
	BOOL	dir = 0;
	uchar_t	*rescan = 0;
	register uchar_t	*s, *cs, *p;
	struct argnod	*schain = gchain;
	BOOL	slash;

#ifdef NLSDEBUG
	debug("expand",as);
#endif

	if (trapnote & SIGSET)
		return(0);
	s = cs = as;

	/*
	 * check for meta chars
	 */
	{
		register BOOL open;

		slash = 0;
		open = 0;
		do
		{
			switch (*cs++)
			{
			case 0:
				if (rcnt && slash)
					break;
				else
					return(0);

			case '/':
				slash++;
				open = 0;
				continue;

			case '[':
				open++;
				continue;

			case ']':
				if (open == 0)
					continue;
			case '?':
			case '*':
				if (rcnt > slash)
					continue;
				else
					cs--;
				break;


			default:
				continue;
			}
			break;
		} while (TRUE);
	}

	for (;;)
	{
		if (cs == s)
		{
			s = nullstr;
			break;
		}
		else if (*--cs == '/')
		{
			*cs = 0;
			if (s+1 == cs) {
				static uchar_t fnls_slash[] = { FNLS, '/', '\0' };
				s = fnls_slash;
			}
			break;
		}
	}

#ifdef NLSDEBUG
	debug("expand open",s);
#endif

	
	p = *s ? NLSndecode (s) : (uchar_t *)".";

	if ((dirf = opendir((char *)p)) != NULL)
		dir++;

	count = 0;
	if (*cs == 0)
		*cs++ = 0200;
	if (dir)		/* check for rescan */
	{
		register uchar_t *rs;
		struct dirent *e;

		rs = cs;
		do
		{
			if (*rs == '/')
			{
				rescan = rs;
				*rs = 0;
				gchain = 0;
			}
		} while (*rs++);

		while ((e = readdir(dirf)) && (trapnote & SIGSET) == 0)
		{
			NLSencode(e->d_name, entry, sizeof(entry));
			NLSskiphdr(cs);
			if (entry[1] == '.' && *cs != '.')
				continue;

			if (gmatch(entry, cs))
			{
				addg(s, entry, rescan);
				count++;
			}
		}
		closedir(dirf);

		if (rescan)
		{
			register struct argnod	*rchain;

			rchain = gchain;
			gchain = schain;
			if (count)
			{
				count = 0;
				while (rchain)
				{
					count += expand(rchain->argval, slash + 1);
					rchain = rchain->argnxt;
				}
			}
			*rescan = '/';
		}
	}

	{
		register uchar_t	c;

		s = as;
		while (c = *s) {
			*s++ = (c & STRIP ? c : '/');
			if (NLSfontshift(c) && (*s & 0x80)) 
				*s++;
		}
	}
	return(count);
}


gmatch(s, p)
char	*s, *p;
{
	int		ccc;
	char		ifbuf[16], *ib, *eb, *pp;
#ifdef KJI
	extern char 	*NLSndechr();
#define GET_COLLATE(s,t)							\
	{									\
	    if (NLSfontshift(s))						\
		{char *ch = NLSndechr(t); t += _NCdec2(ch[0], ch[1], s) + 1; }	\
	    else t++;								\
	}
#else
#define GET_COLLATE(s,t) 							\
	{ 									\
	    if (_NCis2(s,*t))  s = _NCd2(s,*t++); 				\
	    else if ((s== FSH0) && (*t & 0x80)) s = *t++; 			\
	}
#endif
	register int    scc,c;

	/* often called with a piece of an encoded string */
	NLSskiphdr(s);
	NLSskiphdr(p);

	if (scc = *s++){
		if ((scc &= STRIP) == 0)
			scc=0200;
	}

	switch (c = *p++)
	{
	case '[':
#ifdef KJI
		s--;
#endif

		GET_COLLATE(scc,s);
		{
			BOOL ok;
			int lc;
			int notflag = 0;

			ok = 0;
			lc = -1;
			if (*p == '!')
			{
				notflag = 1;
				p++;
			}
			while (c = *p++)
			{
				if (c == ']')
					return(ok ? gmatch(s, p) : 0);
#ifdef KJI
				else if ((c == MINUS) && (lc > 0))
#else
				else if (c == MINUS)
#endif
				{
                                        int nc;
                                        wchar_t ncu, cu, co, scu, lcu;
					char tb[3], *tbp = tb;

					nc = *p++ & STRIP;
#ifdef KJI
                                        p--;
#endif
					GET_COLLATE(nc,p);

					scu = NCcoluniq(scc);
					if( ((co = NCcollate(scc)) < 0 ) &&
					    (co = _NLxcolu(co, &s, 0, &scu)) ) ;

					tb[0] = tb[1] = tb[2] = '\0';
					_NCe2(lc, tb[0], tb[1]);
					lcu = NCcoluniq(lc);
					if( ((co = NCcollate(lc)) < 0 ) &&
					    (co = _NLxcolu(co, &tbp, 0, &lcu))) ;

					ncu = NCcoluniq(nc);
					if( ((co = NCcollate(nc)) < 0 ) &&
					    (co = _NLxcolu(co, &p, 0, &ncu)) ) ;

					if (notflag)
					{
						/* if got valid coluniq values */
            if( scu && lcu && ncu && (scu < lcu || scu > ncu) )
              ok++;
						else
							return(0);
					}
					else
					{
						/* if got valid coluniq values */
						if( scu && lcu && ncu )
							ok += (lcu <= scu && scu <= ncu);
					}
				}
				/* Check for character class. */
				else if ((c =='[') && (*p == ':'))
				{
				    pp = p;
				    ib = ifbuf;
				    eb = &ifbuf[15];
					/* the following saves string */
					/* "[:...." in ifbuf, up to   */
					/* ibuflen or "]" or "-"      */
				    do {			
					if(c == '\0' || c == '\n') 
					    break;
					*ib++ = c;
				    } while(((c = *pp++) != ']') && (c != '-') && (c != '[') && (ib < eb));
				    if (c == ']')  
					*ib++ = c;
				    *ib = '\0';
				    /* check table for entry      */
				    for (ccc = 0; ccc < NISTAB; ccc++) 
					if((strcmp(ifbuf,istab[ccc].isstr))==0){
					    ok |= (*istab[ccc].isfunc)(scc);
					    p += strlen(ifbuf) - 1;
					    break;
					}
				}
				else
				{
					lc = c & STRIP;
#ifdef KJI
					p--;
#endif
					GET_COLLATE(lc,p);
					if (notflag)
					{
						if (scc && scc != lc)
							ok++;
						else
							return(0);
					}
					else
					{
						if (scc == lc)
							ok++;
					}
				}
			}
			return(0);
		}

	default:
		c &= STRIP;
		if (c != scc)
			return(0);
#ifdef KJI
		if (NLSfontshift(c)) {
			ccc = NLSenclen(scc);
			while (--ccc)
				if ((c = *p++) && (scc = *s++) && (c != scc))
					return (0);
		}
#endif
		return(scc ? gmatch(s, p) : 0);

	case '?':
		/* Don't match ? against just a fshift */
		if (NLSfontshift(scc&STRIP))
# ifdef KJI
			s += NLSenclen(scc) - 1;
# else
			s++;
# endif
		return(scc ? gmatch(s, p) : 0);

	case '*':
		while (*p == '*')
			p++;

		if (*p == 0)
			return(1);
		--s;
		while (*s)
		{
			if (gmatch(s++, p))
				return(1);
			/* don't match * against just a font-shift */
#ifdef KJI
			if (NLSfontshift(s[-1])) 
				s += NLSenclen(s[-1]) - 1;
#endif
			if (NLSfontshift(s[-1])) s++;
		}
		return(0);

	case 0:
		return(scc == 0);
	}
}

static int
addg(as1, as2, as3)
uchar_t	*as1, *as2, *as3;
{
	register uchar_t	*s1, *s2;
	register int	c;
#ifdef KJI	
	register int	j;
#endif

	s2 = locstak() + BYTESPERWORD;
	s1 = as1;
	while (c = *s1++)
	{
		if ((c &= STRIP) == 0)
		{
			needmem(s2);
			*s2++ = '/';
			break;
		}
		needmem(s2);
		*s2++ = c;
#if defined(NLS) || defined(KJI)
		if (NLSfontshift (c)) {
# ifdef KJI
			j = NLSenclen (c);
			while (--j) {
# endif
			needmem (s2);
			*s2++ = *s1++;
# ifdef KJI		
			}
# endif
		}
#endif
	}
	s1 = as2;
	while (1) {
		needmem(s2);
		if(!(*s2 = *s1++))
			break;
		s2++;
	}
	if (s1 = as3)
	{
		needmem(s2);
		*s2++ = '/';
		do
			needmem(s2);
		while (*s2++ = *++s1);
	}
	makearg(endstak(s2));
}

makearg(args)
	register struct argnod *args;
{
	args->argnxt = gchain;
	gchain = args;
}
