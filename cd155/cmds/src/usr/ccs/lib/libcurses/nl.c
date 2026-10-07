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
static char sccsid[] = "@(#)nl.c  1.7  com/lib/curses,3.1,9008 1/17/90 14:25:02";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   nl
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

#include        "cursesext.h"

/*
 * NAME:        nl
 */

nl()	
{


#ifdef USG
	(cur_term->Nttyb).c_iflag |= ICRNL;
	(cur_term->Nttyb).c_oflag |= ONLCR;

# ifdef DEBUG
	if(outf) fprintf(outf,
		"nl(), file %x, SP %x, flags %x,%x\n",
		SP->term_file, SP, cur_term->Nttyb.c_iflag,
		cur_term->Nttyb.c_oflag);
# endif
#else
	(cur_term->Nttyb).sg_flags |= CRMOD;
# ifdef DEBUG
	if(outf) fprintf(outf,
		"nl(), file %x, SP %x, flags %x\n",
		SP->term_file, SP, cur_term->Nttyb.sg_flags);
# endif
#endif
	reset_prog_mode();
}
