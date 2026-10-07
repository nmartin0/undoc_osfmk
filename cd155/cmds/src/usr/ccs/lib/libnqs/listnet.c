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
 * HISTORY
 * $Log: listnet.c,v $
 * Revision 1.2  1994/11/19  02:27:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)listnet.c	1.3 (listnet.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>			/* for stderr */
#include "nqs.h"			/* NQS types and definitions */

#if OSF
#include        "core_dump.h"
#endif

#include "netpacket.h"			/* NQS networking */
#include <pwd.h>			/* Password stuff */
#include NETDB				/* Network database declarations */
/* CERN Boissat #include NMAP	 Network mapping codes */

extern void analyzetcm();		/* Print message */
extern char *bufsockch();		/* Ptr to extra chars from socket */
extern int establish();			/* Set up connection */
extern int extrasockch();		/* Number of extra chars from socket */
extern struct passwd *fetchpwuid();	/* NQS version of getpwuid */
extern long filecopyentire();		/* File descr to file descr copy */
extern void interclear();		/* Zero out inter() structs */
extern void interw32i();		/* Write a 32 bit integer */
extern void interwstr();		/* Write a string */
extern int localmid();			/* Get our mid */
extern int machspec();			/* Get an mid */
extern void nqssleep();			/* NQS version of sleep() */
extern char *strncpy();			/* Anchored string copy */
					/* database file */

#define	ERR_ENOMAP	"Networked command failed due to NMAP_ENOMAP"
#define	ERR_ENOPRIV	"Networked command failed due to NMAP_ENOPRIV"

/*** 
 *	get_host_id
 *
 *	Obtain host id from hostname.
 *	Host id is stored in itsmid.
 *
 *	Returns 0 on SUCCESS and negative value on failure.
 */
int get_host_id (  hname, itsmid)
char	*hname;				/* host */
mid_t 	*itsmid;			/* The machine id of the queue */
{
	struct hostent *ent;
	char *localname;		/* The queue name without "@host" */


	/* step 1. get host structure for host name */

        if ((ent = gethostbyname (hname)) == (struct hostent *) 0) {
		fprintf(stderr,"Host not found,thus  \n");
                return (-2);                    /* This machine is not */
                                                /* known to us */
        }

	/* step 2. get host id from host record, host id is stored in itsmid */

        switch(nmap_get_mid (ent, itsmid)) {
        case NMAP_SUCCESS:              /* Successfull */
		return(0);
        case NMAP_ENOMAP:               /* What?  No local mid defined! */
		fprintf(stderr,"%s\n",ERR_ENOMAP);
                return (-2);
        case NMAP_ENOPRIV:              /* No privilege */
		fprintf(stderr,"%s\n",ERR_ENOPRIV);
                return (-3);
        }

}	/* end of get_host_id() */


call_host(whomuid,flags,itsmid,localname)
uid_t	whomuid;
long	flags;
mid_t 	itsmid;
char	*localname;
{
	struct passwd *passwd;		/* Password file entry */
	uid_t myuid;			/* Local user-id */
	char myname [MAX_ACCOUNTNAME+1];/* Local account name */
	int sd;				/* Socket descriptor */
	int extrach;			/* Number of chs already read */
	short output;			/* Boolean */
	int timeout;			/* Timeout in between tries */
	long transactcc;		/* Transaction completion code */
#if	NETWORKED
#if	STREAM_SOCKETS

	/* step 1. if localname = NULL then change it to a empty string
	 *	   as sending NULL over the net is interperted as sending
	 *	   a 0 over the net - which will produce an error
	 */
	if (localname == NULL)
		localname="";

	myuid = getuid();
	if ((passwd = fetchpwuid (myuid)) == (struct passwd *) 0) {
			fprintf (stderr, "Networked command: Who are you?\n");
			return (0);
	}
	strncpy (myname, passwd->pw_name, MAX_ACCOUNTNAME+1);
					/* Save account name */
	if ((passwd = fetchpwuid (whomuid)) == (struct passwd *) 0) {
			fprintf (stderr, "Who is user %d?\n", whomuid);
			return (0);
	}
	if (whomuid == 0) {
		fprintf(stderr,"User is root - Root is not allowed \n");
		fprintf(stderr," to access a different machine on the net as root\n");
		return(0);
	}
	interclear ();
	interw32i (flags);	/* SHO_??? */
	interwstr (localname);	/* Queue name without @machine */
		/*
		 *  This implementation uses "whomuid" to
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

extern struct hostent *gethostbyname();	/* Get hostent structure for name */
extern int localmid();			/* Get machine-id of local host */
extern int nmap_get_mid();		/* Get machine-id for name */

/*** machspec
 *
 *
 *	int machspec():
 *
 *	Determine the machine-id of the explicit or implied machine
 *	specification as given in the text parameter.
 *
 *	The form of an explicit machine specification is:
 *
 *		@machine-name	OR
 *		@[integer#]
 *
 *	where the "machine-name" form represents the principal name
 *	or alias assigned to a particular machine, and the "[integer#]"
 *	form is supported so that explicit machine-ids can be specified
 *	in place of a name or alias.
 *
 *	Returns:
 *		0: if successful, in which case the machine-id
 *		   parameter is successfully updated as
 *		   appropriate.
 *
 *	       -1: if an invalid machine-specification syntax
 *		   is encountered.
 *
 *	       -2: if the explicit or implied machine-specification
 *		   is not recognized by the local system (NMAP_ENOMAP).
 *
 *	       -3: if the Network Mapping Procedures (NMAP_)
 *		   deny access to the caller (NMAP_ENOPRIV).
 *
 *	       -4: if some other general NMAP_ error occurs.
 */
int check_rmt (text, machine_id)
register char *text;			/* Literal text */
mid_t *machine_id;			/* Pointer of where to return */
					/* the proper machine-id */
{
	register mid_t mid_spec;	/* Machine-id specification */
	struct hostent *ent;		/* Host table entry structure */

	while (*text && *text != '@') {
		text++;			/* Look for possible machine- */
	}				/* specification */
	if (*text == '\0') {
		/*
		 *  No machine-specification was given.  The
		 *  local machine is assumed.
		 */
		return (-1 );
	}
	/*
	 *  A machine-name specification appears.
	 */
	text++;				/* Step past machine-specification */
					/* introducer character */
	if (*text == '\0') {
		/*
		 *  Missing machine-specification.
		 */
		return (-2);		/* Bad syntax */
	}
	/*
	 *  Determine the machine-id of the destination machine.
	 */
	if (*text == '[') {
		/*
		 *  Could be an explicit machine specification by machine-id.
		 */
		text++;
		mid_spec = 0;
		while (*text >= '0' && *text <= '9') {
			mid_spec *= 10;
			mid_spec += *text - '0';
			*text++;
		}
		if (*text == ']') {
			/*
			 *	[ <digit-sequence> ]
			 */
			if (*++text) {		/* Invalid machine-id spec */
				return (-2);	/* Bad syntax */
			}
			*machine_id = mid_spec;
			return (0);		/* Success */
		}
		return (-2);			/* Invalid syntax */
	}
	/*
	 *  The destination host has been specified as a name.
	 */
	if ((ent = gethostbyname (text)) == (struct hostent *) 0) {
		return (-2);			/* This machine is not */
						/* known to us */
	}
	switch (nmap_get_mid (ent, machine_id)) {
	case NMAP_SUCCESS:		/* Successfully got local machine-id */
		return (0);
	case NMAP_ENOMAP:		/* What?  No local mid defined! */
		return (-2);
	case NMAP_ENOPRIV:		/* No privilege */
		return (-3);
	}
	return (-4);			/* General NMAP_ error */
}

call_host_del(whomuid,orig_seqno,orig_mid,sig,reqstate,host_id,localname)
uid_t	whomuid;
long	orig_seqno;
mid_t 	orig_mid;
int 	sig;
int	reqstate;
mid_t 	host_id;
char	*localname;
{
	struct passwd *passwd;		/* Password file entry */
	uid_t myuid;			/* Local user-id */
	char myname [MAX_ACCOUNTNAME+1];/* Local account name */
	int sd;				/* Socket descriptor */
	int extrach;			/* Number of chs already read */
	short output;			/* Boolean */
	int timeout;			/* Timeout in between tries */
	long transactcc;		/* Transaction completion code */
#if	NETWORKED
#if	STREAM_SOCKETS

	/* step 1. if localname = NULL then change it to a empty string
	 *	   as sending NULL over the net is interperted as sending
	 *	   a 0 over the net - which will produce an error
	 */
	if (localname == NULL)
		localname="";

	myuid = getuid();
	if ((passwd = fetchpwuid (myuid)) == (struct passwd *) 0) {
			fprintf (stderr, "Networked command: Who are you?\n");
			return (0);
	}
	strncpy (myname, passwd->pw_name, MAX_ACCOUNTNAME+1);
					/* Save account name */
	if ((passwd = fetchpwuid (whomuid)) == (struct passwd *) 0) {
			fprintf (stderr, "Who is user %d?\n", whomuid);
			return (0);
	}
	interclear ();
	interw32i (whomuid);	/* send uid */
	interwstr (passwd->pw_name);	/* send name */
		/*
		 *  This implementation uses "whomuid" to
		 *  determine, for local queues, whose requests should be
		 *  highlighted.  We presume that the remote implementation
		 *  does the same.  However, we do not assume that nmap is
		 *  used to come up with the remote whomuid.  Pass both the
		 *  client's whomuid and the client's "whom-username".
		 *  This lets the netserver decide how to come up with
		 *  whomuid on the server machine.
		 */
	interw32i (orig_seqno);
	interw32i (orig_mid);
	interw32i (sig);
	interw32i (reqstate);
	sd = establish (NPK_DELREQ, host_id, myuid, myname,
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
				interw32i (whomuid);	/* send uid */
				interwstr (passwd->pw_name); /* send name */
				interw32i (orig_seqno);
				interw32i (orig_mid);
				interw32i (sig);
				interw32i (reqstate);
				sd = establish (NPK_DELREQ, host_id,
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
