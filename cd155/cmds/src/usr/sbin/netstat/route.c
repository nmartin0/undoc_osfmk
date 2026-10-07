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
 * $Log: route.c,v $
 * Revision 1.5  1994/11/19  03:12:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/06/17  15:26:44  paul
 * Added changes for TNC. -I <unit> now supports TNC style interface names
 * such as <201>el0 where the node number which has the interface is included
 * in the name. At the same time, the server changed to return the long form
 * of the interface name, and because these names are longer, the hippi
 * interface name no longer fits in the 7 byte wide print field, so now this
 * utility does two passes in order to determine the maximum width interface
 * name. It then makes the field wider than 7 if necessary.
 *
 * Also, added a -D switch which in conjunction with the -r switch produces
 * routes including all the _mi invisible routes. This is probably only of
 * interest to developers, but may prove useful to people tuning applications.
 *
 *  Reviewer: Bernie Keany
 *  Risk: L
 *  Benefit or PTS #: 8808 9892
 *  Testing: 1, 2, & 3 netserver systems, on boot/nonboot configurations
 *  Module(s): if.c main.c route.c Makefile
 *
 * Revision 1.3  1993/06/04  22:31:54  shala
 * Fix typo in checking malloc return code (rn instead of rc).
 *
 * Revision 1.2  1992/12/11  01:39:35  shala
 * Support New table call.
 *
 * Revision 2.2  92/11/18  14:43:08  loverso
 * 	Add use of table() for OSF/1 AD.
 * 
 * Revision 2.7  90/10/07  22:34:54  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:51:28  gm]
 * 
 * Revision 2.6  90/07/27  11:39:48  devrcs
 * 	Update to BSD Reno release, with exception of ISO.
 * 	Use bcopy (in xns) instead of structure assignment to avoid segv.
 * 	Leave out BSD "newtree" stuff for now - no kinfo.
 * 	[90/07/18  15:03:18  tmt]
 * 
 * Revision 2.5  90/06/22  22:24:36  devrcs
 * 	Delete redundant index() declaration.
 * 	[90/06/14  10:04:23  tmt]
 * 
 * Revision 2.4  90/04/27  20:15:02  devrcs
 * 	Use _SOCKADDR_LEN for new sockaddrs. Tune up route printing, by
 * 	removing "(root node)" messages, and printing a more meaningful
 * 	netmask per family.
 * 	[90/04/19  15:09:29  tmt]
 * 
 * Revision 2.3  90/03/27  21:23:58  gm
 * 	Working version against new Posix includes and libraries.
 * 	Output format needs work!
 * 	[90/03/14  16:49:45  tmt]
 * 
 * 	Update to 4.4 alpha release and OSF/1.
 * 	Major change for new routing.
 * 	[90/03/06  12:51:11  tmt]
 * 
 * Revision 2.2  90/01/03  12:56:29  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:54:09  gm]
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
/* Updated to some of: "@(#)route.c	5.19 (Berkeley) 6/18/90"; */
static char sccsid[] = "@(#)route.c	5.15 (Berkeley) 12/7/88";
#endif /* not lint */

#include <sys/param.h>
#define	_SOCKADDR_LEN
#include <sys/socket.h>
#include <sys/mbuf.h>

#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>

#include <netns/ns.h>

#include <netdb.h>

#include <stdio.h>
#include <strings.h>

#ifdef OSF1_ADFS
#define TABLE_ROUTE		/* include routing structures */
#include <sys/table.h>
#endif

extern	int kmem;
extern	int nflag;
extern	char *routename(), *netname(), *ns_print(), *plural();
extern	char *malloc();
#define kget(p, d) \
	(klseek(kmem, (off_t)(p), 0), read(kmem, (char *)&(d), sizeof (d)))

/*
 * Definitions for showing gateway flags.
 */
struct bits {
	short	b_mask;
	char	b_val;
} bits[] = {
	{ RTF_UP,	'U' },
	{ RTF_GATEWAY,	'G' },
	{ RTF_HOST,	'H' },
	{ RTF_DYNAMIC,	'D' },
	{ RTF_MODIFIED,	'M' },
	{ RTF_CLONING,	'C' },
	{ RTF_XRESOLVE,	'R' },
	{ RTF_LLINFO,	'L' },
	{ 0 }
};

/*
 * Print routing tables.
 */
routepr(hostaddr, netaddr, hashsizeaddr, treeaddr)
	off_t hostaddr, netaddr, hashsizeaddr, treeaddr;
{
	printf("Routing tables\n");
	printf("%-16.16s %-18.18s %-6.6s  %6.6s%8.8s  %s\n",
		"Destination", "Gateway",
		"Flags", "Refs", "Use", "Interface");
	
	if (kmem >= 0)
		kmem_routepr(hostaddr, netaddr, hashsizeaddr, treeaddr);
	else
		table_routepr();
}
	
kmem_routepr(hostaddr, netaddr, hashsizeaddr, treeaddr)
	off_t hostaddr, netaddr, hashsizeaddr, treeaddr;
{
	struct mbuf mb;
	register struct ortentry *rt;
	register struct mbuf *m;
	char name[16], *flags;
	struct mbuf **routehash;
	int hashsize;
	int i, doinghost = 1;

	if (treeaddr)
		return treestuff(treeaddr);
	if (hostaddr == 0) {
		printf("rthost: symbol not in namelist\n");
		return;
	}
	if (netaddr == 0) {
		printf("rtnet: symbol not in namelist\n");
		return;
	}
	if (hashsizeaddr == 0) {
		printf("rthashsize: symbol not in namelist\n");
		return;
	}
	kget(hashsizeaddr, hashsize);
	routehash = (struct mbuf **)malloc( hashsize*sizeof (struct mbuf *) );
	klseek(kmem, hostaddr, 0);
	read(kmem, (char *)routehash, hashsize*sizeof (struct mbuf *));
again:
	for (i = 0; i < hashsize; i++) {
		if (routehash[i] == 0)
			continue;
		m = routehash[i];
		while (m) {
			kget(m, mb);
			p_rtentry((struct rtentry *)(mb.m_dat));
			m = mb.m_next;
		}
	}
	if (doinghost) {
		klseek(kmem, netaddr, 0);
		read(kmem, (char *)routehash, hashsize*sizeof (struct mbuf *));
		doinghost = 0;
		goto again;
	}
	free((char *)routehash);
	return;
}

table_routepr()
{
#ifdef TBL_RTHASH_HOST
	struct tbl_rthash *routehash = NULL;
#endif
	int i, rc, nc, hc;

	if (table_treestuff()) {
		return;
	}

#ifdef TBL_RTHASH_HOST
	nc = table(TBL_RTHASH_NET, 0, NULL, SHRT_MAX, 0);
	if (nc > 0) {
		hashsize = nc;
		routehash = (struct tbl_rthash *)
				malloc(sizeof(struct tbl_rthash)*nc);
		rc = table(TBL_RTHASH_NET, 0, routehash, nc,
				sizeof(struct tbl_rthash));
		if (rc > 0)
			for (i = 0; i < rc; i++) {
				/* XXX: WRONG */
				p_rtentry(routehash[i].t_rtentry);
			}
		if (rc < 0) {
			perror("TBL_RTHASH_NET: get data");
			goto done;
		}
	}
	if (nc < 0) {
		perror("TBL_RTHASH_NET: get size");
		goto done;
	}

	hc = table(TBL_RTHASH_HOST, 0, NULL, SHRT_MAX, 0);
	if (hc > 0) {
		hashsize = hc;
		if (routehash)
			routehash = (struct tbl_rthash *)
					realloc(sizeof(struct tbl_rthash)*hc);
		else
			routehash = (struct tbl_rthash *)
					malloc(sizeof(struct tbl_rthash)*hc);
		rc = table(TBL_RTHASH_HOST, 0, routehash, hc,
				sizeof(struct tbl_rthash));
		if (rc > 0)
			for (i = 0; i < rc; i++) {
				/* XXX: WRONG */
				p_rtentry(routehash[i].t_rtentry);
			}
		if (rc < 0) {
			perror("TBL_RTHASH_HOST: get data");
			goto done;
		}
	}
	if (hc < 0) {
		perror("TBL_RTHASH_HOST: get size");
		goto done;
	}

	if (nc == 0 && hc == 0)
		printf("table() returns no data!?\n");

done:
	if (routehash)
		free((char *)routehash);
#endif
}

static union sahack {
	struct	sockaddr u_sa;
	u_short	u_data[128];
} pt_u;
static struct rtentry rtentry;

treestuff(rtree)
off_t rtree;
{
	struct radix_node_head *rnh, head;
	    
	for (kget(rtree, rnh); rnh; rnh = head.rnh_next) {
		kget(rnh, head);
		if (head.rnh_af == 0) {
			printf("Netmasks:\n");
			p_tree(head.rnh_treetop, 0);
		} else {
			printf("\nRoute Tree for Protocol Family %d:\n",
								head.rnh_af);
			p_tree(head.rnh_treetop, 1);
		}
	}
}

table_treestuff()
{
	struct tbl_rnh head;
	int	rc, headidx = 0;

again:
	rc = table(TBL_RTREE_HEAD, headidx, &head, 1, sizeof(struct tbl_rnh));
	if (rc < 0) {
		perror("TBL_RTEE_HEAD");
		return headidx > 0;
	}
	if (rc != 1)
		return headidx > 0;
	headidx++;
	if (head.rnh_af == 0) {
		printf("Netmasks:\n");
		p_nodes(head.rnh_af);
	} else {
		printf("\nRoute Tree for Protocol Family %d:\n", head.rnh_af);
		p_nodes(head.rnh_af);
	}
	goto again;
	/*NOTREACHED*/
}

/*
 * A word of explanation for the following define. Under a TNC system, the
 * -D switch tells netstat to use the debug version of the TBL_RTREE_NODE
 * which works the same except that it includes invisible routes. If the
 * utility is built in a non-tnc environment, this define just makes the
 * -D switch a noop. If you actually try to run a version of netstat with
 * the -D switch, but under a non-TNC server, the table command will
 * return an error, and the user will get the TBL_TREE_NODE perror message.
 */
#ifndef TBL_RTREE_NODE_DEBUG
#define TBL_RTREE_NODE_DEBUG    TBL_RTREE_NODE
#endif

p_nodes(af)
int af;
{
	extern int Dflag;
	struct tbl_rn *rn, *rnp;
	int	rc, count, i;

	rc = table(Dflag ? TBL_RTREE_NODE_DEBUG : TBL_RTREE_NODE,
		   TRNIDX(af, 0), NULL, SHRT_MAX, 0);
	if (rc < 0) {
		perror("TBL_TREE_NODE: get size");
		return;
	}
	if (rc == 0)
		return;
	rn = (struct tbl_rn *)malloc(sizeof(struct tbl_rn) * rc);
	if (!rn) {
		printf("malloc for %d nodes of size %d failed\n",
			rc, sizeof(struct tbl_rn));
		return;
	}
	rc = table(Dflag ? TBL_RTREE_NODE_DEBUG : TBL_RTREE_NODE,
		   TRNIDX(af, 0), (char *)rn, rc,
			sizeof(struct tbl_rn));
	if (rc < 0) {
		perror("TBL_RTREE_NODE: get data");
		return;
	}

	/* We believe the last count we were given */

	for (rnp = rn, i = 0; i < rc; i++, rnp++)
		if (af != 0) {
			p_sockaddr((struct sockaddr *)&rnp->key,
				rnp->rt_flags, 16);
			p_sockaddr((struct sockaddr *)&rnp->rt_gateway,
				0, 18);
			p_flags(rnp->rt_flags, "%-6.6s ");
			printf("%6d %8d ", rnp->rt_refcnt, rnp->rt_use);
			if (rnp->if_name[0])
				printf(" %.15s%d",
					rnp->if_name,
					rnp->if_unit);
			putchar('\n');
		} else {
			p_maskentry((union sahack *)&rnp->key);
		}
}

struct sockaddr *
kgetsa(dst)
register struct sockaddr *dst;
{
	kget(dst, pt_u.u_sa);
	if (pt_u.u_sa.sa_len > sizeof (pt_u.u_sa)) {
		klseek(kmem, (off_t)dst, 0);
		read(kmem, pt_u.u_data, pt_u.u_sa.sa_len);
	}
	return (&pt_u.u_sa);
}

p_tree(rn, do_rtent)
struct radix_node *rn;
{
	struct radix_node rnode;
	register u_short *s, *slim;
	int len;

again:
	kget(rn, rnode);
	if (rnode.rn_b < 0) {
		if (rnode.rn_flags & RNF_ROOT)
			/* printf("(root node)\n") */;
		else if (do_rtent) {
			kget(rn, rtentry);
			p_rtentry(&rtentry);
		} else {
			/* BSD
			p_sockaddr(kgetsa((struct sockaddr *)rnode.rn_key),
				    0, 44);
			putchar('\n');
			*/
			kget(rnode.rn_key, pt_u);
			p_maskentry(&pt_u);
		}
		if (rn = rnode.rn_dupedkey)
			goto again;
	} else {
		p_tree(rnode.rn_l, do_rtent);
		p_tree(rnode.rn_r, do_rtent);
	}
}

p_sockaddr(sa, flags, width)
struct sockaddr *sa;
int flags, width;
{
	char format[20], workbuf[128], *cp, *cplim;
	register char *cpout;

	switch(sa->sa_family) {
	case AF_INET:
	    {
		register struct sockaddr_in *sin = (struct sockaddr_in *)sa;

		cp = (sin->sin_addr.s_addr == 0) ? "default" :
		      ((flags & RTF_HOST) ?
			routename(sin->sin_addr) : netname(sin->sin_addr, 0L));
	    }
		break;

	case AF_NS:
		cp = ns_print((struct sockaddr_ns *)sa);
		break;

	default:
	    {
		register u_short *s = ((u_short *)sa->sa_data),
				*slim = ((sa->sa_len + 1)/2) + s;

		cp = workbuf;
		cplim = cp + sizeof(workbuf) - 6;
		cp += sprintf(cp, "(%d)", sa->sa_family);
		while (s < slim && cp < cplim)
			cp += sprintf(cp, "%x ", *s++);
		cp = workbuf;
	    }
	}
	if (nflag)
		printf("%-*s ", width, cp);
	else
		printf("%-*.*s ", width, width, cp);
}

p_flags(f, format)
register int f;
char *format;
{
	char name[33], *flags;
	register struct bits *p = bits;
	for (flags = name; p->b_mask; p++)
		if (p->b_mask & f)
			*flags++ = p->b_val;
	*flags = '\0';
	printf(format, name);
}

p_rtentry(rt)
register struct rtentry *rt;
{
	char name[16];
	register struct sockaddr *sa;
	struct ifnet ifnet;

	p_sockaddr(kgetsa(rt_key(rt)), rt->rt_flags, 16);
	p_sockaddr(kgetsa(rt->rt_gateway), 0, 18);
	p_flags(rt->rt_flags, "%-6.6s ");
	printf("%6d %8d ", rt->rt_refcnt, rt->rt_use);
	if (rt->rt_ifp == 0) {
		putchar('\n');
		return;
	}
	kget(rt->rt_ifp, ifnet);
	klseek(kmem, (off_t)ifnet.if_name, 0);
	read(kmem, name, 16);
	printf(" %.15s%d\n", name, ifnet.if_unit);
}

p_ortentry(rt)
register struct ortentry *rt;
{
	char name[16], *flags;
	register struct bits *p;
	register struct sockaddr_in *sin;
	struct ifnet ifnet;

	p_sockaddr(&rt->rt_dst, rt->rt_flags, 16);
	p_sockaddr(&rt->rt_gateway, 0, 18);
	p_flags(rt->rt_flags, "%-6.6s ");
	printf("%6d %8d ", rt->rt_refcnt, rt->rt_use);
	if (rt->rt_ifp == 0) {
		putchar('\n');
		return;
	}
	kget(rt->rt_ifp, ifnet);
	klseek(kmem, (off_t)ifnet.if_name, 0);
	read(kmem, name, 16);
	printf(" %.15s%d\n", name, ifnet.if_unit);
}

p_maskentry(sah)
union sahack *sah;
{
	if (sah->u_sa.sa_family == 0) {	/* XNS does this, right now at least */
		sah->u_sa.sa_family = sah->u_sa.sa_len;
		pt_u.u_sa.sa_len = 0;
	}
	switch(sah->u_sa.sa_family) {
	case AF_INET:
		printf("%-16.16s %-18.18s ", "Inet",
		    routename(((struct sockaddr_in *)&sah->u_sa)->sin_addr));
		break;
	case AF_NS:
		printf("%-16s %-18s ", "XNS",
		    ns_print(&((struct sockaddr_ns *)&sah->u_sa)->sns_addr));
		break;
	default: {
		register u_short *s, *slim;
		int len;
		printf("(%-2d)%-13s", sah->u_sa.sa_family, " ");
		if ((len = sah->u_sa.sa_len) == 0 || len > MAXKEYLEN)
			len = MAXKEYLEN;
		s = sah->u_data + 1;
		for (slim = s + ((len - 1)/2); s < slim; s++)
			printf("%04x ", *s);
		}
	}
/* Maybe later we can figure out the interface... */
	putchar('\n');
}

char *
routename(in)
	struct in_addr in;
{
	register char *cp;
	static char line[MAXHOSTNAMELEN + 1];
	struct hostent *hp;
	static char domain[MAXHOSTNAMELEN + 1];
	static int first = 1;

	if (first) {
		first = 0;
		if (gethostname(domain, MAXHOSTNAMELEN) == 0 &&
		    (cp = index(domain, '.')))
			(void) strcpy(domain, cp + 1);
		else
			domain[0] = 0;
	}
	cp = 0;
	if (!nflag) {
		hp = gethostbyaddr((char *)&in, sizeof (struct in_addr),
			AF_INET);
		if (hp) {
			if ((cp = index(hp->h_name, '.')) &&
			    !strcmp(cp + 1, domain))
				*cp = 0;
			cp = hp->h_name;
		}
	}
	if (cp)
		strncpy(line, cp, sizeof(line) - 1);
	else {
#define C(x)	((x) & 0xff)
		in.s_addr = ntohl(in.s_addr);
		sprintf(line, "%u.%u.%u.%u", C(in.s_addr >> 24),
			C(in.s_addr >> 16), C(in.s_addr >> 8), C(in.s_addr));
	}
	return (line);
}

/*
 * Return the name of the network whose address is given.
 * The address is assumed to be that of a net or subnet, not a host.
 */
char *
netname(in, mask)
	struct in_addr in;
	u_long mask;
{
	char *cp = 0;
	static char line[MAXHOSTNAMELEN + 1];
	struct netent *np = 0;
	u_long net;
	register i;
	int subnetshift;

	i = ntohl(in.s_addr);
	if (!nflag && i) {
		if (mask == 0) {
			if (IN_CLASSA(i)) {
				mask = IN_CLASSA_NET;
				subnetshift = 8;
			} else if (IN_CLASSB(i)) {
				mask = IN_CLASSB_NET;
				subnetshift = 8;
			} else {
				mask = IN_CLASSC_NET;
				subnetshift = 4;
			}
			/*
			 * If there are more bits than the standard mask
			 * would suggest, subnets must be in use.
			 * Guess at the subnet mask, assuming reasonable
			 * width subnet fields.
			 */
			while (i &~ mask)
				mask = (long)mask >> subnetshift;
		}
		net = i & mask;
		while ((mask & 1) == 0)
			mask >>= 1, net >>= 1;
		np = getnetbyaddr(net, AF_INET);
		if (np)
			cp = np->n_name;
	}	
	if (cp)
		strncpy(line, cp, sizeof(line) - 1);
	else if ((i & 0xffffff) == 0)
		sprintf(line, "%u", C(i >> 24));
	else if ((i & 0xffff) == 0)
		sprintf(line, "%u.%u", C(i >> 24) , C(i >> 16));
	else if ((i & 0xff) == 0)
		sprintf(line, "%u.%u.%u", C(i >> 24), C(i >> 16), C(i >> 8));
	else
		sprintf(line, "%u.%u.%u.%u", C(i >> 24),
			C(i >> 16), C(i >> 8), C(i));
	return (line);
}

/*
 * Print routing statistics
 */
rt_stats(off)
	off_t off;
{
	struct rtstat rtstat;
	int rc;

	if (kmem >= 0) {
		if (off == 0) {
			printf("rtstat: symbol not in namelist\n");
			return;
		}
		klseek(kmem, off, 0);
		read(kmem, (char *)&rtstat, sizeof (rtstat));
	} else {
		rc = table(TBL_RTSTAT, 0, &rtstat, 1, sizeof(struct rtstat));
		if (rc < 0) {
			perror("table: TBL_RTSTAT");
			return;
		}
		if (rc != 1) {
			printf("table: TBL_RTSTAT returned %d entries\n", rc);
			return;
		}
	}
	printf("routing:\n");
	printf("\t%u bad routing redirect%s\n",
		rtstat.rts_badredirect, plural(rtstat.rts_badredirect));
	printf("\t%u dynamically created route%s\n",
		rtstat.rts_dynamic, plural(rtstat.rts_dynamic));
	printf("\t%u new gateway%s due to redirects\n",
		rtstat.rts_newgateway, plural(rtstat.rts_newgateway));
	printf("\t%u destination%s found unreachable\n",
		rtstat.rts_unreach, plural(rtstat.rts_unreach));
	printf("\t%u use%s of a wildcard route\n",
		rtstat.rts_wildcard, plural(rtstat.rts_wildcard));
}
short ns_nullh[] = {0,0,0};
short ns_bh[] = {-1,-1,-1};

char *
ns_print(sns)
struct sockaddr_ns *sns;
{
	struct ns_addr work;
	union { union ns_net net_e; u_long long_e; } net;
	u_short port;
	static char mybuf[50], cport[10], chost[25];
	char *host = "";
	register char *p; register u_char *q;

	bcopy((caddr_t)&sns->sns_addr, (caddr_t)&work, sizeof (work));
	port = ntohs(work.x_port);
	work.x_port = 0;
	net.net_e  = work.x_net;
	if (ns_nullhost(work) && net.long_e == 0) {
		if (port ) {
			sprintf(mybuf, "*.%xH", port);
			upHex(mybuf);
		} else
			sprintf(mybuf, "*.*");
		return (mybuf);
	}

	if (bcmp(ns_bh, work.x_host.c_host, 6) == 0) {
		host = "any";
	} else if (bcmp(ns_nullh, work.x_host.c_host, 6) == 0) {
		host = "*";
	} else {
		q = work.x_host.c_host;
		sprintf(chost, "%02x%02x%02x%02x%02x%02xH",
			q[0], q[1], q[2], q[3], q[4], q[5]);
		for (p = chost; *p == '0' && p < chost + 12; p++);
		host = p;
	}
	if (port)
		sprintf(cport, ".%xH", htons(port));
	else
		*cport = 0;

	sprintf(mybuf,"%xH.%s%s", ntohl(net.long_e), host, cport);
	upHex(mybuf);
	return(mybuf);
}

char *
ns_phost(sns)
struct sockaddr_ns *sns;
{
	struct sockaddr_ns work;
	static union ns_net ns_zeronet;
	char *p;
	
	work = *sns;
	work.sns_addr.x_port = 0;
	work.sns_addr.x_net = ns_zeronet;

	p = ns_print(&work);
	if (strncmp("0H.", p, 3) == 0) p += 3;
	return(p);
}
upHex(p0)
char *p0;
{
	register char *p = p0;
	for (; *p; p++) switch (*p) {

	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		*p += ('A' - 'a');
	}
}
