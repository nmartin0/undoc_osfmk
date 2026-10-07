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
static char rcsid[] = "@(#)$RCSfile: dol.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:25 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: Dfix Dfix1 Dfix2 Dword DgetC Dgetdol setDolp unDredc Dredc 
 *            Dtest Dtestq heredoc
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

#ifdef KJI
#include <NLctype.h>
#endif

#include "sh.h"

/*
 * C shell
 */

/*
 * These routines perform variable substitution and quoting via ' and ".
 * To this point these constructs have been preserved in the divided
 * input words.  Here we expand variables and turn quoting via ' and " into
 * QUOTE bits on character (which prevent further interpretation).
 * If the `:q' modifier was applied during history expansion, then
 * some QUOTEing may have occurred already, so we dont "scan(,&trim)" here.
 */
/* Under NLS, quoted character not only have their QUOTE bit on but are
 * preceded by the character NLQUOTE.  The QUOTE protects further
 * interpretation, but the NLQUOTE identifies the character as an
 * originally ASCII character rather than an extended character.
 */

#ifdef KJI
/*
 *	Under KJI, all quoted character are preceded by an NLQUOTE.
 */
#endif

int	Dpeekc, Dpeekrd;		/* Peeks for DgetC and Dreadc */
uchar_t	*Dcp, **Dvp;			/* Input vector for Dreadc */

#define	DEOF	-1

#define	unDgetC(c)	Dpeekc = c

char	*QUOTES = "\\'`\"";

/*
 * The following variables give the information about the current
 * $ expansion, recording the current word position, the remaining
 * words within this expansion, the count of remaining words, and the
 * information about any : modifier which is being applied.
 */
uchar_t	*dolp;			/* Remaining uchar_ts from this word */
uchar_t	**dolnxt;		/* Further words */
int	dolcnt;			/* Count of further words */
uchar_t	dolmod;			/* : modifier character */
int	dolmcnt;		/* :gx -> 10000, else 1 */

int	Dtest();		/* Test for \ " ` or ' */

/*
 * Fix up the $ expansions and quotations in the
 * argument list to command t.
 */
Dfix(t)
	register struct command *t;
{

	if (noexec)
		return;
	gflag = 0, rscan(t->t_dcom, Dtest);
	if (gflag == 0)
		return;
	Dfix2(t->t_dcom);
	blkfree(t->t_dcom), t->t_dcom = gargv, gargv = 0;
}

/*
 * $ substitute one word, for i/o redirection
 */
uchar_t *
Dfix1(cp)
	register uchar_t *cp;
{
	uchar_t *Dv[2];

	if (noexec)
		return (0);
	Dv[0] = cp; Dv[1] = NOSTR;
	Dfix2(Dv);
	if (gargc != 1) {
		setname(cp);
		bferr(MSGSTR(M_AMBIG, "Ambiguous"));
	}
	cp = savestr(gargv[0]);
	blkfree(gargv), gargv = 0;
	return (cp);
}

/*
 * Subroutine to do actual fixing after state initialization.
 */
Dfix2(v)
	uchar_t **v;
{
	uchar_t *agargv[GAVSIZ];

	ginit(agargv);			/* Initialize glob's area pointers */
	Dvp = v; Dcp = (uchar_t *)"";		/* Setup input vector for Dreadc */
	unDgetC(0); unDredc(0);		/* Clear out any old peeks (at error) */
	dolp = 0; dolcnt = 0;		/* Clear out residual $ expands (...) */
	while (Dword())
		continue;
	gargv = copyblk(gargv);
}

/*
 * Get a word.  This routine is analogous to the routine
 * word() in sh.lex.c for the main lexical input.  One difference
 * here is that we don't get a newline to terminate our expansion.
 * Rather, DgetC will return a DEOF when we hit the end-of-input.
 */
Dword()
{
	register int c, c1;
	uchar_t wbuf[BUFR_SIZ];
	register uchar_t *wp = wbuf;
	register int i = BUFR_SIZ - 4;
	register bool dolflg;
	bool sofar = 0;

loop:
	c = DgetC(DODOL);
	switch (c) {

	case DEOF:
deof:
		if (sofar == 0)
			return (0);
		/* finish this word and catch the code above the next time */
		unDredc(c);
		/* fall into ... */

	case '\n':
		*wp = 0;
		goto ret;

	case ' ':
	case '\t':
		goto loop;

	case '`':
		/* We preserve ` quotations which are done yet later */
		*wp++ = c, --i;
	case '\'':
	case '"':
		/*
		 * Note that DgetC never returns a QUOTES character
		 * from an expansion, so only true input quotes will
		 * get us here or out.
		 */
		c1 = c;
		dolflg = c1 == '"' ? DODOL : 0;
		for (;;) {
			c = DgetC(dolflg);
			if (c == c1)
				break;
			if (c == '\n' || c == DEOF)  {
				char e[NL_TEXTMAX];

				sprintf(e,MSGSTR(M_NOQUOTE,"Unmatched %c"),c1);
				error(e);
			}
#ifdef KJI
			if (c == NLQUOTE) {
				c = DgetC(0);
				if (c != '\n')
				    *wp++ = NLQUOTE, i--;
				if ((c == HIST) && (c1 == '`'))
				    *wp++ = '\\', i--;
				if ((i -= (c < 0xff) ? 1 : 2) <= 0)
					goto toochars;
				PUTCH (wp,c);
				continue;
			}
			if ((--i - 2) <= 0)
#else
			if ((c & (QUOTE|TRIM)) == ('\n' | QUOTE) &&
				wp[-1] == NLQUOTE )
				/*
				 *  Let Q be NLQUOTE.
				 *  Quoted \n was preceded by \ Q in input
				 *  Since a Q was appended to \ in processing,
				 *  wbuf is now ... Q \ Q
				 *  This statement strips Q \ off end of wbuf:
				 */
				wp-=2, i+=2;
			if (--i <= 0)
#endif
				goto toochars;
			switch (c1) {

			case '"':
				/*
				 * Leave any `s alone for later.
				 * Other uchar_ts are all quoted, thus `...`
				 * can tell it was within "...".
				 */
#ifdef KJI	
				if ((c != '`') && (c != HIST))
					*wp++ = NLQUOTE, --i;
				PUTCH (wp,c);
#else
				if (c != '`' && (c&QUOTE)==0 && c!=NLQUOTE ) {
					*wp++ = NLQUOTE;
					*wp++ = c | QUOTE;
				}
				else
					*wp++ = c;
#endif
				break;

			case '\'':
				/* Prevent all further interpretation */
#ifdef KJI
				*wp++ = NLQUOTE, --i;
				PUTCH (wp,c);
#else
				if ((c&QUOTE) == 0 && c!=NLQUOTE) {
					*wp++ = NLQUOTE;
					*wp++ = c | QUOTE;
				}
				else
					*wp++ = c;
#endif
				break;

			case '`':
				/* Leave all text alone for later */
#ifdef KJI
				PUTCH (wp,c);
#else
				*wp++ = c;
#endif
				break;
			}
#ifdef KJI
			if (c > 0xff) --i;
#endif
		}
		if (c1 == '`')
			*wp++ = '`', --i;
		goto pack;		/* continue the word */

	case '\\':
		c = DgetC(0);		/* No $ subst! */
		if (c == '\n' || c == DEOF)
			goto loop;
#ifdef KJI
		if ((i -= (c < 0xff) ? 2 : 3) <= 0)
			goto toochars;
		*wp++ = NLQUOTE;
		PUTCH (wp,c);
		goto pack;

	case NLQUOTE:
		c = DgetC(0);		/* No $ subst! */
		if (c == '\n' || c == DEOF)
			goto loop;
		if ((i -= (c < 0xff) ? 1 : 2) <= 0)
			goto toochars;
		PUTCH (wp,c);
		goto pack;
#else
		if ((c&QUOTE)==0)
			c |= (NLQUOTE<<8) | QUOTE;
		break;
#endif
	}
		/* following three lines were added/changed for APAR 2683 */
	if (c !=  NLQUOTE)
		unDgetC(c);
	else *wp++ = c;
pack:
	sofar = 1;
	/* pack up more character in this word */
	for (;;) {
		c = DgetC(DODOL);
#ifdef KJI
		if ((c == NLQUOTE) || (c == '\\')) {
#else
		if (c == '\\') {
#endif
			c = DgetC(0);
			if (c == DEOF)
				goto deof;
			if (c == '\n')
				c = ' ';
			else
#ifdef KJI
				{
				*wp++ = NLQUOTE, --i;
				PUTCH(wp,c);
				continue;
				}
#else
				if ((c&QUOTE) == 0)
					c |= (NLQUOTE<<8) | QUOTE;
#endif
		}
		if (c == DEOF)
			goto deof;
#ifdef KJI
		if (any(c, " '`\"\t\n") || isjspace(c)) {
#else
		if (any(c, " '`\"\t\n")) {
#endif
			unDgetC(c);
			if (any(c, QUOTES))
				goto loop;
			*wp++ = 0;
			goto ret;
		}
#ifdef KJI
		if ((--i <= 0) || ((c > 0xff) && (--i <= 0)))
			goto toochars;
		PUTCH (wp,c);
#else
		if (--i <= 0)
toochars:
			error("Word too long");
		if ( c>>8 )
			*wp++ = c>>8;
		*wp++ = c;
#endif
	}
ret:
	Gcat("", wbuf);
	return (1);
#ifdef KJI
toochars:
	error(MSGSTR(M_WORD, "Word too long"));
#endif
}

#ifdef KJI
/*
 * Get a 1- or 2-byte character and return as an integer.
 */
DgetC(flag)
	register int flag;
{
	register int c,d;
	c = kji_DgetC(flag);
	if ((c < 0xff) && NCisshift (c))
		d = kji_DgetC(flag), c = _NCd2(c, d);
	return (c);
}
#endif

/*
 * Get a character, performing $ substitution unless flag is 0.
 * Any QUOTES character which is returned from a $ expansion is
 * QUOTEd so that it will not be recognized above.
 */
#ifdef KJI
static int shiftseen = 0;
static int quoted = 0;
kji_DgetC(flag)
#else
DgetC(flag)
#endif
	register int flag;
{
	register int c;

top:
	if (c = Dpeekc) {
#ifndef KJI
		if ((c >> 8) == NLQUOTE) {
		    c = NLQUOTE;
		    Dpeekc &= 0377;
		}
		else
#endif
		Dpeekc = 0;
		return (c);
	}
	if (lap) {
#ifdef KJI
		c = *lap++;
#else
		c = *lap++ & (QUOTE|TRIM);
#endif
		if (c == 0) {
			lap = 0;
			goto top;
		}
quotspec:
#ifdef KJI
		if (!shiftseen) {
			if (NCisshift(c))
				shiftseen++;
		} else {
			shiftseen = 0;
			return (c);
		}
		if (quoted) {
			if (!shiftseen) 
				quoted = 0;
		} else if (any(c, QUOTES)) {
			Dpeekc = c;
			return (NLQUOTE);
		} else if (c == NLQUOTE)  {
			quoted++;
#else
		if (any(c, QUOTES))
		{
			Dpeekc = c | QUOTE;
			return (NLQUOTE);
#endif
		}
		return (c);
	}
	if (dolp) {
		if (c = *dolp++ & (QUOTE|TRIM))
			goto quotspec;
		if (dolcnt > 0) {
			setDolp(*dolnxt++);
			--dolcnt;
			return (' ');
		}
		dolp = 0;
	}
	if (dolcnt > 0) {
		setDolp(*dolnxt++);
		--dolcnt;
		goto top;
	}
	c = Dredc();
	if (c == '$' && flag) {
		Dgetdol();
		goto top;
	}
	return (c);
}

uchar_t	*nulvec[] = { 0 };
struct	varent nulargv = { nulvec, (uchar_t *)"argv", 0 };
/* The following undef is to prevent a conflict with a macro in sys/param.h */
#undef isset

/*
 * Handle the multitudinous $ expansion forms.
 * Ugh.
 */
Dgetdol()
{
	register uchar_t *np;
	register struct varent *vp;
#ifdef KJI
	uchar_t name[256];
#else
	uchar_t name[128];
#endif
	int c, sc;
	int subscr = 0, lwb = 1, upb = 0;
	bool dimen = 0, isset = 0;
	uchar_t wbuf[BUFR_SIZ];

        uchar_t **vec_tmp;
		/*  variable to avoid requote quoted argv. */
		/*  Clumsy but work! */
	static  int	flag ;
	static  uchar_t	*ex_vec_tmp = 0;

	dolmod = dolmcnt = 0;
	c = sc = DgetC(0);
	if (c == '{')
		c = DgetC(0);		/* sc is { to take } later */
#ifdef KJI
	if (c == NLQUOTE) {
		c = DgetC(0);
		if (c != '#') { 
			unDgetC(c);
			c = NLQUOTE;
		}
	}
	if (c == '#')
#else
	if (c==NLQUOTE)
		/* if !isatty, # gets quoted by getC() in lex.c */
		/* this statement undoes the quote 'just enough' */
		c = DgetC(0);
	if ((c & TRIM) == '#')
#endif
		dimen++, c = DgetC(0);		/* $# takes dimension */
	else if (c == '?')
		isset++, c = DgetC(0);		/* $? tests existence */
	switch (c) {
	
	case '$':
		if (dimen || isset)
			goto syntax;		/* No $?$, $#$ */
		setDolp(doldol);
		goto eatbrac;

      /* To know this is quoted need to be able to check previous uchar_t */
#ifdef KJI
	case NLQUOTE:
		c = DgetC(0);
		if (c != '<')
			goto syntax;
#else
	case '<'|QUOTE:
#endif
		if (dimen || isset)
			goto syntax;		/* No $?<, $#< */
		for (np = wbuf; read(OLDSTD, np, 1) == 1; np++) {
			if (np >= &wbuf[BUFR_SIZ-1])
				error(MSGSTR(M_TOOLONG,"$< line too long"));
			if (*np <= 0 || *np == '\n')
				break;
		}
		*np = 0;

                /* For input via $< the entire typed-in line is assigned as a
                   single "word", with no further interpretation at all.  Thus
                   we can have:
                      % set x = $<
                      a b $c " \t q     <---- typed in
                      % echo $x
                      a b $c " \t q
                      % echo $x[1]
                      a b $c " \t q
                      % set y = ($x)
                      % echo $y
                      a b $c " \t q
                      % echo $y[1]
                      a
                      % echo $y[3]
                      $c
                      % echo $y[4]
                      "
                */
		/*
		 * KLUDGE: dolmod is set here because it will
		 * cause setDolp to call domod and thus to copy wbuf.
		 * Otherwise setDolp would use it directly. If we saved
		 * it ourselves, no one would know when to free it.
                 * The actual function of the 'q' causes ALL uchar_ts to
                 * be quoted, no expansion to be done, and the entire
                 * input line to be treated as one word.
		 */
		dolmod = 'q';
		dolmcnt = 10000;
		setDolp(wbuf);
		goto eatbrac;

	case DEOF:
	case '\n':
		goto syntax;

	case '*':
		strcpy(name, "argv");
		vp = adrof("argv");

                  /* note that argv has not been quoted at 1st time */
                vec_tmp = vp->vec;

		  /* check if argv has been quoted */
		if (ex_vec_tmp != *vec_tmp)
		{
		  flag = 0;
		  while (*vec_tmp)
                    {
			*vec_tmp = domod (*vec_tmp, 'q');
			if (flag == 0) {
			   ex_vec_tmp = *vec_tmp;
			   flag = 1;
		        }
                        vec_tmp++;
		    }
                }
                
		subscr = -1;			/* Prevent eating [...] */
		break;

	default:
		np = name;
		if (digit(c)) {
			if (dimen)
				goto syntax;	/* No $#1, e.g. */
			subscr = 0;
			do {
				subscr = subscr * 10 + c - '0';
				c = DgetC(0);
			} while (digit(c));
			unDredc(c);
			if (subscr < 0)
				goto oob;
			if (subscr == 0) {
				if (isset) {
					dolp = file ? (uchar_t *)"1" : (uchar_t *)"0";
					goto eatbrac;
				}
				if (file == 0)
					error(MSGSTR(M_NOFILE,"No file for $0"));
				setDolp(file);
				goto eatbrac;
			}
			if (isset)
				goto syntax;
			vp = adrof("argv");
			if (vp == 0) {
				vp = &nulargv;
				goto eatmod;
			}
			break;
		}
		if (!alnum(c))
			goto syntax;
		for (;;) {
#ifdef KJI
			PUTCH (np,c);
#else
			*np++ = c;
#endif
			c = DgetC(0);
			if (!alnum(c))
				break;
#ifdef KJI
			if (np >= &name[sizeof name - (c > 0xff ? 3 : 2)])
				goto syntax;
#else
			if (np >= &name[sizeof name - 2])
syntax:
				error(MSGSTR(M_VARSYN, "Variable syntax"));
#endif
		}
		*np++ = 0;
		unDredc(c);
		vp = adrof(name);
	}
	if (isset) {
		dolp = (vp || getenv(name)) ? (uchar_t *)"1" : (uchar_t *)"0";
		goto eatbrac;
	}
	if (vp == 0) {
		np = getenv(name);
		if (np) {
			addla(np);
			goto eatbrac;
		}
		udvar(name);
		/*NOTREACHED*/
	}
	c = DgetC(0);
	upb = blklen(vp->vec);
	if (dimen == 0 && subscr == 0 && c == '[') {
		np = name;
		for (;;) {
			c = DgetC(DODOL);	/* Allow $ expand within [ ] */
			if (c == ']')
				break;
			if (c == '\n' || c == DEOF)
				goto syntax;
#ifdef KJI
			if (np >= &name[sizeof name - (c > 0xff ? 3 : 2)])
				goto syntax;
			PUTCH (np, c);
#else
			if (np >= &name[sizeof name - 2])
				goto syntax;
			*np++ = c;
#endif
		}
		*np = 0, np = name;
		if (dolp || dolcnt)		/* $ exp must end before ] */
			goto syntax;
		if (!*np)
			goto syntax;
		if (digit(*np)) {
			register int i = 0;

			while (digit(*np))
				i = i * 10 + *np++ - '0';
			if ((i < 0 || i > upb) && !any(*np, "-*")) {
#ifdef KJI
				goto oob;
#else
oob:
				setname(vp->name);
				error(MSGSTR(M_SUBOUT,"Subscript out of range"));
#endif
			}
			lwb = i;
			if (!*np)
				upb = lwb, np = (uchar_t *)"*";
		}
		if (*np == '*')
			np++;
		else if (*np != '-')
			goto syntax;
		else {
			register int i = upb;

			np++;
			if (digit(*np)) {
				i = 0;
				while (digit(*np))
					i = i * 10 + *np++ - '0';
				if (i < 0 || i > upb)
					goto oob;
			}
			if (i < lwb)
				upb = lwb - 1;
			else
				upb = i;
		}
		if (lwb == 0) {
			if (upb != 0)
				goto oob;
			upb = -1;
		}
		if (*np)
			goto syntax;
	} else {
		if (subscr > 0)
			if (subscr > upb)
				lwb = 1, upb = 0;
			else
				lwb = upb = subscr;
		unDredc(c);
	}
	if (dimen) {
		uchar_t *cp = putn(upb - lwb + 1);

		addla(cp);
		xfree(cp);
	} else {
eatmod:
		c = DgetC(0);
		if (c == ':') {
			c = DgetC(0), dolmcnt = 1;
			if (c == 'g')
				c = DgetC(0), dolmcnt = 10000;
			if (!any(c, "htrqxe"))
				error(MSGSTR(M_DOLMOD, "Bad : mod in $"));
			dolmod = c;
			if (c == 'q')
				dolmcnt = 10000;
		} else
			unDredc(c);
		dolnxt = &vp->vec[lwb - 1];
		dolcnt = upb - lwb + 1;
	}
eatbrac:
	if (sc == '{') {
		c = Dredc();
		if (c != '}')
			goto syntax;
	}
#ifdef KJI
	return;
oob:
	setname(vp->name);
	error(MSGSTR(M_SUBOUT,"Subscript out of range"));
syntax:
	error(MSGSTR(M_VARSYN, "Variable syntax"));
#endif
}

setDolp(cp)
	register uchar_t *cp;
{
	register uchar_t *dp;

	if (dolmod == 0 || dolmcnt == 0) {
		dolp = cp;
		return;
	}
	dp = domod(cp, dolmod);
	if (dp) {
		dolmcnt--;
		addla(dp);
		xfree(dp);
	} else
		addla(cp);
	dolp = (uchar_t *)"";
}

unDredc(c)
	int c;
{

	Dpeekrd = c;
}

#ifdef KJI
Dredc()
{
	register int c,d;
	c = kji_Dredc();
	if ((c < 0xff) && NCisshift (c))
		d = kji_Dredc(), c = _NCd2(c, d);
	return (c);
}

kji_Dredc()
#else
Dredc()
#endif
{
	register int c;

	if (c = Dpeekrd) {
		Dpeekrd = 0;
		return (c);
	}
	if (Dcp && (c = *Dcp++))
		return (c&(QUOTE|TRIM));
	if (*Dvp == 0) {
		Dcp = 0;
		return (DEOF);
	}
	Dcp = *Dvp++;
	return (' ');
}

Dtest(c)
	register int c;
{

	/* Note that c isn't trimmed thus !...:q's aren't lost */
	if (any(c, "$\\'`\""))
		gflag = 1;
}

Dtestq(c)
	register int c;
{

#ifdef KJI
	if ( any (c, "\\'`\"")  || (c == NLQUOTE))
#else
	if ( (c & QUOTE) || any (c, "\\'`\"") )
#endif
		gflag = 1;
}

/*
 * Form a shell temporary file (in unit 0) from the words
 * of the shell input up to a line the same as "term".
 * Unit 0 should have been closed before this call.
 */
heredoc(term)
	uchar_t *term;
{
	register int c;
	uchar_t *Dv[2];
	uchar_t obuf[BUFR_SIZ], lbuf[BUFR_SIZ], mbuf[BUFR_SIZ];
	int ocnt, lcnt, mcnt;
	register uchar_t *lbp, *obp, *mbp;
	uchar_t **vp;
	bool quoted;
#ifndef KJI
	bool NLquoted;
#endif

	if (creat(shtemp, 0600) < 0)
		Perror(shtemp);
	close(0);
	if (open(shtemp, 2) < 0) {
		int oerrno = errno;

		unlink(shtemp);
		errno = oerrno;
		Perror(shtemp);
	}
	unlink(shtemp);			/* 0 0 inode! */

	Dv[0] = term;
        Dv[1] = NOSTR;
        gflag = 0;

#ifdef KJI
	/*
	 * Should not "scan" here since will need to match "term" exactly 
	 * with input line.
	 */
#else
	scan(Dv, trim);
#endif
        rscan(Dv, Dtestq);
        quoted = gflag;

	ocnt = BUFR_SIZ; obp = obuf;
	for (;;) {
		/*
		 * Read up a line
		 */
		lbp = lbuf; lcnt = BUFR_SIZ - 4;
		for (;;) {
			c = readc(1);		/* 1 -> Want EOF returns */
#ifdef KJI
			if (c == '\\') {
				c = readc(1);
				*lbp++ = (c == '\\') ? NLQUOTE : '\\';
				if (--lcnt < 0) {
					setname("<<");
					error(MSGSTR(M_OVER, "Line overflow"));
				} 
			}
#else
			if (NLquoted = (c == NLQUOTE))
				c = readc(1);
#endif
			if (c < 0) {
				setname(term);
				bferr(MSGSTR(M_TERM,"<< terminator not found"));
			}
			if (c == '\n')
				break;
#ifndef KJI
			if (NLquoted)
				c &= TRIM;
#endif
			if (c) {
#ifdef KJI
				PUTCH(lbp,c);
				if ((--lcnt == 0) || ((c > 0xff) && (--lcnt == 0))) {
#else
				*lbp++ = c;
				if (--lcnt < 0) {
#endif
					setname((uchar_t *)"<<");
					error(MSGSTR(M_OVER, "Line overflow"));
				} 
			}
		}
		*lbp = 0;

		/*
		 * Compare to terminator -- before expansion
		 */
		if (eq(lbuf, term)) {
			write(0, obuf, BUFR_SIZ - ocnt);
			lseek(0, 0l, 0);
			return;
		}

		/*
		 * If term was quoted or -n just pass it on
		 */
		if (quoted || noexec) {
			*lbp++ = '\n'; *lbp = 0;
			for (lbp = lbuf; c = *lbp++;) {
				*obp++ = c;
				if (--ocnt == 0) {
					write(0, obuf, BUFR_SIZ);
					obp = obuf; ocnt = BUFR_SIZ;
				}
			}
			continue;
		}

		/*
		 * Term wasn't quoted so variable and then command
		 * expand the input line
		 */
		Dcp = lbuf; Dvp = Dv + 1; mbp = mbuf; mcnt = BUFR_SIZ - 4;
		for (;;) {
			c = DgetC(DODOL);
#ifndef KJI
			if (NLquoted = (c == NLQUOTE))
				c = DgetC(DODOL);
#endif
			if (c == DEOF)
				break;
#ifndef KJI
			if (NLquoted)
				c &= TRIM;
#endif
			if (c == 0)
				continue;
			/* \ quotes \ $ ` here */
#ifdef KJI	
			if ((c == NLQUOTE) || (c == '\\')) {
				c = DgetC(0);
				*mbp++ = NLQUOTE;
				if (--mcnt == 0) {
					setname("<<");
					bferr(MSGSTR(M_OVER, "Line overflow"));
				}
				if (!any(c, "$\\`"))  {
					*mbp++ = '\\';
					if (--mcnt == 0) {
						setname("<<");
						bferr(MSGSTR(M_OVER, "Line overflow"));
					}
				}
			}
			PUTCH (mbp, c);
			if ((--mcnt == 0) || ((c > 0xff) && (--mcnt == 0))) {
				setname("<<");
				bferr(MSGSTR(M_OVER, "Line overflow"));
			}
#else
			if (c =='\\') {
				c = DgetC(0);
				if (!any(c, "$\\`")) {
					unDgetC(c | QUOTE | (NLQUOTE<<8) );
					c = '\\';
				}
				else {
					*mbp++ = NLQUOTE;
					c |= QUOTE;
					if (--mcnt == 0) {
						setname((uchar_t *)"<<");
						bferr(MSGSTR(M_OVER, "Line overflow"));
					}
				}
			}
			*mbp++ = c;
			if (--mcnt == 0) {
				setname((uchar_t *)"<<");
				bferr(MSGSTR(M_OVER, "Line overflow"));
			}
#endif
		}
		*mbp++ = 0;

		/*
		 * If any ` in line do command substitution
		 */
		mbp = mbuf;
#ifdef KJI
		if (any_noquote('`', mbp)) {
#else
		if (any('`', mbp)) {
#endif
			/*
			 * 1 arg to dobackp causes substitution to be literal.
			 * Words are broken only at newlines so that all blanks
			 * and tabs are preserved.  Blank lines (null words)
			 * are not discarded.
			 */
			vp = dobackp(mbuf, 1);
		} else
			/* Setup trivial vector similar to return of dobackp */
			Dv[0] = mbp, Dv[1] = NOSTR, vp = Dv;

		/*
		 * Resurrect the words from the command substitution
		 * each separated by a newline.  Note that the last
		 * newline of a command substitution will have been
		 * discarded, but we put a newline after the last word
		 * because this represents the newline after the last
		 * input line!
		 */
		for (; *vp; vp++) {
#ifdef KJI
			for (mbp = *vp; *mbp;) {
				if (*mbp == NLQUOTE) 
					mbp++;
				ocnt -= NLchrlen(mbp);
				PUTSTR (obp, mbp)
				if (ocnt <= 0) {
#else
			for (mbp = *vp; *mbp; mbp++) {
				if (*mbp == NLQUOTE)
					*obp++ = *++mbp & TRIM;
				else
					*obp++ = *mbp;
				if (--ocnt == 0) {
#endif
					write(0, obuf, BUFR_SIZ);
					obp = obuf; ocnt = BUFR_SIZ;
				}
			}
			*obp++ = '\n';
			if (--ocnt == 0) {
				write(0, obuf, BUFR_SIZ);
				obp = obuf; ocnt = BUFR_SIZ;
			}
		}
		if (pargv)
			blkfree(pargv), pargv = 0;
	}
}
