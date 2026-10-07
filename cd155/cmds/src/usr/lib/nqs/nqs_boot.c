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
 
/*++ nqs_boot.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_boot.c,v $
 *
 * DESCRIPTION:
 *
 *	This module handles the booting/initialization of NQS.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	June 19, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.8 $ $Date: 1995/01/24 01:10:52 $ $State: Exp $)
 * $Log: nqs_boot.c,v $
 * Revision 1.8  1995/01/24  01:10:52  davidl
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
 * Revision 1.7  1994/11/19  02:52:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/10/07  00:46:39  kremenek
 *  Reviewer:  davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 5706
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/lib/nqs/netdaemon.c, nqs_abort.c,
 *             nqs_boot.c, nqs_mai.c, nqs_main.c
 *
 * Revision 1.5  1993/11/02  00:57:09  mwan
 * R1.2 mods
 *
 * Revision 1.4  1993/07/13  17:51:05  mwan
 * T11 - fixed PTS 5022
 *
 * Revision 1.3  1993/05/19  18:03:54  mwan
 * Before T10 freeze. Fixed several PTS
 *
 * Revision 1.2  1992/10/09  22:24:59  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:50  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:04:41  hender
 * Sterling version 4/22/87
 * 
 * Added O_SYNC on all opendb as per Intergraph Bill Mar	TAC
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_boot.c	50.3 (nqs_boot.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include <signal.h>
#include "nqs.h"			/* Include NQS constants/types	*/
#include "nqspacket.h"			/* NQS local message packet types */
#include "nqsxvars.h"			/* NQS global variables and */
					/* directories */
#include "transactcc.h"			/* NQS transaction completion codes */

#ifdef SDSC
#include <syslog.h>
#include "buddyxvar.h"
#endif

/*
 *	UNIX external definitions:
 *
 *	Defined in signal.h:	extern int (* signal ())();
 */
extern int errno;			/* System call error# */
extern char *sys_errlist[];

/*
 *	External functions:
 */
extern void bufstderr();		/* Block buffer stderr */
extern void bufstdout();		/* Block buffer stdout */
extern void closepwdb();		/* Close account/password database */
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *getenv();			/* Get environment variable */
#else
#if	BSD42 | BSD43 | ULTRIX
#else
BAD SYSTEM TYPE
#endif
#endif
extern gid_t getegid();			/* Get effective group-id of process */
extern gid_t geteuid();			/* Get effective user-id of process */
extern long inter();			/* Send message to local daemon */
extern void interclear();		/* Clear packet contents */
extern void interset();			/* Set the file-descriptor used to */
					/* write packets to the local NQS */
					/* daemon. */
extern int localmid();			/* Get local machine-id */
extern void nqs_abort();		/* Abort NQS execution */
extern void nqs_ldconf();		/* Load NQS configuration */
extern void nqs_rbuild();		/* Rebuild NQS state */
extern void nqs_valarm();		/* Catch SIGALRM signal for virtual */
					/* timer (see nqs_vtimer.c) AND */
					/* enable virtual timers. */
extern struct confd *opendb();		/* Open database file */
extern int parseserv();			/* Parse an NQS server command */

#ifdef SDSC
extern void init_que ();
extern int read_param ();
extern int init_rootp ();
extern void set_time ();
extern int smd_cancel_all ();
extern void prt_param ();
#endif

#if	BSD42 | BSD43 | ULTRIX
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern int putenv();			/* Add environment variable */
extern long timezone;			/* Number of seconds from GMT */
extern char *tzname [2];		/* Time zone name */
#else
BAD SYSTEM TYPE
#endif
#endif


/*** nqs_boot
 *
 *
 *	void nqs_boot():
 *	Boot/initialize NQS.
 */
void nqs_boot()
{
#if	BSD42 | BSD43 | ULTRIX
	char *new_envp [1];		/* SUbordinate daemons' environment */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	static char default_tz [16];	/* Default timezone environment var */
	char *new_envp [2];		/* Subordinate daemons' environment */
#else
BAD SYSTEM TYPE
#endif
#endif
	char *new_argv [MAX_SERVERARGS+1];
					/* Subordinate daemons' arguments */
	int pipefd1 [2];		/* Pipe file descriptors used to */
	int pipefd2 [2];		/* communicate with the NQS log */
					/* process. */
	int pid;			/* Pid of requester */
	int i;				/* Loop variable. */
	char pathname [MAX_PATHNAME + 1];	/* Holds path for execve() */
#ifdef SDSC
	int smd_sock;
#endif

	/*
	 *  Ensure that stdout and stderr are buffered.  This is quite
	 *  critical since messages must be sent in coherent chunks to
	 *  the NQS log process from multiple processes (NQS daemon and
	 *  servers).
	 *
	 *  Thou shalt NOT think to thyself, that you can get rid of stderr.
	 *  It is used in nqs_spawn.c, when stdout has been opened as the
	 *  device....
	 */
	bufstdout();
	bufstderr();
	/*
	 *  Disable certain signals for safety and paranoia sake.
	 */
	signal (SIGHUP, SIG_IGN);
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGPIPE, SIG_IGN);
#if	BSD42 | BSD43 | ULTRIX
	signal (SIGTERM, SIG_IGN);
#else
#if	UNICOS | SGI | SYS52 | UTS | OSF
	signal (SIGTERM, SIG_IGN);
	signal (SIGUSR1, SIG_IGN);
	signal (SIGUSR2, SIG_IGN);
#if OSF
	signal (SIGCHLD, SIG_DFL);	/* Quite rational paranoia when */
                                        /* dealing with the Cray-2 */
#else
        signal (SIGCLD, SIG_DFL);       /* Quite rational paranoia when */
                                        /* dealing with the Cray-2 */
#endif

#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  ALL SIGALRM signals are to be directed to the virtual timer
	 *  module.
	 */
	signal (SIGALRM, nqs_valarm);	/* Virtual timer SIGALRM catcher */
					/* in nqs_vtimer.c */

	/*
	 *  NQS is considered to be BOOTING!
	 */
	Booted = 0;			/* NQS is not yet booted. */
					/* See nqs_rbuild.c and nqs_upd.c. */
	Shutdown = 0;			/* We are starting up! */

	if (geteuid() != 0 || getuid() != 0) {
		/*
		 *  The real AND effective user-id of NQS MUST be "root".
		 */
		printf ("NQS(FATAL): NQS not running as root.\n");
		errno = 0;			/* Not a system call error */
		nqs_abort();
	}
#if	SGI | SYS52 | UNICOS | UTS | OSF
	/*
	 *  System V based implementations of NQS require the presence
	 *  of the TZ environment variable.  This is used in
	 *  ../src/nqs_reqser.c and ../src/logdaemon.c.
	 */
	if (getenv ("TZ") == (char *)0) {
		/*
		 *  No TZ environment variable is present.
		 *  Make up a default TZ environment variable.
		 */
		sprintf (default_tz, "TZ=%-.3s%1d%-.3s", tzname [0],
			 timezone / 3600, tzname [1]);
	} else {
		sprintf (default_tz, "TZ=%s", getenv("TZ") );
	}
	new_envp[0] = default_tz;
	new_envp [1] = (char *) 0;
#else
#if	BSD42 | BSD43 | ULTRIX
	new_envp [0] = (char *) 0;
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Set real real-gid of this process to the effective group-id.
	 */
#if UNICOS & NEWPWD
	if (acctid (0,pw->pw_acid[0])== -1) {
		printf ("NQS(FATAL):Invalid account ID.\n");
		nqs_abort();
	}
#endif

#if OSF
        if (setgid ((gid_t) getegid ()) == -1) {
                printf ("NQS(FATAL): Unable to setgid().\n");
                nqs_abort();
        }
#else
	if (setgid ((int) getegid ()) == -1) {
		printf ("NQS(FATAL): Unable to setgid().\n");
		nqs_abort();
	}
#endif
	/*
	 *  All files are to be created with the stated permissions.
	 *  No additional permission bits are to be turned off.
	 */
	umask (0);
	/*
	 *  Create pipes to the NQS log process.
	 */
	if (pipe (pipefd1) == -1 || pipe (pipefd2) == -1) {
		printf ("NQS(FATAL): Unable to create pipe between ");
		printf ("NQS local daemon and NQS log daemon.\n");
		nqs_abort();
	}
	/*
	 *  Fork off the NQS message log daemon.
	 */
	fflush (stdout);		/* Flush any diagnostic messages */
	fflush (stderr);		/* prior to the fork() */
	if ((Logdaepid = fork()) == -1) {
		printf ("NQS(FATAL): Unable to fork NQS log process.\n");
		nqs_abort();
	}
	else if (Logdaepid == 0) {
		/*
		 *  We are the NQS log process.
		 *  We will use:
		 *
		 *	pipefd1 [0] to read logfile and command output
		 *		from other NQS processes.
		 *	pipefd2 [1] to write to the NQS local daemon
		 *		confirming a successful switch of log files.
		 *
		 *  Divorce ourselves from any tty.
		 */
#if		SGI | SYS52 | UTS | UNICOS
		setpgrp ();			/* Process group = pid */
#else
#if		BSD42 | BSD43 | ULTRIX
		setpgrp (0, getpid());		/* Process group = pid */
#else
#if		OSF
		if (0 != setpgid((pid_t)0, (pid_t)getpid())) {
		                printf ("NQS(FATAL): Unable to setpgid).\n");
                nqs_abort();
		}
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
		close (pipefd1 [1]);		/* Not needed */
		close (pipefd2 [0]);		/* Not needed */
		fcntl (STDOUT, F_SETFL,		/* Make sure stdout open */
		       O_APPEND);		/* for append. */
		/*
		 *  Make stdin read from the output pipe from the NQS
		 *  local daemon (and later from other processes).
		 */
		close (STDIN);
		fcntl (pipefd1 [0], F_DUPFD, STDIN);
		close (pipefd1 [0]);
		/*
		 *  Make stderr write back to the NQS local daemon.
		 */
		close (STDERR);
		fcntl (pipefd2 [1], F_DUPFD, STDERR);
		close (pipefd2 [1]);
		/*
		 *  Begin execution as the NQS log process.
		 *
		 *	Stdin reads messages from the NQS local daemon
		 *	and network daemon (and their associated server
		 *	processes).
		 *
		 *	Stdout writes to the original stdout of the
		 *	NQS local daemon, and is completely buffered.
		 *
		 *	Stderr writes back to the NQS local daemon in
		 *	response to certain NQS local daemon requests.
		 */
#ifndef	NQS_LIBEXE
The symbol:  NQS_LIBEXE  MUST be defined from the Makefile.
#else
		sprintf (pathname, "%s/logdaemon", NQS_LIBEXE);
#endif
		/*
		 *  Execve() logdaemon.
		 */
		new_argv [0] = "NQS logdaemon";
		new_argv [1] = (char *) 0;

#ifdef SDSC
                if ((-1) == execve (pathname, new_argv, new_envp))
                {
                        (void)syslog(LOG_CRIT,
                        "NQS start failed: %m (please see log files in /usr/spool/nqs/log.d directory).");
                        (void)syslog(LOG_CRIT | LOG_DAEMON,
                        "NQS start failed: %m (please see log files in /usr/spool/nqs/log.d directory).");
                        console_write(
                        "NQS start failed (please see log files in /usr/spool/nqs/log.d directory). ");
                }
#else
                execve (pathname, new_argv, new_envp);
#endif


		/*
		 * If the NQS logdaemon cannot be started, there is no
		 * point in letting the NQS local daemon boot.
		 */
		kill (getppid (), SIGKILL);
	}
	/*
	 *  We are the NQS local daemon.
	 *  We will use:
	 *
	 *	pipefd1 [1] to write output to the NQS log process,
	 *		    and to send commands to the NQS log
	 *		    process.
	 *	pipefd2 [0] to read NQS log process completion codes.
	 */
	close (pipefd1 [0]);			/* Unnecessary */
	close (pipefd2 [1]);			/* Unnecessary */
	close (STDOUT);				/* Make stdout write to */
	fcntl (pipefd1 [1], F_DUPFD, STDOUT);	/* the NQS log process. */
	close (STDERR);				/* Make stderr write to */
	fcntl (pipefd1 [1], F_DUPFD, STDERR);	/* the NQS log process. */
	close (pipefd1 [1]);
	Fromlog = pipefd2 [0];			/* Pipe from the NQS */
						/* log message process */
	if (Fromlog != 3) {
		/*
		 *  Force Fromlog to file-descriptor #3.
		 */
		close (3);
		fcntl (Fromlog, F_DUPFD, 3);
		Fromlog = 3;
	}
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
	fflush (stdout);			/* Stdout is a pipe */
	fflush (stderr);			/* Stderr is a pipe */
	errno = 0;				/* Clear errno */
	/*
	 *  At this point:
	 *
	 *	Stdout  (fd #1) writes (buffered) to the NQS log message
	 *			process.
	 *	Stderr  (fd #2) writes (buffered) to the NQS log message
	 *			process.
	 *	Fromlog (fd #3) reads completion messages from the NQS
	 *			log message process.
	 *
	 *
	 *  Set the name of this process to a more appropriate string.
	 *
	 *  We use sprintf() to pad with spaces rather than null
	 *  characters so that slimy programs like ps(1) which
	 *  walk the stack will not get confused.
	 */
	sprintf (Argv0, "%-*s", Argv0size, "NQS nqsdaemon");
	/*
	 *  Change directory to the NQS "root" directory.
	 */
	if (chdir (Nqs_root) == -1) {
		printf ("F$Unable to chdir() to %s.\n", Nqs_root);
		nqs_abort();
	}
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
	/*
	 *  Check for the possibility of multiple NQS local daemons running.
	 */
	interclear();			/* Clear message buffer */
	if (inter (PKT_SENSEDAEMON) != TCML_NOLOCALDAE) {
		/*
		 *  Egads!  An NQS local daemon is already running, or
		 *  is still in the process of shutting down.
		 *  Bail-out NOW!
		 */
		printf ("F$Another NQS local daemon is already running.\n");
		errno = 0;		/* Not a system call error */
		nqs_abort();
	}
#else
#if	BSD42 | BSD43
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Determine the machine-id of the local host.
	 *  This MUST be done before nqs_ldconf() is called!!!!
	 */
	if (localmid (&Locmid) != 0) {
		printf ("F$Unable to determine machine-id of local host.\n");
		nqs_abort();
	}
#if	SGI | SYS52 | UNICOS | ULTRIX | UTS | OSF
	/*
	 *  Create the named pipe that will serve as the "mailbox"
	 *  for NQS request packets.
	 */
	unlink (Nqs_ffifo);			/* Delete old pipe */
	if (mknod (Nqs_ffifo, 0010622) == -1) {
		printf ("F$Mknod() error for: %s.\n", Nqs_ffifo);
		nqs_abort ();
	}
	/*
	 *  We open the NQS mailbox named-pipe for reading AND
	 *  WRITING (on separate file descriptors) so that when no
	 *  other processes have the pipe open for writing, we do
	 *  not immediately return from the read() call with zero
	 *  bytes read.
	 *
	 *  Furthermore, the Write_fifo file descriptor is used
	 *  by:
	 *
	 *	1.  Request server "shepherd processes"
	 *	    to send request completion messages;
	 *
	 *	2.  Pipe queue server processes to send
	 *	    pipe queue destination status messages
	 *	    to the NQS local daemon;
	 *
	 *	3.  Server processes to send process group/
	 *	    family-ids;
	 *
	 *	4.  The NQS network daemon (if present).
	 *
	 *  The descriptor opened explicitly for writing is closed
	 *  when a shutdown request is received so that we can tell
	 *  when no other processes have the request pipe open (and
	 *  can hence know when it is safe to shutdown).
	 *
	 *  Note that when opening the FIFO for reading, we do so
	 *  with O_NDELAY set, so that we do not block forever.
	 */
	if ((Read_fifo = open (Nqs_ffifo, O_RDONLY | O_NDELAY)) == -1) {
		/*
		 *  Unable to open the FIFO request pipe.
		 */
		printf ("F$Error opening %s for reading.\n", Nqs_ffifo);
		nqs_abort();
	}
	
#if OSF
 	if (chmod(Nqs_ffifo , 0010622) == -1) {
          printf("I$Unable to chmod(%s,0010622) on the NQS request FIFO. (%s. errno=%d)\n",
                 Nqs_ffifo, sys_errlist[errno],  errno);
        }

#endif
	
	/*
	 *  Now, clear O_NDELAY on the read FIFO, so that we will block
	 *  waiting for data, or until no one else has the pipe open
	 *  for writing.
	 */
	fcntl (Read_fifo, F_SETFL, O_RDONLY);
#else
#if	BSD42 | BSD43
	/*
	 *  Berkeley based UNIX implementations do not support
	 *  named-pipes, and so we have to do things differently.
	 *
	 *  So much for the highly touted UNIX "portability"
	 *  properties.  Sigh....
	 *
	 *  Create the Read_fifo/Write_fifo packet request pipe.
	 */
	if (pipe (pipefd1) == -1) {
		printf("F$Unable to create read/write packet request pipe.\n");
		nqs_abort();
	}
	Read_fifo = pipefd1 [0];	/* Read FIFO is pipefd1 [0] */
	Write_fifo = pipefd1 [1];	/* Write FIFO is pipefd1 [1] */
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Make STDIN into the Read_fifo since we do not need
	 *  the original STDIN anymore, and it is logical to have
	 *  STDIN as the file descriptor reading the request pipe.
	 */
	close (STDIN);
	fcntl (Read_fifo, F_DUPFD, STDIN);	/* Make STDIN */
						/* refer to Read_fifo */
	close (Read_fifo);	/* Don't need original Read_fifo now */
	Read_fifo = STDIN;	/* Read_fifo is STDIN */
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
	if ((Write_fifo = open (Nqs_ffifo, O_WRONLY | O_APPEND)) == -1) {
		/*
		 *  Unable to open the FIFO request pipe.
		 */
		printf ("F$Error opening %s for writing.\n", Nqs_ffifo);
		nqs_abort ();
	}
#else
#if	BSD42 | BSD43
#else
BAD SYSTEM TYPE
#endif
#endif
	if (Write_fifo != 4) {
		/*
		 *  It is absolutely CRITICAL that Write_fifo be
		 *  opened on descriptor #4!!!  This fact is used in
		 *  ../src/nqs_spawn.c, ../src/nqs_reqser.c, and
		 *  ../src/nqs_reqexi.c.
		 */
		close (4);
		fcntl (Write_fifo, F_DUPFD, 4);
		close (Write_fifo);
		Write_fifo = 4;
	}
	interset (Write_fifo);	/* Inform ../lib/inter.c.... */
	/*
 	 *
	 *  At this point:
	 *
	 *	STDIN	   (fd #0) reads from the FIFO request pipe.
	 *	Stdout     (fd #1) writes (buffered) to the NQS log
	 *			   message process.
	 *	Stderr     (fd #2) writes (buffered) to the NQS log
	 *			   message process.
	 *	Fromlog    (fd #3) reads completion messages from
	 *			   the NQS log message process.
	 *	Write_fifo (fd #4) writes to the FIFO request pipe.
	 * 
	 *
	 *  Open the network queue descriptor file (and create it if it
	 *  does not already exist).
	 */
	if ((Netqueuefile = opendb (Nqs_netqueues, O_RDWR |O_CREAT | 
		O_SYNC)) == NULL) {
		/*
		 *  We could not open or create the network queue
		 *  descriptor file.
		 */
		printf ("F$Unable to create/open Netqueuefile.\n");
		nqs_abort();
	}
	/*
	 *  Open the queue complex descriptor file (and create it if it
	 *  does not already exist).
	 */
	if ((Qcomplexfile = opendb (Nqs_qcomplex, O_RDWR | O_CREAT |
		O_SYNC)) == NULL) {
		/*
		 *  We could not open or create the queue complex
		 *  descriptor file.
		 */
		printf ("F$Unable to create/open Qcomplexfile.\n");
		nqs_abort();
	}
	/*
	 *  Open the non-network queue descriptor file (and create it if it
	 *  does not already exist).
	 */
	if ((Queuefile = opendb (Nqs_queues, O_RDWR | O_CREAT | O_SYNC
		)) == NULL) {
		/*
		 *  We could not open or create the non-network queue
		 *  descriptor file.
		 */
		printf ("F$Unable to create/open Queuefile.\n");
		nqs_abort();
	}
	/*
	 *  Open the device descriptor file (and create it if it does not
	 *  already exist).
	 */
	if ((Devicefile = opendb (Nqs_devices, O_RDWR | O_CREAT | O_SYNC
		)) == NULL) {
		/*
		 *  We could not open or create the device descriptor file.
		 */
		printf ("F$Unable to create/open Devicefile.\n");
		nqs_abort();
	}
	/*
	 *  Open the queue/device/destination mapping descriptor file
	 *  (and create it if it does not already exist).
	 */
	if ((Qmapfile = opendb (Nqs_qmaps, O_RDWR | O_CREAT | O_SYNC
		)) == NULL) {
		/*
		 *  We could not open or create the queue/device/destination
		 *  mapping descriptor file.
		 */
		printf ("F$Unable to create/open Qmapfile.\n");
		nqs_abort();
	}
#if	BSD42 | BSD43 | ULTRIX
	/*
	 *  Berkeley UNIX based implementations do not have named pipes
	 *  (with the exception of ULTRIX), and so the mechanism used to
	 *  detect (and avoid) multiple NQS daemons, and to detect the
	 *  presence of a running NQS daemon (see ../lib/daepres.c) is
	 *  completely different....
	 *
	 *  Note that ULTRIX uses a hybrid of the approach used on System
	 *  V, and pure Berkeley based UNIX implementations....
	 *
	 *  If an advisory exclusive lock exists on the Qmapfile, then
	 *  another NQS daemon is already running, or is still running
	 *  and is in the process of shutting down.
	 */
	if (flock (Qmapfile->fd, LOCK_EX | LOCK_NB) == -1) {
		/*
		 *  Egads!  An NQS local daemon is already running, or
		 *  is still in the process of shutting down.
		 *  Bail-out NOW!
		 */
		printf ("F$Another NQS local daemon is already running.\n");
		errno = 0;		/* Not a system call error */
		nqs_abort();
	}
	/*
	 *  We were able to successfully apply an advisory exclusive lock
	 *  to the Qmapfile.  Therefore, we are the only local NQS daemon
	 *  running on the system, which is how things MUST be!
	 *
	 *  We must now apply an advisory exclusive lock on the Queuefile,
	 *  for the benefit of ../lib/daepres.c.
	 *
	 *  A local NQS daemon always has an advisory exclusive lock on
	 *  the Queuefile, while running.  This lock is released when the
	 *  local NQS system is first told to shutdown.  The lock on the
	 *  Qmapfile is retained however, until the NQS system is completely
	 *  shutdown.
	 */
	if (flock (Queuefile->fd, LOCK_EX | LOCK_NB) == -1) {
		/*
		 *  We should have gotten this lock....
		 */
		printf ("F$Unable to flock() Queuefile.\n");
		nqs_abort();		/* Abort execution */
	}
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Open the pipe queue destination descriptor file
	 *  (and create it if it does not already exist).
	 */
	if ((Pipeqfile = opendb (Nqs_pipeto, O_RDWR | O_CREAT | O_SYNC
		)) == NULL) {
		/*
		 *  We could not open or create the pipe queue
		 *  destination descriptor file.
		 */
		printf ("F$Unable to create/open Pipeqfile.\n");
		nqs_abort();
	}
	/*
	 *  Open the general parameters file (and create it if it does not
	 *  already exist).
	 */
	if ((Paramfile = opendb (Nqs_params, O_RDWR | O_CREAT | O_SYNC
		)) == NULL) {
		/*
		 *  We could not open or create the general parameters
		 *  file.
		 */
		printf ("F$Unable to create/open Paramfile.\n");
		nqs_abort();
	}
	/*
	 *  Open the NQS managers file (and create it if it does not already
	 *  exist).
	 */
	if ((Mgrfile = opendb (Nqs_mgracct, O_RDWR | O_CREAT | O_SYNC
		)) == NULL) {
		/*
		 *  We could not open or create the NQS manager access
		 *  list file.
		 */
		printf ("F$Unable to create/open Mgrfile.\n");
		nqs_abort();
	}
	/*
	 *  Open the NQS forms file (and create it if it does not already
	 *  exist).
	 */
	if ((Formsfile = opendb (Nqs_forms, O_RDWR | O_CREAT | O_SYNC
		)) == NULL) {
		/*
		 *  We could not open or create the NQS forms
		 *  list file.
		 */
		printf ("F$Unable to create/open Formsfile.\n");
		nqs_abort();
	}
	/*
	 *  Load NQS queue, device, and parameter state/configuration.
	 *
	 *  WARNING/NOTE:
	 *	Nqs_ldconf() invokes the function:
	 *
	 *		fetchpwuid()
	 *
	 *	which opens the account/password database on the
	 *	local system, and keeps it open.  All subsequent
	 *	calls to fetchpwuid() and fetchpwnam() will use
	 *	the account/password database that existed at
	 *	the time NQS was booted.
	 *
	 *	This side-effect is bad in that NQS will NOT notice
	 *	NEW (different i-node) /etc/passwd files created
	 *	AFTER NQS was booted.  (However, if changes are
	 *	made DIRECTLY to the /etc/passwd file--that is, the
	 *	/etc/passwd file is not UNLINKED and replaced with a
	 *	new one, then NQS WILL notice the changes.)
	 *
	 *	This side-effect is good in that NQS always has access
	 *	to SOME version of the local account/password database,
	 *	thus preventing a system-wide shortage in the file-
	 *	table from affecting NQS when trying to access the
	 *	account/password database.
	 *
	 *	This side-effect is also beneficial to the extent that
	 *	NQS does not have to reopen the account/password data-
	 *	base every single time that an account entry must be
	 *	gotten from a user-id, or username.
 	 *
	 */
	nqs_ldconf();
	if (Debug) {
		printf ("D$main(): Configuration loaded.\n");
		fflush (stdout);
	}

	/*
	 *  Fork to create the detached NQS local daemon.
	 */
	fflush (stdout);		/* Flush any diagnostic messages */
	fflush (stderr);		/* prior to the fork() */
	if ((pid = fork()) == -1) {
		printf ("F$Unable to fork() NQS local daemon.\n");
		nqs_abort ();
	}
	else if (pid) exit (0);			/* Parent exits here */

	/*
	 *  We are now the NQS local daemon!
	 *  Divorce ourselves from any tty.
	 */
#if	SGI | SYS52 | UTS | UNICOS
	setpgrp ();				/* Process group = pid */
#else
#if	BSD42 | ULTRIX
	setpgrp (0, getpid());			/* Process group = pid */
#else
#if     OSF
        if (0 != setpgid((pid_t)0, (pid_t)getpid())) {
        	printf ("NQS(FATAL): Unable to setpgid).\n");
                nqs_abort();
        }
#else
#if	BSD43
	/*
	 *  The version of Berkeley 4.3 UNIX that was running when we did
	 *  the port of NQS had an absolutely marvelous bug, such that when
	 *  the parent of a process-group exited, the kernel went in and tried
	 *  to divorce all of the progeny of the exiting process-group leader
	 *  from any signals.  This worked fine on 4.2.  In 4.3, they managed
	 *  to screw up, and instead--the process group of the children was
	 *  changed to some other random value.
	 *
	 *  This caused the network daemon spawned later, to often wind up
	 *  in a process group other than the process group of the local
	 *  NQS daemon, and so the qmgr shutdown sequence no longer worked....
	 *
	 *  The following code waits for the parent to exit first, before
	 *  setting the process group, thus we avoid the bug.
	 *
	 *  Sigh....
	 */
	while (getppid() != 1)			/* Wait for parent to exit */
		;				/* to avoid kernel bug */
	setpgrp (0, getpid());			/* Process group = pid */
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#endif

	/*
	 *  If a network daemon is configured for this system, then
	 *  fork off the network daemon at this time.
	 *
	 *  The following constraints are REQUIRED!
	 *
	 *	1.  All of the requisite NQS database files must have
	 *	    been created prior to the forking of the NQS
	 *	    NETWORK daemon, since the NETWORK daemon will
	 *	    immediately open all of them for reading....
	 *
	 *	2.  The NQS NETWORK daemon and the NQS LOCAL daemon
	 *	    MUST be in the same process group (for shutdown
	 *	    purposes).
	 *
	 *	3.  The NQS NETWORK daemon MUST be the child of the
	 *	    NQS LOCAL daemon, so that a crash of the NQS
	 *	    NETWORK daemon can be detected by the NQS LOCAL
	 *	    daemon.
	 */
	Netdaepid = 0;			/* No network daemon at this time */
	if (Netdaemon [0] != '\0') {
		/*
		 *  A network daemon is configured.
		 */
		fflush (stdout);	/* Flush any diagnostic messages */
		fflush (stderr);	/* prior to the fork() */
		if ((Netdaepid = fork()) == -1) {
			printf ("F$(Unable to fork() NQS network daemon.\n");
			nqs_abort();
		}
		else if (Netdaepid == 0) {
			/*
			 *  We are the child, and will become the NQS
			 *  NETWORK daemon, if all goes well....
			 */
			if (parseserv (Netdaemon, new_argv) == -1) {
				/*
				 *  Too many arguments.
				 */
				printf ("F$Too many NQS network daemon ");
				printf ("arguments.\n");
				errno = 0;	/* Not a system call error */
				nqs_abort();	/* Abort execution */
			}
			/*
			 *  Close ALL files with the exception of stdout,
			 *  stderr, and the Write_fifo descriptor writing
			 *  to the local NQS daemon request pipe.
			 *
			 *  When done here, all files will be closed with
			 *  the exceptions:
			 *
			 *	File-descr 1: writes to the NQS log process.
			 *	File-descr 2: writes to the NQS log process.
			 *	File-descr 3: writes to the local NQS daemon
			 *		      request pipe.
			 */
			closepwdb();		/* Close account/password */
						/* database */
			close (0);		/* Close stdin */
			if (Write_fifo != 3) {
				/*
				 *  Force write-fifo to file-descriptor #3.
				 */
				close (3);
				fcntl (Write_fifo, F_DUPFD, 3);
				close (Write_fifo);
			}
			interset (3);	/* Write to local daemon on #3 */
#if	BSD42 | BSD43 | ULTRIX
			i = getdtablesize();	/* #of file descrs per proc */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
			i = _NFILE;		/* #of file descrs per proc */
#else
BAD SYSTEM TYPE
#endif
#endif
			while (--i >= 4) close (i);
			/*
			 *  The NQS NETWORK daemon MUST be vulnerable
			 *  to SIGTERM, since this signal will be broadcast
			 *  to all descendents of the LOCAL NQS daemon upon
			 *  shutdown.  (Queue and device servers are signalled
			 *  independently of this mechanism upon shutdown).
			 *
			 *  The NQS NETWORK daemon should, when appropriate,
			 *  establish a SIGTERM handler to gracefully shutdown
			 *  when so signalled by the LOCAL NQS daemon.
			 */
			signal (SIGTERM, SIG_DFL);
			/*
			 *  Execute the NQS network daemon.
			 */
			execve (new_argv [0], new_argv, new_envp);
			/*
			 *  The exec of the NQS network daemon failed...
			 */
			printf ("F$Unable to execve() NQS network daemon.\n");
			nqs_abort();		/* Abort execution */
		}
	}

#ifdef SDSC
	printf ("I$ Starting NQS 2.0 - Time: %s\n", timstr (time (0)));

        (void)syslog(LOG_INFO,
        "Starting NQS 2.0 ");
        (void)syslog(LOG_INFO| LOG_DAEMON,
        "Starting NQS 2.0 ");
        console_write("Starting NQS 2.0 ");

	/* release all smd request */

        if ((smd_sock = smd_open ()) >= 0) {
            if (smd_cancel_all (smd_sock) < 0)
                printf ("I$nqs_boot: Cannot send msg to SMD\n");
            close (smd_sock);
        }

	/* initial the queues */
 
        init_que ();

	/* read the sched_param file */

        if (read_param () == 0) {

	    /* prt the read in values */

	    prt_param ();

	    /* check the nodef of partitions */

            if (chk_nodef () < 0) {
	        printf ("I$(FATAL):nqs_boot: chk_nodef problem\n");
		glb.sched_flag = NO_SCHED;
		nqs_abort ();
	    } else {

        	/* initial the root struct */
 
        	if (init_rootp () != 0) {
		    printf ("I$nqs_boot: Problem with init_rootp. NQS exiting");
	    	    nqs_abort ();
		}

        	/* set the time for prime start and prime end */
 
                putenv (TIME_ZONE);
        	set_time ();

		glb.sched_flag = SCHED;
	    }
        } else {
	    printf ("I$(FATAL)nqs_boot: read_param problem\n");
	    glb.sched_flag = NO_SCHED;
	    nqs_abort ();
	}
	glb.macs_flag = 0;
#endif

	/*
	 *  Rebuild the NQS queue state.
	 */
	if (Debug) {
		printf ("D$main(): Rebuild queue state.\n");
		fflush (stdout);
	}
	nqs_rbuild();
	if (Debug) {
		printf ("D$main(): Queue state rebuilt.\n");
		fflush (stdout);
	}
	/*
	 *  Enable virtual timers.
	 */
	if (Debug) {
		printf ("D$main(): Enabling virtual timers.\n");
		fflush (stdout);
	}
	nqs_valarm();
}
