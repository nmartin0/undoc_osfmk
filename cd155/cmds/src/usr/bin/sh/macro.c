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
static char rcsid[] = "@(#)$RCSfile: macro.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:12 $";
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
#include	"sym.h"
#include        <termios.h>
#if defined(NLS) || defined(KJI)
#include 	"shctype.h"
#endif

static uchar_t	quote;	/* used locally */
static uchar_t	quoted;	/* used locally */
static uchar_t	dqmac;	/* used locally */

static	int	comsubst ();
static	uchar_t	*copyto ();
static	void	flush ();
static	uchar_t	getch ();

static uchar_t *
copyto(endch)
register uchar_t   endch;
{
	register uchar_t	c;

	while ((c = getch(endch)) != endch && c)
		pushstak(c | quote);
	zerostak();
	if (c != endch)
		error(MSGSTR(M_BADSUB,(char *)badsub));
}

static
skipto(endch)
register uchar_t	endch;
{
	/*
	 * skip uchar_ts up to }
	 */
	register uchar_t	c;

	while ((c = readc()) && c != endch)
	{
		switch (c)
		{
		case SQUOTE:
			skipto(SQUOTE);
			break;

		case DQUOTE:
			skipto(DQUOTE);
			break;

		case DOLLAR:
			if (readc() == BRACE)
				skipto('}');
		}
	}
	if (c != endch)
		error(MSGSTR(M_BADSUB,(char *)badsub));
}

static uchar_t
getch(endch)
uchar_t	endch;
{
	register uchar_t	d;

retry:
	d = readc();
	if (!subchar(d))
		return(d);
	if (d == DOLLAR)
	{
		register unsigned int	c;
#ifdef KJI
		register unsigned int	nlc;
#endif

		if (dqmac)
			return(d);
#if defined(NLS) || defined(KJI)
# ifdef KJI
		c = readc();
		nlc = readwc(c);
		if (dolchar(c)|| (NLSencchar(c) && NLSletter(nlc)))
# else /* NLS */
		if ((c = readc(), dolchar(c)||NLSalphanum(c)))
# endif
#else
		if ((c = readc(), dolchar(c)))
#endif
		/* if this works, new dolchar() should be derived */
		{
			struct namnod *n = (struct namnod *)NIL;
			int		dolg = 0;
			BOOL		bra;
			BOOL		nulflg;
#if defined(NLS) || defined(KJI)
			BOOL		qflag = 0;
#endif
			register uchar_t	*argp, *v;
			uchar_t		idb[2];
			uchar_t		*id = idb;

			if (bra = (c == BRACE))
#ifdef KJI
			{
				c = readc();
				nlc = readwc(c);
			}
#else
				c = readc();
#endif
#if defined(NLS) || defined(KJI)
# ifdef KJI
			if (NLSencchar(c) && NLSletter(nlc))
# else
			if (NLSletter(c))
# endif
			{
				register uchar_t *s;
				argp = (uchar_t *)relstak();
				pushstak(FNLS);
# ifdef KJI
				while (NLSencchar(c) && NLSalphanum (nlc)) {
					pushstak (c);
					if (NLSfontshift(c))
					    NLpushstak (nlc);
					c = readc ();
					nlc = readwc(c);
				}	
				if (NLSfontshift (c))
					unreadwc (nlc);
# else
				while (1)
				{
					if (NLSfontshift(c)) {
						pushstak(c);
						c = readc();
					}
					else if (!NLSalphanum(c)) break;
					pushstak(c);
					c = readc();
				}
# endif
				zerostak();
				n =  lookup(NLSndecode(absstak(argp)));
#else
			if (letter(c))
			{
				argp = (char *)relstak();
				while (alphanum(c))
				{
					pushstak(c);
					c = readc();
				}
				zerostak();
				n = lookup(absstak(argp));
#endif
				setstak(argp);
				if (n->namflg & N_FUNCTN)
					error(MSGSTR(M_BADSUB,(char *)badsub));
				v = n->namval;
				id = n->namid;
				peekc = c | MARK;
			}
			else if (digchar(c))
			{
				*id = c;
				idb[1] = 0;
				if (astchar(c))
				{
					dolg = 1;
					c = '1';
				}
				c -= '0';
				v = ((c == 0) ? cmdadr : (c <= dolc) ? dolv[c] : (uchar_t *)(dolg = 0));
			}
			else if (c == '$')
				v = pidadr;
			else if (c == '!')
				v = pcsadr;
			else if (c == '#')
			{
				itos(dolc);
				v = numbuf;
			}
			else if (c == '?')
			{
				itos(retval);
				v = numbuf;
			}
			else if (c == '-')
				v = flagadr;
			else if (bra)
				error(MSGSTR(M_BADSUB,(char *)badsub));
			else
				goto retry;
			c = readc();
			if (c == ':' && bra)	/* null and unset fix */
			{
				nulflg = 1;
				c = readc();
			}
			else 
			{
				nulflg = 0;
			}
#if defined(NLS) || defined(KJI)
			if ( (c == '?') && bra && (!quote) )
				qflag = 1;
#endif
			if (!defchar(c) && bra)
				error(MSGSTR(M_BADSUB,(char *)badsub));
			argp = 0;
			if (bra)
			{
				if (c != '}')
				{
					argp = (uchar_t *)relstak();
					if ((v == 0 || (nulflg && *v == 0))
					    ^ (setchar(c)))
					{
#if defined(NLS) || defined(KJI)
					/* if possibility of conditional   */
					/* substitution, add magic header  */
					/* to mark string encoded         */
					    if (qflag == 1)
						pushstak(FNLS);
#endif
					    copyto('}');
					}
					else
					    skipto('}');
					argp = absstak(argp);
				}
			}
			else
			{
				peekc = c | MARK;
				c = 0;
			}
			if (v && (!nulflg || *v))
			{
				uchar_t tmp = (*id == '*' ? SP | quote : SP);

				if (c != '+')
				{
					for (;;)
					{
						if (*v == 0 && quote) {
							pushstak(QUOTE);
						} else
						{
#if defined(NLS) || defined(KJI)
							/* a bizarre inversion is happening here.
							Ordinarily decoded uchar_tacters are put into the
							input stream and encoded by readc().  macro() sets
							a flag to allow it to put encoded uchar_tacters into
							the stream.  But within $ substitutions it gets
							decoded strings from lookup.  So here it encodes
							them & pushes them into the stream.  The quote
							flag is merged into uchar_tacters as required. No
							FNLS uchar_tacter is pushed on the stack.
							*/

# ifdef KJI
							if (NLSisencoded(v))
							    while (c = *(++v))
									pushstak(c | quote);
							else
# endif
							    NLSencode_stk(v,quote);
#else
							while (c = *v++)
								pushstak(c | quote);
#endif
						}

						if (dolg == 0 || (++dolg > dolc))
							break;
						else
						{
							v = dolv[dolg];
							pushstak(tmp);
						}
					}
				}
			}
			else if (argp)
			{
				if (c == '?') {
					/*
					 * if there is an error message behind
					 * the '?', print it.  else print the
					 * standard "parameter null or not
					 * set" error
					 */

#if defined(NLS) || defined(KJI)
					/*
					 * have to do something special here
					 * for this to work with the FNLS
					 * code...
					 */
					uchar_t *arg = argp;

					/*
					 * if the arg is encoded, but that's
					 * the only thing here, point at the
					 * NULL so we get the proper mesg
					 */
					if (NLSisencoded(arg)) {
						(void) NLSskiphdr(arg);
						if (*arg != '\0')
							arg = argp;
						}

					failed(id, *arg ? arg : 
#else
					failed(id, *argp ? argp : 
#endif
					       (uchar_t *)(MSGSTR(M_BADPARAM,(char *)badparam)));
					}
				else if (c == '=')
				{
					if (n)
					{
						trim(argp);
#if defined(NLS) || defined(KJI)
						assign(n, NLSndecode (argp));
#else
						assign(n, argp);
#endif
					}
					else
						error(MSGSTR(M_BADSUB,(char *)badsub));
				}
			}
			else if (flags & setflg)
				failed(id, MSGSTR(M_UNSET,(char *)unset));
			goto retry;
		}
		else
#ifdef KJI
		{
			peekc = c | MARK;
			if (NLSfontshift(c))
				unreadwc (nlc);
		}
#else
			peekc = c | MARK;
#endif
	}
	else if (d == endch)
		return(d);
	else if (d == SQUOTE)
	{
		if (dqmac)
			return(d);
		comsubst();
		goto retry;
	}
	else if (d == DQUOTE)
	{
		quoted++;
		quote ^= QUOTE;
		goto retry;
	}
	return(d);
}

uchar_t *
macro(as)
uchar_t	*as;
{
	/*
	 * Strip "" and do $ substitution
	 * Leaves result on top of stack
	 */
	register BOOL	savqu = quoted;
	register uchar_t	savq = quote;
	struct fileheader fb;

	push(&fb);
	estabf(as);
#if defined(NLS) || defined(KJI)
# ifdef NLSDEBUG
	debug("macro in",as);
# endif
	standin->fraw = TRUE;

	/* estabf puts the given string into a file descriptor, then
	   processes it (using copyto()) as if it were input.  In general,
	   such processing encodes NLS strings to the internal format.
	   Since in this case the string 'as' is already encoded,
	   the flag fraw tells readc() to disable NLS encoding.
	   Some uchar_tacters may be quoted by the shell at this point;
	   all extended NLS uchar_tacters are preceded by some Font Shift.
	*/
#endif
	usestak();
	quote = 0;
	quoted = 0;
	copyto(0);
	pop();
#if defined(NLS) || defined(KJI)
	if (quoted && (stakbot == staktop-1))
		/* quoted null string still has FNLS header */
#else
	if (quoted && (stakbot == staktop))
#endif
		pushstak(QUOTE);
/*
 * above is the fix for *'.c' bug
 */
	quote = savq;
	quoted = savqu;
#if defined (NLSDEBUG)
	{ register uchar_t *s = fixstak();
	  fprintf ( stderr , "macro out = <%s>\n",s);
	  return(s);
	}
#else
	return(fixstak());
#endif
}

uchar_t *
dqmacro(as)
uchar_t	*as;
{
	dqmac = 1;
	as = macro(as);
	dqmac = 0;
	return(as);
}

static int
comsubst()
{
	/*
	 * command substn
	 */
	struct fileblk	cb;
	register uchar_t	d;
	register uchar_t *savptr = fixstak();

	usestak();
	while ((d = readc()) != SQUOTE && d)
		pushstak(d);
	{
		register uchar_t	*argc;

		trim(argc = fixstak());
		push(&cb);
		estabf(argc);
#if defined(NLS) || defined(KJI)
		standin->fraw = TRUE;
#endif
	}
	{
		register struct trenod *t = makefork(FPOU, cmd(EOFSYM, MTFLG | NLFLG));
		int		pv[2];

		/*
		 * this is done like this so that the pipe
		 * is open only when needed
		 */
		chkpipe(pv);
		initf(pv[INPIPE]);
		execute(t, 0, (int)(flags & errflg), 0, pv);
		close(pv[OTPIPE]);
	}
	tdystak(savptr);
	staktop = movstr(savptr, stakbot);
	while (d = readc())
		pushstak(d | quote);
	await(0, 0);
#ifdef KJI
	/*  For SJIS, cannot strip trailing newlines from top of
	 *  stack down by examining top byte only.  Need to find
	 *  start of uchar_tacter at top of stack. ((0x0a | 0x80) is 
	 *  not a legal 1-byte uchar_tacter but is a legal second byte
	 *  of a 2-byte uchar_tacter -- if so, previous byte could be 
	 *  a SJIS font shift or the second byte of a 2-byte uchar_tacter.)
	 */
	{
	do {
		savptr = stakbot;
		while (d = *savptr & STRIP)
		    if ((savptr += NLSenclen (d)) >= staktop)
			break;
		/* d will be ASCII uchar_tacter or magic font shift */
		if (d != NL)
			break;
#ifdef NLSDEBUG
		debug ("comsubst - NL stripped", d);
#endif
	} while ((staktop -= NLSenclen (d)) > stakbot);
	}
#else
	while (stakbot != staktop)
	{
#ifdef NLS
		if ((*--staktop & STRIP) != NL || ((stakbot != staktop) &&
			(NLSfontshift (staktop [-1]))))
#else
		if ((*--staktop & STRIP) != NL)
#endif
		{
			++staktop;
			break;
		}
	}
#endif
	pop();
}

#define CPYSIZ	512

subst(in, ot)
int	in, ot;
{
	register uchar_t	c;
	struct fileblk	fb;
	register int	count = CPYSIZ;

	push(&fb);
	initf(in);
#if defined(NLS) || defined(KJI)
	standin->fraw = TRUE;   /* avoid re-NLS-encoding input */
#endif
	/*
	 * DQUOTE used to stop it from quoting
	 */
	while (c = (getch(DQUOTE) & STRIP))
	{
		pushstak(c);
#if defined(NLS) || defined(KJI)
		if (NLSfontshift(c))
			pushstak(getch(DQUOTE));
# ifdef KJI
		if ((c == FSH20) || (c == FSH21))
			pushstak(getch(DQUOTE));
# endif
		if (--count <= 0)  /* using stack, CPYSIZ can be off 1 */
#else
		if (--count == 0)
#endif
		{
			flush(ot);
			count = CPYSIZ;
		}
	}
	flush(ot);
	pop();
}

static void
flush(ot)
{
#if defined(NLS) || defined(KJI)
	*staktop = 0;
# if NLSDEBUG
	debug("flush in",stakbot);
# endif
	NLSdecode(stakbot);
	staktop = stakbot + strlen(stakbot);
# if NLSDEBUG
	debug("flush out",stakbot);
# endif
#endif
	write(ot, stakbot, staktop - stakbot);

		/*	PTM 3737 - The following has been commented out
		*	because of PTM 3737. The execpr is a define in
		*	defs.h that will mask for the "-x" flag. Why it
		*	does this is a mystery, because it will place a
		*	"\n" in the stderr file.
		*	I have left the code here because I do not know
		*	if anything else is affected. I ran some tests
		*	and they appeared to run fine.
	if (flags & execpr)
		write(output, stakbot, staktop - stakbot);
		*/

	staktop = stakbot;
}
