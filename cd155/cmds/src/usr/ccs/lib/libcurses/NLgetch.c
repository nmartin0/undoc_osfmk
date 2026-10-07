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
static char rcsid[] = "@(#)$RCSfile: NLgetch.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:16:05 $";
#endif
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS: NLwgetch, _catch_alarm, _fpk
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * NLgetch.c	1.9  com/lib/curses,3.1,9008 12/14/89 17:52:34
 */

# include	"cursesext.h"
# include	<signal.h>
# include	<errno.h>

static _fpk();		/* predefine to remove warnings -- TEJ */

static int sig_caught;

/*
 * Global two-character string array to handle character conversions.
 */
static char conv[2];
static short convret;

/*
 * NAME:        NLwgetch
 *
 * FUNCTION:
 *
 *      This routine reads in a character from the window.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      wgetch MUST return an int, not a char, because it can return
 *      things like ERR, meta characters, and function keys > 256.
 */

int
NLwgetch(win)
register WINDOW	*win;
{

	int inp;
	register int i, j;
	int arg;
	char	weset = FALSE;
	FILE *inf;

	if (SP->fl_echoit && !win->_scroll && (win->_flags&_FULLWIN)
	    && win->_curx == win->_maxx && win->_cury == win->_maxy)
		return ERR;
# ifdef DEBUG
	if(outf) fprintf(outf,
		"WGETCH: SP->fl_echoit = %c, SP->fl_rawmode = %c\n",
		SP->fl_echoit ? 'T' : 'F', SP->fl_rawmode ? 'T' : 'F');
	if (outf) fprintf(outf,
		"_use_keypad %d, kp_state %d\n",
		win->_use_keypad, SP->kp_state);
# endif
	if (SP->fl_echoit && !SP->fl_rawmode) {
		cbreak();
		weset++;
	}

#ifdef KEYPAD
	/* Make sure keypad on is in proper state */
	if (win->_use_keypad != SP->kp_state) {
		_kpmode(win->_use_keypad);
		fflush(stdout);
	}
#endif

	/* Make sure we are in proper nodelay state */
	if (win->_nodelay != SP->fl_nodelay)
		_fixdelay(SP->fl_nodelay, win->_nodelay);

	/* Check for pushed typeahead.  We make the assumption that
	 * if a function key is being typed, there is no pushed
	 * typeahead from the previous key waiting.
	 */
	if (SP->input_queue[0] >= 0) {
		inp = SP->input_queue[0];
		for (i=0; i<16; i++) {
			SP->input_queue[i] = SP->input_queue[i+1];
			if (SP->input_queue[i] < 0)
				break;
		}
		goto gotit;
	}

	inf = SP->input_file;
	if (inf == stdout)	/* so output can be teed somewhere */
		inf = stdin;
#ifdef FIONREAD
	if (win->_nodelay) {
		ioctl(fileno(inf), FIONREAD, &arg);
#ifdef DEBUG
		if (outf) fprintf(outf, "FIONREAD returns %d\n", arg);
#endif
		if (arg < 1)
			return -1;
	}
#endif
	for (i = -1; i<0; ) {
		extern int errno;
		sig_caught = 0;
		i = read(fileno(inf), conv, 1);
		if (i == 1 && NCisshift(conv[0]))
			i = read(fileno(inf), &(conv[1]), 1);
			
		/*
		 * I hope the system won't retern infinite EINTRS - maybe
		 * there should be a hop count here.
		 */
		if (i < 0 && errno != EINTR && !sig_caught) {
			inp = ERR;
			goto gotit;
		}
	}
	if (i > 0) {
		inp = NCdechr(conv);
	} else {
		inp = ERR;
		goto gotit;
	}
# ifdef DEBUG
	if(outf) fprintf(outf,"WGETCH got '%s'\n",unctrl(inp));
# endif

#ifdef KEYPAD
	/* Check for arrow and function keys */
	if (win->_use_keypad) {
		SP->input_queue[0] = inp;
		SP->input_queue[1] = -1;
		for (i=0; SP->kp[i].keynum > 0; i++) {
			if (SP->kp[i].sends[0] == inp) {
				for (j=0; ; j++) {
					if ((int) SP->kp[i].sends[j] <= 0)
						break;	/* found */
					if (SP->input_queue[j] == -1) {
						SP->input_queue[j] =
							_fpk(inf);
						SP->input_queue[j+1] = -1;
					}
					if (SP->kp[i].sends[j] !=
						SP->input_queue[j])
							/* not this one */
						goto contouter;
				}
				/* It matched the function key. */
				inp = SP->kp[i].keynum;
				SP->input_queue[0] = -1;
				goto gotit;
			}
		contouter:;
		}
		/* Didn't match any function keys. */
		inp = SP->input_queue[0];
		for (i=0; i<16; i++) {
			SP->input_queue[i] = SP->input_queue[i+1];
			if (SP->input_queue[i] < 0)
				break;
		}
		goto gotit;
	}
#endif

	if (SP->fl_echoit) {
		/*
		 * Don't want to call wrefresh() until both bytes
		 * in a two-byte NLS character are stored in the window
		 * buffer.
		 */
		convret = NCencode((NLchar *) &inp, conv);
		waddch(win, (chtype) conv[0]);
		if (convret == 2)
			waddch(win, (chtype) conv[1]);
		wrefresh(win);
	}
gotit:
	if (weset)
		nocbreak();
#ifdef DEBUG
	if(outf) fprintf(outf, "getch returns %o, pushed %o %o %o\n",
		inp, SP->input_queue[0], SP->input_queue[1],
		SP->input_queue[2]);
#endif
	return inp;
}

/*
 * NAME:        _catch_alarm
 */

static void
_catch_alarm()
{
	sig_caught = 1;
}

/*
 * NAME:        _fpk(f)
 *
 * FUNCTION:    Fast peek key.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      Like getchar but if the right flags are set, times out
 *      quickly if there is nothing waiting, returning -1.
 *      f is an output stdio descriptor, we read from the fileno.  win is
 *      the window this is supposed to have something to do with.
 *
 *      Traditional implementation.  The best resolution we have is 1
 *      second, so we set a 1 second alarm and try to read.  If we fail for
 *      1 second, we assume there is no key waiting.  Problem here is that
 *      1 second is too long, people can type faster than this.
 *
 *      If we have the select system call (FIONREAD), we can do much better.
 *      We wait for long enough for a terminal to send another character
 *      (at 15cps repeat rate, this is 67 ms, I'm using 100ms to allow
 *      a bit of a fudge factor) and time out more quickly.  Even if we
 *      don't have the real 4.2BSD select, we can emulate it with napms
 *      and FIONREAD.  napms might be done with only 1 second resolution,
 *      but this is no worse than what we have above.
 */

#ifndef FIONREAD
static
_fpk(f)
FILE *f;
{
	char c;
	int rc;
	int (*oldsig)();
	int oldalarm;

	oldsig = (FUNC) signal((int)SIGALRM,
				(void (*)(int))(int)_catch_alarm);
	oldalarm = alarm(1);
	sig_caught = 0;
	rc = read(fileno(f), &c, 1);
	if (sig_caught) {
		sig_caught = 0;
		alarm(oldalarm);
		return -2;
	}
	alarm(oldalarm);
	signal((int)SIGALRM, (void (*) (int))(int)oldsig);
	return rc == 1 ? c : -1;
}

#else FIONREAD

#include	<sys/time.h>

static
_fpk(f)
FILE *f;
{
	int rc;
	fd_set infd;
	struct timeval timeout;
	char c;

	FD_ZERO(&infd);
	FD_SET(fileno(f), &infd);
	timeout.tv_sec = 0;
	timeout.tv_usec = 100;
	rc = select(20, &infd, NULL, NULL, &timeout);
	if (rc < 0)
		return -2;
	rc = read(fileno(f), &c, 1);
	return rc == 1 ? c : -1;
}
#endif /* FIONREAD */
