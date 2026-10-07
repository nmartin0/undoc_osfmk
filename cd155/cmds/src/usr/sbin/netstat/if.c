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
 * $Log: if.c,v $
 * Revision 1.6  1994/11/19  03:12:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/09/19  18:34:53  slk
 * Changed the format of the banner printf so that
 * the full interface name will be printed in the banner.
 *  Reviewer: Bob Yasi, Bernie Keany
 *  Risk: Low.
 *  Benefit or PTS #: 10944
 *  Testing: netstat -d -I 'name' 2; netstat -I 'name' 10
 *  Module(s): if.c
 *
 * Revision 1.4  1994/06/17  15:24:38  paul
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
 * Revision 1.3  1994/03/25  01:24:45  yazz
 *  Reviewer: Bernie Keany
 *  Risk: Lo
 *  Benefit or PTS #: #8706
 *  Testing: Testcase now passes.
 *  Module(s): .../usr/sbin/netstat/if.c
 *
 * Widened width of "Name" field from 5 chars to 7 to accommodate the full
 * char string "ifhip0*".
 *
 * Revision 1.2  1992/12/11  01:39:17  shala
 * Support New table call.
 *
 * Revision 2.2  92/11/18  14:35:43  loverso
 * 	Add use of table() for OSF/1 AD.
 * 
 * Revision 2.7  90/10/07  22:34:27  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:50:47  gm]
 * 
 * Revision 2.6  90/07/27  11:39:25  devrcs
 * 	Update to BSD Reno release, with exception of ISO.
 * 	[90/07/18  14:57:43  tmt]
 * 
 * Revision 2.5  90/06/22  22:24:27  devrcs
 * 	Rearrange code to fix types for gcc compile.
 * 	[90/06/14  10:02:43  tmt]
 * 
 * Revision 2.4  90/04/27  20:14:54  devrcs
 * 	Use _SOCKADDR_LEN token for new sockaddrs.
 * 	[90/04/19  15:07:32  tmt]
 * 
 * Revision 2.3  90/03/27  21:23:31  gm
 * 	Working version against new Posix includes and libraries.
 * 	[90/03/14  16:46:38  tmt]
 * 
 * 	Update to 4.4 alpha release and OSF/1.
 * 	[90/03/06  12:47:52  tmt]
 * 
 * Revision 2.2  90/01/03  12:56:24  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:54:02  gm]
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
/* Updated to some of: "@(#)if.c	5.14 (Berkeley) 6/18/90"; */
static char sccsid[] = "@(#)if.c	5.10 (Berkeley) 9/25/89";
#endif /* not lint */

#include <sys/types.h>
#define	_SOCKADDR_LEN
#include <sys/socket.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netns/ns.h>
#include <netns/ns_if.h>

#include <stdio.h>
#include <strings.h>
#include <signal.h>

#define	YES	1
#define	NO	0

#ifdef OSF1_ADFS
#define TABLE_IF	/* include interface structures */
#include <sys/table.h>
#endif

extern	int kmem;
extern	int tflag;
extern	int dflag;
extern	int nflag;
extern	char *interface;
extern	int unit;
extern	char *routename(), *netname(), *ns_phost();

/*
 * Print a description of the network interfaces.
 */
intpr(interval, ifnetaddr)
	int interval;
	off_t ifnetaddr;
{
	struct ifnet ifnet, *ifp;
	union ifaddrany ifaddr, *ifap;
	struct sockaddr *sa;
	char *name;
	off_t ifaddraddr = 0;
#ifdef OSF1_ADFS
	struct tbl_if tbl_if;
	int if_ctr = 0;
	int ifa_ctr = 0, ifa_max = 0;
	int rc = 0;
#endif
	int printing_this_time = 0;
	int widest_field_so_far = 7;
	char printf_format_string[64];
	int newline = 0;

	if (kmem >= 0 && ifnetaddr == 0) {
		printf("ifnet: symbol not defined\n");
		return;
	}

	if (interval) {
		sidewaysintpr((unsigned)interval, ifnetaddr);
		return;
	}

	if (kmem >= 0) {
		klseek(kmem, ifnetaddr, 0);
		read(kmem, (char *)&ifnetaddr, sizeof ifnetaddr);
	}

twice:

#ifdef OSF1_ADFS
	if_ctr = 0;
	ifa_ctr = 0, ifa_max = 0;
	rc = 0;
#endif

	if(printing_this_time){
		sprintf(printf_format_string,"%%-%d.%ds %%-5.5s ",
			widest_field_so_far,widest_field_so_far);
		printf(printf_format_string,"Name", "Mtu");
		printf("%-11.11s %-15.15s %8.8s %5.5s %8.8s %5.5s",
			"Network", "Address", "Ipkts", "Ierrs",
			"Opkts", "Oerrs");
		printf(" %5s", "Coll");
		if (tflag)
			printf(" %s", "Time");
		if (dflag)
			printf(" %s", "Drop");
		putchar('\n');
	}
	for (;;) {
		struct sockaddr_in *sin;
		register char *cp;
		int n, m;
		struct in_addr in, inet_makeaddr();

		if (kmem >= 0) {
			if (!ifnetaddr && !ifaddraddr)
				break;
			if (ifaddraddr == 0) {
				klseek(kmem, ifnetaddr, 0);
				read(kmem, (char *)&ifnet, sizeof ifnet);
				klseek(kmem, (off_t)ifnet.if_name, 0);
				read(kmem, ifnet.if_name, 16);
				ifp = &ifnet;
				name = &ifnet.if_name[0];
				ifnetaddr = (off_t) ifp->if_next;
				ifaddraddr = (off_t)ifp->if_addrlist;

				newline = 1;
			}
		} else {
			if (ifa_max == 0 || ifa_ctr >= ifa_max) {
				rc = table(TBL_IF, if_ctr++, &tbl_if, 1,
					sizeof(struct tbl_if));
				if (rc != 1){
					break;
				}
				ifp = &tbl_if.if_net;
				name = &tbl_if.if_name[0];
				ifa_max = tbl_if.ifaddrcount;
				ifa_ctr = 0;

				newline = 1;
			}
		}
		if (newline) {
			name[15] = '\0';
			if (interface != NULL && (unit != ifp->if_unit ||
			   interface_match(name,interface) != 0)) {
				if (kmem >= 0)
					ifaddraddr = (off_t)ifap->ifa.ifa_next;
				else
					ifa_ctr++;
				continue;
			}

			newline = 0;
			cp = index(name, '\0');
			*cp++ = ifp->if_unit + '0';
			if ((ifp->if_flags&IFF_UP) == 0)
				*cp++ = '*';
			*cp = '\0';
		}

		if(strlen(name) > widest_field_so_far){
			widest_field_so_far = strlen(name);
		}
		if(printing_this_time == 0){
			if (kmem >= 0)
				ifaddraddr = (off_t)ifap->ifa.ifa_next;
			else
				ifa_ctr++;
			continue;
		}

		sprintf(printf_format_string,"%%-%d.%ds %%-5d ",
			widest_field_so_far,widest_field_so_far);
		printf(printf_format_string, name, ifp->if_mtu);

		if ((kmem >= 0 && ifaddraddr == 0)
		    || (kmem <0 && ifa_max == 0)) {
			printf("%-11.11s ", "none");
			printf("%-15.15s ", "none");
		} else {
			if (kmem >= 0) {
				klseek(kmem, ifaddraddr, 0);
				read(kmem, (char *)&ifaddr, sizeof ifaddr);
#define CP(x) ((char *)(x))
				cp = (CP(ifaddr.ifa.ifa_addr) -
					CP(ifaddraddr)) + CP(&ifaddr);
				ifap = &ifaddr;
				sa = (struct sockaddr *)cp;
			} else {
				ifap = &tbl_if.ifaddrs[ifa_ctr].ifa;
				sa = (struct sockaddr *)
					&tbl_if.ifaddrs[ifa_ctr].sa;
			}

			switch (sa->sa_family) {
			case AF_UNSPEC:
				printf("%-11.11s ", "none");
				printf("%-15.15s ", "none");
				break;
			case AF_INET:
				sin = (struct sockaddr_in *)sa;
#ifdef notdef
				/* can't use inet_makeaddr because kernel
				 * keeps nets unshifted.
				 */
				in = inet_makeaddr(ifap->in.ia_subnet,
					INADDR_ANY);
				printf("%-11.11s ", netname(in));
#else
				in.s_addr = htonl(ifap->in.ia_subnet);
				printf("%-11.11s ",
					netname(in, ifap->in.ia_subnetmask));
#endif
				printf("%-15.15s ", routename(sin->sin_addr));
				break;
			case AF_NS:
				{
				struct sockaddr_ns *sns =
					(struct sockaddr_ns *)sa;
				u_long net;
				char netnum[8];
				char *ns_phost();
				*(union ns_net *) &net = sns->sns_addr.x_net;
				sprintf(netnum, "%lxH", ntohl(net));
				upHex(netnum);
				printf("ns:%-8s ", netnum);
				printf("%-15s ", ns_phost(sns));
				}
				break;
			case AF_LINK:
				{
				struct sockaddr_dl *sdl =
					(struct sockaddr_dl *)sa;
				    cp = (char *)LLADDR(sdl);
				    n = sdl->sdl_alen;
				}
				m = printf("<Link>");
				goto hexprint;
			default:
				m = printf("(%d)", sa->sa_family);
				for (cp = sa->sa_len + (char *)sa;
					--cp > sa->sa_data && (*cp == 0);) {}
				n = cp - sa->sa_data + 1;
				cp = sa->sa_data;
			hexprint:
				while (--n >= 0)
					m += printf("%x%c", *cp++ & 0xff,
						    n > 0 ? '.' : ' ');
				m = 28 - m;
				while (m-- > 0)
					putchar(' ');
				break;
			}

			if (kmem >= 0)
				ifaddraddr = (off_t)ifap->ifa.ifa_next;
			else
				ifa_ctr++;
		}
		printf("%8d %5d %8d %5d %5d",
		    ifp->if_ipackets, ifp->if_ierrors,
		    ifp->if_opackets, ifp->if_oerrors,
		    ifp->if_collisions);
		if (tflag)
			printf(" %3d", ifp->if_timer);
		if (dflag)
			printf(" %3d", ifp->if_snd.ifq_drops);
		putchar('\n');
	}

	if(printing_this_time == 0){
		printing_this_time = 1;
		goto twice;
	}

	if (rc < 0)
		perror("table: TBL_IF");
	if (rc > 0)
		printf("table: TBL_IF returned %d entries\n", rc);
}

int
interface_match(name, interface)
char *name;
char *interface;
{
	register char *ncp,*icp;

	ncp = name;
	icp = interface;

	/*
	 * If the _<node> or <node> construct was specified on the command
	 * line, then it must match on the name. Otherwise, a name like em0
	 * will match <0>em0, <1>em0, etc.
	 */
	if (icp[0] != '_' && icp[0] != '<') {
		if (ncp[0] == '<' || ncp[1] == '<') {
			while (ncp[0]) {
				if (ncp[0] == '>') {
					ncp++;
					break;
				}
				ncp++;
			}
		}
	}

	return(strcmp(icp,ncp));

}


#define	MAXIF	10
struct	iftot {
	char	ift_name[16];		/* interface name */
	int	ift_ip;			/* input packets */
	int	ift_ie;			/* input errors */
	int	ift_op;			/* output packets */
	int	ift_oe;			/* output errors */
	int	ift_co;			/* collisions */
	int	ift_dr;			/* drops */
} iftot[MAXIF];

u_char	signalled;			/* set if alarm goes off "early" */

/*
 * Called if an interval expires before sidewaysintpr has completed a loop.
 * Sets a flag to not wait for the alarm.
 */
void catchalarm()
{
	signalled = YES;
}

/*
 * Print a running summary of interface statistics.
 * Repeat display every interval seconds, showing statistics
 * collected over that interval.  Assumes that interval is non-zero.
 * First line printed at top of screen is always cumulative.
 */
sidewaysintpr(interval, off)
	unsigned interval;
	off_t off;
{
	struct ifnet ifnet, *ifp = &ifnet;
	off_t firstifnet;
	register struct iftot *ip, *total;
	register int line;
	struct iftot *lastif, *sum, *interesting;
	int oldmask;
#ifdef OSF1_ADFS
	struct tbl_if tbl_if;
	int rc = 0, if_cnt;
#endif

	if (kmem >= 0) {
		klseek(kmem, off, 0);
		read(kmem, (char *)&firstifnet, sizeof (off_t));
		off = firstifnet;
	} else {
		if_cnt = 0;
		ifp = &tbl_if.if_net;
	}
	lastif = iftot;
	sum = iftot + MAXIF - 1;
	total = sum - 1;
	interesting = iftot;
	ip = iftot;
	for (;;) {
		char *cp;

		ip->ift_name[0] = '(';
		if (kmem >= 0) {
			if (!off)
				break;
			klseek(kmem, off, 0);
			read(kmem, (char *)&ifnet, sizeof ifnet);
			klseek(kmem, (off_t)ifnet.if_name, 0);
			read(kmem, ip->ift_name + 1, 15);
		} else {
			rc = table(TBL_IF, if_cnt++, &tbl_if, 1,
				sizeof(struct tbl_if));
			if (rc != 1)
				break;
			bcopy(&tbl_if.if_name[0], ip->ift_name + 1, 15);
		}
		if (interface && strcmp(ip->ift_name + 1, interface) == 0 &&
		    unit == ifp->if_unit)
			interesting = ip;
		ip->ift_name[15] = '\0';
		cp = index(ip->ift_name, '\0');
		sprintf(cp, "%d)", ifp->if_unit);
		ip++;
		if (ip >= iftot + MAXIF - 2)
			break;
		if (kmem >= 0)
			off = (off_t) ifnet.if_next;
	}
	if (rc < 0) {
		perror("table: TBL_IF");
		return;
	}
	/* we either ran out of interfaces, or hit MAXIF */
	if (rc != 0 && rc != 1) {
		printf("table: TBL_IF returned %d entries, expected 0 or 1\n",
			rc);
		return;
	}

	lastif = ip;

	(void)signal(SIGALRM, catchalarm);
	signalled = NO;
	(void)alarm(interval);
banner:
	printf("  input  %-13.13s output       ", interesting->ift_name);
	if (lastif - iftot > 0) {
		if (dflag)
			printf("      ");
		printf("     input   (Total)   output");
	}
	for (ip = iftot; ip < iftot + MAXIF; ip++) {
		ip->ift_ip = 0;
		ip->ift_ie = 0;
		ip->ift_op = 0;
		ip->ift_oe = 0;
		ip->ift_co = 0;
		ip->ift_dr = 0;
	}
	putchar('\n');
	printf("%8.8s %5.5s %8.8s %5.5s %5.5s ",
		"packets", "errs", "packets", "errs", "colls");
	if (dflag)
		printf("%5.5s ", "drops");
	if (lastif - iftot > 0)
		printf(" %8.8s %5.5s %8.8s %5.5s %5.5s",
			"packets", "errs", "packets", "errs", "colls");
	if (dflag)
		printf(" %5.5s", "drops");
	putchar('\n');
	fflush(stdout);
	line = 0;
loop:
	sum->ift_ip = 0;
	sum->ift_ie = 0;
	sum->ift_op = 0;
	sum->ift_oe = 0;
	sum->ift_co = 0;
	sum->ift_dr = 0;
	if (kmem >= 0)
		off = firstifnet;
	else
		if_cnt = 0;
	for (ip = iftot; ip < lastif; ip++) {
		if (kmem >= 0) {
			if (!off)
				break;
			klseek(kmem, off, 0);
			read(kmem, (char *)&ifnet, sizeof ifnet);
		} else {
			rc = table(TBL_IF, if_cnt++, &tbl_if, 1,
				sizeof(struct tbl_if));
			if (rc != 1)
				break;
		}
		if (ip == interesting) {
			printf("%8d %5d %8d %5d %5d",
				ifp->if_ipackets - ip->ift_ip,
				ifp->if_ierrors - ip->ift_ie,
				ifp->if_opackets - ip->ift_op,
				ifp->if_oerrors - ip->ift_oe,
				ifp->if_collisions - ip->ift_co);
			if (dflag)
				printf(" %5d",
				    ifp->if_snd.ifq_drops - ip->ift_dr);
		}
		ip->ift_ip = ifp->if_ipackets;
		ip->ift_ie = ifp->if_ierrors;
		ip->ift_op = ifp->if_opackets;
		ip->ift_oe = ifp->if_oerrors;
		ip->ift_co = ifp->if_collisions;
		ip->ift_dr = ifp->if_snd.ifq_drops;
		sum->ift_ip += ip->ift_ip;
		sum->ift_ie += ip->ift_ie;
		sum->ift_op += ip->ift_op;
		sum->ift_oe += ip->ift_oe;
		sum->ift_co += ip->ift_co;
		sum->ift_dr += ip->ift_dr;
		if (kmem >= 0)
			off = (off_t) ifnet.if_next;
	}
	if (rc < 0) {
		perror("table: TBL_IF");
		return;
	}
	if (rc != 1) {
		printf("table: TBL_IF returned %d entries, expected 1\n", rc);
		return;
	}

	if (lastif - iftot > 0) {
		printf("  %8d %5d %8d %5d %5d",
			sum->ift_ip - total->ift_ip,
			sum->ift_ie - total->ift_ie,
			sum->ift_op - total->ift_op,
			sum->ift_oe - total->ift_oe,
			sum->ift_co - total->ift_co);
		if (dflag)
			printf(" %5d", sum->ift_dr - total->ift_dr);
	}
	*total = *sum;
	putchar('\n');
	fflush(stdout);
	line++;
	oldmask = sigblock(sigmask(SIGALRM));
	if (! signalled) {
		sigpause(0);
	}
	sigsetmask(oldmask);
	signalled = NO;
	(void)alarm(interval);
	if (line == 21)
		goto banner;
	goto loop;
	/*NOTREACHED*/
}
