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
static char rcsid[] = "@(#)$RCSfile: func.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 01:21:32 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: isbfunc func dolabel doonintr donohup dozip prvars doalias 
 *            unalias dologout dologin donewgrp islogin doif reexecute doelse
 *            dogoto doswitch dobreak doexit doforeach dowhile preread 
 *            doend docontin doagain dorepeat doswbrk srchx search getword 
 *            toend wfree doecho doglob echo dosetenv dounsetenv setenv 
 *            unsetenv doumask findlim dolimit getval limtail plim dounlimit
 *            setlim dosuspend doeval
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

#include <sys/ioctl.h>
#include <locale.h>

/*
 * C shell
 */

#ifdef KJI
#include <NLctype.h>
#endif
#include "sh.h"

struct biltins *
isbfunc(t)
	register struct command *t;
{
	register uchar_t *cp = t->t_dcom[0];
	register uchar_t *dp;
	register struct biltins *bp;
	int dolabel(), dofg1(), dobg1();
	static struct biltins label = { (uchar_t *)"", dolabel, 0, 0 };
	static struct biltins foregnd = { (uchar_t *)"%job", dofg1, 0, 0 };
	static struct biltins backgnd = { (uchar_t *)"%job &", dobg1, 0, 0 };

	if (lastchr(cp) == ':') {
		label.bname = cp;
		return (&label);
	}
	if (*cp == '%') {
		if (t->t_dflg & FAND) {
			t->t_dflg &= ~FAND;
			backgnd.bname = cp;
			return (&backgnd);
		} 
		foregnd.bname = cp;
		return (&foregnd);
	}
	for (bp = bfunc; dp = bp->bname; bp++) {
		if (dp[0] == cp[0] && eq(dp, cp))
			return (bp);
		if (dp[0] > cp[0])
			break;
	}
	return (0);
}

func(t, bp)
	register struct command *t;
	register struct biltins *bp;
{
	int i;

	xechoit(t->t_dcom);
	setname(bp->bname);
	i = blklen(t->t_dcom) - 1;
	if (i < bp->minargs)
		bferr(MSGSTR(M_TOOFEW, "Too few arguments"));
	if (i > bp->maxargs)
		bferr(MSGSTR(M_TOOMANY, "Too many arguments"));
	(*bp->bfunct)(t->t_dcom, t);
}

dolabel()
{

}

doonintr(v)
	uchar_t **v;
{
	register uchar_t *cp;
	register uchar_t *vv = v[1];
	struct sigvec nsv;

	if (parintr == SIG_IGN)
		return;
	if (setintr && intty)
		bferr(MSGSTR(M_CANT, "Can't from terminal"));
	cp = gointr, gointr = 0, xfree(cp);
	if (vv == 0) {
		if (setintr)
			sigblock(sigmask(SIGINT));
		else  {
			nsv.sv_handler = SIG_DFL;
			nsv.sv_mask = 0;
			nsv.sv_onstack = 0;
			(void)sigvec(SIGINT, &nsv, (struct sigvec *)0);
		}
		gointr = 0;
	} else if (eq((vv = strip(vv)), "-")) {
		nsv.sv_handler = SIG_IGN;
		nsv.sv_mask = 0;
		nsv.sv_onstack = 0;
		(void)sigvec(SIGINT, &nsv, (struct sigvec *)0);
		gointr = (uchar_t *)"-";
	} else {
		gointr = savestr(vv);
		nsv.sv_handler = (void (*)(int))pintr;
		nsv.sv_mask = 0;
		nsv.sv_onstack = 0;
		(void)sigvec(SIGINT, &nsv, (struct sigvec *)0);
	}
}

donohup()
{
	struct sigvec nsv;

	if (intty)
		bferr(MSGSTR(M_CANT, "Can't from terminal"));
	if (setintr == 0) {
		nsv.sv_handler = SIG_IGN;
		nsv.sv_mask = 0;
		nsv.sv_onstack = 0;
		(void)sigvec(SIGHUP, &nsv, (struct sigvec *)0);
#ifdef CC
		submit(getpid());
#endif
	}
}

dozip()
{

	;
}

prvars()
{

	plist(&shvhed);
}

doalias(v)
	register uchar_t **v;
{
	register struct varent *vp;
	register uchar_t *p;

	v++;
	p = *v++;
	if (p == 0)
		plist(&aliases);
	else if (*v == 0) {
		vp = adrof1(strip(p), &aliases);
		if (vp)
			blkpr(vp->vec), printf("\n");
	} else {
		if (eq(p, "alias") || eq(p, "unalias")) {
			setname(p);
			bferr(MSGSTR(M_ALIAS, "Too dangerous to alias that"));
		}
		set1(strip(p), saveblk(v), &aliases);
	}
}

unalias(v)
	uchar_t **v;
{

	unset1(v, &aliases);
}

dologout()
{

	islogin();
	goodbye();
}

dologin(v)
	uchar_t **v;
{
	struct sigvec nsv;

	islogin();
	rechist();
	nsv.sv_handler = parterm;
	nsv.sv_mask = 0;
	nsv.sv_onstack = 0;
	(void)sigvec(SIGTERM,&nsv,(struct sigvec *)0);
	execl("/bin/login", "login", v[1], 0);
	untty();
	exit(1);
}

dosetsenv(v)
        uchar_t **v;
{
        struct sigvec nsv;

        if (chkstop == 0 && setintr)
                panystop(0);
        nsv.sv_handler = parterm;
        nsv.sv_mask = 0;
        nsv.sv_onstack = 0;
        (void)sigvec(SIGTERM,&nsv,(struct sigvec *)0);
        execl("/bin/setsenv", "setsenv", v[1], 0);
        untty();
        exit(1);
}

dosetgroups(v)
        uchar_t **v;
{
        struct sigvec nsv;

        if (chkstop == 0 && setintr)
                panystop(0);
        nsv.sv_handler = parterm;
        nsv.sv_mask = 0;
        nsv.sv_onstack = 0;
        (void)sigvec(SIGTERM,&nsv,(struct sigvec *)0);
        execv("/bin/setgroups", v);
        untty();
        exit(1);
}

donewgrp(v)
	uchar_t **v;
{
	struct sigvec nsv;

	if (chkstop == 0 && setintr)
		panystop(0);
	nsv.sv_handler = parterm;
	nsv.sv_mask = 0;
	nsv.sv_onstack = 0;
	(void)sigvec(SIGTERM,&nsv,(struct sigvec *)0);
	execl("/bin/newgrp", "newgrp", v[1], 0);
	execl("/usr/bin/newgrp", "newgrp", v[1], 0);
	exit(1);
}

doinlib(v)
uchar_t **v;
{
	if(!ldr_install(v[1]))
		return(0);
	error(MSGSTR(M_BADINLIB, "inlib install failed."));
}

dormlib(v)
uchar_t **v;
{
	if(!ldr_remove(v[1]))
		return(0);
	error(MSGSTR(M_BADRMLIB, "rmlib removal failed."));
}

islogin()
{

	if (chkstop == 0 && setintr)
		panystop(0);
	if (loginsh)
		return;
	error(MSGSTR(M_NOTLOGIN, "Not login shell"));
}

doif(v, kp)
	uchar_t **v;
	struct command *kp;
{
	register int i;
	register uchar_t **vv;

	v++;
	i = exp(&v);
	vv = v;
	if (*vv == NOSTR)
		bferr(MSGSTR(M_EMPTYIF, "Empty if"));
	if (eq(*vv, "then")) {
		if (*++vv)
			bferr(MSGSTR(M_THEN, "Improper then"));
		setname((uchar_t *)"then");
		/*
		 * If expression was zero, then scan to else,
		 * otherwise just fall into following code.
		 */
		if (!i)
			search(ZIF, 0);
		return;
	}
	/*
	 * Simple command attached to this if.
	 * Left shift the node in this tree, munging it
	 * so we can reexecute it.
	 */
	if (i) {
		lshift(kp->t_dcom, vv - kp->t_dcom);
		reexecute(kp);
		donefds();
	}
}

/*
 * Reexecute a command, being careful not
 * to redo i/o redirection, which is already set up.
 */
reexecute(kp)
	register struct command *kp;
{

	kp->t_dflg &= FSAVE;
	kp->t_dflg |= FREDO;
	/*
	 * If tty is still ours to arbitrate, arbitrate it;
	 * otherwise dont even set pgrp's as the jobs would
	 * then have no way to get the tty (we can't give it
	 * to them, and our parent wouldn't know their pgrp, etc.
	 */
	execute(kp, tpgrp > 0 ? tpgrp : -1, (int *)0, (int *)0);
}

doelse()
{

	search(ZELSE, 0);
}

dogoto(v)
	uchar_t **v;
{
	register struct whyle *wp;
	uchar_t *lp;

	/*
	 * While we still can, locate any unknown ends of existing loops.
	 * This obscure code is the WORST result of the fact that we
	 * don't really parse.
	 */
	for (wp = whyles; wp; wp = wp->w_next)
		if (wp->w_end == 0) {
			search(ZBREAK, 0);
			wp->w_end = btell();
		} else
			bseek(wp->w_end);
	search(ZGOTO, 0, lp = globone(v[1]));
	xfree(lp);
	/*
	 * Eliminate loops which were exited.
	 */
	wfree();
}

doswitch(v)
	register uchar_t **v;
{
	register uchar_t *cp, *lp;

	v++;
	if (!*v || *(*v++) != '(')
		goto syntax;
	cp = **v == ')' ? (uchar_t *)"" : *v++;
	if (*(*v++) != ')')
		v--;
	if (*v)
syntax:
		error(MSGSTR(M_SYNERR, "Syntax error"));
	search(ZSWITCH, 0, lp = globone(cp));
	xfree(lp);
}

dobreak()
{

	if (whyles)
		toend();
	else
		bferr(MSGSTR(M_WHILE, "Not in while/foreach"));
}

doexit(v)
	uchar_t **v;
{

	if (chkstop == 0)
		panystop(0);
	/*
	 * Don't DEMAND parentheses here either.
	 */
	v++;
	if (*v) {
		set("status", putn(exp(&v)));
		if (*v)
			bferr(MSGSTR(M_EXPR, "Expression syntax"));
	}

	btoeof();
	if (intty)
		close(SHIN);
}

doforeach(v)
	register uchar_t **v;
{
	register uchar_t *cp;
	register struct whyle *nwp;

	v++;
	cp = strip(*v);
#ifdef KJI
	while (*cp) {
		int n, nlc;
		n = NCdec (cp, &nlc);
		if (!letter(nlc))
			break;
		cp += n;
	}
	if (*cp || strlen(*v) >= 40)
#else
	while (*cp && letter(*cp))
		cp++;
	if (*cp || strlen(*v) >= 20)
#endif
		bferr(MSGSTR(M_INVVAR, "Invalid variable"));
	cp = *v++;
	if (v[0][0] != '(' || v[blklen(v) - 1][0] != ')')
		bferr(MSGSTR(M_PAREN, "Words not ()'ed"));
	v++;
	gflag = 0, rscan(v, tglob);
        v = glob(v);
        if (v == 0)
                bferr(MSGSTR(M_NOMATCH, "No match"));
	nwp = (struct whyle *) calloc(1, sizeof *nwp);
	nwp->w_fe = nwp->w_fe0 = v; gargv = 0;
	nwp->w_start = btell();
	nwp->w_fename = savestr(cp);
	nwp->w_next = whyles;
	whyles = nwp;
	/*
	 * Pre-read the loop so as to be more
	 * comprehensible to a terminal user.
	 */
	if (intty)
		preread();
	doagain();
}

dowhile(v)
	uchar_t **v;
{
	register int status;
	register bool again = whyles != 0 && whyles->w_start == lineloc &&
	    whyles->w_fename == 0;

	v++;
	/*
	 * Implement prereading here also, taking care not to
	 * evaluate the expression before the loop has been read up
	 * from a terminal.
	 */
	if (intty && !again)
		status = !exp0(&v, 1);
	else
		status = !exp(&v);
	if (*v)
		bferr(MSGSTR(M_EXPR, "Expression syntax"));
	if (!again) {
		register struct whyle *nwp = (struct whyle *) calloc(1, sizeof (*nwp));

		nwp->w_start = lineloc;
		nwp->w_end = 0;
		nwp->w_next = whyles;
		whyles = nwp;
		if (intty) {
			/*
			 * The tty preread
			 */
			preread();
			doagain();
			return;
		}
	}
	if (status)
		/* We ain't gonna loop no more, no more! */
		toend();
}

preread()
{

	whyles->w_end = -1;
	if (setintr)
		sigrelse(SIGINT);
	search(ZBREAK, 0);
	if (setintr)
		sighold(SIGINT);
	whyles->w_end = btell();
}

doend()
{

	if (!whyles)
		bferr(MSGSTR(M_WHILE, "Not in while/foreach"));
	whyles->w_end = btell();
	doagain();
}

docontin()
{

	if (!whyles)
		bferr(MSGSTR(M_WHILE, "Not in while/foreach"));
	doagain();
}

doagain()
{

	/* Repeating a while is simple */
	if (whyles->w_fename == 0) {
		bseek(whyles->w_start);
		return;
	}
	/*
	 * The foreach variable list actually has a spurious word
	 * ")" at the end of the w_fe list.  Thus we are at the
	 * of the list if one word beyond this is 0.
	 */
	if (!whyles->w_fe[1]) {
		dobreak();
		return;
	}
	set(whyles->w_fename, savestr(*whyles->w_fe++));
	bseek(whyles->w_start);
}

dorepeat(v, kp)
	uchar_t **v;
	struct command *kp;
{
	register int i;

	i = getn(v[1]);
	if (setintr)
		sighold(SIGINT);
	lshift(v, 2);
	while (i > 0) {
		if (setintr)
			sigrelse(SIGINT);
		reexecute(kp);
		--i;
	}
	donefds();
	if (setintr)
		sigrelse(SIGINT);
}

doswbrk()
{

	search(ZBRKSW, 0);
}

srchx(cp)
	register uchar_t *cp;
{
	register struct srch *sp;

	for (sp = srchn; sp->s_name; sp++)
		if (eq(cp, sp->s_name))
			return (sp->s_value);
	return (-1);
}

uchar_t	Stype;
uchar_t	*Sgoal;

/*VARARGS2*/
search(type, level, goal)
	int type;
	register int level;
	uchar_t *goal;
{
	uchar_t wordbuf[BUFR_SIZ];
	register uchar_t *aword = wordbuf;
	register uchar_t *cp;

	Stype = type; Sgoal = goal;
	if (type == ZGOTO)
		bseek(0l);
	do {
		if (intty && fseekp == feobp)
			printf("? "), flush();
		aword[0] = 0, getword(aword);
		switch (srchx(aword)) {

		case ZELSE:
			if (level == 0 && type == ZIF)
				return;
			break;

		case ZIF:
			while (getword(aword))
				continue;
			if ((type == ZIF || type == ZELSE) && eq(aword, "then"))
				level++;
			break;

		case ZENDIF:
			if (type == ZIF || type == ZELSE)
				level--;
			break;

		case ZFOREACH:
		case ZWHILE:
			if (type == ZBREAK)
				level++;
			break;

		case ZEND:
			if (type == ZBREAK)
				level--;
			break;

		case ZSWITCH:
			if (type == ZSWITCH || type == ZBRKSW)
				level++;
			break;

		case ZENDSW:
			if (type == ZSWITCH || type == ZBRKSW)
				level--;
			break;

		case ZLABEL:
			if (type == ZGOTO && getword(aword) && eq(aword, goal))
				level = -1;
			break;

		default:
			if (type != ZGOTO && (type != ZSWITCH || level != 0))
				break;
			if (lastchr(aword) != ':')
				break;
			aword[strlen(aword) - 1] = 0;
			if (type == ZGOTO && eq(aword, goal) || type == ZSWITCH && eq(aword, "default"))
				level = -1;
			break;

		case ZCASE:
			if (type != ZSWITCH || level != 0)
				break;
			getword(aword);
			if (lastchr(aword) == ':')
				aword[strlen(aword) - 1] = 0;
			cp = strip(Dfix1(aword));
			if (Gmatch(goal, cp))
				level = -1;
			xfree(cp);
			break;

		case ZDEFAULT:
			if (type == ZSWITCH && level == 0)
				level = -1;
			break;
		}
		getword(NOSTR);
	} while (level >= 0);
}

getword(wp)
	register uchar_t *wp;
{
	register int found = 0;
	register int c, d;

	c = readc(1);
	d = 0;
	do {
#ifdef KJI
		while (c == ' ' || c == '\t' || isjspace(c))
#else
		while (c == ' ' || c == '\t')
#endif
			c = readc(1);
		if (c == '#')
			do
				c = readc(1);
			while (c >= 0 && c != '\n');
		if (c < 0)
			goto past;
		if (c == '\n') {
			if (wp)
				break;
			return (0);
		}
		unreadc(c);
		found = 1;
		do {
			c = readc(1);
			if (c == '\\' && (c = readc(1)) == '\n')
				c = ' ';
			if (any(c, "'\""))
				if (d == 0)
					d = c;
				else if (d == c)
					d = 0;
			if (c < 0)
				goto past;
			if (wp)
#ifdef KJI
				PUTCH (wp,c);
		} while ((d || c != ' ' && c != '\t' && !isjspace(c)) && c != '\n');
#else
				*wp++ = c;
		} while ((d || c != ' ' && c != '\t') && c != '\n');
#endif
	} while (wp == 0);
	unreadc(c);
	if (found)
		*--wp = 0;
	return (found);

past:
	switch (Stype) {

	case ZIF:
		bferr(MSGSTR(M_ZIF, "then/endif not found"));

	case ZELSE:
		bferr(MSGSTR(M_ZELSE, "endif not found"));

	case ZBRKSW:
	case ZSWITCH:
		bferr(MSGSTR(M_ZSWITCH, "endsw not found"));

	case ZBREAK:
		bferr(MSGSTR(M_ZBREAK, "end not found"));

	case ZGOTO:
		setname(Sgoal);
		bferr(MSGSTR(M_ZGOTO, "label not found"));
	}
	/*NOTREACHED*/
}

toend()
{

	if (whyles->w_end == 0) {
		search(ZBREAK, 0);
		whyles->w_end = btell() - 1;
	} else
		bseek(whyles->w_end);
	wfree();
}

wfree()
{
	long o = btell();

	while (whyles) {
		register struct whyle *wp = whyles;
		register struct whyle *nwp = wp->w_next;

		if (o >= wp->w_start && (wp->w_end == 0 || o < wp->w_end))
			break;
		if (wp->w_fe0)
			blkfree(wp->w_fe0);
		if (wp->w_fename)
			xfree(wp->w_fename);
		xfree((char *)wp);
		whyles = nwp;
	}
}

doecho(v)
	uchar_t **v;
{

	echo(' ', v);
}

doglob(v)
	uchar_t **v;
{

	echo(0, v);
	flush();
}

echo(sep, v)
	uchar_t sep;
	register uchar_t **v;
{
	register uchar_t *cp;
	int nonl = 0;

	if (setintr)
		sigrelse(SIGINT);
	v++;
	if (*v == 0)
		return;
	gflag = 0; rscan(v, tglob);
	if (gflag) {
		v = glob(v);
		if (v == 0)
			bferr(MSGSTR(M_NOMATCH, "No match"));
		if (*v == 0)
			return;
	} else
		scan(v, trim);
	if (sep == ' ' && !strcmp(*v, "-n"))
		nonl++, v++;
	while (cp = *v++) {
		register int c;
		/* quoting here is to print control uchar_ts as is */
		while (c = *cp++) {
			if (c < 040 || c == 0177)
				putchar(NLQUOTE);
			putchar(c);
		}
		if (*v) {
			if (sep<040 || sep== 0177)
				putchar(NLQUOTE);
			putchar(sep);
		}
	}
	if (sep && nonl == 0)
		putchar('\n');
	else
		flush();
	if (setintr)
		sighold(SIGINT);
	if (gargv)
		blkfree(gargv), gargv = 0;
}

extern uchar_t	**environ;

dosetenv(v)
	register uchar_t **v;
{
	uchar_t *vp, *lp;

	v++;
	if ((vp = *v++) == (uchar_t *)0) {
		register uchar_t **ep;

		if (setintr)
			(void) sigsetmask(sigblock(0) & ~ sigmask(SIGINT));
		for (ep = environ; *ep; ep++)
			printf("%s\n", *ep);
		return;
	}
	if ((lp = *v++) == (uchar_t *)0)
            setenv(vp, "");
	setenv(vp, lp = globone(lp));
	if (eq(vp, "PATH")) {
		importpath(lp);
		dohash();
	}
        else if (eq(vp, "LANG")) setlocale(LC_ALL, "");
        else if (eq(vp, "LC_MESSAGES")) setlocale(LC_MESSAGES, (char *)lp);
        else if (eq(vp, "LC_CTYPE")) setlocale(LC_CTYPE, (char *)lp);
        else if (eq(vp, "LC_COLLATE")) setlocale(LC_COLLATE, (char *)lp);

	xfree(lp);
}

dounsetenv(v)
	register uchar_t **v;
{

	v++;
	do
#if defined(NLS) || defined(KJI)
	{
		unsetenv(*v);
#if defined(NOT_NEW_CODE)
		if (eq(*v,"NLFILE") || eq(*v,"NLCTAB"))
			NLgetctab(0);
#endif
		v++;
	}
#else
		unsetenv(*v++);
#endif
	while (*v);
}

setenv(name, value)
	uchar_t *name, *value;
{
	register uchar_t **ep = environ;
	register uchar_t *cp, *dp;
	uchar_t *blk[2], **oep = ep;

#if defined(DEBUG) && defined(debugp)
printf("debug: environ=0x%x\n", ep);
printf("debug: setenv(%s, %s)\n", name, value);
#endif
	for (; *ep; ep++) {
#if defined(DEBUG) && defined(debugp)
printf("debug:ep=0x%x, *ep=0x%x\n", ep, *ep);
printf("debug: *ep=%s\n", *ep);
printf("debug:**ep=%c\n", **ep);
#endif
		for (cp = name, dp = *ep; *cp && *cp == *dp; cp++, dp++)
{
#if defined(DEBUG) && defined(debugp)
printf("debug: *dp=%c\n", *dp);
#endif
			continue;
}
#if defined(DEBUG) && defined(debugp)
printf("debug name=%s\n",name);
#endif
		if (*cp != 0 || *dp != '=')
			continue;
		cp = strspl("=", value);
#if defined(DEBUG) && defined(debugp)
printf("debug: before first free, *ep=%s\n", *ep);
#endif
		xfree(*ep);
		*ep = strspl(name, cp);
#if defined(DEBUG) && defined(debugp)
printf("debug: before second free, cp=%s\n", cp);
#endif
		xfree(cp);
		scan(ep, trim);
		return;
	}
#if defined(DEBUG) && defined(debugp)
printf("here\n");
#endif
	blk[0] = strspl(name, "="); blk[1] = 0;
#if defined(DEBUG) && defined(debugp)
printf("here\n");
#endif
	environ = blkspl(environ, blk);
#if defined(DEBUG) && defined(debugp)
printf("here\n");
#endif
	xfree((char *)oep);
	setenv(name, value);
}

unsetenv(name)
	uchar_t *name;
{
	register uchar_t **ep = environ;
	register uchar_t *cp, *dp;
	uchar_t **oep = ep;

	for (; *ep; ep++) {
		for (cp = name, dp = *ep; *cp && *cp == *dp; cp++, dp++)
			continue;
		if (*cp != 0 || *dp != '=')
			continue;
		cp = *ep;
		*ep = 0;
		environ = blkspl(environ, ep+1);
		*ep = cp;
		xfree(cp);
		xfree((char *)oep);
		return;
	}
}

doumask(v)
	register uchar_t **v;
{
	register uchar_t *cp = v[1];
	register int i;

	if (cp == 0) {
		i = umask(0);
		umask(i);
		printf("%o\n", i);
		return;
	}
	i = 0;
	while (digit(*cp) && *cp != '8' && *cp != '9')
		i = i * 8 + *cp++ - '0';
	if (*cp || i < 0 || i > 0777)
		bferr("Improper mask");
	umask(i);
}

struct limits {
        int     limconst;
        char    *limname;
        int     limdiv;
        char    *limscale;
} limits[] = {
        RLIMIT_CPU,     "cputime",      1,      "seconds",
        RLIMIT_FSIZE,   "filesize",     1024,   "kbytes",
        RLIMIT_DATA,    "datasize",     1024,   "kbytes",
        RLIMIT_STACK,   "stacksize",    1024,   "kbytes",
        RLIMIT_CORE,    "coredumpsize", 1024,   "kbytes",
        RLIMIT_RSS,     "memoryuse",    1024,   "kbytes",
        -1,             0,
};


struct limits *
findlim(cp)
	uchar_t *cp;
{
	register struct limits *lp, *res;

	res = 0;
	for (lp = limits; lp->limconst >= 0; lp++)
		if (prefix(cp, lp->limname)) {
			if (res)
				bferr(MSGSTR(M_AMBIG, "Ambiguous"));
			res = lp;
		}
	if (res)
		return (res);
	bferr(MSGSTR(M_NOLIMIT, "No such limit"));
}

dolimit(v)
	register uchar_t **v;
{
	register struct limits *lp;
	register int limit;
	uchar_t hard = 0;

	v++;
        if (*v && eq(*v, "-h")) {
                hard = 1;
                v++;
        }
	if (*v == 0) {
		for (lp = limits+1; lp->limconst >= 0; lp++)
			plim(lp, hard);
		return;
	}
	lp = findlim(v[0]);
	if (v[1] == 0) {
		plim(lp, hard);
		return;
	}
	limit = getval(lp, v+1);
        if (setlim(lp, hard, limit) < 0)
                error(NOSTR);
}

getval(lp, v)
	register struct limits *lp;
	uchar_t **v;
{
	register float f;
	double atof();
	uchar_t *cp = *v++;

	f = atof(cp);
	while (digit(*cp) || *cp == '.' || *cp == 'e' || *cp == 'E')
		cp++;
	if (*cp == 0) {
		if (*v == 0)
			return ((int)(f+0.5) * lp->limdiv);
		cp = *v;
	}
	switch (*cp) {

	case ':':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
		return ((int)(f * 60.0 + atof(cp+1)));

	case 'h':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
		limtail(cp, "hours");
		f *= 3600.;
		break;

	case 'b':
		if (lp->limconst == RLIMIT_CPU)
			goto badscal;
		limtail(cp, "blocks");
		f *= 512.;
		break;
	case 'm':
		if (lp->limconst == RLIMIT_CPU) {
			limtail(cp, "minutes");
			f *= 60.;
			break;
		}
	case 'M':
		if (lp->limconst == RLIMIT_CPU)
			goto badscal;
		*cp = 'm';
		limtail(cp, "megabytes");
		f *= 1024.*1024.;
		break;

	case 's':
		if (lp->limconst != RLIMIT_CPU)
			goto badscal;
		limtail(cp, "seconds");
		break;

	case 'k':
		if (lp->limconst == RLIMIT_CPU)
			goto badscal;
		limtail(cp, "kbytes");
		f *= 1024;
		break;

	case 'u':
		limtail(cp, "unlimited");
		return (RLIM_INFINITY);

	default:
badscal:
		bferr(MSGSTR(M_SCALE, "Improper or unknown scale factor"));
	}
	return ((int)(f+0.5));
}

limtail(cp, str0)
	uchar_t *cp, *str0;
{
	register uchar_t *str = str0;

	while (*cp && *cp == *str)
		cp++, str++;
	if (*cp)
		error(MSGSTR(M_BAD,"Bad scaling; did you mean ``%s''?"), str0);
}

plim(lp, hard)
	register struct limits *lp;
	uchar_t hard;
{
	struct rlimit rlim;
	unsigned int lim;

	printf("%s \t", lp->limname);
        (void) getrlimit(lp->limconst, &rlim);
        lim = hard ? rlim.rlim_max : rlim.rlim_cur;
	if (lim == RLIM_INFINITY)
		printf(MSGSTR(M_UNLIMITED, "unlimited"));
	else if (lp->limconst == RLIMIT_CPU)
		psecs((long)lim);
	else
		printf("%u %s", lim / lp->limdiv, lp->limscale);
	printf("\n");
}

dounlimit(v)
        register uchar_t **v;
{
        register struct limits *lp;
        int err = 0;
        uchar_t hard = 0;

        v++;
        if (*v && eq(*v, "-h")) {
                hard = 1;
                v++;
        }
        if (*v == 0) {
                for (lp = limits; lp->limconst >= 0; lp++)
                        if (setlim(lp, hard, (int)RLIM_INFINITY) < 0)
                                err++;
                if (err)
                        error(NOSTR);
                return;
        }
        while (*v) {
                lp = findlim(*v++);
                if (setlim(lp, hard, (int)RLIM_INFINITY) < 0)
                        error(NOSTR);
        }
}

setlim(lp, hard, limit)
        register struct limits *lp;
        uchar_t hard;
{
        struct rlimit rlim;

        (void) getrlimit(lp->limconst, &rlim);
        if (hard)
                rlim.rlim_max = limit;
        else if (limit == RLIM_INFINITY && geteuid() != 0)
                rlim.rlim_cur = rlim.rlim_max;
        else
                rlim.rlim_cur = limit;
	if(lp->limconst == RLIMIT_FSIZE)  /* must use ulimit() here :-( */
	{				  /* due to POSIX contraints */
		if(ulimit( 2, (rlim.rlim_cur / 512)) < 0)  /* set fsize limit */
		{
                	printf("%s: %s: Can't %s%s limit\n", bname, lp->limname,
                    	limit == RLIM_INFINITY ? "remove" : "set", hard ? " hard" : "");
                	return (-1);
		}
	}
	else
	{
        	if (setrlimit(lp->limconst, &rlim) < 0) {
                	printf("%s: %s: Can't %s%s limit\n", bname, lp->limname,
                    	limit == RLIM_INFINITY ? "remove" : "set", hard ? " hard" : "");
                	return (-1);
        	}
	}
        return (0);
}

dosuspend()
{
	int old, ldisc;
	pid_t ctpgrp;
#if _BSD
	struct sigvec nsv, osv;

	if (loginsh)
		error(MSGSTR(M_SUSPEND,"Can't suspend a login shell (yet)"));
	untty();
	nsv.sv_handler = SIG_DFL;
	nsv.sv_mask = SA_RESTART;
	nsv.sv_onstack = 0;
	(void)sigvec(SIGTSTP, &nsv, &osv);
	kill(0, SIGTSTP);
	/* the shell stops here */
	(void)sigvec(SIGTSTP, &osv, (struct sigvec *)0);
	if (tpgrp != -1) {
retry:
		ioctl(FSHTTY, TIOCGPGRP, &ctpgrp);
		if (ctpgrp != opgrp) {
			(void)sigvec(SIGTTIN, &nsv, &osv);
			kill(0, SIGTTIN);
			(void)sigvec(SIGTTIN, &osv, (struct sigvec *)0);
			goto retry;
		}
		nsv.sv_handler = SIG_IGN;
		(void)sigvec(SIGTTOU, &nsv, &osv);
		ioctl(FSHTTY, TIOCSPGRP, &shpgrp);
		(void)sigvec(SIGTTOU, &osv, (struct sigvec *)0);
		setpgrp(0, shpgrp);
	}

#else
	okpcntl();
#endif
}

doeval(v)
	uchar_t **v;
{
	uchar_t **oevalvec = evalvec;
	uchar_t *oevalp = evalp;
	jmp_buf osetexit;
	int reenter;
	uchar_t **gv = 0;

	v++;
	if (*v == 0)
		return;
	gflag = 0; 
        rscan(v, tglob);
	if (gflag) 
	{
		gv = glob(v);
		v = gv;
		gargv = 0;
		if (v == 0)
			error(MSGSTR(M_NOMATCH, "No match"));
		v = copyblk(v);
	} else
		scan(v, trim);
	getexit(osetexit);
	reenter = 0;
	setexit();
	reenter++;
	if (reenter == 1) {
		evalvec = v;
		evalp = 0;
		process(0);
	}
	evalvec = oevalvec;
	evalp = oevalp;
	doneinp = 0;
	if (gv)
		blkfree(gv);
	resexit(osetexit);
	if (reenter >= 2)
		error(NOSTR);
}
