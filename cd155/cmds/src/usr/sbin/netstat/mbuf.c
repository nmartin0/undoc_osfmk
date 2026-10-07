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
 * $Log: mbuf.c,v $
 * Revision 1.3  1994/11/19  03:12:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/12/11  01:39:29  shala
 * Support New table call.
 *
 * Revision 2.2  92/11/18  14:41:46  loverso
 * 	Add use of table() for OSF/1 AD.
 * 
 * Revision 2.6  90/10/07  22:34:46  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:51:14  gm]
 * 
 * Revision 2.5  90/07/27  11:39:40  devrcs
 * 	Update to BSD Reno release, with exception of ISO.
 * 	Look at mclbytes variable to find actual size.
 * 	[90/07/18  15:01:03  tmt]
 * 
 * Revision 2.4  90/04/27  20:14:57  devrcs
 * 	Fix size of clusters, avoid a core dump elsewhere.
 * 	[90/04/19  15:08:12  tmt]
 * 
 * Revision 2.3  90/03/27  21:23:48  gm
 * 	Working version against new Posix includes and libraries.
 * 	[90/03/14  16:48:30  tmt]
 * 
 * 	Update to 4.4 alpha release and OSF/1.
 * 	[90/03/06  12:50:20  tmt]
 * 
 * Revision 2.2  90/01/03  12:56:27  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:54:05  gm]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1983, 1988 Regents of the University of California.
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
 */

#ifndef lint
/* Updated to some of: "@(#)mbuf.c	5.9 (Berkeley) 6/18/90"; */
static char sccsid[] = "@(#)mbuf.c	5.7 (Berkeley) 9/25/89";
#endif /* not lint */

#include <stdio.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#define	YES	1
typedef int bool;

#ifdef OSF1AD
#include <sys/table.h>
#endif

struct	mbstat mbstat;
extern	int kmem;

static struct mbtypes {
	int	mt_type;
	char	*mt_name;
} mbtypes[] = {
	{ MT_DATA,	"data" },
	{ MT_OOBDATA,	"oob data" },
	{ MT_CONTROL,	"ancillary data" },
	{ MT_HEADER,	"packet headers" },
	{ MT_SOCKET,	"socket structures" },
	{ MT_PCB,	"protocol control blocks" },
	{ MT_RTABLE,	"routing table entries" },
	{ MT_HTABLE,	"IMP host table entries" },
	{ MT_ATABLE,	"address resolution tables" },
	{ MT_FTABLE,	"fragment reassembly queue headers" },
	{ MT_SONAME,	"socket names and addresses" },
	{ MT_SOOPTS,	"socket options" },
	{ MT_RIGHTS,	"access rights" },
	{ MT_IFADDR,	"interface addresses" },
	{ 0, 0 }
};

int nmbtypes = sizeof(mbstat.m_mtypes) / sizeof(mbstat.m_mtypes[0]);
bool seen[256];			/* "have we seen this type yet?" */

/*
 * Print mbuf statistics.
 */
mbpr(mbaddr, mclbytaddr)
	off_t mbaddr, mclbytaddr;
{
	register int totmem, totfree, totmbufs;
	register int i;
	register struct mbtypes *mp;
	int mclbytes = 0, rc;

	if (kmem >= 0) {
		if (mbaddr == 0) {
			printf("mbstat: symbol not in namelist\n");
			return;
		}
		klseek(kmem, mbaddr, 0);
		if (read(kmem, (char *)&mbstat, sizeof (mbstat))
		    != sizeof (mbstat)) {
			printf("mbstat: bad read\n");
			return;
		}
		if (mclbytaddr) {
			klseek(kmem, mclbytaddr, 0);
			(void) read(kmem, (char *)&mclbytes, sizeof (mclbytes));
		}
	} else {
		rc = table(TBL_MBSTAT, 0, &mbstat, 1, sizeof(struct mbstat));
		if (rc < 0) {
			perror("table: TBL_MBSTAT");
			return;
		}
		if (rc != 1) {
			printf("mbstat: table returned %d entries\n", rc);
			return;
		}
		/* use default mclbytes */
	}
	if (mclbytes <= 0)
		mclbytes = MCLBYTES;
	totmbufs = 0;
	for (mp = mbtypes; mp->mt_name; mp++)
		totmbufs += mbstat.m_mtypes[mp->mt_type];
	printf("%u/%u mbufs in use:\n", totmbufs, mbstat.m_mbufs);
	if (mbstat.m_mbufs - mbstat.m_mfree != totmbufs)
		printf("\tmbuf free count %u doesn't agree\n", mbstat.m_mfree);
	for (mp = mbtypes; mp->mt_name; mp++)
		if (mbstat.m_mtypes[mp->mt_type]) {
			seen[mp->mt_type] = YES;
			printf("\t%u mbufs allocated to %s\n",
			    mbstat.m_mtypes[mp->mt_type], mp->mt_name);
		}
	seen[MT_FREE] = YES;
	for (i = 0; i < nmbtypes; i++)
		if (!seen[i] && mbstat.m_mtypes[i]) {
			printf("\t%u mbufs allocated to <mbuf type %d>\n",
			    mbstat.m_mtypes[i], i);
		}
	printf("%u/%u mapped pages in use\n",
		mbstat.m_clusters - mbstat.m_clfree, mbstat.m_clusters);
	/* Small mbufs come from cluster pages */
	totmem = mbstat.m_clusters * mclbytes;
	totfree = mbstat.m_clfree * mclbytes;
	printf("%u Kbytes allocated to network (%d%% in use)\n",
		totmem / 1024, totmem ? (totmem - totfree) * 100 / totmem : 0);
	printf("%u requests for memory denied\n", mbstat.m_drops);
	printf("%u requests for memory delayed\n", mbstat.m_wait);
	printf("%u calls to protocol drain routines\n", mbstat.m_drain);
}
