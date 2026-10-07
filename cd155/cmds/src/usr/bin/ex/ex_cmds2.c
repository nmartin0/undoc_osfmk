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
static char *rcsid = "@(#)$RCSfile: ex_cmds2.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:48 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) exc_cmds2.c
 *
 * FUNCTION: cmdreg, donewline, endcmd, eol, erewind, error, error0, error1,
 * exclam, fixol, makargs, next, nomore, quickly, resetflav, serror, setflav,
 * skipend, tail, tail2of, tailprim, tailspec, vcontin, vnfl
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
 * ex_cmds2.c  1.4  com/cmd/edit/vi,3.1,9013 1/10/90 16:50:51
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

extern short	pflag, nflag;		/* mjm: extern; also in ex_cmds.c */
extern int	poffset;		/* mjm: extern; also in ex_cmds.c */

static void error0(), error1();

/*
 * Subroutines for major command loop.
 */

/*
 * Is there a single letter indicating a named buffer next?
 */
cmdreg()
{
	register int c = 0;
	register int wh = skipwh();

	if (wh && isascii(peekchar()) && isalpha(peekchar()))
		c = ex_getchar();
	return (c);
}

/*
 * Tell whether the character ends a command
 */
endcmd(ch)
	int ch;
{
	switch (ch) {
	
	case '\n':
	case EOF:
		endline = 1;
		return (1);
	
	case '|':
	case '"':
		endline = 0;
		return (1);
	}
	return (0);
}

/*
 * Insist on the end of the command.
 */
eol()
{

	if (!skipend())
		error(msg(M_025, "Extra chars|Extra characters at end of command"));
	ignnEOF();
}

/*
 * Print out the message in the error message file at str,
 * with i an integer argument to printf.
 */
/*VARARGS2*/
void error(str, i)
	register char *str;
	int i;
{

	error0();
	merror(str, i);
	if (writing) {
		serror(msg(M_026, " [Warning - %s is incomplete]"), file);
		writing = 0;
	}
	error1(str != NULL);
}

/*
 * Rewind the argument list.
 */
erewind()
{

	argc = argc0;
	argv = argv0;
	args = args0;
	if (argc > 1 && !hush && cur_term) {
		ex_printf(mesg(msg(M_027, "%d files@to edit")), argc);
		if (inopen)
			ex_putchar(' ');
		else
			putNFL();
	}
}

/*
 * Guts of the pre-printing error processing.
 * If in visual and catching errors, then we dont mung up the internals,
 * just fixing up the echo area for the print.
 * Otherwise we reset a number of externals, and discard unused input.
 */
static void error0()
{

	if (laste) {
		tlaste();
		laste = 0;
		ex_sync();
	}
	if (vcatch) {
		if (splitw == 0)
			fixech();
		if (!enter_standout_mode || !exit_standout_mode)
			dingdong();
		return;
	}
	if (input) {
		input = NCstrend(input) - 1;
		if (*input == '\n')
			setlastchar('\n');
		input = 0;
	}
	setoutt();
	flush();
	resetflav();
	if (!enter_standout_mode || !exit_standout_mode)
		dingdong();
	if (inopen) {
		/*
		 * We are coming out of open/visual ungracefully.
		 * Restore columns, undo, and fix tty mode.
		 */
		columns = OCOLUMNS;
		undvis();
		ostop(normf);
		/* ostop should be doing this
		putpad(cursor_normal);
		putpad(key_eol);
		*/
		putnl();
	}
	inopen = 0;
	holdcm = 0;
}

/*
 * Post error printing processing.
 * Close the i/o file if left open.
 * If catching in visual then throw to the visual catch,
 * else if a child after a fork, then exit.
 * Otherwise, in the normal command mode error case,
 * finish state reset, and throw to top.
 */
static void error1(wasstr)
	short wasstr;
{
	short die;

	if (io > 0) {
		close(io);
		io = -1;
	}
	die = (getpid() != ppid);	/* Only children die */
	inappend = inglobal = 0;
	globp = vglobp = vmacp = 0;
	if (vcatch && !die) {
		inopen = 1;
		vcatch = 0;
		if (wasstr)
			noonl();
		fixol();
		longjmp(vreslab,1);
	}
	if (wasstr && !vcatch)
		putNFL();
	if (die)
		exit(1);
	lseek(0, 0L, 2);
	if (inglobal)
		setlastchar('\n');
	while (lastchar() != '\n' && lastchar() != EOF)
		ignchar();
	ungetchar(0);
	endline = 1;
	reset();
}

fixol()
{
	_fixol_(1);
}

_fixol_(do_vcont)
int do_vcont;
{
	if (Outchar != vputchar) {
		flush();
		if (state == ONEOPEN || state == HARDOPEN)
			outline = destline = 0;
		Outchar = vputchar;
		if (do_vcont)
			vcontin(1);
	} else {
		if (destcol)
			vclreol();
		vclean();
	}
}

/*
 * Does an ! character follow in the command stream?
 */
exclam()
{

	if (peekchar() == '!') {
		ignchar();
		return (1);
	}
	return (0);
}

/*
 * Make an argument list for e.g. next.
 */
makargs()
{

	glob(&frob);
	argc0 = frob.argc0;
	argv0 = frob.argv;
	args0 = argv0[0];
	erewind();
}

/*
 * Advance to next file in argument list.
 */
next()
{
	extern short isalt;	/* defined in ex_io.c */

	if (argc == 0)
		error(msg(M_028, "No more files@to edit"));
	morargc = argc;
	isalt = (strcmp(altfile, args)==0) + 1;
	if (savedfile[0])
		strcpy(altfile, savedfile);
	strcpy(savedfile, args);
	argc--;
	args = argv ? *++argv : strend(args) + 1;
}

/*
 * Eat trailing flags and offsets after a command,
 * saving for possible later post-command prints.
 */
donewline()
{
	register int c;

	resetflav();
	for (;;) {
		c = ex_getchar();
		switch (c) {

		case '^':
		case '-':
			poffset--;
			break;

		case '+':
			poffset++;
			break;

		case 'l':
			listf++;
			break;

		case '#':
			nflag++;
			break;

		case 'p':
			listf = 0;
			break;

		case ' ':
		case '\t':
			continue;

		case '"':
			comment();
			setflav();
			return;

		default:
			if (!endcmd(c))
serror(msg(M_029, "Extra chars|Extra characters at end of \"%s\" command"), Command);
			if (c == EOF)
				ungetchar(c);
			setflav();
			return;
		}
		pflag++;
	}
}

/*
 * Before quit or respec of arg list, check that there are
 * no more files in the arg list.
 */
nomore()
{

	if (argc == 0 || morargc == argc)
		return;
	morargc = argc;
	if (argc == 1)
		error(msg(M_291, "%d more file to edit"), argc);
	else
		error(msg(M_291, "%d more files to edit"), argc);
}

/*
 * Before edit of new file check that either an ! follows
 * or the file has not been changed.
 */
quickly()
{

	if (exclam())
		return (1);
	if (chng && dol > zero) {
		xchng = 0;
		error(msg(M_032, "No write@since last change (:%s! overrides)"), Command);
	}
	return (0);
}

/*
 * Reset the flavor of the output to print mode with no numbering.
 */
resetflav()
{

	if (inopen)
		return;
	listf = 0;
	nflag = 0;
	pflag = 0;
	poffset = 0;
	setflav();
}

/*
 * Print an error message with a %s type argument to printf.
 * Message text comes from error message file.
 */
void serror(str, cp)
	char *str;
	char *cp;
{

	error0();
	smerror(str, cp);
	error1(str != NULL);
}

/*
 * Set the flavor of the output based on the flags given
 * and the number and list options to either number or not number lines
 * and either use normally decoded (ARPAnet standard) characters or list mode,
 * where end of lines are marked and tabs print as ^I.
 */
setflav()
{

	if (inopen)
		return;
	setnumb(nflag || value(NUMBER));
	setlist(listf || value(LIST));
	setoutt();
}

/*
 * Skip white space and tell whether command ends then.
 */
skipend()
{

	pastwh();
	return (endcmd(peekchar()) && peekchar() != '"');
}

/*
 * Set the command name for non-word commands.
 */
tailspec(c)
	NLchar c;
{
	static char foocmd[3];	/* static: 2 byte char is null terminated */

	foocmd[1] = '\0';	/* guarantee 1 byte char is null terminated */
	NCenc(&c,foocmd);	/* 1 or 2 bytes */
	Command = foocmd;
}

/*
 * Try to read off the rest of the command word.
 * If alphabetics follow, then this is not the command we seek.
 */
void tail(comm)
	NLchar *comm;
{

	tailprim(comm, 1, 0);
}

void tail2of(comm)
	NLchar *comm;
{

	tailprim(comm, 2, 0);
}

static NLchar	tcommand[20];

void tailprim(comm, i, notinvis)
	register NLchar *comm;
	int i;
	short notinvis;
{
	register NLchar *cp;
	register int c;
	static char Cmdbuf[30];

	Command = Cmdbuf;
	NCencstr(comm,Command,30);
	for (cp = tcommand; i > 0; i--)
		*cp++ = *comm++;
	while (*comm && peekchar() == *comm)
		*cp++ = ex_getchar(), comm++;
	c = peekchar();
	if (notinvis || NCisalpha(c)) {
		/*
		 * Of the trailing lp funny business, only dl and dp
		 * survive the move from ed to ex.
		 */
		if (tcommand[0] == 'd' && any(c, "lp"))
			goto ret;
		if (tcommand[0] == 's' && any(c, "gcr"))
			goto ret;
		while (cp < &tcommand[19] && NCisalpha(peekchar()))
			*cp++ = ex_getchar();
		*cp = 0;
		if (notinvis)
			serror(msg(M_033, "What?|%S: No such command from open/visual"), tcommand);
		else
			serror(msg(M_034, "What?|%S: Not an editor command"), tcommand);
	}
ret:
	*cp = 0;
}

/*
 * Continue after a : command from open/visual.
 */
vcontin(ask)
	short ask;
{

	if (vcnt > 0)
		vcnt = -vcnt;
	if (inopen) {
		if (state != VISUAL) {
			/*
			 * We don't know what a shell command may have left on
			 * the screen, so we move the cursor to the right place
			 * and then put out a newline.  But this makes an extra
			 * blank line most of the time so we only do it for :sh
			 * since the prompt gets left on the screen.
			 *
			 * BUG: :!echo longer than current line \\c
			 * will screw it up, but be reasonable!
			 */
			if (state == CRTOPEN) {
				termreset();
				vgoto(WECHO, 0);
			}
			if (!ask) {
				putch('\r');
				putch('\n');
			}
			return;
		}
		if (ask) {
			merror(msg(M_035, "[Hit return to continue] "));
			flush();
		}
#ifndef CBREAK
		vraw();
#endif
		if (ask) {
			if(getkey() == ':') {
				/* Ugh. Extra newlines, but no other way */
				putch('\n');
				outline = WECHO;
				ungetkey(':');
			}
		}
		vclrech(1);
		if (Peekkey != ':') {
			fixterm();
			putpad(enter_ca_mode);
			tostart();
		}
	}
}

/*
 * Put out a newline (before a shell escape)
 * if in open/visual.
 */
vnfl()
{

	if (inopen) {
		if (state != VISUAL && state != CRTOPEN && destline <= WECHO)
			vclean();
		else
			vmoveitup(1, 0);
		vgoto(WECHO, 0);
		vclrcp(vtube[WECHO], WCOLS);
		tostop();
	}
	flush();
}
