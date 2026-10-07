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
 * $Log: pass4.c,v $
 * Revision 1.3  1994/11/19  03:18:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/12  22:08:14  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.8.2.2  91/06/03  13:41:54  garyf
 * 	add messaging support
 * 	[91/05/13  11:34:54  garyf]
 * 
 * Revision 2.8  91/01/07  17:23:43  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:33:11  dwm]
 * 
 * Revision 2.7  90/10/07  22:51:25  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	[90/10/02  09:40:16  gmf]
 * 
 * Revision 2.6  90/07/27  11:45:34  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:31:27  pam]
 * 
 * Revision 2.5  90/06/22  22:13:02  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:32:08  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: pass4.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:18:18 $";
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

/*** "pass4.c	5.10 (Berkeley) 7/20/90"; ***/

#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#include <stdlib.h>
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

int	pass4check();

pass4()
{
	register ino_t inumber;
	register struct zlncnt *zlnp;
	struct dinode *dp;
	struct inodesc idesc;
	int n;

	bzero((char *)&idesc, sizeof(struct inodesc));
	idesc.id_type = ADDR;
	idesc.id_func = pass4check;
	for (inumber = ROOTINO; inumber <= lastino; inumber++) {
		idesc.id_number = inumber;
		switch (statemap[inumber]) {

		case FSTATE:
		case DFOUND:
			n = lncntp[inumber];
			if (n)
				adjust(&idesc, (short)n);
			else {
				for (zlnp = zlnhead; zlnp; zlnp = zlnp->next)
					if (zlnp->zlncnt == inumber) {
						zlnp->zlncnt = zlnhead->zlncnt;
						zlnp = zlnhead;
						zlnhead = zlnhead->next;
						free((char *)zlnp);
						clri(&idesc, MSGSTR(UNREF, "UNREF"), 1);
						break;
					}
			}
			break;

		case DSTATE:
			clri(&idesc, MSGSTR(UNREF, "UNREF"), 1);
			break;

		case DCLEAR:
			dp = ginode(inumber);
			if (dp->di_size == 0) {
				clri(&idesc, MSGSTR(ZEROLEN, "ZERO LENGTH"), 1);
				break;
			}
			/* fall through */
		case FCLEAR:
			clri(&idesc, MSGSTR(BADDUP, "BAD/DUP"), 1);
			break;

		case USTATE:
			break;

		default:
			errexit(MSGSTR(BADSTATE2, "BAD STATE %d FOR INODE I=%d"),
			    statemap[inumber], inumber);
		}
	}
}

pass4check(idesc)
	register struct inodesc *idesc;
{
	register struct dups *dlp;
	int nfrags, res = KEEPON;
	daddr_t blkno = idesc->id_blkno;

	for (nfrags = idesc->id_numfrags; nfrags > 0; blkno++, nfrags--) {
		if (chkrange(blkno, 1)) {
			res = SKIP;
		} else if (testbmap(blkno)) {
			for (dlp = duplist; dlp; dlp = dlp->next) {
				if (dlp->dup != blkno)
					continue;
				dlp->dup = duplist->dup;
				dlp = duplist;
				duplist = duplist->next;
				free((char *)dlp);
				break;
			}
			if (dlp == 0) {
				clrbmap(blkno);
				n_blks--;
			}
		}
	}
	return (res);
}
