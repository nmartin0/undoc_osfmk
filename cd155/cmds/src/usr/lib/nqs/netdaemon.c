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
 
/*++ netdaemon.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/netdaemon.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module is the main NQS network daemon module and processes
 *	requests from remote NQS client processes.
 *
 *	The NQS network daemon is a child of the local NQS daemon, and
 *	is in the same process group as the local NQS daemon.
 *
 *	When exec'ed from the local NQS daemon, the following file
 *	descriptors and file streams are open:
 *
 *
 *	  Stream stdout (fd 1):	writes to the NQS log
 *				process. 
 *
 *	  Stream stderr (fd 2):	writes to the NQS log
 *				process.
 *
 *	  File descriptor #3:	writes to the named request
 *				FIFO pipe of the local NQS daemon.
 *
 *
 *	The NQS network daemon CANNOT and MUST NOT be executed
 *	independently of the local NQS daemon.  The NQS network
 *	daemon must only be started up by a startup of the local
 *	NQS daemon.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Robert W. Sandstrom.
 *	Sterling Software Incorporated.
 *	December 19, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.7 $ $Date: 1995/01/24 01:10:40 $ $State: Exp $)
 * $Log: netdaemon.c,v $
 * Revision 1.7  1995/01/24  01:10:40  davidl
 * Cleaned up console_write() so it works the way it was (apparently)
 * originally intended to work.  Also cleaned up some of the calls to
 * console_write() (for example, added missing close parenthesis).
 *
 *  Reviewer: doyle
 *  Risk: Low
 *  Benefit or PTS #: 11648
 *  Testing: Developer testing, EATs
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/bytezero.c
 *             cmds_libs/src/usr/lib/nqs/netdaemon.c
 *             cmds_libs/src/usr/lib/nqs/nqs_abort.c
 *             cmds_libs/src/usr/lib/nqs/nqs_boot.c
 *             cmds_libs/src/usr/lib/nqs/nqs_main.c
 *
 * Revision 1.6  1994/12/07  22:12:27  davidl
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
 * Revision 1.5  1994/11/19  02:52:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/07  00:45:06  kremenek
 *  Reviewer:  davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 5706
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/lib/nqs/netdaemon.c, nqs_abort.c,
 *             nqs_boot.c, nqs_mai.c, nqs_main.c
 *
 * Revision 1.3  1994/05/06  20:48:12  mwan
 * Fixed 8230, 8802, 6877 and 9010.
 *
 * Reviewer: kremenek
 *  Risk: Low
 *  Benefit or PTS #: 8230, 8802, 6877 and 9010.
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c netdaemon.c nqs_pip.c nqs_reqser.c
 * 	    ccs/lib/libnqs/listq.c
 *
 * Revision 1.2  1992/10/09  22:24:45  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:40  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:03:59  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)netdaemon.c	1.4 (netdaemon.c OSF/1 NQS2.0 GJK) 6/24/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#define	WRITE_FIFO	3		/* Write pipe to daemon is on #3 */
#if	NETWORKED			/* Networking is enabled */
#if	STREAM_SOCKETS			/* Networking performed via 4.2 BSD */
					/* socket virtual circuits */
#include "nqs.h"			/* Include NQS constants/types */
					/* (includes <sys/types.h>) */
#include "nqsvars.h"			/* NQS global vars */
#include "netpacket.h"			/* NQS network packet types */
#include "transactcc.h"			/* NQS transaction completion codes */
#include NETDB				/* 4.2 BSD network database; */
					/* Proto/Makefile will tell us */
					/* where to find it */
#include <errno.h>			/* Error number */
#include <pwd.h>			/* Password stuff */
#if	UNICOS
#include <fcntl.h>			/* File control */
#include <nlist.h>
#include <sys/param.h>
#include <sys/jtab.h>
#else
#if	SGI | SYS52 | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>			/* So we can include resource.h */
#include <sys/resource.h>		/* For struct rusage */
#include <sys/wait.h>			/* For WNOHANG, union wait */
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#include <signal.h>			/* SIGINT, etc. */
#include <sys/socket.h>			/* 4.2 BSD structs */
#include <netinet/in.h>			/* Internet architecture specific */
#include <sys/stat.h>			/* For stat() */
#ifdef SDSC
#include <syslog.h>
#if OSF
#include <sys/wait.h>                   /* For WNOHANG, union wait */
#endif
#endif


/*
 *	UNIX external definitions:
 *
 *	Defined in signal.h:	extern int (* signal ())();
 */

extern int errno;			/* In case not declared in errno.h */

/*
 *	External functions:
 */
extern char *asciierrno();		/* Return string about errno */
extern int atoi();			/* ASCII digit sequence to int */
extern void closedb();			/* Close NQS database file */
extern int daepres();			/* Boolean function */
extern long delreq();			/* Delete request */
extern char *getenv();			/* Get environment var text */
extern uid_t geteuid();			/* Get effective user-id */
extern struct hostent *gethostbyaddr();	/* Get host by address */
extern int getsockch();			/* Get char from socket */
extern int interfmt();			/* Format message packet */
extern int intern32i();			/* Return number of 32-bit integers */
					/* in received message packet */
extern int internstr();			/* Return number of strings in the */
					/* received message packet */
extern long interr32i();		/* Return the n-th 32-bit integer */
					/* from the received message packet */
#if	BSD42 | BSD43 | ULTRIX
extern int interr32sign();		/* Return sign of the n-th 32-bit */
					/* precision int from message packet */
extern unsigned long interr32u();	/* Return the n-th 32-bit unsigned */
					/* long from the received packet */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
extern int interlstr();			/* Return the length of the n-th */
					/* string from the received message */
					/* packet */
extern int interread();			/* Read another message packet */
extern char *interrstr();		/* Return the n-th string from the */
					/* received message packet */
extern int interset();			/* Set file-descr for writing to */
					/* NQS daemon request FIFO */
#if	BSD42 | BSD43 | ULTRIX
extern void interwbytes();		/* Write byte string into packet */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
extern int localmid();			/* Get machine-id of local host */
extern struct passwd *mapuser();	/* Get passwd given remote info */
extern struct gendescr *nextdb();	/* Return ptr to next entry in file */
extern int nmap_get_mid();		/* Hostent -> mid */
extern struct confd *opendb();		/* Open an NQS database file */
extern int parseserv();			/* Break a string into argv format */
extern void seekdb();			/* Seek on an NQS database file */
extern void setsockfd();		/* Use with getsockch() */
extern int shoallque();			/* Show all queues */
extern int shoqbyname();		/* Show queue given name */
/*
 * Sprintf() returns an int on System V, and a char * on BSD.
 */

/*
 * Variables exported from this module.
 */
int scpflag = 0;
#if	UNICOS
struct	jtab	jtab[NJOB];
#endif

/*
 * Variables local to this module.
 */
static FILE *logfile;
 

/*** main
 *
 *
 *	int main():
 *	Start of the NQS network daemon.
 */
int main (argc, argv)
int argc;
char *argv[];
{
	void net_abort();		/* Abort network daemon */
	void net_daeshutdown();		/* Network daemon shutdown */
#if	BSD42 | BSD43 | ULTRIX| OSF
	void reapchild();		/* Wait for one or more kids */
#else
#if	SGI | SYS52 | UNICOS | UTS
#else
BAD SYSTEM TYPE
#endif
#endif
	void reload();			/* Get parameters off disk */
	void pre_server();		/* Called after fork in child */

	static char logfile_buffer [BUFSIZ];	/* logfile output buffer */

	int logfd;			/* logfile file-descriptor */
	struct sockaddr_in sin;		/* Internet socket */
	struct sockaddr_in from;	/* Socket-addr from accept() */
	struct servent *servent;	/* Server entry structure */
	struct stat stat_buf;		/* Stat() buffer */
	int sd;				/* Socket descriptor */
	int new_socket;			/* New socket from accept() */
	int from_addrlen;		/* Address length of client */

	if (getuid () != 0 || geteuid () != 0) {
		printf ("F$Netdaemon uid or euid wrong.\n");
		logfile = stdout;	/* Set pointer so that output is */
		errno = 0;		/* correctly produced in net_abort(). */
		net_abort();
	}
	Argv0 = argv[0];		/* Save pointer to daemon argv[0] */
	Argv0size = strlen (Argv0);	/* and size for later use */
	/*
	 *  Set the name of this process to a more appropriate string.
	 *
	 *  We use sprintf() to pad with spaces rather than null
	 *  characters so that slimy programs like ps(1) which
	 *  walk the stack will not get confused.
	 */
	sprintf (Argv0, "%-*s", Argv0size, "NQS netdaemon");
	/*
	 *  Create a file stream in place of stdout and stderr that
	 *  writes to the NQS log process, since stdout and stderr
	 *  might be directed back on the socket connection below.
	 */
	if ((logfd = fcntl (1, F_DUPFD, 4)) == -1) {
		printf ("F$Netdaemon unable to duplicate stdout.\n");
		logfile = stdout;	/* Set pointer so that output */
		net_abort();		/* is correctly produced in */
	}				/* net_abort(). */
	if ((logfile = fdopen (logfd, "w")) == NULL) {
		printf ("F$Netdaemon unable to create logfile stream.\n");
		logfile = stdout;	/* Set pointer so that output */
		net_abort();		/* is correctly produced in */
	}				/* net_abort(). */
	/*
	 *  Buffer output to the log process. Messages must appear
	 *  in coherent chunks, and many processes write output to
	 *  the NQS log process.  Therefore, we must flush before
	 *  our buffers get full.
	 */
#if	BSD42 | BSD43 | ULTRIX
	setbuffer (logfile, logfile_buffer, BUFSIZ);
#else
#if	SGI | SYS52 | UTS
	setvbuf (logfile, _IOFBF, logfile_buffer, BUFSIZ);
#else
#if	UNICOS | OSF
	setvbuf (logfile, logfile_buffer, _IOFBF, BUFSIZ);
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
	fflush (logfile);		/* Logfile is a pipe */
	/*
	 *  Initialize the in-core copy of the parameters.
	 *  The NQS local daemon uses SIGINT to let us know
	 *  when the parameters have changed.  Reload() assumes
	 *  that the variable "Paramfile" has been set.
	 */
	if ((Paramfile = opendb (Nqs_params, O_RDONLY)) == NULL) {
		fprintf (logfile, "F$Netdaemon: unable to open general ");
		fprintf (logfile, "parameters file.\n");
		net_abort();		/* Abort execution */
	}
	signal (SIGINT, reload);
	ldparam ();
	/*
	 *  Disable certain signals.
	 */
	signal (SIGHUP, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGPIPE, SIG_IGN);
#if	BSD42 | BSD43 | ULTRIX
	signal (SIGCHLD, reapchild);
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	signal (SIGUSR1, SIG_IGN);
	signal (SIGUSR2, SIG_IGN);
#if OSF
#ifdef SDSC 
        signal (SIGCHLD, reapchild);
#else
	signal (SIGCHLD, SIG_IGN);	/* Exiting children will disappear */
					/* without us having to wait.  This */
					/* is really quite disgusting, but */
					/* is an effective means of doing */
					/* what we want.... */
#endif 
#else
        signal (SIGCLD, SIG_IGN);       /* Exiting children will disappear */
                                        /* without us having to wait.  This */
                                        /* is really quite disgusting, but */
                                        /* is an effective means of doing */
                                        /* what we want.... */
#endif
#else
BAD SYSTEM TYPE
#endif
#endif
	signal (SIGTERM, net_daeshutdown); /* Shutdown on receipt of SIGTERM */
	/*
	 *  All files are to be created with the stated permissions.
	 *  No additional permission bits are to be turned off.
	 */
	umask (0);
	/*
	 *  Change directory to the NQS "root" directory.
	 */
	if (chdir (Nqs_root) == -1) {
		fprintf (logfile, "F$Netdaemon: Unable to chdir to %s.\n",
			Nqs_root);
		net_abort();		/* Abort execution */
	}
	/*
	 *  Determine the machine-id of the local host.
	 */
	if (localmid (&Locmid) != 0) {
		fprintf (logfile, "F$Netdaemon: unable to determine ");
		fprintf (logfile, "machine-id of local host.\n");
		net_abort();		/* Abort execution */
	}
	/*
	 *  Build the address that we will bind() to.
	 */
	servent = getservbyname ("nqs", (char *) 0);
	if (servent == (struct servent *) 0) {
		fprintf (logfile, "F$Netdaemon: Service [nqs] ");
		fprintf (logfile, "is unknown.\n");
		errno = 0;		/* Not a system call error */
		net_abort();		/* Abort execution */
	}
	bzero ((char *)&sin, sizeof (struct sockaddr_in));
	sin.sin_family = AF_INET;
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	sin.sin_addr.s_addr = INADDR_ANY;
#else
#if	UNICOS
	sin.sin_addr = INADDR_ANY;
#else
BAD SYSTEM TYPE
#endif
#endif
	sin.sin_port = servent->s_port;
	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		fprintf (logfile, "F$Netdaemon: Socket() error.\n");
		net_abort();		/* Abort execution */
	}
	/*
	 *  Test to see that file descr #WRITE_FIFO is really open on the
	 *  local NQS daemon FIFO request pipe.  This partially tests whether
	 *  or not the NQS network daemon has been properly exec'ed from
	 *  the local NQS daemon.
	 */
	if (fstat (WRITE_FIFO, &stat_buf) == -1) {
		/*
		 *  Fstat can only fail by EBADF or EFAULT.
		 *  Since stat_buf is ok, the error is EBADF.
		 */
		fprintf (logfile, "F$Netdaemon: Improper invocation.\n");
		net_abort();		/* Abort execution */
	}
	/*
	 *  Inform inter() that file descr #WRITE_FIFO is connected to the
	 *  local NQS daemon.
	 */
	interset (WRITE_FIFO);
	/*
	 *  Prepare to listen for connections.
	 */
	if (bind (sd, (struct sockaddr_in *) &sin, sizeof (sin)) == -1) {
		fprintf (logfile, "F$Netdaemon: Bind() error.\n");
		fprintf (logfile, "I$Netdaemon: possible attempt to ");
		fprintf (logfile, "execute multiple Netdaemons.\n");
		net_abort();		/* Abort execution */
	}
	if (listen (sd, 5) == -1) {	/* Queue up to 5 connections */
		fprintf (logfile, "F$Netdaemon: Listen() error.\n");
		net_abort();		/* Abort execution */
	}
	from_addrlen = sizeof (from);
	/*
	 *  Loop forever handling NQS network requests, until a SIGTERM
	 *  shutdown signal is received from the local NQS daemon.
	 */
	for (;;) {
		new_socket = accept (sd, &from, &from_addrlen);
		if (getppid() <= 1) net_abort();
		if (new_socket == -1) {
			if (errno != EINTR) {
				fprintf (logfile, "F$Netdaemon: ");
				fprintf (logfile, "accept() error.\n");
				net_abort();	/* Abort execution */
			}
		}
		else {
			/*
			 *  Make sure to flush any diagnostic messages
			 *  PRIOR to the fork!
			 */
			fflush (logfile);
			switch (fork()) {
			case -1:
				/*
				 *  The fork() failed, due to a lack of
				 *  available processes.
				 */
				close (new_socket);	/* Break connection */
				fprintf (logfile, "W$Netdaemon: fork ");
				fprintf (logfile, "failed.\n");
				fprintf (logfile, "I$Netdaemon: net ");
				fprintf (logfile, "request aborted.\n");
				fflush (logfile);
				break;
			case 0:
				/*
				 *  The fork() succeeded.  We are the
				 *  child server process.
				 *
				 *  Close the original socket before handling
				 *  the new request.
				 *
				 *  System V: continue to ignore SIGCLD.
				 *  Berkeley: begin to ignore SIGCHLD.
				 */
				signal (SIGTERM, SIG_DFL);
				signal (SIGINT, SIG_IGN);
#if	BSD42 | BSD43 | ULTRIX
				signal (SIGCHLD, SIG_IGN);
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
				close (sd);
				closedb (Paramfile);	/* Child opens fresh */
							/* copy */
				pre_server (new_socket, &from);
				break;
			default:
				/*
				 *  The fork() succeeded.  We are the
				 *  NQS network daemon.  Close new socket
				 *  and loop to handle another request.
				 */
				close (new_socket);
				break;
			}
		}
	}
}


/*** pre_server
 *
 *
 *	void pre_server():
 *
 *	Handle NQS network requests from a remote NQS client process.
 *	We are a server process for an NQS network client. If there
 *	is a lot of work to do, we will exec the netserver code.
 *
 *	We are running with the effective and real user-id of root.
 */
static void pre_server (newsd, from)
int newsd;				/* Socket-descr */
struct sockaddr_in *from;		/* Address of the client */
{
	void badpacket();		/* Display bad packet message */
	void delreqserver();		/* Delete a request */
	void execnetserver();		/* Exec the netserver */
	void qstatserver();		/* Report on this machine's queues */
#if	BSD42 | BSD43 | ULTRIX
	void relaypackets();		/* Relay message packets */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
	void sendandabort();		/* Send TCM and abort this server */
	void server_abort();		/* Server aborting execution */
	void showbytes();		/* Show array of bytes */

	/*
	 *  Local variables.
	 *  ----------------
	 */

#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS | OSF
#else
#if	UNICOS
	unsigned long castableaddr;	/* UNICOS is wierd */
#else
BAD SYSTEM TYPE
#endif
#endif
	struct hostent *chostent;	/* Client hostent using OUR file */
	mid_t cmid;			/* Client machine-id using OUR nmap */
	struct passwd *pw;		/* Passwd entry */
	int integers;			/* Number of 32-bit precision signed-*/
					/* integer values present in the */
					/* received message packet */
	int strings;			/* Number of character/byte strings */
					/* in the received message packet */
	int i;				/* Loop index var */
	
	/*
	 *  Set the name of this process to a more appropriate string.
	 *  We use sprintf() to pad with spaces rather than null
	 *  characters so that disgusting programs like ps(1) which
	 *  walk the stack will not get confused.
	 */
	sprintf (Argv0, "%-*s", Argv0size, "NQS netserver");
	if (Debug > 2) {
		fprintf (logfile, "D$Netdaemon: born by fork.\n");
		fflush (logfile);
	}
	/*
	 *  True NQS clients always bind to a port < IPPORT_RESERVED.
	 */
	if (htons ((u_short) from->sin_port) >= IPPORT_RESERVED) {
		fprintf (logfile, "W$Netdaemon: Client on non-secure port ");
		fprintf (logfile, "(client port >= %1d).\n", IPPORT_RESERVED);
		sendandabort (newsd, TCMP_NONSECPORT);
	}
	/*
	 *  Determine the machine-id of the client.
	 */
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	chostent = gethostbyaddr ((char *) &from->sin_addr,
				  sizeof (struct in_addr), AF_INET);
#else
#if	UNICOS
	castableaddr = (unsigned long) from->sin_addr;
	chostent = gethostbyaddr ((char *) &castableaddr,
				  sizeof (struct in_addr), AF_INET);
#else
BAD SYSTEM TYPE
#endif
#endif
	if (chostent == (struct hostent *) 0) {
		/*
		 *  The client host address is not recognized on this
		 *  system!
		 */
		fprintf (logfile,"W$Netdaemon: client internet addr unknown ");
		fprintf (logfile, "to local host.\n");
		sendandabort (newsd, TCMP_NETDBERR);
	}
	switch (nmap_get_mid (chostent, &cmid)) {
	case NMAP_ENOMAP:
	case NMAP_ENOPRIV:
	case NMAP_EUNEXPECT:
		fprintf (logfile,"W$Netdaemon: client hostname unknown ");
		fprintf (logfile, "to local host.\n");
		sendandabort (newsd, TCMP_CLIMIDUNKN);
	case NMAP_SUCCESS:
		break;
	}
	/*
	 *  Perform NQS connection protocol preamble.
	 */
	setsockfd (newsd);		/* Teach getsockch() */
	switch (interread (getsockch)) {
	case 0: if (Debug > 2) {
			fprintf (logfile, "D$Netdaemon: got 1st packet.\n");
			fflush (logfile);
		}
		break;
	case -1:
		/*
		 *  The remote client has closed the connection.
		 *  This network request session is over.
		 */
		if (Debug > 2) {
			fprintf (logfile, "D$Netdaemon: session ");
			fprintf (logfile, "concluded, exiting.\n");
			fflush (logfile);
		}
		exit (0);
	case -2:			/* Bad packet received */
					/* Abort execution */
		fprintf (logfile, "W$Netdaemon received bad packet.\n");
		sendandabort (newsd, TCMP_PROTOFAIL);
	}
	/*
	 *  Otherwise, the packet is self-consistent.
	 *  We have a request to process.
	 */
	integers = intern32i();		/* Get number of 32-bit precision */
					/* integers in message packet */
	strings = internstr();		/* Get number of strings in packet */
#if	BSD42 | BSD43
	if (integers == 1 && strings == 0) {
		/*
		 *  The first message packet contains only a single integer.
		 *  The message packet must therefore be an NPK_SERVERCONN
		 *  packet, requesting the creation of a message packet relay
		 *  process.  NPK_SERVERCONN packets can ONLY come from
		 *  processes on the SAME machine.
		 */
		if (Locmid != cmid) {
			/*
			 *  Client process is not local.
			 */
			fprintf (logfile, "W$Netdaemon: NPK_SERVERCONN ");
			fprintf (logfile, "received from remote client.\n");
			sendandabort (newsd, TCMP_PROTOFAIL);
		}
		if (interr32i (1) != NPK_SERVERCONN) {
			/*
			 *  Bad packet type or protocol error.
			 */
			sendandabort (newsd, TCMP_PROTOFAIL);
		}
		/*
		 *  The client process is running on the local machine.
		 *  Become a local message packet relay process to the
		 *  local NQS daemon.
		 */
		relaypackets ();	/* Relay message packets to the */
	}				/* local NQS daemon */
#else
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  The message packet is of the ordinary variety, originating from
	 *  a remote client machine.
	 */
	if (integers < 6 || strings < 3) {
		/*
		 *  The first message packet at a minimum must contain
		 *  these integers and strings.
		 */
		fprintf (logfile, "I$Netdaemon received message ");
		fprintf (logfile, "packet lacking appended info.\n");
		sendandabort (newsd, TCMP_PROTOFAIL);
	}
	integers -= 6;			/* Focus on non-appended integers */
	strings -= 3;			/* Focus on non-appended strings */
	if (interr32u (integers + 1) != NPK_MAGIC1) {
		fprintf (logfile, "I$Netdaemon: client has bad magic.\n");
		sendandabort (newsd, TCMP_PROTOFAIL);
	}
	/* The 0 means map by username, not by nmap */
	if ((pw = mapuser (interr32i (integers + 5), interrstr (strings + 2),
		cmid, chostent->h_name, 0)) == (struct passwd *) 0) {
		fprintf (logfile, "I$Netdaemon: mapuser() denies access.\n");
		sendandabort (newsd, TCMP_NOACCAUTH);
	}
	if (interr32i (integers + 3) != cmid) {
		fprintf (logfile, "I$Netdaemon: client/local mid conflict.\n");
		sendandabort (newsd, TCMP_MIDCONFLCT);
	}
	if (interr32i (integers + 4) != Locmid) {
		fprintf (logfile, "I$Netdaemon: local/client mid conflict.\n");
		sendandabort (newsd, TCMP_MIDCONFLCT);
	}
	/*
	 *  Reopen the general parameters file.  In the future, we
	 *  will check the NQS network password for the client machine.
	 *  This password will be stored in the corresponding network
	 *  peer record stored in the parameters file.
	 *
	 *  The NQS parameters file must also be opened for the benefit
	 *  of execnetserver() (see below).
	 */
	if ((Paramfile = opendb (Nqs_params, O_RDONLY)) == NULL) {
		fprintf (logfile, "I$Netdaemon: unable to open general ");
		fprintf (logfile, "parameters file.\n");
		sendandabort (newsd, TCMP_INTERNERR);
	}
	/*
	 *  The packet has been minimally authenticated.
	 */
	if (Debug > 1) {
		fprintf (logfile, "D$Netdaemon: Received packet ");
		fprintf (logfile, "from remote client.\n");
		fprintf (logfile, "D$Netdaemon: Packet type is: %1ld\n",
			interr32i (integers + 2));
		fflush (logfile);
		if (integers != 0 || strings != 0) {
			fprintf (logfile, "D$Netdaemon: Packet contents ");
			fprintf (logfile, "are as follows:\n");
			for (i = 1; i <= strings; i++) {
				fprintf (logfile, "D$Netdaemon: Str [%1d] = ",
					 i);
				showbytes (interrstr (i), interlstr (i));
				fflush (logfile);
			}
			for (i = 1; i <= integers; i++) {
				fprintf (logfile, "D$Netdaemon: Integer ");
				fprintf (logfile, "[%1d] = %1ld\n", i,
					 interr32i (i));
				fflush (logfile);
			}
		}
	}
	/*
	 *  Based upon the network packet type, we perform a case
	 *  statement.  Each case exits.  Note that we ignore SIGTERM
	 *  beyond this point.
	 */
	signal (SIGTERM, SIG_IGN);
	switch ((int) interr32i (integers + 2)) {
	/*
	 *  Batch requests and device requests.
	 */
	case NPK_QUEREQ:
		if (strings == 0 && integers == 0) {
			execnetserver (newsd, pw, cmid, "NPK_QUEREQ");
		}
		else badpacket ();
		break;
	case NPK_DELREQ:
		if (strings == 1 && integers == 5 ) {
			delreqserver (newsd, pw, cmid, chostent->h_name,
				interr32i (1), interrstr (1), interr32i (2),
				interr32i (3), interr32i (4), interr32i (5));
		}
		else badpacket ();
		break;
	case NPK_COMMIT:
		/*
		 * NPK_COMMIT should never be used to start a conversation.
		 */
		badpacket ();
		break;
	/*
	 * File system work.
	 */
	case NPK_DONE:
		/*
		 * NPK_DONE should never be used to start a conversation.
		 */
		badpacket ();
		break;
	case NPK_MKDIR:
		if (strings == 0 && integers == 0 ) {
			badpacket ();
		}
		else badpacket ();
		break;
	case NPK_REQFILE:
		if (strings == 0 && integers == 0 ) {
			execnetserver (newsd, pw, cmid, "NPK_REQFILE");
		}
		else badpacket ();
		break;
	case NPK_CPINFILE:
	case NPK_CPOUTFILE:
	case NPK_MVINFILE:
		if (strings == 0 && integers == 0 ) {
			badpacket ();
		}
		else badpacket ();
		break;
	case NPK_MVOUTFILE:
		if (integers == 0 && strings == 0) {
			execnetserver (newsd, pw, cmid, "NPK_MVOUTFILE");
		}
		else badpacket ();
		break;
	/*
	 * Status
	 */
	case NPK_QDEV:
	case NPK_QLIMIT:
	case NPK_QMGR:
		if (strings == 0 && integers == 0 ) {
			badpacket ();
		}
		else badpacket ();
		break;
	case NPK_QSTAT:
		if (integers == 2 && strings == 2) {
			qstatserver (newsd, cmid, chostent->h_name,
				interr32i (1), interr32i (2), interrstr (1),
				interrstr (2));
		}
		else badpacket ();
		break;
	default:
		badpacket ();
		break;
	}
	exit (0);			/* This line not reached */
}


/*** execnetserver
 *
 *
 *	void execnetserver():
 *
 */
static void execnetserver (newsd, pw, cmid, op)
int newsd;				/* Socket descriptor */
struct passwd *pw;			/* Whom we are acting on behalf of */
mid_t cmid;				/* The client's machine id */
char *op;				/* The operation */
{
	void sendandabort();		/* Send a TCM to client and abort */
	
	struct gendescr *descr;		/* NQS database information */
	char *argv [MAX_SERVERARGS + 1];/* Construct argv for netserver here */
	char environment [MAX_ENVIRONMENT];
	int envpsize;			/* First free byte in environment */
	char *envp [MAX_ENVIRONVARS +1];/* Construct env for netserver here */
	int envpcount;			/* First free slot in envp */
	int i;				/* Loop variable */

	/*
	 * Lose our privileges.
	 */
#if UNICOS & NEWPWD
	if (acctid (0,pw->pw_acid[0]) == -1) {
		/*
		 *	Invalid account ID
		 */
		fprintf (logfile, "Invalid account ID\n");
		sendandabort (newsd, TCMP_NOACCAUTH);
	}
#endif
	setgid (pw->pw_gid);
	setuid (pw->pw_uid);
	seekdb (Paramfile, Udbnetprocs);
	descr = nextdb (Paramfile);
	if (descr->v.par.paramtype != PRM_NETPROCS) {
		fprintf (logfile, "I$Netdaemon: Bad parameters file.\n");
		sendandabort (newsd, TCMP_INTERNERR);
	}
	/*
	 *  Enforce file descriptor associations:
	 *
	 *	0: new socket;
	 *	1,2,4: Log process pipe;
	 *	WRITE_FIFO: Local daemon FIFO;
	 *	remaining: closed
	 */
	if (newsd != 0) {
		close (0);
		fcntl (newsd, F_DUPFD, 0);
		close (newsd);
	}
#if	BSD42 | BSD43 | ULTRIX
	i = getdtablesize();		/* #of file descrs per proc */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	i = _NFILE;			/* #of file descrs per proc */
#else
BAD SYSTEM TYPE
#endif
#endif
	while (--i >= 5) close (i);
	if (parseserv (descr->v.par.v.netprocs.netserver,
			argv) == -1) {
		fprintf (logfile, "I$Netdaemon: Parse ");
		fprintf (logfile, "of server path and args failed.\n");
		sendandabort (newsd, TCMP_INTERNERR);
	}
	envpcount = 0;
	envpsize = 0;
	envp [envpcount++] = environment;
	sprintf (environment, "DEBUG=%1d", Debug);
	envpsize += strlen (environment) + 1;
	envp [envpcount++] = environment + envpsize;
	sprintf (environment + envpsize, "OP=%s", op);
	envpsize += strlen (environment + envpsize) + 1;
	envp [envpcount++] = environment + envpsize;
	sprintf (environment + envpsize, "CLIENTMID=%d", cmid);
	envpsize += strlen (environment + envpsize) + 1;
	/*
	 * Mark the end of the array of character pointers.
	 */
	envp [envpcount] = (char *) 0;
	/* MOREHERE: set up a SIGPIPE handler in the exec'd process */
	fflush (logfile);
	execve (argv [0], argv, envp);
	fprintf (logfile, "I$Netdaemon: Exec error.\n");
	sendandabort (newsd, TCMP_INTERNERR);
}


/*** delreqserver
 *
 *
 *	void delreqserver():
 *
 */
static void
delreqserver (sd, pw, cmid, chostname, whomuid, whomname, orig_seqno,
	      orig_mid, sig, states)
int sd;				/* Socket connection */
struct passwd *pw;		/* Password entry on this machine */
mid_t cmid;			/* Our guess as to their mid */
char *chostname;		/* Our guess as to their principal name */
uid_t whomuid;			/* The request belongs to this user */
char *whomname;			/* The request belongs to this user */
long orig_seqno;		/* Original sequence number */
mid_t orig_mid;			/* Original machine id */
int sig;			/* Which signal to send */
int states;			/* Only proceed if in these states */
{
	void sendandabort();

	char packet [MAX_PACKET];	/* To hold our reply */
	int packetsize;			/* Bytes in our reply */
	struct passwd *whompw;		/* Whose request it is */
	long tcm;			/* Transaction completion code */
	
	if (Debug > 2) {
		fprintf (logfile, "D$Netdaemon: NPK_DELREQ.\n");
		fflush (logfile);
	}
	/*
	 * Lose our privileges.
	 */
#if UNICOS & NEWPWD
	if (acctid (0,pw->pw_acid[0]) == -1) {
		/*
		 *	Invalid account ID
		 */
		fprintf (logfile, "Invalid account ID\n");
		sendandabort (sd, TCMP_NOACCAUTH);
	}
#endif
	setgid (pw->pw_gid);
	setuid (pw->pw_uid);
	/*
	 *  The 0 means we are mapping by username.
	 */
	whompw = mapuser (whomuid, whomname, cmid, chostname, 0);
	if (whompw == (struct passwd *) 0) {
		sendandabort (sd, TCMP_NOACCAUTH);
	}
	tcm = delreq (whompw->pw_uid, orig_seqno, orig_mid, sig, states);
	
	interclear ();
	interw32i (tcm);
	if ((packetsize = interfmt (packet)) == -1) {
		fprintf (logfile, "I$Netdaemon: Packet ");
		fprintf (logfile, "would be too large.\n");
		errno = 0;		/* Not a system call error */
		server_abort ();	/* Exit nicely */
	}
	write (sd, packet, packetsize);	/* Write response */
	exit (0);
}


/*** qstatserver
 *
 *
 *	void qstatserver():
 *
 */
static void
qstatserver (sd, cmid, chostname, flags, whomuid, queuename, whomname)
int sd;				/* Socket connection */
mid_t cmid;			/* Our guess as to their mid */
char *chostname;		/* Our guess as to their principal name */
int flags;			/* Display flags */
int whomuid;			/* -u mapped uid */
char *queuename;		/* If 0 length, then show all queues */
char *whomname;			/* -u mapped name */
{
	void outanderrto();
	void sendandabort();
	
	char packet [MAX_PACKET];	/* To hold our reply */
	int packetsize;			/* Bytes in our reply */
	struct passwd *whompw;		/* Whom for qstat */
	
	if (Debug > 2) {
		fprintf (logfile, "D$Netdaemon: NPK_QSTAT.\n");
		fflush (logfile);
	}
	/*
	 *  Open the non-network queue descriptor file.
	 */
	if ((Queuefile = opendb (Nqs_queues, O_RDONLY)) == NULL) {
		fprintf (logfile, "I$Netdaemon: unable to open Queuefile.\n");
		sendandabort (sd, TCMP_INTERNERR);
	}
	if ((Qcomplexfile = opendb (Nqs_qcomplex, O_RDONLY)) == NULL) {
		fprintf(logfile, "I$Netdaemon: unable to open Qcomplexfile.\n");
		sendandabort (sd, TCMP_INTERNERR);
	}
	if ((Qmapfile = opendb (Nqs_qmaps, O_RDONLY)) == NULL) {
		fprintf(logfile, "I$Netdaemon: unable to open Qmapfile.\n");
		sendandabort (sd, TCMP_INTERNERR);
	}
	if ((Pipeqfile = opendb (Nqs_pipeto, O_RDONLY)) == NULL) {
		fprintf(logfile, "I$Netdaemon: unable to open Pipeqfile.\n");
		sendandabort (sd, TCMP_INTERNERR);
	}
#if UNICOS
	/*
	 *  Read job table from kernel memory.
	 */
	read_j_table();
#endif


	/*
	 *  The 0 means we are mapping by username.
	 */
	whompw = mapuser (whomuid, whomname, cmid, chostname, 0);
	if (whompw == (struct passwd *) 0) {
		sendandabort (sd, TCMP_NOACCAUTH);
	}
	interclear ();
	interw32i (TCMP_CONTINUE);
	if ((packetsize = interfmt (packet)) == -1) {
		fprintf (logfile, "I$Netdaemon: Packet ");
		fprintf (logfile, "would be too large.\n");
		errno = 0;		/* Not a system call error */
		server_abort ();	/* Exit nicely */
	}
	write (sd, packet, packetsize);	/* Write response */
	/*
	 *  Cause file descriptors 1 and 2 to point to the socket.
	 */
	outanderrto (sd);


        /*
         *      This is kludge to allow old NQS versions to network
         *      to this machine.  We can tell as an old style NQS
         *      will not have SHO_BATCH SHO_DEVICE SHO_PIPE
         */
        if ( (!(flags & SHO_PIPE )) && (!(flags & SHO_BATCH)) &&
                (!(flags & SHO_DEVICE)))
                flags |= (SHO_PIPE | SHO_BATCH | SHO_DEVICE);


	/*
	 *  To deny a user the right to learn about another user's
	 *  requests, add code in shoqbyname.c or qstat.c, not here.
	 */
	if (queuename [0] == '\0') {
		list(Queuefile,Qcomplexfile,NULL,flags,whomname,Qmapfile,Pipeqfile);
		/* CERN Boissaat */
	}
	else {
		list(Queuefile,Qcomplexfile,queuename,flags,whomname,Qmapfile,Pipeqfile);
	}
	exit (0);
}


#if	BSD42 | BSD43
/*** relaypackets
 *
 *
 *	void relaypackets():
 *
 *	The network server process is a "relay message packet" server,
 *	passing message packets from client processes to the local NQS
 *	daemon.
 *
 *	This function exists because Berkeley style UNIX does not support
 *	named-pipes.  This little function, and the additional network
 *	packet of: NPK_SERVERCONN, allow us to get by on Berkeley style UNIX
 *	systems.
 */
static void relaypackets ()
{
	char packet [MAX_PACKET];	/* Relay packet */
	register int n_integers;	/* Number of integers in packet */
	register int n_strings;		/* Number of strings in packet */
	register int i;

	while (1) {
		/*
		 *  Loop until EOF from client.
		 */
		switch (interread (getsockch)) {
		case 0:	/*
			 *  A message packet has been received from the
			 *  local client process.  Forward the message
			 *  packet in a single chunk to the local NQS
			 *  daemon.
			 */
			interclear();	/* Clear outgoing message packet */
			n_integers = intern32i();
			n_strings = internstr();
			for (i = 1; i <= n_integers; i++) {
				/*
				 *  Forward the integer data.
				 */
				if (interr32sign (i)) {
					interw32i (interr32i (i));
				}
				else interw32u (interr32u (i));
			}
			for (i = 1; i <= n_strings; i++) {
				/*
				 *  Forward the byte/string data.
				 */
				interwbytes (interrstr (i),
					    (unsigned) interlstr (i));
			}
			i = interfmt (packet);	/* Format message packet */
			/*
			 *  Now, send the message packet to the local
			 *  NQS daemon.
			 */
			if (write (WRITE_FIFO, packet, i) != i) {
				fprintf (logfile, "W$Netdaemon: Relay ");
				fprintf (logfile, "packet write error.\n");
				server_abort ();	/* Abort */
			}
			break;
		case -1:
			/*
			 *  The local client process has closed the
			 *  connection, or has exited.
			 */
			exit (0);
		case -2:
			/*
			 *  A bad message packet was received from the
			 *  local client process.
			 */
			fprintf (logfile, "E$Netdaemon: Bad message packet ");
			fprintf (logfile, "received.\n");
			errno = 0;		/* Not a system call error */
			server_abort();		/* Abort */
		}
	}
}
#else
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif


/*** badpacket
 *
 *
 *	void badpacket():
 *
 *	Write a message to the NQS log process concerning the bad packet
 *	received.
 */
static void badpacket ()
{
	void server_abort();		/* Abort netserver execution */

	fprintf (logfile, "E$Netdaemon: bad network packet received.\n");
	errno = 0;			/* Not a system call error */
	server_abort ();		/* Abort execution */
}


/*** net_abort
 *
 *
 *	void net_abort():
 *
 *	Abort Network daemon execution, writing a message to
 *	the NQS log process.
 */
static void net_abort()
{
	if (errno) fprintf (logfile, "I$%s.\n", asciierrno());
	fprintf (logfile, "I$Netdaemon aborted.\n");

#ifdef SDSC
        (void)syslog(LOG_CRIT,
        "NQS netdaemon aborted. (please see log files in /usr/spool/nqs/log.d directory)");
        (void)syslog(LOG_CRIT | LOG_DAEMON,
        "NQS netdaemon aborted. (please see log files in /usr/spool/nqs/log.d directory)");
        console_write(
        "NQS netdaemon aborted. (please see log files in /usr/spool/nqs/log.d directory) ");
#endif

	exit (1);
}


/*** net_daeshutdown
 *
 *
 *	void net_daeshutdown():
 *	Catch SIGTERM signal to shutdown.
 */
static void net_daeshutdown()
{
	signal (SIGTERM, SIG_IGN);
	fprintf (logfile, "I$Netdaemon shutting down.\n");

#ifdef SDSC
	(void)syslog(LOG_INFO,
	"NQS Netdaemon shutting down. (please see log files in /usr/spool/nqs/log.d directory)");
	(void)syslog(LOG_INFO | LOG_DAEMON,
	"NQS Netdaemon shutting down. (please see log files in /usr/spool/nqs/log.d directory");
#endif

	exit (0);
}


/*** outanderrto
 *
 *
 *	void outanderrto():
 *
 *	Force STDOUT and STDERR to be directed at the socket
 *	connection established between the remote client and
 *	this server.  The streams stdout and stderr stay open.
 */
static void outanderrto (sd)
int sd;						/* Socket descriptor */
{
	static char stdout_buffer [SOCKBUFSIZ];	/* Stdout output buffer */
	static char stderr_buffer [SOCKBUFSIZ];	/* Stderr output buffer */
	
	fflush (stdout);
	fflush (stderr);
	close (STDOUT);			/* Close original STDOUT */
	close (STDERR);			/* Close original STDERR */
	if (fcntl (sd, F_DUPFD, STDOUT) != STDOUT ) {
		fprintf (logfile, "I$Netdaemon cannot replicate ");
		fprintf (logfile, "socket descriptor.\n");
		sendandabort (sd, errnototcm ());
	}
	if (fcntl (sd, F_DUPFD, STDERR) != STDERR) {
		fprintf (logfile, "I$Netdaemon cannot replicate ");
		fprintf (logfile, "socket descriptor.\n");
		sendandabort (sd, errnototcm ());
	}
	/*
	 *  Make sure that formatted output returned to the remote
	 *  client process is transmitted in blocks for greater
	 *  efficiency.
	 */
#if	BSD42 | BSD43 | ULTRIX
	setbuffer (stdout, stdout_buffer, SOCKBUFSIZ);
#else
#if	SGI | SYS52 | UTS
	setvbuf (stdout, _IOFBF, stdout_buffer, SOCKBUFSIZ);
#else
#if	UNICOS | OSF
	setvbuf (stdout, stdout_buffer, _IOFBF, SOCKBUFSIZ);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#if	BSD42 | BSD43 | ULTRIX
	setbuffer (stderr, stderr_buffer, SOCKBUFSIZ);
#else
#if	SGI | SYS52 | UTS
	setvbuf (stderr, _IOFBF, stderr_buffer, SOCKBUFSIZ);
#else
#if	UNICOS | OSF
	setvbuf (stderr, stderr_buffer, _IOFBF, SOCKBUFSIZ);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
}


#if	BSD42 | BSD43 | ULTRIX| OSF
/*** reapchild
 *
 *
 *	void reapchild ():	
 *	SIGCHLD handler.
 *
 */
static void reapchild ()
{
#if OSF
	int status;
#else
	union wait status;
#endif

	/*
	 * Don't need to reinstate Berkeley signal handlers.
	 */
	while (wait3 (&status, WNOHANG, (struct rusage *) 0) > 0) {
	}
}
#else
#if	UNICOS | SGI | SYS52 | UTS
#else
BAD SYSTEM TYPE
#endif
#endif


/*** reload
 *
 *
 *	void reload ():	
 *	When the local daemon updates a parameter,
 *	it will send a SIGINT to the net daemon.  The net daemon
 *	should then update its copy of the parameters off the disk.
 *
 */
static void reload ()
{
	signal (SIGINT, reload);
	seekdb (Paramfile, 0);
	ldparam ();
}


/*** sendandabort
 *
 *
 *	void sendandabort ():
 *
 *	Abort Network server execution, sending a TCM code
 *	to the client and writing a message to the NQS log.
 */
static void sendandabort (sd, tcm)
int sd;					/* Socket descriptor */
long tcm;				/* What to send */
{
	char packet [MAX_PACKET];
	int packetsize;
	
	interclear ();
	interw32i (tcm);
	if ((packetsize = interfmt (packet)) == -1) {
		fprintf (logfile, "I$Netdaemon: Packet ");
		fprintf (logfile, "would be too large.\n");
		errno = 0;		/* Not a system call error */
		server_abort ();	/* Exit nicely */
	}
	write (sd, packet, packetsize);	/* Write response */
	errno = 0;			/* Not a system call error */
	server_abort ();
}


/*** server_abort
 *
 *
 *	void server_abort():
 *
 *	Abort Network server execution, writing a message to
 *	the NQS log process.
 */
static void server_abort ()
{
	if (errno) fprintf (logfile, "I$%s.\n", asciierrno());
	fprintf (logfile, "I$Netdaemon aborting.\n");
	exit (1);
}


/*** showbytes
 *
 *
 *	void showbytes():
 *	Show byte/string datum as received in message packet.
 */
static void showbytes (bytes, length)
register char *bytes;			/* Array of bytes. */
register unsigned length;		/* Length of byte array. */
{
	register short ch;		/* Byte/character */
	register int col;		/* Column counter */
	short truncate_flag;		/* String truncated flag */

	col = 0;			/* # of columns written */
	truncate_flag = 0;
	if (length > 300) {
		truncate_flag = 1;	/* Truncate to 300 chars on */
		length = 300;		/* display */
	}
	while (length--) {
		if (col >= 24) {
			fputc ('\n', logfile);
			fprintf (logfile, "D$Netdaemon: ");
			col = 0;
		}
		ch = (*bytes & 0377);	/* Mask to 8 bits */
		bytes++;		/* Increment to next byte */
		if (ch < ' ' || ch > 127) {
			/*
			 *  This character is unprintable, or is a tab
			 *  character.
			 */
			fprintf (logfile, "[\\%03o]", ch);
			col += 6;
		}
		else {
			fputc (ch, logfile);
			col += 1;
		}
	}
	fputc ('\n', logfile);
	if (truncate_flag) {
		/*
		 *  String truncated.
		 */
		fprintf (logfile, "D$Netdaemon: String truncated.\n");
	}
}


/*** validstr
 *
 *
 *	int validstr():
 *	Validate string.
 *
 *	Returns:
 *		1: if the specified string parameter in the received
 *		   message packet is not null, and is of length less
 *		   than, or equal to size_limit.
 *		0: otherwise.
 */
static int validstr (str_n, size_limit)
register int str_n;			/* Message packet string [n] */
register int size_limit;		/* Maximum size limit */
{
	register int size;

	size = interlstr (str_n);
	if (size && size <= size_limit) return (1);
	return (0);
}
#else
BAD NETWORK TYPE
#endif
#else
/*** main
 *
 *
 *	If networking is not supported, then the NQS network daemon
 *	is reduced to the following stub.
 */
main ()
{
	printf ("E$Networking not supported in this version of NQS.\n");
	printf ("I$Network daemon exiting.\n");
	exit (0);
}
#endif


