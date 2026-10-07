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
static char *rcsid = "@(#)$RCSfile: ex_addr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:46 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_addr.c
 *
 * FUNCTION: address, getnum, setCNL, setNAEOL, setall, setcount, setdot,
 * setdot1, setnoaddr
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
 * ex_addr.c  1.3  com/cmd/edit/vi,3.1,9013 10/6/89 17:15:26
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_re.h"

/*
 * Routines for address parsing and assignment and checking of address bounds
 * in command mode.  The routine address is called from ex_cmds.c
 * to parse each component of a command (terminated by , ; or the beginning
 * of the command itself.  It is also called by the scanning routine
 * in ex_voperate.c from within open/visual.
 *
 * Other routines here manipulate the externals addr1 and addr2.
 * These are the first and last lines for the current command.
 *
 * The variable bigmove remembers whether a non-local glitch of . was
 * involved in an address expression, so we can set the previous context
 * mark '' when such a motion occurs.
 */

static	short bigmove;

/*
 * Set up addr1 and addr2 for commands whose default address is dot.
 */
setdot()
{

	setdot1();
	if (bigmove)
		markDOT();
}

/*
 * Call setdot1 to set up default addresses without ever
 * setting the previous context mark.
 */
setdot1()
{

	if (addr2 == 0)
		addr1 = addr2 = dot;
	if (addr1 > addr2) {
		notempty();
		error(msg(M_002, "Addr1 > addr2|First address exceeds second"));
	}
}

/*
 * Ex allows you to say
 *	delete 5
 * to delete 5 lines, etc.
 * This is implemented by setcount.
 */
setcount()
{
	register int cnt;

	pastwh();
	if (!(isascii(peekchar()) && isdigit(peekc))) {
		setdot();
		return;
	}
	addr1 = addr2;
	setdot();
	cnt = getnum();
	if (cnt <= 0)
		error(msg(M_003, "Bad count|Nonzero count required"));
	addr2 += cnt - 1;
	if (addr2 > dol)
		addr2 = dol;
	nonzero();
}

/*
 * Parse a number out of the command input stream.
 */
getnum()
{
	register int cnt;

	for (cnt = 0; isascii(peekcd()) && isdigit(peekc);)
		cnt = cnt * 10 + ex_getchar() - '0';
	return (cnt);
}

/*
 * Set the default addresses for commands which use the whole
 * buffer as default, notably write.
 */
setall()
{

	if (addr2 == 0) {
		addr1 = one;
		addr2 = dol;
		if (dol == zero) {
			dot = zero;
			return;
		}
	}
	/*
	 * Don't want to set previous context mark so use setdot1().
	 */
	setdot1();
}

/*
 * No address allowed on, e.g. the file command.
 */
setnoaddr()
{

	if (addr2 != 0)
		error(msg(M_004, "No address allowed@on this command"));
}

/*
 * Parse an address.
 * Just about any sequence of address characters is legal.
 *
 * If you are tricky you can use this routine and the = command
 * to do simple addition and subtraction of cardinals less
 * than the number of lines in the file.
 */
line *
address(in_line)
NLchar *in_line;
{
	register line *addr;
	register int offset, c;
	short lastsign;

	bigmove = 0;
	lastsign = 0;
	offset = 0;
	addr = 0;
	for (;;) {
		if (isascii(peekcd()) && isdigit(peekc)) {
			if (addr == 0) {
				addr = zero;
				bigmove = 1;
			}
			loc1 = 0;
			addr += offset;
			offset = getnum();
			if (lastsign >= 0)
				addr += offset;
			else
				addr -= offset;
			lastsign = 0;
			offset = 0;
		}
		switch (c = getcd()) {

		case '?':
		case '/':
		case '$':
		case '\'':
		case '\\':
			bigmove++;
		case '.':
			if (addr || offset)
				error(msg(M_005, "Badly formed address"));
		}
		offset += lastsign;
		lastsign = 0;
		switch (c) {

		case ' ':
		case '\t':
			continue;

		case '+':
			lastsign = 1;
			if (addr == 0)
				addr = dot;
			continue;

		case '^':
		case '-':
			lastsign = -1;
			if (addr == 0)
				addr = dot;
			continue;

		case '\\':
		case '?':
		case '/':
			c = compile(c, 1);
			notempty();
			savere(scanre);
			addr = dot;
			if (in_line && execute(0, dot)) {
				if (c == '/') {
					while (loc1 <= in_line) {
						if (loc1 == loc2)
							loc2++;
						if (!execute(1, (line *)0))
							goto nope;
					}
					break;
				} else if (loc1 < in_line) {
					NLchar *last;
doques:

					do {
						last = loc1;
						if (loc1 == loc2)
							loc2++;
						if (!execute(1, (line *)0))
							break;
					} while (loc1 < in_line);
					loc1 = last;
					break;
				}
			}
nope:
			for (;;) {
				if (c == '/') {
					addr++;
					if (addr > dol) {
						if (value(WRAPSCAN) == 0)
error(msg(M_006, "No match to BOTTOM|Address search hit BOTTOM without matching pattern"));
						addr = zero;
					}
				} else {
					addr--;
					if (addr < zero) {
						if (value(WRAPSCAN) == 0)
error(msg(M_007, "No match to TOP|Address search hit TOP without matching pattern"));
						addr = dol;
					}
				}
				if (execute(0, addr)) {
					if (in_line && c == '?') {
						in_line = &linebuf[LBSIZE];
						goto doques;
					}
					break;
				}
				if (addr == dot)
					error(msg(M_008, "Fail|Pattern not found"));
			}
			continue;

		case '$':
			addr = dol;
			continue;

		case '.':
			addr = dot;
			continue;

		case '\'':
			c = markreg(ex_getchar());
			if (c == 0)
				error(msg(M_009, "Marks are ' and a-z"));
			addr = getmark(c);
			if (addr == 0)
				error(msg(M_010, "Undefined mark@referenced"));
			break;

		default:
			ungetchar(c);
			if (offset) {
				if (addr == 0)
					addr = dot;
				addr += offset;
				loc1 = 0;
			}
			if (addr == 0) {
				bigmove = 0;
				return (0);
			}
			if (addr != zero)
				notempty();
			addr += lastsign;
			if (addr < zero)
				error(msg(M_011, "Negative address@- first buffer line is 1"));
			if (addr > dol)
				error(msg(M_012, "Not that many lines@in buffer"));
			return (addr);
		}
	}
}

/*
 * Abbreviations to make code smaller
 * Left over from squashing ex version 1.1 into
 * 11/34's and 11/40's.
 */
setCNL()
{

	setcount();
	donewline();
}

setNAEOL()
{

	setnoaddr();
	eol();
}
