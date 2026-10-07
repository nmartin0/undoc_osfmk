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
static char rcsid[] = "@(#)$RCSfile: edit.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:14 $";
#endif
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
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
/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *  edit.c - common routines for vi and emacs one line editors in shell
 *
 *   David Korn				P.D. Sullivan
 *   AT&T Bell Laboratories		AT&T Bell Laboratories
 *   Room 3C-526B			Room 1D-245
 *   Murray Hill, N. J. 07974		Columbus, OH 43213
 *   Tel. x7975				Tel. x 2655
 *
 *   Coded April 1983.
 */

#include	<errno.h>
#include	<sys/ioctl.h>
#include	<sys/ttydev.h>

#ifdef KSHELL
#   include	"defs.h"
#   include	"terminal.h"
#   include	"builtins.h"
#   include	"sym.h"
#else
#   include	"io.h"
#   include	"terminal.h"
#   undef SIG_NORESTART
#   define SIG_NORESTART	1
#   define _sobuf	ed_errbuf
    extern char ed_errbuf[];
#ifdef  KSH_88D
    char e_version[] = "\n@(#)Editlib version 11/16/88d\0\n";
#else
    char e_version[] = "\n@(#)Editlib version 11/16/88\0\n";
#endif /* KSH_88D */
#endif	/* KSHELL */
#include	"history.h"
#include	"edit.h"

#define BAD	-1
#define GOOD	0
#define	SYSERR	-1

#ifdef  KSH_88D
#ifdef OLDTERMIO
#   undef tcgetattr
#   undef tcsetattr
#endif /* OLDTERMIO */
#endif /* KSH_88D */

#define lookahead	editb.e_index
#define env		editb.e_env
#define previous	editb.e_lbuf
#define fildes		editb.e_fd
#ifdef  KSH_88D
#define in_raw          editb.e_addnl
#endif /* KSH_88D */


#ifdef _sgtty_
#   ifdef TIOCGETP
	static int l_mask;
	static struct tchars l_ttychars;
	static struct ltchars l_chars;
	static  char  l_changed;	/* set if mode bits changed */
#	define L_CHARS	4
#	define T_CHARS	2
#	define L_MASK	1
#   endif /* TIOCGETP */
#endif /* _sgtty_ */

#ifdef  KSH_88D
#ifdef  IODELAY
#       undef   _SELECT5_
#endif /* IODELAY */
#ifdef  _SELECT5_
#   ifndef included_sys_time_
#       include <sys/time.h>
#   endif /* included_sys_time_ */
        static int delay;
#   ifndef KSHELL
            int tty_speeds[] = {0, 50, 75, 110, 134, 150, 200, 300,
                600,1200,1800,2400,9600,19200,0};
#   endif       /* KSHELL */
#endif /* _SELECT5_ */
#else
#ifdef _SELECT_
#   ifndef included_sys_time_
#	include	<sys/time.h>
#   endif /* included_sys_time_ */
	static int delay;
#   ifndef KSHELL
    	    int tty_speeds[] = {0, 50, 75, 110, 134, 150, 200, 300,
		600,1200,1800,2400,9600,19200,0};
#   endif	/* KSHELL */
#endif /* _SELECT_ */
#endif /* KSH_88D */

#ifdef KSHELL
    extern int		f_complete();
    extern char		*sh_tilde();
#if (defined(NLS) || defined(KJI))
	static char macro[5]	= {'_','?','?',0,0};
#else
    static char macro[]	= "_??";
#endif
#   define slowsig()	(sh.trapnote&SIGSLOW)
#else
    struct edit editb;
    extern int errno;
#   define slowsig()	(0)
#endif	/* KSHELL */


static struct termios savetty;
static int savefd = -1;

#ifdef	KSH_88
static int compare();
#endif

#if (defined(NLS) || defined(KJI))
extern int is_print();			/* in vi.c */
#endif

#if VSH || ESH
    static struct termios ttyparm;	/* initial tty parameters */
    static struct termios nttyparm;	/* raw tty parameters */
    static char bellchr[] = "\7";	/* bell char */
#   define tenex 1
#   ifdef tenex
	static char *overlay();
#   endif /* tenex */
#endif /* VSH || ESH */


/*
 * This routine returns true if fd refers to a terminal
 * This should be equivalent to isatty
 */

int tty_check(int fd)
{
			/*	PTM 12563 and 13362
			*	replaced call to tty_get with call to
			*	isatty because tty_get did not work
			*	until proper line disciplines were set
			*
			*	return(tty_get(fd,(struct termios*)0)==0);
			*/
	return (isatty(fd));
}

/*
 * Get the current terminal attributes
 * This routine remembers the attributes and just returns them if it
 *   is called again without an intervening tty_set()
 */

int tty_get(int fd, struct termios *tty)
{
	if(fd != savefd)
	{
#ifndef SIG_NORESTART
		VOID (*savint)() = st.intfn;
		st.intfn = 0;
#endif	/* SIG_NORESTART */
		while(tcgetattr(fd,&savetty) == SYSERR)
		{
			if(errno !=EINTR)
			{
#ifndef SIG_NORESTART
				st.intfn = savint;
#endif	/* SIG_NORESTART */
				return(SYSERR);
			}
			errno = 0;
		}
#ifndef SIG_NORESTART
		st.intfn = savint;
#endif	/* SIG_NORESTART */
		savefd = fd;
	}
	if(tty)
		*tty = savetty;
	return(0);
}

/*
 * Set the terminal attributes
 * If fd<0, then current attributes are invalidated
 */

/* VARARGS 2 */
int tty_set(fd, action, tty)
struct termios *tty;
{
	if(fd >=0)
	{
#ifndef SIG_NORESTART
		VOID (*savint)() = st.intfn;
#endif	/* SIG_NORESTART */
#ifdef future
		if(savefd>=0 && compare(&savetty,tty,sizeof(struct termios)))
			return(0);
#endif
#ifndef SIG_NORESTART
		st.intfn = 0;
#endif	/* SIG_NORESTART */
		while(tcsetattr(fd, action, tty) == SYSERR)
		{
			if(errno !=EINTR)
			{
#ifndef SIG_NORESTART
				st.intfn = savint;
#endif	/* SIG_NORESTART */
				return(SYSERR);
			}
			errno = 0;
		}
#ifndef SIG_NORESTART
		st.intfn = savint;
#endif	/* SIG_NORESTART */
		savetty = *tty;
	}
	savefd = fd;
	return(0);
}

#if ESH || VSH
/*{	TTY_COOKED( fd )
 *
 *	This routine will set the tty in cooked mode.
 *	It is also called by error.done().
 *
}*/

void tty_cooked(int fd)
{
#ifdef  KSH_88D
        if(editb.e_raw==0)
                return;
        if (fd < 0)
                fd = savefd;
#else
	/*** don't do tty_set unless ttyparm has valid data ***/
	/* or in raw mode */

	if(savefd<0 || editb.e_raw==0)
		return;

#ifdef TIOCGETC
	if(editb.e_raw!=RAWMODE)
		return;
#endif /* TIOCGETC */
	if(tty_set(fd, TCSANOW, &ttyparm) == SYSERR )
		return;
#endif /* KSH_88D */
#ifdef L_MASK
	/* restore flags */
	if(l_changed&L_MASK)
		ioctl(fd,TIOCLSET,&l_mask);
	if(l_changed&T_CHARS)
		/* restore alternate break character */
		ioctl(fd,TIOCSETC,&l_ttychars);
	if(l_changed&L_CHARS)
		/* restore alternate break character */
		ioctl(fd,TIOCSLTC,&l_chars);
	l_changed = 0;
#endif	/* L_MASK */
#ifdef  KSH_88D
        /*** don't do tty_set unless ttyparm has valid data ***/
        if(savefd<0 || tty_set(fd, TCSANOW, &ttyparm) == SYSERR)
                return;
#endif /* KSH_88D */
	editb.e_raw = 0;
	return;
}

/*{	TTY_RAW( fd )
 *
 *	This routine will set the tty in raw mode.
 *
}*/

tty_raw(int fd)
{
#ifdef L_MASK
	struct ltchars lchars;
#endif	/* L_MASK */
	if(editb.e_raw==RAWMODE)
		return(GOOD);
#ifndef RAWONLY
	if(editb.e_raw != ALTMODE)
#endif /* RAWONLY */
	{
		if(tty_get(fd,&ttyparm) == SYSERR)
			return(BAD);
	}
#if  L_MASK || VENIX
	if(!(ttyparm.sg_flags&ECHO) || (ttyparm.sg_flags&LCASE))
		return(BAD);
	nttyparm = ttyparm;
	nttyparm.sg_flags &= ~(ECHO | TBDELAY);
#   ifdef CBREAK
	nttyparm.sg_flags |= CBREAK;
#   else
	nttyparm.sg_flags |= RAW;
#   endif /* CBREAK */
	editb.e_erase = ttyparm.sg_erase;
	editb.e_kill = ttyparm.sg_kill;
	editb.e_eof = cntl('D');
	if( tty_set(fd, TCSADRAIN, &nttyparm) == SYSERR )
		return(BAD);
	editb.e_ttyspeed = (ttyparm.sg_ospeed>=B1200?FAST:SLOW);
#ifdef  KSH_88D
#   ifdef _SELECT5_
            delay = tty_speeds[ttyparm.sg_ospeed];
#   endif /* _SELECT5_ */
#else
#   ifdef _SELECT_
	    delay = tty_speeds[ttyparm.sg_ospeed];
#   endif /* _SELECT_ */
#endif /* KSH_88D */
#   ifdef TIOCGLTC
	/* try to remove effect of ^V  and ^Y and ^O */
	if(ioctl(fd,TIOCGLTC,&l_chars) != SYSERR)
	{
		lchars = l_chars;
		lchars.t_lnextc = -1;
		lchars.t_flushc = -1;
		lchars.t_dsuspc = -1;	/* no delayed stop process signal */
		if(ioctl(fd,TIOCSLTC,&lchars) != SYSERR)
			l_changed |= L_CHARS;
	}
#   endif	/* TIOCGLTC */
#else

	if (!(ttyparm.c_lflag & ECHO ))
		return(BAD);

	nttyparm = ttyparm;
#  ifndef u370
	nttyparm.c_iflag &= ~(IGNPAR|PARMRK|INLCR|IGNCR|ICRNL);
	nttyparm.c_iflag |= BRKINT;
	nttyparm.c_lflag &= ~(ICANON|ECHO);
#   else
	nttyparm.c_iflag &= 
			~(IGNBRK|PARMRK|INLCR|IGNCR|ICRNL|INPCK);
	nttyparm.c_iflag |= (BRKINT|IGNPAR);
	nttyparm.c_lflag &= ~(ICANON|ECHO);
#   endif	/* u370 */
	nttyparm.c_cc[VTIME] = 0;
	nttyparm.c_cc[VMIN] = 1;
	editb.e_eof = ttyparm.c_cc[VEOF];
	editb.e_erase = ttyparm.c_cc[VERASE];
	editb.e_kill = ttyparm.c_cc[VKILL];
	if( tty_set(fd, TCSADRAIN, &nttyparm) == SYSERR )
		return(BAD);
	editb.e_ttyspeed = ttyparm.c_cflag & CBAUD;
#endif
	editb.e_raw = RAWMODE;
	return(GOOD);
}

#ifndef RAWONLY

/*
 *
 *	Get tty parameters and make ESC and '\r' wakeup characters.
 *
 */

#   ifdef TIOCGETC
tty_alt(int fd)
{
	int mask;
	struct tchars ttychars;
	if(editb.e_raw==ALTMODE)
		return(GOOD);
	if(editb.e_raw==RAWMODE)
		tty_cooked(fd);
	l_changed = 0;
	if( editb.e_ttyspeed == 0)
	{
		if((tty_get(fd,&ttyparm) != SYSERR))
			editb.e_ttyspeed = (ttyparm.sg_ospeed>=B1200?FAST:SLOW);
#ifdef  KSH_88D
                editb.e_raw = ALTMODE;
#endif /* KSH_88D */
	}
	if(ioctl(fd,TIOCGETC,&l_ttychars) == SYSERR)
		return(BAD);
	if(ioctl(fd,TIOCLGET,&l_mask)==SYSERR)
		return(BAD);
	ttychars = l_ttychars;
	mask =  LCRTBS|LCRTERA|LCTLECH|LPENDIN|LCRTKIL;
	if((l_mask|mask) != l_mask)
		l_changed = L_MASK;
	if(ioctl(fd,TIOCLBIS,&mask)==SYSERR)
		return(BAD);
#ifdef  KSH_88D
        if (ttychars.t_brkc != ESC)
        {
#endif /* KSH_88D */
		ttychars.t_brkc = ESC;
		l_changed |= T_CHARS;
		if(ioctl(fd,TIOCSETC,&ttychars) == SYSERR)
			return(BAD);
#ifdef  KSH_88D
        }
#else
        editb.e_raw = ALTMODE;
#endif /* KSH_88D */
	return(GOOD);
}
#   else
#ifdef  KSH_88D
#       ifndef PENDIN
#           define PENDIN       0
#       endif /* PENDIN */
#       ifndef IEXTEN
#           define IEXTEN       0
#       endif /* IEXTEN */
#endif /* KSH_88D */
tty_alt(int fd)
{
	if(editb.e_raw==ALTMODE)
		return(GOOD);
	if(editb.e_raw==RAWMODE)
		tty_cooked(fd);
	if((tty_get(fd, &ttyparm)==SYSERR) || (!(ttyparm.c_lflag&ECHO)))
		return(BAD);
	nttyparm = ttyparm;
	editb.e_eof = ttyparm.c_cc[VEOF];
#	ifdef ECHOCTL
	    /* escape character echos as ^[ */
#ifdef  KSH_88D
                nttyparm.c_lflag |= (ECHOE|ECHOK|ECHOCTL|IEXTEN);
#else
                nttyparm.c_lflag |= (ECHOE|ECHOK|ECHOCTL);
#endif /* KSH_88D */
	    nttyparm.c_cc[VEOL2] = ESC;
#	else
	    /* switch VEOL2 and EOF, since EOF isn't echo'd by driver */
	    nttyparm.c_iflag &= ~(IGNCR|ICRNL);
	    nttyparm.c_iflag |= INLCR;
	    nttyparm.c_lflag |= (ECHOE|ECHOK);
	    nttyparm.c_cc[VEOF] = ESC;	/* make ESC the eof char */
	    nttyparm.c_cc[VEOL] = '\r';	/* make CR an eol char */
	    nttyparm.c_cc[VEOL2] = editb.e_eof;	/* make EOF an eol char */
#	endif /* ECHOCTL */
#	ifdef VWERASE
	    nttyparm.c_cc[VWERASE] = cntl('W');
#	endif /* VWERASE */
#	ifdef VLNEXT
	    nttyparm.c_cc[VLNEXT] = cntl('V');
#	endif /* VLNEXT */
	editb.e_erase = ttyparm.c_cc[VERASE];
	editb.e_kill = ttyparm.c_cc[VKILL];
	if( tty_set(fd, TCSADRAIN, &nttyparm) == SYSERR )
		return(BAD);
	editb.e_ttyspeed = ((ttyparm.c_cflag&CBAUD)>=B1200?FAST:SLOW);
	editb.e_raw = ALTMODE;
	return(GOOD);
}

#   endif /* TIOCGETC */
#endif	/* RAWONLY */

/*
 *	E_WINDOW()
 *
 *	return the window size
 */

#ifndef WINSIZE
#ifdef  KSH_88D
#   undef TIOCGWINSZ
#else
#   undef TIOCGWSIZE
#endif /* KSH_88D */
#   undef TIOCGWSIZE
#endif /* !WINSIZE */
#ifdef TIOCGWINSZ
#   ifdef _sys_stream_
#	include	<sys/stream.h>
#   endif /* _sys_ptem_ */
#   ifdef _sys_ptem_
#	include	<sys/ptem.h>
#   endif /* _sys_stream_ */
#else
#   ifdef _sys_jioctl_
#	include	<sys/jioctl.h>
#	define winsize		jwinsize
#	define ws_col		bytesx
#	define TIOCGWINSZ	JWINSIZE
#   endif /* _sys_jioctl_ */
#endif /*TIOCGWINSZ */
int ed_window(void)
{
	register int n = DFLTWINDOW-1;
	register char *cp = nam_strval(COLUMNS);
	if(cp)
	{
		n = atoi(cp)-1;
		if(n > MAXWINDOW)
			n = MAXWINDOW;
	}
#ifdef TIOCGWINSZ
	else
	{
		/* for 5620's and 630's */
		struct winsize size;
		if (ioctl(ERRIO, TIOCGWINSZ, &size) != -1)
			if(size.ws_col > 0)
				n = size.ws_col - 1;
	}
#endif /*TIOCGWINSZ */
	if(n < MINWINDOW)
		n = MINWINDOW;
	return(n);
}

/*	E_FLUSH()
 *
 *	Flush the output buffer.
 *
 */

void ed_flush(void)
{
	register int n = editb.e_outptr-editb.e_outbase;
	register int fd = ERRIO;
	if(n<=0)
		return;
	write(fd,editb.e_outbase,(unsigned)n);
	editb.e_outptr = editb.e_outbase;
#if     (!defined(KSH_88D) && defined(_SELECT_)) || (defined(KSH_88D) && defined(_SELECT5_))
	if(delay && n > delay/100)
	{
		/* delay until output drains */
		struct timeval timeloc;
		n *= 10;
		timeloc.tv_sec = n/delay;
		timeloc.tv_usec = (1000000*(n%delay))/delay;
		select(0,(fd_set*)0,(fd_set*)0,(fd_set*)0,&timeloc);
	}
#else
#   ifdef IODELAY
	if(editb.e_raw==RAWMODE && n > 16)
		tty_set(fd, TCSADRAIN, &nttyparm);
#   endif /* IODELAY */
#endif /* _SELECT_ */
}

/*
 * send the bell character ^G to the terminal
 */

void ed_ringbell(void)
{
	write(ERRIO,bellchr,(unsigned)1);
}

/*
 * send a carriage return line feed to the terminal
 */

void ed_crlf(void)
{
#ifdef cray
	ed_putchar('\r');
#endif /* cray */
#ifdef u370
	ed_putchar('\r');
#endif	/* u370 */
#ifdef VENIX
	ed_putchar('\r');
#endif /* VENIX */
	ed_putchar('\n');
	ed_flush();
}
 
/*	E_SETUP( max_prompt_size )
 *
 *	This routine sets up the prompt string
 *	The following is an unadvertised feature.
 *	  Escape sequences in the prompt can be excluded from the calculated
 *	  prompt length.  This is accomplished as follows:
 *	  - if the prompt string starts with "%\r, or contains \r%\r", where %
 *	    represents any char, then % is taken to be the quote character.
 *	  - strings enclosed by this quote character, and the quote character,
 *	    are not counted as part of the prompt length.
 */

void	ed_setup(int fd)
{
	register char *pp;
	register char *last;
	char *ppmax;
	int myquote = 0;
	int qlen = 1;
	char inquote = 0;
	editb.e_fd = fd;
	p_setout(ERRIO);
#ifdef KSHELL
	last = _sobuf;
#else
	last = editb.e_prbuff;
#endif /* KSHELL */
	if(hist_ptr)
	{
		register struct history *fp = hist_ptr;
		editb.e_hismax = fp->fixind;
		editb.e_hloff = 0;
		editb.e_hismin = fp->fixind-fp->fixmax;
		if(editb.e_hismin<0)
			editb.e_hismin = 0;
	}
	else
	{
		editb.e_hismax = editb.e_hismin = editb.e_hloff = 0;
	}
	editb.e_hline = editb.e_hismax;
	editb.e_wsize = ed_window()-2;
	editb.e_crlf = YES;
	pp = editb.e_prompt;
	ppmax = pp+PRSIZE-1;
	*pp++ = '\r';
	{
		register int c;
		while(c= *last++) {
#if (defined(NLS) || defined(KJI))
		if (NCisshift(c)) {
			if ((pp+1) < ppmax) {
				*pp++ = c;
				c = (c<<8) | *last++;
			}
			else 
				break;
		}
#endif
		switch(c) {
			case '\r':
				if(pp == (editb.e_prompt+2)) /* quote char */
					myquote = *(pp-1);
				/*FALLTHROUGH*/

			case '\n':
				/* start again */
				editb.e_crlf = YES;
				qlen = 1;
				inquote = 0;
				pp = editb.e_prompt+1;
				break;

			case '\t':
				/* expand tabs */
				while((pp-editb.e_prompt)%TABSIZE)
					if(pp < ppmax)
						*pp++ = ' ';
				break;

			case BELL:
				/* cut out bells */
				break;

			default:
				if(c==myquote)
				{
					qlen += inquote;
					inquote ^= 1;
				}
				if(pp < ppmax)
				{
					qlen += inquote;
#if (defined(KJI) || defined(NLS))
					*pp++ = c&STRIP;
					if(!inquote && !is_print(c))
#else
					*pp++ = c;
					if(!inquote && !isprint(c))
#endif
						editb.e_crlf = NO;
				}
		}
	}}
	editb.e_plen = pp - editb.e_prompt - qlen;
	*pp = 0;
#ifdef  KSH_88D
        if((editb.e_wsize -= editb.e_plen) < 7)
        {
                register int shift = 7-editb.e_wsize;
                editb.e_wsize = 7;
                pp = editb.e_prompt+1;
                strcpy(pp,pp+shift);
                editb.e_plen -= shift;
                last[-editb.e_plen-2] = '\r';
        }
#else
        editb.e_wsize -= editb.e_plen;
#endif /* KSH_88D */
	p_flush();
	editb.e_outptr = _sobuf;
	editb.e_outbase = editb.e_outptr;
	editb.e_outlast = editb.e_outptr + IOBSIZE-3;
}

#ifdef KSHELL
/*
 * look for edit macro named _i
 * if found, puts the macro definition into lookahead buffer and returns 1
 */

ed_macro(int i)
{
	register char *out;
	struct namnod *np;
	genchar buff[LOOKAHEAD+1];
	if(i != '@')
#if (defined(KJI) || defined(NLS))
	{
		if (i > 0377) {
			macro[1] = i>>8;
			macro[2] = i&0xff;
		}
		else {
			macro[1] = i;
			/* undocumented feature, macros of the form <ESC>[c evoke alias __c */
			if(i=='_') {
				i = ed_getchar();
				if (i > 0377) {
					macro[2] = i>>8;
					macro[3] = i&0xff;
				}
				else {
					macro[2] = i;
					macro[3] = 0; 
				}
			}
			else
				macro[2] = 0;
		}
	}
	if (NLSalphanum(i)&&(np=nam_search(macro,sh.alias_tree,N_NOSCOPE))&&(out=nam_strval(np)))
	{
		/* copy to buff in internal representation */
		int c = out[LOOKAHEAD];
		out[LOOKAHEAD] = 0;
		i = ed_internal(out,buff);
		out[LOOKAHEAD] = c;
#else
		macro[1] = i;
	/* undocumented feature, macros of the form <ESC>[c evoke alias __c */
	if(i=='_')
		macro[2] = ed_getchar();
	else
		macro[2] = 0;
	if (isalnum(i)&&(np=nam_search(macro,sh.alias_tree,N_NOSCOPE))&&(out=nam_strval(np)))
	{
#	ifdef MULTIBYTE
		/* copy to buff in internal representation */
		int c = out[LOOKAHEAD];
		out[LOOKAHEAD] = 0;
		i = ed_internal(out,buff);
		out[LOOKAHEAD] = c;
#	else
		strncpy((char*)buff,out,LOOKAHEAD);
		i = strlen((char*)buff);
#	endif /* MULTIBYTE */
#endif /* NLS | KJI */
		while(i-- > 0)
			ed_ungetchar((int)buff[i]);
		return(1);
	} 
	return(0);
}
/*
 * file name generation for edit modes
 * non-zero exit for error, <0 ring bell
 * don't search back past beginning of the buffer
 * mode is '*' for inline expansion,
 * mode is '\' for filename completion
 * mode is '=' cause files to be listed in select format
 */

ed_expand(char outbuff[], int *cur, int *eol, int mode)
{
	STKPTR staksav = sh.stakbot;
	struct comnod  *comptr = (struct comnod*)stak_alloc(sizeof(struct comnod));
	struct argnod *ap = (struct argnod*)stak_begin();
	register char *out;
	char *begin;
	int addstar;
	int istilde = 0;
	int rval = 0;
	int strip;
	register int len;
	optflag savflags = opt_flags;

#if (defined(MULTIBYTE) || defined(KJI) || defined(NLS))
	{
		register int c = *cur;
		register genchar *cp;
		/* adjust cur */
		cp = (genchar *)outbuff + *cur;
		c = *cp;
		*cp = 0;
		*cur = ed_external((genchar*)outbuff,(char*)sh.stakbot);
		*cp = c;
		*eol = ed_external((genchar*)outbuff,outbuff);
	}
#endif /* MULTIBYTE|NLS|KJI */
	out = outbuff + *cur;
	comptr->comtyp = COMSCAN;
	comptr->comarg = ap;
	ap->argflag = (A_MAC|A_EXP);
	ap->argnxt.ap = 0;
	{
		wchar_t c;
		register char *ptr = ap->argval;
		int chktilde = 0;
		char *cp;
		if(out>outbuff)
		{
#ifdef KJI
			register char *p;

			/* KJI requires search from beginning of string */
			begin = NULL;
			p = outbuff;
			while(p < out) {
				len = NCdec(p,&c);
				if (isqmeta(c))
					begin = p+len;
				p += len;
			}
			if (begin)
				out = begin;
			else
				out = outbuff;
#else
			/* go to beginning of word */
			do
			{
				out--;
				c = *(unsigned char*)out;
			}
			while(out>outbuff && !isqmeta(c));
			/* copy word into arg */
			if(isqmeta(c))
				out++;
#endif
		}
		else
			out = outbuff;
		begin = out;
		/* addstar set to zero if * should not be added */
		addstar = '*';
		strip = TRUE;
		/* copy word to arg and do ~ expansion */
		do
		{
#ifdef KJI
			len = NCdec(out,&c);
			ptr += NCenc(&c,ptr);
#else
			len = 1;
			c = *(unsigned char*)out;
			*ptr++ = c;
#endif
			if(isexp(c))
				addstar = 0;
			if ((c == '/') && (addstar == 0))
				strip = FALSE;
			if(chktilde==0 && (c==0 || c == '/'))
			{
				chktilde++;
				*out = 0;
				if(cp=sh_tilde(begin))
				{
					istilde++;
					ptr = sh_copy(cp,ap->argval);
					*ptr++ = c;
					if(c==0)
					{
						addstar = 0;
						strip = FALSE;
					}
				}
				*out = c;
			}
			out += len;

		} while (c && !isqmeta(c));

		out -= len;
#ifdef tenex
		if(mode=='\\')
			addstar = '*';
#endif /* tenex */
		*(ptr-len) = addstar;
#ifdef KJI
		if(len>1)
			*(ptr-1) = 0;
#endif
		stak_end(ptr);
	}
	if(mode!='*')
		on_option(MARKDIR);
	{
		register char **com;
		int	 narg;
		register int size;
		VOID (*savfn)();
		savfn = st.intfn;
		com = arg_build(&narg,comptr);
		st.intfn = savfn;
		/*  match? */
		if (*com==0 || (!istilde && narg <= 1 && eq(ap->argval,*com)))
		{
			rval = -1;
			goto done;
		}
		if(mode=='=')
		{
			if (strip)
			{
				register char **ptrcom;
				for(ptrcom=com;*ptrcom;ptrcom++)
					/* trim directory prefix */
					*ptrcom = path_basename(*ptrcom);
			}
			p_setout(ERRIO);
			newline();
			p_list(narg,(const char **)com);
			p_flush();
			goto done;
		}
		/* see if there is enough room */
		size = *eol - (out-begin);
#ifdef tenex
		if(mode=='\\')
		{
			/* just expand until name is unique */
			size += strlen(*com);
		}
		else
#endif
		{
			size += narg;
			{
				char **savcom = com;
				while (*com)
					size += strlen(*com++);
				com = savcom;
			}
		}
		/* see if room for expansion */
		if(outbuff+size >= &outbuff[MAXLINE])
		{
			com[0] = ap->argval;
			com[1] = 0;
		}
		/* save remainder of the buffer */
		strcpy(sh.stakbot,out);
		out = sh_copy(*com++, begin);
#ifndef KSH_88D
		if(*com==0 && out[-1]!='/')
			*out++ = ' ';
#endif /* KSH_88D */
#ifdef tenex
		if(mode=='\\')
		{
#       ifdef   KSH_88D
                if(*com==0 && out[-1]!='/')
                        *out++ = ' ';
#       endif /* KSH_88D */
			while (*com && *begin)
				out = overlay(begin,*com++);
			if(*begin==0)
				ed_ringbell();
		}
		else
#endif
			while (*com)
			{
				*out++  = ' ';
				out = sh_copy(*com++,out);
			}
		*cur = (out-outbuff);
		/* restore rest of buffer */
		out = sh_copy(sh.stakbot,out);
		*eol = (out-outbuff);
	}
 done:
	stak_reset(staksav);
	opt_flags = savflags;
#if (defined(MULTIBYTE) || defined(KJI) || defined(NLS))
	{
		register int c;
		/* first re-adjust cur */
		out = outbuff + *cur;
		c = *out;
		*out = 0;
		*cur = ed_internal(outbuff,(genchar*)sh.stakbot);
		*out = c;
		outbuff[*eol+1] = 0;
		*eol = ed_internal(outbuff,(genchar*)outbuff);
	}
#endif /* MULTIBYTE|NLS|KJI */
	return(rval);
}

#   ifdef tenex
static char *overlay(str,newstr)
register char *str,*newstr;
{
	while(*str && *str == *newstr++)
		str++;
	*str = 0;
	return(str);
}
#   endif

/*
 * Enter the fc command on the current history line
 */
ed_fulledit()
{
	register char *cp;
	/* use EDITOR on current command */
	if(editb.e_hline == editb.e_hismax)
	{
		if(editb.e_eol<=0)
			return(BAD);
		editb.e_inbuf[editb.e_eol+1] = 0;
		p_setout(hist_ptr->fixfd);
		p_str((char*)editb.e_inbuf,0);
		st.states |= FIXFLG;
		hist_flush();
	}
	cp = sh_copy(e_runvi, (char*)editb.e_inbuf);
	cp = sh_copy(sh_itos(editb.e_hline), cp);
	editb.e_eol = (unsigned char*)cp - (unsigned char*)editb.e_inbuf;
	return(GOOD);
}
#endif	/* KSHELL */
 

/*
 * routine to perform read from terminal for vi and emacs mode
 */


int 
ed_getchar(void)
{
	register int i;
	register int c;
	register int maxtry = MAXTRY;
	unsigned nchar = READAHEAD; /* number of characters to read at a time */
#ifdef MULTIBYTE
	static int curchar;
	static int cursize;
#endif /* MULTIBYTE */
	char readin[LOOKAHEAD] ;
	if (lookahead)
	{
		c = previous[--lookahead];
		/*** map '\r' to '\n' ***/
#ifdef  KSH_88D
                if(c == '\r' && !in_raw)
#else
                if(c == '\r')
#endif /* KSH_88D */
			c = '\n';
		return(c);
	}
	
	ed_flush() ;
	/*
	 * you can't chance read ahead at the end of line
	 * or when the input is a pipe
	 */
#ifdef  KSH_88D
#ifdef KSHELL
        if((editb.e_cur>=editb.e_eol) || fnobuff(io_ftable[fildes]))
#else
        if(editb.e_cur>=editb.e_eol)
#endif /* KSHELL */
#else
        if((editb.e_cur>=editb.e_eol) || fnobuff(io_ftable[fildes]))
#endif /* KSH_88D */
		nchar = 1;
	/* Set 'i' to indicate read failed, in case intr set */
retry:
	i = -1;
	errno = 0;
	editb.e_inmacro = 0;
	while(slowsig()==0 && maxtry--)
	{
		errno=0;
		if ((i = read(fildes,readin, nchar)) != -1)
			break;
	}
#if (defined(NLS) || defined(KJI))
	lookahead = maxtry = i;
	i = 0;
	while (i < maxtry)
	{
		c = readin[i++] & STRIP;
		if (NCisshift(c)) {
			if (i == maxtry)
				read(fildes,&readin[maxtry],(unsigned)1);
			c = (c<<8) | (readin[i++] & STRIP);
		}
		previous[--lookahead] = c;
#endif
#ifdef MULTIBYTE
	lookahead = maxtry = i;
	i = 0;
	while (i < maxtry)
	{
		c = readin[i++] & STRIP;
	next:
		if(cursize-- > 0)
		{
			curchar = (curchar<<7) | (c&~HIGHBIT);
			if(cursize==0)
			{
				c = curchar;
				goto gotit;
			}
			else if(i>=maxtry)
				goto retry;
			continue;
		}
		else if(curchar = echarset(c))
		{
			cursize = in_csize(curchar);
			if(curchar != 1)
				c = 0;
			curchar <<= 7*(ESS_MAXCHAR-cursize);
			if(c)
				goto next;
			else if(i>=maxtry)
				goto retry;
			continue;
		}
	gotit:
		previous[--lookahead] = c;
#endif
#if !(defined(MULTIBYTE) || defined(NLS) || defined(KJI))
	while (i > 0)
	{
		c = readin[--i] & STRIP;
		previous[lookahead++] = c;
#endif /* normal, non-MULTI, etc. */
#ifndef CBREAK
		if( c == '\0' )
		{
			/*** user break key ***/
			lookahead = 0;
# ifdef KSHELL
			sh_fault(SIGINT);
			longjmp(env, UINTR);
# endif	/* KSHELL */
		}
#endif	/* !CBREAK */
	}
#if (defined(MULTIBYTE) || defined(NLS) || defined(KJI))
	/* shift lookahead buffer if necessary */
	if(lookahead)
	{
		for(i=lookahead;i < maxtry;i++)
			previous[i-lookahead] = previous[i];
	}
	lookahead = maxtry-lookahead;
#endif /* MULTIBYTE|NLS|KJI */
	if (lookahead > 0)
		return(ed_getchar());
	longjmp(env,(i==0?UEOF:UINTR)); /* What a mess! Give up */
	/* NOTREACHED */
}

void ed_ungetchar(int c)
{
	if (lookahead < LOOKAHEAD)
		previous[lookahead++] = c;
	return;
}

/*
 * put a character into the output buffer
 */

void	ed_putchar(int c)
{
	register char *dp = editb.e_outptr;
#ifdef KJI
	/* check for place holder */
	if(c == MARKER)
		return;
#endif
#ifdef MULTIBYTE
	register int d;
	/* check for place holder */
	if(c == MARKER)
		return;
	if(d = icharset(c))
	{
		if(d == 2)
			*dp++ = ESS2;
		else if(d == 3)
			*dp++ = ESS3;
		d = in_csize(d);
		while(--d>0)
			*dp++ = HIGHBIT|(c>>(7*d));
		c |= HIGHBIT;
	}
#endif	/* MULTIBYTE */
	if (c == '_')
	{
		*dp++ = ' ';
		*dp++ = '\b';
	}
#if (defined(NLS) || defined(KJI))
	if (NCisshift(c>>8))
		*dp++ = c>>8;
	*dp++ = c&STRIP;
#else
	*dp++ = c;
#endif
	*dp = '\0';
	if(dp >= editb.e_outlast)
		ed_flush();
	else
		editb.e_outptr = dp;
}

/*
 * copy virtual to physical and return the index for cursor in physical buffer
 */
ed_virt_to_phys(genchar *virt, genchar *phys, int cur, int voff, int poff)
{
	register genchar *sp = virt;
	register genchar *dp = phys;
	register int c;
	genchar *curp = sp + cur;
	genchar *dpmax = phys+MAXLINE;
	int r;
#ifdef MULTIBYTE
	int d;
#endif /* MULTIBYTE */
	sp += voff;
	dp += poff;
	for(r=poff;c= *sp;sp++)
	{
		if(curp == sp)
			r = dp - phys;
#ifdef MULTIBYTE
		d = out_csize(icharset(c));
		if(d>1)
		{
			/* multiple width character put in place holders */
			*dp++ = c;
			while(--d >0)
				*dp++ = MARKER;
			/* in vi mode the cursor is at the last character */
			if(dp>=dpmax)
				break;
			continue;
		}
		else
#endif	/* MULTIBYTE */
#ifdef KJI
		if (NCisshift(c>>8)) {
			*dp++ = c;
			/* 2-byte character; put in place holder */
			*dp++ = MARKER;
			/* in vi mode the cursor is at the last character */
			if(dp>=dpmax)
				break;
			continue;
		}
#endif  /* KJI */
#if (defined(NLS) || defined(KJI))
		if (!is_print(c))
#else
		if(!isprint(c))
#endif
		{
			if(c=='\t')
			{
				c = dp-phys;
				if(is_option(EDITVI))
					c += editb.e_plen;
				c = TABSIZE - c%TABSIZE;
				while(--c>0)
					*dp++ = ' ';
				c = ' ';
			}
			else
			{
				*dp++ = '^';
				c ^= TO_PRINT;
			}
			/* in vi mode the cursor is at the last character */
			if(curp == sp && is_option(EDITVI))
				r = dp - phys;
		}
		*dp++ = c;
		if(dp>=dpmax)
			break;
	}
	*dp = 0;
	return(r);
}

#if     (defined(MULTIBYTE) || defined(NLS) || defined(KJI))
/*
 * convert external representation <src> to an array of genchars <dest>
 * <src> and <dest> can be the same
 * returns number of chars in dest
 */

int	ed_internal(src,dest)
register unsigned char *src;
genchar *dest;
{
	register int c;
	register genchar *dp = dest;
#if (defined(NLS) || defined(KJI))
        c = strlen( src ) ;
        if (   (unsigned char*)dest == src ||
             ( (unsigned char*)dest < src &&
               (unsigned char*)dest + (c <<1) > src + c ) ||
             ( (unsigned char*)dest > src &&
               (unsigned char*)dest < src + c ) )
#else /* MULTIBYTE */
	register int d;
	register int size;
	if((unsigned char*)dest == src)
#endif /* NLS || KJI */
	{
		genchar buffer[MAXLINE];
		c = ed_internal(src,buffer);
		ed_gencpy(dp,buffer);
		return(c);
	}
	while(c = *src++)
	{
#if (defined(NLS) || defined(KJI))
                if (NCisshift(c))
                        c = (c<<8) | *src++;
#else /* MULTIBYTE */
		if(size = echarset(c))
		{
			d = (size==1?c:0);
			c = size;
			size = in_csize(c);
			c <<= 7*(ESS_MAXCHAR-size);
			if(d)
			{
				size--;
				c = (c<<7) | (d&~HIGHBIT);
			}
			while(size-- >0)
				c = (c<<7) | ((*src++)&~HIGHBIT);
		}
#endif /* NLS || KJI */
		*dp++ = c;
	}
	*dp = 0;
	return(dp-dest);
}

/*
 * convert internal representation <src> into character array <dest>.
 * The <src> and <dest> may be the same.
 * returns number of chars in dest.
 */

int	ed_external(src,dest)
genchar *src;
char *dest;
{
	register int c;
	register char *dp = dest;
#ifdef  MULTIBYTE
        register int d;
#endif /* MULTIBYTE */
	char *dpmax = dp+sizeof(genchar)*MAXLINE-2;
	if((char*)src == dp)
	{
		char buffer[MAXLINE*sizeof(genchar)];
		c = ed_external(src,buffer);
		strcpy(dest,buffer);
		return(c);
	}
	while((c = *src++) && dp<dpmax)
	{
#if (defined(NLS) || defined(KJI))
                if (NCisshift(c>>8))
                        *dp++ = c>>8;
                *dp++ = c&STRIP;
#else /* MULTIBYTE */
		if(d = icharset(c))
		{
			if(d == 2)
				*dp++ = ESS2;
			else if(d == 3)
				*dp++ = ESS3;
			d = in_csize(d);
			while(--d>0)
				*dp++ = HIGHBIT|(c>>(7*d));
			c |= HIGHBIT;
		}
		*dp++ = c;
#endif /* NLS || KJI */
	}
	*dp = 0;
	return(dp-dest);
}
#endif /* MULTIBYTE */

#if 0
#if (defined(NLS) || defined(KJI))
/*
 * convert external representation <src> to an array of genchars <dest>
 * <src> and <dest> can be the same
 * returns number of elements in dest
 */

int	ed_internal(src,dest)
register unsigned char *src;
genchar *dest;
{
	register int c;
	register genchar *dp = dest;

	c = strlen( (const char *)src ) ;
	if (   (unsigned char*)dest == src ||
	     ( (unsigned char*)dest < src &&
	       (unsigned char*)dest + (c <<1) > src + c ) ||
	     ( (unsigned char*)dest > src &&
	       (unsigned char*)dest < src + c ) )
	{
		genchar buffer[MAXLINE];
		c = ed_internal(src,buffer);
		ed_gencpy(dp,buffer);
		return(c);
	}
	while(c = *src++)
	{
		if (NCisshift(c))
			c = (c<<8) | *src++;
		*dp++ = c;
	}
	*dp = 0;
	return(dp-dest);
}

/*
 * convert internal representation <src> into character array <dest>.
 * The <src> and <dest> may be the same.
 * returns number of chars in dest.
 */

int	ed_external(src,dest)
genchar *src;
char *dest;
{
	register int c;
	register char *dp = dest;
	char *dpmax = dp+sizeof(genchar)*MAXLINE-2;

	if((char*)src == dp)
	{
		char buffer[MAXLINE*sizeof(genchar)];
		c = ed_external(src,buffer);
		strcpy(dest,buffer);
		return(c);
	}
	while((c = *src++) && dp<dpmax)
	{
		if (NCisshift(c>>8)) 
			*dp++ = c>>8;
		*dp++ = c&STRIP;
	}
	*dp = 0;
	return(dp-dest);
}
#endif /* NLS | KJI */
#endif /* 0 */

#if (defined(MULTIBYTE) || defined(NLS) || defined(KJI))
/*
 * copy <sp> to <dp>
 */

int	ed_gencpy(dp,sp)
register genchar *dp;
register genchar *sp;
{
	while(*dp++ = *sp++);
}

/*
 * copy at most <n> items from <sp> to <dp>
 */

int	ed_genncpy(dp,sp, n)
register genchar *dp;
register genchar *sp;
register int n;
{
	while(n-->0 && (*dp++ = *sp++));
}

/*
 * find the string length of <str>
 */

int	ed_genlen(str)
register genchar *str;
{
	register genchar *sp = str;
	while(*sp++);
	return(sp-str-1);
}
#endif /* MULTIBYTE|NLS|KJI */
#endif /* ESH || VSH */

#ifdef MULTIBYTE
/*
 * set the multibyte widths
 * format of string is x1[:y1][,x2[:y2][,x3[:y3]]]
 * returns 1 if string in not in this format, 0 otherwise.
 */

extern char int_charsize[];
ed_setwidth(string)
char *string;
{
	register int indx = 0;
	register int state = 0;
	register int c;
	register int n = 0;
	static char widths[6] = {1,1};
	while(1) switch(c = *string++)
	{
		case ':':
			if(state!=1)
				return(1);
			state++;
			/* fall through */

		case 0:
		case ',':
			if(state==0)
				return(1);
			widths[indx++] = n;
			if(state==1)
				widths[indx++] = n;
			if(c==0)
			{
				for(n=1;n<= 3;n++)
				{
					int_charsize[n] = widths[c++];
					int_charsize[n+4] = widths[c++];
				}
				return(0);
			}
			else if(c==',')
				state = 0;
			n = 0;
			break;

		case '0': case '1': case '2': case '3': case '4':
			if(state&1)
				return(1);
			n = c - '0';
			state++;
			break;
			
		default:
			return(1);
	}
	/* NOTREACHED */
}
#endif /* MULTIBYTE */

#ifdef future
/*
 * returns 1 when <n> bytes starting at <a> and <b> are equal
 */
static int compare(a,b,n)
register char *a;
register char *b;
register int n;
{
	while(n-->0)
	{
		if(*a++ != *b++)
			return(0);
	}
	return(1);
}
#endif

#ifdef  KSH_88D
#ifdef OLDTERMIO

#   include     <sys/termio.h>

#ifndef ECHOCTL
#   define ECHOCTL      0
#endif /* !ECHOCTL */
char echoctl;
static char tcgeta;
static struct termio ott;

/*
 * For backward compatibility only
 * This version will use termios when possible, otherwise termio
 */


tcgetattr(fd,tt)
struct termios *tt;
{
        register int r;
        register int i;
        tcgeta = 0;
        echoctl = (ECHOCTL!=0);
        if((r=ioctl(fd,TCGETS,tt))>=0 ||  errno!=EINVAL)
                return(r);
        if((r=ioctl(fd,TCGETA,&ott)) >= 0)
        {
                tt->c_lflag = ott.c_lflag;
                tt->c_oflag = ott.c_oflag;
                tt->c_iflag = ott.c_iflag;
                tt->c_cflag = ott.c_cflag;
                for(i=0; i<NCC; i++)
                        tt->c_cc[i] = ott.c_cc[i];
                tcgeta++;
                echoctl = 0;
        }
        return(r);
}

tcsetattr(fd,mode,tt)
register int mode;
struct termios *tt;
{
        register int r;
        if(tcgeta)
        {
                register int i;
                ott.c_lflag = tt->c_lflag;
                ott.c_oflag = tt->c_oflag;
                ott.c_iflag = tt->c_iflag;
                ott.c_cflag = tt->c_cflag;
                for(i=0; i<NCC; i++)
                        ott.c_cc[i] = tt->c_cc[i];
                if(tt->c_lflag&ECHOCTL)
                {
                        ott.c_lflag &= ~(ECHOCTL|IEXTEN);
                        ott.c_iflag &= ~(IGNCR|ICRNL);
                        ott.c_iflag |= INLCR;
                        ott.c_cc[VEOF]= ESC;  /* ESC -> eof char */
                        ott.c_cc[VEOL] = '\r'; /* CR -> eol char */
                        ott.c_cc[VEOL2] = tt->c_cc[VEOF]; /* EOF -> eol char */
                }
                switch(mode)
                {
                        case TCSANOW:
                                mode = TCSETA;
                                break;
                        case TCSADRAIN:
                                mode = TCSETAW;
                                break;
                        case TCSAFLUSH:
                                mode = TCSETAF;
                }
                return(ioctl(fd,mode,&ott));
        }
        return(ioctl(fd,mode,tt));
}
#endif /* OLDTERMIO */
#endif /* KSH_88D */

