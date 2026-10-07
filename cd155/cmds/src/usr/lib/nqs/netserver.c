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
 
/*++ netserver.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/netserver.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS network server. Exec'd when necessary by the network daemon.
 *
 *	
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	April 23, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.6 $ $Date: 1994/11/19 02:52:44 $ $State: Exp $)
 * $Log: netserver.c,v $
 * Revision 1.6  1994/11/19  02:52:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/11/02  00:57:02  mwan
 * R1.2 mods
 *
 * Revision 1.4  1993/07/13  17:48:23  mwan
 * T11 - fixed PTS - 5012 and others.
 *
 * Revision 1.3  1992/12/16  23:26:09  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:24:48  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:42  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:04:12  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)netserver.c	50.2 (netserver.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>			/* Standard I/O */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include <errno.h>			/* System call error numbers */
#include <signal.h>			/* For SIGPIPE */
#include <pwd.h>			/* Password stuff */
#include "nqs.h"			/* Types and definitions */
#include <sys/stat.h>			/* So we can call stat() */
#include "nqspacket.h"			/* To talk to the local daemon */
#include "netpacket.h"			/* Network packet types */
#include "informcc.h"			/* Information completion codes */
#include "requestcc.h"			/* Request completion codes */
#include "transactcc.h"			/* Transaction completion codes */
#include "nqsdirs.h"			/* Pathnames of directories */

extern int atoi();			/* Ascii to integer */
extern void bufstderr();		/* Block buffer stderr */
extern void bufstdout();		/* Block buffer stdout */
extern long delreq();			/* Sends packet to local daemon */
extern long errnototcm();		/* Convert errno to tcm */
extern void exiting();			/* Relinquish the inter-process */
					/* communication file from the */
					/* local NQS daemon */
extern FILE *fdopen();			/* Open fd as stream */
extern struct passwd *fetchpwuid();	/* NQS version of getpwuid */
extern long filecopysized();		/* File copy, stopping BEFORE EOF */
extern char *getenv();			/* Get environment var text */
extern int getrreq();			/* Get the header for reading */
extern int getsockch();			/* Get a char from a socket */
extern long inter();			/* Send bytes to local daemon */
extern void interclear();		/* Get a clean slate */
extern int interfmt();			/* Format for delivery */
extern int intern32i();			/* Number of 32 bit ints */
extern int internstr();			/* Number of strings */		
extern long interr32i();		/* Read a 32 bit int */
extern int interr32sign();		/* Return sign bit of i-th packet */
					/* integer datum */
extern unsigned long interr32u();	/* Return i-th packet unsigned int */
extern int interread();			/* Read a message packet */
extern char *interrstr();		/* Read a string */
extern void interset();			/* Set the file-descriptor used */
					/* to communicate messages to the */
					/* local NQS daemon */
extern void interw32i();		/* Write a 32 bit int */
extern long lseek();			/* System call */
extern void pack6name();		/* Construct a pathname */
extern int readhdr();			/* Read rawreq header into core */
extern int readreq();			/* Read a rawreq into core */
extern long setpeertcm();		/* Flip on peer bit of tcm */
extern void setsockfd();		/* Teach getsockch() */
extern int strlen();			/* String length */
extern char *strncpy();			/* Anchored string copy */
extern int tra_read();			/* Read a transaction descriptor */
extern int tra_setstate();		/* Set half of transaction descriptor */
extern int verifyreq();			/* Verify request */
extern int writereq();			/* Write header to control file */

extern int errno;			/* System call error# */

/*
 *	Variables global to this module.
 */
static int Debug;
static FILE *Logfile;
static char *Argv0;
static int Argv0size;

/*** main
 *
 *
 *	main():
 *
 *	This process is exec'd by the child of the network daemon
 *	with the following file descriptors open as described:
 *
 *	File descriptor 0: The new socket.
 *	File descriptor 1: The NQS log process.
 *	File descriptor 2: The NQS log process.
 *	File descriptor 3: Connected to the local NQS daemon request
 *			   FIFO pipe.
 *	File descriptor 4: The NQS log process.
 *	File descriptors [5.._NFILE] are closed.
 *
 *	The current working directory of this process is the NQS
 *	root directory.
 *
 *	All signal actions are set to SIG_DFL.
 *
 *	The environment of this process contains the environment string:
 *
 *		DEBUG=nnn
 *
 *	where nnn specifies (in integer ASCII decimal format), the
 *	debug level in effect when this server was exec'd over
 *	the child of the NQS netdaemon.
 *
 *	The environment of this process also contains the following
 *	environment variables:
 *
 *		OP=opstring
 *		CLIENTMID=mid
 *
 *	where opstring is one of:
 *
 *		OP=NPK_MVOUTFILE
 *			Catch one or more staged-out files
 *		OP=NPK_QUEREQ
 *			Catch the header of a control file, and
 *			attempt to put a request in the arrive state in
 *			a queue on this machine
 *		OP=NPK_REQFILE
 *			Catch the tail of a control file, and all
 *			of the data files associated with this request
 *
 *	and 	mid	is the ASCII decimal integer value of
 *			the client's machine-id.
 *
 */
main (argc, argv, envp)
int argc;
char *argv [];
char *envp [];
{
	void mvoutfileserver();
	void quereqserver();
	void reqfileserver();
	void server_exit();

	char *cp;
	mid_t clientmid;
	char Logfile_buffer [BUFSIZ];		/* Logfile output buffer */
	
	Argv0 = argv [0];
	Argv0size = strlen (Argv0);
	sprintf (Argv0, "%-*s", Argv0size, "NQS netserver");
	if ((cp = getenv ("DEBUG")) != (char *) 0) Debug = atoi (cp);
	else Debug = 0;
	if ((cp = getenv ("CLIENTMID")) == (char *) 0) {
		server_exit ();
	}
	clientmid = atoi (cp);
	if ((cp = getenv ("OP")) == (char *) 0) {
		server_exit ();
	}
	if ((Logfile = fdopen (4, "w")) == (FILE *) 0) {
		server_exit ();
	}
	bufstderr();			/* In case, anyone ever decides to */
	bufstdout();			/* write on these streams */
#if	BSD42 | BSD43 | ULTRIX
	setbuffer (Logfile, Logfile_buffer, BUFSIZ);
#else
#if	SGI | SYS52 | UTS
	setvbuf (Logfile, _IOFBF, Logfile_buffer, BUFSIZ);
#else
#if	UNICOS | OSF
	setvbuf (Logfile, Logfile_buffer, _IOFBF, BUFSIZ);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
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
	fflush (Logfile);		/* Logfile is a pipe */
	if (Debug > 2) {
		while (*argv != NULL) {
			fprintf (Logfile, "D$Netserver argv: %s\n", *argv);
			argv++;
		}
		while (*envp != NULL) {
			fprintf (Logfile, "D$Netserver envp: %s\n", *envp);
			envp++;
		}
		fflush (Logfile);
	}
	/*
	 * We ignore SIGPIPE. We MUST check the return value of all
	 * write() calls against the number we sought to write,
	 * not against -1.
	 */
	signal (SIGPIPE, SIG_IGN);
	/*
	 * Mvoutfileserver, quereqserver, and reqfileserver exit() when done.
	 */
	if (!strcmp (cp, "NPK_MVOUTFILE")) mvoutfileserver ();
	if (!strcmp (cp, "NPK_QUEREQ")) quereqserver (clientmid);
	if (!strcmp (cp, "NPK_REQFILE")) reqfileserver ();
}


/*** mvoutfileserver
 *
 *
 *	void mvoutfileserver():
 *
 *	Catch one or more files that are being staged out.
 *	If our work is aborted before it is finished, do not
 *	unlink any of the files that are already here.
 *	
 */
static void mvoutfileserver ()
{
	void sendtcm();
	void server_exit();

	struct passwd *passwd;		/* Pointer to password entry */
	int integers;			/* Number of integers */
	int strings;			/* Number of strings */
	char *path;			/* Where to put the file */
	int fd;				/* File descriptor */
	short done;			/* Boolean */

	if ((passwd = fetchpwuid (getuid())) == (struct passwd *) 0) {
		sendtcm (TCMP_NOACCAUTH);
		server_exit ();
	}
	else {
		if (chdir (passwd->pw_dir) == -1) {
			sendtcm (setpeertcm (errnototcm (errno)));
			server_exit ();
		}
	}
	sendtcm (TCMP_CONTINUE);
	/*
	 * We are now ready to work for the client
	 */
	setsockfd (0);			/* Tell interread we'll use fd #0 */
	/*
	 *  The client sends the following packets:
	 *  1: the daemon already saw this packet
	 *
	 *  2: this packet contains:
	 *	a) NPK_MVOUTFILE
	 *	b) mode
	 *	c) size
	 *	d) destination pathname
	 *
	 *  3 thru n-1: these optional packets contain:
	 *	a) the file mentioned previously
	 *	b) NPK_MVOUTFILE
	 *	c) mode
	 *	d) size
	 *	e) destination pathname
	 *
	 *  n: this packet contains:
	 *	a) the file mentioned previously
	 *	b) NPK_DONE
	 *	c) a place holder
	 *	d) a place holder
	 *	e) a place holder
	 *
	 *  We first process packet 2 and part of 3,
	 *  then the rest of packet 3 and part of 4,
	 *		   ...
	 *  then the rest of packet n-1 and all of n.
	 *
	 *  Get size information.
	 */
	errno = 0;			/* Set errno to zero for below */
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		server_exit ();
	case -2:
		if (errno) fprintf (Logfile, "I$Netserver errno: %s.\n",
				    asciierrno());
		server_exit();
	}
	integers = intern32i ();
	strings = internstr ();
	done = 0;
	if (integers != 3 || strings != 1) {
		sendtcm (TCMP_PROTOFAIL);
		server_exit();
	}
	for ( ;interr32i (1) == NPK_MVOUTFILE; ) {
		/*
		 * The client has told us about a file.  Get that file.
		 * int 1: NPK_MVOUTFILE
		 * int 2: mode (not umask)
		 * int 3: size
		 * str 1: destination
		 */
		if (Debug > 2) {
			fprintf (Logfile, "D$Netserver: mode = %d\n",
				interr32i (2));
			fprintf (Logfile, "D$Netserver: size = %d\n",
				interr32i (3));
			fprintf (Logfile, "D$Netserver: path = %s\n",
				interrstr (1));
			fflush (Logfile);
		}
		path = interrstr (1);
		if ((fd = open (path, O_WRONLY | O_CREAT | O_TRUNC,
			interr32i (2) & 0777)) == -1) {
			sendtcm (setpeertcm (errnototcm (errno)));
			server_exit ();
		}
		sendtcm (TCMP_CONTINUE);
		errno = 0;
		if (filecopysized (0, fd, interr32i(3)) != interr32i (3)) {
			sendtcm (setpeertcm (errnototcm (errno)));
			unlink (path);
			server_exit ();
		}
		errno = 0;		/* Set errno to zero for below */
		switch (interread (getsockch)) {
		case 0:
			break;
		case -1:
			server_exit ();
		case -2:
			if (errno) {
				fprintf (Logfile, "I$Netserver errno: %s.\n",
					 asciierrno());
			}
			server_exit();
		}
		integers = intern32i ();
		strings = internstr ();
		if (integers != 3 || strings != 1) {
			sendtcm (TCMP_PROTOFAIL);
			server_exit();
		}
	}				/* End of for loop */
	sendtcm (TCMP_COMPLETE);
	server_exit ();
}


/*** quereqserver
 *
 *
 *	void quereqserver ():
 *
 *	Catch a request from a pipe queue.
 *	If our work is aborted before it is finished, do not
 *	unlink any of the files that are already here.
 */
static void quereqserver (clientmid)
mid_t clientmid;			/* Client machine id */
{
	int receivehdr();
	void sendtcm();
	void server_exit();

	char packedname[MAX_PATHNAME+1];/* Holds relative path name */
	struct rawreq rawreq;		/* To hold most of control file */
	struct stat statbuf;		/* Holds output of stat() */
	struct rawreq rawreq2;		/* Header already on server machine */
	long quereqtcm;			/* Transaction completion code */
	int ctrlfd;			/* Control file file descriptor */
	int integers;			/* Number of integers */
	int strings;			/* Number of strings */
	struct transact transact;	/* In-core transaction descriptor */
	long transactcc;		/* Transaction completion code */

	interset (3);			/* This "inter" is unlike the others */
	sendtcm (TCMP_CONTINUE);
	/*
	 * We are now ready to work for the client
	 */
	setsockfd (0);			/* Tell interread we'll use fd #0 */
	/*
	 * The client sends the following packets
	 * as part of the enqueueing connection:
	 * 1: The daemon already saw this packet
	 * 2: This packet contains the control file header
	 *	An immense amount of information is received.
	 * 3: This packet commits the two phase commit
	 *
	 * The client sends the following packets
	 * as part of the request file delivery connection:
	 * 1: The daemon will see this packet later
	 * 2: reqfileserver will see this packet later
	 * 3 thru n-1: reqfileserver will see these packets later
	 * n: reqfileserver will see this packet later
	 */
	if (receivehdr (&rawreq) == -1) {
		sendtcm (TCMP_PROTOFAIL);
		server_exit ();
	}
	pack6name (packedname, Nqs_control,
		(int) (rawreq.orig_seqno % MAX_CTRLSUBDIRS),
		(char *) 0, rawreq.orig_seqno, 5, (long) rawreq.orig_mid,
		6, 0, 0);
	if (stat (packedname, &statbuf) == -1) {
		if (errno != ENOENT) {
			sendtcm (setpeertcm (errnototcm ()));
			server_exit ();
		}
	}
	else if (statbuf.st_uid != getuid ()) {
		sendtcm (TCMP_REQCOLLIDE);
		server_exit ();
	}
	/*
	 * At this point, there are two possibilities:
	 * a) the file does not exist
	 * b) the file does exist, and we own it
	 */
	if ((ctrlfd = open (packedname, O_RDWR | O_CREAT | O_EXCL, 0744))
		== -1) {
		if (errno != EEXIST) {
			sendtcm (setpeertcm (errnototcm ()));
			server_exit ();
		}
		/*
		 * There is a control file already here, and we own it.
		 */
		ctrlfd = getrreq (rawreq.orig_seqno, rawreq.orig_mid, &rawreq2);
		if (ctrlfd == -1) {
			if (errno != 0) {
				/*
				 * We are prevented from learning
				 * about the old request.
				 */
				sendtcm (setpeertcm (errnototcm ()));
				server_exit ();
			}
		}
		else {
			if (rawreq.create_time != rawreq2.create_time) {
				sendtcm (TCMP_REQCOLLIDE);
				server_exit ();
			}
		}
		/*
		 * The old request is either corrupt, or a duplicate of the
		 * request we are now working on. Delete the old request.
		 */
		switch (delreq (getuid (), rawreq.orig_seqno,
				rawreq.orig_mid, 0, RQS_ARRIVING)) {
		case TCML_NOSUCHREQ:
			unlink (packedname);
			break;
		case TCML_REQDELETE:
			break;
		default:
			unlink (packedname);
			sendtcm (TCMP_UNAFAILURE);
			server_exit ();
		}
		/*
		 * The server machine has completely obliterated all
		 * traces of the request that was in conflict with us.
		 */
		if ((ctrlfd = open ( packedname,
			O_RDWR | O_CREAT | O_EXCL, 0744)) == -1) {
			sendtcm (setpeertcm (errnototcm ()));
			server_exit ();
		}
	}
	/*
	 * At this point, we have an open file descriptor,
	 * and any old request that might have been at the same
	 * path name is gone.
	 */
	if (writereq (ctrlfd, &rawreq) != 0) {
		unlink (packedname);
		sendtcm (setpeertcm (errnototcm ()));
		server_exit ();
	}
	/*
	 * At this point, the control file header is
	 * in the file system, and is not corrupt.
	 */
	interclear ();
	interw32i (rawreq.orig_seqno);
	interw32i ((long) rawreq.orig_mid);
	interw32i ((long) clientmid);
	/*
	 * Rawreq.quename was filled in with the name of the destination
	 * queue in receivehdr().
	 */
	interwstr (rawreq.quename);
	if (((quereqtcm = inter (PKT_RMTQUEREQ)) & XCI_FULREA_MASK)
		!= TCML_SUBMITTED) {
		if (Debug > 2) {
			fprintf (Logfile, "D$Netserver: quereqtcm oct = %o\n",
				quereqtcm);
			fflush (Logfile);
		}
		sendtcm (setpeertcm (quereqtcm));
		server_exit ();
	}
	/*
	 * The local daemon has just allocated a transaction descriptor
	 * and set the state of the transaction to RTS_PREARRIVE.
	 * Find out which transaction descriptor.
	 */
	if (readhdr (ctrlfd, &rawreq) != 0) {
		unlink (packedname);
		sendtcm (setpeertcm (errnototcm ()));
		server_exit ();
	}
	sendtcm (TCMP_CONTINUE);
	/*
	 * Get packet 3 of the enqueueing connection.
	 */
	if (Debug > 2) {
		fprintf (Logfile, "D$Netserver: before reading NPK_COMMIT\n");
		fflush (Logfile);
	}
	errno = 0;			/* Set errno to zero for below */
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		server_exit ();
	case -2:
		if (errno) fprintf (Logfile, "I$Netserver errno: %s.\n",
				    asciierrno());
		server_exit();
	}
	integers = intern32i ();
	strings = internstr ();
	if (integers != 1 || strings != 0) {
		sendtcm (TCMP_PROTOFAIL);
		server_exit();
	}
	if (interr32i (1) != NPK_COMMIT) {
		sendtcm (TCMP_PROTOFAIL);
		server_exit ();
	}
	if (Debug > 2) {
		fprintf (Logfile, "D$Netserver: after reading NPK_COMMIT\n");
		fflush (Logfile);
	}
	transact.state = RTS_ARRIVE;
	tra_setstate (rawreq.trans_id, &transact);
	interclear ();
	interw32i (rawreq.orig_seqno);
	interw32i ((long) rawreq.orig_mid);
	if ((transactcc = inter (PKT_RMTARRIVE)) != TCML_COMPLETE) {
		sendtcm (setpeertcm (transactcc));
		server_exit ();
	}
	tra_read (rawreq.trans_id, &transact);
	sendtcm (setpeertcm (quereqtcm));
	server_exit ();
}


/*** receivehdr 
 *
 *
 *	int receivehdr():
 *
 *	Receive NQS request header file from a remote client connected
 *	via a stream socket connection.
 *
 *	Returns:
 *		 0: if successful;
 *		-1: otherwise (connection severed).
 */
static int receivehdr (rawreq)
register struct rawreq *rawreq;		/* Raw request to be packaged */
{
	typedef unsigned long longcard;	/* Less typing */

	int unpackheader();
	
	register short i;		/* Loop and temp var */
	int n_integers;			/* Number of packet integers */
	int n_strings;			/* Number of packet strings */

	i = interread (getsockch);
	if (i < 0) {
		/*
		 *  The client process terminated, or sent us a bad
		 *  packet (i == -1 is EOF).
		 */
		if (i == -2) {
			/*
			 *  Bad message packet received from client.
			 *  Inform client.
			 */
			fprintf (Logfile,
				 "W$Netserver: Inconsistent packet recv'd.\n");
			fflush (Logfile);
		}
		return (-1);		/* No request */
	}
	/*
	 *  We got a self-consistent packet.
	 */
	n_integers = intern32i();	/* #of packet integers */
	n_strings = internstr();	/* #of packet strings */
	if (n_integers == 16 + MAX_DEVPREF && n_strings == 5 + MAX_DEVPREF) {
		/*
		 *  The request had better be a device request!
		 */
		if (unpackheader (rawreq) >= 0 &&
		    rawreq->type == RTYPE_DEVICE && !interr32sign (16)) {
			/*
			 *  The common request header was successfully
			 *  unpacked, and no fields in the device variant
			 *  have been detected as invalid (yet).
			 */
			strncpy (rawreq->v.dev.forms, interrstr (5),
				 MAX_FORMNAME+1);
			rawreq->v.dev.copies = interr32i (13);
			rawreq->v.dev.reserved1 = interr32i (14);
			rawreq->v.dev.reserved2 = interr32i (15);
			rawreq->v.dev.size = interr32u (16);
			for (i = 0; i < MAX_DEVPREF; i++) {
				strncpy (rawreq->v.dev.devprefname [i],
					 interrstr (6+i), MAX_DEVNAME+1);
				rawreq->v.dev.devprefmid [i] = interr32i(17+i);
			}
			if (verifyreq (rawreq) >= 0) {
				/*
				 *  The request is valid.
				 */
				return (0);	/* Success */
			}
		}
		fprintf (Logfile,
			 "W$Netserver: Invalid device request packet.\n");
		fflush (Logfile);
		return (-1);		/* No request */
	}
	else if (n_integers != 84 + MAX_INSTAPERREQ + MAX_OUSTAPERREQ ||
		n_strings != 8 + MAX_PREDECESSOR) {
		/*
		 *  We have a protocol failure.  The packet is invalid.
		 */
		fprintf (Logfile,
			 "W$Netserver: non-batch, non-device packet.\n");
		fflush (Logfile);
		return (-1);		/* No request */
	}
	/*
	 *  The request had better be a batch request!
	 */
	if (unpackheader (rawreq) >= 0 && rawreq->type == RTYPE_BATCH) {
		/*
		 *  The common request header was successfully unpacked,
		 *  and no fields in the batch variant have been detected
		 *  as invalid (yet).
		 *
		 *  Make sure all quota coefficients are positive.
		 */
		i = 16;
		while (i <= 70 && !interr32sign (i)) i += 2;
		if (i > 70) {
			/*
			 *  No quota coefficients were detected to be out of
			 *  range.  Unpack the batch request variant fields.
			 */
			rawreq->v.bat.umask = interr32i (13);
			strncpy (rawreq->v.bat.shell_name, interrstr (5),
				 MAX_SHELLNAME+1);
			rawreq->v.bat.explicit = interr32i (14);
			rawreq->v.bat.infinite = interr32i (15);
			/*
			 *  Unpack per-process corefile size limits.
			 */
			rawreq->v.bat.ppcoresize.max_quota = interr32u (16);
			rawreq->v.bat.ppcoresize.max_units = interr32i (17);
			rawreq->v.bat.ppcoresize.warn_quota = interr32u (18);
			rawreq->v.bat.ppcoresize.warn_units = interr32i (19);
			/*
			 *  Unpack per-process data-segment size limits.
			 */
			rawreq->v.bat.ppdatasize.max_quota = interr32u (20);
			rawreq->v.bat.ppdatasize.max_units = interr32i (21);
			rawreq->v.bat.ppdatasize.warn_quota = interr32u (22);
			rawreq->v.bat.ppdatasize.warn_units = interr32i (23);
			/*
			 *  Unpack per-process permanent file size limits.
			 */
			rawreq->v.bat.pppfilesize.max_quota = interr32u (24);
			rawreq->v.bat.pppfilesize.max_units = interr32i (25);
			rawreq->v.bat.pppfilesize.warn_quota = interr32u (26);
			rawreq->v.bat.pppfilesize.warn_units = interr32i (27);
			/*
			 *  Unpack per-request permanent file space limits.
			 */
			rawreq->v.bat.prpfilespace.max_quota = interr32u (28);
			rawreq->v.bat.prpfilespace.max_units = interr32i (29);
			rawreq->v.bat.prpfilespace.warn_quota = interr32u (30);
			rawreq->v.bat.prpfilespace.warn_units = interr32i (31);
			/*
			 *  Unpack per-process quick file size limits.
			 */
			rawreq->v.bat.ppqfilesize.max_quota = interr32u (32);
			rawreq->v.bat.ppqfilesize.max_units = interr32i (33);
			rawreq->v.bat.ppqfilesize.warn_quota = interr32u (34);
			rawreq->v.bat.ppqfilesize.warn_units = interr32i (35);
			/*
			 *  Unpack per-request quick file space limits.
			 */
			rawreq->v.bat.prqfilespace.max_quota = interr32u (36);
			rawreq->v.bat.prqfilespace.max_units = interr32i (37);
			rawreq->v.bat.prqfilespace.warn_quota = interr32u (38);
			rawreq->v.bat.prqfilespace.warn_units = interr32i (39);
			/*
			 *  Unpack per-process temporary file size limits.
			 */
			rawreq->v.bat.pptfilesize.max_quota = interr32u (40);
			rawreq->v.bat.pptfilesize.max_units = interr32i (41);
			rawreq->v.bat.pptfilesize.warn_quota = interr32u (42);
			rawreq->v.bat.pptfilesize.warn_units = interr32i (43);
			/*
			 *  Unpack per-request temporary file space limits.
			 */
			rawreq->v.bat.prtfilespace.max_quota = interr32u (44);
			rawreq->v.bat.prtfilespace.max_units = interr32i (45);
			rawreq->v.bat.prtfilespace.warn_quota = interr32u (46);
			rawreq->v.bat.prtfilespace.warn_units = interr32i (47);
			/*
			 *  Unpack per-process memory size limits.
			 */
			rawreq->v.bat.ppmemsize.max_quota = interr32u (48);
			rawreq->v.bat.ppmemsize.max_units = interr32i (49);
			rawreq->v.bat.ppmemsize.warn_quota = interr32u (50);
			rawreq->v.bat.ppmemsize.warn_units = interr32i (51);
			/*
			 *  Unpack per-request memory size limits.
			 */
			rawreq->v.bat.prmemsize.max_quota = interr32u (52);
			rawreq->v.bat.prmemsize.max_units = interr32i (53);
			rawreq->v.bat.prmemsize.warn_quota = interr32u (54);
			rawreq->v.bat.prmemsize.warn_units = interr32i (55);
			/*
			 *  Unpack per-process stack-segment size limits.
			 */
			rawreq->v.bat.ppstacksize.max_quota = interr32u (56);
			rawreq->v.bat.ppstacksize.max_units = interr32i (57);
			rawreq->v.bat.ppstacksize.warn_quota = interr32u (58);
			rawreq->v.bat.ppstacksize.warn_units = interr32i (59);
			/*
			 *  Unpack per-process working set size limits.
			 */
			rawreq->v.bat.ppworkset.max_quota = interr32u (60);
			rawreq->v.bat.ppworkset.max_units = interr32i (61);
			rawreq->v.bat.ppworkset.warn_quota = interr32u (62);
			rawreq->v.bat.ppworkset.warn_units = interr32i (63);
			/*
			 *  Unpack per-process CPU time limits.
			 */
			rawreq->v.bat.ppcputime.max_seconds = interr32u (64);
			rawreq->v.bat.ppcputime.max_ms = interr32i (65);
			rawreq->v.bat.ppcputime.warn_seconds = interr32u (66);
			rawreq->v.bat.ppcputime.warn_ms = interr32i (67);
			/*
			 *  Unpack per-request CPU time limits.
			 */
			rawreq->v.bat.prcputime.max_seconds = interr32u (68);
			rawreq->v.bat.prcputime.max_ms = interr32i (69);
			rawreq->v.bat.prcputime.warn_seconds = interr32u (70);
			rawreq->v.bat.prcputime.warn_ms = interr32i (71);
			/*
			 *  Unpack remaining batch request fields.
			 */
			rawreq->v.bat.ppnice = interr32i (72);
			rawreq->v.bat.prdrives = interr32i (73);
			rawreq->v.bat.prncpus = interr32i (74);
			for (i=0; i < MAX_PREDECESSOR; i++) {
				strncpy (rawreq->v.bat.predecessors [i],
					 interrstr (i + 6), MAX_REQNAME+1);
			}
			rawreq->v.bat.stderr_acc = interr32i (75);
			rawreq->v.bat.stdlog_acc = interr32i (76);
			rawreq->v.bat.stdout_acc = interr32i (77);
			rawreq->v.bat.stderr_mid = interr32i (78);
			rawreq->v.bat.stdlog_mid = interr32i (79);
			rawreq->v.bat.stdout_mid = interr32i (80);
			strncpy (rawreq->v.bat.stderr_name,
				 interrstr (MAX_PREDECESSOR+6), MAX_REQPATH+1);
			strncpy (rawreq->v.bat.stdlog_name,
				 interrstr (MAX_PREDECESSOR+7), MAX_REQPATH+1);
			strncpy (rawreq->v.bat.stdout_name,
				 interrstr (MAX_PREDECESSOR+8), MAX_REQPATH+1);
			rawreq->v.bat.instacount = interr32i (81);
			rawreq->v.bat.oustacount = interr32i (82);
			rawreq->v.bat.instahiermask = interr32i (83);
			rawreq->v.bat.oustahiermask = interr32i (84);
			for (i=0; i < MAX_INSTAPERREQ; i++) {
				rawreq->v.bat.instamid [i] = interr32i (i+85);
			}
			for (i=0; i < MAX_OUSTAPERREQ; i++) {
				rawreq->v.bat.oustamid [i]
					= interr32i (i + MAX_INSTAPERREQ + 85);
			}
			if (verifyreq (rawreq) >= 0) {
				/*
				 *  The request is valid.
				 */
				return (0);	/* Success */
			}
		}
	}
	fprintf (Logfile,
		 "W$Netserver: Invalid batch request packet.\n");
	fflush (Logfile);
	return (-1);			/* No request to queue */
}


/*** reqfileserver
 *
 *
 *	void reqfileserver ():
 *
 *	Catch the tail of a control file and all
 *	the data files associated with a request.
 *	If our work is aborted before it is finished, do not
 *	unlink any of the files that are already here.
 */
static void reqfileserver ()
{
	void sendtcm();
	void server_exit();
 
	int integers;			/* Number of integers */
	int strings;			/* Number of strings */
	struct rawreq rawreq;		/* To hold most of control file */
	char packedname [29];		/* Holds relative path name */
					/* dir:14, file:14, slash:1 */
	struct stat statbuf;		/* Holds stat() output */
	int ctrlfd;			/* Control file file descriptor */
	long headersize;		/* Bytes in control file header */
	int datafd;			/* Data file file descriptor */
	struct transact transact;	/* In-core transaction descriptor */
	long transactcc;		/* Transaction completion code */

	interset (3);			/* This "inter" is unlike the others */
	sendtcm (TCMP_CONTINUE);
	/*
	 * We are now ready to work for the client
	 */
	setsockfd (0);			/* Tell interread we'll use fd #0 */
	/*
	 *  The client sends the following packets in the
	 *  enqueueing connection:
	 *  1: The daemon already saw this packet
	 *  2: quereqserver already saw this packet
	 *  3: quereqserver already saw this packet
	 *
	 *  The client sends the following packets in the delivery
	 *  connection:
	 *  1: The daemon already saw this packet
	 *  2: This packet contains 
	 *	a) packet type (NPK_REQFILE)
	 *	b) ordinal number of file (-1 == tail of control file)
	 *	c) number of bytes in file
	 *	d) original sequence number
	 *	e) original mid
	 *
	 *  3 thru n-1: These optional packets contain:
	 *	a) the file mentioned previously
	 *	b) packet type (NPK_REQFILE)
	 *	c) ordinal number of file
	 *	d) number of bytes in file
	 *
	 *  n: This packet contains:
	 *	a) the file mentioned previously
	 *	b) packet type (NPK_DONE)
	 *	c) a place holder
	 *	d) a place holder
	 *
	 *  Get packet 2 of the deliver connection.
	 */
	errno = 0;			/* Set to zero for below */
	switch (interread (getsockch)) {
	case 0:
		break;
	case -1:
		server_exit ();
	case -2:
		if (errno) fprintf (Logfile, "I$Netserver errno: %s.\n",
				    asciierrno());
		server_exit();
	}
	integers = intern32i ();
	strings = internstr ();
	if (integers != 5 || strings != 0) {
		sendtcm (TCMP_PROTOFAIL);
		server_exit();
	}
	if (Debug > 2) {
		fprintf (Logfile, "D$Netserver: NPK = %d\n", interr32i (1));
		fprintf (Logfile, "D$Netserver: data# = %d\n", interr32i (2));
		fprintf (Logfile, "D$Netserver: bytes = %d\n", interr32i (3));
		fprintf (Logfile, "D$Netserver: seqno = %d\n", interr32i (4));
		fprintf (Logfile, "D$Netserver: mid = %d\n", interr32i (5));
		fflush (Logfile);
	}
	pack6name (packedname, Nqs_control,
		(int) (interr32i (4) % MAX_CTRLSUBDIRS), (char *) 0,
		interr32i (4), 5, interr32i (5), 6, 0, 0);
	if (stat (packedname, &statbuf) == -1) {
		/*
		 * It might be that a previous delivery worked, the
		 * client machine crashed before hearing the news,
		 * and the batch request has already run to completion.
		 */
		if (errno == ENOENT) sendtcm (TCMP_NOSUCHREQ);
		else sendtcm (setpeertcm (errnototcm ()));
		server_exit ();
	}
	if (statbuf.st_uid != getuid ()) {
		sendtcm (TCMP_REQCOLLIDE);
		server_exit ();
	}
	/*
	 * At this point, we know that the control file exists,
	 * and that we own it.
	 */
	if ((ctrlfd = open (packedname, O_RDWR)) == -1) {
		sendtcm (setpeertcm (errnototcm ()));
		server_exit ();
	}
	if (readreq (ctrlfd, &rawreq) == -1) {
		sendtcm (TCMP_BADCDTFIL);
		server_exit ();
	}
	/*
	 * It might be that a previous delivery worked, and
	 * the client machine crashed before hearing the news.
	 */
	tra_read (rawreq.trans_id, &transact);
	if (transact.state != RTS_ARRIVE) {
		fprintf (Logfile, "D$Netserver: caught 2nd delivery.\n");
		sendtcm (TCMP_COMPLETE);
		server_exit ();
	}
	/*
	 * Below here, ctrlfd is still open
	 */
	for ( ;interr32i (1) == NPK_REQFILE; ) {
		if (Debug > 2) {
			fprintf (Logfile, "D$Netserver: catch %d bytes\n",
				interr32i (3));
			fflush (Logfile);
		}
		if (interr32i (2) == -1) {
			/*
			 * Tail of control file
			 */
			headersize = ((char *) &rawreq.v - (char *) &rawreq);
			if (rawreq.type == RTYPE_DEVICE) {
				headersize += sizeof (struct rawdevreq);
			}
			else {
				headersize += sizeof (struct rawbatreq);
			}
			lseek (ctrlfd, (long) headersize, 0);
			sendtcm (TCMP_CONTINUE);
			errno = 0;
			if (filecopysized (0, ctrlfd, interr32i(3))
				!= interr32i (3)) {
				sendtcm (setpeertcm (errnototcm (errno)));
				server_exit ();
			}
		}
		else {
			/*
			 * Data file
			 * If there are n data files, interr32i(2)
			 * will range from [0..n-1].
			 */
			if (interr32i (2) >= rawreq.ndatafiles) {
				sendtcm (TCMP_BADCDTFIL);
				server_exit ();
			}
			pack6name (packedname, Nqs_data,
				(int) (rawreq.orig_seqno % MAX_DATASUBDIRS),
				(char *) 0, rawreq.orig_seqno, 5,
				(long) rawreq.orig_mid, 6, interr32i (2), 3);
			if ((datafd = open (packedname,
				O_WRONLY | O_CREAT | O_TRUNC, 0777)) == -1) {
				sendtcm (setpeertcm (errnototcm (errno)));
				server_exit ();
			}
			else sendtcm (TCMP_CONTINUE);
			errno = 0;
			if (filecopysized (0, datafd, interr32i(3))
				!= interr32i (3)) {
				sendtcm (setpeertcm (errnototcm (errno)));
				server_exit ();
			}
		}
		errno = 0;		/* Set to zero for below */
		switch (interread (getsockch)) {
		case 0:
			break;
		case -1:
			server_exit ();
		case -2:
			if (errno) {
				fprintf (Logfile, "I$Netserver errno: %s.\n",
					 asciierrno());
			}
			server_exit();
		}
		integers = intern32i ();
		strings = internstr ();
		if (integers != 3 || strings != 0) {
			sendtcm (TCMP_PROTOFAIL);
			server_exit();
		}
	}
	/*
	 * We just saw NPK_DONE.
	 */
	transact.state = RTS_STASIS;
	tra_setstate (rawreq.trans_id, &transact);
	interclear ();
	interw32i (rawreq.orig_seqno);
	interw32i (rawreq.orig_mid);
	if ((transactcc = inter (PKT_RMTRECEIVED)) != TCML_COMPLETE) {
#ifdef SDSC
		fprintf (Logfile, "I$Netserver received code: %o.\n", 
		transactcc);
		switch (delreq (getuid (), rawreq.orig_seqno,
				rawreq.orig_mid, 0, RQS_ARRIVING)) {
		case TCML_NOSUCHREQ:
			unlink (packedname);
			break;
		case TCML_REQDELETE:
			break;
		default:
			unlink (packedname);
		}
#endif
		sendtcm (setpeertcm (transactcc));
		server_exit ();
	}
	sendtcm (TCMP_COMPLETE);
	server_exit ();
}


/*** sendtcm
 *
 *	void sendtcm:
 *
 *	Send on file descriptor 0 a packet containing nothing but a
 *	transaction completion code.  If there is trouble, exit.
 */
static void sendtcm (tcm)
long tcm;				/* Transaction completion code */
{
	char packet [MAX_PACKET];	/* To hold our reply */
	int packetsize;			/* Bytes in our reply */
	
	interclear ();
	interw32i (tcm);
	if ((packetsize = interfmt (packet)) == -1) {
		fprintf (Logfile, "D$Netserver interfmt.\n");
		server_exit ();
	}
	if (write (0, packet, packetsize) != packetsize) {
		fprintf (Logfile, "D$Netserver write.\n");
		server_exit ();
	}
}


/*** server_exit
 *
 *	void server_exit:
 *
 *	Exit gracefully.
 */
static void server_exit ()
{
	exiting ();			/* Unlink IPC file, close FIFO */
	exit (0);
}


/*** unpackheader
 *
 *
 *	int unpackheader():
 *	Unpack common request header from message packet.
 *
 *	Returns:
 *		 0: if successful;
 *		-1: otherwise.
 */
static int unpackheader (rawreq)
register struct rawreq *rawreq;		/* Rawreq for request */
{
	if (interr32sign (2) || interr32sign (3) || interr32sign (10)) {
		/*
		 *  Negative values encountered where only positive
		 *  numbers ( >= 0 ) are allowed.
		 */
		return (-1);
	}
	rawreq->magic1 = interr32i (1);
	rawreq->trans_id = 0;
#ifdef SDSC
	rawreq->period_inx = 0;
	rawreq->cube_account = 0;
	rawreq->major_nodes = 0;
#else
	rawreq->reserved1 = 0;
	rawreq->reserved2 = 0;
	rawreq->reserved3 = 0;
#endif
	rawreq->node_group = 0;
	rawreq->reserved5 = 0;
	rawreq->reserved6 = 0;
	strncpy (rawreq->trans_quename, "", MAX_QUEUENAME + 1);
	rawreq->create_time = interr32u (2);
	rawreq->enter_time = interr32u (3);
	strncpy (rawreq->quename, interrstr (1), MAX_QUEUENAME+1);
	rawreq->type = interr32i (4);
	rawreq->orig_uid = interr32i (5);
	rawreq->orig_mid = interr32i (6);
	rawreq->tcm = TCML_UNDEFINED;
	rawreq->orig_seqno = interr32i (7);
	rawreq->rpriority = interr32i (8);
	rawreq->flags = (interr32i (9) | RQF_EXTERNAL);
	rawreq->start_time = interr32u (10);
	rawreq->ndatafiles = interr32i (11);
	strncpy (rawreq->reqname, interrstr (2), MAX_REQNAME+1);
	strncpy (rawreq->username, interrstr (3), MAX_ACCOUNTNAME+1);
	strncpy (rawreq->mail_name, interrstr (4), MAX_ACCOUNTNAME+1);
	rawreq->mail_mid = interr32i (12);
	return (0);			/* No errors detected */
}

