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
 
/*++ qdel.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qdel/qdel.c,v $
 *
 * DESCRIPTION:
 *
 *	Delete a queued req, or send a signal to a running request.
 *	If the request is running, and a signal was specified to be
 *	sent, then the signal is sent to all processes in the request.
 *
 * RETURNS:
 *	0    -  all requests deleted succesfully
 *	-1   -  error in calling sequence
 *      n    -  number of requests not deleted
 *
 *	In all error cases, messages are sent to the standard output file.
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 14, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 01:35:23 $ $State: Exp $)
 * $Log: qdel.c,v $
 * Revision 1.3  1994/11/19  01:35:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:29:45  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:26:30  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:59:01  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:11:00  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)qdel.c	1.2 (qdel.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#define	MAX_REQS	100		/* Maximum number of reqs that */
					/* can be deleted/signalled at */
					/* a time. */
#include <stdio.h>
#include <signal.h>			/* Signal definitions */
#include <pwd.h>			/* Password file definitions */
#include "nqs.h"			/* NQS types and definitions */
#include NETDB				/* Network database header file; */
					/* Proto/Makefile will tell us */
					/* where to find netdb.h */
#include "nqsdirs.h"			/* NQS files and directories */
#include "transactcc.h"			/* Transaction completion codes */


/*
 *	External functions:
 */
extern long delreq();			/* Delete request */
extern void closepwdb();		/* Close account/password database */
extern void exiting();			/* Exiting.  Relinquish inter- */
					/* process communication file */
extern struct passwd *fetchpwnam();	/* Fetch passwd entry by name */
extern uid_t getuid();			/* Get real user-id */
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
#if	BSD42 | BSD43
extern int interconn();			/* Make up for the lack of named */
					/* pipes */
#else
BAD SYSTEM TYPE
#endif
#endif
extern int reqspec();			/* Parse request-id returning */
					/* completion code, request-seq#, */
					/* and req machine-id */
extern int localmid();			/* Get local machine-id */
extern int nqspriv();			/* Return non-zero if NQS mgr */

/*
 *	Global variables:
 */
char *Qdel_prefix = "Qdel";

/*** main
 *
 *
 *	qdel [ { -k , -<integer-signal> } ] [ -u <username> ] <request-id(s)>
 */
main (argc, argv)
int argc;
char **argv;
{
	void cleanup();		/* Cleanup on SIGINT, etc. */
	extern int  diagdel();/* Diagnose delreq completion code */
	void showhow();		/* Show how to use this command */

	int n_errs;			/* Number of requests not deleted. */
	int n_reqs;			/* Number of reqs to delete/signal. */
	struct {
		long orig_seqno;	/* Sequence# for req */
		mid_t machine_id;	/* Machine-id of originating machine */
	} reqs [MAX_REQS];		/* Reqs to delete/signal */
	struct passwd *passwd;		/* Password structure ptr */
	char **scan_reqs;		/* Scan reqs */
	char *argument;			/* Ptr to cmd line arg text */
	char *cp;			/* Scanning character ptr */
	char *hname = NULL;		/* host name */
	short sendsig;			/* Boolean non-zero if a signal */
					/* should be sent if the req is */
					/* running */
	short sig;			/* Signal to send, if the req is */
					/* running */
	uid_t real_uid;			/* Real user-id */
	mid_t local_mid;		/* local machine-id */
	int mid;


	/*
	 *  Catch 4 common household signals:  SIGINT, SIGQUIT,SIGHUP, 
	 *  and SIGTERM.  This is quite important, because we do not want
	 *  to leave useless inter-process communication files hanging
	 *  around in the NQS directory hierarchy.  However, if we do,
	 *  it is not fatal, it will eventually be cleaned up by NQS.
	 */
	signal (SIGINT, cleanup);
	signal (SIGQUIT, cleanup);
	signal (SIGHUP, cleanup);
	signal (SIGTERM, cleanup);
	if (chdir (Nqs_root) == -1) {
		fprintf (stderr, "%s(FATAL): Unable to chdir() to the NQS ",
			 Qdel_prefix);
		fprintf (stderr, "root directory.\n");
		exit (-1);
	}
	/*
	 *  On systems with named pipes, we get a pipe to the local
	 *  daemon automatically the first time we call inter().
	 */
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
#if	BSD42 | BSD43
	if (interconn () < 0) {
		fprintf (stderr, "%s(FATAL): Unable to get ", Qdel_prefix);
		fprintf (stderr, "a pipe to the local daemon.\n");
		exit (-1);
	}
#else
BAD SYSTEM TYPE
#endif
#endif
	sendsig = 0;			/* Don't kill a running req by */
					/* default */
	sig = 9;			/* Default signal to send */
	passwd = NULL;			/* No -u flag seen */
	while (*++argv != NULL && **argv == '-') {
		argument = *argv;
		switch (*++argument) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (sendsig) {
				fprintf (stderr,
					 "Multiple signal specifications.\n");
				exit (-1);
			}
			cp = argument;
			sig = 0;
			while (*cp >= '0' && *cp <= '9') {
				sig *= 10;
				sig += *cp++ - '0';
			}
			if (*cp) {
				/*
				 *  A non-digit character has been
				 *  found.
				 */
				fprintf (stderr, "Invalid signal ");
				fprintf (stderr, "specification.\n");
				showhow();
			}
			sendsig = 1;	/* Send signal if req is running */
			break;
		case 'k':
			sendsig = 1;	/* Send signal if req is */
					/* running */
			break;
		case 'u':		/* User-name specification */
			if (*++argv == NULL) {
				fprintf (stderr, "Missing username.\n");
				exit (-1);
			}
			if (passwd != NULL) {
				fprintf (stderr,
					"Multiple -u specifications.\n");
				exit (-1);
			}
			if ((passwd = fetchpwnam (*argv)) == NULL) {
				fprintf (stderr, "No such user on this ");
				fprintf (stderr, "machine.\n");
				exit (-1);
			}
			if (localmid (&local_mid) != 0) {
				fprintf (stderr, "%s(FATAL): ", Qdel_prefix);
				fprintf (stderr, "Unable to get machine-id");
				fprintf (stderr, "of local host.\n");
				exit(-1);
			}
			if ((nqspriv (getuid(), local_mid, local_mid)
				& QMGR_OPER_PRIV) ||
			    passwd->pw_uid == getuid()) {
				/*
				 *  We have NQS operator privileges, or we
				 *  are just going after our own requests.
				 */
				real_uid = passwd->pw_uid;
			}
			else {
				fprintf (stderr, "Insufficient privilege ");
				fprintf (stderr, "for -u specification.\n");
				exit (-1);
			}
			break;
		case 'h':		/* delete request on given host */
			if (*++argv == NULL){
				fprintf (stderr, "Missing hostname.\n");
                                exit (-1);
			}
			if (hname != NULL){
				fprintf (stderr, " More than one host was specified.\n");
				exit(-1);
			}
			hname = *argv;
			get_host_id(hname,&mid);
			break;
		default:
			fprintf (stderr, "Invalid option flag ");
			fprintf (stderr, "specified.\n");
			showhow();
		}
	}
	if (passwd == NULL) {
		/*
		 *  No username specified.  We assume the invoker.
		 */
		real_uid = getuid();		/* Get real user-id */
	}
	else closepwdb();		/* Close account/password database */
	if (!sendsig) {
		/*
		 *  No running req should be killed.
		 */
		sig = 0;
	}
	/*
	 *  Build the set of reqs to be deleted.
	 */
	if (*argv == NULL) {
		/*
		 *  No request-ids were specified.
		 */
		fprintf (stderr, "No request-id(s) specified.\n");
		showhow();
	}
	else {
		n_reqs = 0;			/* #of reqs to delete/signal */
		scan_reqs = argv;		/* Set req scan pointer */
		while (*scan_reqs != NULL &&	/* Loop to delete reqs */
		       n_reqs < MAX_REQS) {
			switch (reqspec (*scan_reqs, &reqs [n_reqs].orig_seqno,
					 &reqs [n_reqs].machine_id)) {
			case -1:
				fprintf (stderr, "Invalid request-id syntax ");
				fprintf (stderr, "for request-id: %s.\n",
					*scan_reqs);
				exit (-1);
			case -2:
				fprintf (stderr, "Unknown machine for");
				fprintf (stderr, "request-id: %s.\n",
					*scan_reqs);
				exit (-1);
			case -3:
				fprintf (stderr, "Network mapping database ");
				fprintf (stderr, "inaccessible.  Seek staff ");
				fprintf (stderr, "support.\n");
				exit (-1);
			case -4:
				fprintf (stderr, "Network mapping database ");
				fprintf (stderr, "error when parsing ");
				fprintf (stderr, "request-id: %s.\n",
					*scan_reqs);
				fprintf (stderr, "Seek staff support.\n");
				exit (-1);
			}
			scan_reqs++;		/* One more req */
			n_reqs++;
		}
		if (*scan_reqs != NULL) {
			/*
			 *  Too many reqs were specified to be deleted.
			 */
			fprintf (stderr, "Too many requests given to ");
			fprintf (stderr, "delete.\n");
			exit (-1);
		}
		/*
		 *  Now that everything has been parsed and legitimized,
		 *  delete the specified set of requests.
		 */
		n_reqs = 0;
		n_errs = 0;
		while (*argv != NULL) {		/* Loop to delete reqs */
			if (hname != NULL ) {
			n_errs +=  call_host_del(real_uid,
					reqs[n_reqs].orig_seqno,
				      	reqs[n_reqs].machine_id, sig,
				      	RQS_DEPARTING | RQS_RUNNING | RQS_STAGING |
				      	RQS_QUEUED | RQS_WAITING | RQS_HOLDING |
				      	RQS_ARRIVING,mid);

			}
			else
			n_errs += diagdel (delreq (real_uid,
				 reqs [n_reqs].orig_seqno,
				 reqs [n_reqs].machine_id, sig,
				 RQS_DEPARTING | RQS_RUNNING | RQS_STAGING |
				 RQS_QUEUED | RQS_WAITING | RQS_HOLDING |
				 RQS_ARRIVING), *argv);
			argv++;			/* One more req */
			n_reqs++;
		}
	}
	exiting();			/* Delete our comm. file */
	exit (n_errs);
}


/*** cleanup
 *
 *
 *	Catch certain signals, and delete the inter-process
 *	communication file we have been using.
 */
static void cleanup (sig)
int sig;				/* Signal that was caught */
{
	signal (sig, SIG_IGN);		/* Ignore multiple signals */
	exiting();			/* Delete our comm. file */
}



/*** showhow
 *
 *
 *	showhow():
 *	Show how to use this command.
 */
static void showhow()
{
	fprintf (stderr, "Command format is:\n\n");
	fprintf (stderr, "    qdel [ { -k , -<signal#> } ] [ -h hostname ] [ -u <username> ] ");
	fprintf (stderr, "<request-id(s)>\n\n");
	exit (0);
}
