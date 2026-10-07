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
static char *rcsid = "@(#)$RCSfile: ex_put.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:00 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_put.c
 *
 * FUNCTION: fgoto, draino, flush, flush1, flush2, flusho, gTTY, gotab,
 * listchar, lprintf, noonl, normal, normchar, normline, noteinp, numbline,
 * ostart, ostop, plod, plodput, pstart, pstop, putNFL, putch, ex_putchar,
 * putnl, putpad, sTTY, setlist, setnumb, setoutt, setty, slobber, termchar,
 * termreset, tostart, tostop, ttcharoff, vcook, vraw
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
 * ex_put.c  1.7  com/cmd/edit/vi,3.1,9013 2/20/90 07:10:17
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_tty.h"
#include "ex_vis.h"

/*
 * Terminal driving and line formatting routines.
 * Basic motion optimizations are done here as well
 * as formatting of lines (printing of control characters,
 * line numbering and the like).
 */

/*
 * The routines outchar, putchar and pline are actually
 * variables, and these variables point at the current definitions
 * of the routines.  See the routine setflav.
 * We sometimes make outchar be routines which catch the characters
 * to be printed, e.g. if we want to see how long a line is.
 * During open/visual, outchar and putchar will be set to
 * routines in the file ex_vput.c (vputchar, vinschar, etc.).
 */
static	int	normchar();
int	(*Outchar)() = termchar;
int	(*Putchar)() = normchar;
int	(*Pline)() = normline;
# ifdef 	TIOCSETC	/* V7 */
struct tchars nttyc, ottyc;
# endif
# ifdef		TIOCLGET	/* Berkeley 4BSD */
struct ltchars nlttyc, olttyc;
# endif
struct termios posnttyc, posottyc;

static slobber();
static flush2();
static plod();
static ttcharoff();
static sTTY();

int (*
setlist(t))()
	short t;
{
	register int (*P)();

	listf = t;
	P = Putchar;
	Putchar = t ? listchar : normchar;
	return (P);
}

int (*
setnumb(t))()
	short t;
{
	register int (*P)();

	numberf = t;
	P = Pline;
	Pline = t ? numbline : normline;
	return (P);
}

/*
 * Format c for list mode; leave things in common
 * with normal print mode to be done by normchar.
 */
listchar(c)
	register NLchar c;
{
	if (c == QUOTE_NL) {
		outchar('$');
	} else if (c != '\n' && is_cntrl(c)) {
		outchar('^');
		c = ctlof(c);
	}
	normchar(c);
}

/*
 * Format c for printing.  Handle funnies of upper case terminals
 * and crocky hazeltines which don't have ~.
 */
static normchar(c)
	register int c;
{
	register  char *colp;

	if (c == '~' && tilde_glitch) {
		normchar('\\');
		c = '^';
	}
	if (is_cntrl(c) && (c != '\b' || !over_strike) && c != '\n' && c != '\t') {
		ex_putchar('^');
		c = ctlof(c);
	} else {
		switch (c) {
		case QUOTE_NUL:
#if defined(KJI)
		case QUOTE_KJI:
#endif
			return;
		case QUOTE_SP:
		case QUOTE_BSP:
			break;
		case QUOTE_NL:
			c = '\n';
			break;
		case QUOTE_CR:
			c = '\r';
			break;
		default:
			if (UPPERCASE) {
				if (NCisupper(c)) {
					outchar('\\');
					c = NCtolower(c);
				} else {
					colp = "({)}!|^~'`";
					while (*colp++) {
						if (c == *colp++) {
							outchar('\\');
							c = colp[-2];
							break;
						}
					}
				}
			}
		}
	}
	outchar(c);
}

/*
 * Print a line with a number.
 */
numbline(i)
	int i;
{

	if (shudclob)
		slobber(' ');
	ex_printf("%6d  ", i);
	normline();
}

/*
 * Normal line output, no numbering.
 */
normline()
{
	register NLchar *cp;

	if (shudclob)
		slobber(linebuf[0]);
	/* pdp-11 doprnt is not reentrant so can't use "printf" here
	   in case we are tracing */
	for (cp = linebuf; *cp;)
		ex_putchar(*cp++);
	if (!inopen)
		ex_putchar(QUOTE_NL);
}

/*
 * Given c at the beginning of a line, determine whether
 * the printing of the line will erase or otherwise obliterate
 * the prompt which was printed before.  If it won't, do it now.
 */
static slobber(c)
	int c;
{

	shudclob = 0;
	switch (c) {

	case '\t':
		if (Putchar == listchar)
			return;
		break;

	default:
		return;

	case ' ':
	case 0:
		break;
	}
	if (over_strike)
		return;
	flush();
	putch(' ');
	tputs(cursor_left, 0, putch);
}

static	NLchar	linb[66];
static	NLchar	*linp = linb;

/*
 * Phadnl records when we have already had a complete line ending with \n.
 * If another line starts without a flush, and the terminal suggests it,
 * we switch into -nl mode so that we can send lineffeeds to avoid
 * a lot of spacing.
 */
static	short phadnl;

/*
 * Indirect to current definition of putchar.
 */
ex_putchar(int c)
{

	(*Putchar)(c);
}

/*
 * Termchar routine for command mode.
 * Watch for possible switching to -nl mode.
 * Otherwise flush into next level of buffering when
 * small buffer fills or at a newline.
 */
termchar(c)
	int c;
{

	if (pfast == 0 && phadnl)
		pstart();
	if (c == '\n')
		phadnl = 1;
	else if (linp >= &linb[63])
		flush1();
	*linp++ = c;
	if (linp >= &linb[62]) {
		fgoto();
		flush1();
	}
}

flush()
{

	flush1();
	flush2();
}

/*
 * Flush from small line buffer into output buffer.
 * Work here is destroying motion into positions, and then
 * letting fgoto do the optimized motion.
 */
flush1()
{
	register NLchar *lp;
	register NLchar c;

	*linp = 0;
	lp = linb;
	while (*lp) {
		switch (c = *lp++) {

		case '\r':
			destline += destcol / columns;
			destcol = 0;
			continue;

		case '\b':
#if defined(KJI)
/*! ignore jxterm bug that moves across an entire double-wide character !*/
#endif
			if (destcol)
				destcol--;
			continue;

		case ' ':
			destcol++;
			continue;

		case '\t':
			destcol += value(TABSTOP) - destcol % value(TABSTOP);
			continue;

		case '\n':
			destline += destcol / columns + 1;
			if (destcol != 0 && destcol % columns == 0)
				destline--;
			destcol = 0;
			continue;

		default:
			fgoto();
			for (;;) {
				if (auto_right_margin == 0 && outcol == columns)
					fgoto();
				/* c &= TRIM; */
				switch (c) {
				case QUOTE_NUL:
					c = '\0';
					break;
				case QUOTE_BSP:
					c = '\b';
					break;
				case QUOTE_SP:
					c = ' ';
					break;
				}
				putch(c);
				if (c == '\b') {
/*
 * Note: This is only used in one place -- to erase the '^' or '0' before a ^D
 * -- so we don't need to worry about double-wide characters here.
 */
					putch(' ');
					putch('\b');
					outcol--;
					destcol--;
				} else if (is_print(c)) {
#if !defined(KJI)
					outcol++;
					destcol++;
#else
					outcol += PRINTWID(c);
					destcol += PRINTWID(c);
#endif
					if (eat_newline_glitch && outcol % columns == 0) {
						putch('\r');
						putch('\n');
					}
				}
				c = *lp++;
				if (c <= ' ')
					break;
			}
			--lp;
			continue;
		}
	}
	linp = linb;
}

static flush2()
{

	fgoto();
	flusho();
	pstop();
}

/*
 * Sync the position of the output cursor.
 * Most work here is rounding for terminal boundaries getting the
 * column position implied by wraparound or the lack thereof and
 * rolling up the screen to get destline on the screen.
 */
fgoto()
{
	register int l, c;

	if (destcol > columns - 1) {
		destline += destcol / columns;
		destcol %= columns;
	}
	if (outcol > columns - 1) {
		l = (outcol + 1) / columns;
		outline += l;
		outcol %= columns;
		if (auto_right_margin == 0) {
			while (l > 0) {
				if (pfast)
					tputs(carriage_return, 0, putch);
				tputs(cursor_down, 0, putch);
				l--;
			}
			outcol = 0;
		}
		if (outline > lines - 1) {
			destline -= outline - (lines - 1);
			outline = lines - 1;
		}
	}
	if (destline > lines - 1) {
		l = destline;
		destline = lines - 1;
		if (outline < lines - 1) {
			c = destcol;
			if (pfast == 0 && (!cursor_address || holdcm))
				destcol = 0;
			fgoto();
			destcol = c;
		}
		while (l > lines - 1) {
			/*
			 * The following linefeed (or simulation thereof)
			 * is supposed to scroll up the screen, since we
			 * are on the bottom line.
			 *
			 * Superbee glitch:  in the middle of the screen we
			 * have to use esc B (down) because linefeed screws up
			 * in "Efficient Paging" (what a joke) mode (which is
			 * essential in some SB's because CRLF mode puts garbage
			 * in at end of memory), but you must use linefeed to
			 * scroll since down arrow won't go past memory end.
			 * I turned this off after recieving Paul Eggert's
			 * Superbee description which wins better.
			 */
			if (scroll_forward /* && !beehive_glitch */ && pfast)
				tputs(scroll_forward, 0, putch);
			else
				putch('\n');
			l--;
			if (pfast == 0)
				outcol = 0;
		}
	}
	if (destline < outline && !(cursor_address && !holdcm || cursor_up || cursor_home))
		destline = outline;
	if (cursor_address && !holdcm)
		if (plod(costCM) > 0)
			plod(0);
		else
			tputs(tparm(cursor_address, destline, destcol), 0, putch);
	else
		plod(0);
	outline = destline;
	outcol = destcol;
}

/*
 * Tab to column col by flushing and then setting destcol.
 * Used by "set all".
 */
gotab(col)
	int col;
{

	flush1();
	destcol = col;
}

/*
 * Move (slowly) to destination.
 * Hard thing here is using home cursor on really deficient terminals.
 * Otherwise just use cursor motions, hacking use of tabs and overtabbing
 * and backspace.
 */

static int plodcnt, plodflg;

static plodput(c)
{

	if (plodflg)
		plodcnt--;
	else
		putch(c);
}

static plod(cnt)
{
	register int i, j, k;
	register int soutcol, soutline;

	plodcnt = plodflg = cnt;
	soutcol = outcol;
	soutline = outline;
	/*
	 * Consider homing and moving down/right from there, vs moving
	 * directly with local motions to the right spot.
	 */
	if (cursor_home) {
		/*
		 * i is the cost to home and tab/space to the right to
		 * get to the proper column.  This assumes cursor_right costs
		 * 1 char.  So i+destcol is cost of motion with home.
		 */
		if (tab)
			i = (destcol / value(HARDTABS)) + (destcol % value(HARDTABS));
		else
			i = destcol;
		/*
		 * j is cost to move locally without homing
		 */
		if (destcol >= outcol) {	/* if motion is to the right */
			j = destcol / value(HARDTABS) - outcol / value(HARDTABS);
			if (tab && j)
				j += destcol % value(HARDTABS);
			else
				j = destcol - outcol;
		} else
			/* leftward motion only works if we can backspace. */
			if (outcol - destcol <= i && (cursor_left))
				i = j = outcol - destcol; /* cheaper to backspace */
			else
				j = i + 1; /* impossibly expensive */

		/* k is the absolute value of vertical distance */
		k = outline - destline;
		if (k < 0)
			k = -k;
		j += k;

		/*
		 * Decision.  We may not have a choice if no cursor_up.
		 */
		if (i + destline < j || (!cursor_up && destline < outline)) {
			/*
			 * Cheaper to home.  Do it now and pretend it's a
			 * regular local motion.
			 */
			tputs(cursor_home, 0, plodput);
			outcol = outline = 0;
		} else if (cursor_to_ll) {
			/*
			 * Quickly consider homing down and moving from there.
			 * Assume cost of cursor_to_ll is 2.
			 */
			k = (lines - 1) - destline;
			if (i + k + 2 < j && (k<=0 || cursor_up)) {
				tputs(cursor_to_ll, 0, plodput);
				outcol = 0;
				outline = lines - 1;
			}
		}
	} else
		/*
		 * No home and no up means it's impossible, so we return an
		 * incredibly big number to make cursor motion win out.
		 */
		if (!cursor_up && destline < outline)
			return (500);
	if (tab)
		i = destcol % value(HARDTABS)
		    + destcol / value(HARDTABS);
	else
		i = destcol;
		j = outcol - destcol;
	/*
	 * If we will later need a \n which will turn into a \r\n by
	 * the system or the terminal, then don't bother to try to \r.
	 */
	if ((NONL || !pfast) && outline < destline)
		goto dontcr;
	/*
	 * If the terminal will do a \r\n and there isn't room for it,
	 * then we can't afford a \r.
	 */
	if (!carriage_return && outline >= destline)
		goto dontcr;
	/*
	 * If we've scrolled past the echo area, then we don't want to
	 * go too far left then recover, as this may overwrite significant
	 * characters with junk.  E.g., type a ':' in vi mode, and type more
	 * than a line's worth, and then a ^X.  We don't want to trounce
	 * the ':'.
	 */
	if (splitw && (Outchar == termchar) && (destcol != 0))
		goto dontcr;
	/*
	 * If it will be cheaper, or if we can't back up, then send
	 * a return preliminarily.
	 */
	if (j > i + 1 || outcol > destcol && !cursor_left) {
		/*
		 * BUG: this doesn't take the (possibly long) length
		 * of carriage_return into account.
		 */
		if (carriage_return) {
			tputs(carriage_return, 0, plodput);
			outcol = 0;
		} else if (newline) {
			tputs(newline, 0, plodput);
			outline++;
			outcol = 0;
		}
	}
dontcr:
	/* Move down, if necessary, until we are at the desired line */
	while (outline < destline) {
		j = destline - outline;
		if (j > costDP && parm_down_cursor) {
			/* Win big on Tek 4025 */
			tputs(tparm(parm_down_cursor, j), j, plodput);
			outline += j;
		}
		else {
			outline++;
			if (cursor_down && pfast)
				tputs(cursor_down, 0, plodput);
			else
				plodput('\n');
		}
		if (plodcnt < 0)
			goto out;
		if (NONL || pfast == 0)
			outcol = 0;
	}
	if (back_tab){
		k = strlen(back_tab);	/* should probably be cost(back_tab) and moved out */
		/*
		 * Make sure we don't go too far left if we've scrolled
		 * out of the echo area.
		 */
		if (splitw && (Outchar == termchar) && (value(HARDTABS) > 4))
			k += value(HARDTABS) - 4;
	}
	/* Move left, if necessary, to desired column */
	while (outcol > destcol) {
		if (plodcnt < 0)
			goto out;
		if (back_tab && !insmode && outcol - destcol > 4+k) {
			tputs(back_tab, 0, plodput);
			outcol--;
			outcol -= outcol % value(HARDTABS); /* outcol &= ~7; */
			continue;
		}
		j = outcol - destcol;
		if (j > costLP && parm_left_cursor) {
			tputs(tparm(parm_left_cursor, j), j, plodput);
			outcol -= j;
		}
		else {
			outcol--;
			tputs(cursor_left, 0, plodput);
		}
	}
	/* Move up, if necessary, to desired row */
	while (outline > destline) {
		j = outline - destline;
		if (parm_up_cursor && j > 1) {
			/* Win big on Tek 4025 */
			tputs(tparm(parm_up_cursor, j), j, plodput);
			outline -= j;
		}
		else {
			outline--;
			tputs(cursor_up, 0, plodput);
		}
		if (plodcnt < 0)
			goto out;
	}
	/*
	 * Now move to the right, if necessary.  We first tab to
	 * as close as we can get.
	 */
	if (tab && !insmode && destcol - outcol > 1) {
		/* tab to right as far as possible without passing col */
		for (;;) {
			i = tabcol(outcol, value(HARDTABS));
			if (i > destcol)
				break;
			if (tab)
				tputs(tab, 0, plodput);
			else
				plodput('\t');
			outcol = i;
		}
		/* consider another tab and then some backspaces */
		if (destcol - outcol > 4 && i < columns && cursor_left) {
			tputs(tab, 0, plodput);
			outcol = i;
			/*
			 * Back up.  Don't worry about parm_left_cursor because
			 * it's never more than 4 spaces anyway.
			 */
			while (outcol > destcol) {
				outcol--;
				tputs(cursor_left, 0, plodput);
			}
		}
	}
	/*
	 * We've tabbed as much as possible.  If we still need to go
	 * further (not exact or can't tab) space over.  This is a
	 * very common case when moving to the right with space.
	 */
	while (outcol < destcol) {
		j = destcol - outcol;
		if (j > costRP && parm_right_cursor) {
			/*
			 * This probably happens rarely, if at all.
			 * It seems mainly useful for ANSI terminals
			 * with no hardware tabs, and I don't know
			 * of any such terminal at the moment.
			 */
			tputs(tparm(parm_right_cursor, j), j, plodput);
			outcol += j;
		}
		else {
			/*
			 * move one column to the right.  We don't use right
			 * because it's better to just print the character we
			 * are moving over.  There are various exceptions,
			 * however.
			 * If !inopen, vtube contains garbage.  If the character
			 * is a null or a tab we want to print a space.  Other
			 * random chars we use space for instead, too.
			 */
			/*
			 * In addition, if we've scrolled past the echo area,
			 * we have avoided backing up too far, so a forward
			 * space means we need a blank.
			 */
			if (!inopen || (vtube[outline]==NULL) ||
			    (splitw && (Outchar == termchar)) ||
				(i=vtube[outline][outcol]) < ' ')
				i = ' ';
			if (i == QUOTE_NUL || i == QUOTE_SP)
				i = ' ';
#if !defined(KJI)
			if (insmode && cursor_right)
				tputs(cursor_right, 0, plodput);
			else
				plodput(i);
			outcol++;
#else
			if (cursor_right && (insmode || i == QUOTE_KJI || j < PRINTWID(i))) {
				tputs(cursor_right, 0, plodput);
				outcol++;
			} else {
				plodput(i);
				outcol += PRINTWID(i);
			}
#endif
		}
		if (plodcnt < 0)
			goto out;
	}
out:
	if (plodflg) {
		outcol = soutcol;
		outline = soutline;
	}
	return(plodcnt);
}

/*
 * An input line arrived.
 * Calculate new (approximate) screen line position.
 * Approximate because kill character echoes newline with
 * no feedback and also because of long input lines.
 */
noteinp()
{

	outline++;
	if (outline > lines - 1)
		outline = lines - 1;
	destline = outline;
	destcol = outcol = 0;
}

/*
 * Something weird just happened and we
 * lost track of whats happening out there.
 * Since we cant, in general, read where we are
 * we just reset to some known state.
 * On cursor addressible terminals setting to unknown
 * will force a cursor address soon.
 */
termreset()
{

	endim();
	if (enter_ca_mode)
		putpad(enter_ca_mode);	
	destcol = 0;
	destline = lines - 1;
	if (cursor_address) {
		outcol = UKCOL;
		outline = UKCOL;
	} else {
		outcol = destcol;
		outline = destline;
	}
}

/*
 * Low level buffering, with the ability to drain
 * buffered output without printing it.
 */
static char	*obp = obuf;

draino()
{

	obp = obuf;
}

flusho()
{
	if ((obp != obuf)  && !hush) { 
		write(1, obuf, obp - obuf);
#ifdef TRACE
		if (trace)
			fwrite(obuf, 1, obp-obuf, trace);
#endif
		obp = obuf;
	}
}

putnl()
{

	ex_putchar('\n');
}

putch(c)
	int c;
{

#ifdef OLD3BTTY		/* mjm */
	if(c == '\n')	/* mjm: Fake "\n\r" for '\n' til fix in 3B firmware */
		putch('\r');	/* mjm: vi does "stty -icanon" => -onlcr !! */
#endif
#if defined(KJI)
	if (c == QUOTE_KJI) return;	/*! This probably isn't possible !*/
	if (c == QUOTE_BULLET) c = svalue(PARTIALCHAR)[0];
#endif
	if (c == QUOTE_NUL) c = '\0';	/*! This probably isn't possible !*/
	if (c == QUOTE_SP) c = ' ';
	NLsputc(obp, c);
	if (obp >= &obuf[sizeof obuf]-1)
		flusho();
}

/*
 * Miscellaneous routines related to output.
 */

/*
 * Put with padding
 */
putpad(cp)
	char *cp;
{

	flush();
	tputs(cp, 0, putch);
}

/*
 * Set output through normal command mode routine.
 */
setoutt()
{

	Outchar = termchar;
}

/*
 * Printf (temporarily) in list mode.
 */
/*VARARGS2*/
lprintf(cp, dp)
	char *cp, *dp;
{
	register int (*P)();

	P = setlist(1);
	ex_printf(cp, dp);
	Putchar = P;
}

/*
 * Newline + flush.
 */
putNFL()
{

	putnl();
	flush();
}

/*
 * Try to start -nl mode.
 */
pstart()
{

	if (NONL)
		return;
 	if (!value(OPTIMIZE))
		return;
	if (ruptible == 0 || pfast)
		return;
	fgoto();
	flusho();
	pfast = 1;
	normtty++;
#ifndef USG
	tty.sg_flags = normf & ~(ECHO|XTABS|CRMOD);
#else
	tty = normf;
	tty.c_oflag &= ~(ONLCR|TAB3);
	tty.c_lflag &= ~ECHO;
#endif
	saveterm();
	sTTY(2);
}

/*
 * Stop -nl mode.
 */
pstop()
{

	if (inopen)
		return;
	phadnl = 0;
	linp = linb;
	draino();
	normal(normf);
	pfast &= ~1;
}

/*
 * Prep tty for open mode.
 */
ttymode
ostart()
{
	ttymode f;

	/*
	if (!intty)
		error("Open and visual must be used interactively");
	*/
	gTTY(2);
	normtty++;
#ifndef USG
	f = tty.sg_flags;
	tty.sg_flags = (normf &~ (ECHO|XTABS|CRMOD)) |
# ifdef CBREAK
							CBREAK;
# else
							RAW;
# endif
# ifdef TIOCGETC
	ttcharoff();
# endif
#else
	f = tty;
	tty = normf;
	tty.c_iflag &= ~ICRNL;
	tty.c_lflag &= ~(ECHO|ICANON);
	tty.c_oflag &= ~(TAB3|ONLCR);
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;
	ttcharoff();
#endif
	sTTY(2);
	tostart();
	pfast |= 2;
	saveterm();
	return (f);
}

/* actions associated with putting the terminal in open mode */
tostart()
{
	putpad(cursor_visible);
	putpad(keypad_xmit);
	if (!value(MESG)) {
		if (ttynbuf[0] == 0) {
			register char *tn;
			if ((tn=ttyname(2)) == NULL &&
			    (tn=ttyname(1)) == NULL &&
			    (tn=ttyname(0)) == NULL)
				ttynbuf[0] = 1;
			else
				strcpy(ttynbuf, tn);
		}
		if (ttynbuf[0] != 1) {
			struct stat sbuf;
			stat(ttynbuf, &sbuf);
			ttymesg = sbuf.st_mode & 0777;
			chmod(ttynbuf,
#ifdef UCBV7
	/*
	 * This applies to the UCB V7 Pdp-11 system with the
	 * -u write option only.
	 */
					0611	/* 11 = urgent only allowed */
#else
					0600
						);
#endif
		}
	}
}

/*
 * Turn off start/stop chars if they aren't the default ^S/^Q.
 * This is so idiots who make esc their start/stop don't lose.
 * We always turn off quit since datamedias send ^\ for their
 * right arrow key.
 * For NLS code page useability, the '\377' entries in the code
 * below were changed to '\000'then back when codepage 850 came in.
 * 
 */
static

#ifdef USG

ttcharoff()
{
	posnttyc.c_cc[VDSUSP] = '\377';	/* ^Y */
	tty.c_cc[VQUIT] = '\377';
#ifdef VSTART
	/*
	 * The following code is necessary because we support changing all the 
	 * extra characters.
	 */
	if (tty.c_cc[VSTART] != Ctrl('Q'))
		tty.c_cc[VSTART] = '\377';
	if (tty.c_cc[VSTOP] != Ctrl('S'))
		tty.c_cc[VSTOP] = '\377';
	posnttyc.c_cc[VSUSP] = '\377';	/* ^Z */
	tty.c_cc[VSWTCH] = '\377';	/* ^Z */
	posnttyc.c_cc[VFLUSH] = '\377';	/* ^O */
	posnttyc.c_cc[VLNEXT] = '\377';	/* ^V */
# endif
}

#else /* USG */

ttcharoff()
{
	nttyc.t_quitc = '\377';
	if (nttyc.t_startc != Ctrl('Q'))
		nttyc.t_startc = '\377';
	if (nttyc.t_stopc != Ctrl('S'))
		nttyc.t_stopc = '\377';
#ifdef TIOCLGET
	nlttyc.t_suspc = '\377';	/* ^Z */
	nlttyc.t_dsuspc = '\377';	/* ^Y */
	nlttyc.t_flushc = '\377';	/* ^O */
	nlttyc.t_lnextc = '\377';	/* ^V */
#endif
}

#endif /* USG */

/*
 * Stop open, restoring tty modes.
 */
ostop(f)
	ttymode f;
{

#ifndef USG
	pfast = (f & CRMOD) == 0;
#else
	pfast = (f.c_oflag & ONLCR) == 0;
#endif
	termreset(), fgoto(), flusho();
	normal(f);
	tostop();
}

/* Actions associated with putting the terminal in the right mode. */
tostop()
{
	putpad(clr_eos);
	putpad(cursor_normal);
	putpad(keypad_local);
	if (!value(MESG) && ttynbuf[0]>1)
		chmod(ttynbuf, ttymesg);
}

#ifndef CBREAK
/*
 * Into cooked mode for interruptibility.
 */
vcook()
{

	tty.sg_flags &= ~RAW;
	sTTY(2);
}

/*
 * Back into raw mode.
 */
vraw()
{

	tty.sg_flags |= RAW;
	sTTY(2);
}
#endif

/*
 * Restore flags to normal state f.
 */
normal(f)
	ttymode f;
{

	if (normtty > 0) {
		setty(f);
		normtty--;
	}
}

/*
 * Straight set of flags to state f.
 */
ttymode
setty(f)
	ttymode f;
{
	int isnorm = 0;
#ifndef USG
	register int ot = tty.sg_flags;
#else
	ttymode ot;
	ot = tty;
#endif

#ifndef USG
	if (f == normf) {
		nttyc = ottyc;
		isnorm = 1;
# ifdef		TIOCLGET	/* Berkeley 4BSD */
		nlttyc = olttyc;
# endif
	} else
		ttcharoff();
	tty.sg_flags = f;
#else
	if (tty.c_lflag & ICANON)
		ttcharoff();
	else
	{
		posnttyc = posottyc; 	/* Restore the posix modes too. */
		isnorm = 1;
	}
	
	tty = f;
#endif
	sTTY(2);
	if (!isnorm)
		saveterm();
	return (ot);
}

gTTY(i)
	int i;
{

#ifndef USG
	ignore(gtty(i, &tty));
# ifdef TIOCGETC
	ioctl(i, TIOCGETC, &ottyc);
	nttyc = ottyc;
# endif
#else
	ioctl(i, TCGETA, &tty);
#endif
# ifdef		TIOCLGET	/* Berkeley 4BSD */
/* Moved out of USG/non-USG since Locus has both */
/* Turned back off with ifndef when tcgetattr() added.*/

	ioctl(i, TIOCGLTC, &olttyc);
	nlttyc = olttyc;
# endif
	tcgetattr(i,&posottyc);
	posnttyc = posottyc;
}

/*
 * sTTY: set the tty modes on file descriptor i to be what's
 * currently in global "tty".  (Also use nttyc if needed.)
 */
static sTTY(i)
	int i;
{
/* Moved this out of ifndef USG since we have a system with both
 * the USG ioctl's and Berkeley ^Z, etc.
 * (I do this before the TCSETAW so the non-Berkeley chars will be overwritten).
 * Turned back off with ifndef when tcsetattr() added.
 */
# ifndef TIOCSLTC
	ioctl(i, TIOCSLTC, &nlttyc);
# endif

#ifndef USG

# ifdef TIOCSETN
	/* Don't flush typeahead if we don't have to */
	ioctl(i, TIOCSETN, &tty);
# else
	/* We have to.  Too bad. */
	stty(i, &tty);
# endif

# ifdef TIOCGETC
	/* Update the other random chars while we're at it. */
	ioctl(i, TIOCSETC, &nttyc);
# endif
#else
	tcsetattr(i,TCSANOW, &posnttyc);
	/* USG 3 very simple: just set everything */
	ioctl(i, TCSETAW, &tty);
#endif
}

/*
 * Print newline, or blank if in open/visual
 */
noonl()
{

	ex_putchar(Outchar != termchar ? ' ' : '\n');
}

