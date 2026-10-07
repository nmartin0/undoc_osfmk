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
static char *rcsid = "@(#)$RCSfile: ex_tty.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:06 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_tty.c
 *
 * FUNCTION: NCkpadd, NCkpboth, cost, countnum, fkey, gettmode, kpadd, kpboth,
 * setterm
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
 *  ex_tty.c  1.13  com/cmd/edit/vi,3.1,9013 3/13/90 10:40:34
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_tty.h"
#include <values.h>

#define CNOSTR ((char *) 0)

static void kpboth(), kpadd(), NCkpboth(), NCkpadd();

static NLchar allocspace[1024];
static NLchar *freespace;

/*
 * Terminal type initialization routines,
 * and calculation of flags at entry or after
 * a shell escape which may change them.
 */
static short GT;

gettmode()
{

	GT = 1;
	if (ioctl(2, TCGETA, &tty) < 0)
		return;
	if (ospeed != (tty.c_cflag & CBAUD))	/* mjm */
		value(SLOWOPEN) = (tty.c_cflag & CBAUD) < B1200;
	ospeed = tty.c_cflag & CBAUD;
	normf = tty;
	UPPERCASE = (tty.c_iflag & IUCLC) != 0;
	if ((tty.c_oflag & TABDLY) == TAB3 || teleray_glitch)
		GT = 0;
	NONL = (tty.c_oflag & ONLCR) == 0;
}

setterm(type)
	char *type;
{
	register int unknown, i;
	register int l;
	int errret;
	extern char termtype[];

	unknown = 0;
	if (cur_term && exit_ca_mode)
		putpad(exit_ca_mode);
	cur_term = NULL;
	strcpy(termtype, type);
#ifdef TRACE
	if (trace) fprintf(trace, "before setupterm & ioctl, termtype %s ,lines %d, columns %d, clear_screen '%s', cursor_address '%s'\n", termtype, lines, columns, clear_screen, cursor_address);
#endif
	setupterm(type, 2, &errret);
	switch(errret) {
	    case -1:			/* Terminfo database not found */
		merror(msg(M_513, "Cannot locate Terminfo database."));
		unknown++;
		/* FALLTHROUGH */
	    case 0:			/* Terminal description not found */
		if (strcmp(type, "dumb") != 0) unknown++;
		cur_term = NULL;
		setupterm("unknown", 1, &errret);
		break;
	    case 1:			/* terminal description found */
		break;
	}
	resetterm();
#ifdef TRACE
	if (trace) fprintf(trace, "after setupterm, termtype %s, lines %d, columns %d, clear_screen '%s', cursor_address '%s'\n", termtype, lines, columns, clear_screen, cursor_address);
#endif
	setsize();
	/*
	 * Initialize keypad arrow keys.
	 */
	freespace = allocspace;

	kpadd(arrows, key_ic, "i", "inschar");
	kpadd(immacs, key_ic, "\033", "inschar");
	kpadd(arrows, key_eic, "i", "inschar");
	kpadd(immacs, key_eic, "\033", "inschar");

	kpboth(arrows, immacs, key_up, "k", "up");
	kpboth(arrows, immacs, key_down, "j", "down");
	kpboth(arrows, immacs, key_left, "h", "left");
	kpboth(arrows, immacs, key_right, "l", "right");
	kpboth(arrows, immacs, key_home, "H", "home");
	kpboth(arrows, immacs, key_il, "o\033", "insline");
	kpboth(arrows, immacs, key_dl, "dd", "delline");
	kpboth(arrows, immacs, key_clear, "\014", "clear");
	kpboth(arrows, immacs, key_eol, "d$", "clreol");
	kpboth(arrows, immacs, key_sf, "\005", "scrollf");
	kpboth(arrows, immacs, key_dc, "x", "delchar");
	kpboth(arrows, immacs, key_npage, "\006", "npage");
	kpboth(arrows, immacs, key_ppage, "\002", "ppage");
	kpboth(arrows, immacs, key_sr, "\031", "sr");
	kpboth(arrows, immacs, key_eos, "dG", "clreos");

	/*
	 * Handle funny termcap capabilities
	 */
	if (change_scroll_region && save_cursor && restore_cursor) insert_line=delete_line="";
	if (parm_insert_line && insert_line==NULL) insert_line="";
	if (parm_delete_line && delete_line==NULL) delete_line="";
	if (insert_character && enter_insert_mode==NULL) enter_insert_mode="";
	if (insert_character && exit_insert_mode==NULL) exit_insert_mode="";
	if (GT == 0)
		tab = back_tab = CNOSTR;

#ifdef SIGTSTP
	/*
	 * Now map users susp char to ^Z, being careful that the susp
	 * overrides any arrow key, but only for hackers (=new tty driver).
	 */
	{
		static NLchar sc[2];
		int i, l, fnd; /* l added to avoid auto init warning */

#ifdef NTTYDISC
		ioctl(2, TIOCGETD, &ldisc);
#endif
		if (!value(NOVICE)) {
			sc[0] = olttyc.t_suspc;
			sc[1] = 0;
			if (olttyc.t_suspc == Ctrl('Z')) {
				for (i=0; i<=4; i++)
					if (arrows[i].cap != NULL &&
					    arrows[i].cap[0] == Ctrl('Z'))
						addmac(sc, NULL, NULL, arrows);
			} else
				addmac(sc, "\32", "susp", arrows);
		}
	}
#endif

#ifdef TRACE
		if (trace) fprintf(trace, "before OOPS , termtype %s ,lines %d, columns %d, clear_screen '%s', cursor_address '%s'\n", termtype, lines, columns, clear_screen, cursor_address);
#endif
	costCM = cost(tparm(cursor_address, 10, 8));
	if (costCM >= 10000)
		cursor_address = NULL;
	costSR = cost(scroll_reverse);
	costAL = cost(insert_line);
	costDP = cost(tparm(parm_down_cursor, 10));
	costLP = cost(tparm(parm_left_cursor, 10));
	costRP = cost(tparm(parm_right_cursor, 10));
	costCE = cost(clr_eol);
	costCD = cost(clr_eos);
	/* proper strings to change tty type */
	termreset();
	gettmode();
	value(REDRAW) = insert_line && delete_line;
	value(OPTIMIZE) = !cursor_address && !tab;
	if (ospeed == B1200 && !value(REDRAW))
		value(SLOWOPEN) = 1;	/* see also gettmode above */
	if (unknown)
		serror(msg(M_202, "%s: Unknown terminal type"), type);
#ifdef TRACE
		if (trace) fprintf(trace, "exit setterm , termtype %s ,lines %d, columns %d, clear_screen '%s', cursor_address '%s'\n", termtype, lines, columns, clear_screen, cursor_address);
#endif
}

setsize()
{
	register int l, i;
	int olines = lines, ocols = columns; /* NO clobbering by bad ioctl */

#ifdef TIOCGWINSZ
	struct winsize win;

	if (ioctl(0, TIOCGWINSZ, &win) < 0) {
#endif
		i = lines = tgetnum("li");
		columns = tgetnum("co");
#ifdef TIOCGWINSZ
	} else {
		if ((lines = winsz.ws_row = win.ws_row) == 0)
			lines = tgetnum("li");
		i = lines;
		if ((columns = winsz.ws_col = win.ws_col) == 0)
			columns = tgetnum("co");
	}
#endif
	if (!lines && olines)	/* Prevent clobbering by bad ioctl */
		i = lines = olines;
	if (!columns && ocols)
		columns = ocols;
	if (lines <= 5)
		lines = 24;
	if (lines > TUBELINES)
		lines = TUBELINES;
	l = lines;
	if (ospeed < B1200)
		l = 9;	/* including the message line at the bottom */
	else if (ospeed < B2400)
		l = 17;
	if (l > lines)
		l = lines;
	if (columns <= 4)
		columns = 1000;
	value(WINDOW) = options[WINDOW].odefault = l - 1;
	if (defwind) value(WINDOW) = defwind;
	value(SCROLL) = (options[SCROLL].odefault =
		hard_copy ? 11 : (value(WINDOW) / 2));
	if (i <= 0)
		lines = 2;
}

static void kpboth(m1, m2, k, m, d)
struct maps *m1, *m2;
char *k, *m, *d;
{
	if (k) {
		NLchar *key, *mapto, *descr;
		key = NCstrcpy2(freespace, k);
		freespace += NCstrlen(freespace)+1;
		mapto = NCstrcpy2(freespace, m);
		freespace += NCstrlen(freespace)+1;
		descr = NCstrcpy2(freespace, d);
		freespace += NCstrlen(freespace)+1;
		NCkpboth(m1, m2, key, mapto, descr);
	}
}

static void kpadd(map, k, m, d)
struct maps *map;
char *k, *m, *d;
{
	if (k) {
		NLchar *key, *mapto, *descr;
		key = NCstrcpy2(freespace, k);
		freespace += NCstrlen(freespace)+1;
		mapto = NCstrcpy2(freespace, m);
		freespace += NCstrlen(freespace)+1;
		descr = NCstrcpy2(freespace, d);
		freespace += NCstrlen(freespace)+1;
		NCkpadd(map, key, mapto, descr);
	}
}

/*
 * Map both map1 and map2 as below.  map2 surrounded by esc and a.
 */
static void NCkpboth(map1, map2, key, mapto, desc)
struct maps *map1, *map2;
NLchar *key, *mapto, *desc;
{
	NLchar *p;

	NCkpadd(map1, key, mapto, desc);
	if (any(*key, "\b\n "))
		return;
	p = freespace;
	*freespace++ = '\033';
	NCstrcpy(freespace, mapto);
	freespace = NCstrend(freespace);
	*freespace++ = 'a';
	*freespace++ = '\0';
	NCkpadd(map2, key, p, desc);
}

/*
 * Define a macro.  mapstr is the structure (mode) in which it applies.
 * key is the input sequence, mapto what it turns into, and desc is a
 * human-readable description of what's going on.
 */
static void NCkpadd(mapstr, key, mapto, desc)
struct maps *mapstr;
NLchar *key, *mapto, *desc;
{
	int i;

	for (i=0; i<MAXNOMACS; i++)
		if (mapstr[i].cap == 0)
			break;
	if (i >= MAXNOMACS)
		return;
	mapstr[i].cap = key;
	mapstr[i].mapto = mapto;
	mapstr[i].descr = desc;
}

char *
fkey(i)
	int i;
{
	switch (i) {
	case 0: return key_f0;
	case 1: return key_f1;
	case 2: return key_f2;
	case 3: return key_f3;
	case 4: return key_f4;
	case 5: return key_f5;
	case 6: return key_f6;
	case 7: return key_f7;
	case 8: return key_f8;
	case 9: return key_f9;
	default: return CNOSTR;
	}
}

/*
 * cost figures out how much (in characters) it costs to send the string
 * str to the terminal.  It takes into account padding information, as
 * much as it can, for a typical case.	(Right now the typical case assumes
 * the number of lines affected is the size of the screen, since this is
 * mainly used to decide if insert_line or scroll_reverse is better, and
 * this always happens at the top of the screen.  We assume cursor motion
 * (cursor_address) has little * padding, if any, required, so that case,
 * which is really more important than insert_line vs scroll_reverse,
 * won't be really affected.)
 */
static int costnum;
cost(str)
char *str;
{
	int countnum();

	if (str == NULL || *str=='O')	/* OOPS */
		return 10000;	/* infinity */
	costnum = 0;
	tputs(str, lines, countnum);
	return costnum;
}

/* ARGSUSED */
countnum(ch)
char ch;
{
	costnum++;
}
