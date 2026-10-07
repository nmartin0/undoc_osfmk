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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: pass3.c,v $
 * Revision 1.3  1994/11/19  03:18:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/12  22:08:12  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.7.2.2  90/12/07  19:29:37  dwm
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:33:06  dwm]
 * 
 * Revision 2.7  90/10/07  22:51:20  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	[90/10/02  09:40:00  gmf]
 * 
 * Revision 2.6  90/07/27  11:45:30  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:31:20  pam]
 * 
 * Revision 2.5  90/06/22  22:12:59  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:31:56  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: pass3.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:18:17 $";
#endif
/*
 * Copyright (c) 1980, 1986 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*** "pass3.c	5.10 (Berkeley) 6/1/90"; ***/

#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#include "fsck.h"

pass3()
{
	register struct inoinfo **inpp, *inp;
	ino_t orphan;
	int loopcnt;

	for (inpp = &inpsort[inplast - 1]; inpp >= inpsort; inpp--) {
		inp = *inpp;
		if (inp->i_number == ROOTINO ||
		    !(inp->i_parent == 0 || statemap[inp->i_number] == DSTATE))
			continue;
		if (statemap[inp->i_number] == DCLEAR)
			continue;
		for (loopcnt = 0; ; loopcnt++) {
			orphan = inp->i_number;
			if (inp->i_parent == 0 ||
			    statemap[inp->i_parent] != DSTATE ||
			    loopcnt > numdirs)
				break;
			inp = getinoinfo(inp->i_parent);
		}
		(void)linkup(orphan, inp->i_dotdot);
		inp->i_parent = inp->i_dotdot = lfdir;
		lncntp[lfdir]--;
		statemap[orphan] = DFOUND;
		propagate();
	}
}
