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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)flushinp.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:18:13";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   flushinp
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
 */

#include "cursesext.h"

/*
 * NAME:        flushinp
 */

flushinp()
{
#ifdef DEBUG
	if(outf) fprintf(outf,
		"flushinp(), file %x, SP %x\n", SP->term_file, SP);
#endif
#ifdef USG
	ioctl(cur_term -> Filedes, TCFLSH, 0);
#else
	/* for insurance against someone using their own buffer: */
	ioctl(cur_term -> Filedes, TIOCGETP, &(cur_term->Nttyb));

	/*
	 * SETP waits on output and flushes input as side effect.
	 * Really want an ioctl like TCFLSH but Berkeley doesn't have one.
	 */
	ioctl(cur_term -> Filedes, TIOCSETP, &(cur_term->Nttyb));
#endif
	/*
	 * Have to doupdate() because, if we've stopped output due to
	 * typeahead, now that typeahead is gone, so we'd better catch up.
	 */
	doupdate();
}
