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
 * Copyright 1993, Intel Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: hippi_config.c,v $
 * Revision 1.8  1995/03/06  23:28:32  arlin
 *  Added Multiple Packet per Connection support
 *  and CONTinuation support for multiple I/O
 *  per packet and connection.
 *
 *  Reviewer: Jerrie Coffman, Bernie Keany
 *  Risk: medium
 *  Benefit or PTS #: 12411
 *  Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *     Also developed special applications to test
 *     new MPC and CONT modes.
 *  Module(s):
 *     hippi_memfree.c, hippi_status.h, hippi_memget.c,
 *     hippi_var.h, hippi_open.c, hippi_write.c,
 *     hippi_bind.c, hippi_read.c, raw_hippi.h,
 *     hippi_close.c, hippi_read_complete.c,
 *     hippi_config.c, hippi_read_request.c
 *
 * Revision 1.7  1995/01/17  22:39:04  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 11084
 *  Testing: Developer testing.
 *  Module(s): hippi_bind.c hippi_close.c hippi_config.c hippi_memfree.c
 * 	    hippi_memget.c hippi_open.c hippi_read.c hippi_read_complete.c
 * 	    hippi_read_request.c hippi_write.c
 *
 *  Originally this problem should have just caused a change to hippi_bind.c
 *  but the compiler pointed out that all the function declarations were
 *  old style .. so we changed them all in conjunction with this PTS.
 *
 * Revision 1.6  1994/11/19  02:24:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/05/06  22:07:01  hobbes
 *  Merged in the final R1_2 changes which were applied
 *  to fix PTS 6677. Done after WW19.
 *
 * Revision 1.3.4.2  1994/04/19  23:04:21  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: tests provided in the PTS and HIPPI EATS
 *  Module(s): all .c modules in libhippi
 *
 * Revision 1.4  1994/01/17  23:19:57  hobbes
 * Merged R1.2 changes into the mainline. See checkin below for details.
 *
 * Revision 1.3.4.1  1994/01/15  01:00:54  hobbes
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 6677
 *  Testing: ran EATS against a WW03 server/emulator.
 *  Module(s): hippi_var.h hippi_config.c
 *
 * Revision 1.3  1993/07/20  20:19:46  hobbes
 * Added the ulp parameter for HIPPI_DATA mode .. and squirreled it away.
 *
 * Revision 1.2  1993/07/16  22:17:45  hobbes
 * Changes to get HIPPI_DATA mode to work.
 *
 * Revision 1.1  1993/05/28  02:26:52  hobbes
 * Initial checkin.
 *
 */

/*
 *
 * hippi_config(ihandle, ifield, ulp, b, d1_data, d1_len)
 *
 */

#include <sys/types.h>
#include <errno.h>
#include <mach.h>
#include <raw_hippi.h>
#include "hippi_var.h"

extern struct hippi_table	lt[];

int
hippi_config(int ihandle, u_long ifield, u_char ulp, u_long b,
		char *d1_data, u_short d1_len)
{
	/* attempt to catch bogus ihandles */
	if ((ihandle < 0) || (ihandle >= MAXCONNS)) {
		errno = EBADF;
		return (-1);
	}

	if (lt[ihandle].hippi_mode & HIPPI_DATA) {
		lt[ihandle].ifield = ifield;
		lt[ihandle].ulp = (u_long)ulp;
		lt[ihandle].b = b;
		if ( d1_data != NULL )
			lt[ihandle].d1_data = d1_data;
		else
			lt[ihandle].d1_data = NULL;
		if ((d1_len >= 0) && (d1_len <= MAX_D1_SIZE))
			lt[ihandle].d1_size = d1_len;
		else
			return (-1);
	} else {
		errno = EBADF;
		return(-1);
	}
}
