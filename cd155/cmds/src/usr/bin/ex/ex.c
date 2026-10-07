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
#ifndef lint
char *copyright =
"@(#) Copyright (c) 1980 Regents of the University of California.\n\
 All rights reserved.\n";
#endif
#if !defined(lint) && !defined(_NOIDENT)
static char *rcsid = "@(#)$RCSfile: ex.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:43 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex.c
 *
 * FUNCTION: main, init, tailpath
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
 * 
 * Copyright (c) 1981 Regents of the University of California 
 * 
 * ex.c  1.6  com/cmd/edit/vi,3.1,9013 3/7/90 06:07:19
 */

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "ex.h"
#include "ex_argv.h"
#include "ex_temp.h"
#include "ex_tty.h"

/*
 * The code for ex is divided as follows:
 *
 * ex.c 		Entry point and routines handling interrupt, hangup
 *			signals; initialization code.
 *
 * ex_addr.c		Address parsing routines for command mode decoding.
 *			Routines to set and check address ranges on commands.
 *
 * ex_cmds.c		Command mode command decoding.
 *
 * ex_cmds2.c		Subroutines for command decoding and processing of
 *			file names in the argument list.  Routines to print
 *			messages and reset state when errors occur.
 *
 * ex_cmdsub.c		Subroutines which implement command mode functions
 *			such as append, delete, join.
 *
 * ex_data.c		Initialization of options.
 *
 * ex_get.c		Command mode input routines.
 *
 * ex_io.c		General input/output processing: file i/o, unix
 *			escapes, filtering, source commands, preserving
 *			and recovering.
 *
 * ex_put.c		Terminal driving and optimizing routines for low-level
 *			output (cursor-positioning); output line formatting
 *			routines.
 *
 * ex_re.c		Global commands, substitute, regular expression
 *			compilation and execution.
 *
 * ex_set.c		The set command.
 *
 * ex_subr.c		Loads of miscellaneous subroutines.
 *
 * ex_temp.c		Editor buffer routines for main buffer and also
 *			for named buffers (Q registers if you will.)
 *
 * ex_tty.c		Terminal dependent initializations from termcap
 *			data base, grabbing of tty modes (at beginning
 *			and after escapes).
 *
 * ex_unix.c		Routines for the ! command and its variations.
 *
 * ex_v*.c		Visual/open mode routines... see ex_v.c for a
 *			guide to the overall organization.
 */

#include <locale.h>
short ivis;

/*
 * Main procedure.  Process arguments and then
 * transfer control to the main command processing loop
 * in the routine commands.  We are entered as either "ex", "edit", "vi"
 * or "view" and the distinction is made here.	Actually, we are "vi" if
 * there is a 'v' in our name, "view" is there is a 'w', and "edit" if
 * there is a 'd' in our name.	For edit we just diddle options;
 * for vi we actually force an early visual command.
 */
main(ac, av)
	register int ac;
	register char *av[];
{
	register char *cp;
	register int c;
	static char buft[SBUFSIZ]; /* temporary buffer to form char strings  */
	short recov = 0;
 	short itag = 0;
	short fast = 0;
	extern int verbose;
#ifdef TRACE
	register char *tracef;
#endif


	/*
	 * Immediately grab the tty modes so that we wont
	 * get messed up if an interrupt comes in quickly.
	 */
	gTTY(2);
#ifndef USG
	normf = tty.sg_flags;
#else
	normf = tty;
#endif
	setlocale(LC_ALL,"");		/* required by NLS environment tests */
	ex_catd = catopen(MF_EX,0);
	ppid = getpid();

	/*
	 * Defend against d's, v's, w's, and a's in directories of
	 * path leading to our true name.
	 */
	av[0] = tailpath(av[0]);

	/*
	 * Figure out how we were invoked: ex, edit, vi, view.
	 */
	ivis = any('v', av[0]); /* "vi" */
	if (any('w', av[0]))	/* "view" */
		value(READONLY) = 1;
	if (any('d', av[0])) {	/* "edit" or "vedit" */
		value(NOVICE) = 1;
		value(REPORT) = 1;
		value(MAGIC) = 0;
		value(SHOWMODE) = 1;
	}

	/*
	 * Open the error message file.
	 */
	draino();
	pstop();

	/*
	 * Initialize interrupt handling.
	 */
	oldhup = signal(SIGHUP, SIG_IGN);
	if (oldhup == SIG_DFL)
		signal(SIGHUP, onhup);
	oldquit = signal(SIGQUIT, SIG_IGN);
	ruptible = (signal(SIGINT, SIG_IGN) == SIG_DFL);
	if (signal(SIGTERM, SIG_IGN) == SIG_DFL)
	signal(SIGILL, oncore);
	signal(SIGTRAP, oncore);
	signal(SIGIOT, oncore);
	signal(SIGFPE, oncore);
	signal(SIGBUS, oncore);
	signal(SIGSEGV, oncore);
	signal(SIGPIPE, oncore);

	/*
	 * Process flag arguments.
	 */
	ac--, av++;
	while (ac && av[0][0] == '-') {
		c = av[0][1];
		if (c == 0) {
			hush = 1;
			value(AUTOPRINT) = 0;
			fast++;
		} else switch (c) {

		case 'R':
			value(READONLY) = 1;
			break;

		case 'T':
#ifdef TRACE
			if (av[0][2] == 0)
				tracef = "trace";
			else {
				static char tttrace[] = { '/','d','e','v','/','t','t','y','x','x',0 };
				tracef = tttrace;
				tracef[8] = av[0][2];
				if (tracef[8])
					tracef[9] = av[0][3];
				else
					tracef[9] = 0;
			}
			trace = fopen(tracef, "w");
			if (trace == NULL)
				ex_printf("Trace create error\n");
			else
#if 1
				setbuf(trace,NULL);
#else
				setbuffer(trace,tracbuf,sizeof(tracbuf));
#endif
#endif
			break;

		case 'l':
			value(LISP) = 1;
			value(SHOWMATCH) = 1;
			break;

		case 'r':
			recov++;
			break;

		case 'V':
			verbose = 1;
			break;

		case 't':
			if (ac > 1 && av[1][0] != '-') {
				ac--, av++;
				itag = 1;
				/* truncates too long tag. */
				NCdecstr( av[0], lasttag,
					 NCSIZE(lasttag));
			}
			break;

		case 'v':
			ivis = 1;
			break;

		case 'w':
			defwind = 0;
			if (av[0][2] == 0) defwind = 3;
			else for (cp = &av[0][2];
				  isascii(*cp) && isdigit(*cp);
				  cp++)
				defwind = 10*defwind + *cp - '0';
			break;

		default:
			smerror(msg(M_000, "Unknown option %s\n"), av[0]);
			break;
		}
		ac--, av++;
	}

#ifdef SIGTSTP
# ifdef		TIOCLGET	/* Berkeley 4BSD */
	if (!hush && olttyc.t_suspc != '\377'
	    && signal(SIGTSTP, SIG_IGN) == SIG_DFL){
 		dosusp++;
		signal(SIGTSTP, onsusp);
	}
#endif
#endif

	if (ac && av[0][0] == '+') {
	    firstpat = firstpatbuf;
	    NCdecstr(&av[0][1],firstpat,NCSIZE(firstpatbuf));
	    ac--, av++;
	}

	/*
	 * If we are doing a recover and no filename
	 * was given, then execute an exrecover command with
	 * the -r option to type out the list of saved file names.
	 * Otherwise set the remembered file name to the first argument
	 * file name so the "recover" initial command will find it.
	 */
	if (recov) {
		if (ac == 0) {
			ppid = 0;
			setrupt();
			execl(EXRECOVER, "exrecover", "-r", 0);
			filioerr(EXRECOVER);
			exit(1);
		}
		strcpy(savedfile, *av++), ac--;
	}

	/*
	 * Initialize the argument list.
	 */
	argv0 = av;
	argc0 = ac;
	args0 = av[0];

	/*
	 * Initialize a temporary file (buffer) and
	 * set up terminal environment.  Read user startup commands.
	 */
	if (setexit() == 0) {
		setrupt();
		intty = isatty(0);
		value(PROMPT) = intty;
		if (cp = getenv("SHELL"))
			strcpy(shell, cp);
		if (fast)
			setterm("dumb");
		else {
			gettmode();
			cp = getenv("TERM");
			if (cp == NULL || *cp == '\0')
				cp = "unknown";
			setterm(cp);
		}
	}

	erewind();

	if (setexit() == 0 && !fast) {
		char *p = getenv ("EXINIT");

		if (p && *p) {
			/* need to count the number of code points */
			int len = NLstrdlen(p) + 1;

			globp = (NLchar *) malloc(sizeof(NLchar) * len);
			NCdecstr(p, globp, len);
			commands(1,1);
		}
		else {
			globp = 0;
			if ((cp = getenv("HOME")) != NULL && *cp) {
				strcat(strcpy(buft, cp), "/.exrc");
				if (iownit(buft))
					source(buft,1);
			}
		}
		/*
		 * Allow local .exrc too.  This loses if . is $HOME,
		 * but nobody should notice unless they do stupid things
		 * like putting a version command in .exrc.  Besides,
		 * they should be using EXINIT, not .exrc, right?
		 */
		if (iownit(".exrc"))
			source(".exrc", 1);
	}
#ifdef OSF_MMAP
        pagesize = sysconf(_SC_PAGE_SIZE);
        fendcore = (line *)mmap(NULL, pagesize, PROT_READ | PROT_WRITE,
                                MAP_ANONYMOUS | MAP_PRIVATE | MAP_VARIABLE,
                                -1, (off_t)0);
        endcore = fendcore + (pagesize/sizeof(line)) - 1;
#else
#ifdef  UNIX_SBRK
	/*
	 * Initialize end of core pointers.
	 * Normally we avoid breaking back to fendcore after each
	 * file since this can be expensive (much core-core copying).
	 * If your system can scatter load processes you could do
	 * this as ed does, saving a little core, but it will probably
	 * not often make much difference.
	 */
	fendcore = (line *) sbrk(0);
	endcore = fendcore - 2;
#else
	/*
	 * Allocate all the memory we will ever use in one chunk.
	 * This is for system such as VMS where sbrk() does not
	 * guarantee that the memory allocated beyond the end is
	 * consecutive.  VMS's RMS does all sorts of memory allocation
	 * and screwed up ex royally because ex assumes that all
	 * memory up to "endcore" belongs to it and RMS has different
	 * ideas.
	 */
	fendcore = (line *) malloc((unsigned)
		value(LINELIMIT) * sizeof (line *));
	if (fendcore == NULL) {
		lprintf("ex: cannot handle %d lines\n", value(LINELIMIT));
		lprintf("ex: set \"linelimit\" lower\n");
		flush();
		exit(1);
	}
	endcore = fendcore + (value(LINELIMIT) - 1);
#endif
#endif OSF_MMAP
	init(); /* moved after prev 2 chunks to fix directory option */

	/*
	 * Initial processing.	Handle tag, recover, and file argument
	 * implied next commands.  If going in as 'vi', then don't do
	 * anything, just set initev so we will do it later (from within
	 * visual).
	 */
	if (setexit() == 0) {
		static NLchar globbuf[8];	/* fit strlen("recover")+1 */

		if (recov)
		    NCdecstr("recover",globp=globbuf, NCSIZE(globbuf));
		else if (itag)
		    NCdecstr(ivis ? "tag" : "tag|p", globp=globbuf, NCSIZE(globbuf));
		else if (argc)
		    NCdecstr("next",globp=globbuf, NCSIZE(globbuf));
		if (ivis)
			initev = globp;
		else if (globp) {
			inglobal = 1;
			commands(1, 1);
			inglobal = 0;
		}
	}

	/*
	 * Vi command... go into visual.
	 * Strange... everything in vi usually happens
	 * before we ever "start".
	 */
	if (ivis) {
		static NLchar globbuf[7];	/* fit strlen("visual")+1 */

		/*
		 * Don't have to be upward compatible with stupidity
		 * of starting editing at line $.
		 */
		if (dol > zero)
			dot = one;
		NCdecstr("visual",globp = globbuf, NCSIZE(globbuf));
		if (setexit() == 0)
			commands(1, 1);
	}

	/*
	 * Clear out trash in state accumulated by startup,
	 * and then do the main command loop for a normal edit.
	 * If you quit out of a 'vi' command by doing Q or ^\,
	 * you also fall through to here.
	 */
	seenprompt = 1;
	ungetchar(0);
	globp = 0;
	initev = 0;
	setlastchar('\n');
	setexit();
	commands(0, 0);
	cleanup(1);
	exit(0);
/* NOTREACHED */
}

/*
 * Initialization, before editing a new file.
 * Main thing here is to get a new buffer (in fileinit),
 * rest is peripheral state resetting.
 */
init()
{
	register int i;

	fileinit();
	dot = zero = truedol = unddol = dol = fendcore;
	one = zero+1;
	undkind = UNDNONE;
	chng = 0;
	edited = 0;
	for (i = 0; i <= 'z'-'a'+1; i++)
		names[i] = 1;
	anymarks = 0;
}

/*
 * Return last component of unix path name p.
 */
char *
tailpath(p)
register char *p;
{
	register char *r;

	for (r=p; *p; p++)
		if (*p == '/')
			r = p+1;
	return(r);
}

/*
 * Check ownership of file.  Return nonzero if it exists and is owned by the
 * user or the option sourceany is used
 */
iownit(file)
char *file;
{
	struct stat sb;

	if (stat(file, &sb) == 0 && (value(SOURCEANY) || sb.st_uid == getuid()))
		return(1);
	else
		return(0);
}
