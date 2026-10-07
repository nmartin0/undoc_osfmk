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
/*
 * Copyright (c) 1988-90 SecureWare, Inc.
 *   All rights reserved.
 *
 * routines that deal with column counts for printouts
 */

#ident "@(#)printbuf.c	2.1 16:18:09 4/20/90 SecureWare"
/*
 * Based on:
 *   "@(#)printbuf.c	2.2.1.3 23:48:06 1/8/90 SecureWare"
 */

#include <sys/types.h>

static int num_cols;

extern char *getenv();
extern int strcspn();
extern int strspn();
#ifndef _OSF_SOURCE
extern int columns;	/* from curses */
#endif

static void
set_num_cols()
{
	char *cptr;
#ifndef _OSF_SOURCE
	int curses_ret;
#endif

	/* figure out how many columns */
	if ((cptr = getenv ("COLUMNS")) != (char *) 0)
		num_cols = atoi (cptr);
	else {
#ifndef _OSF_SOURCE
		setupterm ((char *) 0, 1, &curses_ret);
		if (curses_ret == 1) {
			num_cols = columns;
			resetterm ();
		}
		else
#endif
			num_cols = 80;
	}
}

/* print a character string on stdout, making sure we don't overflow the
 * column count.
 */

void
printbuf (buf, col, separators)
char *buf;
int  col;
char *separators;
{
	int	cur_col;	/* column counter */
	int	count;		/* number of characters fitting condition */
	char	*pbuf = buf;	/* first character to print next time */
	int	column ;

	if (num_cols == 0)
		set_num_cols();

	column = col ;
	count  = strcspn(buf, separators) ;
	if ((column + count + 1) >= num_cols && (num_cols / 3) < column - 10) {

		/*
		 * If the first "word" doesn't fit on the line given
		 * the specified indent and the specified indent is
		 * 10 or more characters greater than 1/3 of the total
		 * line length, we start on a new line with a smaller
		 * indent.
		 */

		putchar('\n') ;
		column = num_cols / 3 ;
		printf("%*s", column, "") ;
	}

	for (cur_col = column + 1; *buf != '\0'; ) {
		count = strcspn (buf, separators);	
		/* if the next field causes us to overflow, spit out what
		 * we've got so far.
		 */
		if (count + cur_col >= num_cols) {
			/* If we have a field that doesn't fit in the
			 * available space, just print what we can.
			 * Otherwise we would loop forever.
			 */
			if (pbuf == buf) {
				printf("%*s\\", num_cols - cur_col, pbuf);
				pbuf += num_cols - cur_col;
				buf = pbuf;
			} else {
				for (; pbuf < buf; pbuf++)
					putchar (*pbuf);
			}
			putchar ('\n');
			printf ("%*s", column, "");
			cur_col = column + 1;
			/* skip over leading whitespace on the next line */
			count = strspn (buf, " ");
			pbuf += count;
			buf += count;
		} else {
			cur_col += count;
			buf += count;
			if (*buf) {
				cur_col++;
				buf++;
			}
		}
	}
	if (*pbuf != '\0')
		printf ("%s\n", pbuf);
}
