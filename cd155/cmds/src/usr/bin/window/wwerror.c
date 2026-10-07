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
static char rcsid[] = "@(#)$RCSfile: wwerror.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:49:27 $";
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
 * 	wwerror.c	3.5 (Berkeley) 6/29/88
 */


#include "defs.h"

char *
wwerror()
{
	extern errno;
	extern char *sys_errlist[];

	switch (wwerrno) {
	case WWE_NOERR:
		return (MSGSTR(NOERROR, "No error"));
	case WWE_SYS:
		return sys_errlist[errno];
	case WWE_NOMEM:
		return (MSGSTR(OUTOFMEM, "Out of memory"));
	case WWE_TOOMANY:
		return (MSGSTR(TOOMANYWIN, "Too many windows"));
	case WWE_NOPTY:
		return (MSGSTR(NOPTTYS, "Out of pseudo-terminals"));
	case WWE_SIZE:
		return (MSGSTR(BADSIZE, "Bad window size"));
	case WWE_BADTERM:
		return (MSGSTR(BADTYPE, "Unknown terminal type"));
	case WWE_CANTDO:
		return (MSGSTR(NODICE, "Can't run window on this terminal"));
	default:
		return (MSGSTR(UNKNOWNE, "Unknown error"));
	}
}
