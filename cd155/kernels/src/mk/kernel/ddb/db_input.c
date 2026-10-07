/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * 02-Oct-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Fixed history management: Do not store repeated typed
 *	command. Null terminate current command in case it is a
 *	substring of the last command.
 *
 * $Log: db_input.c,v $
 * Revision 1.5  1994/11/18  20:29:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:20:35  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:18:41  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:19  dleslie
 * First R1_0 release
 *
 * Revision 2.7.3.2  92/09/15  17:14:26  jeffreyh
 * 	Fixed history code. (Only one char. out of 2 was checked to
 * 	compare to last command)
 * 	[barbou@gr.osf.org]
 * 
 * Revision 2.7.3.1  92/03/03  16:13:30  jeffreyh
 * 	Pick up changes from TRUNK
 * 	[92/02/26  10:59:36  jeffreyh]
 * 
 * Revision 2.8  92/02/19  15:07:44  elf
 * 	Added delete_line (Ctrl-U).
 * 	[92/02/17            kivinen]
 * 
 * 	Added command line history. Ctrl-P = previous, Ctrl-N = next. If
 * 	DB_HISTORY_SIZE is 0 then command history is disabled.
 * 	[92/02/17            kivinen]
 * 
 * Revision 2.7  91/10/09  16:00:03  af
 * 	 Revision 2.6.2.1  91/10/05  13:06:12  jeffreyh
 * 	 	Fixed incorrect db_lbuf_end setting.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.6.2.1  91/10/05  13:06:12  jeffreyh
 * 	Fixed incorrect db_lbuf_end setting.
 * 	[91/08/29            tak]
 * 
 * Revision 2.6  91/07/09  23:15:49  danner
 * 	Add include of machine/db_machdep.h to allow machine-specific
 * 	 overrides via defines.
 * 	[91/07/08            danner]
 *
 * Revision 2.5  91/05/14  15:34:03  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/14  14:41:53  mrt
 * 	Add input line editing.
 * 	[90/11/11            dbg]
 * 
 * Revision 2.3  91/02/05  17:06:32  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:18:13  mrt]
 * 
 * Revision 2.2  90/08/27  21:51:03  dbg
 * 	Reduce lint.
 * 	[90/08/07            dbg]
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */
#include <mach/boolean.h>
#include <machine/db_machdep.h>
#include <ddb/db_output.h>

#ifndef DB_HISTORY_SIZE
#define DB_HISTORY_SIZE 4000
#endif DB_HISTORY_SIZE

/*
 * Character input and editing.
 */

/*
 * We don't track output position while editing input,
 * since input always ends with a new-line.  We just
 * reset the line position at the end.
 */
char *	db_lbuf_start;	/* start of input line buffer */
char *	db_lbuf_end;	/* end of input line buffer */
char *	db_lc;		/* current character */
char *	db_le;		/* one past last character */
#if DB_HISTORY_SIZE != 0
char    db_history[DB_HISTORY_SIZE];	/* start of history buffer */
int     db_history_size = DB_HISTORY_SIZE;/* size of history buffer */
char *  db_history_curr = db_history;	/* start of current line */
char *  db_history_last = db_history;	/* start of last line */
char *  db_history_prev = (char *) 0;	/* start of previous line */
#endif
	
#define	CTRL(c)		((c) & 0x1f)
#define	isspace(c)	((c) == ' ' || (c) == '\t')
#define	BLANK		' '
#define	BACKUP		'\b'

void
db_putstring(s, count)
	char	*s;
	int	count;
{
	while (--count >= 0)
	    cnputc(*s++);
}

void
db_putnchars(c, count)
	int	c;
	int	count;
{
	while (--count >= 0)
	    cnputc(c);
}

/*
 * Delete N characters, forward or backward
 */
#define	DEL_FWD		0
#define	DEL_BWD		1
void
db_delete(n, bwd)
	int	n;
	int	bwd;
{
	register char *p;

	if (bwd) {
	    db_lc -= n;
	    db_putnchars(BACKUP, n);
	}
	for (p = db_lc; p < db_le-n; p++) {
	    *p = *(p+n);
	    cnputc(*p);
	}
	db_putnchars(BLANK, n);
	db_putnchars(BACKUP, db_le - db_lc);
	db_le -= n;
}

void
db_delete_line()
{
	db_delete(db_le - db_lc, DEL_FWD);
	db_delete(db_lc - db_lbuf_start, DEL_BWD);
	db_le = db_lc = db_lbuf_start;
}

#if DB_HISTORY_SIZE != 0
#define INC_DB_CURR() \
    do { \
	     db_history_curr++; \
	     if (db_history_curr > \
		 db_history + db_history_size - 1) \
		     db_history_curr = db_history; \
       } while (0)
#define DEC_DB_CURR() \
    do { \
	     db_history_curr--; \
	     if (db_history_curr < db_history) \
		 db_history_curr = db_history + \
		 db_history_size - 1; \
       } while (0)
#endif
		
/* returns TRUE at end-of-line */
boolean_t
db_inputchar(c)
	int	c;
{
	switch (c) {
	    case CTRL('b'):
		/* back up one character */
		if (db_lc > db_lbuf_start) {
		    cnputc(BACKUP);
		    db_lc--;
		}
		break;
	    case CTRL('f'):
		/* forward one character */
		if (db_lc < db_le) {
		    cnputc(*db_lc);
		    db_lc++;
		}
		break;
	    case CTRL('a'):
		/* beginning of line */
		while (db_lc > db_lbuf_start) {
		    cnputc(BACKUP);
		    db_lc--;
		}
		break;
	    case CTRL('e'):
		/* end of line */
		while (db_lc < db_le) {
		    cnputc(*db_lc);
		    db_lc++;
		}
		break;
	    case CTRL('h'):
	    case 0177:
		/* erase previous character */
		if (db_lc > db_lbuf_start)
		    db_delete(1, DEL_BWD);
		break;
	    case CTRL('d'):
		/* erase next character */
		if (db_lc < db_le)
		    db_delete(1, DEL_FWD);
		break;
	    case CTRL('k'):
		/* delete to end of line */
		if (db_lc < db_le)
		    db_delete(db_le - db_lc, DEL_FWD);
		break;
	    case CTRL('u'):
		/* delete line */
	        db_delete_line();
		break;
	    case CTRL('t'):
		/* twiddle last 2 characters */
		if (db_lc >= db_lbuf_start + 2) {
		    c = db_lc[-2];
		    db_lc[-2] = db_lc[-1];
		    db_lc[-1] = c;
		    cnputc(BACKUP);
		    cnputc(BACKUP);
		    cnputc(db_lc[-2]);
		    cnputc(db_lc[-1]);
		}
		break;
#if DB_HISTORY_SIZE != 0
	    case CTRL('p'):
	        DEC_DB_CURR();
	        while (db_history_curr != db_history_last) {
			DEC_DB_CURR();
			if (*db_history_curr == '\0')
			    break;
		}
		db_delete_line();
		if (db_history_curr == db_history_last) {
			INC_DB_CURR();
			db_le = db_lc = db_lbuf_start;
		} else {
			register char *p;
			INC_DB_CURR();
			for (p = db_history_curr, db_le = db_lbuf_start;
			     *p; ) {
				*db_le++ = *p++;
				if (p == db_history + db_history_size) {
					p = db_history;
				}
			}
			db_lc = db_le;
			*db_le = '\0';
		}
		db_putstring(db_lbuf_start, db_le - db_lbuf_start);
		break;
	    case CTRL('n'):
	        while (db_history_curr != db_history_last) {
			if (*db_history_curr == '\0')
			    break;
			INC_DB_CURR();
		}
		if (db_history_curr != db_history_last) {
			INC_DB_CURR();
			db_delete_line();
			if (db_history_curr != db_history_last) {
				register char *p;
				for (p = db_history_curr,
				     db_le = db_lbuf_start; *p;) {
					*db_le++ = *p++;
					if (p == db_history +
					    db_history_size) {
						p = db_history;
					}
				}
				db_lc = db_le;
				*db_le = '\0';
			}
			db_putstring(db_lbuf_start, db_le - db_lbuf_start);
		}
		break;
#endif
	    case CTRL('r'):
		db_putstring("^R\n", 3);
		if (db_le > db_lbuf_start) {
			db_putstring(db_lbuf_start, db_le - db_lbuf_start);
			db_putnchars(BACKUP, db_le - db_lc);
		}
		break;
	    case '\n':
	    case '\r':
#if DB_HISTORY_SIZE != 0
		/* Check if it same than previous line */
		if (db_history_prev) {
			register char *pp, *pc;

			/* Is it unmodified */
			for (pp = db_history_prev, pc = db_lbuf_start;
			     pc != db_le && *pp;) {
				if (*pp != *pc)
				    break;
				if (++pp == db_history + db_history_size) {
					pp = db_history;
				}
				if (++pc == db_history + db_history_size) {
					pc = db_history;
				}
			}
			if (!*pp && pc == db_le) {
				/* Repeted previous line, not saved */
				db_history_curr = db_history_last;
				*db_le++ = c;
				return (TRUE);
			}
		}
		if (db_le != db_lbuf_start) {
			register char *p;
			db_history_prev = db_history_last;
			for (p = db_lbuf_start; p != db_le; p++) {
				*db_history_last++ = *p;
				if (db_history_last == db_history +
				    db_history_size) {
					db_history_last = db_history;
				}
			}
			*db_history_last++ = '\0';
		}
		db_history_curr = db_history_last;
#endif
		*db_le++ = c;
		return (TRUE);
	    default:
		if (db_le == db_lbuf_end) {
		    cnputc('\007');
		}
		else if (c >= ' ' && c <= '~') {
		    register char *p;

		    for (p = db_le; p > db_lc; p--)
			*p = *(p-1);
		    *db_lc++ = c;
		    db_le++;
		    cnputc(c);
		    db_putstring(db_lc, db_le - db_lc);
		    db_putnchars(BACKUP, db_le - db_lc);
		}
		break;
	}
	return (FALSE);
}

int
db_readline(lstart, lsize)
	char *	lstart;
	int	lsize;
{
	db_force_whitespace();	/* synch output position */

	db_lbuf_start = lstart;
	db_lbuf_end   = lstart + lsize - 1;
	db_lc = lstart;
	db_le = lstart;

	while (!db_inputchar(cngetc()))
	    continue;

	db_putchar('\n');	/* synch output position */

	*db_le = 0;
	return (db_le - db_lbuf_start);
}

void
db_check_interrupt()
{
	register int	c;

	c = cnmaygetc();
	switch (c) {
	    case -1:		/* no character */
		return;

	    case CTRL('c'):
		db_error((char *)0);
		/*NOTREACHED*/

	    case CTRL('s'):
		do {
		    c = cnmaygetc();
		    if (c == CTRL('c'))
			db_error((char *)0);
		} while (c != CTRL('q'));
		break;

	    default:
		/* drop on floor */
		break;
	}
}

