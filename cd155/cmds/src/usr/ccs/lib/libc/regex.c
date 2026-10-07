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
static char rcsid[] = "@(#)$RCSfile: regex.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:42 $";
#endif
/*
 * FUNCTIONS: re_comp, re_exec
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * regex.c	1.3  com/lib/c/gen,3.1,8943 10/16/89 14:00:32
 */

#ifdef MSG
#include "libc_msg.h"
#endif

#ifdef _THREAD_SAFE
#include <regex.h>
#endif

/*
 * FUNCTION: regular expression matching
 *
 */

/*
 * routines to do regular expression matching
 *
 * Entry points:
 *
 *	re_comp(s)
 *		char *s;
 *	 ... returns 0 if the string s was compiled successfully,
 *		     a pointer to an error message otherwise.
 *	     If passed 0 or a null string returns without changing
 *           the currently compiled re (see note 11 below).
 *
 *	re_exec(s)
 *		char *s;
 *	 ... returns 1 if the string s matches the last compiled regular
 *		       expression, 
 *		     0 if the string s failed to match the last compiled
 *		       regular expression, and
 *		    -1 if the compiled regular expression was invalid 
 *		       (indicating an internal error).
 *
 * The strings passed to both re_comp and re_exec may have trailing or
 * embedded newline characters; they are terminated by nulls.
 *
 * The identity of the author of these routines is lost in antiquity;
 * this is essentially the same as the re code in the original V6 ed.
 *
 * The regular expressions recognized are described below. This description
 * is essentially the same as that for ed.
 *
 *	A regular expression specifies a set of strings of characters.
 *	A member of this set of strings is said to be matched by
 *	the regular expression.  In the following specification for
 *	regular expressions the word `character' means any character but NUL.
 *
 *	1.  Any character except a special character matches itself.
 *	    Special characters are the regular expression delimiter plus
 *	    \ [ . and sometimes ^ * $.
 *	2.  A . matches any character.
 *	3.  A \ followed by any character except a digit or ( )
 *	    matches that character.
 *	4.  A nonempty string s bracketed [s] (or [^s]) matches any
 *	    character in (or not in) s. In s, \ has no special meaning,
 *	    and ] may only appear as the first letter. A substring 
 *	    a-b, with a and b in ascending ASCII order, stands for
 *	    the inclusive range of ASCII characters.
 *	5.  A regular expression of form 1-4 followed by * matches a
 *	    sequence of 0 or more matches of the regular expression.
 *	6.  A regular expression, x, of form 1-8, bracketed \(x\)
 *	    matches what x matches.
 *	7.  A \ followed by a digit n matches a copy of the string that the
 *	    bracketed regular expression beginning with the nth \( matched.
 *	8.  A regular expression of form 1-8, x, followed by a regular
 *	    expression of form 1-7, y matches a match for x followed by
 *	    a match for y, with the x match being as long as possible
 *	    while still permitting a y match.
 *	9.  A regular expression of form 1-8 preceded by ^ (or followed
 *	    by $), is constrained to matches that begin at the left
 *	    (or end at the right) end of a line.
 *	10. A regular expression of form 1-9 picks out the longest among
 *	    the leftmost matches in a line.
 *	11. An empty regular expression stands for a copy of the last
 *	    regular expression encountered.
 */


/*
 * constants for re's
 */
#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	18

#define	CSTAR	01

#ifndef _THREAD_SAFE
#define	ESIZE	512
#define	NBRA	9

static	char	expbuf[ESIZE], *braslist[NBRA], *braelist[NBRA];
static	char	circf;
#endif

static	int backref();
static	int advance();


/*
 * compile the regular expression argument into a dfa
 */
#ifdef _THREAD_SAFE
char *
re_comp_r(char *sp, REGEXD *rd)
#else
char *
re_comp(sp)
register char	*sp;
#endif
{
	register int	c;
#ifdef  _THREAD_SAFE
	register char   *expbuf = rd->expbuf;
#endif
	register char	*ep = expbuf;
	int	cclcnt, numbra = 0;
	char	*lastep = 0;
	char	bracket[NBRA];
	char	*bracketp = &bracket[0];
#ifdef MSG
	static	char *retoolong;

	if (!retoolong)
		retoolong = NLgetamsg(MF_LIBC, MS_LIBC, M_RETOOLONG, 
				"Regular expression too long");
#else
	static	char *retoolong = "Regular expression too long";
#endif

#define	comerr(msg) {expbuf[0] = 0; numbra = 0; return(msg); }

	if (sp == 0 || *sp == '\0') {
		if (*ep == 0)
#ifdef MSG
			return(NLgetamsg(MF_LIBC, MS_LIBC, M_NOPREV, 
				"No previous regular expression"));
#else
			return("No previous regular expression");
#endif
		return(0);
	}
	if (*sp == '^') {
#ifdef _THREAD_SAFE
		rd->circf = 1;
#else
		circf = 1;
#endif
		sp++;
	}
	else
#ifdef _THREAD_SAFE
		rd->circf = 0;
#else
		circf = 0;
#endif
	for (;;) {
		if (ep >= &expbuf[ESIZE])
			comerr(retoolong);
		if ((c = *sp++) == '\0') {
			if (bracketp != bracket)
#ifdef MSG
				comerr(NLgetamsg(MF_LIBC, MS_LIBC, M_UNMATCH, 
					"unmatched \\("));
#else
				comerr("unmatched \\(");
#endif
			*ep++ = CEOF;
			*ep++ = 0;
			return(0);
		}
		if (c != '*')
			lastep = ep;
		switch (c) {

		case '.':
			*ep++ = CDOT;
			continue;

		case '*':
			if (lastep == 0 || *lastep == CBRA || *lastep == CKET)
				goto defchar;
			*lastep |= CSTAR;
			continue;

		case '$':
			if (*sp != '\0')
				goto defchar;
			*ep++ = CDOL;
			continue;

		case '[':
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c = *sp++) == '^') {
				c = *sp++;
				ep[-2] = NCCL;
			}
			do {
				if (c == '\0')
#ifdef MSG
					comerr(NLgetamsg(MF_LIBC, MS_LIBC, 
						M_MISSING, "missing ]"));
#else
					comerr("missing ]");
#endif
				if (c == '-' && ep [-1] != 0) {
					if ((c = *sp++) == ']') {
						*ep++ = '-';
						cclcnt++;
						break;
					}
					while (ep[-1] < c) {
						*ep = ep[-1] + 1;
						ep++;
						cclcnt++;
						if (ep >= &expbuf[ESIZE])
							comerr(retoolong);
					}
				}
				*ep++ = c;
				cclcnt++;
				if (ep >= &expbuf[ESIZE])
					comerr(retoolong);
			} while ((c = *sp++) != ']');
			lastep[1] = cclcnt;
			continue;

		case '\\':
			if ((c = *sp++) == '(') {
				if (numbra >= NBRA)
#ifdef MSG
					comerr(NLgetamsg(MF_LIBC, MS_LIBC, 
						M_TOOMANY, 
						"too many \\(\\) pairs"));
#else
					comerr("too many \\(\\) pairs");
#endif
				*bracketp++ = numbra;
				*ep++ = CBRA;
				*ep++ = numbra++;
				continue;
			}
			if (c == ')') {
				if (bracketp <= bracket)
#ifdef MSG
					comerr(NLgetamsg(MF_LIBC, MS_LIBC, 
						M_UNMATCHLEFT, "unmatched \\)"));
#else
					comerr("unmatched \\)");
#endif
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;
			}
			if (c >= '1' && c < ('1' + NBRA)) {
				*ep++ = CBACK;
				*ep++ = c - '1';
				continue;
			}
			*ep++ = CCHR;
			*ep++ = c;
			continue;

		defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
		}
	}
}

/* 
 * match the argument string against the compiled re
 */
int
#ifdef _THREAD_SAFE
re_exec_r(char *p1, REGEXD *rd)
#else
re_exec(p1)
register char	*p1;
#endif
{
#ifdef _THREAD_SAFE
	register char   *p2 = rd->expbuf;
	register char   **braslist = rd->braslist;
	register char   **braelist = rd->braelist;
#else
	register char	*p2 = expbuf;
#endif
	register int	c;
	int	rv;

	for (c = 0; c < NBRA; c++) {
		braslist[c] = 0;
		braelist[c] = 0;
	}
#ifdef _THREAD_SAFE
	if (rd->circf)
		return((advance(p1, p2, rd)));
#else
	if (circf)
		return((advance(p1, p2)));
#endif
	/*
	 * fast check for first character
	 */
	if (*p2 == CCHR) {
		c = p2[1];
		do {
			if (*p1 != c)
				continue;
#ifdef _THREAD_SAFE
			if (rv = advance(p1, p2, rd))
#else
			if (rv = advance(p1, p2))
#endif
				return(rv);
		} while (*p1++);
		return(0);
	}
	/*
	 * regular algorithm
	 */
	do
#ifdef _THREAD_SAFE
		if (rv = advance(p1, p2, rd))
#else
		if (rv = advance(p1, p2))
#endif
			return(rv);
	while (*p1++);
	return(0);
}

/* 
 * try to match the next thing in the dfa
 */
static	int
#ifdef _THREAD_SAFE
advance(char *lp, char *ep, REGEXD *rd)
#else
advance(lp, ep)
register char	*lp, *ep;
#endif
{
	register char	*curlp;
	int	ct, i;
	int	rv;
#ifdef _THREAD_SAFE
	register char	**braslist = rd->braslist;
	register char	**braelist = rd->braelist;
#endif

	for (;;)
		switch (*ep++) {

		case CCHR:
			if (*ep++ == *lp++)
				continue;
			return(0);

		case CDOT:
			if (*lp++)
				continue;
			return(0);

		case CDOL:
			if (*lp == '\0')
				continue;
			return(0);

		case CEOF:
			return(1);

		case CCL:
			if (cclass(ep, *lp++, 1)) {
				ep += *ep;
				continue;
			}
			return(0);

		case NCCL:
			if (cclass(ep, *lp++, 0)) {
				ep += *ep;
				continue;
			}
			return(0);

		case CBRA:
			braslist[*ep++] = lp;
			continue;

		case CKET:
			braelist[*ep++] = lp;
			continue;

		case CBACK:
			if (braelist[i = *ep++] == 0)
				return(-1);
#ifdef _THREAD_SAFE
			if (backref(i, lp, rd)) {
#else
			if (backref(i, lp)) {
#endif
				lp += braelist[i] - braslist[i];
				continue;
			}
			return(0);

		case CBACK|CSTAR:
			if (braelist[i = *ep++] == 0)
				return(-1);
			curlp = lp;
			ct = braelist[i] - braslist[i];
#ifdef _THREAD_SAFE
			while (backref(i, lp, rd))
#else
			while (backref(i, lp))
#endif
				lp += ct;
			while (lp >= curlp) {
#ifdef _THREAD_SAFE
				if (rv = advance(lp, ep, rd))
#else
				if (rv = advance(lp, ep))
#endif
					return(rv);
				lp -= ct;
			}
			continue;

		case CDOT|CSTAR:
			curlp = lp;
			while (*lp++)
				;
			goto star;

		case CCHR|CSTAR:
			curlp = lp;
			while (*lp++ == *ep)
				;
			ep++;
			goto star;

		case CCL|CSTAR:
		case NCCL|CSTAR:
			curlp = lp;
			while (cclass(ep, *lp++, ep[-1] == (CCL|CSTAR)))
				;
			ep += *ep;
			goto star;

		star:
			do {
				lp--;
#ifdef _THREAD_SAFE
				if (rv = advance(lp, ep, rd))
#else
				if (rv = advance(lp, ep))
#endif
					return(rv);
			} while (lp > curlp);
			return(0);

		default:
			return(-1);
		}
}

static
#ifdef _THREAD_SAFE
backref(int i, char *lp, REGEXD *rd)
#else
backref(i, lp)
register int	i;
register char	*lp;
#endif
{
	register char	*bp, *be;

#ifdef _THREAD_SAFE
	bp = rd->braslist[i];
	be = rd->braelist[i];
#else
	bp = braslist[i];
	be = braelist[i];
#endif
	while (*bp++ == *lp++)
		if (bp >= be)
			return(1);
	return(0);
}

int
cclass(set, c, af)
register char	*set, c;
int	af;
{
	register int	n;

	if (c == 0)
		return(0);
	n = *set++;
	while (--n)
		if (*set++ == c)
			return(af);
	return(! af);
}
