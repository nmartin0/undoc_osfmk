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
static char *rcsid = "@(#)$RCSfile: ex_re.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:01 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_re.c
 *
 * FUNCTION: advance, cclass, cerror, compile, comprhs, compsub, confirmed,
 * dosub, dosubcon, execute, fixcase, getsub, global, inrange, place,
 * re_classcode, same, snote, substitute, ugo
 *
 * ORIGINS: 3, 10, 13, 26, 27
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
 * ex_re.c  1.6  com/cmd/edit/vi,3.1,9013 2/20/90 17:03:11
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_re.h"

static void cerror();

#ifndef _NO_PROTO
int _NCxcolu(int descptr, wchar_t **src, wchar_t **xstr, wchar_t *uval);
wchar_t colval(int c);
#endif
/*
 * Global, substitute and regular expressions.
 * Very similar to ed, with some re extensions and
 * confirmed substitute.
 */
global(k)
	short k;
{
	register wchar_t *gp;
	register int c;
	register line *a1;
	wchar_t globuf[GBSIZE];
	char *Cwas;
	int nlines = lineDOL();
	int oinglobal = inglobal;
	wchar_t *oglobp = globp;
	Cwas = Command;
	/*
	 * States of inglobal:
	 *  0: ordinary - not in a global command.
	 *  1: text coming from some buffer, not tty.
	 *  2: like 1, but the source of the buffer is a global command.
	 * Hence you're only in a global command if inglobal==2. This
	 * strange sounding convention is historically derived from
	 * everybody simulating a global command.
	 */
	if (inglobal==2)
		error(msg(M_111, "Global within global@not allowed"));
	markDOT();
	setall();
	nonzero();
	if (skipend())
		error(msg(M_112, "Global needs re|Missing regular expression for global"));
	c = ex_getchar();
	ignore(compile(c, 1));
	savere(scanre);
	gp = globuf;
	while ((c = ex_getchar()) != '\n') {
		switch (c) {

		case EOF:
			c = '\n';
			goto brkwh;

		case '\\':
			c = ex_getchar();
			switch (c) {

			case '\\':
				ungetchar(c);
				break;

			case '\n':
				break;

			default:
				*gp++ = '\\';
				break;
			}
			break;
		}
		*gp++ = c;
		if (gp >= &globuf[GBSIZE - 2])
			error(msg(M_113, "Global command too long"));
	}
brkwh:
	ungetchar(c);
	donewline();
	*gp++ = c;
	*gp++ = 0;
	saveall();
	inglobal = 2;
	for (a1 = one; a1 <= dol; a1++) {
		*a1 &= ~01;
		if (a1 >= addr1 && a1 <= addr2 && execute(0, a1) == k)
			*a1 |= 01;
	}
	if (inopen)
		inopen = -1;
	/*
	 * Now for each marked line, set dot there and do the commands.
	 * Note the n^2 behavior here for lots of lines matching.
	 * This is really needed: in some cases you could delete lines,
	 * causing a marked line to be moved before a1 and missed if
	 * we didn't restart at zero each time.
	 */
	for (a1 = one; a1 <= dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			commands(1, 1);
			a1 = zero;
		}
	}
	globp = oglobp;
	inglobal = oinglobal;
	endline = 1;
	Command = Cwas;
	netchHAD(nlines);
	setlastchar(EOF);
	if (inopen) {
		ungetchar(EOF);
		inopen = 1;
	}
}

short	cflag;
int	scount, slines, stotal;

substitute(c)
	int c;
{
	register line *addr;
	register int n;
	int gsubf, hopcount;

	gsubf = compsub(c);
	if(FIXUNDO)
		save12(), undkind = UNDCHANGE;
	stotal = 0;
	slines = 0;
	for (addr = addr1; addr <= addr2; addr++) {
		scount = hopcount = 0;
		if (dosubcon(0, addr) == 0)
			continue;
		if (gsubf) {
			/*
			 * The loop can happen from s/\</&/g
			 * but we don't want to break other, reasonable cases.
			 */
			hopcount = 0;
			while (*loc2) {
				if (++hopcount > sizeof linebuf)
					error(msg(M_114, "substitution loop"));
				if (dosubcon(1, addr) == 0)
					break;
			}
		}
		if (scount) {
			stotal += scount;
			slines++;
			putmark(addr);
			n = append(getsub, addr);
			addr += n;
			addr2 += n;
		}
	}
	if (stotal == 0 && !inglobal && !cflag)
		error(msg(M_115, "Fail|Substitute pattern match failed"));
	snote(stotal, slines);
	return (stotal);
}

compsub(ch)
{
	register int seof, c, uselastre;
	static int gsubf;

	if (!value(EDCOMPATIBLE))
		gsubf = cflag = 0;
	uselastre = 0;
	switch (ch) {

	case 's':
		ignore(skipwh());
		seof = ex_getchar();
		if (endcmd(seof) || any(seof, "gcr")) {
			ungetchar(seof);
			goto redo;
		}
		if (NCisalnum(seof))
			error(msg(M_116, "Substitute needs re|Missing regular expression for substitute"));
		seof = compile(seof, 1);
		uselastre = 1;
		comprhs(seof);
		gsubf = 0;
		cflag = 0;
		break;

	case '~':
		uselastre = 1;
		/* fall into ... */
	case '&':
	redo:
		if (re.Expbuf[0] == 0)
			error(msg(M_117, "No previous re|No previous regular expression"));
		if (subre.Expbuf[0] == 0)
			error(msg(M_118, "No previous substitute re|No previous substitute to repeat"));
		break;
	}
	for (;;) {
		c = ex_getchar();
		switch (c) {

		case 'g':
			gsubf = !gsubf;
			continue;

		case 'c':
			cflag = !cflag;
			continue;

		case 'r':
			uselastre = 1;
			continue;

		default:
			ungetchar(c);
			setcount();
			donewline();
			if (uselastre)
				savere(subre);
			else
				resre(subre);
			return (gsubf);
		}
	}
}

comprhs(seof)
	int seof;
{
	register wchar_t *rp, *orp;
	register int c;
	wchar_t orhsbuf[RHSSIZE];

	rp = rhsbuf;
	CP(orhsbuf, rp);
	for (;;) {
		c = ex_getchar();
		if (c == seof)
			break;
		switch (c) {

		case '\\':
			c = ex_getchar();
			if (c == EOF) {
				ungetchar(c);
				break;
			}
			if (value(MAGIC)) {
				/*
				 * When "magic", \& turns into a plain &,
				 * and all other chars work fine quoted.
				 */
				if (c != '&')
					*rp++ = '\\';
				break;
			}
magic:
			if (c == '~') {
				for (orp = orhsbuf; *orp; *rp++ = *orp++)
					if (rp >= &rhsbuf[RHSSIZE - 1])
						goto toobig;
				continue;
			}
			*rp++ = '\\';
			break;

		case '\n':
		case EOF:
			if (!(globp && globp[0])) {
				ungetchar(c);
				goto endrhs;
			}

		case '~':
		case '&':
			if (value(MAGIC))
				goto magic;
			break;
		}
		if (rp >= &rhsbuf[RHSSIZE - 1]) {
toobig:
			*rp = 0;
			error(msg(M_119, "Replacement pattern too long@- limit 256 characters"));
		}
		*rp++ = c;
	}
endrhs:
	*rp++ = 0;
}

getsub()
{
	register wchar_t *p;

	if ((p = linebp) == 0)
		return (EOF);
	strcLIN(p);
	linebp = 0;
	return (0);
}

dosubcon(f, a)
	short f;
	line *a;
{

	if (execute(f, a) == 0)
		return (0);
	if (confirmed(a)) {  
		dosub();
		scount++;
	}
	return (1);
}

#define YNSIZ	32
confirmed(a)
	line *a;
{
	register int c;
	char ynbuf[YNSIZ];
	char *ynptr;

	if (cflag == 0)
		return (1);
	pofix();
	pline(lineno(a));
	if (inopen) {
		ex_putchar(QUOTE_NL);
	}
	c = column(loc1 - 1);
	ugo(c - 1 + (inopen ? 1 : 0), ' ');
	ugo(column(loc2 - 1) - c, '^');
	flush();
	ynptr = ynbuf;
	while ((c = getkey()) != EOF && c != '\n' && c != '\r') {
		if (ynptr < &ynbuf[YNSIZ-2]) {
			NLsputc(ynptr, c);
			if (inopen) {
				ex_putchar(c);
				flush();
			}
		}
	}
	if (c == '\r')
		c = '\n';
	if (inopen) {
		ex_putchar(c);
		flush();
	}
	*ynptr = '\0';
	noteinp();
	return (NLyesno(ynbuf) == 1);
}
ugo(cnt, with)
	int with;
	int cnt;
{

	if (cnt > 0)
		do
			ex_putchar(with);
		while (--cnt > 0);
}

int	casecnt;
short	destuc;

dosub()
{
	register wchar_t *lp, *sp, *rp;
	int c;

	lp = linebuf;
	sp = genbuf;
	rp = rhsbuf;
	while (lp < loc1)
		*sp++ = *lp++;
	casecnt = 0;
	while (c = *rp++) {
		/* ^V <return> from vi to split lines */
		if (c == '\r')
			c = '\n';

		if (c == '\\') {
			c = *rp++;
			switch (c) {

			case '&':
				sp = place(sp, loc1, loc2);
				if (sp == 0)
					goto ovflo;
				continue;

			case 'l':
				casecnt = 1;
				destuc = 0;
				continue;

			case 'L':
				casecnt = LBSIZE;
				destuc = 0;
				continue;

			case 'u':
				casecnt = 1;
				destuc = 1;
				continue;

			case 'U':
				casecnt = LBSIZE;
				destuc = 1;
				continue;

			case 'E':
			case 'e':
				casecnt = 0;
				continue;
			}
			if (c >= '1' && c < nbra + '1') {
				sp = place(sp, braslist[c - '1'], braelist[c - '1']);
				if (sp == 0)
					goto ovflo;
				continue;
			}
		}
		if (casecnt)
			c = fixcase(c);
		*sp++ = c;
/*!
 * Here and elsewhere, we are checking against LBSIZE-1 because we sometimes
 * insert two characters at once without checking for overflow inbetween.
!*/
		if (sp >= &genbuf[LBSIZE - 1])
ovflo:
			error(msg(M_120, "Line overflow@in substitute"));
	}
	lp = loc2;
	loc2 = sp + (linebuf - genbuf);
	while (*sp++ = *lp++)
		if (sp >= &genbuf[LBSIZE])
			goto ovflo;
	strcLIN(genbuf);
}

fixcase(c)
	register int c;
{

	if (casecnt == 0)
		return (c);
	casecnt--;
	if (destuc) {
		if (NCislower(c))
			c = _NCtoupper(c);
	} else
		if (NCisupper(c))
			c = _NCtolower(c);
	return (c);
}

wchar_t * place(sp, l1, l2)
	register wchar_t *sp, *l1, *l2;
{

	while (l1 < l2) {
		*sp++ = fixcase(*l1++);
		if (sp >= &genbuf[LBSIZE])
			return (0);
	}
	return (sp);
}

snote(total, nlines)
	register int total, nlines;
{

	if (!notable(total))
		return;
	ex_printf(mesg(msg(M_121, "%d subs|%d substitutions")), total);
	if (nlines != 1 && nlines != total)
		ex_printf(msg(M_122, " on %d lines"), nlines);
	noonl();
	flush();
}

static wchar_t re_classcode(id)
	wchar_t *id;
{
	struct recc { char *cl_name; wchar_t cl_code; };
	static struct recc a[] = {
		{"alpha",   CL_ALPHA},
		{"upper",   CL_UPPER},
		{"lower",   CL_LOWER},
		{"digit",   CL_DIGIT},
		{"xdigit",  CL_XDIGIT},
		{"alnum",   CL_ALNUM},
		{"space",   CL_SPACE},
		{"punct",   CL_PUNCT},
		{"print",   CL_PRINT},
		{"graph",   CL_GRAPH},
		{"cntrl",   CL_CNTRL},
#if defined(KJI)
		{"jalpha",  CL_JALPHA},
		{"jdigit",  CL_JDIGIT},
		{"jspace",  CL_JSPACE},
		{"jpunct",  CL_JPUNCT},
		{"jparen",  CL_JPAREN},
		{"jkanji",  CL_JKANJI},
		{"jhira",   CL_JHIRA},
		{"jkata",   CL_JKATA},
		{"jxdigit", CL_JXDIGIT},
#endif
	};
	register struct recc *p;
	for (p = &a[0]; p < &a[sizeof(a)/sizeof(a[0])]; ++p) {
		register wchar_t *s = id;
		register char *t = p->cl_name;
		while (*s++ == *t) {
			if (*t++ == '\0') return (p->cl_code);
		}
	}
	return (CL_BADCLASS);
}

compile(eof, oknl)
	int eof;
	int oknl;
{
	register int c;
	register wchar_t *ep;
	wchar_t *lastep;
	wchar_t bracket[NBRA], *bracketp, *rhsp;
	wchar_t *lastcp, *lastkp;
	if (NCisalnum(eof))
		error(msg(M_123, "Regular expressions cannot be delimited by letters or digits"));
	ep = expbuf;
	c = ex_getchar();
	if (eof == '\\')
		switch (c) {

		case '/':
		case '?':
			if (scanre.Expbuf[0] == 0)
error(msg(M_124, "No previous scan re|No previous scanning regular expression"));
			resre(scanre);
			return (c);

		case '&':
			if (subre.Expbuf[0] == 0)
error(msg(M_118, "No previous substitute re|No previous substitute regular expression"));
			resre(subre);
			return (c);

		default:
			error(msg(M_126, "Badly formed re|Regular expression \\ must be followed by / or ?"));
		}
	if (c == eof || c == '\n' || c == EOF) {
		if (*ep == 0)
			error(msg(M_117, "No previous re|No previous regular expression"));
		if (c == '\n' && oknl == 0)
			error(msg(M_128, "Missing closing delimiter@for regular expression"));
		if (c != eof)
			ungetchar(c);
		return (eof);
	}
	bracketp = bracket;
	nbra = 0;
	circfl = 0;
	if (c == '^') {
		c = ex_getchar();
		circfl++;
	}
	ungetchar(c);
	for (;;) {
		if (ep >= &expbuf[ESIZE - 2])
complex:
			cerror(msg(M_129, "Re too complex|Regular expression too complicated"));
		c = ex_getchar();
		if (c == eof || c == EOF) {
			if (bracketp != bracket)
cerror(msg(M_130, "Unmatched \\(|More \\('s than \\)'s in regular expression"));
			*ep++ = CEOFC;
			if (c == EOF)
				ungetchar(c);
			return (eof);
		}
		if (value(MAGIC)) {
			if (c != '*' || ep == expbuf)
				lastep = ep;
		} else
			if (c != '\\' || peekchar() != '*' || ep == expbuf)
				lastep = ep;
		switch (c) {

		case '\\':
			c = ex_getchar();
			switch (c) {

			case '(':
				if (nbra >= NBRA)
cerror(msg(M_131, "Awash in \\('s!|Too many \\('d subexressions in a regular expression"));
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;

			case ')':
				if (bracketp <= bracket)
cerror(msg(M_132, "Extra \\)|More \\)'s than \\('s in regular expression"));
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;

			case '<':
				*ep++ = CBRC;
				continue;

			case '>':
				*ep++ = CLET;
				continue;
			}
			if (value(MAGIC) == 0)
magic:
			switch (c) {

			case '.':
				*ep++ = CDOT;
				continue;

			case '~':
				rhsp = rhsbuf;
				while (*rhsp) {
					c = *rhsp++;
					if (c == '\\') {
						c = *rhsp++;
						if (c == '&')
error(msg(M_133, "Replacement pattern contains &@- cannot use in re"));
						if (c >= '1' && c <= '9')
error(msg(M_134, "Replacement pattern contains \\d@- cannot use in re"));
					}
					if (ep >= &expbuf[ESIZE-2])
						goto complex;
					*ep++ = CCHR;
					*ep++ = c;
				}
				continue;

			case '*':
				if (ep == expbuf)
					break;
				if (*lastep == CBRA || *lastep == CKET)
cerror(msg(M_135, "Illegal *|Can't * a \\( ... \\) in regular expression"));
				*lastep |= STAR;
				continue;

			case '[':
				if (ep >= &expbuf[ESIZE-2]) goto complex;
				*ep++ = CCL;
				*ep++ = 0;
				lastcp = lastkp = NULL;
				c = ex_getchar();
				if (c == '^') {
					c = ex_getchar();
					ep[-2] = NCCL;
				}
				if (c == ']')
cerror(msg(M_137, "Bad character class|Empty character class '[]' or '[^]' cannot match"));
				while (c != ']') {
					if (c == '-' && lastcp != NULL && peekchar() != ']') {
						c = *lastcp;
						ep = lastcp;
						if (ep >= &expbuf[ESIZE-4]) goto complex;
						*ep++ = '\\';
						*ep++ = CL_RANGE;
						*ep++ = c;
						if ((c = ex_getchar()) == '\\' && any(peekchar(), "]-^\\")) c = ex_getchar();
						*ep++ = c;
						lastcp = NULL;
					} else {
						if (c == '\\' && any(peekchar(), "]-^\\")) c = ex_getchar();
						if (c == '\n' || c == EOF)
							cerror(msg(M_138, "Missing ]"));
						lastcp = ep;
						if (c == ':' && lastkp != NULL && peekchar() == ']') {
							*ep = '\0';
							if ((c = re_classcode(lastkp)) == CL_BADCLASS)
cerror(msg(M_252, "Unrecognized ctype@class in [: ... :] construct"));
							(void)ex_getchar();
							ep = lastkp - 2;
							*ep++ = '\\';
							lastcp = lastkp = NULL;
						}
						if (ep >= &expbuf[ESIZE-2]) goto complex;
						*ep++ = c;
						if (c == '\\') *ep++ = CL_BACKSLASH;
						if (c == '[' && peekchar() == ':') {
							lastcp = ep;
							*ep++ = ex_getchar();
							lastkp = ep;
						}
					}
					c = ex_getchar();
				}
				lastep[1] = ep - &lastep[1];
				continue;
			}
			if (c == EOF) {
				ungetchar(EOF);
				c = '\\';
				goto defchar;
			}
			*ep++ = CCHR;
			if (c == '\n')
cerror(msg(M_139, "No newlines in re's|Can't escape newlines into regular expressions"));
			*ep++ = c;
			continue;

		case '\n':
			if (oknl) {
				ungetchar(c);
				*ep++ = CEOFC;
				return (eof);
			}
cerror(msg(M_140, "Badly formed re|Missing closing delimiter for regular expression"));

		case '$':
			if (peekchar() == eof || peekchar() == EOF || oknl && peekchar() == '\n') {
				*ep++ = CDOL;
				continue;
			}
			goto defchar;

		case '.':
		case '~':
		case '*':
		case '[':
			if (value(MAGIC))
				goto magic;
defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
			continue;
		}
	}
}

static void cerror(s)
	char *s;
{

	expbuf[0] = 0;
	error(s);
}

same(a, b)
	register int a, b;
{

	return (a == b || value(IGNORECASE) &&
	   ((NCislower(a) && _NCtoupper(a) == b) || (NCislower(b) && _NCtoupper(b) == a)));
}

wchar_t	*locs;

execute(gf, addr)
	line *addr;
{
	register wchar_t *p1, *p2;
	register int c;

	if (gf) {
		if (circfl)
			return (0);
		locs = p1 = loc2;
	} else {
		if (addr == zero)
			return (0);
		p1 = linebuf;
		getline(*addr);
		locs = 0;
	}
	p2 = expbuf;
	if (circfl) {
		loc1 = p1;
		return (advance(p1, p2));
	}
	/* fast check for first character */
	if (*p2 == CCHR) {
		c = p2[1];
		do {
			if (c != *p1 && (!value(IGNORECASE) ||
			   !((NCislower(c) && _NCtoupper(c) == *p1) ||
			   (NCislower(*p1) && _NCtoupper(*p1) == c))))
				continue;
			if (advance(p1, p2)) {
				loc1 = p1;
				return (1);
			}
		} while (*p1++);
		return (0);
	}
	/* regular algorithm */
	do {
		if (advance(p1, p2)) {
			loc1 = p1;
			return (1);
		}
	} while (*p1++);
	return (0);
}


advance(lp, ep)
	wchar_t *lp, *ep;
{
	register wchar_t *curlp;

	for (;;) switch (*ep++) {

	case CCHR:
		if (!same(*ep, *lp))
			return (0);
		ep++, lp++;
		continue;

	case CDOT:
		if (*lp++)
			continue;
		return (0);

	case CDOL:
		if (*lp == 0)
			continue;
		return (0);

	case CEOFC:
		loc2 = lp;
		return (1);

	case CCL:
		if (cclass(ep, &lp, 1)) {
			ep += *ep;
			continue;
		}
		return (0);

	case NCCL:
		if (cclass(ep, &lp, 0)) {
			ep += *ep;
			continue;
		}
		return (0);

	case CBRA:
		braslist[*ep++] = lp;
		continue;

	case CKET:
		braelist[*ep++] = lp;
		continue;

	case CDOT|STAR:
		curlp = lp;
		while (*lp++)
			continue;
		goto star;

	case CCHR|STAR:
		curlp = lp;
		while (same(*lp, *ep))
			lp++;
		lp++;
		ep++;
		goto star;

	case CCL|STAR:
	case NCCL|STAR:
		curlp = lp;
		while (cclass(ep, &lp, ep[-1] == (CCL|STAR)))
			continue;
		ep += *ep;
		goto star;
star:
		do {
			lp--;
			if (lp == locs)
				break;
			if (advance(lp, ep))
				return (1);
		} while (lp > curlp);
		return (0);

	case CBRC:
		if (lp == linebuf)
			continue;
		if (!wordch(&lp[-1]) && wordch(lp))
			continue;
		return (0);

	case CLET:
		if (!wordch(lp))
			continue;
		return (0);

	default:
		error(msg(M_141, "Re internal error"));
	}
}

static int
inrange(c, t, lo, hi) 
int c; 
wchar_t **t;
wchar_t lo, hi;
{
        int cc;
        wchar_t tc;
        wchar_t xt[2];
        wchar_t *pxt, *qxt;
        wchar_t *pt = *t;       /* save input */
        int ulo = colval(lo);   /* get colval for boundary */
        int uhi = colval(hi);
        wchar_t uc = NCcoluniq(c);      /* get unique for subject character */
 
        if ((cc = NCcollate(c)) < 0)
                        cc = _NCxcolu(cc, &pt, (wchar_t **)0, &uc);

	/* check if outside range and ignorecase is set */
	if (value(IGNORECASE) && (uc < ulo || uc > uhi)) {
            if ((tc = NCtoupper(c)) != c) {
                uc=NCcoluniq(tc);       /* get uniq for the other case */
                xt[0] = tc;             /* store in string */
                xt[1] = 0;
                pxt = qxt = xt;
                if ((cc = NCcollate(tc)) < 0)
                        cc = _NCxcolu(cc, &pxt, (wchar_t **)0, &uc);
                if (cc = ((uc >= ulo && uc <= uhi)))  /*found? */
                        if (pxt != qxt) *t++;
                return (cc);
	    }
            else if ((tc = NCtolower(c)) != c) {
                uc=NCcoluniq(tc);       /* get uniq for the other case */
                xt[0] = tc;             /* store in string */
                xt[1] = 0;
                pxt = qxt = xt;
                if ((cc = NCcollate(tc)) < 0)
                        cc = _NCxcolu(cc, &pxt, (wchar_t **)0, &uc);
                if (cc = (uc >= ulo && uc <= uhi))      /*found? */
                        if (pxt != qxt) *t++;
                return (cc);
	    }
	}
        if (cc = (uc >= ulo && uc <= uhi))
                        *t = pt;
        return (cc);
}

wchar_t colval(c)
int c;
{
      wchar_t ifbuf[3];
      wchar_t *ib;
      int cv;
      wchar_t uv;

      ib = ifbuf;
      if (c > 256)
              *ib++ = (c >> 8);
      *ib++ = (c & 0xff);
      *ib = '\0';
      uv = NCcoluniq(c);
      if ((cv= NCcollate(c)) < 0)
              cv = _NCxcolu(cv, &ib, (wchar_t **)0, &uv);
      return (uv);
}

cclass(set, s, af)
      register wchar_t *set;
      wchar_t **s;
      int af;
{
        register int c;
        register wchar_t *endset;
        wchar_t *t = *s;

        c = *t++;
        *s = t;
	if (c == 0)
		return (0);
	endset = set;
	endset += (int)*set++;
	while (set < endset) {
		if (*set != '\\') {
			if (c == *set++) return (af);
		} else {
			++set;
			switch (*set++) {
			case CL_BACKSLASH:
				if (c == '\\') return (af);
				break;
			case CL_RANGE:
                                if (inrange(c, &t, set[0], set[1])) {
                                        *s = t;
                                        return (af);
                                }
				set += 2;
				break;
			case CL_ALPHA:
				if (NCisalpha(c)) return (af);
				break;
			case CL_UPPER:
				if (NCisupper(c)) return (af);
				break;
			case CL_LOWER:
				if (NCislower(c)) return (af);
				break;
			case CL_DIGIT:
				if (NCisdigit(c)) return (af);
				break;
			case CL_XDIGIT:
				if (NCisxdigit(c)) return (af);
				break;
			case CL_ALNUM:
				if (NCisalnum(c)) return (af);
				break;
			case CL_SPACE:
				if (NCisspace(c)) return (af);
				break;
			case CL_PUNCT:
				if (NCispunct(c)) return (af);
				break;
			case CL_PRINT:
				if (NCisprint(c)) return (af);
				break;
			case CL_GRAPH:
				if (NCisgraph(c)) return (af);
				break;
			case CL_CNTRL:
				if (NCiscntrl(c)) return (af);
				break;
#if defined(KJI)
			case CL_JALPHA:
				if (isjalpha(c)) return (af);
				break;
			case CL_JDIGIT:
				if (isjdigit(c)) return (af);
				break;
			case CL_JSPACE:
				if (isjspace(c)) return (af);
				break;
			case CL_JPUNCT:
				if (isjpunct(c)) return (af);
				break;
			case CL_JPAREN:
				if (isjparen(c)) return (af);
				break;
			case CL_JKANJI:
				if (isjkanji(c)) return (af);
				break;
			case CL_JHIRA:
				if (isjhira(c)) return (af);
				break;
			case CL_JKATA:
				if (isjkata(c)) return (af);
				break;
			case CL_JXDIGIT:
				if (isjxdigit(c)) return (af);
				break;
#endif
			default:
				error(msg(M_253, "Internal regexp error in cclass"));
			}
		}
	}
	return (!af);
}
