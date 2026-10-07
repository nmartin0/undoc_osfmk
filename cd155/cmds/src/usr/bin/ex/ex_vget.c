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
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char *rcsid = "@(#)$RCSfile: ex_vget.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:24:11 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_vget.c
 *
 * FUNCTION: addtext, addto, cancelalarm, ex_beep, fastpeekkey, getbr, getesc,
 * getkey, macpush, map, noteit, peekbr, peekkey, readecho, setBUF, setDEL,
 * setLAST, setalarm, trapalarm, ungetkey, vgetcnt, visdump, vudump
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
 * ex_vget.c  1.7  com/cmd/edit/vi,3.1,9013 3/2/90 12:46:30
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include <sys/time.h>
#include "ex.h"
#include "ex_tty.h"
#include "ex_vis.h"

static int fastpeekkey();

static int getbr();
static addto();
static setalarm();
static trapalarm();

/*
 * Input routines for open/visual.
 * We handle upper case only terminals in visual and reading from the
 * echo area here as well as notification on large changes
 * which appears in the echo area.
 */

/*
 * Return the key.
 */
ungetkey(c)
	int c;		/* mjm: char --> int */
{

	if (Peekkey != ATTN)
		Peekkey = c;
}

/*
 * Return a keystroke, but never a ^@.
 */
getkey()
{
	register int c;

	do {
		c = getbr();
		if (c==0)
			ex_beep();
	} while (c == 0);
	return (c);
}

/*
 * Tell whether next keystroke would be a ^@.
 */
peekbr()
{

	Peekkey = getbr();
	return (Peekkey == 0);
}

static short	precbksl;
jmp_buf readbuf;
int 	doingread = 0;

/*
 * Get a keystroke, including a ^@.
 * If an key was returned with ungetkey, that
 * comes back first.  Next comes unread input (e.g.
 * from repeating commands with .), and finally new
 * keystrokes.
 *
 * The hard work here is in mapping of \ escaped
 * characters on upper case only terminals.
 */
static int getbr()
{
	unsigned char ch;
	register int c, d;
	static char Peek2key;
	extern short slevel, ttyindes;

getATTN:
	if (Peekkey) {
		c = Peekkey;
		Peekkey = 0;
		return (c);
	}
	if (Peek2key) {
		c = Peek2key;
		Peek2key = 0;
		return (c);
	}
	if (vmacp) {
		if (*vmacp)
			return(*vmacp++);
		/* End of a macro or set of nested macros */
		vmacp = 0;
		if (inopen == -1)	/* don't screw up undo for esc esc */
			vundkind = VMANY;
		inopen = 1;	/* restore old setting now that macro done */
		vch_mac = VC_NOTINMAC;
	}
	if (vglobp) {
		if (*vglobp)
			return (lastvgk = *vglobp++);
		lastvgk = 0;
		return (ESCAPE);
	}
	flusho();
again:
	if (setjmp(readbuf))
		goto getATTN;
	doingread = 1;
	c=read(slevel == 0 ? 0 : ttyindes, &ch, 1); 
	doingread = 0;
	if (c != 1) {
		if (errno == EINTR)
			goto getATTN;
		error(msg(M_236, "Input read error"));
	}
	if (NCisshift(ch)) {
		if (read(slevel == 0 ? 0 : ttyindes, &Peek2key, 1) != 1) {
			if (errno == EINTR)
				goto getATTN;
			error(msg(M_236, "Input read error"));
		}
	}
	if (_NCis2(ch, Peek2key)) {
		c = _NCd2(ch, Peek2key);
		Peek2key = 0;
	} else {
		c = ch;
	}
	if (beehive_glitch && slevel==0 && c == ESCAPE) {
		if (read(0, &Peek2key, 1) != 1)
			goto getATTN;
		switch (Peek2key) {
		case 'C':	/* SPOW mode sometimes sends \EC for space */
			c = ' ';
			Peek2key = 0;
			break;
		case 'q':	/* f2 -> ^C */
			c = Ctrl('C');
			Peek2key = 0;
			break;
		case 'p':	/* f1 -> esc */
			Peek2key = 0;
			break;
		}
	}

	/*
	 * The algorithm here is that of the UNIX kernel.
	 * See the description in the programmers manual.
	 */
	if (UPPERCASE) {
		if (NCisupper(c))
			c = NCtolower(c);
		if (c == '\\') {
			if (precbksl < 2)
				precbksl++;
			if (precbksl == 1)
				goto again;
		} else if (precbksl) {
			d = 0;
			if (NCislower(c))
				d = NCtoupper(c);
			else {
				register char *colp;
				colp = "({)}!|^~'~";
				while (d = *colp++)
					if (d == c) {
						d = *colp++;
						break;
					} else
						colp++;
			}
			if (precbksl == 2) {
				if (!d) {
					Peekkey = c;
					precbksl = 0;
					c = '\\';
				}
			} else if (d)
				c = d;
			else {
				Peekkey = c;
				precbksl = 0;
				c = '\\';
			}
		}
		if (c != '\\')
			precbksl = 0;
	}
#ifdef TRACE
	if (trace) {
		if (!techoin) {
			tfixnl();
			techoin = 1;
			fprintf(trace, "*** Input: ");
		}
		tracec(c);
	}
#endif
	lastvgk = 0;
	return (c);
}

/*
 * Get a key, but if a delete, quit or attention
 * is typed return 0 so we will abort a partial command.
 */
getesc()
{
	register int c;			/* code point or EOF */

	c = getkey();
	switch (c) {

	case Ctrl('V'):
	case Ctrl('Q'):
		c = getkey();
		return (c);

	case ATTN:
	case QUIT:
		ungetkey(c);
		return (0);

	case ESCAPE:
		return (0);
	}
	return (c);
}

/*
 * Peek at the next keystroke.
 */
peekkey()
{

	Peekkey = getkey();
	return (Peekkey);
}

/*
 * Read a line from the echo area, with single character prompt c.
 * A return value of 1 means the user blewit or blewit away.
 */
readecho(c)
	NLchar c;
{
	register NLchar *sc = cursor;
	register int (*OP)();
	short waste;
	register int OPeek;

	if (WBOT == WECHO)
		vclean();
	else
		vclrech(0);
	splitw++;
	vgoto(WECHO, 0);
	ex_putchar(c);
	vclreol();
	vgoto(WECHO, 1);
	cursor = linebuf; linebuf[0] = 0; genbuf[0] = c;
	if (peekbr()) {
		if (!INS[0] || INS_OVFLOW == TRUE)
			goto blewit;
		vglobp = INS;
	}
	OP = Pline; Pline = normline;
	ignore(vgetline(0, genbuf + 1, &waste, c));
	if (Outchar == termchar)
		ex_putchar('\n');
	vscrap();
	Pline = OP;
	if (Peekkey != ATTN && Peekkey != Ctrl('H')) {
		cursor = sc;
		vclreol();
		return (0);
	}
blewit:
	OPeek = Peekkey==Ctrl('H') ? 0 : Peekkey; Peekkey = 0;
	splitw = 0;
	vclean();
	vshow(dot, NOLINE);
	vnline(sc);
	Peekkey = OPeek;
	return (1);
}

/*
 * A complete command has been defined for
 * the purposes of repeat, so copy it from
 * the working to the previous command buffer.
 */
setLAST()
{

	if (vglobp || vmacp)
		return;
	lastreg = vreg;
	lasthad = Xhadcnt;
	lastcnt = Xcnt;
	*lastcp = 0;
	CP(lastcmd, workcmd);
}

/*
 * Gather up some more text from an insert.
 * If the insertion buffer oveflows, then destroy
 * the repeatability of the insert.
 */
addtext(cp)
	NLchar *cp;
{

	if (vglobp)
		return;
	addto(INS, &INS_OVFLOW, cp);
	if (INS_OVFLOW == TRUE)
		lastcmd[0] = 0;
}

setDEL()
{

	setBUF(DEL,&DEL_OVFLOW);
}

/*
 * Put text from cursor upto wcursor in BUF.
 */
setBUF(BUF,overflow)
	register NLchar *BUF;
	register short * overflow;		/* buffer overflow */
{
	register NLchar c;
	register NLchar *wp = wcursor;

	c = *wp;
	*wp = 0;
	BUF[0] = 0;
	*overflow = FALSE;
	addto(BUF, overflow, cursor);
	*wp = c;
}

static addto(buf, overflow, str)
	register NLchar *buf, *str;
	register short *overflow;
{

	if (*overflow == TRUE)
		return;
	if (NCstrlen(buf) + NCstrlen(str) + 1 >= VBSIZE) {
		*overflow = TRUE;
		return;
	}
	ignore(NCstrcat(buf, str));
}

/*
 * Note a change affecting a lot of lines, or non-visible
 * lines.  If the parameter must is set, then we only want
 * to do this for open modes now; return and save for later
 * notification in visual.
 */
noteit(must)
	short must;
{
	register int sdl = destline, sdc = destcol;

	if (notecnt < 2 || !must && state == VISUAL)
		return (0);
	splitw++;
	if (WBOT == WECHO)
		vmoveitup(1, 1);
	vigoto(WECHO, 0);
/*! Not sure whether notesgn==0 is in fact possible -- check this !*/
	if (notesgn > 0) {
		ex_printf(msg(M_284, "%d more lines"), notecnt);
	} else if (notesgn < 0) {
		ex_printf(msg(M_285, "%d fewer lines"), notecnt);
	} else {
		ex_printf(msg(M_286, "%d lines"), notecnt);
	}
	if (*notenam) {
		ex_printf(" %s", notenam);
		if (*(strend(notenam) - 1) != 'e')
			ex_putchar('e');
		ex_putchar('d');
	}
	vclreol();
	notecnt = 0;
	if (state != VISUAL)
		vcnt = vcline = 0;
	splitw = 0;
	if (state == ONEOPEN || state == CRTOPEN)
		vup1();
	destline = sdl; destcol = sdc;
	return (1);
}

/*
 * Rrrrringgggggg.
 * If possible, flash screen.
 */
ex_beep()
{
	if (flash_screen && value(FLASH))
		vputp(flash_screen, 0);
	else if (bell)
		vputp(bell, 0);
}

/*
 * Map the command input character c,
 * for keypads and labelled keys which do cursor
 * motions.  I.e. on an adm3a we might map ^K to ^P.
 * DM1520 for example has a lot of mappable characters.
 */

map(c,maps)
	register NLchar c;
	register struct maps *maps;
{
	register int d;
	register NLchar *p;
	register NLchar *q;
	NLchar b[10];	/* Assumption: no keypad sends string longer than 10 */

	/*
	 * Mapping for special keys on the terminal only.
	 * BUG: if there's a long sequence and it matches
	 * some chars and then misses, we lose some chars.
	 *
	 * For this to work, some conditions must be met.
	 * 1) Keypad sends SHORT (2 or 3 char) strings
	 * 2) All strings sent are same length & similar
	 * 3) The user is unlikely to type the first few chars of
	 *    one of these strings very fast.
	 * Note: some code has been fixed up since the above was laid out,
	 * so conditions 1 & 2 are probably not required anymore.
	 * However, this hasn't been tested with any first char
	 * that means anything else except escape.
	 */
#ifdef MDEBUG
	if (trace)
		fprintf(trace,"map(%c): ",c);
#endif
	/*
	 * If c==0, the NLchar came from getesc typing escape.  Pass it through
	 * unchanged.  0 messes up the following code anyway.
	 */
	if (c==0)
		return(0);

	b[0] = c;
	b[1] = 0;
	for (d=0; maps[d].mapto; d++) {
#ifdef MDEBUG
		if (trace)
			fprintf(trace,"\ntry '%s', ",maps[d].cap);
#endif
		if (p = maps[d].cap) {
			for (q=b; *p; p++, q++) {
#ifdef MDEBUG
				if (trace)
					fprintf(trace,"q->b[%d], ",q-b);
#endif
				if (*q==0) {
					/*
					 * Is there another NLchar waiting?
					 *
					 * This test is oversimplified, but
					 * should work mostly. It handles the
					 * case where we get an ESCAPE that
					 * wasn't part of a keypad string.
					 */
					if ((c=='#' ? peekkey() : fastpeekkey()) == 0) {
#ifdef MDEBUG
						if (trace)
							fprintf(trace,"fpk=0: will return '%c'",c);
#endif
						/*
						 * Nothing waiting.  Push back
						 * what we peeked at & return
						 * failure (c).
						 *
						 * We want to be able to undo
						 * commands, but it's nonsense
						 * to undo part of an insertion
						 * so if in input mode don't.
						 */
#ifdef MDEBUG
						if (trace)
							fprintf(trace, "Call macpush, b %d %d %d\n", b[0], b[1], b[2]);
#endif
						macpush(&b[1],maps == arrows);
#ifdef MDEBUG
						if (trace)
							fprintf(trace, "return %d\n", c);	
#endif
						return(c);
					}
					*q = getkey();
					q[1] = 0;
				}
				if (*p != *q)
					goto contin;
			}
			macpush(maps[d].mapto,maps == arrows);
			c = getkey();
#ifdef MDEBUG
			if (trace)
				fprintf(trace,"Success: push(%s), return %c",maps[d].mapto, c);
#endif
			return(c);	/* first NLchar of map string */
			contin:;
		}
	}
#ifdef MDEBUG
	if (trace)
		fprintf(trace,"Fail: push(%s), return %c", &b[1], c);
#endif
	macpush(&b[1],0);
	return(c);
}

/*
 * Push st onto the front of vmacp. This is tricky because we have to
 * worry about where vmacp was previously pointing. We also have to
 * check for overflow (which is typically from a recursive macro)
 * Finally we have to set a flag so the whole thing can be undone.
 * canundo is 1 iff we want to be able to undo the macro.  This
 * is false for, for example, pushing back lookahead from fastpeekkey(),
 * since otherwise two fast escapes can clobber our undo.
 */
macpush(st, canundo)
NLchar *st;
int canundo;
{
	NLchar tmpbuf[BUFFERSIZ];

	if (st==0 || *st==0)
		return;
#ifdef MDEBUG
	if (trace){
		fprintf(trace, "*** From canundo(): ");
		NLfprintf(trace, "macpush(%S), canundo=%d\n",st,canundo);
	}
#endif
	if ((vmacp ? NCstrlen(vmacp) : 0) + NCstrlen(st) > BUFFERSIZ)
		error(msg(M_238, "Macro too long@ - maybe recursive?"));
	if (vmacp) {
		NCstrcpy(tmpbuf, vmacp);
		if (!FIXUNDO)
			canundo = 0;	/* can't undo inside a macro anyway */
	}
	NCstrcpy(vmacbuf, st);
	if (vmacp)
		NCstrcat(vmacbuf, tmpbuf);
	vmacp = vmacbuf;
	/* arrange to be able to undo the whole macro */
	if (canundo) {
		vch_mac = VC_NOCHANGE;
	}
}

#ifdef UNDOTRACE
visdump(s)
NLchar *s;
{
	register int i;

	if (!trace) return;

	NLfprintf(trace, "\n%S: basWTOP=%d, basWLINES=%d, WTOP=%d, WBOT=%d, WLINES=%d, WCOLS=%d, WECHO=%d\n",
		s, basWTOP, basWLINES, WTOP, WBOT, WLINES, WCOLS, WECHO);
	fprintf(trace, "   vcnt=%d, vcline=%d, cursor=%d, wcursor=%d, wdot=%d\n",
		vcnt, vcline, cursor-linebuf, wcursor-linebuf, wdot-zero);
	fprintf(trace, "*** From visdump(): ");
	for (i=0; i<TUBELINES; i++)
		if (vtube[i] && *vtube[i])
			NLfprintf(trace, "%d: '%S'\n", i, vtube[i]);
	tvliny();
}

vudump(s)
NLchar *s;
{
	register line *p;
	NLchar savelb[1024];

	if (!trace) return;

	NLfprintf(trace, "\n%S: undkind=%d, vundkind=%d, unddel=%d, undap1=%d, undap2=%d,\n",
		s, undkind, vundkind, lineno(unddel), lineno(undap1), lineno(undap2));
	fprintf(trace, "  undadot=%d, dot=%d, dol=%d, unddol=%d, truedol=%d\n",
		lineno(undadot), lineno(dot), lineno(dol), lineno(unddol), lineno(truedol));
	fprintf(trace, "  [\n");
	CP(savelb, linebuf);
	NLfprintf(trace, "linebuf = '%S'\n", linebuf);
	for (p=zero+1; p<=truedol; p++) {
		fprintf(trace, "%o ", *p);
		getline(*p);
		NLfprintf(trace, "'%S'\n", linebuf);
	}
	fprintf(trace, "]\n");
	CP(linebuf, savelb);
}
#endif

/*
 * Get a count from the keyed input stream.
 * A zero count is indistinguishable from no count.
 */
vgetcnt()
{
	register int c;
	register int cnt;

	cnt = 0;
	for (;;) {
		c = getkey();
		if (!(isascii(c) && isdigit(c)))
			break;
		cnt *= 10, cnt += c - '0';
	}
	ungetkey(c);
	Xhadcnt = 1;
	Xcnt = cnt;
	return(cnt);
}

/*
 * fastpeekkey is just like peekkey but insists the character come in
 * fast (within 1 second). This will succeed if it is the 2nd NLchar of
 * a machine generated sequence (such as a function pad from an escape
 * flavor terminal) but fail for a human hitting escape then waiting.
 */
static
fastpeekkey()
{
	int trapalarm();
	register int c;

	/*
	 * If the user has set notimeout, we wait forever for a key.
	 * If we are in a macro we do too, but since it's already
	 * buffered internally it will return immediately.
	 * In other cases we force this to die in 1 second.
	 * This is pretty reliable (VMUNIX rounds it to .5 - 1.5 secs,
	 * but UNIX truncates it to 0 - 1 secs) but due to system delays
	 * there are times when arrow keys or very fast typing get counted
	 * as separate.  notimeout is provided for people who dislike such
	 * nondeterminism.
	 */
	CATCH
		if (value(TIMEOUT) && inopen >= 0) {
			signal(SIGALRM, (void (*)(int))trapalarm);
			setalarm();
		}
		c = peekkey();
		cancelalarm();
	ONERR
		c = 0;
	ENDCATCH
	/* Should have an alternative method based on select for 4.2BSD */
	return(c);
}

#ifdef FTIOCSET
static int ftfd;
struct requestbuf {
	short time;
	short signo;
};
#endif

/*
 * Arrange for SIGALRM to come in shortly, so we don't
 * hang very long if the user didn't type anything.  There are
 * various ways to do this on different systems.
 */
static setalarm()
{
#ifdef FTIOCSET
	/*
	 * Use nonstandard "fast timer" to get better than
	 * one second resolution.  We must wait at least
	 * 1/15th of a second because some keypads don't
	 * transmit faster than this.
	 */

	/* Open ft psuedo-device - we need our own copy. */
	if (ftfd == 0) {
		char ftname[20];
		strcpy(ftname, "/dev/ft0");
		while (ftfd <= 0 && ftname[7] <= '~') {
			ftfd = open(ftname, 0);
			if (ftfd <= 0)
				ftname[7] ++;
		}
	}
	if (ftfd <= 0) {	/* Couldn't open a /dev/ft? */
		alarm(1);
	} else {
		struct requestbuf rb;
		rb.time = 6;	/* 6 ticks = 100 ms > 67 ms. */
		rb.signo = SIGALRM;
		ioctl(ftfd, FTIOCSET, &rb);
	}
#else
#ifdef _H_SYS_TIME
	ualarm(500000,0);  /* use microsec timer at 1/15 sec if available */
#else
	/*
	 * No special capabilities, so we use alarm, with 1 sec. resolution.
	 */
	alarm(1);
#endif
#endif
}

/*
 * Get rid of any impending incoming SIGALRM.
 */
cancelalarm()
{
#ifdef FTIOCSET
	if (ftfd > 0) {
		struct requestbuf rb;
		rb.time = 0;
		rb.signo = SIGALRM;
		ioctl(ftfd, FTIOCCANCEL, &rb);
	}
#endif
	alarm(0);	/* Have to do this whether or not FTIOCSET */
}

static trapalarm() {
	alarm(0);
	longjmp(vreslab,1);
}
