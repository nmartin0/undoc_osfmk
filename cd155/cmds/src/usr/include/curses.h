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
/* @(#)curses.h 1.17  com/inc,3.1,9013 1/18/90 10:10:41 */
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   curses.h
 *
 * ORIGINS: 3, 10, 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

# undef         HZ              /* in case they've included <sys/param.h> */

#ifndef _CURSES_H_
#define _CURSES_H_

/*
 * If _BSD is defined, the user gets the Berkeley header file info
 * In addition, each routine is #defined to another name to avoid
 * conflicts with SYSV curses.  Both sets of routines are present
 * in libcurses.a.
 * 
 * Note: the BSD curses is *NOT* internationalized.
 */
#ifndef _BSD

/*
 * SYSV/AIX curses
 */

#if !defined(NLS) && !defined (_KJI)
#define NLS
#endif

#if defined(NLS) || defined (_KJI)
# include <sys/NLchar.h>
#endif

#  include  <stdio.h>
  /*
   * This is used to distinguish between USG and V7 systems.
   * Assume that L_ctermid is only defined in stdio.h in USG
   * systems, but not in V7 or Berkeley UNIX.
   */
#  ifdef L_ctermid
#  define USG
#  endif
#  include  <unctrl.h>
#  ifdef USG
#   include <termio.h>
   typedef struct termio SGTTY;
#  else
#   include <sgtty.h>
   typedef struct sgttyb SGTTY;
#  endif

/*********** These are temporary ***************/
# define        bool    char
# define        reg     register

/*
 * Capabilities from termcap
 */

extern bool     AM, BS, CA, DA, DB, EO, HC, HZ, IN, MI, MS, NC, NS, OS, UL,
                XB, XN, XT, XS, XX;
extern char     *AL, *BC, *BT, *CD, *CE, *CL, *CM, *CR, *CS, *DC, *DL,
                *DM, *DO, *ED, *EI, *K0, *K1, *K2, *K3, *K4, *K5, *K6,
                *K7, *K8, *K9, *HO, *IC, *IM, *IP, *KD, *KE, *KH, *KL,
                *KR, *KS, *KU, *LL, *MA, *ND, *NL, *RC, *SC, *SE, *SF,
                *SO, *SR, *TA, *TE, *TI, *UC, *UE, *UP, *US, *VB, *VS,
                *VE, *AL_PARM, *DL_PARM, *UP_PARM, *DOWN_PARM,
                *LEFT_PARM, *RIGHT_PARM;
extern char     PC;

/*
 * chtype is the type used to store a character together with attributes.
 * It can be set to "char" to save space, or "long" to get more attributes.
 */
# ifdef	CHTYPE
	typedef	CHTYPE chtype;
# else
#ifdef NLS
	/*
	 * Under current implementation:
	 *      long = 		32 bits
	 *      NLchar = 	under ISO: 10 bits
	 *      attributes =	under ISO: 22 bits
	 */
	typedef unsigned long chtype;
#else
	typedef unsigned short chtype;
#endif /* NLS */
# endif /* CHTYPE */

# define        TRUE    1
# define        FALSE   0

# define        ERR     (-1)
# define        OK      0


# define	_SUBWIN		01
# define	_ENDLINE	02
# define	_FULLWIN	04
# define	_SCROLLWIN	010
# define	_FLUSH		020
# define	_ISPAD		040
#ifdef NLS
# define	_STANDOUT	0x80000000
#else
# define	_STANDOUT	0200
#endif
# define        _NOCHANGE       (-1)

struct _win_st {
	short	_cury, _curx;
	short	_maxy, _maxx;
	short	_begy, _begx;
	short   _flags;
	chtype  _attrs;
	char    _clear;
	char    _leave;
	char    _scroll;
	char    _use_idl;
	char    _use_keypad;    /* 0=no, 1=yes, 2=yes/timeout */
	char    _use_meta;      /* T=use the meta key */
	char    _nodelay;       /* T=don't wait for tty input */
	chtype	**_y;
	short	*_firstch;
	short	*_lastch;
	short	_tmarg,_bmarg;
};

extern int	LINES, COLS;

typedef struct _win_st	WINDOW;
extern WINDOW	*stdscr, *curscr;

extern char	*Def_term, ttytype[];

typedef struct screen	SCREEN;

# ifndef NOMACROS
#  ifndef MINICURSES
/*
 * psuedo functions for standard screen
 */
# define	addch(ch)	waddch(stdscr, ch)
# define	getch()		wgetch(stdscr)
#if defined(NLS) || defined(_KJI)
# define	NLgetch()	NLwgetch(stdscr)
#endif
# define	addstr(str)	waddstr(stdscr, str)
# define	getstr(str)	wgetstr(stdscr, str)
# define	move(y, x)	wmove(stdscr, y, x)
# define	clear()		wclear(stdscr)
# define	erase()		werase(stdscr)
# define	clrtobot()	wclrtobot(stdscr)
# define	clrtoeol()	wclrtoeol(stdscr)
# define	insertln()	winsertln(stdscr)
# define	deleteln()	wdeleteln(stdscr)
# define	refresh()	wrefresh(stdscr)
# define	inch()		winch(stdscr)
# define	insch(c)	winsch(stdscr,c)
# define	delch()		wdelch(stdscr)
# define	standout()	wstandout(stdscr)
# define	standend()	wstandend(stdscr)
# define	attron(at)	wattron(stdscr,at)
# define	attroff(at)	wattroff(stdscr,at)
# define	attrset(at)	wattrset(stdscr,at)

# define	setscrreg(t,b)	wsetscrreg(stdscr, t, b)
# define	wsetscrreg(win,t,b)	(win->_tmarg=(t),win->_bmarg=(b))

/*
 * mv functions
 */
#define	mvwaddch(win,y,x,ch)	(wmove(win,y,x)==ERR?ERR:waddch(win,ch))
#define	mvwgetch(win,y,x)	(wmove(win,y,x)==ERR?ERR:wgetch(win))
#define	mvwaddstr(win,y,x,str)	(wmove(win,y,x)==ERR?ERR:waddstr(win,str))
#define	mvwgetstr(win,y,x,str)	(wmove(win,y,x)==ERR?ERR:wgetstr(win,str))
#define	mvwinch(win,y,x)	(wmove(win,y,x)==ERR?ERR:winch(win))
#define	mvwdelch(win,y,x)	(wmove(win,y,x)==ERR?ERR:wdelch(win))
#define	mvwinsch(win,y,x,c)	(wmove(win,y,x)==ERR?ERR:winsch(win,c))
#define	mvaddch(y,x,ch)		mvwaddch(stdscr,y,x,ch)
#define	mvgetch(y,x)		mvwgetch(stdscr,y,x)
#define	mvaddstr(y,x,str)	mvwaddstr(stdscr,y,x,str)
#define	mvgetstr(y,x,str)	mvwgetstr(stdscr,y,x,str)
#define	mvinch(y,x)		mvwinch(stdscr,y,x)
#define	mvdelch(y,x)		mvwdelch(stdscr,y,x)
#define	mvinsch(y,x,c)		mvwinsch(stdscr,y,x,c)

#  else /* MINICURSES */

# define	addch(ch)		m_addch(ch)
# define	addstr(str)		m_addstr(str)
# define	move(y, x)		m_move(y, x)
# define	clear()			m_clear()
# define	erase()			m_erase()
# define	refresh()		m_refresh()
# define	standout()		wstandout(stdscr)
# define	standend()		wstandend(stdscr)
# define	attron(at)		wattron(stdscr,at)
# define	attroff(at)		wattroff(stdscr,at)
# define	attrset(at)		wattrset(stdscr,at)
# define	mvaddch(y,x,ch)		move(y, x), addch(ch)
# define	mvaddstr(y,x,str)	move(y, x), addstr(str)
# define	initscr			m_initscr
# define	newterm			m_newterm

/*
 * These functions don't exist in minicurses, so we define them
 * to nonexistent functions to help the user catch the error.
 */
#define	getch		m_getch
#define	getstr		m_getstr
#define	clrtobot	m_clrtobot
#define	clrtoeol	m_clrtoeol
#define	insertln	m_insertln
#define	deleteln	m_deleteln
#define	inch		m_inch
#define	insch		m_insch
#define	delch		m_delch
/* mv functions that aren't valid */
#define	mvwaddch	m_mvwaddch
#define	mvwgetch	m_mvwgetch
#define	mvwaddstr	m_mvaddstr
#define	mvwgetstr	m_mvwgetstr
#define	mvwinch		m_mvwinch
#define	mvwdelch	m_mvwdelch
#define	mvwinsch	m_mvwinsch
#define	mvgetch		m_mvwgetch
#define	mvgetstr	m_mvwgetstr
#define	mvinch		m_mvwinch
#define	mvdelch		m_mvwdelch
#define	mvinsch		m_mvwinsch
/* Real functions that aren't valid */
#define box		m_box
#define delwin		m_delwin
#define longname	m_longname
#define makenew		m_makenew
#define mvprintw	m_mvprintw
#define mvscanw		m_mvscanw
#define mvwin		m_mvwin
#define mvwprintw	m_mvwprintw
#define mvwscanw	m_mvwscanw
#define newwin		m_newwin
#define _outchar        m_outchar
#define overlay		m_overlay
#define overwrite	m_overwrite
#define printw		m_printw
#define putp		m_putp
#define scanw		m_scanw
#define scroll		m_scroll
#define subwin		m_subwin
#define touchwin	m_touchwin
#define _tscroll        m_tscroll
#define _tstp		m_tstp
#define vidattr		m_vidattr
#define waddch		m_waddch
#define waddstr		m_waddstr
#define wclear		m_wclear
#define wclrtobot	m_wclrtobot
#define wclrtoeol	m_wclrtoeol
#define wdelch		m_wdelch
#define wdeleteln	m_wdeleteln
#define werase		m_werase
#define wgetch		m_wgetch
#define wgetstr		m_wgetstr
#define winsch		m_winsch
#define winsertln	m_winsertln
#define wmove		m_wmove
#define wprintw		m_wprintw
#define wrefresh	m_wrefresh
#define wscanw		m_wscanw
#define setscrreg	m_setscrreg
#define wsetscrreg	m_wsetscrreg

#  endif /* MINICURSES */

/*
 * psuedo functions
 */

#define	getyx(win,y,x)	 y = win->_cury, x = win->_curx
#define	winch(win)	 (win->_y[win->_cury][win->_curx])
#define flushok(win,bf)  (bf ? (win->_flags|=_FLUSH):(win->_flags&=~_FLUSH))

WINDOW	*initscr(), *newwin(), *subwin(), *newpad();
char	*longname();
char	erasechar(), killchar();
int	wgetch();	/* because it can return KEY_*, for instance. */
SCREEN	*newterm();

/* Various video attributes */
#ifdef NLS
/*
   We start from the left and attributes bits from left to right to permit
   larger collating sequences.  Add attributes in this fashion if necessary.
*/

#define A_STANDOUT	0x80000000
#define A_UNDERLINE	0x40000000
#define A_REVERSE	0x20000000
#define A_BLINK		0x10000000
#define A_DIM		0x08000000
#define A_BOLD		0x04000000

#define A_INVIS		0x02000000
#define A_PROTECT	0x01000000
#define A_ALTCHARSET	0x00800000

#define A_NORMAL	0x00000000
#define A_ATTRIBUTES	0xff800000
#define A_CHARTEXT	0x007fffff

#else

#define A_STANDOUT	0000200
#define A_UNDERLINE	0000400
#define A_REVERSE	0001000
#define A_BLINK		0002000
#define A_DIM		0004000
#define A_BOLD		0010000

/* The next three are subject to change (perhaps to colors) so don't
					depend on them */
#define A_INVIS		0020000
#define A_PROTECT	0040000
#define A_ALTCHARSET	0100000

#define A_NORMAL	0000000
#define A_ATTRIBUTES	0377600
#define A_CHARTEXT	0000177

#endif /* NLS */

#ifdef NLS
/* Funny "characters" enabled for various special function keys for input */
/*
 * Under NLS, we start their collation around 3500 octal (1856 decimal)
 * to permit room for NLS characters with large values
*/
#define KEY_BREAK	03501		/* break key (unreliable) */
#define KEY_DOWN	03502		/* The four arrow keys ... */
#define KEY_UP		03503
#define KEY_LEFT	03504
#define KEY_RIGHT	03505		/* ... */
#define KEY_HOME	03506		/* Home key (upward+left arrow) */
#define KEY_BACKSPACE	03507		/* backspace (unreliable) */
#define KEY_F0		03510		/* Function keys.  Space for 64 */
#define KEY_F(n)	(KEY_F0+(n))	/* keys is reserved. */
#define KEY_DL		03610		/* Delete line */
#define KEY_IL		03611		/* Insert line */
#define KEY_DC		03612		/* Delete character */
#define KEY_IC		03613		/* Insert char or enter insert mode */
#define KEY_EIC		03614		/* Exit insert char mode */
#define KEY_CLEAR	03615		/* Clear screen */
#define KEY_EOS		03616		/* Clear to end of screen */
#define KEY_EOL		03617		/* Clear to end of line */
#define KEY_SF		03620		/* Scroll 1 line forward */
#define KEY_SR          03621           /* Scroll 1 line backwards (reverse)*/
#define KEY_NPAGE	03622		/* Next page */
#define KEY_PPAGE	03623		/* Previous page */
#define KEY_STAB	03624		/* Set tab */
#define KEY_CTAB	03625		/* Clear tab */
#define KEY_CATAB	03626		/* Clear all tabs */
#define KEY_ENTER	03627		/* Enter or send (unreliable) */
#define KEY_SRESET      03630           /* soft (partial) reset (unreliable)*/
#define KEY_RESET	03631		/* reset or hard reset (unreliable) */
#define KEY_PRINT	03632		/* print or copy */
#define KEY_LL		03633		/* home down or bottom (lower left) */
					/* The keypad is arranged like this:*/
					/*    a1    up    a3   */
					/*   left   b2  right  */
					/*    c1   down   c3   */
#define KEY_A1		03634		/* upper left of keypad */
#define KEY_A3		03635		/* upper right of keypad */
#define KEY_B2		03636		/* center of keypad */
#define KEY_C1		03637		/* lower left of keypad */
#define KEY_C3		03640		/* lower right of keypad */
#define KEY_ACTION      03641           /* Action key            */
#else
/* Funny "characters" enabled for various special function keys for input */
#define KEY_BREAK	0401		/* break key (unreliable) */
#define KEY_DOWN	0402		/* The four arrow keys ... */
#define KEY_UP		0403
#define KEY_LEFT	0404
#define KEY_RIGHT	0405		/* ... */
#define KEY_HOME	0406		/* Home key (upward+left arrow) */
#define KEY_BACKSPACE	0407		/* backspace (unreliable) */
#define KEY_F0		0410		/* Function keys.  Space for 64 */
#define KEY_F(n)	(KEY_F0+(n))	/* keys is reserved. */
#define KEY_DL		0510		/* Delete line */
#define KEY_IL		0511		/* Insert line */
#define KEY_DC		0512		/* Delete character */
#define KEY_IC		0513		/* Insert char or enter insert mode */
#define KEY_EIC		0514		/* Exit insert char mode */
#define KEY_CLEAR	0515		/* Clear screen */
#define KEY_EOS		0516		/* Clear to end of screen */
#define KEY_EOL		0517		/* Clear to end of line */
#define KEY_SF		0520		/* Scroll 1 line forward */
#define KEY_SR          0521            /* Scroll 1 line backwards (reverse)*/
#define KEY_NPAGE	0522		/* Next page */
#define KEY_PPAGE	0523		/* Previous page */
#define KEY_STAB	0524		/* Set tab */
#define KEY_CTAB	0525		/* Clear tab */
#define KEY_CATAB	0526		/* Clear all tabs */
#define KEY_ENTER	0527		/* Enter or send (unreliable) */
#define KEY_SRESET      0530            /* soft (partial) reset (unreliable)*/
#define KEY_RESET	0531		/* reset or hard reset (unreliable) */
#define KEY_PRINT	0532		/* print or copy */
#define KEY_LL		0533		/* home down or bottom (lower left) */
					/* The keypad is arranged like this:*/
					/*    a1    up    a3   */
					/*   left   b2  right  */
					/*    c1   down   c3   */
#define KEY_A1		0534		/* upper left of keypad */
#define KEY_A3		0535		/* upper right of keypad */
#define KEY_B2		0536		/* center of keypad */
#define KEY_C1		0537		/* lower left of keypad */
#define KEY_C3		0540		/* lower right of keypad */
#define KEY_ACTION      0541            /* Action key            */
#endif /* NLS */

#endif /* NOMACROS */

#else	/* _BSD */

/*
 * BSD curses
 */


/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)curses.h	5.4 (Berkeley) 6/30/88
 */


# include	<stdio.h>
 
# include	<sgtty.h>

# define	bool	char
# define	reg	register

# define	TRUE	1
# define	FALSE	0
# define	ERR	(0)
# define	OK	(1)

# define	_ENDLINE	001
# define	_FULLWIN	002
# define	_SCROLLWIN	004
# define	_FLUSH		010
# define	_FULLLINE	020
# define	_IDLINE		040
# define	_STANDOUT	0200
# define	_NOCHANGE	-1

# define	_puts(s)	tputs(s, 0, _putchar)

typedef	struct sgttyb	SGTTY;

/*
 * Capabilities from termcap
 */

extern bool     AM, BS, CA, DA, DB, EO, HC, HZ, IN, MI, MS, NC, NS, OS, UL,
		XB, XN, XT, XS, XX;
extern char	*AL, *BC, *BT, *CD, *CE, *CL, *CM, *CR, *CS, *DC, *DL,
		*DM, *DO, *ED, *EI, *K0, *K1, *K2, *K3, *K4, *K5, *K6,
		*K7, *K8, *K9, *HO, *IC, *IM, *IP, *KD, *KE, *KH, *KL,
		*KR, *KS, *KU, *LL, *MA, *ND, *NL, *RC, *SC, *SE, *SF,
		*SO, *SR, *TA, *TE, *TI, *UC, *UE, *UP, *US, *VB, *VS,
		*VE, *AL_PARM, *DL_PARM, *UP_PARM, *DOWN_PARM,
		*LEFT_PARM, *RIGHT_PARM;
extern char	PC;

/*
 * From the tty modes...
 */

extern bool	GT, NONL, UPPERCASE, normtty, _pfast;

struct _win_st {
	short		_cury, _curx;
	short		_maxy, _maxx;
	short		_begy, _begx;
	short		_flags;
	short		_ch_off;
	bool		_clear;
	bool		_leave;
	bool		_scroll;
	char		**_y;
	short		*_firstch;
	short		*_lastch;
	struct _win_st	*_nextp, *_orig;
};

# define	WINDOW	struct _win_st

extern bool	My_term, _echoit, _rawmode, _endwin;

extern char	*Def_term, ttytype[];

extern int	LINES, COLS, _tty_ch, _res_flg;

extern SGTTY	_tty;

extern WINDOW	*stdscr, *curscr;

/*
 *	Define VOID to stop lint from generating "null effect"
 * comments.
 */
# ifdef lint
int	__void__;
# define	VOID(x)	(__void__ = (int) (x))
# else
# define	VOID(x)	(x)
# endif

/*
 * defines to hide all the BSD coded functions from the SYS V 
 * macros keep their names.
 */

#define _id_subwins	_bsd_id_subwins
#define _putchar	_bsd_putchar
#define _set_subwin_	_bsd_set_subwin_
#define _sscans		_bsd_sscans
#define _swflags_	_bsd_swflags_
#define box 		_bsd_box
#define delwin 		_bsd_delwin
#define endwin 		_bsd_endwin
#define fgoto		_bsd_fgoto
#define fullname	_bsd_fullname
#define getcap		_bsd_getcap
#define gettmode	_bsd_gettmode
#define idlok		_bsd_idlok
#define initscr 	_bsd_initscr
#define longname 	_bsd_longname
#define mvcur 		_bsd_mvcur
#define mvprintw	_bsd_mvprintw
#define mvscanw		_bsd_mvscanw
#define mvwin		_bsd_mvwin
#define mvwprintw	_bsd_mvwprintw
#define mvwscanw	_bsd_mvwscanw
#define newwin 		_bsd_newwin
#define overlay 	_bsd_overlay
#define overwrite 	_bsd_overwrite
#define plod		_bsd_plod
#define plodput		_bsd_plodput
#define printw 		_bsd_printw
#define scanw 		_bsd_scanw
#define scroll 		_bsd_scroll
#define setterm 	_bsd_setterm
#define subwin 		_bsd_subwin
#define tabcol		_bsd_tabcol
#define touchline	_bsd_touchline
#define touchoverlap	_bsd_touchoverlap
#define touchwin 	_bsd_touchwin
#define tstp		_bsd_tstp
#define waddbytes	_bsd_waddbytes
#define waddch 		_bsd_waddch
#define waddstr 	_bsd_waddstr
#define wclear 		_bsd_wclear
#define wclrtobot 	_bsd_wclrtobot
#define wclrtoeol 	_bsd_wclrtoeol
#define wdelch 		_bsd_wdelch
#define wdeleteln 	_bsd_wdeleteln
#define werase 		_bsd_werase
#define wgetch 		_bsd_wgetch
#define wgetstr 	_bsd_wgetstr
#define winsch 		_bsd_winsch
#define winsertln 	_bsd_winsertln
#define wmove 		_bsd_wmove
#define wprintw 	_bsd_wprintw
#define wrefresh 	_bsd_wrefresh
#define wscanw 		_bsd_wscanw
#define wstandend 	_bsd_wstandend
#define wstandout 	_bsd_wstandout
#define zap		_bsd_zap

/*
 * We must not let BSD programs use the termlib/termcap routines built
 * in to libcurses, so we pull the same trick of redefining all the
 * function calls.  These new routines are incorporated into libcurses,
 * where usualy they have remained seprate in libtermcap.
 * Note: These defines are duplicated in the curses library C modules.
 */

#define tfindent 	_bsd_tfindent
#define tgetent 	_bsd_tgetent
#define tgetflag	_bsd_tgetflag
#define tgetnum		_bsd_tgetnum
#define tgetstr		_bsd_tgetstr
#define tgetwinsize	_bsd_tgetwinsize
#define tnamatch	_bsd_tnamatch
#define tnchktc		_bsd_tnchktc
#define tgoto		_bsd_tgoto
#define tputs		_bsd_tputs

/*
 * psuedo functions for standard screen
 */
# define	addch(ch)	VOID(waddch(stdscr, ch))
# define	getch()		VOID(wgetch(stdscr))
# define	addbytes(da,co)	VOID(waddbytes(stdscr, da,co))
# define	addstr(str)	VOID(waddbytes(stdscr, str, strlen(str)))
# define	getstr(str)	VOID(wgetstr(stdscr, str))
# define	move(y, x)	VOID(wmove(stdscr, y, x))
# define	clear()		VOID(wclear(stdscr))
# define	erase()		VOID(werase(stdscr))
# define	clrtobot()	VOID(wclrtobot(stdscr))
# define	clrtoeol()	VOID(wclrtoeol(stdscr))
# define	insertln()	VOID(winsertln(stdscr))
# define	deleteln()	VOID(wdeleteln(stdscr))
# define	refresh()	VOID(wrefresh(stdscr))
# define	inch()		VOID(winch(stdscr))
# define	insch(c)	VOID(winsch(stdscr,c))
# define	delch()		VOID(wdelch(stdscr))
# define	standout()	VOID(wstandout(stdscr))
# define	standend()	VOID(wstandend(stdscr))

/*
 * mv functions
 */
#define	mvwaddch(win,y,x,ch)	VOID(wmove(win,y,x)==ERR?ERR:waddch(win,ch))
#define	mvwgetch(win,y,x)	VOID(wmove(win,y,x)==ERR?ERR:wgetch(win))
#define	mvwaddbytes(win,y,x,da,co) \
		VOID(wmove(win,y,x)==ERR?ERR:waddbytes(win,da,co))
#define	mvwaddstr(win,y,x,str) \
		VOID(wmove(win,y,x)==ERR?ERR:waddbytes(win,str,strlen(str)))
#define mvwgetstr(win,y,x,str)  VOID(wmove(win,y,x)==ERR?ERR:wgetstr(win,str))
#define	mvwinch(win,y,x)	VOID(wmove(win,y,x) == ERR ? ERR : winch(win))
#define	mvwdelch(win,y,x)	VOID(wmove(win,y,x) == ERR ? ERR : wdelch(win))
#define	mvwinsch(win,y,x,c)	VOID(wmove(win,y,x) == ERR ? ERR:winsch(win,c))
#define	mvaddch(y,x,ch)		mvwaddch(stdscr,y,x,ch)
#define	mvgetch(y,x)		mvwgetch(stdscr,y,x)
#define	mvaddbytes(y,x,da,co)	mvwaddbytes(stdscr,y,x,da,co)
#define	mvaddstr(y,x,str)	mvwaddstr(stdscr,y,x,str)
#define mvgetstr(y,x,str)       mvwgetstr(stdscr,y,x,str)
#define	mvinch(y,x)		mvwinch(stdscr,y,x)
#define	mvdelch(y,x)		mvwdelch(stdscr,y,x)
#define	mvinsch(y,x,c)		mvwinsch(stdscr,y,x,c)

/*
 * psuedo functions
 */

#define	clearok(win,bf)	 (win->_clear = bf)
#define	leaveok(win,bf)	 (win->_leave = bf)
#define	scrollok(win,bf) (win->_scroll = bf)
#define flushok(win,bf)	 (bf ? (win->_flags |= _FLUSH):(win->_flags &= ~_FLUSH))
#define	getyx(win,y,x)	 y = win->_cury, x = win->_curx
#define	winch(win)	 (win->_y[win->_cury][win->_curx] & 0177)

#define raw()	 (_tty.sg_flags|=RAW, _pfast=_rawmode=TRUE, stty(_tty_ch,&_tty))
#define noraw()	 (_tty.sg_flags&=~RAW,_rawmode=FALSE,_pfast=!(_tty.sg_flags&CRMOD),stty(_tty_ch,&_tty))
#define cbreak() (_tty.sg_flags |= CBREAK, _rawmode = TRUE, stty(_tty_ch,&_tty))
#define nocbreak() (_tty.sg_flags &= ~CBREAK,_rawmode=FALSE,stty(_tty_ch,&_tty))
#define crmode() cbreak()	/* backwards compatability */
#define nocrmode() nocbreak()	/* backwards compatability */
#define echo()	 (_tty.sg_flags |= ECHO, _echoit = TRUE, stty(_tty_ch, &_tty))
#define noecho() (_tty.sg_flags &= ~ECHO, _echoit = FALSE, stty(_tty_ch, &_tty))
#define nl()	 (_tty.sg_flags |= CRMOD,_pfast = _rawmode,stty(_tty_ch, &_tty))
#define nonl()	 (_tty.sg_flags &= ~CRMOD, _pfast = TRUE, stty(_tty_ch, &_tty))
#define	savetty() ((void) gtty(_tty_ch, &_tty), _res_flg = _tty.sg_flags)
#define	resetty() (_tty.sg_flags = _res_flg, (void) stty(_tty_ch, &_tty))

#define	erasechar()	(_tty.sg_erase)
#define	killchar()	(_tty.sg_kill)
#define baudrate()	(_tty.sg_ospeed)

#ifndef _NO_PROTO
extern WINDOW *initscr(void);
extern WINDOW *newwin(int, int, int, int);
extern WINDOW *subwin(WINDOW*, int, int, int, int);
extern char *longname(const char*, char*), *getcap(const char*);
#else
WINDOW	*initscr(), *newwin(), *subwin();
char	*longname(), *getcap();
#endif

/*
 * Used to be in unctrl.h.
 */
#define	unctrl(c)	_unctrl[(c) & 0177]
extern char *_unctrl[];

#endif /* _BSD */

#endif /* _CURSES_H_ */

