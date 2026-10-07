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
 * $Log: netdb.h,v $
 * Revision 1.3  1994/11/18  21:10:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:18  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:11  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:46  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:21  nandy
 * Initial revision
 *
 * Revision 2.6  92/04/13  14:38:11  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.6  90/10/07  20:32:34  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:53:02  gm]
 * 
 * Revision 2.5  90/08/09  14:27:15  devrcs
 * 	addition of PATHS for BSD4.4 reno
 * 	[90/08/07  08:40:44  jrieden]
 * 
 * Revision 2.4  90/03/13  21:23:11  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  17:50:04  tom]
 * 
 * Revision 2.3  90/01/02  19:15:22  gm
 * 	     Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  10:10:40  gm
 * 	     Added function prototype declarations.
 * 	     [89/06/30  11:58:54  bww]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *      @(#)netdb.h     5.10 (Berkeley) 6/27/88
 */


#ifndef _NETDB_H_
#define _NETDB_H_

#define _PATH_HEQUIV    "/etc/hosts.equiv"
#define _PATH_HOSTS     "/etc/hosts"
#define _PATH_NETWORKS  "/etc/networks"
#define _PATH_PROTOCOLS "/etc/protocols"
#define _PATH_SERVICES  "/etc/services"

/*
 * Structures returned by network data base library.  All addresses are
 * supplied in host order, and returned in network order (suitable for
 * use in system calls).
 */
struct	hostent {
	char	*h_name;	/* official name of host */
	char	**h_aliases;	/* alias list */
	int	h_addrtype;	/* host address type */
	int	h_length;	/* length of address */
	char	**h_addr_list;	/* list of addresses from name server */
#define	h_addr	h_addr_list[0]	/* address, for backward compatiblity */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 */
struct	netent {
	char		*n_name;	/* official name of net */
	char		**n_aliases;	/* alias list */
	int		n_addrtype;	/* net address type */
	unsigned long	n_net;		/* network # */
};

struct	servent {
	char	*s_name;	/* official service name */
	char	**s_aliases;	/* alias list */
	int	s_port;		/* port # */
	char	*s_proto;	/* protocol to use */
};

struct	protoent {
	char	*p_name;	/* official protocol name */
	char	**p_aliases;	/* alias list */
	int	p_proto;	/* protocol # */
};

#ifdef _NO_PROTO
struct hostent	*gethostbyname(), *gethostbyaddr(), *gethostent();
struct netent	*getnetbyname(), *getnetbyaddr(), *getnetent();
struct servent	*getservbyname(), *getservbyport(), *getservent();
struct protoent	*getprotobyname(), *getprotobynumber(), *getprotoent();
#else  /* _NO_PROTO */
extern struct hostent  *gethostbyname(const char*);
extern struct hostent  *gethostbyaddr(const char*, int, int);
extern struct hostent  *gethostent(void);
extern struct netent   *getnetbyname(const char*);
extern struct netent   *getnetbyaddr(long, int);
extern struct netent   *getnetent(void);
extern struct servent  *getservbyname(const char*, const char*);
extern struct servent  *getservbyport(int, const char*);
extern struct servent  *getservent(void);
extern struct protoent *getprotobyname(const char*);
extern struct protoent *getprotobynumber(int);
extern struct protoent *getprotoent(void);
extern void sethostent(int);
extern void endhostent(void);
#endif /* _NO_PROTO */

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */
extern	int h_errno;

#define	HOST_NOT_FOUND	1 /* Authoritative Answer Host not found */
#define	TRY_AGAIN	2 /* Non-Authoritive Host not found, or SERVERFAIL */
#define	NO_RECOVERY	3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define	NO_DATA		4 /* Valid name, no data record of requested type */
#define	NO_ADDRESS	NO_DATA		/* no address, look for MX record */

#endif /* _NETDB_H_ */
