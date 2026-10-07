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
static char *rcsid = "@(#)$RCSfile: ex_subr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:04 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_subr.c
 *
 * FUNCTION: tabcol, Ignore, Ignorf, NCstrcpy2, NCstrend, any, anycp,
 * backtab, change, column, comment, copyw, copywR, dingdong, error, filioerr,
 * fixindent, genindent, getDOT, getmark, if, ignnEOF, iswhite, junk, killcnt,
 * killed, lineDOL, lineDOT, lineno, markDOT, markit, markpr, markreg, merror,
 * mesg, morelines, netchHAD, netchange, nonzero, notable, notempty, oncore,
 * onhup, onintr, onsusp, preserve, printwid, putmark, putmk1, qcolumn, qcount,
 * qdistance, reverse, save, save12, saveall, setrupt, skipwh, smerror, span,
 * strcLIN, strend, ex_sync, syserror, vfindcol, vpastwh, vskipwh, whitecnt
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
 * ex_subr.c  1.7  com/cmd/edit/vi,3.1,9013 3/13/90 10:33:36
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_re.h"
#include "ex_tty.h"
#include "ex_vis.h"

#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
extern short ivis;

/*
 * Random routines, in alphabetical order.
 */

any(c, s)
	int c;
	register char *s;
{
	register int x;

	while (x = NLsgetc(s))
		if (x == c)
			return (1);
	return (0);
}

#if defined(KJI)
/*
 * anycp - return the length of the longest closing punctuation match (0 to 9)
 *	   Example: an ellipsis (...) is 3 characters.
 *	   Checking is right to left.
 */
anycp(sb,se)
	NLchar *sb, *se;		/* string begin, string position */
{
	register char *cpunct;
	register int i, retval;
	register NLchar *mp, *s;
	/* multi-char punctuation up to 9 characters */
	NLchar multipunct[10];

	++se;
	retval = 0;		/* retval holds longest match */
	cpunct = svalue(CLOSEPUNCT);
	while (*cpunct)  {
		if (*cpunct >= '1' && *cpunct <= '9')
			i = *cpunct++ - '0';
		else
			i = 1;
		if (i <= retval) {
			while (i--) (void)NLsgetc(cpunct);
		} else {
			mp = multipunct;
			/* NLsgetc macro increments cpunct */
			while (i--) *mp++ = NLsgetc(cpunct);
			s = se;
			/* right to left matching */
			do {
				if (mp == multipunct) { retval = se-s; break; }
			} while (s > sb && *--mp == *--s);
		}
	}
	return (retval);
}

#endif
backtab(i)
	register int i;
{
	register int j;

	j = i % value(SHIFTWIDTH);
	if (j == 0)
		j = value(SHIFTWIDTH);
	i -= j;
	if (i < 0)
		i = 0;
	return (i);
}

change()
{

	tchng++;
	chng = tchng;
}

/*
 * Column returns the number of
 * columns occupied by printing the
 * characters through position cp of the
 * current line.
 */
column(cp)
	register NLchar *cp;
{

	if (cp == 0)
		cp = &linebuf[LBSIZE - 2];
	return (qcolumn(cp, (NLchar *) 0));
}

/*
 * Ignore a comment to the end of the line.
 * This routine eats the trailing newline so don't call donewline().
 */
comment()
{
	register int c;

	do {
		c = ex_getchar();
	} while (c != '\n' && c != EOF);
	if (c == EOF)
		ungetchar(c);
}

copyw(to, from, size)
	register line *from, *to;
	register int size;
{

	if (size > 0)
		do
			*to++ = *from++;
		while (--size > 0);
}

copywR(to, from, size)
	register line *from, *to;
	register int size;
{

	while (--size >= 0)
		to[size] = from[size];
}

dingdong()
{

	if (flash_screen && value(FLASH))
		putpad(flash_screen);
	else if (value(ERRORBELLS))
		putpad(bell);
}

fixindent(indent)
	int indent;
{
	register int i;
	register NLchar *cp;

	i = whitecnt(genbuf);
	cp = vpastwh(genbuf);
	if (*cp == 0 && i == indent && linebuf[0] == 0) {
		genbuf[0] = 0;
		return (i);
	}
	CP(genindent(i), cp);
	return (i);
}

filioerr(cp)
	char *cp;
{
	register int oerrno = errno;

	lprintf("\"%s\"", cp);
	errno = oerrno;
	syserror(1);
}

NLchar * genindent(indent)
	register int indent;
{
	register NLchar *cp;

	for (cp = genbuf; indent >= value(TABSTOP); indent -= value(TABSTOP))
		*cp++ = '\t';
	for (; indent > 0; indent--)
		*cp++ = ' ';
	return (cp);
}

getDOT()
{

	getline(*dot);
}

line *
getmark(c)
	register int c;
{
	register line *addr;
	
	for (addr = one; addr <= dol; addr++)
		if (names[c - 'a'] == (*addr &~ 01)) {
			return (addr);
		}
	return (0);
}

ignnEOF()
{
	register int c = ex_getchar();

	if (c == EOF)
		ungetchar(c);
	else if (c=='"')
		comment();
}

/* returns one if blank or tab found */
iswhite(c)
	int c;
{

	return (c == ' ' || c == '\t');
}

junk(c)
	register int c;
{

	if (c && !value(BEAUTIFY))
		return (0);
	if (is_print(c))
		return (0);
	switch (c) {

	case '\t':
	case '\n':
	case '\f':
		return (0);

	default:
		return (1);
	}
}

killed()
{

	killcnt(addr2 - addr1 + 1);
}

killcnt(cnt)
	register int cnt;
{

	if (inopen) {
		notecnt = cnt;
		notenam = "";
		notesgn = 0;
		return;
	}
	if (!notable(cnt))
		return;
	ex_printf(msg(M_148, "%d lines"), cnt);
	if (value(TERSE) == 0) {
		ex_printf(" %c%s", Command[0] | ' ', Command + 1);
		if (Command[strlen(Command) - 1] != 'e')
			ex_putchar('e');
		ex_putchar('d');
	}
	putNFL();
}

lineno(a)
	line *a;
{

	return (a - zero);
}

lineDOL()
{

	return (lineno(dol));
}

lineDOT()
{

	return (lineno(dot));
}

markDOT()
{

	markpr(dot);
}

markpr(which)
	line *which;
{

/*
 * This used to say "which <= endcore", but endcore is now virtually
 * meaningless.  Anyway, as near as I can tell the test is always true.
 */
	if ((inglobal == 0 || inopen) && which <= dol) {
		names['z'-'a'+1] = *which & ~01;
		if (inopen)
			ncols['z'-'a'+1] = cursor;
	}
}

markreg(c)
	register int c;
{

	if (c == '\'' || c == '`')
		return ('z' + 1);
	if (c >= 'a' && c <= 'z')
		return (c);
	return (0);
}

/*
 * Mesg decodes the terse/verbose strings. Thus
 *	'xxx@yyy' -> 'xxx' if terse, else 'xxx yyy'
 *	'xxx|yyy' -> 'xxx' if terse, else 'yyy'
 * All others map to themselves.
 */
char *
mesg(str)
	register char *str;
{
	static char errbuf[256];	/* return w/ error message processing */
	register char *cp;
	
	str = strcpy(errbuf, str);
	for (cp = str; *cp; cp++)
		switch (*cp) {

		case '@':
			if (value(TERSE))
				*cp = 0;
			else
				*cp = ' ';
			break;

		case '|':
			if (value(TERSE) == 0)
				return (cp + 1);
			*cp = 0;
			break;
		}
	return (str);
}

/*VARARGS2*/
void
merror(str, i)
	char *str;
	int i;
{
	register char *cp = str;

	if (str == 0)
		return;
	if (*cp == '\n')
		putnl(), cp++;
	if (inopen > 0 && clr_eol)
		vclreol();
	if (enter_standout_mode && exit_standout_mode)
		putpad(enter_standout_mode);
	ex_printf(mesg(cp), i);
	if (enter_standout_mode && exit_standout_mode)
		putpad(exit_standout_mode);
}

morelines()
{
#ifdef OSF_MMAP
        if ((int) mmap((caddr_t) (endcore+1), pagesize, PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1,
                       (off_t)0) == -1)
            return(-1);
        else
            endcore += (pagesize/sizeof(line));
        return(0);
#else
#ifdef UNIX_SBRK
	char *sbrk();

	if ((int) sbrk(1024 * sizeof (line)) == -1)
		return (-1);
	endcore += 1024;
	return (0);
#else
	/*
	 * We can never be guaranteed that we can get more memory
	 * beyond "endcore".  So we just punt every time.
	 */
	return -1;
#endif
#endif OSF_MMAP
}

nonzero()
{

	if (addr1 == zero) {
		notempty();
		error(msg(M_149, "Nonzero address required@on this command"));
	}
}

notable(i)
	int i;
{

	return (hush == 0 && !inglobal && i > value(REPORT));
}


notempty()
{

	if (dol == zero)
		error(msg(M_150, "No lines@in the buffer"));
}


netchHAD(cnt)
	int cnt;
{

	netchange(lineDOL() - cnt);
}

netchange(i)
	register int i;
{
	if (i > 0) {
		notesgn = 1;
	} else {
		notesgn = -1;
		i = -i;
	}
	if (inopen) {
		notecnt = i;
		notenam = "";
		return;
	}
	if (!notable(i))
		return;
	if (notesgn > 0) {
		ex_printf(mesg(msg(M_282, "%d more lines@in file after %s")), i, Command);
	} else {
		ex_printf(mesg(msg(M_283, "%d fewer lines@in file after %s")), i, Command);
	}
	putNFL();
}

putmark(addr)
	line *addr;
{

	putmk1(addr, putline());
}

#if defined(KJI)
printwid(c,fromcol)
NLchar c;
int fromcol;
{
	if (c == '\t') {
		return (fromcol + value(TABSTOP) - (fromcol % value(TABSTOP)));
	} else if (is_cntrl(c)) {
		return (2);
	} else {
		return (PRINTWID(c));
	}
}

#endif
putmk1(addr, n)
	register line *addr;
	int n;
{
	register line *markp;
	register oldglobmk;

	oldglobmk = *addr & 1;
	*addr &= ~1;
	for (markp = (anymarks ? names : &names['z'-'a'+1]);
	  markp <= &names['z'-'a'+1]; markp++)
		if (*markp == *addr)
			*markp = n;
	*addr = n | oldglobmk;
}

static int	qcount();
static short	vcntcol;

qcolumn(lim, gp)
	register NLchar *lim, *gp;
{
	register int x;
	int (*OO)();

	OO = Outchar;
	Outchar = qcount;
	vcntcol = 0;
	if (lim != NULL)
		x = lim[1], lim[1] = 0;
	pline(0);
	if (lim != NULL)
		lim[1] = x;
	if (gp)
		while (*gp)
			ex_putchar(*gp++);
	Outchar = OO;
	return (vcntcol);
}

static int qcount(c)
	int c;
{

	if (c == '\t') {
		vcntcol += value(TABSTOP) - vcntcol % value(TABSTOP);
		return;
	}
#if !defined(KJI)
	vcntcol++;
#else
	/*
	 * count the PARTIALCHAR indicator that is put in the last display
	 * column when bumping over a dblwid character 
	 */
	if (is_dblwid(c) && vcntcol % WCOLS == (WCOLS - 1))
		vcntcol++;
	vcntcol += PRINTWID(c);
#endif
}
#if defined(KJI)

/* qdistance returns display distance between two points */
int
qdistance(begin,bend)
NLchar *begin, *bend;
{
	register NLchar *cp;
	int (*OO)();

	OO = Outchar;
	Outchar = qcount;
	vcntcol = (Pline == numbline ? 8 : 0);
	for (cp = begin; cp < bend; ++cp)
		ex_putchar(*cp);
	Outchar = OO;
	return (vcntcol);
}
#endif

reverse(a1, a2)
	register line *a1, *a2;
{
	register line t;

	for (;;) {
		t = *--a2;
		if (a2 <= a1)
			return;
		*a2 = *a1;
		*a1++ = t;
	}
}

save(a1, a2)
	line *a1;
	register line *a2;
{
	register int more;

	if (!FIXUNDO)
		return;
#ifdef UNDOTRACE
	if (trace)
		vudump("before save\n");
#endif
	undkind = UNDNONE;
	undadot = dot;
	more = (a2 - a1 + 1) - (unddol - dol);
	while (more >= (endcore - truedol))
		if (morelines() < 0)
			error(msg(M_153, "Out of memory@saving lines for undo - try using ed"));
	if (more)
		(*(more > 0 ? copywR : copyw))(unddol + more + 1, unddol + 1,
		    (truedol - unddol));
	unddol += more;
	truedol += more;
	copyw(dol + 1, a1, a2 - a1 + 1);
	undkind = UNDALL;
	unddel = a1 - 1;
	undap1 = a1;
	undap2 = a2 + 1;
#ifdef UNDOTRACE
	if (trace)
		vudump("after save\n");
#endif
}

save12()
{

	save(addr1, addr2);
}

saveall()
{

	save(one, dol);
}

span()
{

	return (addr2 - addr1 + 1);
}

ex_sync()
{

	chng = 0;
	tchng = 0;
	xchng = 0;
}


skipwh()
{
	register int wh;

	wh = 0;
	while (iswhite(peekchar())) {
		wh++;
		ignchar();
	}
	return (wh);
}

/*VARARGS2*/
void smerror(str, cp)
	char *str;
	char *cp;
{

	if (str == 0)
		return;
	if (inopen && clr_eol)
		vclreol();
	if (enter_standout_mode && exit_standout_mode)
		putpad(enter_standout_mode);
	lprintf(mesg(str), cp);
	if (enter_standout_mode && exit_standout_mode)
		putpad(exit_standout_mode);
}

/*
 * strcpy from a (char *) to a (NLchar *)
 * Depends on 1-to-1 mapping between ascii characters and NLchar representation,
 * as least for characters in default strings used in vi sources.
 */
NLchar * NCstrcpy2(s1,s2)
NLchar *s1;
char *s2;
{
	NLchar *s0 = s1;

	while (*s1++ = *s2++);
	return (s0);
}

NLchar * NCstrend(cp)
	register NLchar *cp;
{
	while (*cp)
		cp++;
	return (cp);
}

char * strend(cp)
	register char *cp;
{
	while (*cp)
		cp++;
	return (cp);
}

strcLIN(dp)
	NLchar *dp;
{

	CP(linebuf, dp);
}

/*
 * A system error has occurred that we need to perror.
 * danger is true if we are unsure of the contents of
 * the file or our buffer, e.g. a write error in the
 * middle of a write operation, or a temp file error.
 */
void syserror(danger)
int danger;
{
	register int e = errno;
	extern int sys_nerr;
	extern char *sys_errlist[];

	dirtcnt = 0;
	ex_putchar(' ');
	if (danger)
		edited = 0;	/* for temp file errors, for example */
	if (e >= 0 && e < sys_nerr)
		error(sys_errlist[e]);
	else
		error(msg(M_190, "System error %d"), e);
}

/*
 * Return the column number that results from being in column col and
 * hitting a tab, where tabs are set every ts columns.	Work right for
 * the case where col > columns, even if ts does not divide columns.
 */
tabcol(col, ts)
int col, ts;
{
	int offset, result;

	if (col >= columns) {
		offset = columns * (col/columns);
		col -= offset;
	} else
		offset = 0;
	result = col + ts - (col % ts) + offset;
	return (result);
}

NLchar * vfindcol(i)
	int i;
{
	register NLchar *cp;
	register int (*OO)() = Outchar;

	Outchar = qcount;
	ignore(qcolumn(linebuf - 1, NOSTR));
	for (cp = linebuf; *cp && vcntcol < i; cp++)
		ex_putchar(*cp);
	if (cp != linebuf)
		cp--;
	Outchar = OO;
	return (cp);
}

NLchar * vskipwh(cp)
	register NLchar *cp;
{

	while (iswhite(*cp) && cp[1])
		cp++;
	return (cp);
}

NLchar * vpastwh(cp)
	register NLchar *cp;
{

	while (iswhite(*cp))
		cp++;
	return (cp);
}

whitecnt(cp)
	register NLchar *cp;
{
	register int i;

	i = 0;
	for (;;)
		switch (*cp++) {

		case '\t':
			i += value(TABSTOP) - i % value(TABSTOP);
			break;

		case ' ':
			i++;
			break;

		default:
			return (i);
		}
}

#ifdef lint
Ignore(a)
	char *a;
{

	a = a;
}

Ignorf(a)
	int (*a)();
{

	a = a;
}
#endif

markit(addr)
	line *addr;
{

	if (addr != dot && addr >= one && addr <= dol)
		markDOT();
}

/*
 * When a hangup occurs our actions are similar to a preserve
 * command.  If the buffer has not been [Modified], then we do
 * nothing but remove the temporary files and exit.
 * Otherwise, we sync the temp file and then attempt a preserve.
 * If the preserve succeeds, we unlink our temp files.
 * If the preserve fails, we leave the temp files as they are
 * as they are a backup even without preservation if they
 * are not removed.
 */
void
onhup(sig)
int sig;
{

	/*
	 * USG tty driver can send multiple HUP's!!
	 */
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	if (chng == 0) {
		cleanup(1);
		exit(0);
	}
	if (setexit() == 0) {
		if (preserve()) {
			cleanup(1);
			exit(0);
		}
	}
	exit(1);
}

/*
 * Similar to onhup.  This happens when any random core dump occurs,
 * e.g. a bug in vi.  We preserve the file and then generate a core.
 */
void
oncore(sig)
int sig;
{
	static int timescalled = 0;
	sigset_t set;
	
	/*
	 * USG tty driver can send multiple HUP's!!
	 */
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(sig, SIG_DFL);	/* Insure that we don't catch it again */
	sigemptyset(&set);
	sigaddset(&set, sig);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
	
	if (timescalled++ == 0 && chng && setexit() == 0) {
		if (inopen)
			vsave();
		preserve();
		{
			char *txt = msg(M_191, "\r\nYour file has been preserved\r\n");
			write(1, txt, strlen(txt));
		}
	}
	if (timescalled < 2) {
		normal(normf);
		cleanup(2);
		kill(getpid(), sig);	/* Resend ourselves the same signal */
		/* We won't get past here */
	}
	exit(1);
}

/*
 * An interrupt occurred.  Drain any output which
 * is still in the output buffering pipeline.
 * Catch interrupts again.  Unless we are in visual
 * reset the output state (out of -nl mode, e.g).
 * Then like a normal error (with the \n before Interrupt
 * suppressed in visual mode).
 */
void
onintr(sig)
int sig;
{
#ifndef CBREAK
	signal(SIGINT, onintr);
#else
	signal(SIGINT, inopen ? vintr : onintr);
#endif
	cancelalarm();
	draino();
	if (!inopen) {
		pstop();
		setlastchar('\n');
#ifdef CBREAK
	}
#else
	} else
		vraw();
#endif
	if (inopen != 0) {
		_fixol_(0);	/* for ESC catch, but safe */
		error(msg(M_192, "Interrupt"));
	}
	else
		error(msg(M_193, "\nInterrupt"));
}

/*
 * If we are interruptible, enable interrupts again.
 * In some critical sections we turn interrupts off,
 * but not very often.
 */
setrupt()
{

	if (ruptible) {
#ifndef CBREAK
		signal(SIGINT, onintr);
#else
		signal(SIGINT, inopen ? vintr : onintr);
#endif
#ifdef SIGTSTP
		if (dosusp)
			signal(SIGTSTP, onsusp);
#endif
	}
}

preserve()
{

	tflush();
	synctmp();
	pid = fork();
	if (pid < 0)
		return (0);
	if (pid == 0) {
		close(0);
		dup(tfile);
		execl(EXPRESERVE, "expreserve", (char *) 0);
		exit(1);
	}
	waitfor();
	if (rpid == pid && status == 0)
		return (1);
	return (0);
}

#ifdef SIGTSTP
/*
 * We have just gotten a susp.	Suspend and prepare to resume.
 */
void
onsusp(sig)
int sig;
{
	ttymode f;
	int savenormtty;
	int killreturn;
	struct winsize win;
	sigset_t set;
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"before onsusp\n");
#endif

        if (( state == VISUAL ) && ivis)
           vsave();

	f = setty(normf);
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after setty()\n");
#endif
	vnfl();
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after vnfl()\n");
#endif
	putpad(exit_ca_mode);
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after putpad()\n");
#endif
	flush();
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after flush()\n");
#endif
	resetterm();
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after resetterm()\n");
#endif
	savenormtty = normtty;
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after savenormtty()\n");
#endif
	normtty = 0;
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after normtty()\n");
#endif

	signal(SIGTSTP, SIG_DFL);
	sigemptyset(&set);
	sigaddset(&set, SIGTSTP);
	sigprocmask(SIG_UNBLOCK, &set, NULL);
	
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after signal()\n");
#endif
	killreturn=kill(0, SIGTSTP);

#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after kill--killreturn=%d\n",killreturn);
#endif
	/* the pc stops here */

	signal(SIGTSTP, onsusp);
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after signal()\n");
#endif
	normtty = savenormtty;
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after normtty\n");
#endif
	vcontin(0);
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after vcontin()\n");
#endif
	if ((state == VISUAL) && ivis){
		vgoto(destcol, destline); /* Puts cursor at same position as before Ctrl Z */
		vrepaint(cursor); /* Puts current line in center of screen if possable. */
	}
	setty(f);
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after setty() vcnt = %d, cursor = %d, vmcurs = %d, vcline = %d\n",vcnt, cursor, vmcurs, vcline);
#endif
	if (!inopen)
		error((char *)0);
	else {
		if (ioctl(0, TIOCGWINSZ, &win) >= 0)
			if (win.ws_row != winsz.ws_row ||
			    win.ws_col != winsz.ws_col)
				winchk();
		if (vcnt < 0) {
			vcnt = -vcnt;
			if (state == VISUAL)
				vclear();
			else if (state == CRTOPEN)
				vcnt = 0;
		}
		vdirty(0, lines);
		vrepaint(cursor);
	}
#ifdef SUSPTRACE
	if (trace)
		fprintf(trace,"in onsusp after if-- vcnt = %d, cursor = %d, vmcurs = %d, vcline = %d\n",vcnt, cursor, vmcurs, vcline);
#endif
}
#endif
