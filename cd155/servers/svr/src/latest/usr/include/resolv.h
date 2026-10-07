/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
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
 * $Log: resolv.h,v $
 * Revision 1.3  1994/11/18  21:10:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:23  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:51  nandy
 * Initial revision
 *
 * Revision 2.7  92/04/13  14:38:59  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.7  90/10/07  20:34:13  devrcs
 * 	Removed CMU conditionals.
 * 	[90/10/01  15:58:22  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  20:55:28  gm]
 * 
 * Revision 2.6  90/08/24  13:53:17  devrcs
 * 	fix prototyping in functions: changed 'char' to 'unsigned char'
 * 	[90/08/08  08:55:00  jrieden]
 * 
 * Revision 2.5  90/07/27  10:42:20  devrcs
 * 	additions for BSD4.4reno
 * 	[90/07/19  08:07:25  jrieden]
 * 
 * Revision 2.4  90/03/13  21:23:22  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  17:59:38  tom]
 * 
 * Revision 2.3  90/01/02  19:15:45  gm
 * 	     Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  10:10:55  gm
 * 	     Added function prototype declarations and CMU specific
 * 	     definitions.
 * 	     [89/06/30  12:00:36  bww]
 * 
 * $EndLog$
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
