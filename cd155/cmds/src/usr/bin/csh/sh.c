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
static char rcsid[] = "@(#)$RCSfile: sh.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:45 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: main untty importpath srccat srcunit rechist goodbye exitstat 
 *            phup pintr pintr1 process dosource mailchk gethdir initdesc exit
 *
 * ORIGINS: 10,26,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#include <fcntl.h>
#include <locale.h>
#include <pwd.h>
#include "sh.h"
/*
 * C Shell
 *
 * Bill Joy, UC Berkeley, California, USA
 * October 1978, May 1980
 *
 * Jim Kulp, IIASA, Laxenburg, Austria
 * April 1980
 */
void	phup();

int	ooSHIN;		
char	*pathlist[] =	{ ".", "/bin", "/usr/bin", 0 };
char	*dumphist[] =	{ "history", "-h", 0, 0 };
char	*loadhist[] =	{ "source", "-h", "~/.history", 0 };
#ifdef KJI
int	HIST = '!';
int	HISTSUB = '^';
#else
uchar_t	HIST = '!';
uchar_t	HISTSUB = '^';
#endif
#ifdef _AIX
int	nofile = 0;
#else
bool	nofile;
#endif
bool	reenter;
bool	nverbose;
bool	nexececho;
bool	quitit;
bool	fast;
bool    batch;
bool	prompt = 1;
bool	enterhist = 0;
#if defined(NLS) || defined(KJI)
bool    NLdebug = 0;
#endif

#ifdef MSG
nl_catd catd;       /* message catalog descriptor */
int	FSHMSG = -1;
#endif

int open_max = {0};     /* highest file descriptor opened; we have to track
                           this, because OPEN_MAX is too big.
                         */


main(c, av)
	int c;
	uchar_t **av;
{
	register uchar_t **v, *cp;
	register int f;
	struct sigvec osv;

        settimes();                     /* Immed. estab. timing base */

#ifdef MSG
	/* We need to specify the file descriptor to be used by the
	 * message facility so that we can avoid closing it later.
	 * Also note that we cannot do a deferred open of the message
	 * catalog because the shell frees the storage of error values 
	 * before its error routines print out those values, counting 
	 * on no intervening alloc'ing -- which could happen on the 
	 * first error message if a deferred open is used.
	 */

	catd = NLcatopen(MF_CSH, FSHMSG);
        if (catd->_fd > (FILE *)NULL) {
              FSHMSG = fileno(catd->_fd);
            track_open(FSHMSG);
        }

#endif
	v = av;
	if (eq(v[0], "a.out"))		/* A.out's are quittable */
		quitit = 1;
	uid = getuid();
	loginsh = **v == '-';
	if (loginsh)
		time(&chktim);

	/*
	 * Move the descriptors to safe places.
	 * The variable didfds is 0 while we have only FSH* to work with.
	 * When didfds is true, we have 0,1,2 and prefer to use these.
	 */
#if defined(DEBUG) && defined(debugp)
printf("main: calling initdesc\n"); fflush(stdout);
#endif
	initdesc();

	/*
	 * Initialize the shell variables.
	 * ARGV and PROMPT are initialized later.
	 * STATUS is also munged in several places.
	 * CHILD is munged when forking/waiting
	 */

	set("status", "0");
	dinit(cp = getenv("HOME"));	/* dinit thinks that HOME == cwd in a
					 * login shell */
	if (cp == NOSTR)
		fast++;			/* No home -> can't read scripts */
	else
		set("home", savestr(cp));
	/*
	 * Grab other useful things from the environment.
	 * Should we grab everything??
	 */
	if ((cp = getenv("TERM")) != NOSTR)
		set("term", savestr(cp));

        /*
         * Initialization of $USER, $user:
         *    if $USER exists, copy it into $user
         *    else if $LOGNAME exists, copy it into $USER and $user
         */
        if ((cp = getenv("USER")) != NOSTR)
                set("user", savestr(cp));
        else if ((cp = getenv("LOGNAME")) != NOSTR) {
                setenv("USER", cp);
                set("user", savestr(cp));
        }

	/*
	 * Re-initialize path if set in environment
	 */
	if ((cp = getenv("PATH")) == NOSTR)
		set1("path", saveblk(pathlist), &shvhed);
	else {
		register unsigned i = 0;
		register uchar_t *dp;
		register uchar_t **pv;

		for (dp = cp; *dp; dp++)
			if (*dp == ':')
				i++;
		pv = (uchar_t **)calloc(i+2, sizeof (char **));
		for (dp = cp, i = 0; ;)
			if (*dp == ':') {
				*dp = 0;
				pv[i++] = savestr(*cp ? cp : (uchar_t *)".");
				*dp++ = ':';
				cp = dp;
			} else if (*dp++ == 0) {
				if (*cp)
					pv[i++] = savestr(cp);
				break;
			}
		pv[i] = 0;
		set1("path", pv, &shvhed);
	}

	set("shell", SHELLPATH);
#ifdef CMDEDIT
        if ((cp = getenv("EDITMODE")) != NOSTR)
                set("editmode", savestr(cp));
#endif

	doldol = putn(getpid());		/* For $$ */
	shtemp = strspl("/tmp/sh", doldol);	/* For << */

	setlocale( LC_ALL, "");

	/*
	 * Record the interrupt states from the parent process.
	 * If the parent is non-interruptible our hand must be forced
	 * or we (and our children) won't be either.
	 * Our children inherit termination from our parent.
	 * We catch it only if we are the login shell.
	 */
		/* parents interruptibility */
	(void) sigvec(SIGINT, (struct sigvec *)0, &osv);
	parintr = osv.sv_handler;
		/* parents terminability */
	(void) sigvec(SIGTERM, (struct sigvec *)0, &osv);
	parterm = osv.sv_handler;
	if (loginsh) {
		struct sigvec nsv;

		nsv.sv_handler = (void (*)(int))phup;
		nsv.sv_mask = 0;
		nsv.sv_onstack = 0;
		(void)sigvec(SIGHUP, &nsv,
			(struct sigvec *)0);   /* exit processing on HUP */
		(void)sigvec(SIGXCPU, &nsv,
			(struct sigvec *)0);  /* ...and on XCPU */
		(void)sigvec(SIGXFSZ, &nsv,
			(struct sigvec *)0);  /* ...and on XFSZ */
	}

#if defined(DEBUG) && defined(debugp)
	printf("main: processing arguments\n");fflush(stdout);
#endif
	/*
	 * Process the arguments.
	 *
	 * Note that processing of -v/-x is actually delayed till after
	 * script processing.
	 *
	 * We set the first character of our name to be '-' if we are
	 * a shell running interruptible commands.  Many programs which
	 * examine ps'es use this to filter such shells out.
	 */
	c--, v++;
	while (c > 0 && (cp = v[0])[0] == '-' && !batch) {
		do switch (*cp++) {

		case 0:			/* -	Interruptible, no prompt */
			prompt = 0;
			setintr++;
			nofile++;
			break;

		case 'b':               /* -b   Next arg is input file */
			batch++;
			break;

		case 'c':		/* -c	Command input from arg */
			if (c == 1)
				exit(0);
			c--, v++;
			arginp = v[0];
			prompt = 0;
			nofile++;
			break;

		case 'e':		/* -e	Exit on any error */
			exiterr++;
			break;

		case 'f':		/* -f	Fast start */
			fast++;
			break;

		case 'i':		/* -i	Interactive, even if !intty */
			intact++;
			nofile++;
			break;

		case 'n':		/* -n	Don't execute */
			noexec++;
			break;

		case 'q':		/* -q	(Undoc'd) ... die on quit */
			quitit = 1;
			break;

		case 's':		/* -s	Read from std input */
			nofile++;
			break;

		case 't':		/* -t	Read one line from input */
			onelflg = 2;
			prompt = 0;
			nofile++;
			break;

		case 'v':		/* -v	Echo hist expanded input */
			nverbose = 1;			/* ... later */
			break;

		case 'x':		/* -x	Echo just before execution */
			nexececho = 1;			/* ... later */
			break;

#if defined(NLS) || defined(KJI)
		case 'D':
			NLdebug = 1;
			break;
#endif

		case 'V':		/* -V	Echo hist expanded input */
			setNS("verbose");		/* NOW! */
			break;

		case 'X':		/* -X	Echo just before execution */
			setNS("echo");			/* NOW! */
			break;

		} while (*cp);
		v++, c--;
	}

	if (quitit)  {                  /* With all due haste, for debugging */
		struct sigvec nsv;

		nsv.sv_handler = SIG_DFL;
		nsv.sv_mask = 0;
		nsv.sv_onstack = 0;
		(void)sigvec(SIGQUIT, &nsv,
			(struct sigvec *)0);   /* exit processing on HUP */
	}

	/*
	 * Unless prevented by -, -c, -i, -s, or -t, if there
	 * are remaining arguments the first of them is the name
	 * of a shell file from which to read commands.
	 */
	if (nofile == 0 && c > 0) {
		nofile = open((char *)v[0], 0);
#if defined(DEBUG) && defined(debugp)
	printf("main: open(%s) yields %d\n",v[0],nofile);fflush(stdout);
#endif
		if (nofile < 0) {
			child++;		/* So this ... */
			Perror(v[0]);		/* ... doesn't return */
		}
                track_open(nofile);
		file = v[0];
		SHIN = dmove(nofile, FSHIN);	/* Replace FSHIN */
		prompt = 0;
		c--, v++;
	}
	if (!batch && (uid != geteuid() || getgid() != getegid())) {
		errno = EACCES;
		child++;                        /* So this ... */
		Perror("csh");                  /* ... doesn't return */
	}
	/*
	 * Consider input a tty if it really is or we are interactive.
	 */
	intty = intact || isatty(SHIN);
	/*
	 * Decide whether we should play with signals or not.
	 * If we are explicitly told (via -i, or -) or we are a login
	 * shell (arg0 starts with -) or the input and output are both
	 * the ttys("csh", or "csh</dev/ttyx>/dev/ttyx")
	 * Note that in only the login shell is it likely that parent
	 * may have set signals to be ignored
	 */
	if (loginsh || intact || intty && isatty(SHOUT))
		setintr = 1;
#ifdef CMDEDIT
        if (setintr) {
                register struct varent *vp;

                vp = adrof("editmode");
                if (vp && vp->vec[0] && !eq(vp->vec[0], "none"))
                        cmdedit = 1;
        }
#endif
#ifdef TELL
	settell();
#endif
	/*
	 * Save the remaining arguments in argv.
	 */
	setq("argv", v, &shvhed);

	/*
	 * Set up the prompt.
	 */
	if (prompt)
                set("prompt", uid == 0 ? "# " : "% ");

	/*
	 * If we are an interactive shell, then start fiddling
	 * with the signals; this is a tricky game.
	 */
	shpgrp = getpgrp();
	opgrp = tpgrp = -1;
	oldisc = -1;
#if defined(DEBUG) && defined(debugp)
	printf("main: setintr = %d\n", setintr);fflush(stdout);
#endif
	if (setintr) {
		struct sigvec nsv;

		**av = '-';
		nsv.sv_handler = SIG_IGN;
		nsv.sv_mask = SA_RESTART;
		nsv.sv_onstack = 0;
		if (!quitit)		/* Wary! */
			(void)sigvec(SIGQUIT, &nsv, (struct sigvec *)0);
		nsv.sv_handler = (void (*)(int))pintr;
		(void)sigvec(SIGINT, &nsv, (struct sigvec *)0);
		(void) sigblock(sigmask(SIGINT));
		nsv.sv_handler = SIG_IGN;
		(void)sigvec(SIGTERM, &nsv, (struct sigvec *)0);
		if (quitit == 0 && arginp == 0) {
			(void)sigvec(SIGTSTP, &nsv, (struct sigvec *)0);
			(void)sigvec(SIGTTIN, &nsv, (struct sigvec *)0);
			(void)sigvec(SIGTTOU, &nsv, (struct sigvec *)0);
			/*
			 * Wait till in foreground, in case someone
			 * stupidly runs
			 *	csh &
			 * dont want to try to grab away the tty.
			 */
			if (isatty(FSHDIAG))
				f = FSHDIAG;
			else if (isatty(FSHOUT))
				f = FSHOUT;
			else if (isatty(OLDSTD))
				f = OLDSTD;
			else
				f = -1;
retry:
			if (ioctl(f, TIOCGPGRP, &tpgrp) == 0 && tpgrp != -1) {
				int ldisc;
				struct sigvec osv;
#if defined(DEBUG) && defined(debugp)
	printf("main: tpgrp = %d  shpgrp = %d\n", tpgrp, shpgrp);fflush(stdout);
#endif
				if (tpgrp != shpgrp) {
					nsv.sv_handler = SIG_DFL;
					nsv.sv_mask = SA_RESTART;
					nsv.sv_onstack = 0;
					(void)sigvec(SIGTTIN, &nsv, &osv);
					kill(getpid(), SIGTTIN);
					(void)sigvec(SIGTTIN, &osv,
						(struct sigvec *)0);
					goto retry;
				}
				opgrp = shpgrp;
				shpgrp = getpid();
				tpgrp = shpgrp;
#if defined(DEBUG) && defined(debugp)
	printf("main: setting pgrp tpgrp = %d  shpgrp = %d\n", tpgrp, shpgrp);fflush(stdout);
#endif
				{
				struct sigvec nsv, osv;
		
				nsv.sv_handler = SIG_IGN;
				nsv.sv_mask = 0;
				nsv.sv_onstack = 0;
				(void)sigvec(SIGTTOU, &nsv, &osv);
				ioctl(f, TIOCSPGRP, &shpgrp);
				(void)sigvec(SIGTTOU, &osv, (struct sigvec *)0);
				}
  				setpgid(0, shpgrp);
				dcopy(f, FSHTTY);
				/* Replaced with fcntl to set close on exec */
				/*
				ioctl(FSHTTY, FIOCLEX, 0);
				*/
				fcntl(FSHTTY, F_SETFD, 0);
			} else {

notty:
 printf(MSGSTR(M_NOTTY,
 "Warning: no access to tty; thus no job control in this shell...\n"));
				tpgrp = -1;
#ifdef CMDEDIT
                                cmdedit = 0;
#endif

			}
		}
	}
#if defined(DEBUG) && defined(debugp)
	printf("main: after setting pgrp tpgrp = %d  shpgrp = %d\n", tpgrp, shpgrp);fflush(stdout);
#endif
	if (setintr == 0 && (void(*)(int))parintr == SIG_DFL)
		setintr++;
	{       /* while signals not ready */
		struct sigvec nsv;
		nsv.sv_handler = (void (*)(int))pchild;
		nsv.sv_mask = 0;
		nsv.sv_onstack = 0;
		(void)sigvec(SIGCHLD, &nsv, (struct sigvec *)0);
	}

	/*
	 * Set an exit here in case of an interrupt or error reading
	 * the shell start-up scripts.
	 */
	setexit();
	haderr = 0;		/* In case second time through */
	if (!fast && reenter == 0) {
		reenter++;
		/* Will have value("home") here because set fast if don't */
		srccat(value("home"), "/.cshrc");
		if (!fast && !arginp && !onelflg)
			dohash();
		if (loginsh) {
			srccat(value("home"), "/.login");
		}
		dosource(loadhist);
	}

	/*
	 * Now are ready for the -v and -x flags
	 */
	if (nverbose)
		setNS("verbose");
	if (nexececho)
		setNS("echo");

	/*
	 * All the rest of the world is inside this call.
	 * The argument to process indicates whether it should
	 * catch "error unwinds".  Thus if we are a interactive shell
	 * our call here will never return by being blown past on an error.
	 */
	process(setintr);
	/*
	 * Mop-up.
	 */
	if (loginsh) {
		printf(MSGSTR(M_BYEBYE, "logout\n"));
		close(SHIN);
		child++;
		goodbye();
	}
	rechist();
	exitstat();
}

untty()
{
	struct sigvec nsv, osv;
		
	nsv.sv_handler = SIG_IGN;
	nsv.sv_mask = SA_RESTART;
	nsv.sv_onstack = 0;
	(void)sigvec(SIGTTOU, &nsv, &osv);
	if (tpgrp > 0) {
		setpgid(0, opgrp); 
		ioctl(FSHTTY, TIOCSPGRP, &opgrp);
	}
	(void)sigvec(SIGTTOU, &osv, (struct sigvec *)0);
}

importpath(cp)
uchar_t *cp;
{
	register int i = 0;
	register uchar_t *dp;
	register uchar_t **pv;
	static uchar_t dot[2] = {'.', 0};

	for (dp = cp; *dp; dp++)
		if (*dp == ':')
			i++;
	/*
	 * i+2 where i is the number of colons in the path.
	 * There are i+1 directories in the path plus we need
	 * room for a zero terminator.
	 */
	pv = (uchar_t **) calloc(i+2, sizeof (char **));
	dp = cp;
	i = 0;

	for (dp = cp; *dp; ++dp) {

		if (*dp == ':') {
			*dp = 0;
			pv[i++] = savestr(*cp ? cp : dot);
			cp = dp + 1;
			*dp = ':';
		}
	}
	if (*cp)
		pv[i++] = savestr(cp);

	pv[i] = 0;
	set1("path", pv, &shvhed);
}

/*
 * Source to the file which is the catenation of the argument names.
 */
srccat(cp, dp)
	uchar_t *cp, *dp;
{
	register uchar_t *ep = strspl(cp, dp);
	register int unit = dmove(open((char *)ep, 0), -1);

	/* ioctl(unit, FIOCLEX, NULL); */
	xfree(ep);
#ifdef INGRES
	srcunit(unit, 0, 0);
#else
	srcunit(unit, 1, 0);
#endif
}

/*
 * Source to a unit.  If onlyown it must be our file or our group or
 * we don't chance it.	This occurs on ".cshrc"s and the like.
 */
srcunit(unit, onlyown, hflg)
	register int unit;
	bool onlyown;
	bool hflg;
{
	/* We have to push down a lot of state here */
	/* All this could go into a structure */
	int oSHIN = -1, oldintty = intty;
	int *tmp = &oSHIN;    
	struct whyle *oldwhyl = whyles;
	uchar_t *ogointr = gointr, *oarginp = arginp;
	uchar_t *oevalp = evalp, **oevalvec = evalvec;
	int oonelflg = onelflg;
	bool oenterhist = enterhist;
#ifdef KJI
	int OHIST = HIST;
#else
	uchar_t OHIST = HIST;
#endif
#ifdef TELL
	bool otell = cantell;
#endif
	struct Bin saveB;

	/* The (few) real local variables */
	jmp_buf oldexit;
	static int reenter;
	int omask;

	if (unit < 0)
		return;
	if (didfds)
		donefds();
	if (onlyown) {
		struct stat stb;

		if (fstat(unit, &stb) < 0 ||
		    (stb.st_uid != uid && stb.st_gid != getgid())) {
			close(unit);
			return;
		}
	}

	/*
	 * There is a critical section here while we are pushing down the
	 * input stream since we have stuff in different structures.
	 * If we weren't careful an interrupt could corrupt SHIN's Bin
	 * structure and kill the shell.
	 *
	 * We could avoid the critical region by grouping all the stuff
	 * in a single structure and pointing at it to move it all at
	 * once.  This is less efficient globally on many variable references
	 * however.
	 */
	getexit(oldexit);
	reenter = 0;
	if (setintr)
		omask = sigblock(sigmask(SIGINT));
	setexit();
	reenter++;
	if (reenter == 1) {
		/* Setup the new values of the state stuff saved above */
		copy((char *)&saveB, (char *)&B, sizeof saveB);
		fbuf = (uchar_t **) 0;
		fseekp = feobp = fblocks = 0;
		ooSHIN = SHIN;  
		oSHIN = SHIN, SHIN = unit, arginp = 0, onelflg = 0;
		intty = isatty(SHIN), whyles = 0, gointr = 0;
		evalvec = 0; evalp = 0;
		enterhist = hflg;
		if (enterhist)
			HIST = '\0';
		/*
		 * Now if we are allowing commands to be interrupted,
		 * we let ourselves be interrupted.
		 */
		if (setintr)
			(void) sigsetmask(omask);
#ifdef TELL
		settell();
#endif
		process(0);		/* 0 -> blow away on errors */
	}
	if (setintr)
		(void) sigsetmask(omask);
	if (oSHIN >= 0 || reenter == 2) {   
		register int i;

		/* We made it to the new state... free up its storage */
		/* This code could get run twice but xfree doesn't care */
		for (i = 0; i < fblocks; i++)
			xfree(fbuf[i]);
		xfree((char *)fbuf);

		/* Reset input arena */
		copy((char *)&B, (char *)&saveB, sizeof B);

		close(SHIN), SHIN = oSHIN;
		arginp = oarginp, onelflg = oonelflg;
		evalp = oevalp, evalvec = oevalvec;
		intty = oldintty, whyles = oldwhyl, gointr = ogointr;
		if (enterhist)
			HIST = OHIST;
		enterhist = oenterhist;
#ifdef TELL
		cantell = otell;
#endif
	}

	resexit(oldexit);
	/*
	 * If process reset() (effectively an unwind) then
	 * we must also unwind.
	 */
	if (reenter >= 2)
		error(NOSTR);
}

rechist()
{
	uchar_t buf[BUFR_SIZ];
	int fp, ftmp, oldidfds;

	if (!fast) {
		if (value("savehist")[0] == '\0')
			return;
		strcpy(buf, value("home"));
		strcat(buf, "/.history");
		fp = creat((char *)buf, 0777);
		if (fp == -1)
			return;
		oldidfds = didfds;
		didfds = 0;
		ftmp = SHOUT;
		SHOUT = fp;
		strcpy(buf, value("savehist"));
		dumphist[2] = (char *)buf;
		dohist(dumphist);
		close(fp);
		SHOUT = ftmp;
		didfds = oldidfds;
	}
}

goodbye()
{
	struct sigvec nsv;

	nsv.sv_handler = SIG_IGN;
	nsv.sv_mask = 0;
	nsv.sv_onstack = 0;
	if (loginsh) {
		(void)sigvec(SIGQUIT, &nsv, (struct sigvec *)0);
		(void)sigvec(SIGINT, &nsv, (struct sigvec *)0);
		(void)sigvec(SIGTERM, &nsv, (struct sigvec *)0);
		/*
		 * Allow background processes forked from "~/.logout" to
		 * continue running after logout is completed.
		 *
		 * This allows cleanup routines, etc. to be run automatically
		 * when the user logs out. I can't see any reason not to
		 * allow this since processes can be automatically run when the
		 * user isn't logged on anyway, using at(1).
		 *
		 * Although the 4.1 and 4.2 version of csh don't explicitly
		 * have this code (i.e. catching SIGHUP during the sourcing of
		 * .logout), I believe they do allow the continued execution of
		 * processes forked from the .logout sourcing; perhaps this is
		 * due to a different definition of "process group"?
		 */
		(void)sigvec(SIGHUP, &nsv, (struct sigvec *)0);
		setintr = 0;		/* No interrupts after "logout" */
		if (adrof("home"))
			srccat(value("home"), "/.logout");
	}
#ifdef CMDEDIT
        if (!fast && cmdedit)
                SaveMacros();
#endif
	rechist();
	exitstat();
}

exitstat()
{
	int stat;
	/*
	 * Note that if STATUS is corrupted (i.e. getn bombs)
	 * then error will exit directly because we poke child here.
	 * Otherwise we might continue unwarrantedly (sic).
	 */
	child++;
	stat = getn(value("status"));
	exit(stat);
}

/*
 * in the event of a HUP we want to save the history
 */
void phup()
{
	rechist();
	exit(1);
}

char	*jobargv[2] = { "jobs", 0 };
/*
 * Catch an interrupt, e.g. during lexical input.
 * If we are an interactive shell, we reset the interrupt catch
 * immediately.  In any case we drain the shell output,
 * and finally go through the normal error mechanism, which
 * gets a chance to make the shell go away.
 */
void pintr()
{
	pintr1(1);
}

pintr1(wantnl)
	bool wantnl;
{
	register uchar_t **v;
	int omask;

	omask = sigblock(0);
	if (setintr) {
		(void) sigsetmask(omask & ~sigmask(SIGINT));
		if (pjobs) {
			pjobs = 0;
			printf("\n");
			dojobs(jobargv);
			bferr(MSGSTR(M_INTER, "Interrupted"));
		}
	}
	(void) sigsetmask(omask & ~sigmask(SIGCHLD));
	draino();

	/*
	 * If we have an active "onintr" then we search for the label.
	 * Note that if one does "onintr -" then we shan't be interruptible
	 * so we needn't worry about that here.
	 */
	if (gointr) {
		search(ZGOTO, 0, gointr);
		timflg = 0;
		if (v = pargv)
			pargv = 0, blkfree(v);
		if (v = gargv)
			gargv = 0, blkfree(v);
		reset();
	} else if (intty && wantnl)
		printf("\n");		/* Some like this, others don't */
	error(NOSTR);
}

/*
 * Process is the main driving routine for the shell.
 * It runs all command processing, except for those within { ... }
 * in expressions (which is run by a routine evalav in sh.exp.c which
 * is a stripped down process), and `...` evaluation which is run
 * also by a subset of this code in sh.glob.c in the routine backeval.
 *
 * The code here is a little strange because part of it is interruptible
 * and hence freeing of structures appears to occur when none is necessary
 * if this is ignored.
 *
 * Note that if catch is not set then we will unwind on any error.
 * If an end-of-file occurs, we return.
 */
process(catch)
	bool catch;
{
	register uchar_t *cp;
	jmp_buf osetexit;
	struct command *t;
#ifdef KJI
	register int	 n;
	int		 nl;
	register int    *nlc = &nl;
#endif

	getexit(osetexit);
	for (;;) {
#ifdef CMDEDIT
                int sawhist;
                int edithist = (adrof("edithist") != 0);
#endif
		pendjob();
		paraml.next = paraml.prev = &paraml;
		paraml.word = (uchar_t *)"";
		t = 0;
		setexit();
		justpr = enterhist;	/* execute if not entering history */

		/*
		 * Interruptible during interactive reads
		 */
		if (setintr)
			(void) sigsetmask(sigblock(0) & ~sigmask(SIGINT));

		/*
		 * For the sake of reset()
		 */
		freelex(&paraml), freesyn(t), t = 0;

		if (haderr) {
			if (!catch) {
				/* unwind */
				doneinp = 0;
				resexit(osetexit);
				reset();
			}
			haderr = 0;
			/*
			 * Every error is eventually caught here or
			 * the shell dies.  It is at this
			 * point that we clean up any left-over open
			 * files, by closing all but a fixed number
			 * of pre-defined files.  Thus routines don't
			 * have to worry about leaving files open due
			 * to deeper errors... they will get closed here.
			 */
			closem();
			continue;
		}
		if (doneinp) {
			doneinp = 0;
			break;
		}
		if (chkstop)
			chkstop--;
		if (neednote)
			pnote();
		if (intty && evalvec == 0 && !arginp) {
			mailchk();
			/*
			 * If we are at the end of the input buffer
			 * then we are going to read fresh stuff.
			 * Otherwise, we are rereading input and don't
			 * need or want to prompt.
			 */
#if defined(DEBUG) && defined(debugp)
printf("debug: process\n");
#endif
			if (fseekp == feobp)
				printprompt();
		}
		err = 0;

		/*
		 * Echo not only on VERBOSE, but also with history expansion.
		 * If there is a lexical error then we forego history echo.
		 */
#ifdef CMDEDIT
                /* Also push back input if history went off in
                 * interactive mode.
                 */
                if ((sawhist=lex(&paraml)) && !err && intty ||
                    adrof("verbose")) {
                        haderr = 1;
                        if ((justpr || edithist) && sawhist && cmdedit)
                            prpushlex(&paraml);
                        else
                            prlex(&paraml);
                        haderr = 0;
                }
#else   /* CMDEDIT */
		if (lex(&paraml) && !err && intty ||
		    adrof("verbose")) {
			haderr = 1;
			prlex(&paraml);
			haderr = 0;
		}
#endif /* CMDEDIT */

		/*
		 * The parser may lose space if interrupted.
		 */
		if (setintr)
			(void) sigblock(sigmask(SIGINT));

		/*
		 * Save input text on the history list if 
		 * reading in old history, or it
		 * is from the terminal at the top level and not
		 * in a loop.
		 */
#ifdef CMDEDIT
                /* ... unless we have just pushed the command back.
                 */
                if (!((justpr || edithist) && sawhist && cmdedit))
#endif
		if (enterhist || catch && intty && !whyles)
			savehist(&paraml);

		/*
		 * Print lexical error messages, except when sourcing
		 * history lists.
		 */
		if (!enterhist && err)
			error(err);

		/*
		 * If had a history command :p modifier then
		 * this is as far as we should go
		 */
#ifdef CMDEDIT
                /* ... or if we had an interactive history expand
                 */
                if (justpr || (edithist && sawhist && cmdedit))
                        reset();
#else

		if (justpr)
			reset();
#endif

		alias(&paraml);

		/*
		 * Parse the words of the input into a parse tree.
		 */
		t = syntax(paraml.next, &paraml, 0);
		if (err)
			error(err);

		/*
		 * Execute the parse tree
		 */
#if defined(DEBUG) && defined(debugp)
printf("process: calling execute\n");
#endif
		execute(t, tpgrp, (int *)0, (int *)0);
#if defined(DEBUG) && defined(debugp)
printf("process:  back from execute\n");
#endif

		/*
		 * Made it!
		 */
		freelex(&paraml), freesyn(t);
	}
	resexit(osetexit);
}

dosource(t)
	register uchar_t **t;
{
	register uchar_t *f;
	register int u;
	bool hflg = 0;
	uchar_t buf[BUFSIZ];

	t++;
	if (*t && *t[0] == '-') 
	{
		if ( eq(*t, "-h") ) 
		{
			t++;
			hflg++;
		} else 
		{
			flush ();
			printf (MSGSTR(M_OPTION,"Unknown option : %s \n"), *t);
			haderr = 1;
			return;
		}
        }
	strcpy(buf, *t);
	f = globone(buf);
	u = dmove(open((char *)f, 0), -1);
	xfree(f);
	if (u < 0 && !hflg)
		Perror(f);
	srcunit(u, 0, hflg);
}

/*
 * Check for mail.
 * If we are a login shell, then we don't want to tell
 * about any mail file unless its been modified
 * after the time we started.
 * This prevents us from telling the user things he already
 * knows, since the login program insists on saying
 * "You have mail."
 */
mailchk()
{
	register struct varent *v;
	register uchar_t **vp;
	time_t t;
	int intvl, cnt;
	struct stat stb;
	bool new;

	v = adrof("mail");
	if (v == 0)
		return;
	time(&t);
	vp = v->vec;
	cnt = blklen(vp);
	intvl = (cnt && number(*vp)) ? (--cnt, getn(*vp++)) : MAILINTVL;
	if (intvl < 1)
		intvl = 1;
	if (chktim + intvl > t)
		return;
	for (; *vp; vp++) {
		if (stat((char *)*vp, &stb) < 0)
			continue;
		new = stb.st_mtime > time0.tv_sec;
		if (stb.st_size == 0 || stb.st_atime > stb.st_mtime ||
		    (stb.st_atime < chktim && stb.st_mtime < chktim) ||
		    loginsh && !new)
			continue;
		if (cnt == 1)
			printf (new ? MSGSTR(M_NEWMAIL, "You have new mail.\n")
				    : MSGSTR(M_MAIL, "You have mail.\n"));
		else
			printf (new ? MSGSTR(M_NEWMAILIN, "New mail in %s.\n")
				    : MSGSTR(M_MAILIN, "Mail in %s.\n"), *vp);
	}
	chktim = t;
}

/*
 * Extract a home directory from the password file
 * The argument points to a buffer where the name of the
 * user whose home directory is sought is currently.
 * We write the home directory of the user back there.
 */
gethdir(home)
	uchar_t *home;
{
	register struct passwd *pp = getpwnam((char *)home);

	if (pp == 0)
		return (1);
	strcpy(home, pp->pw_dir);
	return (0);
}

/*
 * Move the initial descriptors to their eventual
 * resting places, closin all other units.
 */
initdesc()
{

	didcch = 0;			/* Havent closed for child */
	didfds = 0;			/* 0, 1, 2 aren't set up */
	SHIN = dcopy(0, FSHIN);
	SHOUT = dcopy(1, FSHOUT);
	SHDIAG = dcopy(2, FSHDIAG);
	OLDSTD = dcopy(SHIN, FOLDSTD);
#if defined(DEBUG) && defined(debugp)
printf("initdesc:closing file desc.\n"); fflush(stdout);
#endif
	closem();
}

exit(i)
	int i;
{

	untty();
#ifdef PROF
	IEH3exit(i);
#else
	_exit(i);
#endif
}
printprompt()
{
	register uchar_t *cp;
#ifdef KJI
	register int	 n;
	wchar_t		 nl;
	register wchar_t    *nlc = &nl;
#endif

	if (!whyles)
		for (cp = value("prompt"); *cp; cp++)
#ifdef KJI
		{
			n = NCdec (cp, nlc);
			if (*nlc == HIST) {
				cp += (n-1);
				printf("%d", eventno+1);
			} else {
				if (*cp == '\\')
					cp++;
				putchar(NLQUOTE);
				putchar (*cp);
				if (NCisshift(*cp))
					putchar (*++cp);
			}
		}
#else   /* not KJI */
			if (*cp == HIST)
				printf("%d", eventno+1);
			else {
				if (*cp == '\\' && cp[1] == HIST)
					cp++;
#ifdef NLS
				if ((*cp&QUOTE)==0)
					putchar(NLQUOTE);
				putchar(*cp);
#else
				putchar(*cp | QUOTE);
#endif
			}
#endif  /* not KJI */
	else
		/*
		 * Prompt for forward reading loop
		 * body content.
		 */
		printf("? ");
	flush();
}
