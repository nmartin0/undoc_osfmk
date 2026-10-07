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
static char rcsid[] = "@(#)$RCSfile: mkstemp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:12 $";
#endif
/*
 * FUNCTIONS: mkstemp
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * mkstemp.c	1.7  com/lib/c/gen,3.1,8943 9/8/89 10:12:56
 */

#include <sys/types.h>
#include <sys/file.h>

#ifdef _THREAD_SAFE
#include "lib_lock.h"

extern lib_lock_functions_t libc_lock_funcs;
#else
extern pid_t getpid();
#endif


int
mkstemp(as)
	char *as;
{
	register char *s;
	register unsigned long p;
	register unsigned long c;
	register int fd;
        static char letters[66] =
	    "abcdefghijklmnopqrstuvwxyz1234567890=_ABCDEFGHIJKLMNOPQRSTUVWXYZ:";


#ifdef _THREAD_SAFE
	p = (unsigned long)lib_thread_id(libc_lock_funcs);
#else
	p = (unsigned long)getpid();	/* u_long ensures 0-fill below */
#endif
	c = ((p & 0x40000000L) != 0);	/* 31st bit set? */

	s = as;

        if (!s || !*s) return(-1); /* oops */

	while (*s++)
		/* void */;
	s--;
	while ((--s >= as) && (*s == 'X')) {
		*s = letters[p & 0x3f];
		p >>= 6;
	}
	if (*(s++)) {
		*s = letters[c]; /* make 31st bit sorta significant */
		while ((fd = open(as, O_CREAT|O_EXCL|O_RDWR, 0600)) == -1) {
			*s = letters[++c];
			if (c > 64)
				return(-1);
		}
		return(fd);
	} else
		return(open(as, O_CREAT|O_EXCL|O_RDWR, 0600));
}
