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
 * OSF/1 Release 1.0.2
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: name.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:15 $";
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

#include	<locale.h>
#include	"defs.h"
#ifdef KJI
#include	"sym.h"
#endif

extern	BOOL	chkid();
extern	int	mailchk;
extern	int	timeout;

static	int	namwalk ();
extern	uchar_t	*simple ();
extern	uchar_t	*getenv ();
static	uchar_t	*staknam ();

      void    check_nls_and_locale (struct namnod *);
static        int     call_putenv (struct namnod *);
extern        uchar_t    *getenv (char *);


struct namnod cdpnod =          /* CDPATH */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	cdpname
};
struct namnod ifsnod =          /* IFS */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	ifsname
};
struct namnod homenod =         /* HOME */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	homename
};
struct namnod mailnod =         /* MAIL */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	mailname
};
struct namnod mchknod =         /* MAILCHECK */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	mchkname
};
struct namnod mailmnod =        /* MAILMSG */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	mailmname
};
struct namnod mailpnod =        /* MAILPATH */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	mailpname
};
struct namnod pathnod =         /* PATH */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	pathname
};
struct namnod ps1nod =          /* PS1 */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	ps1name
};
struct namnod ps2nod =          /* PS2 */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	ps2name
};
struct namnod acctnod =         /* SHACCT */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	acctname
};
struct namnod shellnod =          /* SHELL */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	shellname
};
struct namnod timenod =         /* TIMEOUT */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	timename
};
struct namnod nlspathnod =         /* NLSPATH */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	nlspath
};
struct namnod langnod =         /* LANG */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	lang
};
struct namnod ctypenod =         /* LC_CTYPE */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	ctype
};
struct namnod collatenod =         /* LC_COLLATE */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	collate
};
struct namnod monetarynod =         /* LC_MONETARY */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	monetary
};
struct namnod lctimenod =         /* LC_TIME */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	lctime
};
struct namnod messagenod =         /* LC_MESSAGES */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	messages
};
struct namnod numericnod =         /* LC_NUMERIC */
{
	(struct namnod *)NIL,
	(struct namnod *)NIL,
	numeric
};

static	struct namnod	*namep =0;
#define	SHELL_SH	"SHELL=/bin/sh"

/* ========	variable and string handling	======== */

syslook(w, syswds, n)
	register uchar_t *w;
	register struct sysnod syswds[];
	int n;
{
	int	low;
	int	high;
	int	mid;
	register int cond;

	if (w == 0 || *w == 0)
		return(0);

	w = NLSndecode(w);

	low = 0;
	high = n - 1;

	while (low <= high)
	{
		mid = (low + high) / 2;

		if ((cond = cf(w, syswds[mid].sysnam)) < 0)
			high = mid - 1;
		else if (cond > 0)
			low = mid + 1;
		else
			return(syswds[mid].sysval);
	}
	return(0);
}

setlist(arg, xp)
register struct argnod *arg;
int	xp;
{
	if (flags & exportflg)
		xp |= N_EXPORT;

	while (arg)
	{
		register uchar_t *s = mactrim(arg->argval);
		setname(NLSndecode(s), xp);
		arg = arg->argnxt;
		if (flags & execpr)
		{
			prs(s);
			if (arg)
				blank();
			else
				newline();
		}
	}
}


setname(argi, xp)	/* does parameter assignments */
uchar_t	*argi;
int	xp;
{
	register uchar_t *argscan = argi;
	register struct namnod *n;

	extern uchar_t *setpos;

	chkid(argscan);
	if (argscan = setpos) { {
			*argscan = 0;   /* make name a cohesive string */
			n = lookup(argi);
			*argscan++ = '=';
			attrib(n, xp);
			if (xp & N_ENVNAM) {
				n->namenv = n->namval = argscan;
				check_nls_and_locale (n);
			}
			else
				assign(n, argscan);
			return;
		}
	}
	failed(argi, MSGSTR(M_NOTID,(char *)notid));
}

replace(a, v)
register uchar_t	**a;
uchar_t	*v;
{
	free(*a);
	*a = make(v);
}

dfault(n, v)
struct namnod *n;
uchar_t	*v;
{
	if (n->namval == 0)
		assign(n, v);
}

assign(n, v)
struct namnod *n;
uchar_t	*v;
{
	if (n->namflg & N_RDONLY)
		failed(n->namid, MSGSTR(M_WTFAILED,(char *)wtfailed));
	else if (flags & rshflg)
	{
		if (n == &pathnod || eq(n->namid, "SHELL"))
			failed(n->namid, MSGSTR(M_RESTRICTED,(char *)restricted));
	}
	else if (n->namflg & N_FUNCTN)
	{
		func_unhash(n->namid);
		freefunc(n);

		n->namenv = 0;
		n->namflg = N_DEFAULT;
	}

	if (n == &mchknod)
	{
		mailchk = stoi(v);
	}
	else if (n == &timenod)
	{
		if(*v)
			timeout = stoi(v);
		else
			timeout = 0;
	}
		
#ifdef NLSDEBUG
	debug("assign fr", n->namid);
	debug("assign to", v);
#endif
	replace(&n->namval, v);
	attrib(n, N_ENVCHG);

	if (n == &pathnod)
	{
		zaphash();
		set_dotpath();
		return;
	}

	if (flags & prompt)
	{
		if ((n == &mailpnod) || (n == &mailnod && mailpnod.namflg == N_DEFAULT))
			setmail(n->namval);
	}
}

readvar(names)
uchar_t	**names;
{
	struct fileblk	fb;
	register struct fileblk *f = &fb;
#ifdef KJI
	register uchar_t  *c;
	extern uchar_t    *nextwc();
	uchar_t		ifsch[100], *ifs = ifsch;
#else
	register uchar_t	c;
#endif
	register int	rc = 0;
	struct namnod *n = lookup(*names++);	/* done now to avoid storage mess */
	uchar_t	*rel = (uchar_t *)relstak();
	extern	uchar_t	readvarQuoted ;		/**	5A APAR 4138	**/

	push(f);
	initf(dup(0));
#ifdef NLS
	/* don't encode this data */
	f->fraw = TRUE;
#endif

	if (lseek(0, 0L, 1) == -1)
		f->fsiz = 1;

	/*
	 * strip leading IFS uchar_tacters
	 */
	readvarQuoted = 0 ;
#ifdef KJI
	if (!NLSisencoded(ifsnod.namval)) {
		NLSencode (ifsnod.namval, ifs, 100);
		NLSskiphdr (ifs);
	} else
		ifs = ifsnod.namval;
	pushstak (FNLS);
	while ((NLany((c = nextwc()), ifs)) && !(eolchar(*c)))
	{
#else
	while ((any((c = nextc(0)), ifsnod.namval)) && !(eolchar(c)))
	{
		readvarQuoted = 0 ;	/**	5A APAR 4138	**/
#endif
#ifdef NLSDEBUG
		debug ("readvar - leading IFS stripped", ifs);
#endif
	}

	for (;;)
	{
#ifdef KJI
		if ((*names && NLany(c, ifs)) || eolchar(*c))
#else
		if ((*names && any(c, ifsnod.namval)) || eolchar(c))
#endif
		{
			zerostak();
			assign(n, absstak(rel));
			setstak(rel);
			if (*names)
				n = lookup(*names++);
			else
				n = 0;
#ifdef KJI
			if (eolchar(*c))
#else
			if (eolchar(c))
#endif
			{
				break;
			}
			else		/* strip imbedded IFS uchar_tacters */
			{
#ifdef KJI
				while ((NLany((c = nextwc()), ifs)) &&
					!(eolchar(*c)))
# ifdef NLSDEBUG
					debug("readvar - embedded IFS stripped",
						ifs);
# endif
					;
				pushstak (FNLS);
#else
				while ((any((c = nextc(0)), ifsnod.namval)) &&
					!(eolchar(c)))
					;
#endif
			}
		}
		else
#ifdef KJI
		{
		int j;
		pushstak (*c);
		if (*c == (ESCAPE | QUOTE))	/* to handle quoted backslash */
		    pushstak(ESCAPE);
		j = NLSenclen (*c);
		while (--j) {
			c++;
			pushstak (*c);
		}
		c = nextwc();
		if (eolchar (*c)) 
		{
			uchar_t *ch, d, *sav;
			do {
			    ch = stakbot;
			    while (sav = ch, d = *ch & STRIP)
				if ((ch += NLSenclen (d)) >= staktop)
				    break;
			    /* d will be ASCII uchar_tacter or magic font shift */
			    if (!NLany (sav, ifs))
				    break;
#ifdef NLSDEBUG
			    debug("readvar - trailing IFS stripped", ifs);
#endif
			} while ((staktop -= NLSenclen (d)) > stakbot);
		}
		}
#else
		{
				/*	5A APAR 4138
				*	The following if statement was
				*	added to strip the QUOTE from
				*	the quotes uchar_tacter when
				*	storing a decoded string.
				*/
			if ( readvarQuoted )
			{
				pushstak (( c & (char) (~QUOTE))) ;
				readvarQuoted = 0 ;
			}
			else
				pushstak(c);

			c = nextc(0);

			if (eolchar(c))
			{
				uchar_t *top = staktop;
			
				while (any(*(--top), ifsnod.namval))
					;
				staktop = top + 1;
			}
		}
#endif
	}
	while (n)
	{
		assign(n, nullstr);
		if (*names)
			n = lookup(*names++);
		else
			n = 0;
	}

	if (eof)
		rc = 1;
	lseek(0, (long)(f->fnxt - f->fend), 1);
	pop();
	return(rc);
}

assnum(p, i)
uchar_t	**p;
int	i;
{
	itos(i);
	replace(p, numbuf);
}

uchar_t *
make(v)
uchar_t	*v;
{
	register uchar_t	*p;

	if (v)
	{
		movstr(v, p = alloc(length(v)));
		return(p);
	}
	else
		return(0);
}


struct namnod *
lookup(nam)
	register uchar_t	*nam;
{
	register struct namnod *nscan = namep;
	register struct namnod **prev;
	int		LR;
#ifdef NLSDEBUG
	debug("lookup",nam);
#endif
	if (!chkid(nam))
		failed(nam, MSGSTR(M_NOTID,(char *)notid));
	while (nscan)
	{
		if ((LR = cf(nam, nscan->namid)) == 0)
			return(nscan);

		else if (LR < 0)
			prev = &(nscan->namlft);
		else
			prev = &(nscan->namrgt);
		nscan = *prev;
	}
	/*
	 * add name node
	 */
	nscan = (struct namnod *)alloc(sizeof *nscan);
	nscan->namlft = nscan->namrgt = (struct namnod *)NIL;
	nscan->namid = make(nam);
	nscan->namval = 0;
	nscan->namflg = N_DEFAULT;
	nscan->namenv = 0;
	return(*prev = nscan);
}

/* cheap side-effect trick for the moment */
uchar_t *setpos;

uchar_t *
scanset(s)
{
	chkid(NLSndecode(s));
	return setpos;
}

BOOL
chkid(nam)
uchar_t    *nam;
{
	register uchar_t *cp = nam;
# ifdef KJI
	wchar_t nlc;
# endif
	uchar_t c;
	setpos = 0;

		/* as a side-effect of chkid(), setpos is non-zero if nam */
		/* appears to be a valid envt. assignment; setpos points  */
		/* to the = in nam.                                       */

# ifdef NLSDEBUG
#  ifdef NLS
	if (NLSisencoded(cp))
	    cp = NLSskiphdr(cp);
#  else /* KJI */
	/*  Arguments to chkid() should always be decoded. */
	debug ("chkid", cp);
#  endif
# endif

# ifdef KJI
	cp += NCdec (cp, &nlc);
	if (!NLSletter(nlc))
		return(FALSE);
	while (cp += NCdec (cp, &nlc), nlc) 
	{
		if (nlc == '=') 
		{
			setpos = cp -1;
			return (FALSE);
		}
		if (!NLSalphanum (nlc))
			return (FALSE);
	}
# else /* NLS */
	if (digit(*cp)) return(FALSE);
	while (c = *cp++)
	{
		if (c == '=') {
			setpos = cp-1;
			return(FALSE);
		}
		/* adjusted to accept all NLS uchar_ts as alphanumerics */
		if (NLSfontshift(c))
			cp++;            /* next uchar_t effectively alpha */
		else if (c<128 && !alphanum(c))
			return(FALSE);
	}
# endif
	return(TRUE);
}

static int (*namfn)();
namscan(fn)
	int	(*fn)();
{
	namfn = fn;
	namwalk(namep);
}

static int
namwalk(np)
register struct namnod *np;
{
	if (np)
	{
		namwalk(np->namlft);
		(*namfn)(np);
		namwalk(np->namrgt);
	}
}

printnam(n)
struct namnod *n;
{
	register uchar_t	*s;

	sigchk();

	if (n->namflg & N_FUNCTN)
	{
		prs_buff(n->namid);
		prs_buff("(){\n");
		prf(n->namenv);
		prs_buff("\n}\n");
	}
	else if (s = n->namval)
	{
		prs_buff(n->namid);
		prc_buff('=');
		prs_buff(s);
		prc_buff(NL);
	}
}

static uchar_t *
staknam(n)
register struct namnod *n;
{
	register uchar_t	*p;

	needmem (staktop + length (n->namid) + length (n->namval));
	p = movstr(n->namid, staktop);
	p = movstr("=", p);
	p = movstr(n->namval, p);
	return(getstak(p + 1 - (uchar_t *)(stakbot)));
}

static int namec;

exname(n)
	register struct namnod *n;
{
	register int 	flg = n->namflg;

	if (flg & N_ENVCHG)
	{

		if (flg & N_EXPORT)
		{
			free(n->namenv);
			n->namenv = make(n->namval);
		}
		else
		{
			free(n->namval);
			n->namval = make(n->namenv);
		}
	}

	
	if (!(flg & N_FUNCTN))
		n->namflg = N_DEFAULT;

	if (n->namval)
		namec++;

}

printro(n)
register struct namnod *n;
{
	if (n->namflg & N_RDONLY)
	{
		prs_buff(readonly);
		prc_buff(SP);
		prs_buff(n->namid);
		prc_buff(NL);
	}
}

printexp(n)
register struct namnod *n;
{
	if (n->namflg & N_EXPORT)
	{
		prs_buff(export);
		prc_buff(SP);
		prs_buff(n->namid);
		prc_buff(NL);
	}
}

setup_env()
{
	register uchar_t **e = environ;

	/* sort builtin envt. names according to strcmp order */
	addNode(&mailnod);         /* MAIL */
	addNode(&ifsnod);          /* IFS */
	addNode(&homenod);         /* HOME */
	addNode(&cdpnod);          /* CDPATH */
	addNode(&collatenod);      /* LC_COLLATE */
	addNode(&langnod);         /* LANG */
	addNode(&monetarynod);     /* LC_MONETARY */
	addNode(&ctypenod);        /* LC_CTYPE */
	addNode(&lctimenod);       /* LC_TIME */
	addNode(&messagenod);      /* LC_MESSAGES */
	addNode(&numericnod);      /* LC_NUMERIC */
	addNode(&acctnod);         /* SHACCT */
	addNode(&pathnod);         /* PATH */
	addNode(&mailmnod);        /* MAILMSG */
	addNode(&mchknod);         /* MAILCHECK */
	addNode(&mailpnod);        /* MAILPATH */
	addNode(&nlspathnod);      /* NLSPATH */
	addNode(&ps1nod);          /* PS1 */
	addNode(&ps2nod);          /* PS2 */
	addNode(&shellnod);        /* SHELL */
	addNode(&timenod);         /* TIMEOUT */
	while (*e)
		setname(*e++, N_ENVNAM);

}

addNode(n)
register struct namnod *n;
{
	register struct namnod **np = &namep;
	n->namlft = n->namrgt = 0;
	while(*np)
	{
		if (strcmp(n->namid, (*np)->namid) < 0)
		       np = &(*np)->namlft;
		else
		       np = &(*np)->namrgt;
	}
	*np = n;
}


static uchar_t **argnam;

pushnam(n)
struct namnod *n;
{
	if (n->namval)
		*argnam++ = staknam(n);
}

uchar_t **
setenv()
{
	register uchar_t	**er;

	namec = 0;
	namscan(exname);

	argnam = er = (uchar_t **)getstak(namec * BYTESPERWORD + BYTESPERWORD);
	namscan(pushnam);
	*argnam++ = 0;
	return(er);
}

struct namnod *
findnam(nam)
	register uchar_t	*nam;
{
	register struct namnod *nscan = namep;
	int             LR;

	if (strcmp(nam,"SHELL")!=0)        /* special case main() */
	       nam = NLSndecode(nam);

	if (!chkid(nam))
		return(0);
	while (nscan)
	{
		if ((LR = cf(nam, nscan->namid)) == 0)
			return(nscan);
		else if (LR < 0)
			nscan = nscan->namlft;
		else
			nscan = nscan->namrgt;
	}
	return(0); 
}


unset_name(name)
	register uchar_t 	*name;
{
	register struct namnod	*n;
	uchar_t			empty_string = 0;

	if (n = findnam(name))
	{
		if (n->namflg & N_RDONLY)
			failed(name, MSGSTR(M_WTFAILED,(char *)wtfailed));

		if (n == &pathnod ||
		    n == &ifsnod ||
		    n == &ps1nod ||
		    n == &ps2nod ||
		    n == &mchknod)
		{
			failed(name, MSGSTR(M_BADUNSET,(char *)badunset));
		}
		if ((flags & rshflg) && eq(name, "SHELL"))
			failed(name, MSGSTR(M_RESTRICTED,(char *)restricted));
		if (n->namflg & N_FUNCTN)
		{
			func_unhash(name);
			freefunc(n);
		}
		else
		{
			free(n->namval);
			free(n->namenv);
		}

		n->namval = n->namenv = &empty_string;
		n->namflg = N_DEFAULT;

		check_nls_and_locale (n);
		n->namval = n->namenv = 0;

		if (flags & prompt)
		{
			if (n == &mailpnod)
				setmail(mailnod.namval);
			else if (n == &mailnod && mailpnod.namflg == N_DEFAULT)
				setmail(0);
			else if (n == &timenod)
				timeout = 0;
		}
	}
}


void check_nls_and_locale (struct namnod *n)
{
	int catalog;

	if ( n == &nlspathnod || n == &langnod ) {
                if ( call_putenv (n))
                        return ;
                setlocale (LC_ALL, "");
                catclose (catd);
                catd = NLcatopen (MF_SH, 0);
		return ;
	}
	else if ( n == &ctypenod )
		catalog = LC_CTYPE;
	else if ( n == &collatenod )
		catalog = LC_COLLATE;
	else if ( n == &lctimenod )
		catalog = LC_TIME;
	else if ( n == &messagenod )
		catalog = LC_MESSAGES;
	else if ( n == &monetarynod )
		catalog = LC_MONETARY;
	else if ( n == &numericnod )
		catalog = LC_NUMERIC;
	else
		return;

		/*	LC_* values go to here */
	if (call_putenv(n))
		return;
	setlocale(catalog, (char *)n->namval);
}


static int call_putenv(struct namnod *n)
{
int FSHMSG = -1;

	uchar_t	*env, *oldenv;
	int	env_off;

                        /*      compare old name to new name    */
        oldenv = getenv ( (char *)n->namid );
        if ((!(n->namval) && (oldenv == NULL)) ||
            (oldenv && n->namval && !strcmp (oldenv, n->namval)) )
                return (TRUE);
	env_off = (int) strlen (n->namid);
	if(n->namval)
		env = (uchar_t *)malloc (env_off + (int) strlen(n->namval) + 2);
	else
		env = (uchar_t *)malloc (env_off + 2);
	strcpy(env, n->namid);
	env[env_off] = '=';
	env[env_off+1] = (char) 0;
	if(n->namval)
		strcat (env, n->namval);
	putenv (env);
        return (FALSE);
}
