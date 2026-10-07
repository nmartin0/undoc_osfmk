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
static char rcsid[] = "@(#)$RCSfile: service.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:19 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

#include	"defs.h"
#include	<errno.h>
#ifdef KJI
#include	"shctype.h"
#endif
#include	<sys/mode.h>

#define ARGMK	01

extern	short	topfd;
extern	int	trap_waitrc;	/* PTM 5870 trap of SIGCHLD */
extern	int	trap_status;	/* PTM 5870 trap of SIGCHLD */

char *SGnames[NSIG] = {
        "Signal 0",             "Hangup",               "Interrupt",
        "Quit",                 "Illegal instruction",  "Breakpoint",
        "Abort",                "EMT instruction",      "Floating exception",
        "Killed",               "Bus error",            "Memory fault",
        "Bad system call",      "Broken pipe",          "Alarm call",
        "Terminated",           "Urgent I/O",           "Stopped",
        "Suspended",            "Continue",
        "Child death",          "Background read",      "Background write",
        "I/O completed",        "CPU time exceeded",    "File size exceeded",
        "Virtual time alarm",   "Profiling alarm",      "Window size changed",
        "Information request",  "User signal 1",        "User signal 2"
};


static	uchar_t	*execs ();
static	int	gsort ();
static	int	split ();

/*
 * service routines for `execute'
 */
initio(iop, save)
	struct ionod	*iop;
	int		save;
{
	register uchar_t	*ion;
	register int	iof, fd;
	int		ioufd;
	short	lastfd;

	lastfd = topfd;
	while (iop)
	{
		iof = iop->iofile;
		ion = macro(iop->ioname);
		NLSdecode(ion);
# ifdef NLSDEBUG
		debug("ioinit",ion);
# endif
		ioufd = iof & IOUFD;

		if (*ion && (flags&noexec) == 0)
		{
			if (save)
			{
				fdmap[topfd].org_fd = ioufd;
				fdmap[topfd++].dup_fd = savefd(ioufd);
			}

			if (iof & IODOC)
			{
				struct tempblk tb;

				subst(chkopen(ion), (fd = tmpfil(&tb)));

				poptemp();	/* pushed in tmpfil() --
						   bug fix for problem with
						   in-line scripts
						*/

				fd = chkopen(tmpout);
				unlink(tmpout);
			}
			else if (iof & IOMOV)
			{
				if (eq(minus, ion))
				{
					fd = -1;
					close(ioufd);
				}
				else if ((fd = stoi(ion)) >= USERIO)
					failed(ion, MSGSTR(M_BADFILE,(char *)badfile));
				else
					fd = dup(fd);
			}
			else if ((iof & IOPUT) == 0)
				fd = chkopen(ion);
			else if (flags & rshflg)
				failed(ion, MSGSTR(M_RESTRICTED,(char *)restricted));
			else if (iof & IOAPP && (fd = open(ion, 1)) >= 0)
				lseek(fd, 0L, 2);
			else
#ifdef NLSDEBUG
				{ debug("create",ion); fd=create(ion); }
#else
				fd = create(ion);
#endif
			if (fd >= 0)
				sh_rename(fd, ioufd);
		}

		iop = iop->ionxt;
	}
	return(lastfd);
}

uchar_t *
simple(s)
uchar_t	*s;
{
	uchar_t	*sname;

	sname = s;
	while (1)
	{
		if (any('/', sname))
			while (*sname++ != '/')
				;
		else
			return(sname);
	}
}

uchar_t *
getpath(s)
	uchar_t	*s;
{
	
	register uchar_t	*path;
# ifdef KJI
	/* no quoting here */
	if (any('/', s))
# else
	if (NLany('/', s))
# endif
	{
		if (flags & rshflg)
			failed(s, MSGSTR(M_RESTRICTED,(char *)restricted));
		else
			return(nullstr);
	}
	else if ((path = pathnod.namval) == 0) {

		return ( getuid() ? shdefpath : sudefpath);
	}
	else
		return(cpystak(path));
}

pathopen(path, name)
register uchar_t *path, *name;
{
	register int    f;

	/* works for both encoded and decoded strings */
	if ( NLSisencoded(name) )
		name = NLSndecode(name);
	do
	{
		path = catpath(path, name);
	} while ((f = open(curstak(), 0)) < 0 && path);
	return(f);
}

uchar_t *
catpath(path, name)
register uchar_t	*path;
uchar_t	*name;
{
	/*
	 * leaves result on top of stack
	 * works for both encoded and decoded strings
	 */
	register uchar_t	*scanp = path;
	register uchar_t	*argp = locstak();

	while (*scanp && *scanp != COLON) {
		needmem(argp);
		*argp++ = *scanp++;
	}
	if (scanp != path) {
		needmem(argp);
		*argp++ = '/';
	}
	if (*scanp == COLON)
		scanp++;
	path = (*scanp ? scanp : 0);
	scanp = name;
	NLSskiphdr(scanp);
	do
		needmem(argp);
	while (*argp++ = *scanp++);
	return(path);
}

uchar_t *
nextpath(path)
	register uchar_t	*path;
{
	/* works for both encoded and decoded strings */
	register uchar_t	*scanp = path;

	while (*scanp && *scanp != COLON)
		scanp++;

	if (*scanp == COLON)
		scanp++;

	return(*scanp ? scanp : 0);
}

static uchar_t	*xecmsg;
static uchar_t	**xecenv;

int	execa(at, pos)
	uchar_t	*at[];
	short pos;
{
	register uchar_t	*path;
	register uchar_t	**t = at;
	int		cnt;

	if ((flags & noexec) == 0)
	{
		xecmsg = notfound;
		path = getpath(*t);
		xecenv = setenv();
		NLSdecodeargs(t);

		if (pos > 0)
		{
			cnt = 1;
			while (cnt != pos)
			{
				++cnt;
				path = nextpath(path);
			}
			execs(path, t);
			path = getpath(*t);
		}
		while (path = execs(path,t))
			;
		failed(*t, MSGSTR(((xecmsg==badexec)? M_BADEXEC : M_NOTFOUND),
			(char *)xecmsg));
	}
}

static uchar_t *
execs(ap, t)
uchar_t	*ap;
register uchar_t	*t[];
{
	
	register uchar_t *p, *prefix;
	
	prefix = catpath(ap, t[0]);
	p = curstak();
# ifdef NLSDEBUG
	debug("execs",p);
# endif
	sigchk();


	execve(p, &t[0] ,xecenv);
	switch (errno)
	{
	case ENOEXEC:		/* could be a shell script */
		funcnt = 0;
		flags = 0;
		*flagadr = 0;
		comdiv = 0;
		ioset = 0;
		clearup();	/* remove open files and for loop junk */
		if (input)
			close(input);
		input = chkopen(p);
	
#ifdef ACCT
		preacct(p);	/* reset accounting */
#endif

		/*
		 * set up new args
		 */

		setargs(t);
		longjmp(subshell, 1);

	case ENOMEM:
		failed(p, MSGSTR(M_NOSPACE,(char *)toobig));

	case E2BIG:
		failed(p, MSGSTR(M_ARGLIST,(char *)arglist));

	case ETXTBSY:
		failed(p, MSGSTR(M_TXTBSY,(char *)txtbsy));

	default:
		xecmsg = badexec;
	case ENOENT:
		return(prefix);
	}
}


/*
 * for processes to be waited for
 */
#define MAXP 20
static int	pwlist[MAXP];
static int	pwc;

postclr()
{
	register int	*pw = pwlist;

	while (pw <= &pwlist[pwc])
		*pw++ = 0;
	pwc = 0;
}

post(pcsid)
int	pcsid;
{
	register int	*pw = pwlist;

	if (pcsid)
	{
		while (*pw)
			pw++;
		if (pwc >= MAXP - 1)
			pw--;
		else
			pwc++;
		*pw = pcsid;
	}
}

/*
 * PECULIAR BEHAVIOR of await()
 *
 *   There are three ways this routine is called :
 *      (1) built-in command "wait" is invoked.
 *      (2) When a NON built-in command is invoked, the shell
 *          wait()s for the completion of that process if it
 *          was not a background process (not "&").
 *      (3) command-susbstitution (backquote) is invoked.
 *
 *   This routine will invoke wait() at least once !!!!
 *
 *   SH caveats (historical features) dealing with "await()" :
 *      (A) Background processes are NOT explicitly waited-for.
 *          Processes that begin as background processes ( "&" )
 *          are waited-for IMPLICITLY when this routine is called
 *          on behalf of another task {see (1),(2),(3) above}.
 *          Thus, invoking a large number of background processes
 *          without invoking a foreground process will deplete
 *          process slots.
 *      (B) If the built-in command "wait" is invoked with
 *          a bad "pid" (non-child), this routine returns ONLY
 *          after ALL child processes have been waited-for.
 *          WARNING : if the decision is taken to use the new
 *                    system call "waitpid( ..., pid, ... )",
 *                    the traditional sh behavior will be lost.
 *
 *
 * DESCRIPTION for await() :
 *
 *     if ( i ==  0 )
 *         wait() for ALL processes in "pwlist"
 *     else ( i == -1 )
 *         wait() for ALL child processes
 *     else
 *         wait() for process "i"
 *
 *     NOTE : - implicit wait()s MUST be done on behalf of
 *              other child processes.
 *
 */

await(i, bckg)
int     bckg, /* user invoked built-in "wait" command */
        i;    /* pid to wait-for */
{
        int  rc = 0,
             wx = 0,
             w,
             ipwc = pwc;

        post(i);
        while (pwc)
        {
                register int    p;
                register int    sig;
                int             w_hi,
                                waitErrno,
                                found = 0;

                if ( (p = wait(&w)) == -1 )
                        waitErrno = errno;

                /*      PTM 5870 - trap of SIGCHLD
                 *      wait has been performed in fault
                 */
                if ( trapcom [SIGCHLD] && strlen (trapcom [SIGCHLD]))
                {
                        p = trap_waitrc;
                        w = trap_status;
                }

                if (wasintr)
                {
                        wasintr = 0;
                        if (bckg)
                                break;
                }

                if (p == -1)
                {
                        if ( waitErrno == ECHILD )
                        {
                                postclr();
                                break;
                        }
                        else /* waitErrno == EINTR */
                                continue;
                }
                else
                {
                        register int    *pw = pwlist;
                        while (pw <= &pwlist[ipwc])
                                if (*pw == p)
                                {
                                        *pw = 0;
                                        pwc--;
                                        found++;
                                }
                                else
                                        pw++;
                }

                w_hi = (w >> 8) & LOBYTE;
                if (sig = w & 0177)
                {
                        if (sig == 0177)        /* ptrace! return */
                        {
                                prs("ptrace: ");
                                sig = w_hi;
                        }
                        if (sig!=SIGINT && sig!=SIGPIPE )
                        {
                                if (i != p || (flags & prompt) == 0)
                                {
                                        prp();
                                        prn(p);
                                        blank();
                                }
                                prs(SGnames[sig]);
                                if (w & 0200)
                                        prs(MSGSTR(M_COREDUMP,(char *)coredump));
                        }
                        newline();
                }
                if (rc == 0 && found != 0)
                        rc = (sig ? sig | SIGFLG : w_hi);
                wx |= w;
                if (p == i)
                        break;
        }
        if (wx && flags & errflg)
                exitsh(rc);
        flags |= eflag;
        exitval = rc;
        exitset();
}


BOOL		nosubst;

trim(at)
uchar_t	*at;
{
	register uchar_t	*p;
	register uchar_t 	*ptr;
	register uchar_t   c;
	register uchar_t	q = 0;

	if (p = at)
	{

		ptr = p;
		while (c = *p++)
		{
			/* the nosubst hack is used in input redirection */
			/* to disable shell variable substitution in     */
			/* inline input, e.g. cat <<"END"                */
			register uchar_t c1;
			if (c1 = c & STRIP) {
			     *ptr++ = c1;
			     if (NLSfontshift(c1))
# ifdef KJI
			    {
# endif
				/* don't strip uchar_t after font shift */
				if (*p) *ptr++ = *p++;
# ifdef KJI
				if (c1 != FSH0) 
				    *ptr++ = *p++;
			}
# endif
			}
			q |= c;
		}

		*ptr = 0;
	}
	nosubst = q & QUOTE;
}

uchar_t *
mactrim(s)
uchar_t	*s;
{
	register uchar_t	*t = macro(s);

	trim(t);
	return(t);
}

uchar_t **
scan(argn)
int	argn;
{
	register struct argnod *argp = (struct argnod *)(Rcheat(gchain) & ~ARGMK);
	register uchar_t **comargn, **comargm;

	comargn = (uchar_t **)getstak(BYTESPERWORD * argn + BYTESPERWORD);
	comargm = comargn += argn;
	*comargn = ENDARGS;
	while (argp)
	{
		*--comargn = argp->argval;

		trim(*comargn);
		argp = argp->argnxt;

		if (argp == 0 || Rcheat(argp) & ARGMK)
		{
			gsort(comargn, comargm);
			comargm = comargn;
		}
		/* Lcheat(argp) &= ~ARGMK; */
		argp = (struct argnod *)(Rcheat(argp) & ~ARGMK);
	}
	return(comargn);
}

#if defined(NLS) || defined(KJI)

int 
comp_enc(s, t) 
uchar_t *s, *t; 
{
  	uchar_t *sdec = malloc(strlen(s));
    	uchar_t *tdec = malloc(strlen(t));
    	int r;

    	NLSdecode1(s, sdec);
    	NLSdecode1(t, tdec);
    	r = NLstrcmp(sdec, tdec);
    	free(sdec);
    	free(tdec);
   	return (r);
}

#endif

static int
gsort(from, to)
uchar_t	*from[], *to[];
{
	ptrdiff_t	k, m, n;
	register ptrdiff_t	i, j;

	if ((n = to - from) <= 1)
		return;
	for (j = 1; j <= n; j *= 2)
		;
	for (m = 2 * j - 1; m /= 2; )
	{
		k = n - m;
		for (j = 0; j < k; j++)
		{
			for (i = j; i >= 0; i -= m)
			{
				register uchar_t **fromi;

				fromi = &from[i];
#if defined(NLS) || defined(KJI)
				if (comp_enc(fromi[m], fromi[0]) > 0)
#else
				if (cf(fromi[m], fromi[0]) > 0)
#endif
				{
					break;
				}
				else
				{
					uchar_t *s;

					s = fromi[m];
					fromi[m] = fromi[0];
					fromi[0] = s;
				}
			}
		}
	}
}

/*
 * Argument list generation
 */
getarg(ac)
struct comnod	*ac;
{
	register struct argnod	*argp;
	register int		count = 0;
	register struct comnod	*c;

	if (c = ac)
	{
		argp = c->comarg;
		while (argp)
		{
			count += split(macro(argp->argval));
			argp = argp->argnxt;
		}
	}
	return(count);
}

static int
split(s)		/* blank interpretation routine */
register uchar_t	*s;
{
	register uchar_t	*argp;
	register int	c;
	int		count = 0;
#ifdef KJI	
	uchar_t 		ifsch[100], *ifs = ifsch;
#endif

#if defined(NLS) || defined(KJI)
	NLSskiphdr(s);
	/* ignore encoding string on whole argument */
#endif
#ifdef KJI
	if (!NLSisencoded(ifsnod.namval)) {
		NLSencode (ifsnod.namval, ifs, 100);
		NLSskiphdr(ifs);
	} else
		ifs = ifsnod.namval;	
#endif
	for (;;)
	{
		sigchk();
		argp = locstak() + BYTESPERWORD;
#if defined(NLS) || defined(KJI)
		/* insert encoding string on each argument */
		needmem(argp);
		*argp++ = FNLS;
#endif
#ifdef KJI
		for ( ;; ) 
		{
			c = *s;
			if (!NLany(s, ifs) && c)
			{
				int j;
				j = NLSenclen (c & STRIP);
				while (j--)
				{
					needmem (argp);
					*argp++ = *s++;
				}
			} 
			else 
			{
				s += NLSenclen (c & STRIP);
				break;
			}
		}
#else
		while ((c = *s++, !any(c, ifsnod.namval) && c))
		{
			needmem (argp);
			*argp++ = c;
		}
#endif
#if defined(NLS) || defined(KJI)
		/* if argument is null remove it from list */
		if (argp == staktop + BYTESPERWORD + 1)
#else
		if (argp == staktop + BYTESPERWORD)
#endif
		{
			if (c)
			{
				continue;
			}
			else
			{
				return(count);
			}
		}
		else if (c == 0)
			s--;
		/*
		 * file name generation
		 */

		argp = endstak(argp);

		if ((flags & nofngflg) == 0 && 
			(c = expand(((struct argnod *)argp)->argval, 0)))
			count += c;
		else
		{
			makearg(argp);
			count++;
		}
		gchain = (struct argnod *)(Rcheat(gchain) | ARGMK);
	}
}

#ifdef ACCT
#include	<sys/types.h>
#include	"acctdef.h"
#include	<sys/acct.h>
#include 	<sys/times.h>

struct acct sabuf;
struct tms buffer;
extern clock_t times();
static long before;
static int shaccton;	/* 0 implies do not write record on exit
			   1 implies write acct record on exit
			*/


/*
 *	suspend accounting until turned on by preacct()
 */

suspacct()
{
	shaccton = 0;
}

preacct(cmdadr)
	uchar_t *cmdadr;
{
	uchar_t *simple();

	if (acctnod.namval && *acctnod.namval)
	{
		sabuf.ac_btime = time((long *)0);
		before = times(&buffer);
		sabuf.ac_uid = getuid();
		sabuf.ac_gid = getgid();
		movstrn(simple(cmdadr), sabuf.ac_comm, sizeof(sabuf.ac_comm));
		shaccton = 1;
	}
}

#include	<fcntl.h>

doacct()
{
	int fd;
	long int after;

	if (shaccton)
	{
		after = times(&buffer);
		sabuf.ac_utime = compress(buffer.tms_utime + buffer.tms_cutime);
		sabuf.ac_stime = compress(buffer.tms_stime + buffer.tms_cstime);
		sabuf.ac_etime = compress(after - before);

		if ((fd = open(acctnod.namval, O_WRONLY | O_APPEND | O_CREAT, 0666)) != -1)
		{
			write(fd, &sabuf, sizeof(sabuf));
			close(fd);
		}
	}
}

/*
 *	Produce a pseudo-floating point representation
 *	with 3 bits base-8 exponent, 13 bits fraction
 */

compress(t)
	register time_t t;
{
	register exp = 0;
	register rund = 0;

	while (t >= 8192)
	{
		exp++;
		rund = t & 04;
		t >>= 3;
	}

	if (rund)
	{
		t++;
		if (t >= 8192)
		{
			t >>= 3;
			exp++;
		}
	}

	return((exp << 13) + t);
}
#endif
