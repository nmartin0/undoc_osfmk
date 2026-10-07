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
 
/*++ shoqbyname.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/shoqbyname.c,v $
 *
 * DESCRIPTION:
 *
 *	Print on stdout information about a queue.
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	December 18, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:57 $ $State: Exp $)
 * $Log: shoqbyname.c,v $
 * Revision 1.4  1994/11/19  02:27:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/11/02  18:39:51  kremenek
 *  Reviewer: davidl doyle
 *  Risk: low
 *  Benefit or PTS #: 11273
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/shoqbyname.c
 *
 * Revision 1.2  1992/10/09  21:59:48  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:09  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:16  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>			/* for stderr */
#include "nqs.h"			/* NQS types and definitions */
#include "netpacket.h"			/* NQS networking */
#include <pwd.h>			/* Password stuff */

extern void analyzetcm();		/* Print message */
extern char *bufsockch();		/* Ptr to extra chars from socket */
extern char *destqueue();		/* Strip "@machine" */
extern int establish();			/* Set up connection */
extern int extrasockch();		/* Number of extra chars from socket */
extern struct passwd *fetchpwuid();	/* NQS version of getpwuid */
extern long filecopyentire();		/* File descr to file descr copy */
extern void interclear();		/* Zero out inter() structs */
extern void interw32i();		/* Write a 32 bit integer */
extern void interwstr();		/* Write a string */
extern int localmid();			/* Get our mid */
extern int machspec();			/* Get an mid */
extern struct gendescr *nextdb();	/* Advance to next entry in db */
extern void nqssleep();			/* NQS version of sleep() */
extern int seekdb();			/* Seek on a db file */
extern int shoqbydesc();		/* Show queue given descriptor */
extern char *strncpy();			/* Anchored string copy */
extern long telldb();			/* Return position of last read on a */
					/* database file */

/*** shoqbyname
 *
 *
 *	int shoqbyname():
 *
 *	Print on stdout information about the named queue.
 *	Returns: 0 if output was produced.
 *		-1 if no output was produced.
 */
int shoqbyname_NOT_USED_NOW (queuefile, fullname, flags, whomuid, reqs, dae_present,
		qmapfile, pipeqfile, qcomplexfile)
struct confd *queuefile;		/* NQS queue file */
char *fullname;				/* Name with possible '@' */
long flags;				/* SHO_??? */
uid_t whomuid;				/* Whom we are interested in */
struct reqset *reqs;			/* Looking for these requests only */
short dae_present;			/* NQS local daemon is present */
struct confd *qmapfile;			/* NQS queue/device/destination */
					/* mappings file */
struct confd *pipeqfile;		/* NQS pipe queue destinations file */
struct confd *qcomplexfile;		/* NQS queue complex definition file */
{
	uid_t myuid;			/* Local user-id */
	char myname [MAX_ACCOUNTNAME+1];/* Local account name */
	struct gendescr *descr;		/* Pointer to general descriptor */
	int found;			/* Boolean */
	mid_t itsmid;			/* The machine id of the queue */
	mid_t mymid;			/* The local machine id */
	char *localname;		/* The queue name without "@host" */
	struct passwd *passwd;		/* Password file entry */
	int sd;				/* Socket descriptor */
	int extrach;			/* Number of chs already read */
	short output;			/* Boolean */
	int timeout;			/* Timeout in between tries */
	long transactcc;		/* Transaction completion code */

	switch (machspec (fullname, &itsmid)) {
		case 0:
			break;
		case -1:
		case -2:
			fprintf (stderr, "Invalid hostname specification ");
			fprintf (stderr, "for queue %s.\n", fullname);
			return (0);
		case -3:
		case -4:
			fprintf (stderr, "Unexpected error in machspec(), ");
			fprintf (stderr, "queue = %s.\n", fullname);
			return (0);
	}
	if (localmid (&mymid) != 0) {
		fprintf (stderr, "Unexpected error in localmid().\n");
		return (0);
	}
	localname = destqueue (fullname);
	if (itsmid == mymid) {
		found = 0;
		if (telldb (queuefile) != -1) {
			/*
			 *  We are not necessarily at the start of the
			 *  database file....
			 */
			seekdb (queuefile, 0L);	/* Seek to the beginning */
		}
		descr = nextdb (queuefile);
		while (descr != NULL && !found) {
			if (strcmp (descr->v.que.namev.name, localname) == 0) {
				found = 1;
			}
			else descr = nextdb (queuefile);
		}
		if (found) {
			return (shoqbydesc (queuefile, descr, flags, whomuid,
					    reqs, dae_present, qmapfile,
					    pipeqfile, qcomplexfile));
		}
		else {
			fprintf (stderr, "Queue: %s does not exist.\n",
				 fullname);
			return (0);
		}
	}
	else {
#if	NETWORKED
#if	STREAM_SOCKETS
		myuid = getuid();
		if ((passwd = fetchpwuid (myuid)) == (struct passwd *) 0) {
			fprintf (stderr, "Who are you?\n");
			return (0);
		}
		strncpy (myname, passwd->pw_name, MAX_ACCOUNTNAME+1);
					/* Save account name */
		if ((passwd = fetchpwuid (whomuid)) == (struct passwd *) 0) {
			fprintf (stderr, "Who is user %d?\n", whomuid);
			return (0);
		}
		interclear ();
		interw32i (flags);	/* SHO_??? */
		interwstr (localname);	/* Queue name without @machine */
		/*
		 *  This implementation of shoqbyname() uses "whomuid" to
		 *  determine, for local queues, whose requests should be
		 *  highlighted.  We presume that the remote implementation
		 *  does the same.  However, we do not assume that nmap is
		 *  used to come up with the remote whomuid.  Pass both the
		 *  client's whomuid and the client's "whom-username".
		 *  This lets the netserver decide how to come up with
		 *  whomuid on the server machine.
		 */
		interw32i (whomuid);
		interwstr (passwd->pw_name);
		sd = establish (NPK_QSTAT, itsmid, myuid, myname,
				&transactcc);
		/*
		 *  Establish has just told the network server
		 *  on the remote machine what it is that we want.
		 */
		if (sd < 0) {
			if (sd == -2) {
				/*
				 * Retry is in order.
				 */
				timeout = 1;
				do {
					nqssleep (timeout);
					interclear ();
					interw32i (flags);
					interwstr (localname);
					interw32i (whomuid);
					interwstr (passwd->pw_name);
					sd = establish (NPK_QSTAT, itsmid,
							myuid, myname,
							&transactcc);
					timeout *= 2;
				} while (sd == -2 && timeout <= 16);
				/*
				 *  Beyond this point, give up on retry.
				 */
				if (sd < 0) {
					analyzetcm (transactcc, stderr, "");
				}
			}
			else {
				/*
				 *  Retry would surely fail.
				 */
				analyzetcm (transactcc, stderr, "");
			}
		}
		/*
		 *  The server liked us.
		 */
		output = -1;
		/*
		 *  First, display any characters that
		 *  were already read inside establish().
		 */
		if (extrach = extrasockch ()) {
			output = 0;
			write (STDOUT, bufsockch (), extrach);
		}
		/*
		 *  Then display any characters that the server
		 *  may still want to send us.
		 */
		if (filecopyentire (sd, STDOUT)) return (0);
		else return (output);
#else
BAD NETWORK TYPE
#endif	STREAM_SOCKETS
#else
		fprintf (stderr, "You are asking about a remote queue.\n");
		fprintf (stderr, "Networking is not supported in this ");
		fprintf (stderr, "implementation.\n");
		return (0);
#endif	NETWORKED
	}
}
