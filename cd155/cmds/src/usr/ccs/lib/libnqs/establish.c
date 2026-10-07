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
 
/*++ establish.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/establish.c,v $
 *
 * DESCRIPTION:
 *
 *	Establish a network connection with a remote NQS network
 *	daemon. Give our caller a SIGPIPE handler.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Robert W. Sandstrom.
 *	Sterling Software Incorporated.
 *	April 21, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/12/07 22:12:15 $ $State: Exp $)
 * $Log: establish.c,v $
 * Revision 1.4  1994/12/07  22:12:15  davidl
 *  Code-reviewed and cleaned up all socket code in MACS and NQS:
 *
 *  * Added some bzero() calls for sockaddr_in structures before bind()
 *    and accept() operations.
 *
 *  * Changed some occurrences of gethostbyname(host) (which can cause
 *    failures on systems with multiple Ethernets) to
 *    gethostbyname("localhost").
 *
 *  * Removed some dead (never executed) code.
 *
 *  * Replaced a few hard-coded 0's with appropriate socket #defines.
 *
 *  Reviewer: doyle
 *  Risk: Medium (has not been tested with multiple Enets)
 *  Benefit or PTS #: 11659
 *  Testing: EATs (only)
 *  Module(s):
 *
 *   MACS:	cmds_libs/src/usr/ccs/lib/libmacs/open.c
 * 	cmds_libs/src/usr/lib/macs/macd.c
 * 	cmds_libs/src/usr/lib/macs/ports.c
 *
 *   NQS:	cmds_libs/src/usr/ccs/lib/libnqs/establish.c
 * 	cmds_libs/src/usr/ccs/lib/libnqs/inter.c
 * 	cmds_libs/src/usr/lib/nqs/netdaemon.c
 * 	cmds_libs/src/usr/lib/nqs/res_msg.c
 * 	cmds_libs/src/usr/lib/nqs/smd_msg.c
 *
 * Revision 1.3  1994/11/19  02:26:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:15:57  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:30  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:48:12  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)establish.c	1.2 (establish.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#if	NETWORKED
#if	STREAM_SOCKETS

#include <stdio.h>
#include "nqs.h"			/* NQS types and definitions */
					/* + sys/types.h + nmap.h + quolim.h */
#include "informcc.h"			/* Information completion codes */
#include "transactcc.h"			/* Transaction completion codes */
#include "netpacket.h"			/* Network packet types */
#include NETDB				/* Network database */
#include <signal.h>			/* For SIGPIPE */
#include <pwd.h>			/* Password file formats */
#include <errno.h>			/* System call failure errnos */
#include <sys/socket.h>			/* Socket stuff */
#include <netinet/in.h>			/* Internet stuff */
#if	SGI | SYS52 | UTS | UNICOS 
#include <time.h>
extern char *tzname [2];
extern void tzset();			/* Set local timezone */
#else
#if	BSD42 | BSD43 | ULTRIX  | OSF
#include <sys/time.h>
#if !OSF
extern char *timezone();
#endif
#else
BAD SYSTEM TYPE
#endif
#endif

/*
 *	External variables.
 */
extern int errno;			/* System call error codes */
/*
 *	External functions.
 */
extern char *asciierrno();		/* Return string with errno */
extern char *asctime();			/* Format time */
extern struct passwd *fetchpwuid();	/* NQS version of getpwuid() */
extern uid_t getuid();			/* Get effective user-id */
extern int getsockch();			/* Get char from socket */
#if	NET_ORDER
#else
extern unsigned short htons();		/* A macro if NET_ORDER, */
					/* a routine otherwise */
#endif
extern int interfmt();			/* Put data in packet buffer */
extern int intern32i();			/* Get # of ints */
extern int internstr();			/* Get # of strings */
extern long interr32i();		/* Get an int */
extern int interread();			/* Read a packet */
extern char *interrstr();		/* Get a string */
extern void interw32i();		/* Write 32-bit precision int datum */
extern void interw32u();		/* Write 32-bit precision int datum */
extern void interwstr();		/* Write string for message packet */
extern int localmid();			/* Get local machine-id */
extern struct tm *localtime ();		/* Get local time */
extern char *nmap_get_nam();		/* Get principal name for mid */
extern void setsockfd();		/* Set socket file descriptor */
extern int strcmp();			/* String compare */
#if	SGI | SYS52 | UTS | UNICOS 
extern long time();			/* GMT time */
extern long timezone;			/* Difference in seconds between */
					/* local time and GMT time */
#else
#if	BSD42 | BSD43 | ULTRIX | OSF
extern int gettimeofday();		/* GMT time and then some */
#else
BAD SYSTEM TYPE
#endif
#endif

/*** establish
 *
 *
 *	int establish():
 *
 *	Establish a network connection with a remote NQS network
 *	daemon.
 *
 *	Returns:
 *		>= 0: if successful; the value returned is the
 *		      "file" descriptor associated with the connection.
 *
 *		-1:   if the connection failed, and we should NOT retry.
 *
 *		-2:   if the connection failed, and we should retry
 *			after some seconds or minutes.
 */
int establish (packettype, targetmid, cuid, cusername, transactcc)
int packettype;				/* Network packet/transaction type */
mid_t targetmid;			/* Machine-id of target */
uid_t cuid;				/* Client user id */
char *cusername;			/* Client username */
long *transactcc;			/* To hold completion code */
{
	void brokenconnection();
	
#if	BSD42 | BSD43 | ULTRIX | OSF
	struct timeval tv;
	struct timezone tz;
#else
#if	SGI | SYS52 | UNICOS | UTS  
#else
BAD SYSTEM TYPE
#endif
#endif
	mid_t hostmid;			/* Local host machine-id */
	char *targetname;		/* Hostname of target machine */
	long elapsed;			/* Seconds since 1970 */
	char *tzn;			/* Time zone name */
	struct tm *tp;			/* Time structure pointer */
	char netpassword [MAX_NETPASSWORD+1];
					/* Network password */
	struct sockaddr_in rsin;	/* Remote Internet socket address */
	struct sockaddr_in lsin;	/* Local Internet socket address */
	struct servent *servent;	/* NQS service entry */
	struct hostent *hostent;	/* Host entry for target */
	struct passwd *passwd;		/* Client's password */
	int socket_descr;		/* Socket descriptor */
	char packet [MAX_PACKET];	/* Network message packet */
	int packetsize;			/* Size of network packet */
	short tryport;			/* Try to bind to this port */
	short retry;			/* Boolean retry flag */
	int timeout;			/* Seconds before next connect() */
	int integers;			/* # of 32 bit integers in packet */
	int strings;			/* # of strings in packet */

	/*
	 *  Get the principal name of the target machine.
	 */
	targetname = nmap_get_nam (targetmid);
	if (targetname == (char *) 0) {
		/*
		 *  Cannot determine hostname for mid.
		 */
		*transactcc = TCML_NETDBERR;
		return (-1);
	}
	if (localmid (&hostmid) != 0) {
		/*
		 *  Our attempt at determining the machine-id of the local
		 *  host was unsuccessful.
		 */
		*transactcc = TCML_SELMIDUNKN;
		return (-1);
	}
	servent = getservbyname ("nqs", (char *) 0);
	if (servent == (struct servent *) 0) return (-1);
	/*
	 *  Determine the address of the host that we are trying
	 *  to reach.
	 */
	hostent = gethostbyname (targetname);
	if (hostent == (struct hostent *) 0) {
		*transactcc = TCML_NETDBERR;
		return (-1);
	}
	/*
	 * Verify the existence of the client we are claiming to be.
	 */
	if ((passwd = fetchpwuid (cuid)) == (struct passwd *) 0) {
		*transactcc = TCML_NOACCAUTH;
		return (-1);
	}
	if (strcmp (passwd->pw_name, cusername)) {
		*transactcc = TCML_NOACCAUTH;
		return (-1);
	}
#if	SGI | SYS52 | UTS | UNICOS | OSF
	tzset();			/* Get local timezone */
	time (&elapsed);		/* Fill in elapsed */
	tp = localtime (&elapsed);	/* Convert to struct tm */
	tzn = tzname [tp->tm_isdst];	/* Tm_isdst is 0 or 1 */
#else
#if	BSD42 | BSD43 | ULTRIX 
	gettimeofday (&tv, &tz);	/* Elapsed secs in tv, zone in tz */
	tp = localtime (&tv.tv_sec);	/* Convert to struct tm */
	tzn = timezone (tz.tz_minuteswest, tp->tm_isdst);
#else
BAD SYSTEM TYPE
#endif
#endif
	if (tzn == (char *) 0) tzn = "";
	/*
	 *  Obtain the network password to access the remote NQS machine.
	 */
	netpassword [0] = '\0';		/* NQS password access is not yet */
					/* implemented */
	/*
	 * Clobber whatever SIGPIPE handler our caller has,
	 * and put in one that always returns errno == EPIPE.
	 */
	signal (SIGPIPE, brokenconnection);
	/*
	 *  Append the necessary trailer information to the network
	 *  transaction packet, that is common to ALL NQS network
	 *  transaction packets.
	 */
	interw32u ((unsigned long) NPK_MAGIC1);
	interw32i ((long) packettype);	/* Write packet transaction type */
	interwstr (netpassword);	/* Attach network password to packet */
	interw32i ((long) hostmid);	/* Attach "from-mid" to packet */
	interw32i ((long) targetmid);	/* Attach "to-mid" to packet */
	interw32i ((long) cuid);	/* Attach client's uid to packet */
	interwstr (cusername);		/* Attach client's username to packet */
#if	SGI | SYS52 | UTS | UNICOS | OSF
	interw32i ((long) timezone);	/* Write timezone difference from */
					/* GMT in seconds */
#else
#if	BSD42 | BSD43 | ULTRIX
	interw32i ((long) tz.tz_minuteswest * 60L);
					/* Write our timezone difference */
					/* from GMT in seconds */
#else
BAD SYSTEM TYPE
#endif
#endif
	interwstr (tzn);		/* Write our timezone name */
	/*
	 *  Now that we have defined the network transaction packet
	 *  contents, format the packet.
	 */
	if ((packetsize = interfmt (packet)) == -1) {
		/*
		 *  The packet contents create a packet that is too
		 *  large.  The packet cannot be sent.
		 */
		*transactcc = TCML_INTERNERR;
		return (-1);
	}
	/*
	 *  Store the address of the host we are trying to reach
	 *  in the remote socket address structure.
	 */
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	bzero ((char *) &rsin, sizeof (struct sockaddr_in));
	bytecopy ((char *) &rsin.sin_addr.s_addr, (char *) hostent->h_addr,
		hostent->h_length);
#else
#if	UNICOS
	{
		unsigned long castableaddr;
		bytecopy ((char *) &castableaddr, (char *) hostent->h_addr,
			hostent->h_length);
		rsin.sin_addr = castableaddr;
	}
#else
BAD SYSTEM TYPE
#endif
#endif
	rsin.sin_family = hostent->h_addrtype;
	rsin.sin_port = servent->s_port;
	
	
	/*
	 *  Store the address of the local socket we will be using
	 *  in the local socket address structure.
	 */
	tryport = IPPORT_RESERVED -1;
	timeout = 1;
	bzero ((char *)&lsin, sizeof (struct sockaddr_in));
	lsin.sin_family = AF_INET;
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	lsin.sin_addr.s_addr = INADDR_ANY;
#else
#if	UNICOS
	lsin.sin_addr = 0;
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Try to connect to the remote machine.
	 *  If it is likely that an immediate retry would succeed, go
	 *  ahead and do the retry right now, rather than returning -2.
	 */
	do {
		retry = 0;		/* Adopt an optimistic attitude */
		socket_descr = socket (AF_INET, SOCK_STREAM, 0);
		if (socket_descr == -1) {
			*transactcc = errnototcm (errno);
			if (errno == ENOBUFS) return (-2);
			else return (-1);
		}
		/*
		 *  Try to bind this socket to a reserved port.
		 *  Without a reserved port, the NQS netserver on a
		 *  remote machine would suspect that we were trying
		 *  to pull a fast one.
		 */
		lsin.sin_port = htons (tryport);
		while (bind (socket_descr, &lsin, sizeof (lsin)) < 0) {
			/*
			 *  The bind() failed.
			 *  Try another port.
			 */
			if (errno != EADDRINUSE && errno != EADDRNOTAVAIL) {
				*transactcc = errnototcm (errno);
				return (-1);
			}
			tryport--;
			if (tryport <= IPPORT_RESERVED/2) {
				*transactcc = TCML_NOPORTAVAI;
				return (-2);
			}
			lsin.sin_port = htons(tryport);
		}
		/*
		 *  Connect to the NQS network daemon on the remote machine.
		 */
		if (connect (socket_descr, (char *) &rsin,
			     sizeof (rsin)) == -1) {
			/*
			 *  We were unable to connect to the remote NQS
			 *  network daemon.
			 */
			switch (errno) {
			case EADDRINUSE:
				close (socket_descr);
				tryport--;
				retry = 1;	/* Retry */
				break;
			case ECONNREFUSED:
				if (timeout <= 16) {
					close (socket_descr);
					sleep (timeout);
					timeout *= 2;
					retry = 1;	/* Retry */
				}
				else {
					*transactcc = TCMP_NONETDAE;
					return (-1);
				}
				break;
			case ETIMEDOUT:
				*transactcc = TCML_ETIMEDOUT;
				return (-1);
			default:
				*transactcc = TCMP_NOESTABLSH;
				return (-1);
			}
		}
	} while (retry);
	if (write (socket_descr, packet, packetsize) != packetsize) {
		*transactcc = TCMP_CONNBROKEN;
		return (-1);
	}
	/*
	 * Before returning, check that the server liked us.
	 */
	setsockfd (socket_descr);
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		close (socket_descr);
		*transactcc = TCMP_CONNBROKEN;
		return (-1);
	case -2:
		close (socket_descr);
		*transactcc = TCMP_PROTOFAIL;
		return (-1);
	}
	integers = intern32i();			
	if (integers < 1) {
		close (socket_descr);
		*transactcc = TCMP_PROTOFAIL;
		return (-1);
	}
	*transactcc = interr32i (1);
	switch (*transactcc & XCI_FULREA_MASK) {
	/*
	 * Note that code outside the switch passes the TCM to our caller.
	 */
	case TCMP_CONTINUE:
		return (socket_descr);
	case TCMP_ERRORRETRY:
		/*
		 * We might have remote servers send strings
		 * as well as TCM's.
		 */
		strings = internstr();
		if (strings < 1) {
		}
		return (-1);
	case TCMP_FATALABORT:
		strings = internstr();
		if (strings < 1) {
		}
		return (-1);
	case TCMP_CLIMIDUNKN:
	case TCMP_MAXNETCONN:
	case TCMP_MIDCONFLCT:
	case TCMP_NETPASSWD:
	case TCMP_NOACCAUTH:
	case TCMP_NOMOREPROC:
	case TCMP_NONSECPORT:
	case TCMP_PROTOFAIL:
	case TCMP_UNAFAILURE:
		return (-1);
	default:
		return (-1);
	}
}


/*** brokenconnection
 *
 *	void brokenconnection():
 *	Allow writes on broken connections to return -1 
 *	instead of exiting.
 */
static void brokenconnection ()
{
	signal (SIGPIPE, brokenconnection);
	errno = EPIPE;
}


#else
BAD NETWORK TYPE
#endif	STREAM_SOCKETS
#endif	NETWORKED
