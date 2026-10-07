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
static char rcsid[] = "@(#)$RCSfile: main.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:30:44 $";
#endif
/*
 *	Copyright 1990, Eric Shienbrood
 *
 * This software may be freely copied, distributed, or modified, as long
 * this copyright notice is preserved.
 */

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/exec.h>
#include <setjmp.h>
#include <locale.h>
#include "globals.h"

#ifndef LONG_MAX
#define LONG_MAX	0x7fffffffL
#endif

void	onquit(), end_it(), redraw();
#ifdef NOT_YET
static magic();
#endif

#ifdef SIGTSTP
void	onsusp();
#endif
int	startup = 1;
int	notell = 1;
int	catch_susp;	/* We should catch the SIGTSTP signal */
int	debug;
int	firstf = 1;
int	print_file_names; 
int	wait_opt;
int	force_wait;


main(argc, argv)
int argc;
char *argv[];
{
	register char	*s;
	register char	*p;
	register char	ch;
	int		initopt = 0;
	int		srchopt = 0;
	int		initline;
	char		initbuf[80];
	FILE		*checkf();
	FILE		*checkf1();

	setlocale( LC_ALL, "");
	nfiles = argc;
	fnames = argv;
	if (s = getenv("MORE"))
		argscan(s);
	while (--nfiles > 0) {
		if ((ch = (*++fnames)[0]) == '-') {
			argscan(*fnames + 1);
		}
		else if (ch == '+') {
			s = *fnames;
			if (*++s == '/') {
				srchopt++;
				for (++s, p = initbuf; p < initbuf + 79 && *s != '\0';)
					*p++ = *s++;
				*p = '\0';
			}
			else if (*s == 'g' || *s == 'G')
				initopt = 2;
			else {
				initopt = 1;
				initline = atoi(s);
				if (--initline < 0)
					initopt = 0;
			}
		}
		else break;
	}
	get_terminal_info ();
	/*
	 * allow clreol only if HomeStr and EraseLineStr and EodClrStr
	 * strings are defined, and in that case, make sure we are in
	 * noscroll mode.
	 */
	if(clreol) {
		if (HomeStr == NULL || EraseLineStr == NULL || EodClrStr == NULL)
			clreol = 0;
		else
			noscroll = 1;
	}

	if (lines_to_display == 0)
		lines_to_display = ScreenLength - 1;
	lines_to_scroll = lines_to_display / 2;

	if (nfiles > 1)
		print_file_names++;

	if (!no_intty && nfiles == 0) {
		char *strrchr();

		p = strrchr(argv[0], '/');
		fputs(MSGSTR(USAGE, "Usage: "), stderr);
		fputs(p ? p + 1 : argv[0], stderr);
		fputs(MSGSTR(USAGE2, " [-cdflnpsuv] [+linenum | +/pattern] name1 name2 ...\n"), stderr);
		exit(1);
	}
	else {
		curfile = stdin;
		curfilename = NULL;
	}

	if (!no_tty) {
		signal(SIGQUIT, onquit);
		signal(SIGINT, end_it);
#ifdef SIGWINCH
		signal(SIGWINCH, redraw);
#endif
#ifdef SIGTSTP
		if (signal (SIGTSTP, SIG_IGN) == SIG_DFL) {
			signal(SIGTSTP, onsusp);
			catch_susp++;
		}
#endif
		set_tty();
	}

	if (no_intty) {
		if (no_tty)
			copy_file();
		else {
			(void)checkf1("[stdin]", stdin);
			if ((ch = Getc ()) == '\f')
				doclear ();
			else {
				Ungetc (ch);
				if (noscroll && ch != EOF)
					if (clreol)
						gohome();
					else
						doclear ();
			}
			if (srchopt) {
				search (initbuf, 1, 0);
				if (!ispipe)
					goto_line(Currline-1);
			}
			else if (initopt == 1)
				skiplns (initline);
			if (ch != EOF)
				view_file (NULL);
		}
		no_intty = 0;
		print_file_names++;
		firstf = 0;
	}

	while (fnum < nfiles) {
		/*
		 * See whether this is a displayable file
		 */
		forget_screen_state();
		if ((curfile = checkf (fnames[fnum])) != NULL) {
			curfilename = fnames[fnum];
			Prevline = Currline = 0;
			if (firstf) {
				if (srchopt) {
					search (initbuf, 1, 0);
					if (!ispipe)
						goto_line(Currline-1);
				}
				else if (initopt == 2) {
					if (print_file_names)
						lines_to_display -= 3;
					goto_line(0x7fffffff);
					if (print_file_names)
						lines_to_display += 3;
				}
				else if (initopt)
					skiplns (initline);
			}
			srchopt = initopt = 0;
			if (lines_to_display != 0) {
				if (no_tty)
					copy_file();
				else
					view_file(fnames[fnum]);
			}
			fflush(stdout);
			fclose(curfile);
			Prevline = 0;
		}
		fnum++;
		firstf = 0;
	}
	end_it();
}

argscan(s)
register char *s;
{
	char *cp;
	int fd;

	for (lines_to_display = 0; *s != '\0'; s++) {
		if (isdigit(*s))
			lines_to_display = lines_to_display*10 + *s - '0';
		else if (*s == 'd')
			verbose = 1;
		else if (*s == 'l')
			stop_opt = 0;
		else if (*s == 'f')
			fold_opt = 0;
		else if (*s == 'c')
			clreol = 1;
		else if (*s == 'p')
			noscroll = 1;
		else if (*s == 's')
			ssp_opt = 1;
		else if (*s == 'u')
			ul_opt = 0;
		else if (*s == 'v')
			show_opt = 0;
		else if (*s == 'z')
			show_all_opt = show_opt = 1;
		else if (*s == 'w')
			wait_opt = 1;
		else if (*s == 'D')
			debug = 1;
		else if (*s == 'T') {
			close(1);
			if ((cp = getenv("MORE_TERM")) == NULL)
				cp = "/dev/ttyp0";
			if ((fd = open(cp, O_RDWR)) < 0)
				perror(MSGSTR(TERMDEV, "Cannot open alternate term device"));
			else if (fd != 1)
				fprintf(stderr, MSGSTR(ALTTERM, "Alternate term not on fd 1\n"));
			close(2);
			dup(1);
		}
	}
}

/*
 * Check whether the file named by fs is an ASCII file which the user may
 * access.  If it is, return the opened file. Otherwise return NULL.
 */

FILE *
checkf1(fs, f)
char *fs;
register FILE *f;
{
	struct stat stbuf;

	curfile = f;
	if (fstat (fileno(f), &stbuf) == -1) {
		(void) fflush(stdout);
		perror(fs);
		return (NULL);
	}
	if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
		printf(MSGSTR(DIRECT, "\n*** %s: directory ***\n\n"), fs);
		return (NULL);
	}
	ispipe = (lseek(fileno(f), 0, 0) < 0);
	if (ispipe)
		return(f);

	if ((file_size = stbuf.st_size) == 0) {
		file_size = LONG_MAX;
		return(f);
	}
#ifdef NOT_YET
	/* Try to see whether it is an ASCII file */
	if (magic(f, fs))
		return(NULL);
#endif

	/* Allocate a line index array */
	if ((stbuf.st_mode & S_IFMT) == S_IFREG)
		init_line_index();

	(void) Getc();
	Fseek(0);
	return (f);
}

FILE *
checkf (fs)
register char *fs;
{
	register FILE *f;

	if ((f = Fopen(fs, "r")) == NULL) {
		(void) fflush(stdout);
		perror(fs);
		return (NULL);
	}
	return (checkf1(fs, f));
}

#ifdef NOT_YET
/*
 * magic --
 *	check for file magic numbers.  This code would best be shared with
 *	the file(1) program or, perhaps, more should not try and be so smart?
 */
static
magic(f, fs)
FILE *f;
char *fs;
{
	struct exec ex;

	if (fread(&ex, sizeof(ex), 1, f) == 1)
		switch (getw(f)) {
		case OMAGIC:
		case NMAGIC:
		case ZMAGIC:
		case 0405:
		case 0411:
		case 0177545:
			printf(MSGSTR(NOTTEXT,"\n******** %s: Not a text file ********\n\n"), fs);
			fclose (f);
			return(1);
		default:
			break;
	}
	(void)fseek(f, 0L, L_SET);
	return(0);
}
#endif

static jmp_buf restore;

/*
 * Print out the contents of the file f, one screenful at a time.
 */
view_file (filename)
char *filename;
{
	register int c;

	if (firstf) {
		if (show_lines(lines_to_display, filename) == 0)
			return;
		filename = NULL;
	}
	for (;;) {
		if (setjmp(restore) == 2) {
			/*
			 * Resuming after a suspend: redraw screen
			 */
			if (!ispipe)
				redraw();
		}
		if ((c = Getc()) == EOF && !wait_opt && !force_wait) {
			if (clreol)
				clreos();
			return;
		}
		Ungetc (c);
		if (command(filename) == 0)
			return;
		filename = NULL;
#ifdef notdef
		if (hard && line_width(cursor_line) > 0)
			erase (0);
#endif
	}
}

/*
 * Come here if a quit signal is received
 */

void
onquit()
{
	extern int Interrupt;

	signal(SIGQUIT, SIG_IGN);
	if (!inwait) {
		save_screen_state();
		forget_screen_state();
		putchar ('\n');
		cursor_column = 0;
		cursor_line = screen_end = ScreenLength - 1;
		line_width(screen_end) = 0;
		if (!startup) {
			signal(SIGQUIT, onquit);
			longjmp (restore, 1);
		}
		else {
			Pause++;
			Interrupt++;
		}
	}
	else if (!verbose && notell) {
		printf (MSGSTR(QQUIT, "[Use q or Q to quit]"));
		fflush(stdout);
		notell = 0;
	}
	signal(SIGQUIT, onquit);
}

/*
 * Clean up terminal state and exit.
 * Also come here if interrupt signal received.
 */

void
end_it ()
{
	if (clreol) {
		putchar('\r');
		clreos();
		fflush(stdout);
	}
	else if (promptlen > 0) {
		kill_line ();
		fflush (stdout);
	}
	reset_tty ();
	_exit(0);
}

/*
 * See whether the last component of the path name "path"
 * is equal to the string "string"
 */

tailequ (path, string)
char *path;
register char *string;
{
	register char *tail;

	tail = path + strlen(path) - 1;
	while (tail > path && *tail != '/')
		tail--;
	if (*tail == '/')
		tail++;
	while (*tail++ == *string++)
		if (*tail == '\0')
			return(1);
	return(0);
}

/* Come here when we get a suspend signal from the terminal */

#ifdef SIGTSTP
void
onsusp ()
{
	save_screen_state();
	/* ignore SIGTTOU so we don't get stopped if csh grabs the tty */
	signal (SIGTTOU, SIG_IGN);
	reset_tty ();
	fflush (stdout);
	signal (SIGTTOU, SIG_DFL);
	/* Send the TSTP signal to suspend our process group */
	signal (SIGTSTP, SIG_DFL);
	sigsetmask(0);
	kill (0, SIGTSTP);
	/* Pause for station break */

	/* We're back */
	signal (SIGTSTP, onsusp);
	set_tty ();
	forget_screen_state();
	if (inwait)
		longjmp (restore, 2);
}
#endif
