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
static char rcsid[] = "@(#)$RCSfile: exp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:29 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: exp exp0 exp1 exp2 exp2a exp2b exp2c exp3 exp3a exp4 exp5 exp6 
 *            evalav isa egetn
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

#include "sh.h"

/*
 * C shell
 */

#define IGNORE	1	/* in ignore, it means to ignore value, just parse */
#define NOGLOB	2	/* in ignore, it means not to globone */

#define	ADDOP	1
#define	MULOP	2
#define	EQOP	4
#define	RELOP	8
#define	RESTOP	16
#define	ANYOP	31

#define	EQEQ	1
#define	GTR	2
#define	LSS	4
#define	NOTEQ	6
#define EQMATCH 7
#define NOTEQMATCH 8

exp(vp)
	register uchar_t ***vp;
{

	return (exp0(vp, 0));
}

exp0(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register int p1 = exp1(vp, ignore);
	
#ifdef EDEBUG
	etraci("exp0 p1", p1, vp);
#endif
	if (**vp && eq(**vp, "||")) {
		register int p2;

		(*vp)++;
		p2 = exp0(vp, (ignore&IGNORE) || p1);
#ifdef EDEBUG
		etraci("exp0 p2", p2, vp);
#endif
		return (p1 || p2);
	}
	return (p1);
}

exp1(vp, ignore)
	register uchar_t ***vp;
{
	register int p1 = exp2(vp, ignore);

#ifdef EDEBUG
	etraci("exp1 p1", p1, vp);
#endif
	if (**vp && eq(**vp, "&&")) {
		register int p2;

		(*vp)++;
		p2 = exp1(vp, (ignore&IGNORE) || !p1);
#ifdef EDEBUG
		etraci("exp1 p2", p2, vp);
#endif
		return (p1 && p2);
	}
	return (p1);
}

exp2(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register int p1 = exp2a(vp, ignore);

#ifdef EDEBUG
	etraci("exp3 p1", p1, vp);
#endif
	if (**vp && eq(**vp, "|")) {
		register int p2;

		(*vp)++;
		p2 = exp2(vp, ignore);
#ifdef EDEBUG
		etraci("exp3 p2", p2, vp);
#endif
		return (p1 | p2);
	}
	return (p1);
}

exp2a(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register int p1 = exp2b(vp, ignore);

#ifdef EDEBUG
	etraci("exp2a p1", p1, vp);
#endif
	if (**vp && eq(**vp, "^")) {
		register int p2;

		(*vp)++;
		p2 = exp2a(vp, ignore);
#ifdef EDEBUG
		etraci("exp2a p2", p2, vp);
#endif
		return (p1 ^ p2);
	}
	return (p1);
}

exp2b(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register int p1 = exp2c(vp, ignore);

#ifdef EDEBUG
	etraci("exp2b p1", p1, vp);
#endif
	if (**vp && eq(**vp, "&")) {
		register int p2;

		(*vp)++;
		p2 = exp2b(vp, ignore);
#ifdef EDEBUG
		etraci("exp2b p2", p2, vp);
#endif
		return (p1 & p2);
	}
	return (p1);
}

exp2c(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register uchar_t *p1 = exp3(vp, ignore);
	register uchar_t *p2;
	register int i;

#ifdef EDEBUG
	etracc("exp2c p1", p1, vp);
#endif
	if (i = isa(**vp, EQOP)) {
		(*vp)++;
		if (i == EQMATCH || i == NOTEQMATCH)
			ignore |= NOGLOB;
		p2 = exp3(vp, ignore);
#ifdef EDEBUG
		etracc("exp2c p2", p2, vp);
#endif
		if (!(ignore&IGNORE)) switch (i) {

		case EQEQ:
			i = eq(p1, p2);
			break;

		case NOTEQ:
			i = !eq(p1, p2);
			break;

		case EQMATCH:
			i = Gmatch(p1, p2);
			break;

		case NOTEQMATCH:
			i = !Gmatch(p1, p2);
			break;
		}
		xfree(p1), xfree(p2);
		return (i);
	}
	i = egetn(p1);
	xfree(p1);
	return (i);
}

uchar_t *
exp3(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register uchar_t *p1, *p2;
	register int i;

	p1 = exp3a(vp, ignore);
#ifdef EDEBUG
	etracc("exp3 p1", p1, vp);
#endif
	if (i = isa(**vp, RELOP)) {
		(*vp)++;
		if (**vp && eq(**vp, "="))
			i |= 1, (*vp)++;
		p2 = exp3(vp, ignore);
#ifdef EDEBUG
		etracc("exp3 p2", p2, vp);
#endif
		if (!(ignore&IGNORE)) switch (i) {

		case GTR:
			i = egetn(p1) > egetn(p2);
			break;

		case GTR|1:
			i = egetn(p1) >= egetn(p2);
			break;

		case LSS:
			i = egetn(p1) < egetn(p2);
			break;

		case LSS|1:
			i = egetn(p1) <= egetn(p2);
			break;
		}
		xfree(p1), xfree(p2);
		return (putn(i));
	}
	return (p1);
}

uchar_t *
exp3a(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register uchar_t *p1, *p2, *op;
	register int i;

	p1 = exp4(vp, ignore);
#ifdef EDEBUG
	etracc("exp3a p1", p1, vp);
#endif
	op = **vp;
	if (op && any(op[0], "<>") && op[0] == op[1]) {
		(*vp)++;
		p2 = exp3a(vp, ignore);
#ifdef EDEBUG
		etracc("exp3a p2", p2, vp);
#endif
		if (op[0] == '<')
			i = egetn(p1) << egetn(p2);
		else
			i = egetn(p1) >> egetn(p2);
		xfree(p1), xfree(p2);
		return (putn(i));
	}
	return (p1);
}

uchar_t *
exp4(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register uchar_t *p1, *p2;
	register int i = 0;

	p1 = exp5(vp, ignore);
#ifdef EDEBUG
	etracc("exp4 p1", p1, vp);
#endif
	if (isa(**vp, ADDOP)) {
		register uchar_t *op = *(*vp)++;

		p2 = exp4(vp, ignore);
#ifdef EDEBUG
		etracc("exp4 p2", p2, vp);
#endif
		if (!(ignore&IGNORE)) switch (op[0]) {

		case '+':
			i = egetn(p1) + egetn(p2);
			break;

		case '-':
			i = egetn(p1) - egetn(p2);
			break;
		}
		xfree(p1), xfree(p2);
		return (putn(i));
	}
	return (p1);
}

uchar_t *
exp5(vp, ignore)
	register uchar_t ***vp;
	bool ignore;
{
	register uchar_t *p1, *p2;
	register int i = 0;

	p1 = exp6(vp, ignore);
#ifdef EDEBUG
	etracc("exp5 p1", p1, vp);
#endif
	if (isa(**vp, MULOP)) {
		register uchar_t *op = *(*vp)++;

		p2 = exp5(vp, ignore);
#ifdef EDEBUG
		etracc("exp5 p2", p2, vp);
#endif
		if (!(ignore&IGNORE)) switch (op[0]) {

		case '*':
			i = egetn(p1) * egetn(p2);
			break;

		case '/':
			i = egetn(p2);
			if (i == 0)
				error(MSGSTR(M_DIV, "Divide by 0"));
			i = egetn(p1) / i;
			break;

		case '%':
			i = egetn(p2);
			if (i == 0)
				error(MSGSTR(M_MOD, "Mod by 0"));
			i = egetn(p1) % i;
			break;
		}
		xfree(p1), xfree(p2);
		return (putn(i));
	}
	return (p1);
}

uchar_t *
exp6(vp, ignore)
	register uchar_t ***vp;
{
	int ccode, i;
	register uchar_t *cp, *dp, *ep;

	if (eq(**vp, "!")) {
		(*vp)++;
		cp = exp6(vp, ignore);
#ifdef EDEBUG
		etracc("exp6 ! cp", cp, vp);
#endif
		i = egetn(cp);
		xfree(cp);
		return (putn(!i));
	}
	if (eq(**vp, "~")) {
		(*vp)++;
		cp = exp6(vp, ignore);
#ifdef EDEBUG
		etracc("exp6 ~ cp", cp, vp);
#endif
		i = egetn(cp);
		xfree(cp);
		return (putn(~i));
	}
	if (eq(**vp, "(")) {
		(*vp)++;
		ccode = exp0(vp, ignore);
#ifdef EDEBUG
		etraci("exp6 () ccode", ccode, vp);
#endif
		if (*vp == 0 || **vp == 0 || ***vp != ')')
			bferr(MSGSTR(M_EXPR, "Expression syntax"));
		(*vp)++;
		return (putn(ccode));
	}
	if (eq(**vp, "{")) {
		register uchar_t **v;
		struct command faket;
		uchar_t *fakecom[2];

		faket.t_dtyp = TCOM;
		faket.t_dflg = 0;
		faket.t_dcar = faket.t_dcdr = faket.t_dspr = (struct command *)0;
		faket.t_dcom = fakecom;
		fakecom[0] = (uchar_t *)"{ ... }";
		fakecom[1] = NOSTR;
		(*vp)++;
		v = *vp;
		for (;;) {
			if (!**vp)
				bferr(MSGSTR(M_MISSBRC, "Missing }"));
			if (eq(*(*vp)++, "}"))
				break;
		}
		if (ignore&IGNORE)
			return ((uchar_t *)"");
		psavejob();
		if (pfork(&faket, -1) == 0) {
			*--(*vp) = 0;
			evalav(v);
			exitstat();
		}
		pwait();
		prestjob();
#ifdef EDEBUG
		etraci("exp6 {} status", egetn(value("status")), vp);
#endif
		return (putn(egetn(value("status")) == 0));
	}
	if (isa(**vp, ANYOP))
		return ((uchar_t *)"");
	cp = *(*vp)++;
	if (*cp == '-' && any(cp[1], "erwxfdzo")) {
		struct stat stb;

		if (isa(**vp, ANYOP))
			bferr(MSGSTR(M_NOFNAME, "Missing file name"));
		dp = *(*vp)++;
		if (ignore&IGNORE)
			return ((uchar_t *)"");
		ep = globone(dp);
		switch (cp[1]) {

		case 'r':
			i = !access(ep, 4);
			break;

		case 'w':
			i = !access(ep, 2);
			break;

		case 'x':
			i = !access(ep, 1);
			break;

		default:
			if (stat((char *)ep, &stb)) {
				xfree(ep);
				return ((uchar_t *)"0");
			}
			switch (cp[1]) {

			case 'f':
				i = (stb.st_mode & S_IFMT) == S_IFREG;
				break;

			case 'd':
				i = (stb.st_mode & S_IFMT) == S_IFDIR;
				break;

			case 'z':
				i = stb.st_size == 0;
				break;

			case 'e':
				i = 1;
				break;

			case 'o':
				i = stb.st_uid == uid;
				break;
			}
		}
#ifdef EDEBUG
		etraci("exp6 -? i", i, vp);
#endif
		xfree(ep);
		return (putn(i));
	}
#ifdef EDEBUG
	etracc("exp6 default", cp, vp);
#endif
#ifdef BUG
	return (ignore&NOGLOB ? cp : globone(cp));
#else
	return (ignore&NOGLOB ? savestr(cp) : globone(cp));
#endif
}

evalav(v)
	register uchar_t **v;
{
	struct wordent paraml;
	register struct wordent *hp = &paraml;
	struct command *t;
	register struct wordent *wdp = hp;
	
	set("status", "0");
	hp->prev = hp->next = hp;
	hp->word = (uchar_t *)"";
	while (*v) {
		register struct wordent *new = (struct wordent *) calloc(1, sizeof *wdp);

		new->prev = wdp;
		new->next = hp;
		wdp->next = new;
		wdp = new;
		wdp->word = savestr(*v++);
	}
	hp->prev = wdp;
	alias(&paraml);
	t = syntax(paraml.next, &paraml, 0);
	if (err)
		error(err);
	execute(t, -1, (int *)0, (int *)0);
	freelex(&paraml), freesyn(t);
}

isa(cp, what)
	register uchar_t *cp;
	register int what;
{

	if (cp == 0)
		return ((what & RESTOP) != 0);
	if (cp[1] == 0) {
		if ((what & ADDOP) && any(cp[0], "+-"))
			return (1);
		if ((what & MULOP) && any(cp[0], "*/%"))
			return (1);
		if ((what & RESTOP) && any(cp[0], "()!~^"))
			return (1);
	}
	if ((what & RESTOP) && (any(cp[0], "|&") || eq(cp, "<<") || eq(cp, ">>")))
		return (1);
	if (what & EQOP) {
		if (eq(cp, "=="))
			return (EQEQ);
		if (eq(cp, "!="))
			return (NOTEQ);
		if (eq(cp, "=~"))
			return (EQMATCH);
		if (eq(cp, "!~"))
			return (NOTEQMATCH);
	}
	if (!(what & RELOP))
		return (0);
	if (*cp == '<')
		return (LSS);
	if (*cp == '>')
		return (GTR);
	return (0);
}

egetn(cp)
	register uchar_t *cp;
{

	if (*cp && *cp != '-' && !digit(*cp))
		bferr(MSGSTR(M_EXPR, "Expression syntax"));
	return (getn(cp));
}

/* Phew! */

#ifdef EDEBUG
etraci(str, i, vp)
	uchar_t *str;
	int i;
	uchar_t ***vp;
{

	printf("%s=%d\t", str, i);
	blkpr(*vp);
	printf("\n");
}

etracc(str, cp, vp)
	uchar_t *str, *cp;
	uchar_t ***vp;
{

	printf("%s=%s\t", str, cp);
	blkpr(*vp);
	printf("\n");
}
#endif
