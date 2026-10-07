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
static char rcsid[] = "@(#)$RCSfile: cmd6.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:48:00 $";
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
 * 	cmd6.c	3.15 (Berkeley) 6/29/88";
 */


#include "defs.h"
#include "string.h"
#include "char.h"

/*
 * Debugging commands.
 */

c_debug()
{
	register struct ww *w;

	if (!terse)
		wwputs(MSGSTR(DEBUG, "[m(smap) n(ns) o(os) s(string) v(nvis) w(win)]? "), cmdwin);
	wwcurtowin(cmdwin);
	while (wwpeekc() < 0)
		wwiomux();
	if (!terse)
		wwputc('\n', cmdwin);
	switch (wwgetc()) {
	case 'm':
		wwdumpsmap();
		break;
	case 'n':
		wwdumpns();
		break;
	case 'o':
		wwdumpos();
		break;
	case 's':
		debug_str();
		break;
	case 'v':
		if ((w = getwin()) != 0)
			wwdumpnvis(w);
		break;
	case 'w':
		if ((w = getwin()) != 0)
			wwdumpwin(w);
		break;
	default:
		wwbell();
	}
}

#ifdef STR_DEBUG
debug_str()
{
	register struct ww *w;
	struct string *s;

	if ((w = openiwin(wwnrow - 3, MSGSTR(DEBUG2, "Allocated Strings"))) == 0) {
		error(MSGSTR(DEBUG3, "Can't open string window: %s."), wwerror());
		return;
	}
	for (s = str_head.s_forw; s != &str_head; s = s->s_forw) {
		if (more(w, 0) == 2)
			goto out;
		wwprintf(w, "(0x%x)\t\"%s\"\n", s->s_data, s->s_data);
	}
	waitnl(w);
out:
	closeiwin(w);
}
#else
debug_str()
{
	error(MSGSTR(DEBUG4, "No string debugging."));
}
#endif
