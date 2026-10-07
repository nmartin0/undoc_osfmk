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
static char rcsid[] = "@(#)$RCSfile: command.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:30:38 $";
#endif
/*
 *	Copyright 1990, Eric Shienbrood
 *
 * This software may be freely copied, distributed, or modified, as long
 * this copyright notice is preserved.
 *
 *
 * command.c : command reading and execution
 */

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <varargs.h>
#include "globals.h"

#ifndef HELPFILE
#define HELPFILE	"/usr/lib/more.help"
#endif
#define VI		"/usr/bin/vi"

static int lastcmd, lastarg, lastp, lastdir;
static int lastcolon;
char shell_line[132];

extern unsigned char readch();

/*
 * Read a command and execute it. A command consists of an optional integer
 * argument followed by the command character.  If, following the execution
 * of the command, there is nothing more to display in the current file,
 * then zero is returned, else non-zero is returned.
 */

command (filename)
char *filename;
{
	register int nlines;
	register int retval;
	register int c;
	int initline, startline, targetline;
	int initpos, startpos, startfrag;
	int rev_search;
	int i;
	char colonch;
	unsigned char *re;
	FILE *helpf;
	int done;
	unsigned char comchar, cmdbuf[80];
	struct screeninfo *sp;
	extern int notell;

#define ret(val) retval=(val);done++;break

	done = 0;
	prompt(filename);
	for (;;) {
		nlines = number (&comchar);
		lastp = colonch = 0;
		if (comchar == '.') {	/* Repeat last command */
			lastp++;
			comchar = lastcmd;
			nlines = lastarg;
			if (lastcmd == ':')
				colonch = lastcolon;
		}
		lastcmd = comchar;
		lastarg = nlines;
		if (comchar == erase_ch(savetty)) {
			kill_line ();
			prompt (filename);
			continue;
		}
		putchar ('\r');
		cursor_column = 0;

		switch (comchar) {

		case ':':
			retval = colon (colonch, nlines);
			if (retval >= 0)
				done++;
			break;

		case ' ':
		case 'z':
			if (nlines == 0)
				nlines = lines_to_display;
			else if (comchar == 'z') {
				lines_to_display = nlines;
				lines_to_scroll = lines_to_display / 2;
			}
			if (!noscroll)
				nlines--;
			show_lines(nlines, filename);
			ret (-1);

		case 'b':
		case ctrl('B'):
			if (ispipe) {
				ring_bell();
				return (-1);
			}

			if (nlines == 0) nlines++;
			if (!noscroll) {
				erase (0);
				printf (MSGSTR(BACK, "\n...back %d %s\n\n"),
			 		nlines, 
					nlines > 1 ? MSGSTR(PAGES, "pages") :
						     MSGSTR(PAGE, "page"));
			}
			sp = get_top_screen_line();
			nlines *= lines_to_display;
			for (i = 0, initline = sp->fline; initline > 0 && i < nlines; initline--) {
				if (lineindex[initline].width != -1)
					i++;
			}
			goto_line(initline);;
			show_lines(lines_to_display, filename);
			ret(-1);

		case 'd':
		case ctrl('D'):
			if (nlines != 0)
				lines_to_scroll = nlines;
			show_lines(lines_to_scroll, filename);
			ret(-1);

		case RUBOUT:
		case 'q':
		case 'Q':
			end_it ();

		case 's':
		case 'f':
			if (nlines == 0) nlines++;
			if (comchar == 'f')
				nlines *= lines_to_display;
			erase (0);
			if (!noscroll)
				printf(MSGSTR(SKIP, "\n...skipping %d %s\n\n"),
			 		nlines, 
					nlines > 1 ? MSGSTR(LINES, "lines") :
						     MSGSTR(LINE, "line"));
			skiplns(nlines);
			show_lines(lines_to_display, filename);
			ret (-1);

		case ctrl('E'):
		case '\n':
		case 'j':
			if (nlines == 0)
				nlines = 1;
			show_lines(nlines, filename);
			ret (-1);

		case 'u':	/* Scroll backwards */
		case ctrl('U'):
			if (nlines != 0)
				lines_to_scroll = nlines;
			else
				nlines = lines_to_scroll;
			/* And fall into ... */	
		case ctrl('Y'):
		case 'k':
			if (ispipe) {
				ring_bell();
				break;
			}
			if (nlines == 0)
				nlines = 1;
			if (reverse_scroll(nlines) != 0) {
				ring_bell();
				break;
			}
			ret(-1);

		case 'G':	/* Go to file line */
		case 'g':
			/* nlines is the line number in this case */
			if (nlines == 0)
				nlines = 0x7fffffff;
			else
				nlines--;
			if (goto_line(nlines) == -1) {
				ring_bell();
				break;
			}
			if (!noscroll) {
				erase(0);
				if (nlines != 0x7fffffff)
					printf(MSGSTR(GOTO, 
						"\n... Going to line %d\n\n"),
						nlines+1);
				else
					printf(MSGSTR(GOTO2,
						"\n...Going to last line\n\n"));
			}
			show_lines(lines_to_display, filename);
			ret(-1);

		case ctrl('L'):
			if (!ispipe) {
				redraw();
				ret(-1);
			}
			else {
				ring_bell();
				break;
			}

		case '\'':
			if (!ispipe) {
				kill_line ();
				if (!noscroll)
					printf (MSGSTR(BACK2, "\n***Back***\n\n"));
				goto_line(Prevline);
				show_lines(lines_to_display, filename);
				ret (-1);
			}
			else {
				ring_bell();
				break;
			}

		case '=':
			kill_line ();
			printf ("%d", Currline+1);
			fflush (stdout);
			break;

		case 'N':
		case 'n':
			lastp++;
			/* And fall into ... */
		case '?':
		case '/':
			switch (comchar) {
			case '/':
				lastdir = rev_search = 0;
				break;
			case '?':
				lastdir = rev_search = 1;
				break;
			case 'n':
				rev_search = lastdir;
				break;
			case 'N':
				rev_search = !lastdir;
				break;
			}
			if (rev_search && ispipe) {
				ring_bell();
				break;
			}
			if (nlines == 0) nlines++;
			kill_line ();
			printf(rev_search ? "?" : "/");
			fflush (stdout);
			/*
			 * For backwards search, start with line
			 * at top of screen.
			 */
			startline = Currline;
			startpos = Ftell();
			startfrag = Currfrag;
			if (rev_search) {
				sp = get_top_screen_line();
				/*
				 * If only part of top line is displayed,
				 * start with that line, otherwise start
				 * with previous line.
				 */
				if (sp->fragnum != 0)
					goto_line(sp->fline+1);
				else
					goto_line(sp->fline);
				initline = sp->fline;
				initpos = sp->seek_key;
			}
			else {
				initline = Currline;
				initpos = Ftell();
			}
			if (lastp)
				/* Use previous r.e. */
				re = NULL;
			else {
				if (!read_processed_tty_line (cmdbuf, 78, comchar)) {
					ret(-1);
				}
				re = cmdbuf;
			}
			write (ttyfd,"\r", 1);
			cursor_column = 0;
			if (!search (re, nlines, rev_search)) {
				Currline = startline;
				Currfrag = startfrag;
				Fseek(startpos);
				break;
			}
			if (rev_search) {
				targetline = Currline - 1;
				nlines = initline - targetline;
			}
			else {
				targetline = Currline;
				nlines = Currline - initline;
			}
			if (!ispipe && nlines < ScreenLength/2 &&
					(!rev_search || ScrollUpStr != NULL)) {
				Currline = initline;
				Currfrag = 0;
				Fseek(initpos);
				while (nlines > 0) {
					if (rev_search) {
						reverse_scroll(nlines);
						sp = get_top_screen_line();
						nlines = sp->fline - targetline;
					}
					else {
						show_lines(nlines, filename);
						nlines = targetline - Currline;
					}
				}
				/* Get entire line onto screen */
				if (rev_search && sp->fragnum != 0)
					reverse_scroll(sp->fragnum);
			}
			else {
				/* XXX pipes */
				if (!noscroll)
					printf (MSGSTR(SKIPPING, "\n...skipping\n\n"));
				/*
				 * Back up so line ends up at top of screen.
				 */
				if (!ispipe && Currline > 0)
					goto_line(Currline-1);
				show_lines (lines_to_display, filename);
			}
			ret(-1);

		case '!':
			if (do_shell() == 0)
				break;
			ret(-1);

		case 'h':
			if ((helpf = fopen (HELPFILE, "r")) == NULL) {
				message(MSGSTR(CANTOPEN,"Can't open help file"));
				break;
			}
			save_screen_state();
			if (noscroll)
				doclear ();
			while ((c = getc(helpf)) != EOF)
				putchar(c);
			fflush(stdout);
			fclose (helpf);
			forget_screen_state();
			wait_for_keypress();
			redraw();
			ret(-1);
			break;

		case 'v':	/* This case should go right before default */
			if (!no_intty) {
				save_screen_state();
				kill_line ();
				sprintf((char *)cmdbuf, "+%d",
				    Currline - lines_to_display < 0 ? 0 :
				Currline - (lines_to_display + 1)/2);
				printf("vi %s %s", cmdbuf, fnames[fnum]);
				execute (VI, "vi", cmdbuf, fnames[fnum], 0);
				printf ("\n-------------------------\n");
				wait_for_keypress();
				redraw();
				ret(-1);
			}
		/* And fall into ... */
		default:
			if (verbose)
				message(MSGSTR(INSTRUCT, "[Press 'h' for instructions.]"));
			else
				ring_bell();
			break;
		}
		if (done) break;
	}
endsw:
	inwait = 0;
	notell++;
	return (retval);
}

unsigned char ch;

/*
 * Execute a colon-prefixed command.
 * Returns <0 if not a command that should cause
 * more of the file to be printed.
 */

colon (cmd, nlines)
int cmd;
int nlines;
{
	if (cmd == 0)
		ch = readch ();
	else
		ch = cmd;
	lastcolon = ch;
	switch (ch) {
	case 'f':
		kill_line ();
		if (!no_intty)
			printf (MSGSTR(LINE2, "\"%s\" line %d"), fnames[fnum], Currline+1);
		else if (ispipe)
			printf (MSGSTR(NOTFILE,"[Not a file] line %d"), Currline+1);
		else
			printf (MSGSTR(UNKNOWN, "\"Unknown\" [stdin] line %d"), Currline+1);
		fflush (stdout);
		return (-1);
	case 'n':
		if (nlines == 0) {
			if (fnum >= nfiles - 1)
				end_it ();
			nlines++;
		}
		erase (0);
		skipf (nlines);
		return (0);
	case 'p':
		if (no_intty) {
			ring_bell();
			return (-1);
		}
		erase (0);
		if (nlines == 0)
			nlines++;
		skipf (-nlines);
		return (0);
	case '!':
		do_shell();
		return (-1);
	case 'q':
	case 'Q':
		end_it ();
	case 'v':
		kill_line();
		printf(MSGSTR(VERSION, "more: version 2.0"));
		fflush (stdout);
		return (-1);
	default:
		ring_bell();
		return (-1);
	}
}

/*
 * Read a decimal number from the terminal. Set cmd to the non-digit which
 * terminates the number.
 */

number(cmd)
unsigned char *cmd;
{
	register int i;

	i = 0; ch = kill_ch(savetty);
	for (;;) {
		ch = readch ();
		if (isdigit(ch))
			i = i*10 + ch - '0';
		else if (ch == kill_ch(savetty))
			i = 0;
		else {
			*cmd = ch;
			break;
		}
	}
	return (i);
}

static int	shellp;		/* A previous shell command exists */

do_shell ()
{
	unsigned char cmdbuf[80];
	char *shell;
	int changeval;

	save_screen_state();
	kill_line ();
	printf("!");
	fflush (stdout);
	if (lastp)
		printf ("%s", shell_line);
	else {
		if (!read_processed_tty_line (cmdbuf, 78, '!'))
			return 1;
		if ((changeval = expand (shell_line, cmdbuf)) == 1) {
			kill_line ();
			printf ("!%s", shell_line);
		}
		else if (changeval == -1)
			return 0;
	}
	printf("\n");
	if (clreol)
		clreos();
	fflush (stdout);
	shellp = 1;
	if ((shell = getenv("SHELL")) == NULL)
		shell = "/bin/sh";
	if (!shell_line[0])
		execute (shell, shell, 0);
	else
		execute (shell, shell, "-c", shell_line, 0);
	printf ("-------------------------\n");
	wait_for_keypress();
	redraw();
	return 1;
}

/*
 * Search for nth ocurrence of regular expression 'pattern'.
 * When search is successful, Currline refers to the line *after*
 * the one containing the pattern.
 */

int Interrupt;

search (pattern, n, reverse)
char pattern[];
register int n;
int reverse;
{
	long startpos = Ftell();
	int length;
	int found;
	int rv, re_exec();
	char *s, *re_comp();
	extern unsigned char Line[];

	Prevline = Currline;
	Interrupt = 0;
	if ((s = re_comp (pattern)) != 0) {
		message(s);
		return 0;
	}
	if (reverse) {
		if (Currline == 0) {
			message(MSGSTR(NOTFOUND, "Pattern not found"));
			return 0;
		}
		goto_line(Currline - 1);
	}
	found = 0;
	while (reverse || !feof (curfile)) {
		if (Interrupt)
			break;
		getline(&length, 0);
		if ((rv = re_exec (Line)) == 1) {
			if (--n == 0) {
				/* begin XXX */
				if (ispipe) {
					if (!noscroll)
						kill_line ();
					else if (clreol)
						gohome();
					else
						doclear();
					printf ("%s\n", Line);	/* XXX */
				}
				/* end XXX */
				found++;
				break;
			}
		}
		else if (rv == -1) {
			message(MSGSTR(REGEXP, "Regular expression botch"));
			return 0;
		}
		if (reverse) {
			if (Currline <= 1)
				break;
			goto_line(Currline - 2);
		}
	}
	if (!found) {
		if (!ispipe) {
			Currline = Prevline;
			Fseek (startpos);
		}
		else {
			printf (MSGSTR(PATTERN, "\nPattern not found\n"));
			end_it ();
		}
		message(MSGSTR(PATTERN2, "Pattern not found"));
		return 0;
	}
	return 1;
}

/*VARARGS2*/
execute (va_alist)
va_dcl
{
	int id, argno;
	int n;
	char *args[20];
	va_list argp;
	void (*saveint)();
	void (*savequit)();
	void (*savesusp)();
	extern int catch_susp;
	char *cmd;

	fflush (stdout);
	argno = 0;
	va_start(argp);
	cmd = va_arg(argp, char *);
	while (args[argno++] = va_arg(argp, char *))
		;
	va_end(argp);
	reset_tty ();
	for (n = 10; (id = fork()) < 0 && n > 0; n--)
		sleep (5);
	if (id == 0) {
		if (!isatty(0)) {
			close(0);
			open("dev/tty", O_RDONLY);
		}
		execv (cmd, args);
		write (ttyfd, "exec failed\n", 12);
		exit (1);
	}
	else if (id > 0) {
		saveint = signal (SIGINT, SIG_IGN);
		savequit = signal (SIGQUIT, SIG_IGN);
#ifdef SIGTSTP
		if (catch_susp)
			savesusp = signal(SIGTSTP, SIG_DFL);
#endif
		wait (0);
		signal (SIGINT, saveint);
		signal (SIGQUIT, savequit);
#ifdef SIGTSTP
		if (catch_susp)
			signal(SIGTSTP, savesusp);
#endif
	}
	else
		write(ttyfd, MSGSTR(CANTFORK, "Cannot fork new process\n"), 24);
	set_tty ();
	forget_screen_state();
}

/*
 * Display the next set of lines from the file.
 */
show_lines(lines_to_show, filename)
int lines_to_show;
char *filename;
{
	int retval;
	int ch;
	extern int print_file_names;

	if ((ch = Getc()) == EOF)
		return(0);
	Ungetc(ch);
	if (noscroll && lines_to_show >= lines_to_display) {
		if (clreol)
			gohome();
		else
			doclear();
	}

	if (filename && print_file_names) {
		if (bad_so)
			erase (0);
		printf("::::::::::::::");
		printf ("\n%s\n::::::::::::::\n", filename);
		if (lines_to_show > ScreenLength - 4)
			lines_to_show = ScreenLength - 4;
	}
	Pause = 0;
	retval = 1;
	while (lines_to_show > 0 && !Pause) {
		if (print_line_from_file() == EOF) {
			retval = 0;
			break;
		}
		lines_to_show--;
	}

	if (pstate) {
		tputs(ULexit, 1, putch);
		pstate = 0;
	}
	if (clreol && cursor_line != screen_end)
		clreos();
	fflush(stdout);
	return retval;
}

expand (outbuf, inbuf)
char *outbuf;
char *inbuf;
{
	register char *instr;
	register char *outstr;
	register char ch;
	char temp[200];
	int changed = 0;

	instr = inbuf;
	outstr = temp;
	while ((ch = *instr++) != '\0')
		switch (ch) {
		case '%':
			if (!no_intty) {
				strcpy (outstr, fnames[fnum]);
				outstr += strlen (fnames[fnum]);
				changed = 1;
			}
			else
				*outstr++ = ch;
			break;
		case '!':
			if (!shellp) {
				message(MSGSTR(NOCMD, "No previous command to substitute for"));
				return -1;
			}
			strcpy (outstr, shell_line);
			outstr += strlen (shell_line);
			changed = 1;
			break;
		case '\\':
			if (*instr == '%' || *instr == '!') {
				*outstr++ = *instr++;
				break;
			}
		default:
			*outstr++ = ch;
		}

	*outstr++ = '\0';
	strcpy (outbuf, temp);
	return (changed);
}
