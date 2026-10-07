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
 * 08-Oct-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Moved iprintf to here, result was horible for MPs (cpu number in
 *	front of each iprintf.)
 *
 * $Log: db_output.c,v $
 * Revision 1.6  1994/11/18  20:29:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/07/12  19:17:22  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.4.8.3  1994/06/28  19:01:20  stans
 *   Added db_will_wrap(n) rtn to determine if the current db_output position
 *   plus 'n' will overflow the current db output line width max.
 *
 * Revision 1.4.8.2  1994/03/11  23:05:36  andyp
 * Corrected the use of varargs to be more portable.
 *
 * Revision 1.4.8.1  1994/02/23  21:00:42  rkl
 *  Added `resize' command to set debugger notion of window size based on
 *  current window size.
 *
 * Revision 1.4  1993/06/30  22:20:42  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:18:50  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:18:30  dleslie
 * First R1_0 release
 *
 * Revision 2.6  91/10/09  16:01:26  af
 * 	 Revision 2.5.2.1  91/10/05  13:06:46  jeffreyh
 * 	 	Added "more" like function.
 * 	 	[91/08/29            tak]
 * 
 * Revision 2.5.2.1  91/10/05  13:06:46  jeffreyh
 * 	Added "more" like function.
 * 	[91/08/29            tak]
 * 
 * Revision 2.5  91/07/09  23:15:54  danner
 * 	Include machine/db_machdep.c.
 * 	When db_printf is invoked, call db_printing on luna. This is used
 * 	 to trigger a screen clear. Under luna88k conditional.
 * 	[91/07/08            danner]
 *
 * Revision 2.4  91/05/14  15:34:51  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:06:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  16:18:41  mrt]
 * 
 * Revision 2.2  90/08/27  21:51:25  dbg
 * 	Put extra features of db_doprnt in _doprnt.
 * 	[90/08/20            dbg]
 * 	Reduce lint.
 * 	[90/08/07            dbg]
 * 	Created.
 * 	[90/07/25            dbg]
 * 
 */
/*
 * 	Author: David B. Golub, Carnegie Mellon University
 *	Date:	7/90
 */

/*
 * Printf and character output for debugger.
 */

#include <mach/boolean.h>
#include <sys/varargs.h>
#include <machine/db_machdep.h>
#include <ddb/db_lex.h>

/*
 *	Character output - tracks position in line.
 *	To do this correctly, we should know how wide
 *	the output device is - then we could zero
 *	the line position when the output device wraps
 *	around to the start of the next line.
 *
 *	Instead, we count the number of spaces printed
 *	since the last printing character so that we
 *	don't print trailing spaces.  This avoids most
 *	of the wraparounds.
 */

#ifndef	DB_MAX_LINE
#define	DB_MAX_LINE		24	/* maximum line */
#define DB_MAX_WIDTH		80	/* maximum width */
#endif	DB_MAX_LINE

#define DB_MIN_MAX_WIDTH	20	/* minimum max width */
#define DB_MIN_MAX_LINE		3	/* minimum max line */
#define CTRL(c)			((c) & 0xff)

int	db_output_position = 0;		/* output column */
int	db_output_line = 0;		/* output line number */
int	db_last_non_space = 0;		/* last non-space character */
int	db_tab_stop_width = 8;		/* how wide are tab stops? */
#define	NEXT_TAB(i) \
	((((i) + db_tab_stop_width) / db_tab_stop_width) * db_tab_stop_width)
int	db_max_line = DB_MAX_LINE;	/* output max lines */
int	db_max_width = DB_MAX_WIDTH;	/* output line width */

extern void	db_check_interrupt();

/*
 * Force pending whitespace.
 */
void
db_force_whitespace()
{
	register int last_print, next_tab;

	last_print = db_last_non_space;
	while (last_print < db_output_position) {
	    next_tab = NEXT_TAB(last_print);
	    if (next_tab <= db_output_position) {
		cnputc('\t');
		last_print = next_tab;
	    }
	    else {
		cnputc(' ');
		last_print++;
	    }
	}
	db_last_non_space = db_output_position;
}

static void
db_more()
{
	register  char *p;
	boolean_t quit_output = FALSE;

	for (p = "--db_more--"; *p; p++)
	    cnputc(*p);
	switch(cngetc()) {
	case ' ':
	    db_output_line = 0;
	    break;
	case 'q':
	case CTRL('c'):
	    db_output_line = 0;
	    quit_output = TRUE;
	    break;
	default:
	    db_output_line--;
	    break;
	}
	p = "\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b";
	while (*p)
	    cnputc(*p++);
	if (quit_output) {
	    db_error(0);
	    /* NOTREACHED */
	}
}

/*
 * Output character.  Buffer whitespace.
 */
db_putchar(c)
	int	c;		/* character to output */
{
	if (db_max_line >= DB_MIN_MAX_LINE && db_output_line >= db_max_line-1)
	    db_more();
	if (c > ' ' && c <= '~') {
	    /*
	     * Printing character.
	     * If we have spaces to print, print them first.
	     * Use tabs if possible.
	     */
	    db_force_whitespace();
	    cnputc(c);
	    db_output_position++;
	    if (db_max_width >= DB_MIN_MAX_WIDTH
		&& db_output_position >= db_max_width-1) {
		/* auto new line */
		cnputc('\n');
		db_output_position = 0;
		db_last_non_space = 0;
		db_output_line++;
	    }
	    db_last_non_space = db_output_position;
	}
	else if (c == '\n') {
	    /* Return */
	    cnputc(c);
	    db_output_position = 0;
	    db_last_non_space = 0;
	    db_output_line++;
	    db_check_interrupt();
	}
	else if (c == '\t') {
	    /* assume tabs every 8 positions */
	    db_output_position = NEXT_TAB(db_output_position);
	}
	else if (c == ' ') {
	    /* space */
	    db_output_position++;
	}
	else if (c == '\007') {
	    /* bell */
	    cnputc(c);
	}
	/* other characters are assumed non-printing */
}

/*
 * Return output position
 */
int
db_print_position()
{
	return (db_output_position);
}

/*
 * End line if too long.
 */
void
db_end_line()
{
	if (db_output_position >= db_max_width-1)
	    db_printf("\n");
}

/*
 * if output position is advanced by 'n' will it overflow the max width?
 */
boolean_t
db_will_wrap(n)
	int	n;
{
	register boolean_t	rc = FALSE;

	if ( (db_output_position+n) >= (db_max_width-1) )
		rc = TRUE;
	return rc;
}

/*
 * Printing
 */
extern		_doprnt();
extern int	db_radix;

/*VARARGS1*/
db_printf(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

#ifdef	luna88k
	db_printing();
#endif
	va_start(listp);
	fmt = va_arg(listp, char *);
	_doprnt(fmt, &listp, db_putchar, db_radix);
	va_end(listp);
}

/* alternate name */

/*VARARGS1*/
kdbprintf(va_alist)
	va_dcl
{
	char	*fmt;
	va_list	listp;

	va_start(listp);
	fmt = va_arg(listp, char *);
	_doprnt(fmt, &listp, db_putchar, db_radix);
	va_end(listp);
}

int	indent = 0;

/*
 * Printing (to console) with indentation.
 */
/*VARARGS1*/
iprintf(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;
	register int i;

	for (i = indent; i > 0; ){
	    if (i >= 8) {
		kdbprintf("\t");
		i -= 8;
	    }
	    else {
		kdbprintf(" ");
		i--;
	    }
	}
	va_start(listp);
	fmt = va_arg(listp, char *);
	_doprnt(fmt, &listp, db_putchar, db_radix);
	va_end(listp);
}

#define	X_WIN		0
#define	SUN_WIN		1

static char *size_fmts[] = {
	"\033[%d;%dR",		/* xterm */
	"\033[8;%d;%dt",	/* sun   */
};

static char *get_size_strs[] = {
	"\0337\033[r\033[999;999H\033[6n",	/* xterm */
	"\033[18t",				/* sun   */
};

db_resize_cmd()
{
	extern	int	db_max_line, db_max_width;

	char	buf[16];
	char	*resp = buf;
	char	*str = get_size_strs[X_WIN];	/* default to xterm */
	char	*fmt = size_fmts[X_WIN];	/*    ditto         */
	char	*resize_error = "Bad resize response\n";
	char	*end;
	int	rows, cols;
	int	safety;

	/*
	 *  See if they have specified the window type.
	 */
	if (db_read_token() == tIDENT) {
		if (!strncmp(db_tok_string, "sun", strlen(db_tok_string))) {
			str = get_size_strs[SUN_WIN];
			fmt = size_fmts[SUN_WIN];
		} else {
			str = get_size_strs[X_WIN];
			fmt = size_fmts[X_WIN];
		}
	}

	/*
	 *  Get the window to tell us how big it is.  Note that the
	 *  db_print functions will strip the escapes.
	 */
	while (*str)
		cnputc(*str++);

	/*
	 *  Find the end of the expected input string.
	 */
	for (end = fmt; *end; end++)
		;
	end--;

	/*
	 *  get the response.
	 *  db_readline(buf, sizeof(buf));
	 * strips escapes and echos chars
	 */
	safety = sizeof(buf);
	do {
		*resp = cngetc();
		safety--;
	} while (safety && (*resp != '\n') && (*resp++ != *end));

	/*
	 *  Do a poor mans sscanf.
	 */
	resp = buf;
	while (*fmt && (*fmt == *resp)) {
		fmt++;
		resp++;
	}

	if (*fmt == 0) {
		db_printf(resize_error);
		return;
	}

	if (*fmt++ == '%' && *fmt++ == 'd') {
		rows = 0;
		while(1) {
			if (*resp == *fmt)
				break;
			if (*resp < '0' || *resp > '9') {
				db_printf(resize_error);
				return;
			}

			rows *= 10;
			rows += (*resp - 0x30);
			resp++;
		}
	} else {
		db_printf(resize_error);
		return;
	}

	while (*fmt && (*fmt == *resp)) {
		fmt++;
		resp++;
	}

	if (*fmt++ == '%' && *fmt++ == 'd') {
		cols = 0;
		while(1) {
			if (*resp == *fmt)
				break;
			if (*resp < '0' || *resp > '9') {
				db_printf(resize_error);
				return;
			}

			cols *= 10;
			cols += (*resp - 0x30);
			resp++;
		}
	} else {
		db_printf(resize_error);
		return;
	}

	/*
	 *  Keep things in bounds.
	 */
	if (cols < DB_MIN_MAX_WIDTH)
		cols = DB_MIN_MAX_WIDTH;
	if (rows < DB_MIN_MAX_LINE)
		rows = DB_MIN_MAX_LINE;

	db_printf("\nSetting $lines = %d $maxwidth = %d\n", rows, cols);
	db_max_line  = rows;
	db_max_width = cols;

	return (0);
}
