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
static char rcsid[] = "@(#)$RCSfile: sem.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:43 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: execute doio mypipe chkclob
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

#include <sys/ioctl.h>
#include "sh.h"
#include "proc.h"

/*
 * C shell
 */

/*VARARGS 1*/
execute(t, wanttty, pipein, pipeout)
	register struct command *t;
	int wanttty, *pipein, *pipeout;
{
	bool forked = 0;
	struct biltins *bifunc;
	pid_t pid = 0;
	int pv[2];

#if defined(DEBUG) && defined(debugc)
printf("debug: entering execute\n");fflush(stdout);
#endif
	if (t == 0)
		return;
#if _BSD
	if ((t->t_dflg & FAND) && wanttty > 0)
#else
	if (t->t_dflg & FAND)
#endif
		wanttty = 0;
	switch (t->t_dtyp) {

	case TCOM:
#if defined(NLS)  || defined(KJI)
		/*
		 * If first byte of command is ALIASCHR, we expanded an alias
		 * name which was defined in terms of itself.  This
		 * prevented an infinite loop.  See asyn3() in parse.c.
		 */
		if (t->t_dcom[0][0] == ALIASCHR)
#else
		if ((t->t_dcom[0][0] & (QUOTE|TRIM)) == QUOTE)
#endif
			strcpy(t->t_dcom[0], t->t_dcom[0] + 1);
		if ((t->t_dflg & FREDO) == 0)
			Dfix(t);                /* $ " ' \ */
		if (t->t_dcom[0] == 0)
			return;
		/* fall into... */

	case TPAR:
		if (t->t_dflg & FPOU)
			mypipe(pipeout);
		/*
		 * Must do << early so parent will know
		 * where input pointer should be.
		 * If noexec then this is all we do.
		 */
		if (t->t_dflg & FHERE) {
			close(0);
			heredoc(t->t_dlef);
			if (noexec)
				close(0);
		}
		if (noexec)
			break;

/****
 Remove (temporarily) this test line that gives exit the last command's
 status. t_dcom[] can be NULL, and cause core-dumps. Proper fix means
 checking this, but safe fix for 1.0 means removing the test and
 breaking exit a little...
                if exit builtin, status comes from last command
                if (!eq(t->t_dcom[0], "exit"))
****/
                        set("status", "0");


		/*
		 * This mess is the necessary kludge to handle the prefix
		 * builtins: nice, nohup, time.  These commands can also
		 * be used by themselves, and this is not handled here.
		 * This will also work when loops are parsed.
		 */
		while (t->t_dtyp == TCOM)
			if (eq(t->t_dcom[0], "nice"))
				if (t->t_dcom[1])
					if (any(t->t_dcom[1][0], "+-"))
						if (t->t_dcom[2]) {
							setname((uchar_t *)"nice");
							t->t_nice = getn(t->t_dcom[1]);
							lshift(t->t_dcom, 2);
							t->t_dflg |= FNICE;
						} else
							break;
					else {
#if _BSD
						t->t_nice = 4;
#else
						t->t_nice = 24;
#endif
						lshift(t->t_dcom, 1);
						t->t_dflg |= FNICE;
					}
				else
					break;
			else if (eq(t->t_dcom[0], "nohup"))
				if (t->t_dcom[1]) {
					t->t_dflg |= FNOHUP;
					lshift(t->t_dcom, 1);
				} else
					break;
			else if (eq(t->t_dcom[0], "time"))
				if (t->t_dcom[1]) {
					t->t_dflg |= FTIME;
					lshift(t->t_dcom, 1);
				} else
					break;
			else
				break;
		/*
		 * Check if we have a builtin function and remember which one.
		 */
		bifunc = t->t_dtyp == TCOM ? isbfunc(t) : (struct biltins *) 0;

		/*
		 * We fork only if we are timed, or are not the end of
		 * a parenthesized list and not a simple builtin function.
		 * Simple meaning one that is not pipedout, niced, nohupped,
		 * or &'d.
		 * It would be nice(?) to not fork in some of these cases.
		 */

#if defined(DEBUG)
printf("execute(TPAR): about to fork\n");fflush(stdout);sleep(1);
#endif
		if (((t->t_dflg & FTIME) || (t->t_dflg & FPAR) == 0 &&
		     (!bifunc || t->t_dflg & (FPOU|FAND|FNICE|FNOHUP))))
			{ forked++; pid = pfork(t, wanttty); }
#if defined(DEBUG)
printf("execute(TPAR): back from fork\n");fflush(stdout);sleep(1);
#endif
		if (pid != 0) {
			/*
			 * It would be better if we could wait for the
			 * whole job when we knew the last process
			 * had been started.  Pwait, in fact, does
			 * wait for the whole job anyway, but this test
			 * doesn't really express our intentions.
			 */

			if (didfds==0 && t->t_dflg&FPIN) {
#if defined(DEBUG) && defined(debugc)
printf("execute(TPAR): closing pipes \n");fflush(stdout);
#endif
				(void) close(pipein[0]);
				(void) close(pipein[1]);
			}
			if ((t->t_dflg & (FPOU|FAND)) == 0)
				pwait();
			break;
		}
#if defined(DEBUG) && defined(debugc)
printf("execute(TPAR): doio \n");fflush(stdout);
#endif
		doio(t, pipein, pipeout);
		if (t->t_dflg & FPOU) {
			(void) close(pipeout[0]);
			(void) close(pipeout[1]);
		}

#if defined(DEBUG) && defined(debugc)
printf("execute(TPAR): execute builtin \n");fflush(stdout);
#endif
		/*
		 * Perform a builtin function.
		 * If we are not forked, arrange for possible stopping
		 */
		if (bifunc) {
			func(t, bifunc);
			if (forked)
				exitstat();
			break;
		}
		if (t->t_dtyp != TPAR) {
#if defined(DEBUG) && defined(debugc)
printf("execute(TPAR): doexec call\n");fflush(stdout);
#endif
			doexec(t);
			/*NOTREACHED*/
		}
		/*
		 * For () commands must put new 0,1,2 in FSH* and recurse
		 */
		OLDSTD = dcopy(0, FOLDSTD);
		SHOUT = dcopy(1, FSHOUT);
		SHDIAG = dcopy(2, FSHDIAG);
		close(SHIN), SHIN = -1;
		didcch = 0, didfds = 0;
		wanttty = -1;
		t->t_dspr->t_dflg |= t->t_dflg & FINT;
#if defined(DEBUG) && defined(debugc)
printf("execute(TPAR): execute\n");fflush(stdout);
#endif
		execute(t->t_dspr, wanttty,(int *)0, (int *)0);
		exitstat();

	case TFIL:
		t->t_dcar->t_dflg |= FPOU |
		    (t->t_dflg & (FPIN|FAND|FDIAG|FINT));
#if defined(DEBUG) && defined(debugc)
printf("execute(TFIL): execute\n");fflush(stdout);
#endif
		execute(t->t_dcar, wanttty, pipein, pv);
		t->t_dcdr->t_dflg |= FPIN |
		    (t->t_dflg & (FPOU|FAND|FPAR|FINT));
		if (wanttty > 0)
			wanttty = 0;		/* got tty already */
#if defined(DEBUG) && defined(debugc)
printf("execute(TFILE): execute2\n");fflush(stdout);
#endif
		execute(t->t_dcdr, wanttty, pv, pipeout);
		break;

	case TLST:
		if (t->t_dcar) {
			t->t_dcar->t_dflg |= t->t_dflg & FINT;
#if defined(DEBUG) && defined(debugc)
printf("execute(TLST): execute\n");fflush(stdout);
#endif
			execute(t->t_dcar, wanttty,(int *)0, (int *)0);
			/*
			 * In strange case of A&B make a new job after A
			 */
			if (t->t_dcar->t_dflg&FAND && t->t_dcdr &&
			    (t->t_dcdr->t_dflg&FAND) == 0)
				pendjob();
		}
		if (t->t_dcdr) {
			t->t_dcdr->t_dflg |= t->t_dflg & (FPAR|FINT);
#if defined(DEBUG) && defined(debugc)
printf("execute(TLST): execute2\n");fflush(stdout);
#endif
			execute(t->t_dcdr, wanttty,(int *)0, (int *)0);
		}
		break;

	case TOR:
	case TAND:
		if (t->t_dcar) {
			t->t_dcar->t_dflg |= t->t_dflg & FINT;
#if defined(DEBUG) && defined(debugc)
printf("execute(TOR): execute\n");fflush(stdout);
#endif
			execute(t->t_dcar, wanttty,(int *)0, (int *)0);
			if ((getn(value("status")) == 0) != (t->t_dtyp == TAND))
				return;
		}
		if (t->t_dcdr) {
			t->t_dcdr->t_dflg |= t->t_dflg & (FPAR|FINT);
#if defined(DEBUG) && defined(debugc)
printf("execute(TOR): execute\n");fflush(stdout);
#endif
			execute(t->t_dcdr, wanttty,(int *)0, (int *)0);
		}
		break;
	}
	/*
	 * Fall through for all breaks from switch
	 *
	 * If there will be no more executions of this
	 * command, flush all file descriptors.
	 * Places that turn on the FREDO bit are responsible
	 * for doing donefds after the last re-execution
	 */
	if (didfds && !(t->t_dflg & FREDO))
		donefds();
}

/*
 * Perform io redirection.
 * We may or maynot be forked here.
 */
doio(t, pipein, pipeout)
	register struct command *t;
	int *pipein, *pipeout;
{
	register uchar_t *cp;
	register int flags = t->t_dflg;

#if defined(DEBUG) && defined(debugc)
printf("doio: flags=%d\n", flags);fflush(stdout);
#endif
	if (didfds || (flags & FREDO))
		return;
	if ((flags & FHERE) == 0) {	/* FHERE already done */
		close(0);
#if defined(DEBUG) && defined(debugc)
if(t->t_dlef == NULL)
{
  printf("doio: t->t_dlef=NULL\n");fflush(stdout);
}
else
{
  printf("doio: t->t_dlef=%s\n", t->t_dlef);fflush(stdout);
}
#endif
		if (cp = t->t_dlef) {
			cp = globone(Dfix1(cp));
			xfree(cp);
			if (open(cp, 0) < 0)
				Perror(cp);
                } else if (flags & FPIN) {
                        track_open(dup(pipein[0]));
                        close(pipein[0]);
                        close(pipein[1]);
                }
		else if ((flags & FINT) && tpgrp == -1)
			close(0), open("/dev/null", 0);
		else
			track_open(dup(OLDSTD));
	}
	close(1);
#if defined(DEBUG) && defined(debugc)
printf("doio: close of stdout\n");fflush(stdout);
#endif
	if (cp = t->t_drit) {
		cp = globone(Dfix1(cp));
		xfree(cp);
		if ((flags & FCAT) && open(cp, 1) >= 0)
			lseek(1, 0l, 2);
		else {
			if (!(flags & FANY) && adrof("noclobber")) {
				if (flags & FCAT)
					Perror(cp);
				chkclob(cp);
			}
			if (creat(cp, 0666) < 0)
				Perror(cp);
		}
        } else if (flags & FPOU) {
                track_open(dup(pipeout[1]));
        }
        else {
                track_open(dup(SHOUT));
        }

#if defined(DEBUG) && defined(debugc)
printf("doio: after dup\n");fflush(stdout);
#endif
	close(2);
	track_open(dup((flags & FDIAG) ? 1 : SHDIAG));
	didfds = 1;
#if defined(DEBUG) && defined(debugc)
printf("doio return\n");fflush(stdout);
#endif
}

mypipe(pv)
	register int *pv;
{

	if (pipe(pv) < 0)
		goto oops;
	pv[0] = dmove(pv[0], -1);
	pv[1] = dmove(pv[1], -1);
	if (pv[0] >= 0 && pv[1] >= 0)
		return;
oops:
	error(MSGSTR(M_NOPIPE, "Can't make pipe"));
}

chkclob(cp)
	register uchar_t *cp;
{
	struct stat stb;
	char e[NL_TEXTMAX];

	if (stat((char *)cp, &stb) < 0)
		return;
	if ((stb.st_mode & S_IFMT) == S_IFCHR)
		return;
	sprintf(e,MSGSTR(M_FILE, "%s: File exists"), cp);
	error(e);
}
