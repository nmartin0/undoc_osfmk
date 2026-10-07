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
static char rcsid[] = "@(#)$RCSfile: glob.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:21:33 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: glob ginit collect acollect sort expand matchdir execbrc match 
 *            amatch Gmatch Gcat addpath rscan scan tglob trim tback globone 
 *            dobackp backeval psave pword
 *
 * ORIGINS: 10,26,27
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
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

#include <dirent.h>
#if defined(NLS) || defined(KJI)
#include <NLctype.h>
#endif
#include "sh.h"

/*
 * C Shell
 */

int	globcnt;

char	*globchars =	"`{[*?";

uchar_t	*gpath, *gpathp, *lastgpathp;
int	globbed;
bool	noglob;
bool	nonomatch;
uchar_t	*entp;
uchar_t	**sortbas;

#if defined(KJI) || defined(NLS)
ALPHA(c) {return(NCisNLchar(c) && NCisalpha(c));}
UPPER(c) {return(NCisNLchar(c) && NCisupper(c));}
LOWER(c) {return(NCisNLchar(c) && NCislower(c));}
DIGIT(c) {return(NCisNLchar(c) && NCisdigit(c));}
ALNUM(c) {return(NCisNLchar(c) && NCisalnum(c));}
SPACE(c) {return(NCisNLchar(c) && NCisspace(c));}
PRINT(c) {return(NCisNLchar(c) && NCisprint(c));}
PUNCT(c) {return(NCisNLchar(c) && NCispunct(c));}
XDIGIT(c) {return(NCisNLchar(c) && NCisxdigit(c));}
#ifdef KJI
JALPHA(c) {return(isjalpha(c));}
JDIGIT(c) {return(isjdigit(c));}
JSPACE(c) {return(isjspace(c));}
JPUNCT(c) {return(isjpunct(c));}
JPAREN(c) {return(isjparen(c));}
JKANJI(c) {return(isjkanji(c));} 
JHIRA(c) {return(isjhira(c));}
JKATA(c) {return(isjkata(c));}
JXDIGIT(c) {return(isjxdigit(c));}
#endif /* KJI */
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
#ifdef KJI
	{ "[:jalpha:]", JALPHA },
	{ "[:jdigit:]", JDIGIT },
	{ "[:jspace:]", JSPACE },
	{ "[:jpunct:]", JPUNCT },
	{ "[:jparen:]", JPAREN },
	{ "[:jkanji:]", JKANJI },
	{ "[:jhira:]", JHIRA },
	{ "[:jkata:]", JKATA },
	{ "[:jxdigit:]", JXDIGIT }
#endif

#define NISTAB (sizeof(istab) / sizeof(struct isarray))
};
#endif /* KJI || NLS */

uchar_t **
glob(v)
	register uchar_t **v;
{
	uchar_t agpath[PATH_MAX+1];
	uchar_t *agargv[GAVSIZ];

	gpath = agpath; gpathp = gpath; *gpathp = 0;
	lastgpathp = &gpath[sizeof agpath - 2];
	ginit(agargv); globcnt = 0;
#ifdef GDEBUG
	printf("glob entered: "); blkpr(v); printf("\n");
#endif
	noglob = adrof("noglob") != 0;
	nonomatch = adrof("nonomatch") != 0;
	globcnt = noglob | nonomatch;
	while (*v)
		collect(*v++);
#ifdef GDEBUG
	printf("glob done, globcnt=%d, gflag=%d: ", globcnt, gflag); blkpr(gargv); printf("\n");
#endif
	if (globcnt == 0 && (gflag&1)) {
		blkfree(gargv), gargv = 0;
		return (0);
	} else
		return (gargv = copyblk(gargv));
}

ginit(agargv)
	uchar_t **agargv;
{

	agargv[0] = 0; gargv = agargv; sortbas = agargv; gargc = 0;
	gnleft = NCARGS - 4;
}

collect(as)
	register uchar_t *as;
{
	register int i;

#ifdef KJI
	if (any_noquote('`', as)) {
#else
	if (any('`', as)) {
#endif
#ifdef GDEBUG
		printf("doing backp of %s\n", as);
#endif
		dobackp(as, 0);
#ifdef GDEBUG
		printf("backp done, acollect'ing, pargc = %d\n", pargc);
#endif
		for (i = 0; i < pargc; i++)
			if (noglob)
				Gcat(pargv[i], "");
			else
				acollect(pargv[i]);
		if (pargv)
			blkfree(pargv), pargv = 0;
#ifdef GDEBUG
		printf("acollect done\n");
#endif
	} else if (noglob || eq(as, "{") || eq(as, "{}")) {
		Gcat(as, "");
		sort();
	} else
		acollect(as);
}

acollect(as)
	register uchar_t *as;
{
	register int ogargc = gargc;

	gpathp = gpath; *gpathp = 0; globbed = 0;
	expand(as);
	if (gargc == ogargc) {
		if (nonomatch) {
			Gcat(as, "");
			sort();
		}
	} else
		sort();
}

sort()
{
	register uchar_t **p1, **p2, *c;
	uchar_t **Gvp = &gargv[gargc];

	p1 = sortbas;
	while (p1 < Gvp-1) {
		p2 = p1;
		while (++p2 < Gvp)
#if defined(NLS) || defined(KJI)
			if (NLstrcmp(*p1, *p2) > 0)
#else
			if (strcmp(*p1, *p2) > 0)
#endif
				c = *p1, *p1 = *p2, *p2 = c;
		p1++;
	}
	sortbas = Gvp;
}

#ifdef KJI
#define ADDPATH(cs)	{addpath(*cs); if (NCisshift(*cs)) addpath(*++cs);\
			 cs++;}
#endif

expand(as)
	uchar_t *as;
{
	register uchar_t *cs;
	register uchar_t *sgpathp, *oldcs;
	struct stat stb;

	sgpathp = gpathp;
	cs = as;
#ifdef GDEBUG
	printf ("expand: string= %s(%d)\n", cs, strlen(cs));
#endif
	if (*cs == '~' && gpathp == gpath) {
		addpath('~');
#if defined(NLS) || defined(KJI)
		cs++;
		while (1) {
#ifdef KJI
		    register int  nlc;
		    if (*cs == NLQUOTE) {
			cs++;
			ADDPATH(cs);
		    } else {
			nlc = NCdechr (cs);
			if (letter(nlc) || digit(nlc) || (nlc == '-'))
			    ADDPATH(cs)
			else
			    break;
		    }
#else
		    if (*cs == NLQUOTE) {
			cs++;
			addpath(*cs++ & TRIM);
		    }
		    else if (letter(*cs) || digit(*cs) || *cs == '-')
			addpath(*cs++);
		    else
			break;
#endif
		}
#else
		for (cs++; letter(*cs) || digit(*cs) || *cs == '-';)
			addpath(*cs++);
#endif
		if (!*cs || *cs == '/') {
			if (gpathp != gpath + 1) {
				*gpathp = 0;
				if (gethdir(gpath + 1))  {
					char e[NL_TEXTMAX];

					sprintf(e,MSGSTR(M_UNKNOWN,"Unknown user: %s"), gpath + 1);
					error(e);
				}
				strcpy(gpath, gpath + 1);
			} else
				strcpy(gpath, value("home"));
			gpathp = strend(gpath);
		}
	}
	while (!any(*cs, globchars)) {
		if (*cs == 0) {
			if (!globbed)
				Gcat(gpath, "");
			else if (stat((char *)gpath, &stb) >= 0) {
				Gcat(gpath, "");
				globcnt++;
			}
			goto endit;
		}
#ifdef KJI
		if (*cs == NLQUOTE) 
			cs++;
		ADDPATH(cs);
#else
#ifdef NLS
		if (*cs == NLQUOTE) {
			cs++;
			addpath(*cs++ & TRIM);
		}
		else addpath(*cs++);
#else
		addpath(*cs++);
#endif
#endif
	}
	oldcs = cs;
	while (cs > as && *cs != '/') {
		cs--;
		if (*cs != NLQUOTE)
			gpathp--;
	}
	if (*cs == '/')
		cs++, gpathp++;
	*gpathp = 0;
	if (*oldcs == '{') {
		execbrc(cs, NOSTR);
		return;
	}
#ifdef GDEBUG
	printf ("expand: pre-matchdir cs = %s, gpath = %s(%d)\n", cs,gpath,strlen(gpath));
#endif
	matchdir(cs);
endit:
#ifdef GDEBUG
	printf ("leaving expand: as = %s, gpath = %s\n", as,gpath);
#endif
	gpathp = sgpathp;
	*gpathp = 0;
}

matchdir(pattern)
	uchar_t *pattern;
{
	struct stat stb;
	register DIR   *dirf;
	register struct dirent *dirbuf;
	uchar_t d_name[NAME_MAX+1];
	register int cnt;

	dirf = opendir ( *gpath == 0 ? "." : (char *)gpath);
	if (dirf == NULL) {
		if (globbed)
			return;
		if (!isdir(stb)) 
			errno = ENOTDIR;
		goto patherr;
	}
	if (stat( (*gpath == 0 ? "." : (char *)gpath), &stb) < 0)
		goto patherr;
	while ( (dirbuf = readdir(dirf)) != NULL ) {
			copdent(d_name, dirbuf->d_name);
			if (match(d_name, pattern)) {
				Gcat(gpath, d_name);
				globcnt++;
			}
	}
	closedir(dirf);
	return;

patherr:
	Perror(gpath);
}

copdent(to, from)
	register uchar_t *to, *from;
{
	register int cnt = NAME_MAX;
	do
		*to++ = *from++;
	while (--cnt);
	*to = 0;
}

execbrc(p, s)
	uchar_t *p, *s;
{
	uchar_t restbuf[BUFR_SIZ + 2];
	register uchar_t *pe, *pm, *pl;
	int brclev = 0;
	uchar_t *lm, savec, *sgpathp;

#ifdef KJI
	for (lm = restbuf; ; ) {
		if (*p == '{')
			break;
		if (*p == NLQUOTE) 
			*lm++ = *p++;
		PUTSTR (lm,p);
	}
#else
	for (lm = restbuf; *p != '{'; *lm++ = *p++)
		continue;
#endif
	for (pe = ++p; *pe; pe++)
	switch (*pe) {

	case '{':
		brclev++;
		continue;

	case '}':
		if (brclev == 0)
			goto pend;
		brclev--;
		continue;

	case '[':
#ifdef KJI
		for (pe++; *pe; pe++) {
			if (*pe == ']')
				break;
			if (*pe == NLQUOTE)
				pe++;
			if (NCisshift(*pe)) 
				pe++;
			if (*pe == '\0')
				break;
		}
#else
		for (pe++; *pe && *pe != ']'; pe++)
			continue;
#endif
		if (!*pe)
			error(MSGSTR(M_MISSBRK, "Missing ]"));
		continue;
#ifdef KJI
	case NLQUOTE:
		++pe;
		/* fall through */
	default:
		if (NCisshift(*pe)) 
			pe++;
		continue;
#endif
	}
pend:
	if (brclev || !*pe)
		error(MSGSTR(M_MISSBRC, "Missing }"));
	for (pl = pm = p; pm <= pe; pm++)
	switch (*pm & (QUOTE|TRIM)) {

	case '{':
		brclev++;
		continue;

	case '}':
		if (brclev) {
			brclev--;
			continue;
		}
		goto doit;

#ifdef KJI	
	case NLQUOTE:
		pm++;
		if (*pm != ',') {
			if (NCisshift(*pm))
				pm++;
			continue;
		}
		pm--;
		/* fall through */
#else
	case ','|QUOTE:
#endif
	case ',':
		if (brclev)
			continue;
doit:
		savec = *pm;
		*pm = 0;
		strcpy(lm, pl);
		strcat(restbuf, pe + 1);
		*pm = savec;
		if (s == 0) {
			sgpathp = gpathp;
			expand(restbuf);
			gpathp = sgpathp;
			*gpathp = 0;
		} else if (amatch(s, restbuf))
			return (1);
		sort();
#ifdef KJI
		if (savec == NLQUOTE) pm++;
#endif
		pl = pm + 1;
		continue;

	case '[':
#ifdef KJI
		for (pm++; *pm; pm++) {
			if (*pm == ']')
				break;
			if (*pm == NLQUOTE)
				pm++;
			if (NCisshift(*pm)) 
				pm++;
			if (*pm == '\0')
				break;
		}
#else
		for (pm++; *pm && *pm != ']'; pm++)
			continue;
#endif
		if (!*pm)
			error(MSGSTR(M_MISSBRK, "Missing ]"));
		continue;
#ifdef KJI
	default:
		if (NCisshift(*pm)) 
			pm++;
		continue;
#endif
	}
	return (0);
}

match(s, p)
	uchar_t *s, *p;
{
	register int c;
	register uchar_t *sentp;
	uchar_t sglobbed = globbed;

	if (*s == '.' && *p != '.')
		return (0);
	sentp = entp;
	entp = s;
#ifdef NLDEBUG
	if (NLdebug) printf("match: %s %s\n",s,p);
#endif
	c = amatch(s, p);
	entp = sentp;
	globbed = sglobbed;
#ifdef NLDEBUG
	if (NLdebug) printf("... = %d\n",c);
#endif
	return (c);
}

amatch(s, p)
	uchar_t *s, *p;
{
	register int  scc;
	register int  ccc;
	uchar_t	      ifbuf[16], *ib, *eb, *pp;
	int ok, lc;
	uchar_t *sgpathp;
	struct stat stb;
	int c, cc;

	globbed = 1;
	for (;;) {
		scc = *s++;
		if (scc==NLQUOTE)
		scc = *s++ & TRIM;
		switch (c = *p++) {

		case '{':
			return (execbrc(p - 1, s - 1));

		case '[':
			ok = 0;
                        lc = -1;
                        s--;
                        scc = (NCisshift(*s)) ? s+=2, _NCd2(*(s-2),*(s-1)) : *s++;

			while (cc = *p++) {
				if (cc == ']') {
					if (ok)
						break;
					return (0);
				}
				if ((cc == '-')  && (lc > 0)) {
                                    wchar_t cu, lcu, ccc, tco;
#ifdef KJI
				    if (*p == NLQUOTE)
					++p;
#endif
                                    c = (NCisshift(*p)) ? p+=2, _NCd2(*(p-2),*(p-1)) : *p++;
                                    /* get uniq collation values */
                                    cu = NCcoluniq(c);
                                    if( ((tco = NCcollate(c)) < 0 ) &&
                                        (tco = _NLxcolu(tco, &p, 0, &cu)) ) ;

                                    lcu = NCcoluniq(lc);
                                    if ( (tco = NCcollate(lc)) < 0 )  {
                                        uchar_t tb[3], *tbp = tb;

                                        tb[0] = tb[1] = tb[2] = '\0';
                                        _NCe2(lc,tb[0], tb[1]);
                                        tco = _NLxcolu(tco, &tbp, 0, &lcu);
                                    }

                                    ccc = NCcoluniq (scc);
                                    if ( ((tco = NCcollate(scc)) < 0 ) &&
                                         (tco = _NLxcolu(tco, &s, 0, &ccc)) ) ;

                                    /* if got nonzero collate values */
                                    if( lcu && cu && ccc )
                                        ok += (lcu <= ccc && ccc <= cu);
                                    else
                                        ok += (lc == scc || scc == cu );

				/* Check for character class. */
				} else if ((cc =='[') && (*p == ':')) {
				    pp = p;
				    ib = ifbuf;
				    eb = &ifbuf[15];
						/* the following saves string */
						/* "[:...." in ifbuf, up to   */
						/* ibuflen or "]" or "-"      */
				    do {			
					if(cc == '\0' || cc == '\n') 
					    break;
					*ib++ = cc;
				    } while(cc = NCdechr(pp),pp+=NLchrlen(pp),
						((cc != ']') && (cc != '-') && 
						(cc != '[') && (ib < eb)));
				    if (cc == ']')  
					*ib++ = cc;
				    *ib = '\0';
				    /* check table for entry      */
				    for (ccc = 0; ccc < NISTAB; ccc++) 
					if((strcmp(ifbuf,istab[ccc].isstr))==0){
					    ok |= (*istab[ccc].isfunc)(scc);
					    p += strlen(ifbuf) - 1;
					    break;
					}
				} else {
				    if (cc != NLQUOTE)
					--p;
				    lc = NCdechr (p);
				    p += NLchrlen (p);
				    if (scc == lc)
					ok++;
				}
			}
			if (cc == 0)
				error(MSGSTR(M_MISSBRK, "Missing ]"));
			continue;

		case '*':
			if (!*p)
				return (1);
			if (*p == NLQUOTE)
				p++;
			if (*p == '/') {
				p++;
				goto slash;
			}
			for (s--; *s; s += NLchrlen(s)) {
				if (amatch(s, p))
					return (1);
			}
			return (0);

		case 0:
			return (scc == 0);

		default:
#ifdef KJI
			if (c == NLQUOTE) c = *p++;
#else
			if (c==NLQUOTE) c = *p++ & TRIM;
#endif
			if (c != scc)
				return (0);
#ifdef KJI
			if (NCisshift(c)) {
				if ((c = *p++) && (scc = *s++) && (c != scc))
					return (0);
			}
#endif
			continue;

		case '?':
			if (scc == 0)
				return (0);
			--s;
			s += NLchrlen(s);
			continue;

		case '/':
			if (scc)
				return (0);
slash:
			s = entp;
			sgpathp = gpathp;
			while (*s)
				addpath(*s++);
			addpath('/');
			if (stat((char *)gpath, &stb) == 0 && isdir(stb))
				if (*p == 0) {
					Gcat(gpath, "");
					globcnt++;
				} else
					expand(p);
			gpathp = sgpathp;
			*gpathp = 0;
			return (0);
		}
	}
}

Gmatch(s, p)
	uchar_t *s, *p;
{
	int		ccc;
	uchar_t		ifbuf[16], *ib, *eb, *pp;
#ifdef KJI
	extern char 	*NLSndechr();
#define GET_COLLATE(s,t)							\
	{									\
	    if (NCisshift(s))						\
		{char *ch = NLSndechr(t); t += _NCdec2(ch[0], ch[1], s) + 1; }	\
	    else t++;								\
	}
#else
#define GET_COLLATE(s,t) 							\
	{ 									\
	    if (_NCis2(s,*t))  s = _NCd2(s,*t++); 				\
	    else if ((s== TRIM) && (*t & 0x80)) s = *t++; 			\
	}
#endif
	register int    scc,c;

	/* often called with a piece of an encoded string */

	if (scc = *s++){
		if ((scc &= TRIM) == 0)
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
			bool ok;
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
					return(ok ? Gmatch(s, p) : 0);
#ifdef KJI
				else if ((c == '-') && (lc > 0))
#else
				else if (c == '-')
#endif
				{
                                        int nc;
                                        wchar_t ncu, cu, co, scu, lcu;
					wchar_t tb[3], *tbp = tb;

					nc = *p++ & TRIM;
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
					lc = c & TRIM;
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

#ifdef NLS
	case NLQUOTE:
		c = *p++ & TRIM;
#endif
#ifdef KJI
	case NLQUOTE:
		c = *p++;
		/* fall through */
#endif
	default:
		if (c != scc)
			return(0);
#ifdef KJI
		if (NCisshift(c)) {
                        if (NCisshift(c) && (c = *p++) && (scc = *s++) &&
                                (c != scc))
                                return (0);
		}
#endif
		return(scc ? Gmatch(s, p) : 0);

	case '?':
                /* Don't match ? against just a fshift */
                if (NCisshift(scc&TRIM))
# ifdef KJI
                        s += NLchrlen(scc) - 1;
# else
                        s++;
# endif
                return(scc ? Gmatch(s, p) : 0);

	case '*':
		while (*p == '*')
			p++;

		if (*p == 0)
			return(1);
		--s;
		while (*s)
		{
			if (Gmatch(s++, p))
				return(1);
			/* don't match * against just a font-shift */
#ifdef KJI
			if (NCisshift(s[-1])) 
				s += NLchrlen(s[-1]) - 1;
#endif
			if (NCisshift(s[-1])) s++;
		}
		return(0);

	case 0:
		return(scc == 0);
	}
}

Gcat(s1, s2)
	register uchar_t *s1, *s2;
{

	gnleft -= strlen(s1) + strlen(s2) + 1;
	if (gnleft <= 0 || ++gargc >= GAVSIZ)
		error(MSGSTR(M_ARGS, "Arguments too long"));
	gargv[gargc] = 0;
	gargv[gargc - 1] = strspl(s1, s2);
}

addpath(c)
	uchar_t c;
{

	if (gpathp >= lastgpathp)
		error(MSGSTR(M_PATH, "Pathname too long"));
	*gpathp++ = c;
	*gpathp = 0;
}

rscan(t, f)
	register uchar_t **t;
	unsigned int (*f)();
{
	register uchar_t *p, c;
	extern Dtest();

	while (p = *t++) {
		if (f == tglob)
			if (*p == '~')
				gflag |= 2;
			else if (eq(p, "{") || eq(p, "{}"))
				continue;
		while (c = *p++)
		{
#ifdef KJI
			/* these functions only care about ASCII character */
			if (NCisshift (c)) {
				p++;
				continue;
			}
			/* don't want to match any quoted character */
			if (((f == Dtest) || (f == tglob)) && (c == NLQUOTE)) {
				p += NLchrlen(p);
				continue;
			}
#endif
			(*f)(c);
		}
	}
}

scan(t, f)
	register uchar_t **t;
	int (*f)();
{
	register uchar_t *p, c;
	register uchar_t *q;

	while (p = q = *t++) {
		while (c = *p++)
			if (f==trim) {
				/* Amazing hack! do trim() here */
#ifdef KJI
				if (c != NLQUOTE)
					p--;
				PUTSTR (q,p);
#else
				if (c==NLQUOTE)
					*q++ = *p++ & TRIM;
				else
					*q++ = c;
#endif
			}
			else
				*q++ = (*f)(c);
		*q = 0;
       }
}

unsigned int
tglob(c)
	register uchar_t c;
{

	if (any(c, globchars))
		gflag |= c == '{' ? 2 : 1;
	return (c);
}

trim(c)
	uchar_t c;
{
	/* should never be called */
	bferr("Internal error - trim called\n");
}

tback(c)
	uchar_t c;
{

	if (c == '`')
		gflag = 1;
}

uchar_t *
globone(str)
	register uchar_t *str;
{
	uchar_t *gv[2];
	register uchar_t **gvp;
	register uchar_t *cp;

	gv[0] = str;
	gv[1] = 0;
	gflag = 0;
	rscan(gv, tglob);
	if (gflag) {
		gvp = glob(gv);
		if (gvp == 0) {
			setname(str);
			bferr(MSGSTR(M_NOMATCH, "No match"));
		}
		cp = *gvp++;
		if (cp == 0)
			cp = (uchar_t *)"";
		else if (*gvp) {
			setname(str);
			bferr(MSGSTR(M_AMBIG, "Ambiguous"));
		} else
			cp = strip(cp);
/*
		if (cp == 0 || *gvp) {
			setname(str);
			bferr(cp ? "Ambiguous" : "No output");
		}
*/
		xfree((char *)gargv); gargv = 0;
	} else {
		scan(gv, trim);
		cp = savestr(gv[0]);
	}
	return (cp);
}

/*
 * Command substitute cp.  If literal, then this is
 * a substitution from a << redirection, and so we should
 * not crunch blanks and tabs, separating words only at newlines.
 */
uchar_t **
dobackp(cp, literal)
	uchar_t *cp;
	bool literal;
{
	register uchar_t *lp, *rp;
	uchar_t *ep;
	uchar_t word[BUFR_SIZ];
	uchar_t *apargv[GAVSIZ + 2];

	if (pargv) {
		abort();
		blkfree(pargv);
	}
	pargv = apargv;
	pargv[0] = NOSTR;
	pargcp = pargs = word;
	pargc = 0;
	pnleft = BUFR_SIZ - 4;
	for (;;) {
		for (lp = cp; *lp != '`'; lp++) {
#ifdef KJI
			if (*lp == NLQUOTE) 
				psave (*lp++);
			if (NCisshift (*lp))
				psave (*lp++);
#endif
			if (*lp == 0) {
				if (pargcp != pargs)
					pword();
#ifdef GDEBUG
				printf("leaving dobackp\n");
#endif
				return (pargv = copyblk(pargv));
			}
			psave(*lp);
		}
		lp++;
		for (rp = lp; *rp && *rp != '`'; rp++)
		{
			if (*rp == '\\') {
				rp++;
				if (!*rp)
					goto oops;
			}
#ifdef KJI
			if (NCisshift(*rp))
				rp++;
#endif
		}
		if (!*rp)
oops:
			error(MSGSTR(M_NOBACK, "Unmatched `"));
		ep = savestr(lp);
		ep[rp - lp] = 0;
		backeval(ep, literal);
#ifdef GDEBUG
		printf("back from backeval\n");
#endif
		cp = rp + 1;
	}
}

backeval(cp, literal)
	uchar_t *cp;
	bool literal;
{
	int pvec[2];
#if defined(KJI) || defined(NLS)
	int quoted = (literal || (cp[0] == NLQUOTE)) ? QUOTE : 0;
#else
	int quoted = (literal || (cp[0] & QUOTE)) ? QUOTE : 0;
#endif
	uchar_t ibuf[BUFR_SIZ];
	register int icnt = 0, c;
	register uchar_t *ip;
	bool hadnl = 0;
	uchar_t *fakecom[2];
	struct command faket;

	faket.t_dtyp = TCOM;
	faket.t_dflg = 0;
	faket.t_dlef = 0;
	faket.t_drit = 0;
	faket.t_dspr = 0;
	faket.t_dcom = fakecom;
	fakecom[0] = (uchar_t *)"` ... `";
	fakecom[1] = 0;
	/*
	 * We do the psave job to temporarily change the current job
	 * so that the following fork is considered a separate job.
	 * This is so that when backquotes are used in a
	 * builtin function that calls glob the "current job" is not corrupted.
	 * We only need one level of pushed jobs as long as we are sure to
	 * fork here.
	 */
	psavejob();
	/*
	 * It would be nicer if we could integrate this redirection more
	 * with the routines in sh.sem.c by doing a fake execute on a builtin
	 * function that was piped out.
	 */
	mypipe(pvec);
	if (pfork(&faket, -1) == 0) {
		struct wordent tmp_paraml;
		struct command *t;
		struct sigvec nsv, osv;

		close(pvec[0]);
		dmove(pvec[1], 1);
		dmove(SHDIAG, 2);
		initdesc();
		arginp = cp;
		strip(cp);
		lex(&tmp_paraml);
		if (err)
			error(err);
		alias(&tmp_paraml);
		t = syntax(tmp_paraml.next, &tmp_paraml, 0);
		if (err)
			error(err);
		if (t)
			t->t_dflg |= FPAR;
                nsv.sv_mask = SA_RESTART;
                nsv.sv_onstack = 0;
                nsv.sv_handler = SIG_IGN;
                (void)sigvec(SIGTSTP, &nsv, &osv);
                (void)sigvec(SIGTTIN, &nsv, &osv);
                (void)sigvec(SIGTTOU, &nsv, &osv);
		execute(t, -1, (int *)0, (int *)0);
		exitstat();
	}
	xfree(cp);
	close(pvec[1]);
	do {
		int cnt = 0;
		int squote = 0;
		int dquote = 0;
		for (;;) {
			if (icnt == 0) {
				ip = ibuf;
				while ((icnt = read(pvec[0], ip, BUFR_SIZ)) < 0 && errno == EINTR);
				if (icnt <= 0) {
					c = -1;
					break;
				}
			}
			if (hadnl)
				break;
			--icnt;
			/*
			 * Pick up character from output of `command`
			 */
			c = *ip++;
			if (c == 0)
				break;
			if (c == '\n') {
				/*
				 * Continue around the loop one
				 * more time, so that we can eat
				 * the last newline without terminating
				 * this word.
				 */
				hadnl = 1;
				continue;
			}
			if (c == '\'')
				squote ^= QUOTE;
			if (c == '\"')
				dquote ^= QUOTE;
			if (!quoted && (c == ' ' || c == '\t'))
				break;
#ifdef NLS
			cnt++;
			if ( ( quoted || (!noglob && ( squote || dquote ))) && !(c&QUOTE) )
				psave(NLQUOTE);
#endif
#ifdef KJI
			/* Check for double-wide space also. */
			if (!(quoted || (!noglob && ( squote || dquote ))) && NCisshift(c)) {
				if (isjspace(_NCd2(c, *ip))) {
					++ip;
					break;
				}
			}
			cnt++;
			if (quoted || (!noglob && ( squote || dquote )) )
				psave (NLQUOTE);
			psave (c);
			if (NCisshift(c))
				--icnt, psave (*ip++);
#else
			if (!noglob)
				psave(c | quoted | squote | dquote);
			else
				psave(c | quoted);
#endif
		}
		/*
		 * Unless at end-of-file, we will form a new word
		 * here if there were character in the word, or in
		 * any case when we take text literally.  If
		 * we didn't make empty words here when literal was
		 * set then we would lose blank lines.
		 */
		if (c != -1 && (cnt || literal))
			pword();
		hadnl = 0;
	} while (c >= 0);
#ifdef GDEBUG
	printf("done in backeval, pvec: %d %d\n", pvec[0], pvec[1]);
	printf("also c = %c <%o>\n", c, c);
#endif
	close(pvec[0]);
#ifdef GDEBUG
	printf("backeval, after close(pvec[0]==%d)\n",pvec[0]);
#endif
	pwait();
#ifdef GDEBUG
	printf("backeval, after pwait()\n");
#endif
	prestjob();
#ifdef GDEBUG
	printf("backeval, after prestjob()\n");
#endif
}

psave(c)
	uchar_t c;
{

	if (--pnleft <= 0)
		error(MSGSTR(M_WORD, "Word too long"));
	*pargcp++ = c;
}

pword()
{

	psave(0);
	if (pargc == GAVSIZ)
		error(MSGSTR(M_COMSUB, "Too many words from ``"));
	pargv[pargc++] = savestr(pargs);
	pargv[pargc] = NOSTR;
#ifdef GDEBUG
	printf("got word %s\n", pargv[pargc-1]);
#endif
	pargcp = pargs;
	pnleft = BUFR_SIZ - 4;
}
