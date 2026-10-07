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
 * $Log: host.c,v $
 * Revision 1.3  1994/11/19  03:12:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/12/11  01:39:14  shala
 * Support New table call.
 *
 * Revision 2.2  92/11/18  14:35:21  loverso
 * 	Add use of table() for OSF/1 AD.
 * 
 * Revision 2.7  90/10/07  22:34:22  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:50:41  gm]
 * 
 * Revision 2.6  90/07/27  11:39:21  devrcs
 * 	Update to BSD Reno release, with exception of ISO.
 * 	[90/07/18  14:57:05  tmt]
 * 
 * Revision 2.5  90/06/22  22:24:24  devrcs
 * 	Grok new ARP flags.
 * 	[90/06/15  12:31:41  tmt]
 * 
 * Revision 2.4  90/03/27  21:23:26  gm
 * 	Working version against new Posix includes and libraries.
 * 	[90/03/14  16:45:55  tmt]
 * 
 * 	Update to 4.4 alpha release and OSF/1.
 * 	[90/03/06  12:47:00  tmt]
 * 
 * Revision 2.3  90/01/03  12:56:20  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:53:59  gm]
 * 
 * Revision 2.2  89/12/26  10:56:10  gm
 * 	Fixed arppr() to work with valloc'd arptab [courtesy of
 * 	Robert Baron (rvb)] and changed arppr() to print ethernet
 * 	address in hex format.
 * 	[89/01/17            parker]
 * 
 * 	Added arppr() routine to display ARP cache.
 * 	[85/09/28            mja]
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
/* Updated to some of: "@(#)host.c	5.11 (Berkeley) 6/18/90"; */
static char sccsid[] = "@(#)host.c	5.9 (Berkeley) 9/25/89";
#endif /* not lint */

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netimp/if_imp.h>
#include <netimp/if_imphost.h>
#if	MACH
#include <netinet/if_ether.h>
#endif

#define TABLE_ARPTAB
#include <sys/table.h>

extern	int kmem;
extern 	int nflag;
extern	char *inetname();

/*
 * Print the host tables associated with the ARPANET IMP.
 * Symbolic addresses are shown unless the nflag is given.
 */
hostpr(impsoftcaddr, nimpaddr)
	off_t impsoftcaddr, nimpaddr;
{
	struct mbuf *hosts, mb;
	struct imp_softc imp_softc;
	register struct mbuf *m;
	register struct hmbuf *mh;
	register struct host *hp;
	char flagbuf[10], *flags;
	int i, nimp;

	if (impsoftcaddr == 0) {
		printf("imp_softc: symbol not in namelist\n");
		return;
	}
	if (nimpaddr == 0) {
		printf("nimp: symbol not in namelist\n");
		return;
	}
	klseek(kmem, nimpaddr, 0);
	read(kmem, (char *)&nimp, sizeof (nimp));
	klseek(kmem, impsoftcaddr, 0);
	for (i = 0; i < nimp; i++) {
	    read(kmem, (char *)&imp_softc, sizeof (imp_softc));
	    m = imp_softc.imp_hosts;
	    printf("IMP%d Host Table\n", i);
	    printf("%-5.5s %-6.6s %-8.8s %-4.4s %-9.9s %-4.4s %s\n", "Flags",
	        "Host", "Imp", "Qcnt", "Q Address", "RFNM", "Timer");
	    while (m) {
		klseek(kmem, (off_t)m, 0);
		read(kmem, (char *)&mb, sizeof (mb));
		m = &mb;
		mh = (struct hmbuf *)(m->m_dat);
		if (mh->hm_count == 0) {
			m = m->m_next;
			continue;
		}
		for (hp = mh->hm_hosts; hp < mh->hm_hosts + HPMBUF; hp++) {
			if ((hp->h_flags&HF_INUSE) == 0 && hp->h_timer == 0)
				continue;
			flags = flagbuf;
			*flags++ = hp->h_flags&HF_INUSE ? 'A' : 'F';
			if (hp->h_flags&HF_DEAD)
				*flags++ = 'D';
			if (hp->h_flags&HF_UNREACH)
				*flags++ = 'U';
			*flags = '\0';
			printf("%-5.5s %-6d %-8d %-4d %-9x %-4d %d\n",
				flagbuf,
				hp->h_host,
				ntohs(hp->h_imp),
				hp->h_qcnt,
				hp->h_q,
				hp->h_rfnm,
				hp->h_timer);
		}
		m = m->m_next;
	    }
	}
}

impstats(impsoftcaddr, nimpaddr)
	off_t impsoftcaddr, nimpaddr;
{
	struct imp_softc imp_softc;
	int i, nimp;
	extern char *plural();

	if (impsoftcaddr == 0 || nimpaddr == 0)
		return;
	klseek(kmem, nimpaddr, 0);
	read(kmem, (char *)&nimp, sizeof (nimp));
	klseek(kmem, impsoftcaddr, 0);
	for (i = 0; i < nimp; i++) {
		read(kmem, (char *)&imp_softc, sizeof (imp_softc));
		printf("imp%d statistics:\n", i);
#define	p(f, m)		printf(m, imp_softc.f, plural(imp_softc.f))
		p(imp_if.if_ipackets, "\t%u input message%s\n");
		p(imp_if.if_opackets, "\t%u output message%s\n");
		printf("\t%u times output blocked at least %d sec.\n",
		    imp_softc.imp_block, IMP_OTIMER);
		p(imp_incomplete, "\t%u \"incomplete\" message%s\n");
		p(imp_lostrfnm, "\t%u lost RFNM message%s\n");
		p(imp_badrfnm, "\t%u late/bogus RFNM/incomplete message%s\n");
		p(imp_garbage, "\t%u unknown message type%s\n");
	}
}

#if	MACH
/*
 * Print the ARP cache protocol/hardware address cache.
 * Symbolic addresses are shown unless the nflag is given.
 */
arppr(arptabaddr, bsizaddr, nbaddr)
	off_t arptabaddr;
	off_t bsizaddr;
	off_t nbaddr;
{
	long bsize = 5;
	long nb = 19;
	caddr_t arptabptr;
	register struct arptab *arptab;
	register int i, j = 0, m;
	char flagbuf[10], *flags;
	extern int Aflag;
	int rc;
	register struct tbl_arptab *ta;
	struct tbl_arpparam tap;

	if (kmem >= 0) {
		if (arptabaddr == 0) {
			printf("arptab: symbol(s) not in namelist\n");
			return;
		}
		if (arptabaddr != 0)
		{
			klseek(kmem, arptabaddr, 0);
			read(kmem, &arptabptr, sizeof (arptabptr));
		}
		if (bsizaddr != 0)
		{
			klseek(kmem, bsizaddr, 0);
			read(kmem, &bsize, sizeof (bsize));
		}
		if (nbaddr != 0)
		{
			klseek(kmem, nbaddr, 0);
			read(kmem, &nb, sizeof (nb));
		}
		m = bsize*nb;
		arptab = (struct arptab *)malloc(sizeof(struct arptab)*m);
		if (arptab == 0) {
			printf("arptab: could not allocate %d table entries\n",
				m);
			return;
		}
		klseek(kmem, arptabptr, 0);
		read(kmem, arptab, sizeof (struct arptab)*m);
	} else {
		rc = table(TBL_ARPPARAM, 0, (char *)&tap, 1, sizeof tap);
		if (rc < 0) {
			perror("arptab: table: get arp parameters");
			return;
		}
		if (rc != 1) {
			printf("arptab: table: TBL_ARPPARAM returned %d\n", rc);
			return;
		}

		/*
		 * Get size of arp tabe
		 */
		rc = table(TBL_ARPTAB, 0, NULL, SHRT_MAX, 0);
		if (rc < 0) {
			perror("arptab: table: get arp size");
			return;
		}
		m = rc;
		nb = tap.arptab_nb;
		bsize = tap.arptab_bsiz;
		ta = (struct tbl_arptab *)malloc(sizeof(struct tbl_arptab)*m);
		if (ta == 0) {
			printf("arptab: could not allocate %d table entries\n",
				m);
			return;
		}
		rc = table(TBL_ARPTAB, 0, (char *)ta, m,
						sizeof(struct tbl_arptab));
		if (rc < 0) {
			perror("arptab: table: get arp entries");
			return;
		}
		m = rc;		/* in case table shrunk */
	}
	printf("Address Resolution Mapping Table: %d Buckets %d Deep %d Bytes\n",
		nb, bsize, sizeof (struct arptab));
	printf("Cnt  ");
	if (Aflag)
		printf("%-8.8s ", "ADDR");
	printf("%-8.8s %-5.5s %-24.24s %s\n",
		"Flags", "Timer", "Host", "Phys Addr");
	for (i=0; i<m; i++) {
		register struct arptab *at;
		
		if (kmem >= 0)
			at = &arptab[i];
		else
			at = &ta[i].t_arptab;

		if (at->at_flags == 0)
			continue;
		flags = flagbuf;
		if (at->at_flags&ATF_INUSE)
			*flags++ = 'U';
		if (at->at_flags&ATF_COM)
			*flags++ = 'C';
		if (at->at_flags&ATF_PERM)
			*flags++ = 'P';
		if (at->at_flags&ATF_PUBL)
			*flags++ = 'B';
		if (at->at_flags&ATF_USETRAILERS)
			*flags++ = 'T';
#ifdef	ATF_STALE
		if (at->at_flags & ATF_USE802)	/* not used */
			*flags++ = 'I';
		if (at->at_flags & ATF_STALE)
			*flags++ = 'S';
		if (at->at_flags & ATF_DEAD)
			*flags++ = 'D';
#endif
		*flags = '\0';
		printf("%3d: ", ++j);
		if (Aflag)
			printf("%8x ",arptabaddr+((caddr_t)at-(caddr_t)arptab));
		printf("%-8.8s  %3d  %-24.24s ", flagbuf, at->at_timer, inetname(at->at_iaddr));
		if (at->at_enaddr[5] == 0 && at->at_enaddr[4] == 0 && at->at_enaddr[3] == 0 && at->at_enaddr[2] == 0 && at->at_enaddr[1] == 0)
			printf("#%o", at->at_enaddr[0]);
		else
			printf("%02x:%02x:%02x:%02x:%02x:%02x",
			       at->at_enaddr[0], at->at_enaddr[1], at->at_enaddr[2],
			       at->at_enaddr[3], at->at_enaddr[4], at->at_enaddr[5]);

		/* we get this for free with table() */
		if (kmem == -1 && ta[i].if_name[0])
			printf("  (%s%d)", ta[i].if_name, ta[i].if_unit);
		printf("\n");
	}
}
#endif	/* MACH */
