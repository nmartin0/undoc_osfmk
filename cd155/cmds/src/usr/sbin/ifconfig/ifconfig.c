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
/* @(#)ifconfig.c	4.1 23:48:02 7/12/90 SecureWare */
/*
 * Copyright (c) 1983 Regents of the University of California.
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
char copyright[] =
"@(#) Copyright (c) 1983 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
/* Update to some of: "@(#)ifconfig.c	4.30 (Berkeley) 6/14/90"; */
static char sccsid[] = "@(#)ifconfig.c	4.25 (Berkeley) 9/25/89";
#endif /* not lint */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif

#include <sys/types.h>
#define	_SOCKADDR_LEN
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <netinet/in.h>

#define	NSIP
#include <netns/ns.h>
#include <netns/ns_if.h>

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>

extern int errno;
struct	ifreq		ifr, ridreq;
struct	ifaliasreq	addreq;
struct	sockaddr_in	netmask;

char	name[30];
int	flags;
int	metric;
int	setaddr = 0;
int	setipdst = 0;
int	doalias = 0;
int	clearaddr = 0;
int	iffup = 0;
int	s;
extern	int errno;

int	setifflags(), setifaddr(), setifdstaddr(), setifnetmask();
int	setifmetric(), setifbroadaddr(), setifipdst();
int	notealias();

#define	NEXTARG		0xffffff

struct	cmd {
	char	*c_name;
	int	c_parameter;		/* NEXTARG means next argv */
	int	(*c_func)();
} cmds[] = {
	{ "up",		IFF_UP,		setifflags },
	{ "down",	-IFF_UP,	setifflags },
	{ "trailers",	-IFF_NOTRAILERS,setifflags },
	{ "-trailers",	IFF_NOTRAILERS,	setifflags },
	{ "arp",	-IFF_NOARP,	setifflags },
	{ "-arp",	IFF_NOARP,	setifflags },
	{ "debug",	IFF_DEBUG,	setifflags },
	{ "-debug",	-IFF_DEBUG,	setifflags },
	{ "snap",	IFF_SNAP,	setifflags },
	{ "-snap",	-IFF_SNAP,	setifflags },
	{ "alias",	IFF_UP,		notealias },
	{ "-alias",	-IFF_UP,	notealias },
	{ "delete",	-IFF_UP,	notealias },
#ifdef notdef
#define	EN_SWABIPS	0x1000
	{ "swabips",	EN_SWABIPS,	setifflags },
	{ "-swabips",	-EN_SWABIPS,	setifflags },
#endif
	{ "netmask",	NEXTARG,	setifnetmask },
	{ "metric",	NEXTARG,	setifmetric },
	{ "broadcast",	NEXTARG,	setifbroadaddr },
	{ "ipdst",	NEXTARG,	setifipdst },
	{ 0,		0,		setifaddr },
	{ 0,		0,		setifdstaddr },
};

/*
 * XNS support liberally adapted from
 * code written at the University of Maryland
 * principally by James O'Toole and Chris Torek.
 */
int	in_status(), in_getaddr();
int	xns_status(), xns_getaddr();

/* Known address families */
struct afswtch {
	char *af_name;
	short af_af;
	int (*af_status)();
	int (*af_getaddr)();
	int af_difaddr;
	int af_aifaddr;
	caddr_t af_ridreq;
	caddr_t af_addreq;
} afs[] = {
#define C(x) ((caddr_t) &x)
	{ "inet", AF_INET, in_status, in_getaddr,
	     SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq) },
	{ "ns", AF_NS, xns_status, xns_getaddr,
	     SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq) },
	{ 0,	0,	    0,		0 }
};

struct afswtch *afp;	/*the address family being set or asked about*/

int is_if_present();

main(argc, argv)
	int argc;
	char *argv[];
{
	int af = AF_INET;
	register struct afswtch *rafp;

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
#endif
	if (argc < 2) {
		fprintf(stderr, "usage: ifconfig interface\n%s%s%s%s%s",
		    "\t[ af [ address [ dest_addr ] ] [ up ] [ down ]",
			    "[ netmask mask ] ]\n",
		    "\t[ metric n ]\n",
		    "\t[ trailers | -trailers ]\n",
		    "\t[ arp | -arp ]\n");
		exit(1);
	}
	argc--, argv++;
	strncpy(name, *argv, sizeof(name));
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	argc--, argv++;
	if (argc > 0) {
		for (afp = rafp = afs; rafp->af_name; rafp++)
			if (strcmp(rafp->af_name, *argv) == 0) {
				afp = rafp; argc--; argv++;
				break;
			}
		rafp = afp;
		af = ifr.ifr_addr.sa_family = rafp->af_af;
	}
	s = socket(af, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("ifconfig: socket");
		exit(1);
	}
	if (ioctl(s, SIOCGIFFLAGS, (caddr_t)&ifr) < 0) {
		Perror("ioctl (SIOCGIFFLAGS)");
		exit(1);
	}
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	flags = ifr.ifr_flags;
	if (ioctl(s, SIOCGIFMETRIC, (caddr_t)&ifr) < 0)
		perror("ioctl (SIOCGIFMETRIC)");
	else
		metric = ifr.ifr_metric;
	if (argc == 0) {
		status();
		exit(0);
	}
#if SEC_BASE
	if (!authorized_user("sysadmin")) {
		fprintf(stderr, "ifconfig: need sysadmin authorization\n");
		exit(1);
	}
	if (!forcepriv(SEC_REMOTE)) {
		fprintf(stderr, "ifconfig: insufficient privileges\n");
		exit(1);
	}
#endif
	while (argc > 0) {
		register struct cmd *p;

		for (p = cmds; p->c_name; p++)
			if (strcmp(*argv, p->c_name) == 0)
				break;
		if (p->c_name == 0 && setaddr)
			p++;	/* got src, do dst */
		if (p->c_func) {
			if (p->c_parameter == NEXTARG) {
				(*p->c_func)(argv[1]);
				argc--, argv++;
			} else
				(*p->c_func)(*argv, p->c_parameter);
		}
		argc--, argv++;
	}

	if (setaddr && is_if_present()) {
		fprintf(stderr,
		 "ifconfig: Reconfiguration not permitted.\n");
		exit(1);
	}

	if (setipdst && af==AF_NS) {
		struct nsip_req rq;
		int size = sizeof(rq);

		rq.rq_ns = addreq.ifra_addr;
		rq.rq_ip = addreq.ifra_dstaddr;

		if (setsockopt(s, 0, SO_NSIP_ROUTE, &rq, size) < 0)
			Perror("Encapsulation Routing");
	}
	if (clearaddr) {
		int ret;
		strncpy(rafp->af_ridreq, name, sizeof ifr.ifr_name);
		if ((ret = ioctl(s, rafp->af_difaddr, rafp->af_ridreq)) < 0) {
			if (errno == EADDRNOTAVAIL && (doalias >= 0)) {
				/* means no previous address for interface */
			} else
				Perror("ioctl (SIOCDIFADDR)");
		}
	}
	if (setaddr) {
		strncpy(rafp->af_addreq, name, sizeof ifr.ifr_name);
		if (ioctl(s, rafp->af_aifaddr, rafp->af_addreq) < 0)
			Perror("ioctl (SIOCAIFADDR)");
	}
	exit(0);
}
#define RIDADDR 0
#define ADDR	1
#define MASK	2
#define DSTADDR	3


/*ARGSUSED*/
setifaddr(addr, param)
	char *addr;
	short param;
{
	/*
	 * Delay the ioctl to set the interface addr until flags are all set.
	 * The address interpretation may depend on the flags,
	 * and the flags may change when the address is set.
	 */
	setaddr++;
	if (doalias == 0)
		clearaddr = 1;
	(*afp->af_getaddr)(addr, (doalias >= 0 ? ADDR : RIDADDR));
}

setifnetmask(addr)
	char *addr;
{
	(*afp->af_getaddr)(addr, MASK);
}

setifbroadaddr(addr)
	char *addr;
{
	(*afp->af_getaddr)(addr, DSTADDR);
}

setifipdst(addr)
	char *addr;
{
	in_getaddr(addr, DSTADDR);
	setipdst++;
	clearaddr = 0;
}
#define rqtosa(x) (&(((struct ifreq *)(afp->x))->ifr_addr))
/*ARGSUSED*/
notealias(addr, param)
	char *addr;
{
	if (setaddr && doalias == 0 && param < 0)
		bcopy((caddr_t)rqtosa(af_addreq),
		      (caddr_t)rqtosa(af_ridreq),
		      rqtosa(af_addreq)->sa_len);
	doalias = param;
	if (param < 0) {
		clearaddr = 1;
	} else
		clearaddr = 0;
}

/*ARGSUSED*/
setifdstaddr(addr, param)
	char *addr;
	int param;
{
	(*afp->af_getaddr)(addr, DSTADDR);
}

setifflags(vname, value)
	char *vname;
	short value;
{
 	if (ioctl(s, SIOCGIFFLAGS, (caddr_t)&ifr) < 0) {
 		Perror("ioctl (SIOCGIFFLAGS)");
 		exit(1);
 	}
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
 	flags = ifr.ifr_flags;

	if (value < 0) {
		value = -value;
		flags &= ~value;
	} else
		flags |= value;
	if ( ifr.ifr_flags != flags ) {
		ifr.ifr_flags = flags;
		if (ioctl(s, SIOCSIFFLAGS, (caddr_t)&ifr) < 0)
			Perror(vname);
	}
}

setifmetric(val)
	char *val;
{
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	ifr.ifr_metric = atoi(val);
	if (ioctl(s, SIOCSIFMETRIC, (caddr_t)&ifr) < 0)
		perror("ioctl (set metric)");
}

#define	IFFBITS \
"\020\1UP\2BROADCAST\3DEBUG\4LOOPBACK\5POINTOPOINT\6NOTRAILERS\7RUNNING\10NOARP\
"

/*
 * Print the status of the interface.  If an address family was
 * specified, show it and it only; otherwise, show them all.
 */
status()
{
	register struct afswtch *p = afp;
	short af = ifr.ifr_addr.sa_family;

	printf("%s: ", name);
	printb("flags", flags, IFFBITS);
	if (metric)
		printf(" metric %d", metric);
	putchar('\n');
	if ((p = afp) != NULL) {
		(*p->af_status)(1);
	} else for (p = afs; p->af_name; p++) {
		ifr.ifr_addr.sa_family = p->af_af;
		(*p->af_status)(0);
	}
}

in_status(force)
	int force;
{
	struct sockaddr_in *sin;
	char *inet_ntoa();

	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR, (caddr_t)&ifr) < 0) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
			if (!force)
				return;
			bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
		} else
			perror("ioctl (SIOCGIFADDR)");
	}
	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	printf("\tinet %s ", inet_ntoa(sin->sin_addr));
	strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
	if (ioctl(s, SIOCGIFNETMASK, (caddr_t)&ifr) < 0) {
		if (errno != EADDRNOTAVAIL)
			perror("ioctl (SIOCGIFNETMASK)");
		bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
	} else
		netmask.sin_addr =
		    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
	if (flags & IFF_POINTOPOINT) {
		if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
			else
			    perror("ioctl (SIOCGIFDSTADDR)");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		sin = (struct sockaddr_in *)&ifr.ifr_dstaddr;
		printf("--> %s ", inet_ntoa(sin->sin_addr));
	}
	printf("netmask %x ", ntohl(netmask.sin_addr.s_addr));
	if (flags & IFF_BROADCAST) {
		if (ioctl(s, SIOCGIFBRDADDR, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
			else
			    perror("ioctl (SIOCGIFADDR)");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		sin = (struct sockaddr_in *)&ifr.ifr_addr;
		if (sin->sin_addr.s_addr != 0)
			printf("broadcast %s", inet_ntoa(sin->sin_addr));
	}
	putchar('\n');
}


xns_status(force)
	int force;
{
	struct ns_addr ns;

	close(s);
	s = socket(AF_NS, SOCK_DGRAM, 0);
	if (s < 0) {
		if (errno == EPROTONOSUPPORT)
			return;
		perror("ifconfig: socket");
		exit(1);
	}
	if (ioctl(s, SIOCGIFADDR, (caddr_t)&ifr) < 0) {
		if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
			if (!force)
				return;
			bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
		} else
			perror("ioctl (SIOCGIFADDR)");
	}
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
	bcopy((caddr_t)&((struct sockaddr_ns *)(&ifr.ifr_addr))->sns_addr,
		(caddr_t)&ns, sizeof (ns));
	printf("\tns %s ", ns_ntoa(ns));
	if (flags & IFF_POINTOPOINT) { /* by W. Nesheim@Cornell */
		if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
			if (errno == EADDRNOTAVAIL)
			    bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
			else
			    Perror("ioctl (SIOCGIFDSTADDR)");
		}
		strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
		bcopy((caddr_t)&((struct sockaddr_ns *)(&ifr.ifr_dstaddr))->sns_addr,
			(caddr_t)&ns, sizeof (ns));
		printf("--> %s ", ns_ntoa(ns));
	}
	putchar('\n');
}

Perror(cmd)
	char *cmd;
{
	extern int errno;

	fprintf(stderr, "ifconfig: ");
	switch (errno) {

	case ENXIO:
		fprintf(stderr, "%s: no such interface\n", cmd);
		break;

	case EPERM:
		fprintf(stderr, "%s: permission denied\n", cmd);
		break;

	default:
		perror(cmd);
	}
	exit(1);
}

struct	in_addr inet_makeaddr();

#define SIN(x) ((struct sockaddr_in *) &(x))
struct sockaddr_in *sintab[] = {
SIN(ridreq.ifr_addr), SIN(addreq.ifra_addr),
SIN(addreq.ifra_mask), SIN(addreq.ifra_broadaddr)};

in_getaddr(s, which)
	char *s;
{
	register struct sockaddr_in *sin = sintab[which];
	struct hostent *hp;
	struct netent *np;
	int val;

	sin->sin_len = sizeof(*sin);
	if (which != MASK)
		sin->sin_family = AF_INET;

	if ((val = inet_addr(s)) != -1)
		sin->sin_addr.s_addr = val;
	else if (hp = gethostbyname(s))
		bcopy(hp->h_addr, (char *)&sin->sin_addr, hp->h_length);
	else if (np = getnetbyname(s))
		sin->sin_addr = inet_makeaddr(np->n_net, INADDR_ANY);
	else {
		fprintf(stderr, "%s: bad value\n", s);
		exit(1);
	}
}

/*
 * Print a value a la the %b format of the kernel's printf
 */
printb(s, v, bits)
	char *s;
	register char *bits;
	register unsigned short v;
{
	register int i, any = 0;
	register char c;

	if (bits && *bits == 8)
		printf("%s=%o", s, v);
	else
		printf("%s=%x", s, v);
	bits++;
	if (bits) {
		putchar('<');
		while (i = *bits++) {
			if (v & (1 << (i-1))) {
				if (any)
					putchar(',');
				any = 1;
				for (; (c = *bits) > 32; bits++)
					putchar(c);
			} else
				for (; *bits > 32; bits++)
					;
		}
		putchar('>');
	}
}

#define SNS(x) ((struct sockaddr_ns *) &(x))
struct sockaddr_ns *snstab[] = {
SNS(ridreq.ifr_addr), SNS(addreq.ifra_addr),
SNS(addreq.ifra_mask), SNS(addreq.ifra_broadaddr)};

xns_getaddr(addr, which)
char *addr;
{
	struct sockaddr_ns *sns = snstab[which];

	sns->sns_family = AF_NS;
	sns->sns_len = sizeof(*sns);
	sns->sns_addr = ns_addr(addr);
	if (which == MASK)
		printf("Attempt to set XNS netmask will be ineffectual\n");
}

/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * J.Q. Johnson.
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

#if	0
#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)ns_addr.c	6.6 (Berkeley) 6/6/90";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <netns/ns.h>
#endif

static struct ns_addr addr, zero_addr;
static cvtbase(), Field();

struct ns_addr 
ns_addr(name)
#ifdef	__STDC__
	const char *name;
#else
	char *name;
#endif
{
	char separator = '.';
	char *hostname, *socketname, *cp;
	char buf[50];
	extern char *index();

	addr = zero_addr;
	(void)strncpy(buf, name, 49);

	/*
	 * First, figure out what he intends as a field separtor.
	 * Despite the way this routine is written, the prefered
	 * form  2-272.AA001234H.01777, i.e. XDE standard.
	 * Great efforts are made to insure backward compatability.
	 */
	if (hostname = index(buf, '#'))
		separator = '#';
	else {
		hostname = index(buf, '.');
		if ((cp = index(buf, ':')) &&
		    ( (hostname && cp < hostname) || (hostname == 0))) {
			hostname = cp;
			separator = ':';
		}
	}
	if (hostname)
		*hostname++ = 0;
	Field(buf, addr.x_net.c_net, 4);
	if (hostname == 0)
		return (addr);  /* No separator means net only */

	socketname = index(hostname, separator);
	if (socketname) {
		*socketname++ = 0;
		Field(socketname, (u_char *)&addr.x_port, 2);
	}

	Field(hostname, addr.x_host.c_host, 6);

	return (addr);
}

static
Field(buf, out, len)
char *buf;
u_char *out;
int len;
{
	register char *bp = buf;
	int i, ibase, base16 = 0, base10 = 0, clen = 0;
	int hb[6], *hp;
	char *fmt;

	/*
	 * first try 2-273#2-852-151-014#socket
	 */
	if ((*buf != '-') &&
	    (1 < (i = sscanf(buf, "%d-%d-%d-%d-%d",
			&hb[0], &hb[1], &hb[2], &hb[3], &hb[4])))) {
		cvtbase(1000L, 256, hb, i, out, len);
		return;
	}
	/*
	 * try form 8E1#0.0.AA.0.5E.E6#socket
	 */
	if (1 < (i = sscanf(buf,"%x.%x.%x.%x.%x.%x",
			&hb[0], &hb[1], &hb[2], &hb[3], &hb[4], &hb[5]))) {
		cvtbase(256L, 256, hb, i, out, len);
		return;
	}
	/*
	 * try form 8E1#0:0:AA:0:5E:E6#socket
	 */
	if (1 < (i = sscanf(buf,"%x:%x:%x:%x:%x:%x",
			&hb[0], &hb[1], &hb[2], &hb[3], &hb[4], &hb[5]))) {
		cvtbase(256L, 256, hb, i, out, len);
		return;
	}
	/*
	 * This is REALLY stretching it but there was a
	 * comma notation separting shorts -- definitely non standard
	 */
	if (1 < (i = sscanf(buf,"%x,%x,%x",
			&hb[0], &hb[1], &hb[2]))) {
		hb[0] = htons(hb[0]); hb[1] = htons(hb[1]);
		hb[2] = htons(hb[2]);
		cvtbase(65536L, 256, hb, i, out, len);
		return;
	}

	/* Need to decide if base 10, 16 or 8 */
	while (*bp) switch (*bp++) {

	case '0': case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '-':
		break;

	case '8': case '9':
		base10 = 1;
		break;

	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		base16 = 1;
		break;
	
	case 'x': case 'X':
		*--bp = '0';
		base16 = 1;
		break;

	case 'h': case 'H':
		base16 = 1;
		/* fall into */

	default:
		*--bp = 0; /* Ends Loop */
	}
	if (base16) {
		fmt = "%3x";
		ibase = 4096;
	} else if (base10 == 0 && *buf == '0') {
		fmt = "%3o";
		ibase = 512;
	} else {
		fmt = "%3d";
		ibase = 1000;
	}

	for (bp = buf; *bp++; ) clen++;
	if (clen == 0) clen++;
	if (clen > 18) clen = 18;
	i = ((clen - 1) / 3) + 1;
	bp = clen + buf - 3;
	hp = hb + i - 1;

	while (hp > hb) {
		(void)sscanf(bp, fmt, hp);
		bp[0] = 0;
		hp--;
		bp -= 3;
	}
	(void)sscanf(buf, fmt, hp);
	cvtbase((long)ibase, 256, hb, i, out, len);
}

static
cvtbase(oldbase,newbase,input,inlen,result,reslen)
	long oldbase;
	int newbase;
	int input[];
	int inlen;
	unsigned char result[];
	int reslen;
{
	int d, e;
	long sum;

	e = 1;
	while (e > 0 && reslen > 0) {
		d = 0; e = 0; sum = 0;
		/* long division: input=input/newbase */
		while (d < inlen) {
			sum = sum*oldbase + (long) input[d];
			e += (sum > 0);
			input[d++] = sum / newbase;
			sum %= newbase;
		}
		result[--reslen] = sum;	/* accumulate remainder */
	}
	for (d=0; d < reslen; d++)
		result[d] = 0;
}

/*
 * Copyright (c) 1986 Regents of the University of California.
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

#if	0
#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)ns_ntoa.c	6.5 (Berkeley) 6/1/90";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>
#include <netns/ns.h>
#endif

static char *spectHex();

char *
ns_ntoa(addr)
struct ns_addr addr;
{
	static char obuf[40];
	char *spectHex();
	union { union ns_net net_e; u_long long_e; } net;
	u_short port = htons(addr.x_port);
	register char *cp;
	char *cp2;
	register u_char *up = addr.x_host.c_host;
	u_char *uplim = up + 6;

	net.net_e = addr.x_net;
	sprintf(obuf, "%lx", ntohl(net.long_e));
	cp = spectHex(obuf);
	cp2 = cp + 1;
	while (*up==0 && up < uplim) up++;
	if (up == uplim) {
		if (port) {
			sprintf(cp, ".0");
			cp += 2;
		}
	} else {
		sprintf(cp, ".%x", *up++);
		while (up < uplim) {
			while (*cp) cp++;
			sprintf(cp, "%02x", *up++);
		}
		cp = spectHex(cp2);
	}
	if (port) {
		sprintf(cp, ".%x", port);
		spectHex(cp + 1);
	}
	return (obuf);
}

static char *
spectHex(p0)
char *p0;
{
	int ok = 0;
	int nonzero = 0;
	register char *p = p0;
	for (; *p; p++) switch (*p) {

	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		*p += ('A' - 'a');
		/* fall into . . . */
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		ok = 1;
	case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9':
		nonzero = 1;
	}
	if (nonzero && !ok) { *p++ = 'H'; *p = 0; }
	return (p);
}


#include <net/if_dl.h>
#undef ia_net
#include <netinet/in_var.h>

#define TABLE_IF	/* include interface structures */
#define OSF1_ADFS
#include <sys/table.h>

int
is_if_present()
{
	int kmem = -1;
	char *interface = 0;
	int unit = 0;
	u_long address;

	struct ifnet ifnet, *ifp;
	union ifaddrany ifaddr, *ifap;
	struct sockaddr *sa;
	char *name;
	off_t ifaddraddr = 0;
	struct tbl_if tbl_if;
	int if_ctr = 0;
	int ifa_ctr = 0, ifa_max = 0;
	int rc = 0;
	int newline = 0;

	for (;;) {
		struct sockaddr_in *sin;
		register char *cp;
		int n, m;
		struct in_addr in, inet_makeaddr();

		if (ifa_max == 0 || ifa_ctr >= ifa_max) {
			rc = table(TBL_IF, if_ctr++, &tbl_if, 1,
				sizeof(struct tbl_if));
			if (rc != 1)
				break;

			ifp = &tbl_if.if_net;
			name = &tbl_if.if_name[0];
			ifa_max = tbl_if.ifaddrcount;
			ifa_ctr = 0;

			newline = 1;
		}
		if (newline) {
			newline = 0;
			name[15] = '\0';
			if (interface != 0 &&
			    (strcmp(name, interface) != 0 || unit != ifp->if_unit))
				continue;

			cp = index(name, '\0');
			*cp++ = ifp->if_unit + '0';
			*cp = '\0';
		}
		ifap = &tbl_if.ifaddrs[ifa_ctr].ifa;
		sa = (struct sockaddr *)
			&tbl_if.ifaddrs[ifa_ctr].sa;

		switch (sa->sa_family) {
		case AF_INET:
			if( strcmp(name, ifr.ifr_name) == 0 )
				return(1);	/* 1 is TRUE */
			/*
				printf("%s ", name);
				sin = (struct sockaddr_in *)sa;
				in.s_addr = htonl(ifap->in.ia_subnet);
				address = htonl(sin->sin_addr.s_addr);
				printf("%u.%u.%u.%u\n",
				 ((address>>24)&0xff), ((address>>16)&0xff),
				 ((address>> 8)&0xff), ((address    )&0xff));
			 */
			break;

		case AF_LINK:
			{
			struct sockaddr_dl *sdl =
				(struct sockaddr_dl *)sa;
			    cp = (char *)LLADDR(sdl);
			    n = sdl->sdl_alen;
			}
			break;

		default:
			break;

		}
		ifa_ctr++;
	}
	/*
	 * interface was never found, so return false.
	 */
	return(0);	/* 0 is FALSE */
}
