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
static char rcsid[] = "@(#)$RCSfile: set.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:21:44 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: doset getinx asx getvx dolet xset operate onlyread xfree 
 *            savestr putn putn1 getn value value1 adrof madrof adrof1 set 
 *            set1 setq unset unset1 unsetv unsetv1 setNS shift exportpath
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
 *
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

#include <locale.h>
#include "sh.h"

/*
 * C Shell
 */

doset(v)
	register uchar_t **v;
{
	register uchar_t *p;
	uchar_t *vp, op;
	uchar_t **vecp;
	bool hadsub;
	int subscr;
#ifdef KJI
	register int n;
	wchar_t nlc;
#endif

	v++;
	p = *v++;
	if (p == 0) {
		prvars();
		return;
	}
	do {
		hadsub = 0;
#ifdef KJI
		n = NCdec(p, &nlc);
		if (!letter(nlc))
			goto setsyn;	
		for (vp = p, p += n ; *p; p += n) {
			n = NCdec(p, &nlc);
			if (!alnum(nlc)) 
				break;
		}
#else
		if (!letter(*p))
			goto setsyn;
		for (vp = p; alnum(*p); p++)
			continue;
		if (vp == p)
			goto setsyn;
#endif
		if (*p == '[') {
			hadsub++;
			p = getinx(p, &subscr);
		}
		if (op = *p) {
			*p++ = 0;
			if (*p == 0 && *v && **v == '(')
				p = *v++;
		} else if (*v && eq(*v, "=")) {
			op = '=', v++;
			if (*v)
				p = *v++;
		}
		if (op && op != '=')
#ifdef KJI
			goto setsyn;
#else
setsyn:
			bferr(MSGSTR(M_SYNERR, "Syntax error"));
#endif
		if (eq(p, "(")) {
			register uchar_t **e = v;

			if (hadsub)
				goto setsyn;
			for (;;) {
				if (!*e)
					bferr(MSGSTR(M_MISSP, "Missing )"));
				if (**e == ')')
					break;
				e++;
			}
			p = *e;
			*e = 0;
			vecp = saveblk(v);
			set1(vp, vecp, &shvhed);
			*e = p;
			v = e + 1;
		} else if (hadsub)
			asx(vp, subscr, savestr(p));
		else
			set(vp, savestr(p));
		if (eq(vp, "path")) {
			exportpath(adrof("path")->vec);
			dohash();
		} else if (eq(vp, "histchars")) {
			register uchar_t *p = value("histchars");

#ifdef KJI
			p += NCdec (p, &HIST);
			NCdec (p, &HISTSUB);
#else
			HIST = *p++;
			HISTSUB = *p;
#endif
		} else if (eq(vp, "user"))
			setenv("USER", value(vp));
		else if (eq(vp, "term")) {
			setenv("TERM", value(vp));
#ifdef CMDEDIT
                        dosetupterm();
#endif
		}
		else if (eq(vp, "home"))
			setenv("HOME", value(vp));
                else if (eq(vp, "lang")) {
                        setenv("LANG", value(vp));
                        setlocale(LC_ALL, "");
                }
                else if (eq(vp, "lc_messages")) {
                        setenv("LC_MESSAGES", value(vp));
                        setlocale(LC_MESSAGES, (char *)value(vp));
                }
                else if (eq(vp, "lc_ctype")) {
                        setenv("LC_CTYPE", value(vp));
                        setlocale(LC_CTYPE, (char *)value(vp));
                }
                else if (eq(vp, "lc_collate")) {
                        setenv("LC_COLLATE", value(vp));
                        setlocale(LC_COLLATE, (char *)value(vp));
                }
#ifdef FILEC
		else if (eq(vp, "filec"))
			filec = 1;
#endif
#ifdef CMDEDIT
                else if (eq(vp, "editmode")) {
                        setenv("EDITMODE", value(vp));
                        if (eq(value(vp), "none")) {
                                cmdedit = 0;
                                continue;
                        }
                        cmdedit = 1;
                        dobindings(value(vp));
                        if (!eq(value(vp), "dumb") && !adrof("edithist"))
                                setNS("edithist");
                }
#endif
	} while (p = *v++);
#ifdef KJI
	return;
setsyn:
	bferr(MSGSTR(M_SYNERR, "Syntax error"));
#endif
}

uchar_t *
getinx(cp, ip)
	register uchar_t *cp;
	register int *ip;
{

	*ip = 0;
	*cp++ = 0;
	while (*cp && digit(*cp))
		*ip = *ip * 10 + *cp++ - '0';
	if (*cp++ != ']')
		bferr(MSGSTR(M_SUBERR, "Subscript error"));
	return (cp);
}

asx(vp, subscr, p)
	uchar_t *vp;
	int subscr;
	uchar_t *p;
{
	register struct varent *v = getvx(vp, subscr);

	xfree(v->vec[subscr - 1]);
	v->vec[subscr - 1] = globone(p);
}

struct varent *
getvx(vp, subscr)
{
	register struct varent *v = adrof(vp);

	if (v == 0)
		udvar(vp);
	if (subscr < 1 || subscr > blklen(v->vec))
		bferr(MSGSTR(M_SUBOUT, "Subscript out of range"));
	return (v);
}

uchar_t	plusplus[2] = { '1', 0 };


dolet(v)
	uchar_t **v;
{
	register uchar_t *p;
	uchar_t *vp, c, op;
	bool hadsub;
	int subscr;
#ifdef KJI
	register int n;
	wchar_t nlc;
#endif

	v++;
	p = *v++;
	if (p == 0) {
		prvars();
		return;
	}
	do {
		hadsub = 0;
#ifdef KJI
		n = NCdec(p, &nlc);
		if (!letter(nlc))
			goto letsyn;	
		for (vp = p ;*p; p += n) {
			n = NCdec(p, &nlc);
			if (!alnum(nlc))
				break;
		}
#else
		if (!letter(*p))
			goto letsyn;
		for (vp = p; alnum(*p); p++)
			continue;
#endif
		if (vp == p)
			goto letsyn;
		if (*p == '[') {
			hadsub++;
			p = getinx(p, &subscr);
		}
		if (*p == 0 && *v)
			p = *v++;
		if (op = *p)
			*p++ = 0;
		else
			goto letsyn;
		vp = savestr(vp);
		if (op == '=') {
			c = '=';
			p = xset(p, &v);
		} else {
			c = *p++;
			if (any(c, "+-")) {
				if (c != op || *p)
					goto letsyn;
				p = plusplus;
			} else {
				if (any(op, "<>")) {
					if (c != op)
						goto letsyn;
					c = *p++;
#ifdef KJI
					goto letsyn;
#else
letsyn:
					bferr(MSGSTR(M_SYNERR, "Syntax error"));
#endif
				}
				if (c != '=')
					goto letsyn;
				p = xset(p, &v);
			}
		}
		if (op == '=')
			if (hadsub)
				asx(vp, subscr, p);
			else
				set(vp, p);
		else
			if (hadsub)
#ifndef V6
				/* avoid bug in vax CC */
				{
					struct varent *gv = getvx(vp, subscr);

					asx(vp, subscr, operate(op, gv->vec[subscr - 1], p));
				}
#else
				asx(vp, subscr, operate(op, getvx(vp, subscr)->vec[subscr - 1], p));
#endif
			else
				set(vp, operate(op, value(vp), p));
		if (strcmp(vp, "path") == 0)
			dohash();
		xfree(vp);
		if (c != '=')
			xfree(p);
	} while (p = *v++);
#ifdef KJI
	return;
letsyn:
	bferr(MSGSTR(M_SYNERR, "Syntax error"));
#endif
}

uchar_t *
xset(cp, vp)
	uchar_t *cp, ***vp;
{
	register uchar_t *dp;

	if (*cp) {
		dp = savestr(cp);
		--(*vp);
		xfree(**vp);
		**vp = dp;
	}
	return (putn(exp(vp)));
}

uchar_t *
operate(op, vp, p)
	uchar_t op, *vp, *p;
{
	uchar_t opr[2];
	uchar_t *vec[5];
	register uchar_t **v = vec;
	uchar_t **vecp = v;
	register int i;

	if (op != '=') {
		if (*vp)
			*v++ = vp;
		opr[0] = op;
		opr[1] = 0;
		*v++ = opr;
		if (op == '<' || op == '>')
			*v++ = opr;
	}
	*v++ = p;
	*v++ = 0;
	i = exp(&vecp);
	if (*vecp)
		bferr(MSGSTR(M_EXPR, "Expression syntax"));
	return (putn(i));
}

onlyread(cp)
	uchar_t *cp;
{
	extern uchar_t end[];

	return (cp < end);
}

xfree(cp)
	uchar_t *cp;
{
	extern uchar_t end[];

	if (cp >= end && cp < (uchar_t *) &cp)
		cfree(cp);
}

uchar_t *
savestr(s)
	register uchar_t *s;
{
	register uchar_t *n;

	if (s == 0)
		s = (uchar_t *)"";
	strcpy(n = calloc(1, strlen(s) + 1), s);
	return (n);
}

static	uchar_t *putp;
 
uchar_t *
putn(n)
	register int n;
{
	static uchar_t number[15];

	putp = number;
	if (n < 0) {
		n = -n;
		*putp++ = '-';
	}
	if (sizeof (int) == 2 && n == -32768) {
		*putp++ = '3';
		n = 2768;
	} else if (sizeof (int) == 4 && n == (8<<28)) {
		*putp++ = '2';
		n = 147483648;
	}
	putn1(n);
	*putp = 0;
	return (savestr(number));
}

putn1(n)
	register int n;
{
	if (n > 9)
		putn1(n / 10);
	*putp++ = n % 10 + '0';
}

getn(cp)
	register uchar_t *cp;
{
	register int n;
	int sign, base = 10;

	sign = 0;
	if (cp[0] == '+' && cp[1])
		cp++;
	if (*cp == '-') {
		sign++;
		cp++;
		if (!digit(*cp))
			goto badnum;
	}
	if (cp[0] == '0')
		base = 8;
	n = 0;
	while (digit(*cp))
		n = n * base + *cp++ - '0';
	if (*cp)
		goto badnum;
	return (sign ? -n : n);
badnum:
	bferr(MSGSTR(M_BADNUM, "Badly formed number"));
	return (0);
}

uchar_t *
value(var)
	uchar_t *var;
{

	return (value1(var, &shvhed));
}

uchar_t *
value1(var, head)
	uchar_t *var;
	struct varent *head;
{
	register struct varent *vp;

	vp = adrof1(var, head);
	return (vp == 0 || vp->vec[0] == 0 ? (uchar_t *)"" : vp->vec[0]);
}

static	struct varent *shprev;

struct varent *
adrof(var)
	uchar_t *var;
{

	return (adrof1(var, &shvhed));
}

struct varent *
madrof(pat, head)
	uchar_t *pat;
	struct varent *head;
{
	register struct varent *vp;

	shprev = head;
	for (vp = shprev->link; vp != 0; vp = vp->link) {
		if (Gmatch(vp->name, pat))
			return (vp);
		shprev = vp;
	}
	return (0);
}

struct varent *
adrof1(var, head)
	uchar_t *var;
	struct varent *head;
{
	register struct varent *vp;
	int cmp;

	shprev = head;
	for (vp = shprev->link; vp != 0; vp = vp->link) {
		cmp = strcmp(vp->name, var);
		if (cmp == 0)
			return (vp);
		else if (cmp > 0)
			return (0);
		shprev = vp;
	}
	return (0);
}

/*
 * The caller is responsible for putting value in a safe place
 */
set(var, value)
	uchar_t *var, *value;
{
	register uchar_t **vec = (uchar_t **) calloc(2, sizeof (char **));

	vec[0] = onlyread(value) ? savestr(value) : value;
	set1(var, vec, &shvhed);
}

set1(var, vec, head)
	uchar_t *var, **vec;
	struct varent *head;
{

	register uchar_t **oldv = vec;

	gflag = 0; rscan(oldv, tglob);
	if (gflag) {
		vec = glob(oldv);
		if (vec == 0) {
			bferr(MSGSTR( M_NOMATCH, "No match"));
			blkfree(oldv);
			return;
		}
		blkfree(oldv);
		gargv = 0;
	}
	setq(var, vec, head);
}

setq(var, vec, head)
	uchar_t *var, **vec;
	struct varent *head;
{
	register struct varent *vp;

	vp = adrof1(var, head);
	if (vp == 0) {
		vp = (struct varent *) calloc(1, sizeof *vp);
		vp->name = savestr(var);
		vp->link = shprev->link;
		shprev->link = vp;
	}
	if (vp->vec)
		blkfree(vp->vec);
	scan(vec, trim);
	vp->vec = vec;
}

unset(v)
	register uchar_t *v[];
{

	unset1(v, &shvhed);
	if (adrof("histchars") == 0) {
		HIST = '!';
		HISTSUB = '^';
	}
#ifdef CMDEDIT
        if (adrof("editmode") == 0) {
                register struct varent *vp;

                if ((vp = adrof("edithist")) != 0)
                        unsetv1(vp);
                unsetenv("EDITMODE");
                cmdedit = 0;
        }
#endif
#ifdef FILEC
	if (adrof("filec") == 0)
		filec = 0;
#endif
}

unset1(v, head)
	register uchar_t *v[];
	struct varent *head;
{
	register uchar_t *var;
	register struct varent *vp;
	register int cnt;

	v++;
	while (var = *v++) {
		cnt = 0;
		while (vp = madrof(var, head))
			unsetv1(vp->name, head), cnt++;
		if (cnt == 0)
			setname(var);
	}
}

unsetv(var)
	uchar_t *var;
{

	unsetv1(var, &shvhed);
}

unsetv1(var, head)
	uchar_t *var;
	struct varent *head;
{
	register struct varent *vp;

	vp = adrof1(var, head);
	if (vp == 0)
		udvar(var);
	vp = shprev->link;
	shprev->link = vp->link;
	blkfree(vp->vec);
	xfree(vp->name);
	xfree((char *)vp);
}

setNS(cp)
	uchar_t *cp;
{

	set(cp, "");
}

shift(v)
	register uchar_t **v;
{
	register struct varent *argv;
	register uchar_t *name;

	v++;
	name = *v;
	if (name == 0)
		name = (uchar_t *)"argv";
	else
		strip(name);
	argv = adrof(name);
	if (argv == 0)
		udvar(name);
	if (argv->vec[0] == 0)
		bferr(MSGSTR(M_NOMORE, "No more words"));
	lshift(argv->vec, 1);
}

exportpath(val)
uchar_t **val;
{
	uchar_t exppath[BUFR_SIZ];
	register uchar_t *dir;

	exppath[0] = 0;
	if (val)
		while (*val) {
			if (strlen(*val) + strlen(exppath) + 2 > BUFR_SIZ) {
				printf(MSGSTR(M_LONGPATH,
				"Warning: ridiculously long PATH truncated\n"));
				break;
			}
			strcat(exppath, *val++);
			if (*val == 0 || eq(*val, ")"))
				break;
			strcat(exppath, ":");
		}
	setenv("PATH", exppath);
}
