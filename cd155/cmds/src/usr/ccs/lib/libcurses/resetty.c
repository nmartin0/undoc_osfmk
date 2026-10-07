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
static char sccsid[] = "@(#)resetty.c  1.6  com/lib/curses,3.1,9008 11/14/89 22:38:31";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   resetty
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
 * NAME:        resetty
 */

resetty()
{
#ifdef USG
	if (SP == NULL || (SP->save_tty_buf.c_cflag&CBAUD) == 0)
		return;	/* Never called savetty */
#else
	if (SP == NULL || SP->save_tty_buf.sg_ospeed == 0)
		return;	/* Never called savetty */
#endif
	cur_term->Nttyb = SP->save_tty_buf;
#ifdef DEBUG
# ifdef USG
	if(outf) fprintf(outf,
		"savetty(), file %x, SP %x, flags %x,%x,%x,%x\n",
		SP->term_file, SP, cur_term->Nttyb.c_iflag,
		cur_term->Nttyb.c_oflag, cur_term->Nttyb.c_cflag,
		cur_term->Nttyb.c_lflag);
# else
	if(outf) fprintf(outf,
		"resetty(), file %x, SP %x, flags %x\n",
		SP->term_file, SP, cur_term->Nttyb.sg_flags);
# endif
#endif
	reset_prog_mode();
}
