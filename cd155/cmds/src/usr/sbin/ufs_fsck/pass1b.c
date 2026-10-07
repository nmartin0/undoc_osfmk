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
 * Don't use message catalog for OSF1_ADFS.
 * $Log: pass1b.c,v $
 * Revision 1.3  1994/11/19  03:18:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/12  22:08:05  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.8.2.2  91/06/03  13:41:43  garyf
 * 	add messaging support
 * 	[91/05/13  11:34:35  garyf]
 * 
 * Revision 2.8  91/01/07  17:23:31  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:32:53  dwm]
 * 
 * Revision 2.7  90/10/07  22:51:08  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	[90/10/02  09:39:00  gmf]
 * 
 * Revision 2.6  90/07/27  11:45:22  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:31:05  pam]
 * 
 * Revision 2.5  90/06/22  22:12:53  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:31:46  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: pass1b.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:18:16 $";
#endif
/*
 * Copyright (c) 1980, 1986 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*** "pass1b.c	5.8 (Berkeley) 7/20/90"; ***/

#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#include <string.h>
#include "fsck.h"

#ifdef  OSF1_ADFS
#define MSGSTR(n,s) s
#else
#include <locale.h>
#include "ufs_fsck_msg.h"

extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_UFS_FSCK,n,s) 
#endif  /* OSF1_ADFS */

int	pass1bcheck();
static  struct dups *duphead;

pass1b()
{
	register int c, i;
	register struct dinode *dp;
	struct inodesc idesc;
	ino_t inumber;

	bzero((char *)&idesc, sizeof(struct inodesc));
	idesc.id_type = ADDR;
	idesc.id_func = pass1bcheck;
	duphead = duplist;
	inumber = 0;
	for (c = 0; c < sblock.fs_ncg; c++) {
		for (i = 0; i < sblock.fs_ipg; i++, inumber++) {
			if (inumber < ROOTINO)
				continue;
			dp = ginode(inumber);
			if (dp == NULL)
				continue;
			idesc.id_number = inumber;
			if (statemap[inumber] != USTATE &&
			    (ckinode(dp, &idesc) & STOP))
				return;
		}
	}
}

pass1bcheck(idesc)
	register struct inodesc *idesc;
{
	register struct dups *dlp;
	int nfrags, res = KEEPON;
	daddr_t blkno = idesc->id_blkno;

	for (nfrags = idesc->id_numfrags; nfrags > 0; blkno++, nfrags--) {
		if (chkrange(blkno, 1))
			res = SKIP;
		for (dlp = duphead; dlp; dlp = dlp->next) {
			if (dlp->dup == blkno) {
				blkerror(idesc->id_number, MSGSTR(DUP, "DUP"), blkno);
				dlp->dup = duphead->dup;
				duphead->dup = blkno;
				duphead = duphead->next;
			}
			if (dlp == muldup)
				break;
		}
		if (muldup == 0 || duphead == muldup->next)
			return (STOP);
	}
	return (res);
}
