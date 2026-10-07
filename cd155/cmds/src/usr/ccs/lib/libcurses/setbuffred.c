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
static char sccsid[] = "@(#)setbuffred.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:39:13";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _setbuffered
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

static short    baud_convert[] =
{
	0, 50, 75, 110, 135, 150, 200, 300, 600, 1200,
	1800, 2400, 4800, 9600, 19200, 38400
};

/*
 * NAME:        _setbuffered
 *
 * FUNCTION:
 *
 *      Force output to be buffered.
 *      Also figures out the baud rate.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      Grouped here because they are machine dependent.
 *      This routine is one of the main things
 *      in this level of curses that depends on the outside
 *      environment.
 */

_setbuffered(fd)
FILE *fd;
{
	char *sobuf;
	char *calloc();
	SGTTY   sg;

	sobuf = calloc(1, BUFSIZ);
	setbuf(fd, sobuf);

# ifdef USG
	ioctl (fileno (fd), TCGETA, &sg);
	SP->baud = sg.c_cflag&CBAUD ? baud_convert[sg.c_cflag&CBAUD] : 1200;
# else
	ioctl (fileno (fd), TIOCGETP, &sg);
	SP->baud = sg.sg_ospeed ? baud_convert[sg.sg_ospeed] : 1200;
# endif
}
