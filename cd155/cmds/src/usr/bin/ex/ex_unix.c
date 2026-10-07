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
#if !defined(lint) && !defined(_NOIDENT)
static char *rcsid = "@(#)$RCSfile: ex_unix.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:09 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_unix.c
 *
 * FUNCTION: filter, recover, revocer, unix0, unixex, unixwt, waitfor
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
 * ex_unix.c  1.4  com/cmd/edit/vi,3.1,9013 12/20/89 11:30:42
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1979 Regents of the University of California */

#include "ex.h"
#include "ex_temp.h"
#include "ex_tty.h"
#include "ex_vis.h"

/*
 * Unix escapes, filtering
 */

/*
 * First part of a shell escape,
 * parse the line, expanding # and % and ! and printing if implied.
 */
unix0(warn)
	short warn;
{

	register char *up, *fp, *xfp;
	register int c;
	/* update nc_uxb in parallel with uxb */
	/* Internal vi uses NLchar's, but the shell uses char's */
	/* vglobp is set to nc_uxb and the . command reads from vglobp */
	register NLchar *nc_up;
	short printub;
	char puxb[sizeof(uxb)+1];
	printub = 0;
	strcpy(puxb, uxb);
	c = ex_getchar();
	if (c == '\n' || c == EOF)
		error(msg(M_203, "Incomplete shell escape command@- use 'shell' to get a shell"));
	up = uxb;
	nc_up = nc_uxb;
	do {
		switch (c) {

		case '\\':
			if (any(peekchar(), "%#!"))
				c = ex_getchar();
		default:
			if (up+NCchrlen(c)-1 >= &uxb[UXBSIZE]) {
tunix:
				uxb[0] = 0;
				nc_uxb[0] = 0;
				error(msg(M_204, "Command too long"));
			}
			NLsputc(up,c);
			*nc_up++ = c;
			break;

		case '!':
			fp = puxb;
			if (*fp == 0) {
				uxb[0] = 0;
				nc_uxb[0] = 0;
				error(msg(M_205, "No previous command@to substitute for !"));
			}
			printub++;
			while (*fp) {
				if (up >= &uxb[UXBSIZE])
					goto tunix;
				*up++ = *fp++;
			}
			fp = puxb;
			while (*fp)
				*nc_up++ = NLsgetc(fp);
			break;

		case '#':
			xfp = fp = altfile;
			if (*fp == 0) {
				uxb[0] = 0;
				nc_uxb[0] = 0;
				error(msg(M_206, "No alternate filename@to substitute for #"));
			}
			goto uexp;

		case '%':
			xfp = fp = savedfile;
			if (*fp == 0) {
				uxb[0] = 0;
				nc_uxb[0] = 0;
				error(msg(M_207, "No filename@to substitute for %%"));
			}
uexp:
			printub++;
			while (*fp) {
				if (up >= &uxb[UXBSIZE])
					goto tunix;
				*up++ = *fp++;
			}
			while (*xfp)
				*nc_up++ = NLsgetc(xfp);
			break;
		}
		c = ex_getchar();
	} while (c == '"' || c == '|' || !endcmd(c));
	if (c == EOF)
		ungetchar(c);
	*up = 0;
	*nc_up = 0;
	if (!inopen)
		resetflav();
	if (warn)
		ckaw();
	if (warn && hush == 0 && chng && xchng != chng && value(WARN) && dol > zero) {
		xchng = chng;
		vnfl();
		ex_printf(mesg(msg(M_208, "[No write]|[No write since last change]")));
		noonl();
		flush();
	} else
		warn = 0;
	if (printub) {
		if (uxb[0] == 0)
			error(msg(M_209, "No previous command@to repeat"));
		if (inopen) {
			splitw++;
			vclean();
			vgoto(WECHO, 0);
		}
		if (warn)
			vnfl();
		if (hush == 0)
			lprintf("!%s", uxb);
		if (inopen && Outchar != termchar) {
			vclreol();
			vgoto(WECHO, 0);
		} else
			putnl();
		flush();
	}
}

/*
 * Do the real work for execution of a shell escape.
 * Mode is like the number passed to open system calls
 * and indicates filtering.  If input is implied, newstdin
 * must have been setup already.
 */
ttymode
unixex(opt, up, newstdin, mode)
	char *opt, *up;		/* opt is "-i" or "-c", up is exec'd */
	int newstdin, mode;
{
	int pvec[2];
	ttymode f;

	signal(SIGINT, SIG_IGN);
#ifdef SIGTSTP
	if (dosusp)
		signal(SIGTSTP, SIG_DFL);
#endif
	if (inopen)
		f = setty(normf);
	if ((mode & 1) && pipe(pvec) < 0) {
		/* Newstdin should be io so it will be closed */
		if (inopen)
			setty(f);
		error(msg(M_210, "Can't make pipe for filter"));
	}
#ifndef VFORK
	pid = fork();
#else
	pid = vfork();
#endif
	if (pid < 0) {
		if (mode & 1) {
			close(pvec[0]);
			close(pvec[1]);
		}
		setrupt();
		error(msg(M_211, "No more processes"));
	}
	if (pid == 0) {
		if (mode & 2) {
			close(0);
			dup(newstdin);
			close(newstdin);
		}
		if (mode & 1) {
			close(pvec[0]);
			close(1);
			dup(pvec[1]);
			if (inopen) {
				close(2);
				dup(1);
			}
			close(pvec[1]);
		}
		if (io)
			close(io);
		if (tfile)
			close(tfile);
		signal(SIGHUP, (void (*)(int))oldhup);
		signal(SIGQUIT, (void (*)(int))oldquit);
		if (ruptible)
			signal(SIGINT, SIG_DFL);
		execl(svalue(SHELL), "sh", opt, up, (char *) 0);
		ex_printf(msg(M_212, "No %s!\n"), svalue(SHELL));
		error((char *)0);
	}
	if (mode & 1) {
		io = pvec[0];
		close(pvec[1]);
	}
	if (newstdin)
		close(newstdin);
	return (f);
}

/*
 * Wait for the command to complete.
 * F is for restoration of tty mode if from open/visual.
 * C flags suppression of printing.
 */
unixwt(c, f)
	short c;
	ttymode f;
{

	waitfor();
#ifdef SIGTSTP
	if (dosusp)
		signal(SIGTSTP, (void (*)(int))onsusp);
#endif
	if (inopen)
		setty(f);
	setrupt();
	if (!inopen && c && hush == 0) {
		ex_printf("!\n");
		flush();
		termreset();
		gettmode();
	}
}

/*
 * Setup a pipeline for the filtration implied by mode
 * which is like a open number.  If input is required to
 * the filter, then a child editor is created to write it.
 * If output is catch it from io which is created by unixex.
 */
filter(mode)
	register int mode;
{
	static int pvec[2];
	ttymode f;	/* mjm: was register */
	register int nlines = lineDOL();
	int writer_pid = 0;

	mode++;
	if (mode & 2) {
		signal(SIGINT, SIG_IGN);
		if (pipe(pvec) < 0)
			error(msg(M_213, "Can't make pipe"));
		pid = fork();
		io = pvec[0];
		if (pid < 0) {
			setrupt();
			close(pvec[1]);
			error(msg(M_214, "No more processes"));
		}
		if (pid == 0) {
			setrupt();
			io = pvec[1];
			close(pvec[0]);
			putfile(1);
			exit(0);
		}
		writer_pid = pid;
		close(pvec[1]);
		io = pvec[0];
		setrupt();
	}
	f = unixex("-c", uxb, (mode & 2) ? pvec[0] : 0, mode);
	if (mode == 3) {
		delete(0);
		addr2 = addr1 - 1;
	}
	if (mode == 1)
		deletenone();
	if (mode & 1) {
		if(FIXUNDO)
			undap1 = undap2 = addr2+1;
		ignore(append(getfile, addr2));
#ifdef UNDOTRACE
		if (trace)
			vudump("after append in filter");
#endif
	}
	close(io);
	io = -1;
	unixwt(!inopen, f);
	if ((mode & 2) && writer_pid) {
		/*
		 * This is a hack to be certain that both children
		 * have been raped.  The above call may only have
		 * reaped the child of unixex(), missing the writer.
		 */
		pid = writer_pid;
		waitfor();
	}
	netchHAD(nlines);
}

/*
 * Set up to do a recover, getting io to be a pipe from
 * the recover process.
 */
recover()
{
	static int pvec[2];

	if (pipe(pvec) < 0)
		error(msg(M_215, " Can't make pipe for recovery"));
	pid = fork();
	io = pvec[0];
	if (pid < 0) {
		close(pvec[1]);
		error(msg(M_216, " Can't fork to execute recovery"));
	}
	if (pid == 0) {
		close(2);
		dup(1);
		close(1);
		dup(pvec[1]);
	        close(pvec[1]);
		execl(EXRECOVER, "exrecover", svalue(DIRECTORY), file, (char *) 0);
		close(1);
		dup(2);
		error(msg(M_217, " No recovery routine"));
	}
	close(pvec[1]);
}

/*
 * Wait for the process (pid an external) to complete.
 */
waitfor()
{

	do
		rpid = wait(&status);
	while (rpid != pid && rpid != -1);
	if ((status & 0377) == 0) {
		status = (status >> 8) & 0377;
	} else {
		ex_printf(msg(M_218, "%d: terminated with signal %d"), pid, status & 0177);
		if (status & 0200)
			ex_printf(msg(M_219, " -- core dumped"));
		ex_putchar('\n');
	}
}

/*
 * The end of a recover operation.  If the process
 * exits non-zero, force not edited; otherwise force
 * a write.
 */
revocer()
{

	waitfor();
	if (pid == rpid && status != 0)
		edited = 0;
	else
		change();
}
