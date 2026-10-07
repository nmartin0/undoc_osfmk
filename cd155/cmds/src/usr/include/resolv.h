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
/*
 * Copyright (c) 1983, 1987 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(##)resolv.h	5.10 (Berkeley) 6/1/90
 */

#ifndef _RESOLV_H_
#define _RESOLV_H_

#ifndef _PATH_RESCONF
#define _PATH_RESCONF        "/etc/resolv.conf"
#endif

/*
 * Global defines and variables for resolver stub.
 */


#define	MAXNS		3		/* max # name servers we'll track */
#define MAXDFLSRCH      3               /* # default domain levels to try */
#define MAXDNSRCH       6               /* max # domains in search path */
#define	LOCALDOMAINPARTS 2		/* min levels in name that is "local" */

#define	RES_TIMEOUT	5		/* seconds between retries */

struct state {
	int	retrans;	 	/* retransmition time interval */
	int	retry;			/* number of times to retransmit */
	long	options;		/* option flags - see below. */
	int	nscount;		/* number of name servers */
	struct	sockaddr_in nsaddr_list[MAXNS];	/* address of name server */
#define	nsaddr	nsaddr_list[0]		/* for backward compatibility */
	u_short	id;			/* current packet id */
	char	defdname[MAXDNAME];	/* default domain */
	char	*dnsrch[MAXDNSRCH+1];	/* components of domain to search */
};

/*
 * Resolver options
 */
#define RES_INIT	0x0001		/* address initialized */
#define RES_DEBUG	0x0002		/* print debug messages */
#define RES_AAONLY	0x0004		/* authoritative answers only */
#define RES_USEVC	0x0008		/* use virtual circuit */
#define RES_PRIMARY	0x0010		/* query primary server only */
#define RES_IGNTC	0x0020		/* ignore trucation errors */
#define RES_RECURSE	0x0040		/* recursion desired */
#define RES_DEFNAMES	0x0080		/* use default domain name */
#define RES_STAYOPEN	0x0100		/* Keep TCP socket open */
#define RES_DNSRCH	0x0200		/* search up local domain tree */

#define RES_DEFAULT	(RES_RECURSE | RES_DEFNAMES | RES_DNSRCH)

extern struct state _res;

#ifdef _NO_PROTO
extern char *p_cdname(), *p_rr(), *p_type(), *p_class();

#else /* _NO_PROTO */
#include <stdio.h>
#include <arpa/nameser.h>
extern char *p_cdname(char *, char *, FILE *);
extern char *p_rr(char *, char *, FILE *);
extern char *p_type(int);
extern char *p_class(int);
extern int res_mkquery(int, char *, int, int, char *, int, struct rrec *, char *, int);
extern int res_send(char *,int, char *, int);
extern int res_init(void);
extern int dn_comp(unsigned char *, unsigned char *, int, unsigned char **, unsigned char **);
extern int dn_expand(unsigned char *, unsigned char *, unsigned char *, unsigned char *, int);
#endif /* _NO_PROTO */

 
#endif /* _RESOLV_H_ */
