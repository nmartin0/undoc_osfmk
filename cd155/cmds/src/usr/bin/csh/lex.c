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
static char rcsid[] = "@(#)$RCSfile: lex.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:35 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: lex prlex copylex freelex word getC getdol addla getexcl 
 *            getsub dosub subword domod matchs getsel gethent findev noev 
 *            matchev setexclp unreadc readc bgetc bfree bseek btell 
 *            btoeof settell
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

#include <fcntl.h>
#if defined(NLS) || defined(KJI)
#include <NLchar.h>
#include <NLctype.h>
#endif
#include "sh.h"

/*
 * C shell
 */

/*
 * These lexical routines read input and form lists of words.
 * There is some involved processing here, because of the complications
 * of input buffering, and especially because of history substitution.
 */

uchar_t	*word();

/*
 * Peekc is a peek uchar_tacer for getC, peekread for readc.
 * There is a subtlety here in many places... history routines
 * will read ahead and then insert stuff into the input stream.
 * If they push back a character then they must push it behind
 * the text substituted by the history substitution.  On the other
 * hand in several places we need 2 peek character.  To make this
 * all work, the history routines read with getC, and make use both
 * of ungetC and unreadc.  The key observation is that the state
 * of getC at the call of a history reference is such that calls
 * to getC from the history routines will always yield calls of
 * readc, unless this peeking is involved.  That is to say that during
 * getexcl the variables lap, exclp, and exclnxt are all zero.
 *
 * Getdol invokes history substitution, hence the extra peek, peekd,
 * which it can ungetD to be before history substitutions.
 */
#ifdef NLS
/* As if it weren't complicated enough, NLS processing requires that
 * quotes take up a separate byte.  Since processing sometimes QUOTEs
 * a character before calling ungetC, peekc has room for 2 bytes.
 *
 * How it works: non-ASCII character are never QUOTEd.  ASCII character
 * which are to be QUOTED are preceded by a special mark, NLQUOTE, and
 * then QUOTED.  Space must be reserved for this process when it might
 * happen.  Trimming functions must remove the NLQUOTE.
 */
#endif

#ifdef KJI
/* The quoting mechanism for KJI is to precede each quoted character by
 * an NLQUOTE.
 */
#endif

#if defined(NLS) || defined(KJI)
int     peekc, peekd, peeke;
#else
uchar_t	peekc, peekd;
#endif
#ifdef KJI
static int shiftseen = 0;
int	peekread;
#else
uchar_t    peekread;
#endif

uchar_t	*exclp;			/* (Tail of) current word from ! subst */
struct	wordent *exclnxt;	/* The rest of the ! subst words */
int	exclc;			/* Count of remainig words in ! subst */
uchar_t	*alvecp;		/* "Globp" for alias resubstitution */

/*
 * Lex returns to its caller not only a wordlist (as a "var" parameter)
 * but also whether a history substitution occurred.  This is used in
 * the main (process) routine to determine whether to echo, and also
 * when called by the alias routine to determine whether to keep the
 * argument list.
 */
bool	hadhist;

#define ungetC(c)       peekc = c
#define	ungetD(c)	peekd = c
#ifdef KJI
#define	ungetE(c)	peeke = c
#endif

lex(hp)
	register struct wordent *hp;
{
	register struct wordent *wdp;
	int c;

	lineloc = btell();
	hp->next = hp->prev = hp;
	hp->word = (uchar_t *)"";
	alvecp = 0, hadhist = 0;
	do
		c = readc(0);
#ifdef KJI
	while (c == ' ' || c == '\t' || isjspace(c));
#else
	while (c == ' ' || c == '\t');
#endif
	if (c == HISTSUB && intty)
		/* ^lef^rit	from tty is short !:s^lef^rit */
		getexcl(c);
	else
		unreadc(c);
	wdp = hp;
	/*
	 * The following loop is written so that the links needed
	 * by freelex will be ready and rarin to go even if it is
	 * interrupted.
	 */
	do {
		register struct wordent *new =
			(struct wordent *) calloc(1, sizeof *wdp);

		new->prev = wdp;
		new->next = hp;
		wdp->next = new;
		wdp = new;
		wdp->word = word();
	} while (wdp->word[0] != '\n');
	hp->prev = wdp;
	return (hadhist);
}

prlex(sp0)
	struct wordent *sp0;
{
	register struct wordent *sp = sp0->next;

	for (;;) {
		printf("%s", sp->word);
		sp = sp->next;
		if (sp == sp0)
			break;
		printf(" ");
	}
}

#ifdef CMDEDIT
prpushlex(sp0)
        struct wordent *sp0;
{
        static uchar_t *buffer, *p, *q;
        static int buflen = 0;
        int len = 0;
        register struct wordent *sp = sp0->next;

        for (;;) {
                len += strlen(sp->word) + 1;
                sp = sp->next;
                if (sp->next == sp0)
                        break;
        }
        if (buflen < len) {
                if (buffer)
                        free(buffer);
                buffer = malloc(buflen = len);
        }
        p = buffer;
        sp = sp0->next;
        for (;;) {
                q = sp->word;
                while (*p = *q++)
                        p++;
                sp = sp->next;
                if (sp->next == sp0)
                        break;
                if (sp->word[0] != '\n')
                        *p++ = ' ';
        }
        *p = '\0';
        ShellTypeAheadToTenex = buffer;
}
#endif  /* CMDEDIT */

copylex(hp, fp)
	register struct wordent *hp;
	struct wordent *fp;
{
	register struct wordent *wdp;

	wdp = hp;
	fp = fp->next;
	do {
		register struct wordent *new = 
			(struct wordent *) calloc(1, sizeof *wdp);

		new->prev = wdp;
		new->next = hp;
		wdp->next = new;
		wdp = new;
		wdp->word = savestr(fp->word);
		fp = fp->next;
	} while (wdp->word[0] != '\n');
	hp->prev = wdp;
}

freelex(vp)
	register struct wordent *vp;
{
	register struct wordent *fp;

	while (vp->next != vp) {
		fp = vp->next;
		vp->next = fp->next;
		xfree(fp->word);
		xfree((char *)fp);
	}
	vp->prev = vp;
}

#ifdef KJI
/* Include double-wide space. */
uchar_t WORDMETA[] = {'#', ' ', '\'', '`', '\\', '"', '\t', ';', '&', '<', '>',
		   '(', ')', '|', '\n', 0x81, 0x40};
#else
uchar_t	*WORDMETA =	(uchar_t *)"# '`\"\t;&<>()|\n";
#endif

uchar_t *
word()
{
#if defined(NLS) || defined(KJI)
	register int c, c1;
#else
	register uchar_t c, c1;
#endif
	register uchar_t *wp;
	uchar_t wbuf[BUFR_SIZ];
	register bool dolflg;
	register int i;

	wp = wbuf;
	i = BUFR_SIZ - 4;
loop:
	c = getC(DOALL);
#ifdef KJI
	if (isjspace(c))
		goto loop;
#endif
	switch (c) {

	case ' ':
	case '\t':
		goto loop;

	case '`':
	case '\'':
	case '"':
		*wp++ = c, --i, c1 = c;
		dolflg = c == '"' ? DOALL : DOEXCL;
		for (;;) {
			c = getC(dolflg);
			if (c == c1)
				break;
			if (c == '\n') {
				seterrc(MSGSTR(M_UNMATCH,"Unmatched "), c1);
				ungetC(c);
				goto ret; }
#ifdef KJI
			if ((c == '\\')) {
				c = getC(0);
				if (c == '\n') {
					/*
					 * Note that '\\' quotes newline
					 * only within single and double 
					 * quotes, and does not quote any 
					 * of the quote character.
					 */
					if (c1 == '`') {
						ungetC(c); continue;
					} else {
						if (--i <= 0)
							goto toochars;
						*wp++ = NLQUOTE;
					}
				} else if (c == HIST) {
					if (--i <= 0)
						goto toochars;
					*wp++ = NLQUOTE;
				} else {
					ungetC(c);
					c = '\\';
				}
			}
			if ((--i <= 0) || ((c > 0xff) && (--i <=0)))
				goto toochars;
			PUTCH (wp,c);
#else /* not KJI */
			if (c == '\\') {
				c = getC(0);
				if (c == HIST)
#ifdef NLS
				{
					if (--i <= 0)
						goto toochars;
					*wp++ = NLQUOTE;
					c |= QUOTE;
				}
#else
					c |= QUOTE;
#endif
				else {
					if (c == '\n' && c1 != '`')
#ifdef NLS
						c |= (NLQUOTE<<8) | QUOTE;
#else
						c |= QUOTE;
#endif
					ungetC(c), c = '\\';
				}
			}
			if (--i <= 0)
				goto toochars;
			*wp++ = c;
#endif /* not KJI */
		}
		*wp++ = c, --i;
		goto pack;

	case '&':
	case '|':
	case '<':
	case '>':
		*wp++ = c;
		c1 = getC(DOALL);
		if (c1 == c)
			*wp++ = c1;
		else
			ungetC(c1);
		goto ret;

	case '#':
		if (intty)
			break;
		if (wp != wbuf) {
			ungetC(c);
			goto ret;
		}
		c = 0;
		do {
			c1 = c;
			c = getC(0);
		} while (c != '\n');
		if (c1 == '\\')
			goto loop;
		/* fall into ... */

	case ';':
	case '(':
	case ')':
	case '\n':
		*wp++ = c;
		goto ret;

casebksl:
	case '\\':
		c = getC(0);
		if (c == '\n') {
			if (onelflg == 1)
				onelflg = 2;
			goto loop;
		}
#ifdef KJI	
		if ((i-=2) <= 0)
			goto toochars;
		*wp++ = '\\';
		PUTCH(wp,c);
		goto pack;

#else /* not KJI */
		if ((c&QUOTE) == 0)
			c |= (NLQUOTE<<8) | QUOTE;
		break;
#endif /* not KJI */

	}
	if ( c != NLQUOTE)
	    ungetC(c);
	else *wp++ = c;
pack:
	for (;;) {
		c = getC(DOALL);
#ifdef KJI
		if ((c == '\\') || (c == NLQUOTE)) {
			*wp++ = c;
#else
		if (c == '\\') {
#endif
			c = getC(0);
			if (c == '\n') {
				if (onelflg == 1)
					onelflg = 2;
#ifdef KJI
				wp--;
#endif
				goto ret;
			}
#ifdef KJI	
			if ((i-=2) <= 0)
				goto toochars;
			PUTCH(wp,c);
			continue;
		}
		if (any(c, WORDMETA + intty)) {
#else /* not KJI */		
#ifdef NLS
			if ((c&QUOTE) == 0)
				c |= (NLQUOTE<<8) | QUOTE;
#else
			c |= QUOTE;
#endif
		}
		if (any(c, WORDMETA + intty)) {
#endif /* not KJI */
			ungetC(c);
			if (any(c, "\"'`"))
				goto loop;
			goto ret;
		}
#ifdef NLS
		if ( c>>8 ) {
			if (--i <= 0)
				goto toochars;
			*wp++ = c>>8;
		}
#endif
#ifdef KJI
		if ((--i <= 0) || ((c > 0xff) && (--i <=0)))
			goto toochars;
		PUTCH (wp, c);
#else
		if (--i <= 0)
			goto toochars;
		*wp++ = c;
#endif
	}
toochars:
	seterr(MSGSTR(M_WORD, "Word too long"));
	wp = &wbuf[1];
ret:
	*wp = 0;

	return (savestr(wbuf));
}

#ifdef KJI
/*
 * Get next 1- or 2-byte character as an integral value.
*/
getC(flag)
{
	register int c,d;
	c = kji_getC(flag);
	if ((c < 0xff) && NCisshift (c))
		d = kji_getC(flag), c = _NCd2(c, d);
	return (c);
}

kji_getC(flag)
#else
getC(flag)
#endif
	register int flag;
{
#if defined(NLS) || defined(KJI)
	register int c;
#else
	register uchar_t c;
#endif

top:
	if (c = peekc) {
#ifdef NLS
		if (c >> 8) {
		    c >>= 8;
		    peekc &= 0377;
		}
		else
#endif
		peekc = 0;
		return (c);
	}
	if (lap) {
		c = *lap++;
		if (c == 0) {
			lap = 0;
			goto top;
		}
#if defined(NLS) || defined(KJI)
#ifdef KJI
		if (!shiftseen) {
			if (NCisshift(c))
				shiftseen++;
			else if (any(c, WORDMETA + intty)) {
					peekc = c;
					return (NLQUOTE);
			}
		} else
			shiftseen = 0;
#else
		if (any(c, WORDMETA + intty)) {
			peekc = c | QUOTE;
			return (NLQUOTE);
		}
#endif
#else
		if (any(c, WORDMETA + intty))
			c |= QUOTE;
#endif
		return (c);
	}
	if (c = peekd) {
		peekd = 0;
		return (c);
	}
#ifdef KJI
	if (c = peeke) {
		peeke = 0;
		return (c);
	}
#endif
	if (exclp) {
		if (c = *exclp++)
			return (c);
		if (exclnxt && --exclc >= 0) {
			exclnxt = exclnxt->next;
			setexclp(exclnxt->word);
			return (' ');
		}
		exclp = 0;
		exclnxt = 0;
	}
	if (exclnxt) {
		exclnxt = exclnxt->next;
		if (--exclc < 0)
			exclnxt = 0;
		else
			setexclp(exclnxt->word);
		goto top;
	}
	c = readc(0);
	if (c == '$' && (flag & DODOL)) {
		getdol();
		goto top;
	}
	if (c == HIST && (flag & DOEXCL)) {
		getexcl(0);
		goto top;
	}
	return (c);
}

getdol()
{
	register uchar_t *np;
	uchar_t name[MAX_ENV_NAMLEN];
	register int c;
	int sc;
	bool special = 0;

	np = name, *np++ = '$';
	c = sc = getC(DOEXCL);
#ifdef KJI
	if (any(c, "\t \n") || isjspace(c)) {
		ungetC(NLQUOTE);
		ungetD('$');
		ungetE(c);
#else
	if (any(c, "\t \n")) {
		ungetD(c);
#ifdef NLS
		ungetC('$' | QUOTE | (NLQUOTE<<8));
#else
		ungetC('$' | QUOTE);
#endif
#endif
		return;
	}
	if (c == '{')
		*np++ = c, c = getC(DOEXCL);
	if (c == '#' || c == '?')
		special++, *np++ = c, c = getC(DOEXCL);
#ifdef KJI
	PUTCH (np,c);
#else
	*np++ = c;
#endif
	switch (c) {
	
	case '<':
	case '$':
		if (special)
			goto vsyn;
		goto ret;

	case '\n':
		ungetD(c);
		np--;
		goto vsyn;

	case '*':
		if (special)
			goto vsyn;
		goto ret;

	default:
		if (digit(c)) {
/*
 * let $?0 pass for now
			if (special)
				goto vsyn;
*/
			while (digit(c = getC(DOEXCL))) {
				if (np < &name[sizeof name ])
#ifdef KJI
					PUTCH (np,c);
#else
					*np++ = c;
#endif
			}
		} else if (letter(c))
			while (alnum(c = getC(DOEXCL))) {
				if (np < &name[sizeof name ])
#ifdef KJI
					PUTCH (np,c);
#else
					*np++ = c;
#endif
#ifdef NLS
				 /*
				  *  Avoid dangling shift character
				  */
				 else if (NCisshift(np[-1]))
					np--;

#endif
			}
		else
			goto vsyn;
	}
	if (c == '[') {
		*np++ = c;
		do {
			c = getC(DOEXCL);
			if (c == '\n') {
				ungetD(c);
				np--;
				goto vsyn;
			}
			if (np >= &name[sizeof name - 8])
				goto vsyn;
#ifdef KJI
			PUTCH (np,c);
#else
			*np++ = c;
#endif
		} while (c != ']');
		c = getC(DOEXCL);
	}
	if (c == ':') {
		*np++ = c, c = getC(DOEXCL);
		if (c == 'g')
			*np++ = c, c = getC(DOEXCL);
#ifdef KJI
		PUTCH (np,c);
#else
		*np++ = c;
#endif
		if (!any(c, "htrqxe"))
			goto vsyn;
	} else
		ungetD(c);
	if (sc == '{') {
		c = getC(DOEXCL);
		if (c != '}') {
			ungetC(c);
			goto vsyn;
		}
		*np++ = c;
	}
ret:
	*np = 0;
	addla(name);
	return;

vsyn:
	seterr(MSGSTR(M_VARSYN, "Variable syntax"));
	goto ret;
}

addla(cp)
	uchar_t *cp;
{
	uchar_t buf[BUFR_SIZ];

	if (lap != 0 && strlen(cp) + strlen(lap) >= sizeof (labuf) - 4) {
		seterr(MSGSTR(M_LABUF, "Expansion buf ovflo"));
		return;
	}
	if (lap)
		strcpy(buf, lap);
	strcpy(labuf, cp);
	if (lap)
		strcat(labuf, buf);
	lap = labuf;
}

#ifdef KJI
uchar_t	lhsb[64];
uchar_t	slhs[64];
uchar_t	rhsb[128];
#else
uchar_t	lhsb[32];
uchar_t	slhs[32];
uchar_t	rhsb[64];
#endif
int	quesarg;

getexcl(sc)
#ifdef KJI
	int sc;
#else
	uchar_t sc;
#endif
{
	register struct wordent *hp, *ip;
	int left, right, dol;
	register int c;

	if (sc == 0) {
		sc = getC(0);
		if (sc != '{') {
			ungetC(sc);
			sc = 0;
		}
	}
	quesarg = -1;
	lastev = eventno;
	hp = gethent(sc);
	if (hp == 0)
		return;
	hadhist = 1;
	dol = 0;
	if (hp == alhistp)
		for (ip = hp->next->next; ip != alhistt; ip = ip->next)
			dol++;
	else
		for (ip = hp->next->next; ip != hp->prev; ip = ip->next)
			dol++;
	left = 0, right = dol;
	if (sc == HISTSUB) {
		ungetC('s'), unreadc(HISTSUB), c = ':';
		goto subst;
	}
	c = getC(0);
	if (!any(c, ":^$*-%"))
		goto subst;
	left = right = -1;
	if (c == ':') {
		c = getC(0);
		unreadc(c);
		if (letter(c) || c == '&') {
			c = ':';
			left = 0, right = dol;
			goto subst;
		}
	} else
		ungetC(c);
	if (!getsel(&left, &right, dol))
		return;
	c = getC(0);
	if (c == '*')
		ungetC(c), c = '-';
	if (c == '-') {
		if (!getsel(&left, &right, dol))
			return;
		c = getC(0);
	}
subst:
	exclc = right - left + 1;
	while (--left >= 0)
		hp = hp->next;
	if (sc == HISTSUB || c == ':') {
		do {
			hp = getsub(hp);
			c = getC(0);
		} while (c == ':');
	}
	unreadc(c);
	if (sc == '{') {
		c = getC(0);
		if (c != '}')
			seterr(MSGSTR(M_BADEXCL, "Bad ! form"));
	}
	exclnxt = hp;
}

struct wordent *
getsub(en)
	struct wordent *en;
{
	register uchar_t *cp;
	int delim;
	register int c;
	int sc;
	bool global = 0;
	uchar_t orhsb[sizeof rhsb];

	exclnxt = 0;
	sc = c = getC(0);
	if (c == 'g')
		global++, c = getC(0);
	switch (c) {

	case 'p':
		justpr++;
		goto ret;

	case 'x':
	case 'q':
		global++;
		/* fall into ... */

	case 'h':
	case 'r':
	case 't':
	case 'e':
		break;

	case '&':
		if (slhs[0] == 0) {
			seterr(MSGSTR(M_SUB, "No prev sub"));
			goto ret;
		}
		strcpy(lhsb, slhs);
		break;


	case 's':
		delim = getC(0);
#ifdef KJI
		if (letter(delim) || digit(delim) || any(delim, " \t\n")
		    || isjspace(delim)) {
#else
		if (letter(delim) || digit(delim) || any(delim, " \t\n")) {
#endif
			unreadc(delim);
bads:
			lhsb[0] = 0;
			seterr(MSGSTR(M_BADSUB, "Bad substitute"));
			goto ret;
		}
		cp = lhsb;
		for (;;) {
			c = getC(0);
			if (c == '\n') {
				unreadc(c);
				goto bads;
			}
			if (c == delim)
				break;
			if (cp > &lhsb[sizeof lhsb - 2])
				goto bads;
			if (c == '\\') {
				c = getC(0);
				if (c != delim && c != '\\')
					*cp++ = '\\';
			}
#ifdef KJI
			PUTCH (cp,c);
#else
			*cp++ = c;
#endif
		}
		if (cp != lhsb)
			*cp++ = 0;
		else if (lhsb[0] == 0) {
/*badlhs:*/
			seterr(MSGSTR(M_BADLHS, "No prev lhs"));
			goto ret;
		}
		cp = rhsb;
		strcpy(orhsb, cp);
		for (;;) {
			c = getC(0);
			if (c == '\n') {
				unreadc(c);
				break;
			}
			if (c == delim)
				break;
			if (cp > &rhsb[sizeof rhsb - 2]) {
/*toorhs:*/
				seterr(MSGSTR(M_BADRHS, "Rhs too long"));
				goto ret;
			}
			if (c == '\\') {
				c = getC(0);
				if (c != delim /* && c != '~' */)
					*cp++ = '\\';
			}
#ifdef KJI
			PUTCH (cp,c);
#else
			*cp++ = c;
#endif
		}
		*cp++ = 0;
		break;

	default:
		if (c == '\n')
			unreadc(c);
		seterrc(MSGSTR(M_BADEXCLM, "Bad ! modifier: "), c);
		goto ret;
	}
	strcpy(slhs, lhsb);
	if (exclc)
		en = dosub(sc, en, global);
ret:
	return (en);
}

struct wordent *
dosub(sc, en, global)
	int sc;
	struct wordent *en;
	bool global;
{
	struct wordent lex;
	bool didsub = 0;
	struct wordent *hp = &lex;
	register struct wordent *wdp;
	register int i = exclc;

	wdp = hp;
	while (--i >= 0) {
		register struct wordent *new = 
			(struct wordent *) calloc(1, sizeof *wdp);

		new->prev = wdp;
		new->next = hp;
		wdp->next = new;
		wdp = new;
		en = en->next;
		wdp->word = global || didsub == 0 ?
		    subword(en->word, sc, &didsub) : savestr(en->word);
	}
	if (didsub == 0)
		seterr(MSGSTR(M_MODFAIL, "Modifier failed"));
	hp->prev = wdp;
	return (&enthist(-1000, &lex, 0)->Hlex);
}

uchar_t *
subword(cp, type, adid)
	uchar_t *cp;
	int type;
	bool *adid;
{
	uchar_t wbuf[BUFR_SIZ];
	register uchar_t *wp, *mp, *np;
	register int i;

	switch (type) {

	case 'r':
	case 'e':
	case 'h':
	case 't':
	case 'q':
	case 'x':
		wp = domod(cp, type);
		if (wp == 0)
			return (savestr(cp));
		*adid = 1;
		return (wp);

	default:
		wp = wbuf;
		i = BUFR_SIZ - 4;
#ifdef KJI
		for (mp = cp; *mp; mp+=NLchrlen (mp))
#else
		for (mp = cp; *mp; mp++)
#endif
			if (matchs(mp, lhsb)) {
				for (np = cp; np < mp;)
					*wp++ = *np++, --i;
				for (np = rhsb; *np; np++) switch (*np) {

				case '\\':
					if (np[1] == '&')
						np++;
					/* fall into ... */

				default:
#ifdef KJI
					if (NCisshift (*np)) {
						if ((i-=2) < 0)
							goto ovflo;
						*wp++ = *np++;
					}
#else
					if (--i < 0)
						goto ovflo;
#endif
					*wp++ = *np;
					continue;

				case '&':
					i -= strlen(lhsb);
					if (i < 0)
						goto ovflo;
					*wp = 0;
					strcat(wp, lhsb);
					wp = strend(wp);
					continue;
				}
				mp += strlen(lhsb);
				i -= strlen(mp);
				if (i < 0) {
ovflo:
					seterr(MSGSTR(M_OVFLO,"Subst buf ovflo"));
					return ((uchar_t *)"");
				}
				*wp = 0;
				strcat(wp, mp);
				*adid = 1;
				return (savestr(wbuf));
			}
		return (savestr(cp));
	}
}

uchar_t *
domod(cp, type)
	uchar_t *cp;
	int type;
{
	register uchar_t *wp, *xp;
	register int c;

	switch (type) {

	case 'x':
	case 'q':
#if defined(NLS) || defined(KJI)
		/* string must be big enough to NLquote every character */
		{ register uchar_t *tp;
		  wp = calloc(1, strlen(cp)*2 + 1);
		  for (xp = cp, tp = wp; c = *tp = *xp; xp++, tp++)
#ifdef NLS
			if ((c&QUOTE) == 0)
#endif
				if ((c != ' ' && c != '\t') || type == 'q') {
#ifdef KJI
					if (NCisshift(c) && isjspace(_NCd2(c,*(xp+1)))) {
					    xp++;
					    continue;
					}
					*tp = NLQUOTE;
					*++tp = c;
					if (NCisshift(c))
						*++tp = *++xp;
#else
					*tp = NLQUOTE;
					*++tp = c | QUOTE;
#endif
				}
		}
#else	/* not NLS and not KJI */
		wp = savestr(cp);
		for (xp = wp; c = *xp; xp++)
			if ((c != ' ' && c != '\t') || type == 'q')
				*xp |= QUOTE;
#endif
		return (wp);

	case 'h':
	case 't':
		if (!any('/', cp))	/* what if :h :t are both the same? */
			return (0);
		wp = strend(cp);
		while (*--wp != '/')
			continue;
		if (type == 'h')
take:
			xp = savestr(cp), xp[wp - cp] = 0;
		else
			xp = savestr(wp + 1);
		return (xp);

	case 'e':
	case 'r':
		wp = strend(cp);
		for (wp--; wp >= cp && *wp != '/'; wp--)
			if (*wp == '.') {
				if (type == 'e')
					xp = savestr(wp + 1);
				else
					xp = savestr(cp), xp[wp - cp] = 0;
				return (xp);
			}
		return (savestr(type == 'e' ? (uchar_t *)"" : cp));
	}
	return (0);
}

matchs(str, pat)
	register uchar_t *str, *pat;
{

	while (*str && *pat && *str == *pat)
		str++, pat++;
	return (*pat == 0);
}

getsel(al, ar, dol)
	register int *al, *ar;
	int dol;
{
	register int c = getC(0);
	register int i;
	bool first = *al < 0;

	switch (c) {

	case '%':
		if (quesarg == -1)
			goto bad;
		if (*al < 0)
			*al = quesarg;
		*ar = quesarg;
		break;

	case '-':
		if (*al < 0) {
			*al = 0;
			*ar = dol - 1;
			unreadc(c);
		}
		return (1);

	case '^':
		if (*al < 0)
			*al = 1;
		*ar = 1;
		break;

	case '$':
		if (*al < 0)
			*al = dol;
		*ar = dol;
		break;

	case '*':
		if (*al < 0)
			*al = 1;
		*ar = dol;
		if (*ar < *al) {
			*ar = 0;
			*al = 1;
			return (1);
		}
		break;

	default:
		if (digit(c)) {
			i = 0;
			while (digit(c)) {
				i = i * 10 + c - '0';
				c = getC(0);
			}
			if (i < 0)
				i = dol + 1;
			if (*al < 0)
				*al = i;
			*ar = i;
		} else
			if (*al < 0)
				*al = 0, *ar = dol;
			else
				*ar = dol - 1;
		unreadc(c);
		break;
	}
	if (first) {
		c = getC(0);
		unreadc(c);
		if (any(c, "-$*"))
			return (1);
	}
	if (*al > *ar || *ar > dol) {
bad:
		seterr(MSGSTR(M_BADARG, "Bad ! arg selector"));
		return (0);
	}
	return (1);

}

struct wordent *
gethent(sc)
	int sc;
{
	register struct Hist *hp;
	register uchar_t *np;
	register int c;
	int event;
	bool back = 0;

	c = sc == HISTSUB ? HIST : getC(0);
	if (c == HIST) {
		if (alhistp)
			return (alhistp);
		event = eventno;
		goto skip;
	}
	switch (c) {

	case ':':
	case '^':
	case '$':
	case '*':
	case '%':
		ungetC(c);
		if (lastev == eventno && alhistp)
			return (alhistp);
		event = lastev;
		break;

	case '-':
		back = 1;
		c = getC(0);
		goto number;

	default:
		if (any(c, "(=~")) {
			unreadc(c);
			ungetC(HIST);
			return (0);
		}
		if (digit(c))
			goto number;
		np = lhsb;
#ifdef KJI
		while (!any(c, ": \t\\\n}") && !isjspace(c)) {
			if (np < &lhsb[sizeof lhsb - (c > 0xff ? 3 : 2)])
				PUTCH (np,c);
#else
		while (!any(c, ": \t\\\n}")) {
			if (np < &lhsb[sizeof lhsb - 2])
				*np++ = c;
#endif
			c = getC(0);
		}
		unreadc(c);
		if (np == lhsb) {
			ungetC(HIST);
			return (0);
		}
		*np++ = 0;
		hp = findev(lhsb, 0);
		if (hp)
			lastev = hp->Hnum;
#ifdef KJI
		else
			return (0);
#endif
		return (&hp->Hlex);

	case '?':
		np = lhsb;
		for (;;) {
			c = getC(0);
			if (c == '\n') {
				unreadc(c);
				break;
			}
			if (c == '?')
				break;
#ifdef KJI
			if (np < &lhsb[sizeof lhsb - (c > 0xff ? 3 : 2)])
				PUTCH (np,c);
#else
			if (np < &lhsb[sizeof lhsb - 2])
				*np++ = c;
#endif
		}
		if (np == lhsb) {
			if (lhsb[0] == 0) {
				seterr(MSGSTR(NO_SEARCH,"No prev search"));
				return (0);
			}
		} else
			*np++ = 0;
		hp = findev(lhsb, 1);
		if (hp)
			lastev = hp->Hnum;
		return (&hp->Hlex);

	number:
		event = 0;
		while (digit(c)) {
			event = event * 10 + c - '0';
			c = getC(0);
		}
		if (back)
			event = eventno + (alhistp == 0) - (event ? event : 0);
		unreadc(c);
		break;
	}
skip:
	for (hp = Histlist.Hnext; hp; hp = hp->Hnext)
		if (hp->Hnum == event) {
			hp->Href = eventno;
			lastev = hp->Hnum;
			return (&hp->Hlex);
		}
	np = putn(event);
	noev(np);
	return (0);
}

struct Hist *
findev(cp, anyarg)
	uchar_t *cp;
	bool anyarg;
{
	register struct Hist *hp;

	for (hp = Histlist.Hnext; hp; hp = hp->Hnext)
		if (matchev(hp, cp, anyarg))
			return (hp);
	noev(cp);
	return (0);
}

noev(cp)
	uchar_t *cp;
{

	seterr2(cp, MSGSTR(M_NOEVENT,": Event not found"));
}

matchev(hp, cp, anyarg)
	register struct Hist *hp;
	uchar_t *cp;
	bool anyarg;
{
	register uchar_t *dp;
	struct wordent *lp = &hp->Hlex;
	int argno = 0;

	for (;;) {
		lp = lp->next;
		if (lp->word[0] == '\n')
			return (0);
		for (dp = lp->word; *dp; dp++) {
			if (matchs(dp, cp)) {
				if (anyarg)
					quesarg = argno;
				return (1);
			}
			if (!anyarg)
				return (0);
		}
		argno++;
	}
}

setexclp(cp)
	register uchar_t *cp;
{

	if (cp[0] == '\n')
		return;
	exclp = cp;
}

unreadc(c)
#ifdef KJI
	int c;
#else
	uchar_t c;
#endif
{

	peekread = c;
}

#ifdef KJI
/*
 * Read next 1- or 2-byte character and return as an integral value.
*/
readc(wanteof)
{
	register int c,d;
	c = kji_readc(wanteof);
	if ((c < 0xff) && NCisshift (c)) {
		d = kji_readc(wanteof);
		return (_NCd2(c, d));
	}
	return (c);
}

kji_readc(wanteof)
#else
readc(wanteof)
#endif
	bool wanteof;
{
	int c;
	static sincereal;

	if (c = peekread) {
		peekread = 0;
		return (c);
	}
top:
	if (alvecp) {
		if (c = *alvecp++)
			return (c);
		if (*alvec) {
			alvecp = *alvec++;
			return (' ');
		}
	}
	if (alvec) {
		if (alvecp = *alvec) {
			alvec++;
			goto top;
		}
		/* Infinite source! */
		return ('\n');
	}
	if (evalp) {
		if (c = *evalp++)
			return (c);
		if (*evalvec) {
			evalp = *evalvec++;
			return (' ');
		}
		evalp = 0;
	}
	if (evalvec) {
		if (evalvec == (uchar_t **)1) {
			doneinp = 1;
			reset();
		}
		if (evalp = *evalvec) {
			evalvec++;
			goto top;
		}
		evalvec = (uchar_t **)1;
		return ('\n');
	}
	do {
		if (arginp == (uchar_t *) 1 || onelflg == 1) {
			if (wanteof)
				return (-1);
			exitstat();
		}
		if (arginp) {
			if ((c = *arginp++) == 0) {
				arginp = (uchar_t *) 1;
				return ('\n');
			}
			return (c);
		}
reread:
		c = bgetc();
		if (c < 0) {
			struct sgttyb tty;

			if (wanteof)
				return (-1);
                        /* was isatty but raw with ignoreeof yields problems */
                        if (ioctl(SHIN, TIOCGETP, &tty)==0 &&
                            (tty.sg_flags & RAW) == 0) {

				pid_t ctpgrp;

				if (++sincereal > 25)
					goto oops;
				if (tpgrp != -1 &&
				    ioctl(FSHTTY, TIOCGPGRP, &ctpgrp) == 0 &&
				    tpgrp != ctpgrp) {
					struct sigvec nsv, osv;
		
					nsv.sv_handler = SIG_IGN;
					nsv.sv_mask = SA_RESTART;
					nsv.sv_onstack = 0;
					(void)sigvec(SIGTTOU, &nsv, &osv);
					ioctl(FSHTTY, TIOCSPGRP, &tpgrp);
					(void)sigvec(SIGTTOU, &osv, (struct sigvec *)0);
					/*killpg(ctpgrp, SIGHUP);*/
printf(MSGSTR(M_RESET,"Reset tty pgrp from %d to %d\n"), ctpgrp, tpgrp);
					goto reread;
				}
				if (adrof("ignoreeof")) {
					if (loginsh)
						printf(MSGSTR(M_LOGOUT,"\nUse \"logout\" to logout.\n"));
					else
						printf(MSGSTR(M_EXIT,"\nUse \"exit\" to leave csh.\n"));
					reset();
				}
				if (chkstop == 0)
					panystop(1);
			}
oops:
			doneinp = 1;
			reset();
		}
		sincereal = 0;
		if (c == '\n' && onelflg)
			onelflg--;
	} while (c == 0);
	return (c);
}

bgetc()
{
	register int buf, off, c;
	int flags;
#ifdef FILEC
	uchar_t ttyline[BUFSIZ];
	register int numleft = 0, roomleft;
#endif

#ifdef TELL
	if (cantell) {
		if (fseekp < fbobp || fseekp > feobp) {
			fbobp = feobp = fseekp;
			lseek(SHIN, fseekp, 0);
		}
		if (fseekp == feobp) {
			fbobp = feobp;
			do
				c = read(SHIN, fbuf[0], BUFSIZ);
			while (c < 0 && errno == EINTR);
			if (c <= 0)
				return (-1);
			feobp += c;
		}
		c = fbuf[0][fseekp - fbobp];
		fseekp++;
		return (c);
	}
#endif
again:
	buf = (int) fseekp / BUFSIZ;
	if (buf >= fblocks) {
		register uchar_t **nfbuf = (uchar_t **) calloc(fblocks+2, sizeof (uchar_t **));

		if (fbuf) {
			blkcpy(nfbuf, fbuf);
			xfree((char *)fbuf);
		}
		fbuf = (uchar_t **)nfbuf;
		fbuf[fblocks] = (uchar_t *)calloc(BUFSIZ, sizeof (uchar_t));
		fblocks++;
		goto again;
	}
	if (fseekp >= feobp) {
		buf = (int) feobp / BUFSIZ;
		off = (int) feobp % BUFSIZ;
#ifndef FILEC
		for (;;) {
			c = read(SHIN, fbuf[buf] + off, BUFSIZ - off);
#else
		roomleft = BUFSIZ - off;
		for (;;) {
#ifdef CMDEDIT
                        if ((cmdedit || filec) && intty) {
                                c = numleft ? numleft :
                                        cmdedit ? ed_tenex(ttyline, BUFSIZ) :
                                                        tenex(ttyline, BUFSIZ);
#else
			if (filec && intty) {
				c = numleft ? numleft : tenex(ttyline, BUFSIZ);
#endif
				if (c > roomleft) {
					/* start with fresh buffer */
					feobp = fseekp = fblocks * BUFSIZ;
					numleft = c;
					goto again;
				}
				if (c > 0)
					copy(fbuf[buf] + off, ttyline, c);
				numleft = 0;
			} else
				c = read(SHIN, fbuf[buf] + off, roomleft);
#endif
			if (c >= 0)
				break;
			if (errno == EWOULDBLOCK) {
				int	flags;

				flags = fcntl(SHIN, F_GETFL, 0) | O_NDELAY;	/* set no bloacking */
				fcntl(SHIN, F_SETFL, 0);
			} else if (errno != EINTR)
				break;
		}
		if (c <= 0)
			return (-1);
		feobp += c;
#ifndef FILEC
		goto again;
#else
#ifdef CMDEDIT
                if ((cmdedit || filec) && !intty)
#else
                if (filec && !intty)
#endif
			goto again;
#endif
	}
	c = fbuf[buf][(int) fseekp % BUFSIZ];
	fseekp++;
	return (c);
}

bfree()
{
	register int sb, i;

#ifdef TELL
	if (cantell)
		return;
#endif
	if (whyles)
		return;
	sb = (int) (fseekp - 1) / BUFSIZ;
	if (sb > 0) {
		for (i = 0; i < sb; i++)
			xfree(fbuf[i]);
		blkcpy(fbuf, &fbuf[sb]);
		fseekp -= BUFSIZ * sb;
		feobp -= BUFSIZ * sb;
		fblocks -= sb;
	}
}

bseek(l)
	long l;
{
	register struct whyle *wp;

	fseekp = l;
#ifdef TELL
	if (!cantell) {
#endif
		if (!whyles)
			return;
		for (wp = whyles; wp->w_next; wp = wp->w_next)
			continue;
		if (wp->w_start > l)
			l = wp->w_start;
#ifdef TELL
	}
#endif
}

/* any similarity to bell telephone is purely accidental */
long
btell()
{

	return (fseekp);
}

btoeof()
{

	lseek(SHIN, 0l, 2);
	fseekp = feobp;
	wfree();
	bfree();
}

#ifdef TELL
settell()
{

	cantell = 0;
	if (arginp || onelflg || intty)
		return;
	if (lseek(SHIN, 0l, 1) < 0 || errno == ESPIPE)
		return;
	fbuf = calloc(2, sizeof (char **));
	fblocks = 1;
	fbuf[0] = calloc(BUFR_SIZ, sizeof (char));
	fseekp = fbobp = feobp = tell(SHIN);
	cantell = 1;
}
#endif
