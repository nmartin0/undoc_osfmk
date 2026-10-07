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
static char *rcsid = "@(#)$RCSfile: ex_get.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:59 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_get.c
 *
 * FUNCTION: checkjunk, getach, getcd, ex_getchar, gettty, ignchar, peekcd,
 * peekchar, setin, smunch
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
 * ex_get.c  1.5  com/cmd/edit/vi,3.1,9013 12/20/89 08:36:38
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_tty.h"

static void checkjunk();
static int getach();

/*
 * Input routines for command mode.
 * Since we translate the end of reads into the implied ^D's
 * we have different flavors of routines which do/don't return such.
 */
int	lastc = '\n';			/* must fit EOF or NLchar */

ignchar()
{
	ignore(ex_getchar());
}

ex_getchar(void)
{
	register int c;

	do
		c = getcd();
	while (!globp && c == Ctrl('D'));
	return (c);
}

getcd()
{
	register int c;
	extern short slevel;

again:
	c = getach();
	if (c == EOF)
		return (c);
	if (!inopen && slevel==0)
		if (!globp && c == Ctrl('D'))
			setlastchar('\n');
		else if (junk(c)) {
			checkjunk(c);
			goto again;
		}
	return (c);
}

peekchar()
{

	if (peekc == 0)
		peekc = ex_getchar();
	return (peekc);
}

peekcd()
{
	if (peekc == 0)
		peekc = getcd();
	return (peekc);
}

int verbose;

static int getach()
{
	register int c;
	char in_buf[512];
	static NLchar in_line[512];
	static char peek2c;

	c = peekc;
	if (c != 0) {
		peekc = 0;
		return (c);
	}
	if (peek2c != 0) {
		c = peek2c;
		peek2c = 0;
		return (c);
	}
	if (globp) {
		if (*globp)
			return (*globp++);
		globp = 0;
		return (lastc = EOF);
	}
top:
	if (input) {
		if (c = *input++) {
			return (lastc = c);
		}
		input = 0;
	}
	flush();
	if (intty) {
		c = read(0, in_buf, sizeof in_buf - 4);
		if (c < 0)
			return (lastc = EOF);
		if (c == 0 || in_buf[c-1] != '\n')
			in_buf[c++] = Ctrl('D');
		if (in_buf[c-1] == '\n')
			noteinp();
		in_buf[c] = 0;
		NCdecstr(in_buf,in_line,NCSIZE(in_line));
		input = in_line;
		goto top;
	}

	/* read 1 or 2 bytes and place in an NLchar */
	if (read(0, in_buf, 1) != 1)
		lastc = EOF;
	else if (!NCisshift(in_buf[0]) || read(0, &in_buf[1], 1) != 1) {
		lastc = in_buf[0];
		if (verbose)
			write(2, in_buf, 1);
	} else {
		if (_NCis2(in_buf[0], in_buf[1])) {
			lastc = _NCd2(in_buf[0], in_buf[1]);
		} else {
			lastc = in_buf[0];
			peek2c = in_buf[1];
		}
		if (verbose)
			write(2, in_buf, 2);
	}
	return (lastc);
}

/*
 * Input routine for insert/append/change in command mode.
 * Most work here is in handling autoindent.
 */
static	int	lastin;

gettty()
{
	register int c = 0;
	register NLchar *cp = genbuf;
	NLchar hadup = 0;
	int numbline();
	extern int (*Pline)();
	int offset = Pline == numbline ? 8 : 0;
	int ch;

	if (intty && !inglobal) {
		if (offset) {
			holdcm = 1;
			ex_printf("  %4d  ", lineDOT() + 1);
			flush();
			holdcm = 0;
		}
		if (value(AUTOINDENT) ^ aiflag) {
			holdcm = 1;
			if (value(LISP))
				lastin = lindent(dot + 1);
			gotab(lastin + offset);
			while ((c = getcd()) == Ctrl('D')) {
				if (lastin == 0 && isatty(0) == -1) {
					holdcm = 0;
					return (EOF);
				}
				lastin = backtab(lastin);
				gotab(lastin + offset);
			}
			switch (c) {

			case '^':
			case '0':
				ch = getcd();
				if (ch == Ctrl('D')) {
					if (c == '0')
						lastin = 0;
					if (!over_strike) {
						ex_putchar(QUOTE_BSP);
					}
					gotab(offset);
					hadup = 1;
					c = ex_getchar();
				} else
					ungetchar(ch);
				break;

			case '.':
				if (peekchar() == '\n') {
					ignchar();
					noteinp();
					holdcm = 0;
					return (EOF);
				}
				break;

			case '\n':
				hadup = 1;
				break;
			}
		}
		flush();
		holdcm = 0;
	}
	if (c == 0)
		c = ex_getchar();
	while (c != EOF && c != '\n') {
		if (cp > &genbuf[LBSIZE - 2])
			error(msg(M_067, "Input line too long"));
		*cp++ = c;
		c = ex_getchar();
	}
	if (c == EOF) {
		if (inglobal)
			ungetchar(EOF);
		return (EOF);
	}
	*cp = 0;
	cp = linebuf;
	if ((value(AUTOINDENT) ^ aiflag) && hadup == 0 && intty && !inglobal) {
		lastin = c = smunch(lastin, genbuf);
		for (c = lastin; c >= value(TABSTOP); c -= value(TABSTOP))
			*cp++ = '\t';
		for (; c > 0; c--)
			*cp++ = ' ';
	}
	CP(cp, genbuf);
	if (linebuf[0] == '.' && linebuf[1] == 0)
		return (EOF);
	return (0);
}

/*
 * Crunch the indent.
 * Hard thing here is that in command mode some of the indent
 * is only implicit, so we must seed the column counter.
 * This should really be done differently so as to use the whitecnt routine
 * and also to hack indenting for LISP.
 */
smunch(col, ocp)
	register int col;
	NLchar *ocp;
{
	register NLchar *cp;

	cp = ocp;
	for (;;)
		switch (*cp++) {

		case ' ':
			col++;
			continue;

		case '\t':
			col += value(TABSTOP) - (col % value(TABSTOP));
			continue;

		default:
			cp--;
			CP(ocp, cp);
			return (col);
		}
}

static void checkjunk(c)
	NLchar c;
{
	static	short junkbs;

	if (!junkbs && c == '\b') {
		register char *txt = msg(M_068, "^H discarded\n");
		write(2, txt, strlen(txt));
		junkbs = TRUE;
	}
}

line *
setin(addr)
	line *addr;
{

	if (addr == zero)
		lastin = 0;
	else
		getline(*addr), lastin = smunch(0, linebuf);
}
