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
static char rcsid[] = "@(#)$RCSfile: tttermcap.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:48:52 $";
#endif
/*
 * Copyright (c) 1983 Regents of the University of California.
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
 * 	tttermcap.c	3.9 (Berkeley) 6/29/88
 */


#include "tt.h"

char *tgetstr();
char *tgoto();
char *malloc();

tttputc(c)
{
	ttputc(c);
}

ttxputc(c)
{
	*tt_strp++ = c;
}

struct tt_str *
tttgetstr(str)
	char *str;
{
	register struct tt_str *s;

	if ((str = tgetstr(str, &tt_strp)) == 0)
		return 0;
	if ((s = (struct tt_str *) malloc(sizeof *s)) == 0)
		return 0;
	s->ts_str = str;
	s->ts_n = tt_strp - s->ts_str - 1;
	return s;
}

struct tt_str *
ttxgetstr(str)
	char *str;
{
	register struct tt_str *s;
	char buf[100];
	char *bufp = buf;

	if (tgetstr(str, &bufp) == 0)
		return 0;
	if ((s = (struct tt_str *) malloc(sizeof *s)) == 0)
		return 0;
	s->ts_str = tt_strp;
	tputs(buf, 1, ttxputc);
	s->ts_n = tt_strp - s->ts_str;
	*tt_strp++ = 0;
	return s;
}

tttgoto(s, col, row)
	struct tt_str *s;
{
	register char *p = s->ts_str;

	ttputs(tgoto(p, col, row));
	for (p += s->ts_n; *--p == 0;)
		ttputc(0);
}

ttstrcmp(a, b)
	register struct tt_str *a, *b;
{
	int n, r;

	if (r = bcmp(a->ts_str, b->ts_str,
			(n = a->ts_n - b->ts_n) < 0 ? a->ts_n : b->ts_n))
		return r;
	return n;
}
