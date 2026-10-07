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
static char rcsid[] = "@(#)$RCSfile: main.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:13 $";
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

#include        <sys/types.h>
#include        "defs.h"
#include        "sym.h"
#include        "timeout.h"
#include        <sys/stat.h>
#include        "dup.h"
#include	<fcntl.h>
#include	<sys/ioctl.h>	
#include	<sys/termios.h>	

#include	<locale.h>

#include        <sgtty.h>

static BOOL     beenhere = FALSE;
uchar_t            tmpout[26] = "/tmp/sh-";  /* changed from 20 to 26 - PTM 12638 */
struct fileblk  stdfile;
struct fileblk *standin = &stdfile;
int mailchk = 0;
int timeout = TIMEOUT;
int timecroak;
int FSHMSG = -1;

static uchar_t     *mailp;
static time_t     *mod_time = 0;
static int      exfile();
extern uchar_t     *simple();

#ifdef  MSG
nl_catd catd;           /* message catalog descriptor */
#endif


main(c, v, e)
int     c;
uchar_t    *v[];
uchar_t    *e[];
{
	register uchar_t	*shname;
	register int    rflag = ttyflg;
	int             rsflag = 1;     /* local restricted flag */
	struct namnod   *n;
	int	fd_tty;			/* fd of /dev/tty	*/

#ifndef _SHRLIB
	/*
	 * initialise storage allocation (must be done before setlocale!!!)
	 */
extern void setup_mem();

	setup_mem();

#endif /* NOT _SHRLIB */
	/*	during testing it was discovered that signal
	 *	handling had to be set before a call to setlocale
	 *	setlocale was making a call to the shell's malloc
	 *	which was causing a memory fault
	*/
	stdsigs();		/* set up signals */

	(void) setlocale (LC_ALL, "");

#ifdef MSG
        catd = NLcatopen(MF_SH, FSHMSG);
        if ( catd != NULL && catd->_fd != NULL)
            FSHMSG = fileno( catd->_fd);
#endif

	/*
	 * set names from userenv
	 */
	setup_env();

	/*
	 * 'rsflag' is non-zero if SHELL variable is
	 *  set in environment and has a simple name of
	 *  'Rsh', '-Rsh', 'rsh' or '-rsh'.
	 */
	if ((n = findnam("SHELL")) && n->namval != NULL)
	{
		if (*(shname = simple(n->namval)) == '-')
			++shname;
		if (   (strcmp(shname,"rsh") == 0)
		    || (strcmp(shname,"Rsh") == 0))
			rsflag = 0;
	}

	/*
	 * a shell is also restricted if argv(0) has
	 * a simple name of 'Rsh', '-Rsh', 'rsh' or '-rsh'.
	 */


	if (c > 0) {
		if (*(shname = simple(*v)) == '-')
			++shname;
		if (   (strcmp(shname,"rsh") == 0)
		    || (strcmp(shname,"Rsh") == 0))
			rflag = 0;
	}


	hcreate();
	set_dotpath();

	/*
	 * look for options
	 * dolc is $#
	 */
	eflag = 0;	/* fail on error flag */
	dolc = options(c, v);

	if (dolc < 2)
	{
		flags |= stdflg;
		{
			register uchar_t *flagc = flagadr;

			while (*flagc)
				flagc++;
			*flagc++ = STDFLG;
			*flagc = 0;
		}
	}
	if ((flags & stdflg) == 0)
		dolc--;
	dolv = v + c - dolc;
	dolc--;

	/*
	 * return here for shell file execution
	 * but not for parenthesis subshells
	 */
	setjmp(subshell);

	/*
	 * number of positional parameters
	 */
	replace(&cmdadr, dolv[0]);      /* cmdadr is $0 */

	/*
	 * set pidname '$$'
	 */
	assnum(&pidadr, getpid());

	/*
	 * set up temp file names
	 */
	settmp();

	/*
	 * default internal field separators - $IFS
	 */
	dfault(&ifsnod, sptbnl);

	dfault(&mchknod, MAILCHECK);
	mailchk = stoi(mchknod.namval);
	if(timenod.namval)
		timeout = stoi(timenod.namval);

	/*
	 * catch longjmps that are not caught elsewhere
	 */
	if (setjmp(errshell))
	{
		exit(ERROR);
	}

	if ((beenhere++) == FALSE)      /* ? profile */
	{
		if (*(simple(cmdadr)) == '-')
		{                       /* system profile */

			if ((input = pathopen(nullstr, sysprofile)) >= 0)
				exfile(rflag);          /* file exists */


			if ((input = pathopen(nullstr, profile)) >= 0)
			{
				exfile(rflag);
				flags &= ~ttyflg;
			}
		}
		if (rsflag == 0 || rflag == 0)
			flags |= rshflg;
		/*
		 * open input file if specified
		 */
		if (comdiv)
		{
			estabf(comdiv);
			input = -1;
		}
		else
		{
				/*	PTM 26229
				*	set cmdadr to argv[0] so if
				*	chkopen fails it will print
				*	"sh: file: not found"
				*/
			uchar_t	*oldcmd = cmdadr;
			cmdadr = v[0];
			input = ((flags & stdflg) ? 0 : chkopen(oldcmd));

#ifdef ACCT
			if (input != 0)
				preacct(oldcmd);
#endif
			cmdadr = oldcmd;
			comdiv--;
		}
	}


	exfile(0);
	done();
}

long    mailtime;       /* last time mail was checked */

static int
exfile(prof)
BOOL    prof;
{
	jmp_buf savebuf;
	time_t    curtime = 0;
	register int    userid;

	/*
	 * preserve the previous longjmp context
	 */
	memcpy(savebuf,errshell,sizeof(savebuf));
	/*
	 * move input
	 */
	if (input > 0)
	{
		Ldup(input, INIO);
		input = INIO;
	}

	userid = geteuid();

	/*
	 * decide whether interactive
	 */
	if ((flags & intflg) ||
	    ((flags&oneflg) == 0 &&
	    isatty(output) &&
	    isatty(input)) )

	{
		uchar_t    *stdprompt, *supprompt;
		
		stdprompt = shstdprompt;
		supprompt = shsupprompt;
		
		dfault(&ps1nod, (userid ? stdprompt : supprompt));
		dfault(&ps2nod, readmsg);
		flags |= ttyflg | prompt;
		ignsig(SIGTERM);
		if (mailpnod.namflg != N_DEFAULT)
			setmail(mailpnod.namval);
		else
			setmail(mailnod.namval);
	}
	else
	{
		flags |= prof;
		flags &= ~prompt;
	}

	if (setjmp(errshell) && prof)
	{
		memcpy(errshell,savebuf,sizeof(errshell));
		close(input);
		return;
	}
	/*
	 * error return here
	 */

	loopcnt = peekc = peekn = 0;
	fshift = 0;
	fndef = 0;
	nohash = 0;
	iopend = 0;

	if (input >= 0)
		initf(input);
	/*
	 * command loop
	 */
	for (;;)
	{
		tdystak(0);
		stakchk();      /* may reduce sbrk */
		exitset();

		if ((flags & prompt) && standin->fstak == 0 && !eof)
		{

			timecroak = 0;
			if (mailp || timeout)
			{
				time(&curtime);
				if(timeout)
					timecroak = curtime + 60*timeout;
				if (mailp && (curtime - mailtime) >= mailchk)
				{
					chkmail();
					mailtime = curtime;
				}
			}

			prs(ps1nod.namval);
			if(timeout || mailp && mailchk)
				alarm(mailchk < 60 * timeout ? mailchk
							     : 60 * timeout);
			flags |= waiting;
		}

		trapnote = 0;
		peekc = readc();
		if (eof)
			break;

		alarm(0);

		flags &= ~waiting;

		execute(cmd(NL, MTFLG), 0, eflag);
		eof |= (flags & oneflg);
	}
	/*
	 * restore the previous longjmp context
	 */
	memcpy(errshell,savebuf,sizeof(errshell));
}

chkpr()
{
	if ((flags & prompt) && standin->fstak == 0)
		prs(ps2nod.namval);
}

settmp()
{
	serial = 0;
			/*	PTM 10017 - changed manner in which tmp file
			*	name is generated - old method used pid;
			*	changed to call mktemp - shell will add
			*	uniqueue number at end of tmp name
			*/
	strcpy ( numbuf , "XXXXXX" );	/*  init numbuf for call to mktemp  */
	mktemp ( numbuf );
	tempname = movstr(numbuf, &tmpout[TMPNAM]);
}

Ldup(fa, fb)
register int    fa, fb;
{

#ifdef	i860
	dup2(fa, fb);
#else
	dup(fa | DUPFLG, fb);
#endif
	close(fa);
	ioctl(fb, FIOCLEX, 0);
}


chkmail()
{
	register uchar_t   *s = mailp;
	register uchar_t   *save;

	time_t    *ptr = mod_time;
	uchar_t    *start;
	BOOL    flg;
	struct stat     statb;

	while (s && *s)
	{
		start = s;
		save = 0;
		flg = 0;

		while (*s)
		{
			if (*s != COLON)
			{
				if (*s == '%' && save == 0)
					save = s;

				s++;
			}
			else
			{
				flg = 1;
				*s = 0;
			}
		}

		if (save)
			*save = 0;

		if (*start && stat((char *)start, &statb) >= 0)
		{
			if(statb.st_size && *ptr
				&& statb.st_mtime != *ptr)
			{
				if (save)
				{
					prs(save+1);
					newline();
				}
				else
				{
					if (mailmnod.namflg != N_DEFAULT)
					{
						prs(mailmnod.namval);
						newline();
					} else
						prs(MSGSTR(M_MAILMSG,(char *)mailmsg));
				}
			}
			*ptr = statb.st_mtime;
		}
		else if (*ptr == 0)
			*ptr = 1;

		if (save)
			*save = '%';

		if (flg)
			*s++ = COLON;

		ptr++;
	}
}


setmail(mailpath)
	uchar_t *mailpath;
{
	register uchar_t   *s = mailpath;
	register int    cnt = 1;

	time_t    *ptr;

	free((void *)mod_time);
	if (mailp = mailpath)
	{
		while (*s)
		{
			if (*s == COLON)
				cnt += 1;

			s++;
		}

		ptr = mod_time = (time_t *)alloc(sizeof(long) * cnt);

		while (cnt)
		{
			*ptr = 0;
			ptr++;
			cnt--;
		}
	}
}
