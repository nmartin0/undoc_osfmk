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
static char *rcsid = "@(#)$RCSfile: ex_cmds.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:47 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_cmds.c
 *
 * FUNCTION: commands, serror, switch, tagfind
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
 * ex_cmds.c  1.4  com/cmd/edit/vi,3.1,9013 2/20/90 07:40:21
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_argv.h"
#include "ex_temp.h"
#include "ex_tty.h"
#include "ex_vis.h"

short	pflag, nflag;
int	poffset;

#define nochng()	lchng = chng
/* convert a char string to an NLchar string */
#define S2NLS(s)	(NCdecstr(s,bufc,NCSIZE(bufc)),bufc)
#define dotail(s)	tail(S2NLS(s))
#define dotail2of(s)	tail2of(S2NLS(s))
static	NLchar	bufc[30];	/* char string to NLchar string conversion */
/*
 * Main loop for command mode command decoding.
 * A few commands are executed here, but main function
 * is to strip command addresses, do a little address oriented
 * processing and call command routines to do the real work.
 */
commands(noprompt, exitoneof)
	short noprompt, exitoneof;
{
	register line *addr;
	register int c,r;
	register int lchng;
	int given;
	int seensemi;
	int cnt;
	short hadpr;
	resetflav();
	nochng();
	for (;;) {
		/*
		 * If dot at last command
		 * ended up at zero, advance to one if there is a such.
		 */
		if (dot <= zero) {
			dot = zero;
			if (dol > zero)
				dot = one;
		}
		shudclob = 0;

		/*
		 * If autoprint or trailing print flags,
		 * print the line at the specified offset
		 * before the next command.
		 */
		if (pflag ||
		    lchng != chng && value(AUTOPRINT) && !inglobal && !inopen && endline) {
			pflag = 0;
			nochng();
			if (dol != zero) {
				addr1 = addr2 = dot + poffset;
				if (addr1 < one || addr1 > dol)
error(msg(M_013, "Offset out-of-bounds|Offset after command too large"));
				setdot1();
				goto print;
			}
		}
		nochng();

		/*
		 * Print prompt if appropriate.
		 * If not in global flush output first to prevent
		 * going into pfast mode unreasonably.
		 */
		if (inglobal == 0) {
			flush();
			if (!hush && value(PROMPT) && !globp && !noprompt && endline) {
				ex_putchar(':');
				hadpr = 1;
			}
			TSYNC();
		}

		/*
		 * Gobble up the address.
		 * Degenerate addresses yield ".".
		 */
		addr2 = 0;
		given = seensemi = 0;
		do {
			addr1 = addr2;
			addr = address((NLchar *)0);
			c = getcd();
			if (addr == 0)
				if (c == ',')
					addr = dot;
				else if (addr1 != 0) {
					addr2 = dot;
					break;
				} else
					break;
			addr2 = addr;
			given++;
			if (c == ';') {
				c = ',';
				dot = addr;
				seensemi = 1;
			}
		} while (c == ',');
		if (c == '%') {
			/* %: same as 1,$ */
			addr1 = one;
			addr2 = dol;
			given = 2;
			c = ex_getchar();
		}
		if (addr1 == 0)
			addr1 = addr2;
		if (c == ':')
			c = ex_getchar();

		/*
		 * Set command name for special character commands.
		 */
		tailspec(c);

		/*
		 * If called via : escape from open or visual, limit
		 * the set of available commands here to save work below.
		 */
		if (inopen) {
			if (c=='\n' || c=='\r' || c==Ctrl('D') || c==EOF) {
				if (addr2)
					dot = addr2;
				if (c == EOF)
					return;
				continue;
			}
			if (any(c, "o")) {
notinvis:
				tailprim(S2NLS(Command), 1, 1);
			}
		}
		laste = 0;
		switch (c) {

		case 'a':

			switch(peekchar()) {
			case 'b':
/* abbreviate */
				dotail("abbreviate");
				setnoaddr();
				mapcmd(0, 1);
				anyabbrs = 1;
				continue;
			case 'r':
/* args */
				dotail("args");
				setnoaddr();
				eol();
				pargs();
				continue;
			}

/* append */
			if (inopen)
				goto notinvis;
			dotail("append");
			setdot();
			aiflag = exclam();
			donewline();
			vmacchng(0);
			deletenone();
			setin(addr2);
			inappend = 1;
			ignore(append(gettty, addr2));
			inappend = 0;
			nochng();
			continue;

		case 'c':
			switch (peekchar()) {

/* copy */
			case 'o':
				dotail("copy");
				vmacchng(0);
				move();
				continue;

/* cd */
			case 'd':
				dotail("cd");
				goto changdir;
#ifdef CHDIR
/* chdir */
			case 'h':
				ignchar();
				if (peekchar() == 'd') {
					register char *p;
					dotail2of("chdir");
changdir:
					if (savedfile[0] == '/' || !value(WARN))
						ignore(exclam());
					else
						ignore(quickly());
					if (skipend()) {
						p = getenv("HOME");
						if (p == NULL)
							error(msg(M_015, "Home directory unknown"));
					} else
						getone(), p = file;
					eol();
					if (chdir(p) < 0)
						filioerr(p);
					if (savedfile[0] != '/')
						edited = 0;
					continue;
				}
				if (inopen)
				    tailprim(S2NLS("change"), 2, 1);
				dotail2of("change");
				break;
#endif
			default:
				if (inopen)
					goto notinvis;
				dotail("change");
				break;
			}
/* change */
			aiflag = exclam();
			setCNL();
			vmacchng(0);
			setin(addr1);
			delete(0);
			inappend = 1;
			ignore(append(gettty, addr1 - 1));
			inappend = 0;
			nochng();
			continue;

/* delete */
		case 'd':
			/*
			 * Caution: dp and dl have special meaning already.
			 */
			dotail("delete");
			c = cmdreg();
			setCNL();
			vmacchng(0);
			if (c)
				YANKreg(c);
			else {
				DEL[0] = 0;
				DEL_OVFLOW = FALSE;
			}
			delete(0);
			appendnone();
			continue;

/* edit */
/* ex */
		case 'e':
			dotail(peekchar() == 'x' ? "ex" : "edit");
editcmd:
			if (!exclam() && chng)
				c = 'E';
			filename(c);
			if (c == 'E') {
				ungetchar(lastchar());
				ignore(quickly());
			}
			setnoaddr();
doecmd:
			init();
			addr2 = zero;
			laste++;
			ex_sync();
			rop(c,0);
			nochng();
			continue;

/* file */
		case 'f':
			dotail("file");
			setnoaddr();
			filename(c);
			noonl();
			continue;

/* global */
		case 'g':
			dotail("global");
			global(!exclam());
			nochng();
			continue;

/* insert */
		case 'i':
			if (inopen)
				goto notinvis;
			dotail("insert");
			setdot();
			nonzero();
			aiflag = exclam();
			donewline();
			vmacchng(0);
			deletenone();
			setin(addr2);
			inappend = 1;
			ignore(append(gettty, addr2 - 1));
			inappend = 0;
			if (dot == zero && dol > zero)
				dot = one;
			nochng();
			continue;

/* join */
		case 'j':
			dotail("join");
			c = exclam();
			setcount();
			nonzero();
			donewline();
			vmacchng(0);
			if (given < 2 && addr2 != dol)
				addr2++;
			join(c);
			continue;

/* k */
		case 'k':
casek:
			pastwh();
			c = ex_getchar();
			if (endcmd(c))
			    serror(msg(M_016, "Mark what?|%s requires following letter"), Command);
			donewline();
			if (c > 'z' || c < 'a')
				error(msg(M_017, "Bad mark|Mark must specify a letter"));
			setdot();
			nonzero();
			names[c - 'a'] = *addr2 &~ 01;
			anymarks = 1;
			continue;

/* list */
		case 'l':
			dotail("list");
			setCNL();
			ignorf(setlist(1));
			pflag = 0;
			goto print;

		case 'm':
			if (peekchar() == 'a') {
				ignchar();
				if (peekchar() == 'p') {
/* map */
					dotail2of("map");
					setnoaddr();
					mapcmd(0, 0);
					continue;
				}
/* mark */
				dotail2of("mark");
				goto casek;
			}
/* move */
			dotail("move");
			vmacchng(0);
			move();
			continue;

		case 'n':
			if (peekchar() == 'u') {
				dotail("number");
				goto numberit;
			}
/* next */
			dotail("next");
			setnoaddr();
			ckaw();
			ignore(quickly());
			if (getargs())
				makargs();
			next();
			c = 'e';
			filename(c);
			goto doecmd;

/* open */
		case 'o':
			dotail("open");
			oop();
			pflag = 0;
			nochng();
			continue;

		case 'p':
		case 'P':
			switch (peekchar()) {

/* put */
			case 'u':
				dotail("put");
				setdot();
				c = cmdreg();
				eol();
				vmacchng(0);
				if (c)
					putreg(c);
				else
					put();
				continue;

			case 'r':
				ignchar();
				if (peekchar() == 'e') {
/* preserve */
					dotail2of("preserve");
					eol();
					if (preserve() == 0)
						error(msg(M_018, "Preserve failed!"));
					else
						error(msg(M_019, "File preserved."));
				}
				dotail2of("print");
				break;

			default:
				dotail("print");
				break;
			}
/* print */
			setCNL();
			pflag = 0;
print:
			nonzero();
			if (clear_screen && span() > lines) {
				flush1();
				vclear();
			}
			plines(addr1, addr2, 1);
			continue;

/* quit */
		case 'q':
			dotail("quit");
			setnoaddr();
			c = quickly();
			eol();
			if (!c)
quit:
				nomore();
			if (inopen) {
				vgoto(WECHO, 0);
				if (!ateopr())
					vnfl();
				else {
					tostop();
				}
				flush();
				setty(normf);
			}
			cleanup(1);
			exit(0);

		case 'r':
			if (peekchar() == 'e') {
				ignchar();
				switch (peekchar()) {

/* rewind */
				case 'w':
					dotail2of("rewind");
					setnoaddr();
					if (!exclam()) {
						ckaw();
						if (chng && dol > zero)
							error(msg(M_020, "No write@since last chage (:rewind! overrides)"));
					}
					eol();
					erewind();
					next();
					c = 'e';
					ungetchar(lastchar());
					filename(c);
					goto doecmd;

/* recover */
				case 'c':
					dotail2of("recover");
					setnoaddr();
					c = 'e';
					if (!exclam() && chng)
						c = 'E';
					filename(c);
					if (c == 'E') {
						ungetchar(lastchar());
						ignore(quickly());
					}
					init();
					addr2 = zero;
					laste++;
					ex_sync();
					recover();
					rop2();
					revocer();
					if (status == 0)
						rop3(c);
					if (dol != zero)
						change();
					nochng();
					continue;
				}
				dotail2of("read");
			} else
				dotail("read");
/* read */
			if (savedfile[0] == 0 && dol == zero)
				c = 'e';
			pastwh();
			vmacchng(0);
			if (peekchar() == '!') {
				setdot();
				ignchar();
				unix0(0);
				filter(0);
				continue;
			}
			filename(c);
			rop(c,1);
			nochng();
			if (inopen && endline && addr1 > zero && addr1 < dol)
				dot = addr1 + 1;
			continue;

		case 's':
			switch (peekchar()) {
			/*
			 * Caution: 2nd NLchar cannot be c, g, or r
			 * because these have meaning to substitute.
			 */

/* set */
			case 'e':
				dotail("set");
				setnoaddr();
				set();
				continue;

/* shell */
			case 'h':
{
				ttymode savtty;
				dotail("shell");
				setNAEOL();
				vnfl();
				putpad(exit_ca_mode);
				flush();
				resetterm();
				savtty = unixex("-i", (char *) 0, 0, 0);
				unixwt(1, savtty);
				vcontin(0);
				continue;
}

/* source */
			case 'o':
				dotail("source");
				setnoaddr();
				getone();
				eol();
				source(file, 0);
				continue;
#ifdef SIGTSTP
/* stop, suspend */
			case 't':
				dotail("stop");
				goto suspend;
			case 'u':
				dotail("suspend");
suspend:
				c = exclam();
				eol();
				if (!c)
					ckaw();
				onsusp();
				continue;
#endif

			}
			/* fall into ... */

/* & */
/* ~ */
/* substitute */
		case '&':
		case '~':
			Command = "substitute";
			if (c == 's')
				dotail(Command);
			vmacchng(0);
			if (!substitute(c))
				pflag = 0;
			continue;

/* t */
		case 't':
			if (peekchar() == 'a') {
				dotail("tag");
				tagfind(exclam());
				if (!inopen)
					lchng = chng - 1;
				else
					nochng();
				continue;
			}
			dotail("t");
			vmacchng(0);
			move();
			continue;

		case 'u':
			if (peekchar() == 'n') {
				ignchar();
				switch(peekchar()) {
/* unmap */
				case 'm':
					dotail2of("unmap");
					setnoaddr();
					mapcmd(1, 0);
					continue;
/* unabbreviate */
				case 'a':
					dotail2of("unabbreviate");
					setnoaddr();
					mapcmd(1, 1);
					anyabbrs = 1;
					continue;
				}
/* undo */
				dotail2of("undo");
			} else
				dotail("undo");
			setnoaddr();
			markDOT();
			c = exclam();
			donewline();
			undo(c);
			continue;

		case 'v':
			switch (peekchar()) {

			case 'e':
/* version */
				dotail("version");
				setNAEOL();
				ex_printf("@(#) Version 3.7, 6/7/85."+5);
				noonl();
				continue;

/* visual */
			case 'i':
				dotail("visual");
				if (inopen) {
					c = 'e';
					goto editcmd;
				}
				vop();
				pflag = 0;
				nochng();
				continue;
			}
/* v */
			dotail("v");
			global(0);
			nochng();
			continue;

/* write */
		case 'w':
			c = peekchar();
			dotail(c == 'q' ? "wq" : "write");
wq:
			if (skipwh() && peekchar() == '!') {
				pofix();
				ignchar();
				setall();
				unix0(0);
				filter(1);
			} else {
				setall();
				wop(1);
				nochng();
			}
			if (c == 'q')
				goto quit;
			continue;

/* xit */
		case 'x':
			dotail("xit");
			if (!chng)
				goto quit;
			c = 'q';
			goto wq;

/* yank */
		case 'y':
			dotail("yank");
			c = cmdreg();
			setcount();
			eol();
			vmacchng(0);
			if (c)
				YANKreg(c);
			else
				yank();
			continue;

/* z */
		case 'z':
			zop(0);
			pflag = 0;
			continue;

/* * */
/* @ */
		case '*':
		case '@':
			c = ex_getchar();
			if (c=='\n' || c=='\r')
				ungetchar(c);
			if (any(c, "@*\n\r"))
				c = lastmac;
			if (c <= 'z' && isupper(c))
				c = tolower(c);
			if (c > 'z' || !islower(c))
				error(msg(M_022, "Bad register"));
			donewline();
			setdot();
			cmdmac(c);
			continue;

/* | */
		case '|':
			endline = 0;
			goto caseline;

/* \n */
		case '\n':
			endline = 1;
caseline:
			notempty();
			if (addr2 == 0) {
				if (cursor_up != NULL && c == '\n' && !inglobal)
					c = Ctrl('K');
				if (inglobal)
					addr1 = addr2 = dot;
				else {
					if (dot == dol)
						error(msg(M_023, "At EOF|At end-of-file"));
					addr1 = addr2 = dot + 1;
				}
			}
			setdot();
			nonzero();
			if (seensemi)
				addr1 = addr2;
			getline(*addr1);
			if (c == Ctrl('K')) {
				flush1();
				destline--;
				if (hadpr)
					shudclob = 1;
			}
			plines(addr1, addr2, 1);
			continue;

/* " */
		case '"':
			comment();
			continue;

/* # */
		case '#':
numberit:
			setCNL();
			ignorf(setnumb(1));
			pflag = 0;
			goto print;

/* = */
		case '=':
			donewline();
			setall();
			if (inglobal == 2)
				pofix();
			ex_printf("%d", lineno(addr2));
			noonl();
			continue;

/* ! */
		case '!':
			if (addr2 != 0) {
				vmacchng(0);
				unix0(0);
				setdot();
				filter(2);
			} else {
				ttymode savtty;
				unix0(1);
				pofix();
				putpad(exit_ca_mode);
				flush();
				resetterm();
				savtty = unixex("-c", uxb, 0, 0);
				unixwt(1, savtty);
				vclrech(1);	/* vcontin(0); */
				nochng();
			}
			continue;

/* < */
/* > */
		case '<':
		case '>':
			for (cnt = 1; peekchar() == c; cnt++)
				ignchar();
			setCNL();
			vmacchng(0);
			shift(c, cnt);
			continue;

/* ^D */
/* EOF */
		case Ctrl('D'):
		case EOF:
			if (exitoneof) {
				if (addr2 != 0)
					dot = addr2;
				return;
			}
			if (!isatty(0)) {
				if (intty)
					/*
					 * Chtty sys call at UCB may cause a
					 * input which was a tty to suddenly be
					 * turned into /dev/null.
					 */
					onhup();
				return;
			}
			if (addr2 != 0) {
				setlastchar('\n');
				putnl();
			}
			if (dol == zero) {
				if (addr2 == 0)
					putnl();
				notempty();
			}
			ungetchar(EOF);
			zop(hadpr);
			continue;

		default:
			if (!NCisalpha(c))
				break;
			ungetchar(c);
			tailprim(NCemptystr, 0, 0);
		}
		error(msg(M_024, "What?|Unknown command character '%c'"), c);
	}
}
