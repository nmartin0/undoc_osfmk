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
 
/*++ netclient.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/netclient.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Default NQS network client.  Empties network queues.
 *	Returns stdout, stderr, and stage-out files.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	April 22, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:39 $ $State: Exp $)
 * $Log: netclient.c,v $
 * Revision 1.3  1994/11/19  02:52:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:24:40  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:38  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:03:46  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"			/* Types and definitions */
#include "netpacket.h"			/* Network packet types */
#include "informcc.h"			/* Information completion codes */
#include "requestcc.h"			/* Request completion codes */
#include "transactcc.h"			/* Transaction completon codes */
#include "nqsdirs.h"			/* For nqslib library functions */
#include <errno.h>			/* Error numbers */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#endif
#endif
#include <pwd.h>			/* Password stuff */
#include <sys/stat.h>			/* File status */
#include <signal.h>			/* For SIGPIPE, SIGTERM */

extern int errno;			/* In case not declared in errno.h */

extern int atoi();			/* Ascii to integer */
extern void bufstderr();		/* Block buffer stderr() output */
extern void bufstdout();		/* Block buffer stdout() output */
extern long errnototcm();		/* Convert errno to TCMx_ code */
extern int establish();			/* Establish a connection */
extern struct passwd *fetchpwuid();	/* NQS version of getpwuid() */
extern long filecopyentire();		/* Copy file (from, to) */
extern gid_t getegid();			/* Get effective group-id */
extern char *getenv();			/* Get value of environment variable */
extern uid_t geteuid();			/* Get effective user-id */
#if	NETWORKED
extern int getsockch();			/* Get a char from a socket */
#endif
extern void interclear ();		/* Get a clean slate */
extern int interfmt();			/* Format a packet */
extern int intern32i();			/* Find out how many we got */
extern int internstr();			/* Find out how many we got */
extern long interr32i();		/* Read a 32 bit integer */
extern int interread();			/* Read a packet */
extern void interset();			/* Set the file-descriptor used */
					/* to communicate messages to the */
					/* local NQS daemon */
extern void interw32i();		/* Prepare a 32 bit int for writing */
extern void interwstr();		/* Prepare a string for writing */
extern int localmid();			/* Get nmap machine id */
extern long mergertcm();		/* Overlay bits of rcm, tcm */
extern void mkdefault();		/* Return a reasonable pathname */
extern char *namstderr();		/* Return stderr pathname */
extern char *namstdout();		/* Return stdout pathname */
extern void nqssleep();			/* NQS sleep call */
extern int readreq();			/* Read an NQS control file */
extern void serexit();			/* Write to shepherd on fd #3 */
extern void setsockfd();		/* Set socket file descr */
extern char *strcpy();			/* String to string copy */


/*
 * Variables global to this module.
 */
static int Locmid; 
static int Debug;

/*** main
 *
 *
 *	main():
 *
 *	This process is exec'd by the child of a shepherd process (child of
 *	the NQS daemon) with the following file descriptors open as described:
 *
 *		File descriptor 0: Control file (O_RDWR).
 *		File descriptor 1: Stdout writes to the NQS log process.
 *		File descriptor 2: Stderr writes to the NQS log process.
 *		File descriptor 3: Write file descriptor for serexit()
 *				   back to the shepherd process.
 *		File descriptor 4: Connected to the local NQS daemon
 *				   request FIFO pipe.
 *		File descriptors [5.._NFILE] are closed.
 *
 *
 *	At this time, this process is running with a real AND effective
 *	user-id of root!
 *
 *
 *	The current working directory of this process is the NQS
 *	root directory.
 *
 *
 *	All signal actions are set to SIG_DFL.
 *
 *
 *	The environment of this process contains the environment string:
 *
 *		DEBUG=n
 *
 *	where n specifies (in integer ASCII decimal format), the debug
 *	level in effect when this client was exec'd over the child of
 *	an NQS shepherd.
 *
 *
 *	The user upon whose behalf this work is being performed (the
 *	transaction owner), is identified by the environment variables:
 *
 *		UID=n
 *		USERNAME=username-of-transaction-owner
 *
 *	where n specifies (in integer ASCII decimal format), the integer
 *	user-id of the transaction owner.
 * 
 *
 *	For System V based implementations of NQS, the environment
 *	variable:
 *
 *		TZ=timezonename
 *
 *	will also be present.  Berkeley based implementations of NQS will
 *	not contain this environment variable.
 *
 *
 *	The environment of this process also contains the default retry
 *	state tolerance time, the default retry wait time, and the amount
 *	of time that the network destination has been in a retry mode
 *	(all times are in seconds):
 *
 *		DEFAULT_RETRYWAIT=n
 *		DEFAULT_RETRYTIME=n
 *		ELAPSED_RETRYTIME=n
 *
 *	where n is an ASCII decimal format long integer number.
 *
 *
 *	The environment variable of:
 *
 *		OP=opstring
 *
 *	defines the operation that is to be performed by this instance of
 *	the network queue server.
 *
 *		OP=D	-- deliver this request to its destination;
 *		OP=O	-- perform a stage-out file hierarchy event;
 *
 *	In the OP=O case, the additional environment variable of:
 *
 *		EVENT=n
 *
 *	is also present with n indicating the file staging event
 *	number [0..31] (see ../lib/transact.c).
 *
 */
main (argc, argv, envp)
int argc;
char *argv [];
char *envp [];
{
	long do_delivery();
	long do_stageout();

	struct rawreq rawreq;	/* To hold the control file header */
	char *cp;		/* Ascii value of environmental var */
	int cuid;		/* Client's user id */
	char *cusername;	/* Client's username */
	
	/*
	 *  It is necessary to block buffer all output to the NQS log
	 *  process so that messages from multiple NQS processes appear
	 *  coherently in the log output.
	 *
	 *  This is not optional.  This is REQUIRED!
	 */
	bufstderr();
	bufstdout();
	/*
	 *  On some UNIX implementations, stdio functions alter errno to
	 *  ENOTTY when they first do something where the file descriptor
	 *  corresponding to the stream happens to be a pipe (which is
	 *  the case here).
	 *
	 *  The fflush() calls are added to get this behavior out of the
	 *  way, since bugs have occurred in the past when a server ran
	 *  into difficultly, and printed out an errno value in situations
	 *  where the diagnostic printf() displaying errno occurred AFTER
	 *  the first stdio function call invoked.
	 */
	fflush (stdout);		/* Stdout is a pipe */
	fflush (stderr);		/* Stderr is a pipe */
	if (getuid() != 0 || geteuid() != 0) {
		printf ("I$Netclient: uid or euid not root.\n");
		exit (1);
	}
	/*
	 * SIGTERM is sent to all NQS processes upon NQS shutdown.
	 * Ignore it.
	 */
	signal (SIGTERM, SIG_IGN);
	if ((cp = getenv ("DEBUG")) == (char *) 0) {
		Debug = 0;
	}
	else Debug = atoi (cp);
	interset (4);		/* Use this file descriptor to communicate */
				/* with the local NQS daemon */
	if (Debug > 2) {
		while (*envp != NULL) {
			printf ("D$Testing netclient envp: %s\n", *envp);
			envp++;
		}
		fflush (stdout);
	}
	if (readreq (0, &rawreq) != 0) {
		serexit (RCM_BADCDTFIL, (char *) 0);
	}
	if ((cp = getenv ("UID")) == (char *) 0) {
		serexit (RCM_BADSRVARG, (char *) 0);
	}
	cuid = atoi (cp); 
	if ((cusername = getenv ("USERNAME")) == (char *) 0) {
		serexit (RCM_BADSRVARG, (char *) 0);
	}
	/*
	 * Which operation are we to perform?
	 */
	if ((cp = getenv ("OP")) == (char *) 0) {
		serexit (RCM_BADSRVARG, (char *) 0);
	}
	switch (*cp) {
	case 'D':
		if (localmid (&Locmid) != 0) {
			serexit (RCM_UNAFAILURE, (char *) 0);
		}
		serexit (do_delivery (&rawreq, cuid, cusername), (char *) 0);
		break;
	case 'O':
		if (localmid (&Locmid) != 0) {
			serexit (RCM_UNAFAILURE, (char *) 0);
		}
		serexit (do_stageout (&rawreq, cuid, cusername), (char *) 0);
		break;
	default:
		serexit (RCM_BADSRVARG, (char *) 0);/* Exit */
		break;
	}
}


/*** do_delivery
 *
 *
 *	long do_delivery():
 *
 *	Delivery of requests across machines will soon be done using
 *	the one-two punch of pipe queues and network queues.
 *	This routine empties network queues.
 */
static long do_delivery (rawreq, cuid, cusername)
struct rawreq *rawreq;
uid_t cuid;
char *cusername;
{
	return (mergertcm (RCM_DELIVERFAI, TCML_FATALABORT));
}


/*** do_stageout
 *
 *
 *	long do_stageout():
 *
 *	A batch request has spooled an output file.  Move that file
 *	from the output spooling area to a reasonable final location,
 *	possibly on another machine.
 */
static long do_stageout (rawreq, cuid, cusername)
struct rawreq *rawreq;
uid_t cuid;
char *cusername;
{
	long do_stderr();
	long do_stdout();
	void lostsink();

	char *asciievent;		/* Ascii val of env var */
	struct passwd *passwd;		/* Pointer to password info */

	signal (SIGPIPE, lostsink);
	if ((asciievent = getenv ("EVENT")) == (char *) 0) {
		return (RCM_BADSRVARG);
	}
	/*
	 * It will be nice to know the user's home directory
	 */
	if ((passwd = fetchpwuid (cuid)) == (struct passwd *) 0) {
		return (mergertcm (RCM_STAGEOUTFAI, TCML_UNAFAILURE));
	}
	switch (atoi (asciievent)) {
	case 30:
		return (do_stdout (rawreq, passwd));
	case 31:
		return (do_stderr (rawreq, passwd));
	default:
		/*
		 * Stage-out events 0-29 are not supported yet.
		 */
		return (mergertcm (RCM_STAGEOUTFAI, TCML_FATALABORT));
	}
}


/*** do_stdout
 *
 *
 *	long do_stdout():
 *
 *	Stdout has already been spooled to a regular file.  Move that
 *	file from the output spooling area to a reasonable final
 *	location, possibly on another machine.
 *	Returns: RCM_STAGEOUT (might have information bits)
 *		 RCM_STAGEOUTFAI (will have information bits)
 */
static long do_stdout (rawreq, passwd)
struct rawreq *rawreq;
struct passwd *passwd;
{
	long rightrcm();
	long tryhere();	
	long trythere();	
	
	char *tempname;				/* Spooled file pathname */
	struct stat statbuf;			/* To hold stat() output */
	char path [MAX_REQPATH + 256 + 1];	/* 256 for home dir */
	long primaryrcm;			/* Rcm from first try */
	long secondaryrcm;			/* Rcm from second try */

	if (rawreq->v.bat.stdout_acc & OMD_M_KEEP) {
		rawreq->v.bat.stdout_mid = Locmid;
	}
	tempname = namstdout (rawreq->orig_seqno, rawreq->orig_mid);
	if (stat (tempname, &statbuf) == -1) {
		/*
		 * Assume that a previous attempt worked.
		 */
		return (RCM_STAGEOUT);
	}
	/*
	 * Assume that any previous attempts failed.
	 */
	if (rawreq->v.bat.stdout_mid == Locmid) {
		/*
		 * Lower privileges.
		 */
#if UNICOS & NEWPWD
		if (acctid (0,passwd->pw_acid[0]) == -1) {
			/*
		 	 *	Invalid account ID
 		 	 */	
 			return (mergertcm (RCM_STAGEOUTFAI, TCML_NOACCAUTH));
 		}
#endif
		setgid (passwd->pw_gid);
		setuid (passwd->pw_uid);
		if (rawreq->v.bat.stdout_name [0] != '/') {
			sprintf (path, "%s/%s", passwd->pw_dir,
				rawreq->v.bat.stdout_name);
		}
		else strcpy (path, rawreq->v.bat.stdout_name);
		/*
		 * The following check handles the case of
		 * a bad password file home directory.
		 */
		if (path [0] != '/') {
			return (mergertcm (RCM_STAGEOUTFAI, TCML_UNAFAILURE));
		}
		primaryrcm = tryhere (tempname, path, &statbuf,
			(~rawreq->v.bat.umask & 0666));
		if ((primaryrcm & XCI_REASON_MASK) == RCM_STAGEOUTFAI) {
			/*
			 * We cannot return it to the primary location.
			 * Try to return it to a backup location
			 * on the execution machine.
			 */
			mkdefault (path, passwd->pw_dir, rawreq->reqname,
				rawreq->orig_seqno, 'o');
			/*
			 * If we cannot return it to the backup
			 * location either, report the errno from
			 * the primary location.
			 */
			return (rightrcm (primaryrcm,
				tryhere (tempname, path, &statbuf,
					(~rawreq->v.bat.umask & 0666))));
		}
		else return (primaryrcm);
	}
	else {
#if	NETWORKED
#if	STREAM_SOCKETS
		/*
		 * Try to return this file across the net.
		 */
		primaryrcm = trythere (tempname, rawreq->v.bat.stdout_name,
			rawreq->v.bat.stdout_mid, &statbuf, passwd);
		if (Debug > 2) {
			printf ("D$Netclient: primaryrcm oct = %o\n",
				primaryrcm);
			fflush (stdout);
		}
		if ((primaryrcm & XCI_REASON_MASK) == RCM_STAGEOUTFAI) {
			/*
			 * We cannot return it across the net.
			 * Try to return it to a backup location
			 * on the execution machine.
			 * First, lower privileges.
			 */
#if UNICOS & NEWPWD
			if (acctid (0,passwd->pw_acid[0]) == -1) {
				/*
			 	 *	Invalid account ID
 			 	 */	
 				return (mergertcm (RCM_STAGEOUTFAI,
						TCML_NOACCAUTH));
 			}
#endif
			setgid (passwd->pw_gid);
			setuid (passwd->pw_uid);
			mkdefault (path, passwd->pw_dir,
				rawreq->reqname, rawreq->orig_seqno, 'o');
			/*
			 * If we cannot return it to the backup
			 * location either, report the errno from
			 * the primary location.
			 */
			secondaryrcm = tryhere (tempname, path, &statbuf,
				(~rawreq->v.bat.umask & 0666));
			if (Debug > 2) {
				printf ("D$Secondaryrcm = oct %o\n",
					secondaryrcm);
				fflush (stdout);
			}
			return (rightrcm (primaryrcm, secondaryrcm));
		}
		else return (primaryrcm);
#else
BAD NETWORK TYPE
#endif
#else
		return (mergertcm (RCM_STAGEOUTFAI, TCML_NETNOTSUPP));
#endif
	}
}


/*** do_stderr
 *
 *
 *	long do_stderr():
 *
 *	Stderr has already been spooled to a regular file.  Move that
 *	file from the output spooling area to a reasonable final
 *	location, possibly on another machine.
 *	Returns: RCM_STAGEOUT (might have information bits)
 *		 RCM_STAGEOUTFAI (will have information bits)
 *
 */
static long do_stderr (rawreq, passwd)
struct rawreq *rawreq;
struct passwd *passwd;
{
	long rightrcm();
	long tryhere();	
	long trythere();	
	
	char *tempname;				/* Spooled file pathname */
	struct stat statbuf;			/* To hold stat() output */
	char path [MAX_REQPATH + 256 + 1];	/* 256 for home dir */
	long primaryrcm;			/* Rcm from first try */
	long secondaryrcm;			/* Rcm from second try */

	if (rawreq->v.bat.stderr_acc & OMD_M_KEEP) {
		rawreq->v.bat.stderr_mid = Locmid;
	}
	tempname = namstderr (rawreq->orig_seqno, rawreq->orig_mid);
	if (stat (tempname, &statbuf) == -1) {
		/*
		 * Assume that a previous attempt worked.
		 */
		return (RCM_STAGEOUT);
	}
	/*
	 * Assume that any previous attempts failed.
	 */
	if (rawreq->v.bat.stderr_mid == Locmid) {
		/*
		 * Lower privileges.
		 */
#if UNICOS & NEWPWD
		if (acctid (0,passwd->pw_acid[0]) == -1) {
			/*
		 	 *	Invalid account ID
 		 	 */	
 			return (mergertcm (RCM_STAGEOUTFAI, TCML_NOACCAUTH));
 		}
#endif
		setgid (passwd->pw_gid);
		setuid (passwd->pw_uid);
		if (rawreq->v.bat.stderr_name [0] != '/') {
			sprintf (path, "%s/%s", passwd->pw_dir,
				rawreq->v.bat.stderr_name);
		}
		else strcpy (path, rawreq->v.bat.stderr_name);
		/*
		 * The following check handles the case of
		 * a bad password file home directory.
		 */
		if (path [0] != '/') {
			return (mergertcm (RCM_STAGEOUTFAI, TCML_UNAFAILURE));
		}
		primaryrcm = tryhere (tempname, path, &statbuf,
			(~rawreq->v.bat.umask & 0666));
		if ((primaryrcm & XCI_REASON_MASK) == RCM_STAGEOUTFAI) {
			/*
			 * We cannot return it to the primary location.
			 * Try to return it to a backup location
			 * on the execution machine.
			 */
			mkdefault (path, passwd->pw_dir, rawreq->reqname,
				rawreq->orig_seqno, 'e');
			/*
			 * If we cannot return it to the backup
			 * location either, report the errno from
			 * the primary location.
			 */
			return (rightrcm (primaryrcm,
				tryhere (tempname, path, &statbuf,
					(~rawreq->v.bat.umask & 0666))));
		}
		else return (primaryrcm);
	}
	else {
#if	NETWORKED
#if	STREAM_SOCKETS
		/*
		 * Try to return this file across the net.
		 */
		primaryrcm = trythere (tempname, rawreq->v.bat.stderr_name,
			rawreq->v.bat.stderr_mid, &statbuf, passwd);
		if (Debug > 2) {
			printf ("D$Netclient: primaryrcm oct = %o\n",
				primaryrcm);
			fflush (stdout);
		}
		if ((primaryrcm & XCI_REASON_MASK) == RCM_STAGEOUTFAI) {
			/*
			 * We cannot return it across the net.
			 * Try to return it to a backup location
			 * on the execution machine.
			 * First, lower privileges.
			 */
#if UNICOS & NEWPWD
			if (acctid (0,passwd->pw_acid[0]) == -1) {
		
		/*
		 	 *	Invalid account ID
 		 	 */	
 			return (mergertcm (RCM_STAGEOUTFAI, TCML_NOACCAUTH));
 		}
#endif
			setgid (passwd->pw_gid);
			setuid (passwd->pw_uid);
			mkdefault (path, passwd->pw_dir,
				rawreq->reqname, rawreq->orig_seqno, 'e');
			/*
			 * If we cannot return it to the backup
			 * location either, report the errno from
			 * the primary location.
			 */
			secondaryrcm = tryhere (tempname, path, &statbuf,
				(~rawreq->v.bat.umask & 0666));
			if (Debug > 2) {
				printf ("D$Secondaryrcm oct = %o\n",
					secondaryrcm);
				fflush (stdout);
			}
			return (rightrcm (primaryrcm, secondaryrcm));
		}
		else return (primaryrcm);
#else
BAD NETWORK TYPE
#endif
#else
		return (mergertcm (RCM_STAGEOUTFAI, TCML_NETNOTSUPP));
#endif
	}
}


/*** rightrcm
 *
 *
 *	long rightrcm ():
 *	Return the correct request completion message.
 *
 *	Given that return to the primary location failed,
 *	this function allows us to report one to the following:
 *
 *	1) Successful return to the backup location,
 *		 and why the primary location failed, or
 *	2) Unsuccessful, and why the primary location failed
 *
 *	instead of one of the following:
 *
 *	1) Successful return (neglecting to mention that
 *		it was to the backup location), or
 *	2) Unsuccessful, and why the backup location failed
 *		(never mentioning why return to the primary location failed)
 *
 */
static long rightrcm (remotercm, localrcm)
long remotercm;
long localrcm;
{
	if ((localrcm & XCI_REASON_MASK) == RCM_STAGEOUT) {
		return (RCM_STAGEOUTBAK | (remotercm & XCI_INFORM_MASK));
	}
	else return remotercm;
}


#if	NETWORKED
#if	STREAM_SOCKETS
/*** trythere
 *
 *
 *	long trythere ():
 *
 *	Return an output file to a remote machine.
 *	If successful, return RCM_STAGEOUT and unlink tempname.
 *	If unsuccessful, return RCM_STAGEOUTFAI and do not unlink.
 */
static long trythere (tempname, dest, dest_mid, statbuf, passwd)
char *tempname;				/* Where we spooled it */
char *dest;				/* Path on remote machine */
mid_t dest_mid;				/* Nmap mid for remote machine */
struct stat *statbuf;			/* Stat of spooled copy */
struct passwd *passwd;			/* Password on our machine */
{
	int robustopen();		/* NQS version of open(2) */
	
	int fd;
	int sd;
	long transactcc;
	short timeout;			/* Seconds in between tries */
	int integers;
	int strings;
	char packet [MAX_PACKET];
	int packetsize;
	long tcm;

	if ((fd = robustopen (tempname, O_RDONLY, 0)) == -1) {
		return (mergertcm (RCM_STAGEOUTFAI, errnototcm (errno)));
	}
	interclear ();
	sd = establish (NPK_MVOUTFILE, dest_mid, passwd->pw_uid,
		passwd->pw_name, &transactcc);
	/*
	 * Establish has just told the network server
	 * on the remote machine what we want.
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
				sd = establish (NPK_MVOUTFILE, dest_mid,
					passwd->pw_uid, passwd->pw_name,
					&transactcc);
				timeout *= 2;
			} while (sd == -2 && timeout <= 16);
			/*
			 * Beyond this point, give up on retry.
			 */
			if (sd < 0) {
				return (mergertcm (RCM_STAGEOUTFAI,
					transactcc));
			}
		}
		else {
			/*
			 * Retry would surely fail.
			 */
			return (mergertcm (RCM_STAGEOUTFAI, transactcc));
		}
	}
	/*
	 * The server liked us.
	 */
	interclear();
	interw32i ((long) NPK_MVOUTFILE);
	interw32i ((long) statbuf->st_mode);
	interw32i ((long) statbuf->st_size);
	interwstr (dest);
	if ((packetsize = interfmt (packet)) == -1) {
		return (mergertcm (RCM_STAGEOUTFAI, TCML_INTERNERR));
	}
	if (write (sd, packet, packetsize) != packetsize) {
		return (mergertcm (RCM_STAGEOUTFAI, TCMP_CONNBROKEN));
	}
	setsockfd (sd);
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		return (mergertcm (RCM_STAGEOUTFAI, TCMP_CONNBROKEN));
	case -2:
		return (mergertcm (RCM_STAGEOUTFAI, TCML_PROTOFAIL));
	}
	integers = intern32i();
	strings = internstr();
	if (integers == 1 && strings == 0) {
		if ((tcm = interr32i (1)) != TCMP_CONTINUE) {
			return (mergertcm (RCM_STAGEOUTFAI, tcm));
		}
	}
	else return (mergertcm (RCM_STAGEOUTFAI, TCML_PROTOFAIL));
	if (filecopyentire (fd, sd) != statbuf->st_size) {
		return (mergertcm (RCM_STAGEOUTFAI, TCMP_CONNBROKEN));
	}
	interclear();
	interw32i ((long) NPK_DONE);
	interw32i ((long) 0);
	interw32i ((long) 0);
	interwstr ("");
	if ((packetsize = interfmt (packet)) == -1) {
		return (mergertcm (RCM_STAGEOUTFAI, TCML_INTERNERR));
	}
	if (write (sd, packet, packetsize) != packetsize) {
		return (mergertcm (RCM_STAGEOUTFAI, TCMP_CONNBROKEN));
	}
	/*
	 * We think the transfer was successful.
	 */
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		return (mergertcm (RCM_STAGEOUTFAI, TCMP_CONNBROKEN));
	case -2:
		return (mergertcm (RCM_STAGEOUTFAI, TCML_PROTOFAIL));
	}
	integers = intern32i();
	strings = internstr();
	if (integers == 1 && strings == 0) {
		if ((tcm = interr32i(1)) == TCMP_COMPLETE) {
			unlink (tempname);
			return (RCM_STAGEOUT);
		}
		else {
			return (mergertcm (RCM_STAGEOUTFAI, tcm));
		}
	}
	else return (mergertcm (RCM_STAGEOUTFAI, TCML_PROTOFAIL));
}
#else
BAD NETWORK TYPE
#endif
#endif


/*** tryhere
 *
 *
 *	long tryhere ():
 *
 *	Return an output file to the local machine.
 *	If successful, return RCM_STAGEOUT and unlink tempname.
 *	If unsuccessful, return RCM_STAGEOUTFAI and do not unlink.
 */
static long tryhere (temp, dest, statbuf, mode)
char *temp;				/* Temporary name */
char *dest;				/* Destination name */
struct stat *statbuf;			/* Result of stat() on temp */
int mode;				/* Destination file mode */
{
	void inodeavail();		/* I-node became available message */
	void waitinode();		/* Waiting for i-node to become */
					/* available */

	short waitmsg;			/* Wait message flag */
	short linkstatus;		/* Result of link() call */

	if (statbuf->st_nlink != 1) {
		/*
		 *  The output process has already been completed by
		 *  linking the final file to the temporary.  We just
		 *  have not gotten around to deleting the temporary
		 *  file just yet.
		 */
		unlink (temp);
		return (RCM_STAGEOUT);
	}
	waitmsg = 0;				/* No lack of inodes yet */
	while ((linkstatus = link (temp, dest)) == -1 && errno == ENOSPC) {
		if (!waitmsg) {
			waitmsg = 1;
			waitinode();		/* Write log mssg that we are */
		}				/* waiting.... */
		nqssleep (RESOURCE_WAIT);	/* Sleep for a while */
	}
	if (waitmsg) inodeavail();		/* I-node became available */
	if (linkstatus == 0) {
		/*
		 *  The simple link case succeeded.
		 */
		chmod (dest, mode);		/* Update mode */
		unlink (temp);			/* Remove temp file */
		return (RCM_STAGEOUT);		/* Complete success */
	}
	/*
	 *  The link failed.  Try copying the file.
	 */
	if (copy (temp, dest, mode) == 0) {
		/*
		 *  File copy successful.
		 */
		unlink (temp);			/* Remove temp file */
		return (RCM_STAGEOUT);		/* Complete success */
	}
	return (mergertcm (RCM_STAGEOUTFAI, errnototcm (errno)));
}


/*** copy
 *
 *
 *	int copy():
 *	Copy temp output file to specified destination file.
 *
 *	Returns:
 *		0: if successful.
 *	       -1: if unsuccessful.
 */
static int copy (temp, dest, mode)
char *temp;				/* Name of temporary file */
char *dest;				/* Name of destination file */
int mode;				/* File creation mode */
{
	int robustopen();			/* Our version of open() */

	register int srcfd;
	register int desfd;

	if ((srcfd = robustopen (temp, O_RDONLY, 0)) == -1) {
		return (-1);
	}
	if ((desfd = robustopen (dest, O_WRONLY|O_CREAT|O_TRUNC, mode)) == -1) {
		return (-1);
	}
	/*
	 *  Copy the output file.
	 */
	if (filecopyentire (srcfd, desfd) == -1) {
		/*
		 *  The file copy failed.
		 */
		unlink (dest);		/* Unlink destination */
		return (-1);
	}
	/*
	 *  The file copy succeeded!
	 */
	return (0);			/* Success */
}


/*** robustopen
 *
 *
 *	int robustopen ():
 *
 *	Behave just as the open(2) system call does, only
 *	try to turn failure into success.
 */
static int robustopen (path, flags, mode)
char *path;					/* Path name */
int flags;					/* O_??? */
int mode;					/* Rwx permissions, not */
						/* used if we only read */
{
	void descravail();		/* File-descr has become available */
	void inodeavail();		/* Free i-node has become available */
	void waitdescr();		/* Waiting for file descr msg */
	void waitinode();		/* Waiting for free i-node msg */
	
	short descr_wait;			/* Boolean */
	short inode_wait;			/* Boolean */
	int fd;					/* File descriptor */

	/*
	 *  If the system file table is full, or no free i-nodes exist,
	 *  then we wait, and wait, and wait...........
	 */
	descr_wait = 0;
	inode_wait = 0;
	/*
	 * C allows us to pass the wrong number of args to open().
	 */
	while (
		((fd = open (path, flags, mode)) == -1) &&
		(errno == ENFILE || errno == EINTR || errno == ENOSPC)
		) {
		if (errno != EINTR) {
			if (errno == ENFILE) {
				if (inode_wait) {
					inode_wait = 0;
					inodeavail ();
				}
				if (!descr_wait) {
					descr_wait = 1;
					waitdescr ();
				}
			}
			else {
				if (descr_wait) {
					descr_wait = 0;
					descravail ();
				}
				if (!inode_wait) {
					inode_wait = 1;
					waitinode ();
				}
			}
			nqssleep (RESOURCE_WAIT);
		}
	}
	return (fd);
}


/*** descravail
 *
 *
 *	void descravail():
 *	Write-out file-descriptor has become available message.
 */
static void descravail()
{
	printf ("I$File descriptor has become available.\n");
	printf ("I$Process %1d is no longer blocked.\n", getpid());
	fflush (stdout);
}


/*** inodeavail
 *
 *
 *	void inodeavail():
 *	Write-out i-node has become available message.
 */
static void inodeavail()
{
	printf ("I$Inode has become available.\n");
	printf ("I$Process %1d is no longer blocked.\n", getpid());
	fflush (stdout);
}


/*** waitdescr
 *
 *
 *	void waitdescr():
 *	Write-out waiting for file descriptor message.
 */
static void waitdescr()
{
	printf ("W$Process %1d waiting for system ", getpid());
	printf ("file table overflow\n");
	printf ("W$activity to cease in order to return output file.\n");
	fflush (stdout);
}


/*** waitinode
 *
 *
 *	void waitinode():
 *	Write-out waiting for available i-node message.
 */
static void waitinode()
{
	printf ("W$Process %1d waiting for free i-node to", getpid());
	printf ("become available\n");
	printf ("W$in order to return output file.\n");
	fflush (stdout);
}


/*** lostsink
 *
 *
 *	void lostsink():
 *	Called upon receiving SIGPIPE.
 */
static void lostsink()
{
	signal (SIGPIPE, lostsink);
	serexit (mergertcm (RCM_STAGEOUTFAI, errnototcm (EPIPE)), (char *) 0);
}

