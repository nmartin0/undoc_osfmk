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
 
/*++ nqs_reqser.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_reqser.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module contains all of the logic associated with the
 *	request server created for each spawned NQS request.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 5, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.8 $ $Date: 1994/11/19 02:53:14 $ $State: Exp $)
 * $Log: nqs_reqser.c,v $
 * Revision 1.8  1994/11/19  02:53:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/08/31  20:23:29  bradf
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.5.2.1  1994/08/05  20:37:10  kremenek
 *  Reviewer: George Kremenek SDSC
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS passed
 *  Module(s):
 *
 * Revision 1.6  1994/08/05  17:15:04  mwan
 *  Reviewer: George Kremenek
 *  Risk: Low
 *  Benefit or PTS #: 6589 9491 9950 10355
 *  Testing: EATS
 *  Module(s):
 *
 * Revision 1.3.2.2  1994/04/26  21:54:51  mwan
 * Fixed 8230, 8802, 6877 and 9010.
 *
 * Reviewer: kremenek
 *  Risk: Medium
 *  Benefit or PTS #: 8230, 8802, 6877 and 9010.
 *  Testing:
 *  Module(s): usr/lib/nqs/macs_rootp.c netdaemon.c nqs_pip.c nqs_reqser.c
 * 	    ccs/lib/libnqs/listq.c
 *
 * Revision 1.3.2.1  1994/01/26  20:03:29  mwan
 * Certain error situations cause NQS to empty its queues
 *
 *  Reviewer: jkearns
 *  Risk: L
 *  Benefit or PTS #: 7838, 7858
 *  Testing:
 *  Module(s): usr/include/nqs/requestcc.h, usr/include/nqs/buddy.h,
 *                    usr/ccs/lib/rcmmsgs.c,
 *                    usr/lib/nqs/nqs_reqser.c, usr/lib/nqs/nqs_reqexi.c,
 *                    usr/lib/nqs/nqs_reqcom.c, usr/lib/nqs/nqs_spawn.c
 * 		   usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.1  1993/03/08  20:43:38  shala
 * Check it in again since it got corrupted on March 5, 93 while tagging the CVS
 * for transmittal 9 (R1.0).
 *
 * Revision 1.4  1992/12/16  23:26:33  mwan
 * T6 update 2
 *
 * Revision 1.3  1992/10/26  18:47:28  mwan
 * T6 update 1
 *
 * Revision 1.2  1992/10/09  22:26:14  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:32  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:08:00  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_reqser.c	1.3 (nqs_reqser.c OSF/1 NQS2.0 GJK) 7/7/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <errno.h>
#include <pwd.h>			/* Used to determine GID */
#include <signal.h>			/* Signal definitions */
#include <sys/types.h>			/* types definitions */
#include <sys/stat.h>			/* stat definitions */
#include "nqs.h"			/* NQS constants and data types */
#include "nqspacket.h"			/* NQS local message packet types */
#include "nqsmail.h"			/* NQS mail definitions */
#include "nqsxvars.h"			/* NQS global variables and dirs */
#include "requestcc.h"			/* NQS request completion codes */
#include "nqsacct.h"			/* Accounting file structures */
#if	SGI | SYS52 | UNICOS | UTS | OSF | OSF
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif

#ifdef SDSC
#include "buddyxvar.h"
#include <grp.h>
#include <ctype.h>
#include <pwd.h>
#ifdef RES
#include "res.h"
#endif
#ifdef NXACCT
#include <nx/nxacct.h>
#endif
#endif

/*
 *
 *	External functions and variables.
 */
extern int errno;			/* System call error record */
extern char *asciierrno();		/* Return ASCII version of errno */
extern void enf_bsdcpu();		/* Enforce BSD4.2 time limit */
extern void enf_bsdnic();		/* Enforce BSD4.2 nice */
extern void enf_bsdquo();		/* Enforce BSD4.2 resource limit */
extern void enf_sy5cpu();		/* Enforce other OS time limit */
extern void enf_sy5nic();		/* Enforce other OS nice */
extern void enf_sy5quo();		/* Enforce other OS resource limit */
extern long errnototcm();		/* Errno to TCMx_ code */
extern char *fmtmidname();		/* Format machine name for mid */
extern char *fmttime();			/* Format date/time */
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *getenv();			/* Get environment var */
#else
#if	BSD42 | BSD43 | ULTRIX
#else
BAD SYSTEM TYPE
#endif
#endif
extern int inter();			/* Send message packet */
extern void interclear();		/* Clear message packet */
extern void interw32i();		/* Write packet integer datum */
extern long lseek();			/* Lseek() */
extern int mai_send();			/* Send NQS request mail */
extern long mergertcm();		/* Merge transaction code (TCMx_) */
					/* with request completion code */
					/* (RCM_) */
extern char *namstderr();		/* Return name of temp stderr file */
extern char *namstdout();		/* Return name of temp stdout file */
extern char *nmap_get_nam();		/* Get principal name of machine */
extern void nqssleep();			/* NQS sleep() call */
extern int opendata();			/* Open an NQS data file */
extern int parseserv();			/* Parse server arguments */
extern void serexit();			/* Server exit procedure */
/*
 * Sprintf() returns an int on System V, a char * on BSD.
 */
extern char *strcpy();			/* String copy */
extern int strlen();			/* String length */
extern time_t time();
#if	SGI | SYS52 | UTS | UNICOS | OSF
extern char *strrchr();			/* Find last occurrence of char */
#else
#if	BSD42 | BSD43 | ULTRIX
extern char *rindex();			/* Find last occurrence of char */
#else
BAD SYSTEM TYPE
#endif
#endif

#ifdef SDSC
#ifdef NXACCT
extern gid_t nx_getacctid ();
/* extern int nx_setacctid ();	No longer needed. Just set the ACCOUNT var */
extern struct nxacct *nx_getanam ();
#else
extern struct passwd *getpwuid ();
extern struct group *getgrnam ();
extern struct group *getgrgid ();
#endif
#ifdef RES
extern int my_open ();
extern int my_sendmsg ();
extern int my_runok ();
#endif
extern int smd_set_alarm ();
extern int smd_cancel ();
extern int smd_open ();
#endif


/*
 *	Variables local to this module.
 */
static short Stateset;			/* Semaphore */
static long jid;			/* job ID */


/*** nqs_reqser
 *
 *
 *	void nqs_reqser():
 *
 *	We are the request server or shell process.
 *	We are the child of the shepherd process created
 *	in nqs_spawn.c.
 *
 *	  At this point:
 *		File descriptor 0: Control file (O_RDWR).
 *		File descriptor 1: Stdout writing to log process.
 *		File descriptor 2: Stderr writing to log process.
 *		File descriptor 3: Write file descriptor for sending
 *				   back request completion messages
 *				   from the server to the shepherd.
 *		File descriptor 4: Write file descriptor to NQS daemon
 *				   request pipe, this is enforced
 *				   by startup code in nqs_boot.c!
 *
 *	  If we are spawning a batch request with no specific shell,
 *	  and the shell strategy is FREE, then
 *
 *		File descriptor 5: Read file descriptor from the
 *				   shepherd process to the server
 *				   shell process containing the
 *				   user-process visible shell
 *				   script link name.
 *
 *	  otherwise:
 *
 *		File descriptor 5: is closed.
 *
 *	  In all cases:
 *
 *		File descriptors [6.._NFILE-1] are closed.
 *
 *		(On Berkeley UNIX implementations:
 *		 FILE descriptors [6..getdtablesize()-1] are closed.)
 *
 *
 *
 *	One of two distinct situations now exists:
 *
 *	  Case 1:  We are spawning a batch request with a shell
 *		   strategy of FREE.
 *
 *	.-----------------------.
 *	|  NQS daemon process	|<============================================.
 *	|			|<==.					     ||
 *	`-----------------------'  || Named-pipe	Named-pipe connection||
 *		    |		   || connected to	to the local NQS     ||
 *		    V		   || the NQS daemon	daemon request/event ||
 *	.-----------------------.  || request/event	pipe.		     ||
 *	|  Server shepherd	|>==' pipe.				     ||
 *	|  process.		|>=======================================.   ||
 *	|			|<==.					||   ||
 *	`-----------------------'  ||			Pipe from	||   ||
 *		    |		   || Serexit() pipe	shepherd process||   ||
 *		    V		   || TO shepherd	to shell with	||   ||
 *	.-----------------------.  || from shell.	scriptfile name.||   ||
 *	|  Shell process.	|>=='					||   ||
 *	|			|<======================================='   ||
 *	|			|>============================================'
 *	`-----------------------'
 *
 *
 *
 *	  Case 2:  We are spawning a batch request with a shell
 *		   strategy of FIXED or LOGIN (or the request
 *		   specifies a particular shell), or we are spawning
 *		   a device, network, or pipe request.
 * 
 *	.-----------------------.
 *	|  NQS daemon process	|<============================================.
 *	|			|<==.					     ||
 *	`-----------------------'  || Named-pipe	Named-pipe connection||
 *		    |		   || connected to	to the local NQS     ||
 *		    V		   || the NQS daemon	daemon request/event ||
 *	.-----------------------.  || request/event	pipe.		     ||
 *	|  Server shepherd	|>==' pipe.				     ||
 *	|  process.		|<==.					     ||
 *	`-----------------------'  ||					     ||
 *		    |		   || Serexit() pipe			     ||
 *		    V		   || TO shepherd			     ||
 *	.-----------------------.  || from shell.			     ||
 *	|  Shell process.	|>=='					     ||
 *	|			|>============================================'
 *	`-----------------------'
 *
 *
 */
#ifndef SDSC
void nqs_reqser (request, rawreq, restartflag, queue, device, passwd)
#else
void nqs_reqser (request, rawreq, restartflag, queue, device, passwd, j_req)
#endif
struct request *request;		/* Request structure */
struct rawreq *rawreq;			/* Raw request header from */
					/* control file */
int restartflag;			/* BOOLEAN request is restarting */
					/* execution */
struct queue *queue;			/* Queue being served */
struct device *device;			/* Device to use (can be NIL) */
struct passwd *passwd;			/* Password entry for request */
#ifdef SDSC
struct job_req *j_req;
#endif
{
	int mkspool();			/* Make a spooled output file */
	void serexecute();		/* Server executing request */

#if	MAX_QUEUENAME > MAX_DEVNAME
	char argv0buffer [MAX_QUEUENAME + 8];
#else
	char argv0buffer [MAX_DEVNAME + 8];
#endif
					/* Space for argv [0] name of */
					/* queue or device name + " server" */
	char environment [MAX_ENVIRONMENT];
					/* Space for environment */
	char restart_msg [80];		/* Batch request restart message */
	char *server;			/* Server and/or server arguments, */
					/* or pathname of batch shell */
	char *argv [MAX_SERVERARGS+1];	/* Execve() argument pointers */
	char *envp [MAX_ENVIRONVARS+1];	/* Execve() environment pointers */
	struct qdestmap *mapdest;	/* Used to walk destset for pipe-q */
	int fd;				/* File descriptor for device */
	int envpcount;			/* Number of environment vars */
	int envpsize;			/* Size of environment in chars */
	int filemask;			/* = O_CREAT if restartflag is TRUE */
					/* = O_CREAT | O_TRUNC if */
					/*   restartflag is FALSE */
	register int i;			/* Iteration counter & scratch var */
	register char *cp;		/* Character pointer */
	register int persist;		/* Persistent pipe queue request */
	/*
	 *  Variables specific to the spawning of a batch request.
	 */
	char minusname [257];		/* Name of shell (path removed) */
					/* prefixed with a "-" character */
	char buffer [MAX_REQPATH+2];	/* Batch request control file line */
					/* buffer */
	char *shellname;		/* Name of shell (path removed) */
	char *hostname;			/* Name of submitter's host */
	time_t timeval;			/* Current time */
	unsigned long retry_time;	/* Retry time */
	int script;			/* Shell script file descriptor */
	int standout;			/* Standard output file */
	int standerr;			/* Standard error file */
	int fd_acct;			/* Accounting file descriptor */
	struct nqsacct_init acct_init;	/* Accounting record */
	char **cpp;
	int openflags;
	int cmask;
	struct stat sbuf;
	char fullname [MAX_REQPATH+2];
	char *retrymsg = "I$Device offline (%s:%s). retries = %d\n";

#ifdef SDSC
        char *bp;
        int c;  
	int macs_retry;
#ifdef NXACCT
        struct nxacct *nxacct;
#else
        struct group *grp;
#endif
	struct passwd *pwd;
#ifdef RES
        struct nqs_job_info res_info;
#endif
	int smd_sock;
#endif


	envpcount = 0;			/* No environment vars exist yet */
	envpsize = 0;
	if (queue->q.type != QUE_BATCH) {
		/*
		 *  Device, network, and pipe queue servers always have
		 *  a debug level environment variable.
		 */
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "DEBUG=%1d", Debug);
		envpsize += strlen (cp) + 1;
		if (queue->q.type != QUE_DEVICE) {
			/*
			 *  Network queue and pipe queue servers are
			 *  spawned with a real AND effective user-id
			 *  of root.  This is necessary so that the
			 *  ../lib/establish.c module can bind to a
			 *  system socket port (<1024).
			 *
			 *  These servers however must know the account
			 *  upon whose behalf they are operating.  This
			 *  information is passed to them through the
			 *  environment.
			 */
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			sprintf (cp, "UID=%1d", passwd->pw_uid);
			envpsize += strlen (cp) + 1;
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			sprintf (cp, "USERNAME=%s", passwd->pw_name);
			envpsize += strlen (cp) + 1;
#if	SGI | SYS52 | UNICOS | UTS | OSF
			/*
			 *  Network queue and pipe queue servers for
			 *  System V based implementations of NQS always
 			 *  have a TZ (timezone) environment variable
			 *  (for ../lib/establish.c as invoked by network
			 *  and pipe queue servers).  (Please note that
			 *  the TZ environment variable is guaranteed
			 *  to be present, since nqs_boot.c checks for
			 *  it.)
			 *
			 *  Berkeley style NQS implementations do not
			 *  need or use TZ.
			 */
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			envpsize += sprintf (cp, "TZ=%s", getenv ("TZ")) + 1;
#else
#if	BSD42 | BSD43 | ULTRIX
#else
BAD SYSTEM TYPE
#endif
#endif
		}
	}
	switch (queue->q.type) {
	case QUE_BATCH:
		/*
		 *  We are spawning a batch request.
		 */
		filemask = O_CREAT;
		if (!restartflag) {
			/*
			 *  This request is being spawned for the very
			 *  first time.  Set filemask to also include
			 *  O_TRUNC.
			 */
			filemask |= O_TRUNC;
		}
		/*
		 *  Open the shell script file.
		 */
		if (Shell_strategy == SHSTRAT_FREE &&
		    rawreq->v.bat.shell_name [0] == '\0') {
			/*
			 *  The batch request is being run with a shell
			 *  strategy of FREE.  File descriptor #5 has the
			 *  the user-process visible link name of the
			 *  shell script file for the request.
			 */
			script = 5;	/* Read on file-descr #5 */
		}
		else {
			/*
			 *  The batch request is being run with a shell
			 *  strategy of FIXED or LOGIN (or the request
			 *  specifies a particular shell).
			 */
			if ((script = opendata (rawreq->orig_seqno,
						rawreq->orig_mid, 1)) == -1) {
				/*
				 *  We were unable to open the shell script
				 *  file.
				 */
				if (errno == ENFILE) {
					serexit (RCM_ENFILERUN, (char *) 0);
				}
				serexit (RCM_BADCDTFIL,
					"Unable to open shell script file.");
			}
		}
		/*
		 *  Determine the principal name of the request owner's
		 *  machine as seen from the execution machine.
		 */
		errno = 0;		/* Clear errno so that we can tell */
					/* if a system call error occurred */
					/* in the nmap_get_nam() function */
		hostname = nmap_get_nam (rawreq->orig_mid);
		if (hostname == (char *) 0) {
			/*
			 *  Unable to determine the name of the
			 *  request owner's machine.
			 */
			if (errno == ENFILE) {
				serexit (RCM_ENFILERUN, (char *) 0);
			}
			/*
			 *  The request owner's machine-id is no
			 *  longer known to the execution machine.
			 */
			serexit (RCM_MIDUNKNOWN, (char *) 0);
		}
		/*
		 *  Build the login environment and select the proper
		 *  shell.
		 */
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "HOME=%s", passwd->pw_dir);
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		if (rawreq->v.bat.shell_name [0] == '\0') {
		    /*
		     *  The batch request does not explicitly
		     *  specify a shell.  NQS must choose the
		     *  shell used to execute the batch request
		     *  based upon the configured shell
		     *  strategy of the local system.
		     */
		    if (Shell_strategy == SHSTRAT_FIXED) {
			/*
			 *  A shell strategy of FIXED is in
			 *  effect.
			 */
			sprintf (cp, "SHELL=%s", Fixed_shell);
		    }
		    else {
			/*
			 *  A shell strategy of FREE of LOGIN
			 *  is presently in effect.
			 */
			if (passwd->pw_shell == (char *) 0 ||
			    passwd->pw_shell [0] == '\0') {
			    /*
			     *  The default shell is the Bourne shell.
			     */
			    sprintf (cp, "SHELL=%s", "/bin/sh");
			}
			else {
			    /*
			     *  The password file entry for the owner
			     *  identifies a shell to be used.
			     */
			    sprintf (cp, "SHELL=%s", passwd->pw_shell);
			}
		    }
		}
		else {
		    /*
		     *  The request specifies that a specific shell
		     *  be used.
		     */
		    sprintf (cp, "SHELL=%s", rawreq->v.bat.shell_name);
		}
		envpsize += strlen (cp) + 1;
		/*
		 *  At this time, the character pointer variable: cp
		 *  refers to the SHELL= environment variable.
		 *
		 *  Set the variable:  server  to point to the
		 *  fully qualified pathname of the shell that
		 *  is going to be spawned.
		 */
		server = cp + 6;		/* Step over "SHELL=" */
		/*
		 *  Build the remaining login environment variable
		 *  set.
		 */
#if	SGI | SYS52 | UTS | UNICOS | OSF
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		if (passwd->pw_uid == 0) {
			/*
			 *  We are running as root.  The default path
			 *  is slightly different.
			 */
			envpsize += sprintf (cp, "PATH=/bin:/etc:/usr/bin") +1;
		}
		else {
			/*
			 *  The request is not owned by root.
			 */
			envpsize += sprintf (cp, "PATH=:/bin:/usr/bin") + 1;
		}
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		envpsize += sprintf (cp, "LOGNAME=%s", passwd->pw_name) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		envpsize += sprintf (cp, "MAIL=/usr/mail/%s",
				     passwd->pw_name) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		envpsize += sprintf (cp, "TZ=%s", getenv ("TZ")) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		envpsize += sprintf (cp, "ENVIRONMENT=BATCH") + 1;
		if ((shellname = strrchr (server, '/')) != (char *) 0)
#else
#if	BSD42 | BSD43 | ULTRIX
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		if (passwd->pw_uid == 0) {
			/*
			 *  We are running as root.  The default path
			 *  is slightly different.
			 */
			sprintf (cp, "PATH=/usr/ucb:/bin:/etc:/usr/bin");
		}
		else {
			/*
			 *  The request is not owned by root.
			 */
			sprintf (cp, "PATH=:/usr/ucb:/bin:/usr/bin");
		}
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "USER=%s", passwd->pw_name);
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "MAIL=/usr/spool/mail/%s", passwd->pw_name);
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "ENVIRONMENT=BATCH");
		envpsize += strlen (cp) + 1;
		if ((shellname = rindex (server, '/')) != (char *) 0)
#else
BAD SYSTEM TYPE
#endif
#endif
									{
			shellname += 1;		/* Step over last '/' */
		}
		else shellname = server;	/* No '/' to step over */
		/*
		 *  Construct the proper argv array.
		 */
#ifdef SDSC
		/* take out the -. Don't want to source .cshrc and .login
		 * to pickup the env.
		 */
                if (USELOGIN == 0) {
                    sprintf (minusname, "%s", shellname);
                        /* do not use .login, .cshrc */
                }
                else {
                    sprintf (minusname, "-%s", shellname);
                        /*  use .login, .cshrc */
                }
#else
		sprintf (minusname, "-%s", shellname);
#endif
		argv [0] = minusname;
		argv [1] = (char *) 0;
		/*
		 *  Read the varying portion of the control file as a
		 *  sequence of lines terminated with newline characters,
		 *  adding environment variables as appropriate.
		 */
		fseek (stdin, lseek (fileno (stdin), 0L, 1), 0);
		while (!feof (stdin) &&
			fgets (buffer, MAX_REQPATH+2, stdin) != (char *) 0) {
		    i = strlen (buffer) - 1;	/* Get length of buffer -1 */
		    if (buffer [i] != '\n') {
			/*
			 *  We did not see a new line character.
			 *  The control file line is too long.
			 */
			serexit (RCM_BADCDTFIL,
				 "Varying control file line too long.");
		    }
		    buffer [i] = '\0';		/* Delete the newline char */
		    if (buffer [0] == 'D') {
			/*
			 *  This line has the value for the environment
			 *  variable:  QSUB_WORKDIR.
			 */
			i--;		/* i now equals the length of the */
					/* QSUB_WORKDIR directory string*/
			/*
			 *  Check to see if the environment variables:
			 *  QSUB_HOST, QSUB_REQID, QSUB_REQNAME,
			 *  and QSUB_WORKDIR will fit.
			 */
#ifdef SDSC
			if (j_req == NULL) {
			    if (envpcount + 4 >= MAX_ENVIRONVARS ||
			        strlen (hostname) * 2 + envpsize + 59 + i +
			        strlen (rawreq->reqname) > MAX_ENVIRONMENT) {
				    /*
				     *  There is not sufficient space in
				     *  the environment array for all of
				     *  this information.
				     */
				    serexit (RCM_2MANYENVARS, (char *) 0);
			    }
			} else {
			    if (envpcount + 6 >= MAX_ENVIRONVARS ||
			        strlen (hostname) * 2 + envpsize + 59 + i +
			        strlen (j_req->part_name) + 
			        strlen ("NX_DFLT_PART") +
				strlen ("QUEUE_NAME") +
				strlen (j_req->que_name) +
			        strlen (rawreq->reqname) > MAX_ENVIRONMENT) {
				    serexit (RCM_2MANYENVARS, (char *) 0);
			    }
			}

#else
			if (envpcount + 4 >= MAX_ENVIRONVARS ||
			    strlen (hostname) * 2 + envpsize + 59 + i +
			    strlen (rawreq->reqname) > MAX_ENVIRONMENT) {
				/*
				 *  There is not sufficient space in
				 *  the environment array for all of
				 *  this information.
				 */
				serexit (RCM_2MANYENVARS, (char *) 0);
			}
#endif
			/*
			 *  Create the "QSUB_HOST=" environment variable.
			 *  QSUB_HOST=hostname
			 */
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			sprintf (cp, "QSUB_HOST=%s", hostname);
			envpsize += strlen (cp) + 1;
			/*
			 *  Create the "QSUB_REQID=" environment variable.
			 *  QSUB_REQID=nnnnn.hostname
			 */
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			sprintf (cp, "QSUB_REQID=%1ld.%s",
					    (long) rawreq->orig_seqno,
					     hostname);
			envpsize += strlen (cp) + 1;
			/*
			 *  Create the "QSUB_REQNAME=" environment variable.
			 *  QSUB_REQNAME=reqname
			 */
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			sprintf (cp, "QSUB_REQNAME=%s", rawreq->reqname);
			envpsize += strlen (cp) + 1;
			/*
			 *  Create the "QSUB_WORKDIR=" environment variable.
			 *  QSUB_WORKDIR=workdir
			 */
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			sprintf (cp, "QSUB_WORKDIR=%s", buffer+1);
			envpsize += strlen (cp) + 1;
#ifdef SDSC
			/*
			 *  Create the "NX_DFLT_PART=" environment variable.
			 */
			if (j_req != NULL) {
			    cp = environment + envpsize;
			    envp [envpcount++] = cp;
			    sprintf (cp, "NX_DFLT_PART=%s", j_req->part_name);
			    envpsize += strlen (cp) + 1;
			    cp = environment + envpsize;
			    envp [envpcount++] = cp;
			    sprintf (cp, "QUEUE_NAME=%s", j_req->que_name);
			    envpsize += strlen (cp) + 1;
			}
#endif
		    }
		    else if (buffer [0] == 'E') {
			/*
			 *  This is a simple environment variable.
			 *  Get space for the environment variable
			 *  (i has the length of the entire environment
			 *  string + 1), and store the environment
			 *  string.
			 */
			if (envpcount >= MAX_ENVIRONVARS ||
			    envpsize + i > MAX_ENVIRONMENT) {
				/*
				 *  There is not sufficient space in
				 *  the environment array.
				 */
				serexit (RCM_2MANYENVARS, (char *) 0);
			}
#ifdef SDSC
			/* check only if cube_account == 0. i.e., request
			 * from remote station. For local request, it
			 * would have been checked by qsub.
			 */

			if (rawreq->cube_account == 0) {
			    if (strncmp (buffer + 1, "ACCOUNT=",
			    strlen("ACCOUNT=")) == 0) {

                		/* Now, check cube_account */

                		/* is cube_account an integer ? */

				bp = buffer+1+strlen("ACCOUNT=");
                		while ((c = (int) *bp) != '\0') {
                    		    if (isdigit (c) == 0)
                        		break;
                    		    bp++;
                		}
                		if (c != '\0') {    /* not a digit */

                    		    /* take me out. We are using gid in place 
				     * of acct_id for now */

				    bp = buffer+1+strlen("ACCOUNT=");
#ifdef NXACCT
                    		    if ((nxacct = nx_getanam (bp)) == NULL)
                        		serexit (RCM_NOACCAUTH, (char *) 0);
                		    rawreq->cube_account = nxacct->acct_id;
#else
                    		    if ((grp = getgrnam (bp)) == NULL)
                        		serexit (RCM_NOACCAUTH, (char *) 0);
                		    rawreq->cube_account = grp->gr_gid;
#endif
                		} else { 	/* it is a digit */
				    bp = buffer+1+strlen("ACCOUNT=");
				    if (*bp != '\0') {
					rawreq->cube_account = atoi (bp);
				    }
				}
			    }
			}
#endif
#ifdef SDSC
			/* Ignore NX_DFLT_PART and ACCOUNT */

		        if (strncmp (buffer + 1, "NX_DFLT_PART=",
		        strlen("NX_DFLT_PART=")) != 0 &&
			strncmp (buffer + 1, "ACCOUNT=",
			strlen("ACCOUNT=")) != 0) {
			    cp = environment + envpsize;
			    envp [envpcount++] = cp;
			    strcpy (cp, buffer+1);
			    envpsize += i;
			}
#else
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			strcpy (cp, buffer+1);
			envpsize += i;
#endif
		    }
		    /*
		     *  File staging event specifications: [I,O]
		     *  are, and should always be, ignored here.
		     *  Their implementation will always be done
		     *  elsewhere.
		     */
		}
#ifdef SDSC
		/* Export ACCOUNT instead of using nx_setacctid () for the
		 * new NXACCT stuff.
		 */
                cp = environment + envpsize;
                envp [envpcount++] = cp;
                sprintf (cp, "ACCOUNT=%d", rawreq->cube_account);
                envpsize += strlen (cp) + 1;

#ifdef DEBUG
		fprintf (stderr, "I$Cube_account = %d\n", 
		rawreq->cube_account);
#endif
		/* Check with resd on the cube_account */

		if (j_req != NULL) {
		    j_req->acct_id = rawreq->cube_account;
#ifdef RES
		    if (MACS_FLAG != MACS_OFF) {
		        macs_retry = 0;
		        while (1) {
		    	    if (my_open () < 0) {
				macs_retry ++;
			    	fprintf (stderr, 
			    	"I$nqs_reqser: Unable to connect to MACS\n"); 
				if (macs_retry <= MACS_RETRY) {
				    sleep (MACS_SLEEPT);
				    continue;
				}
			    	if (MACS_FLAG == MACS_ON) {
			            fprintf (stderr,
			            "I$nqs_reqser : Request %d requeued\n",
                	    	    rawreq->orig_seqno);
				    fflush (stderr);
                            	    serexit (RCM_NQSABORT, (char *) 0);
			        }
				break;
			    } else {
				break;
			    }
		      	}
		    	if ((c = my_runok (rawreq->cube_account, 
			rawreq->orig_uid, j_req->req_nodes)) < 0) {
			    fprintf (stderr, 
			    "I$ACCT REJECTED: acc=%d, uid=%d, nod=%d\n"
		    	    , rawreq->cube_account 
			    , rawreq->orig_uid, j_req->req_nodes);
			    if (MACS_FLAG == MACS_ON)
                                serexit (RCM_NOACCAUTH, (char *) 0);
		    	}
		        /* Inform resd the job is being started */
		        res_info.uid = j_req->uid;
			res_info.acct_id = rawreq->cube_account;
			res_info.part_id = j_req->part_id;
			res_info.part_size = j_req->req_nodes;
			res_info.cpu_time = 0;
			res_info.part_active = 0;
			res_info.part_idle = 0;
			res_info.event_time = time (0);
			strncpy (res_info.queue_name, j_req->que_name, 
			NAME_LEN);
#ifndef R1_1
                        res_info.submit_time =  rawreq->create_time;
                        /* j_req->start_time ? */
                        res_info.requested_time = j_req->req_time;
#endif
		    	if (my_open () >= 0) {
			   if (my_sendmsg (NQS_JOB_START, &res_info) < 0)
			        fprintf (stderr, 
				"I$nqs_reqser: Cannot send msg to MACS\n");
			}
#define noGJK_DEBUG
                        else {
                            printf ("I$nqs_reqser: Cannot send msg to MACS\n");
                        }
		    }
#endif
		    if ((rawreq->v.bat.infinite & LIM_PRCPUT) == 0) {
		        if ((smd_sock = smd_open ()) >= 0) {
		       	    if (smd_set_alarm (smd_sock, j_req, getppid (), 0) 
			    < 0)
			        fprintf (stderr, 
				"I$nqs_reqser:Cannot send msg to SMD\n"); 
		            if (smd_set_alarm (smd_sock, j_req, getppid (), 
			    GRACE_TIME) < 0) {
			    	fprintf (stderr, 
				"I$nqs_reqser:Cannot send msg to SMD\n"); 
				fflush (stderr);
			    }
			    close (smd_sock);
		      	} else {
			    fprintf (stderr, 
			    "I$nqs_reqser: cannot connect to SMD\n"); 
			    fflush (stderr);
		      	}
		    }
		}
#endif

		/*
		 *  If the output file is to be spooled, then create the
		 *  temporary version of it in the NQS output spooling
		 *  directory BEFORE we chdir() to the user's home
		 *  directory.
		 *
		 *  It is possible that NQS will someday be linked
		 *  with a Newcastle or equivalent distributed file
		 *  system library, which intercepts system calls always
		 *  passing ABSOLUTE file paths to the kernel on the
		 *  appropriate machine.  Since the spool files reside
		 *  within the NQS protected file hierarchy, it is
		 *  necessary to be running as root, when a spool file
		 *  is being created under Newcastle-like file system
		 *  implementations.
		 */
		if (rawreq->v.bat.stdout_acc & OMD_SPOOL) {
			/*
			 *  The output file is to be spooled.
			 */
			standout = mkspool (rawreq, namstdout, passwd,
					    filemask);
			if (standout == -1) {
				/*
				 *  Alas, we failed!
				 */
				if (errno == ENFILE) {
					/*
					 *  The system file table is full.
					 */
					serexit (RCM_ENFILERUN, (char *) 0);
				}
				if (errno == ENOSPC) {
					/*
					 *  Insufficient space.
					 */
					serexit (RCM_ENOSPCRUN, (char *) 0);
				}
				serexit (RCM_UNAFAILURE,
					"Unable to create spooled stdout.");
			}
		}
		/*
		 *  If the error file is to be spooled, then create the
		 *  temporary version of it in the NQS output spooling
		 *  directory BEFORE we chdir() to the user's home
		 *  directory.
		 */
		if ((rawreq->v.bat.stderr_acc & OMD_SPOOL) &&
		    (rawreq->v.bat.stderr_acc & OMD_EO) == 0) {
			/*
			 *  The error file is to be spooled.
			 */
			standerr = mkspool (rawreq, namstderr, passwd,
					    filemask);
			if (standerr == -1) {
				/*
				 *  Alas, we failed!
				 */
				if (errno == ENFILE) {
					/*
					 *  The system file table is full.
					 */
					serexit (RCM_ENFILERUN, (char *) 0);
				}
				if (errno == ENOSPC) {
					/*
					 *  Insufficient space.
					 */
					serexit (RCM_ENOSPCRUN, (char *) 0);
				}
				serexit (RCM_UNAFAILURE,
					"Unable to create spooled stderr.");
			}
		}
		break;
	case QUE_DEVICE:
		/*
		 *  The request is a device request.  Try to open the device
		 *  for reading AND writing.  If this fails, then try to
		 *  open the device for writing only.  If that fails, then
		 *  go through a wait/retry process until the retry limit
		 *  is reached.
		 */
		close (1);		/* Stdout is going to become the */
					/* device leaving only stderr for */
					/* catastrophic error reporting */
		i = 0;
		openflags = 0;
		if ((stat (device->fullname, &sbuf)) == -1) {
			serexit(RCM_DEVOPEFAI, asciierrno());
		}
		if ((sbuf.st_mode & S_IFMT) == S_IFDIR) {
			sprintf(fullname,"%s/%ld.%s",device->fullname,
				(long) rawreq->orig_seqno,
				getmacnam(rawreq->orig_mid));
			cmask = umask(0177);
			openflags = O_CREAT;
		} else
			strcpy(fullname, device->fullname);
		do {
			if ((fd = open (fullname, O_RDWR |
				openflags, 0777)) == -1) {
				/*
				 *  Would not open for read and write.
				 *  Try just write.
				 */
				if ((fd = open (fullname, O_WRONLY |
					openflags,0777)) == -1) {
					if (i < Maxoperet) {
						fprintf(stderr,retrymsg,
						 	device->name,
							fullname,i+1);
						fflush(stderr);
						if (Opewai == 0) nqssleep (1);
						else nqssleep (Opewai);
					}
					i++;	/* Retry count */
				}
			}
		} while (i <= Maxoperet && fd == -1);
		if (fd == -1) serexit (RCM_DEVOPEFAI, asciierrno ());
		if ((sbuf.st_mode & S_IFMT) == S_IFDIR) {
			/*
			 * Make sure the user pays for the spooled 
			 * disk space (it was created while running
			 * under root). 
			 */
			umask (cmask);
			chown(fullname, passwd->pw_uid, passwd->pw_gid);
		}
		/*
	 	 *  Make sure that the device is open 
		 *  as stdout (fd#1).
	 	 */
		if (fd != 1) {
			fcntl (fd, F_DUPFD, 1);
			close (fd);
		}
		/*
		 *  We have the device open.
		 *  Get the name of the server and args to execve.
		 */
		server = device->server;
		break;
	case QUE_NET:
		/*
		 *  We are spawning a network queue request.
		 */
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "DEFAULT_RETRYTIME=%1ld", (long) Defnetrettim);
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "DEFAULT_RETRYWAIT=%1ld", (long) Defnetretwai);
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "ELAPSED_RETRYTIME=0");
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "OP=O");		/* Output file transaction */
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "EVENT=%1d", request->v1.subreq.event);
		envpsize += strlen (cp) + 1;
		server = queue->q.v1.network.server;
		break;				/* Get server/args to execve */
	case QUE_PIPE:
		/*
		 *  We are spawning a pipe queue request.
		 *  Get the server, and format the destination set
		 *  into the environment for the pipe queue server.
		 */
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "DEFAULT_RETRYTIME=%1ld", (long) Defdesrettim);
		envpsize += strlen (cp) + 1;
		cp = environment + envpsize;
		envp [envpcount++] = cp;
		sprintf (cp, "DEFAULT_RETRYWAIT=%1ld", (long) Defdesretwai);
		envpsize += strlen (cp) + 1;
		i = 0;				/* Destination# */
		persist = 0;			/* Unassuming request */
		mapdest = queue->v1.pipe.destset;
		timeval = time ((time_t *) 0);	/* Current time */
		while (mapdest != (struct qdestmap *) 0) {
		    if (mapdest->pipeto->status & DEST_ENABLED) {
			/*
			 *  The destination is enabled.
			 *  Add this destination to the
			 *  set of pipe queue destinations
			 *  in the environment.
			 *
			 *  Note that no explicit checks are
			 *  made here to see if we are running
			 *  off the end of the environment arrays!
			 *
			 *  We are depending on the #defines in
			 *  nqs.h to have been honestly defined.
			 */
			retry_time = 0;
			if ((mapdest->pipeto->status & DEST_RETRY) &&
			     timeval > mapdest->pipeto->retrytime) {
			    retry_time = (unsigned long) timeval
				- (unsigned long) mapdest->pipeto->retrytime;
			}
			cp = environment + envpsize;
			envp [envpcount++] = cp;
			sprintf (cp, "D%03d=%1ld %1lu %s", i++,
				(long) mapdest->pipeto->rhost_mid,
				 retry_time, mapdest->pipeto->rqueue);
			envpsize += strlen (cp) + 1;
		    }
		    else {
			/*
			 *  A destination exists that is presently disabled
			 *  for this pipe queue.  Since not all destinations
			 *  are therefore available to the pipe queue server
			 *  (pipeclient), the request should be allowed to
			 *  persist even if none of the enabled destinations
			 *  accept it, since it is not known that all possible
			 *  destinations for request have been tried.
			 */
			persist = 1;		/* Let the request persist */
		    }
		    mapdest = mapdest->next;	/* Get the next destination */
		}
		if (persist) {
		    /*
		     *  The request should be allowed to persist, even
		     *  if no destination accepts it this time.
		     */
		    cp = environment + envpsize;
		    envp [envpcount++] = cp;
		    strcpy (cp, "PERSIST=Y");
		    envpsize += strlen (cp) + 1;
		}
		server = queue->q.v1.pipe.server;
	}					/* Get server/args to execve */
	/*
	 *  Place the NIL pointer at the end of the environment var
	 *  list for the server.
	 */
	envp [envpcount] = (char *) 0;
	/*
	 *  In ALL cases, the current working directory is the NQS
	 *  root directory, and the appropriate environment has been
	 *  constructed for the server.
	 */
	if (queue->q.type != QUE_BATCH) {
		/*
		 *  A device, network, or pipe queue request is
		 *  being spawned.
		 */
		if (parseserv (server, argv) == -1) {	/* Break into args */
			/*
			 *  Too many server arguments in server command line.
			 */
			serexit (RCM_2MANYSVARGS, (char *) 0);
		}
		server = argv [0];	/* Get ptr to program to execve */
		if (queue->q.type == QUE_DEVICE) cp = device->name;
		else if (queue->q.type == QUE_NET) {
			cp = fmtmidname (queue->q.namev.to_destination);
		}
		else cp = queue->q.namev.name;
		argv [0] = argv0buffer;
		sprintf (argv [0], "%s %s", cp, "server");
	}
	/*
	 *  Show debugging information if required.
	 */
	if (Debug) {
		/*
		 *  Display debug information.  Note that we have to use
		 *  stderr, since stdout is used for devices....
		 */
		fprintf (stderr, "D$nqs_reqser().\n");
		fprintf (stderr, "D$Execve'ing server: %s.\n", server);
		fflush (stderr);
		if (Debug > 1) {
			i = 0;
			while (argv [i] != (char *) 0) {
				fprintf (stderr, "D$argv[%1d]=%s.\n", i,
					 argv [i]);
				fflush (stderr);
				i++;
			}
			i = 0;
			while (envp [i] != (char *) 0) {
				fprintf (stderr, "D$envp[%1d]=%s.\n", i,
					 envp [i]);
				fflush (stderr);
				i++;
			}
		}
	}
	/*
	 *  Send mail as necessary notifying the user that their request
	 *  is beginning or restarting execution.
	 */
	if (queue->q.type == QUE_BATCH || queue->q.type == QUE_DEVICE) {
		if (restartflag) {	/* Restarting execution */
			if (rawreq->flags & RQF_RESTARTMAIL) {
				mai_send (rawreq, (struct requestlog *) 0,
					  MSX_RESTARTING);
			}
		}
		else {			/* Beginning execution for the first */
			if (rawreq->flags & RQF_BEGINMAIL) {	/* time */
				mai_send (rawreq, (struct requestlog *) 0,
					  MSX_BEGINNING);
			}
		}
	}
	/*
	 *  Set nice value for batch requests.
	 */
	if (queue->q.type == QUE_BATCH) {
#if		(VALID_LIMITS & LIM_PPNICE)
#if		BSD42 | BSD43 | ULTRIX
		enf_bsdnic (rawreq->v.bat.ppnice);
#else
#if		SGI | SYS52 | UTS | UNICOS | OSF
		enf_sy5nic (rawreq->v.bat.ppnice);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
	}
	/*
	 *  Set ourselves in our very own process group/family so that
	 *  the NQS daemon can kill us (and our children) as a group if
	 *  necessary.
	 */
#if	UNICOS
	if (queue->q.type == QUE_BATCH) {
		jid = setjob();		/* Create a new job */
	} else {
		jid = getpid();
	}
	setpgrp ();			/* Process group = pid */
#else
#if	SGI | SYS52 | UTS
	jid = getpid();
	setpgrp ();			/* Process group = pid */
#else
#if	BSD42 | BSD43 | ULTRIX
	jid = getpid();
	setpgrp (0, jid);		/* Process group = pid */
#else
#if     OSF
	jid = getpid();
        if (0 != setpgid((pid_t)jid, (pid_t)jid)) { 
                fprintf (stderr, "D$NQS(FATAL): Unable to setpgid).\n");
		nqs_abort();
        }
#ifdef NXACCT
	/* no longer needed - just set the ACCOUNT variable.
        if (0 != nx_setacctid(rawreq->cube_account)) { 
                fprintf (stderr, "I$Nqs_reqser: Unable to setacctid).\n");
	}
	*/
#endif
	
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#endif
	if (queue->q.type == QUE_BATCH) {
		/*
		 *  We are spawning a batch request.
		 *  Enforce resource limits on the process or process group
		 *  while we're still root.
		 */
#if		(VALID_LIMITS & LIM_PPCORE)
#if		BSD42 | BSD43 | ULTRIX | OSF
		enf_bsdquo (&rawreq->v.bat.ppcoresize,
			    rawreq->v.bat.infinite & LIM_PPCORE, LIM_PPCORE);
#else
#if		SGI | SYS52 | UTS | UNICOS
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if		(VALID_LIMITS & LIM_PPDATA)
#if		BSD42 | BSD43 | ULTRIX | OSF
		enf_bsdquo (&rawreq->v.bat.ppdatasize,
			    rawreq->v.bat.infinite & LIM_PPDATA, LIM_PPDATA);
#else
#if		SGI | SYS52 | UTS | UNICOS 
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if		(VALID_LIMITS & LIM_PPPFILE)
#if		BSD42 | BSD43 | ULTRIX | OSF
		enf_bsdquo (&rawreq->v.bat.pppfilesize,
			    rawreq->v.bat.infinite & LIM_PPPFILE, LIM_PPPFILE);
#else
#if		SGI | SYS52 | UTS | UNICOS
		enf_sy5quo (&rawreq->v.bat.pppfilesize,
			    rawreq->v.bat.infinite & LIM_PPPFILE, LIM_PPPFILE);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if		(VALID_LIMITS & LIM_PPMEM)
#if		UNICOS
		enf_sy5quo (&rawreq->v.bat.ppmemsize,
			    rawreq->v.bat.infinite & LIM_PPMEM, LIM_PPMEM);
#else
#if		BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if		(VALID_LIMITS & LIM_PRMEM)
#if		UNICOS
		enf_sy5quo (&rawreq->v.bat.prmemsize,
			    rawreq->v.bat.infinite & LIM_PRMEM, LIM_PRMEM);
#else
#if		BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if		(VALID_LIMITS & LIM_PPSTACK)
#if		BSD42 | BSD43 | ULTRIX | OSF
		enf_bsdquo (&rawreq->v.bat.ppstacksize,
			    rawreq->v.bat.infinite & LIM_PPSTACK, LIM_PPSTACK);
#else
#if		SGI | SYS52 | UTS | UNICOS
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if		(VALID_LIMITS & LIM_PPWORK)
#if		BSD42 | BSD43 | ULTRIX | OSF
		enf_bsdquo (&rawreq->v.bat.ppworkset,
			    rawreq->v.bat.infinite & LIM_PPWORK, LIM_PPWORK);
#else
#if		SGI | SYS52 | UTS | UNICOS
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if		(VALID_LIMITS & LIM_PPCPUT)
#if		BSD42 | BSD43 | ULTRIX | OSF
		enf_bsdcpu (&rawreq->v.bat.ppcputime,
			    rawreq->v.bat.infinite & LIM_PPCPUT, LIM_PPCPUT);
#else
#if		UNICOS
		enf_sy5cpu (&rawreq->v.bat.ppcputime,
			    rawreq->v.bat.infinite & LIM_PPCPUT, LIM_PPCPUT);
#else
#if		SGI | SYS52 | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#endif

#if		(VALID_LIMITS & LIM_PRCPUT)
#if		UNICOS
		enf_sy5cpu (&rawreq->v.bat.prcputime,
			    rawreq->v.bat.infinite & LIM_PRCPUT, LIM_PRCPUT);
#else
#if		BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTSQ | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
	}

	if (queue->q.type == QUE_BATCH) {
		/*
	 	*  Write an entry in an NQS accounting file.
	 	*/
		fd_acct = open(NQSACCT_FILE,
			O_WRONLY|O_APPEND|O_CREAT, 0644);
		if (fd_acct < 0) {
			fprintf (stderr, "E$Error opening NQS account");
			fprintf (stderr, " file;  Errno = %d\n", errno);
			fflush (stderr);
		} else {
			bytezero((char *)&acct_init, sizeof(acct_init));
			acct_init.h.type = NQSACCT_INIT;
			acct_init.h.length = sizeof(acct_init);
			acct_init.h.jobid = jid;
			strncpy(acct_init.user, rawreq->username,
				sizeof(acct_init.user));
			strncpy(acct_init.queue, rawreq->quename,
				sizeof(acct_init.queue));
			acct_init.priority = queue->q.priority;
			acct_init.sub_time = rawreq->create_time;
			acct_init.start_time = rawreq->start_time;
			acct_init.init_time = time ((time_t *) 0);
			acct_init.orig_mid = rawreq->orig_mid;
#if	UNICOS
			for (cpp = envp; *cpp != (char *)0; cpp++) {
				if (strncmp(*cpp, "SCP_MF", 6) == 0) {
					strncpy(acct_init.scp_mf, *cpp+7,
						sizeof(acct_init.scp_mf));
					continue;
				}
				if (strncmp(*cpp, "SCP_TID", 7) == 0) {
					strncpy(acct_init.scp_tid, *cpp+8,
						sizeof(acct_init.scp_tid));
				}
			}
#endif
			write(fd_acct, (char *)&acct_init, sizeof(acct_init));
		}
	}
	if (queue->q.type == QUE_BATCH || queue->q.type == QUE_DEVICE) {
		/*
		 *  Batch and device requests must now set their real and
		 *  effective user and group-ids to that of the request
		 *  owner.
		 *
		 *  Network queue and pipe queue servers must NOT do this.
		 *  Network queue and pipe queue servers must be spawned
		 *  as root.
		 */
#if UNICOS & NEWPWD
		if (acctid (0,passwd->pw_acid[0]) == -1) {
			fprintf (stderr, "Invalid account ID\n");
			fflush (stderr);
		}
#endif
		setgid ((int) passwd->pw_gid);	/* Set login group-id */
#ifdef SDSC
		/* Security fix from Purdue. If initgroups is not done,
		 * will inherit parents's group set.
		 */
		initgroups (passwd->pw_name, passwd->pw_gid);
#endif
		setuid ((int) passwd->pw_uid);	/* Set mapped user-id */
	}
	/*
	 *  Explicitly set the signal receipt actions to their defaults so
	 *  that the server gets a completely "clean" signal environment.
	 *  (Note that for requests running in batch queues, every signal is
	 *  set to default except for SIGTERM, which is set to SIG_IGN.)
	 */
	signal (SIGHUP, SIG_DFL);
	signal (SIGINT, SIG_DFL);
	signal (SIGQUIT, SIG_DFL);
	signal (SIGILL, SIG_DFL);
	signal (SIGTRAP, SIG_DFL);
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	signal (SIGIOT, SIG_DFL);
	signal (SIGEMT, SIG_DFL);
#else
#if	UNICOS
	signal (SIGHWE, SIG_DFL);
#else
BAD SYSTEM TYPE
#endif
#endif
	signal (SIGFPE, SIG_DFL);
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	signal (SIGBUS, SIG_DFL);
	signal (SIGSEGV, SIG_DFL);
#else
#if	UNICOS
	signal (SIGPRE, SIG_DFL);
	signal (SIGORE, SIG_DFL);
#else
BAD SYSTEM TYPE
#endif
#endif
	signal (SIGSYS, SIG_DFL);
	signal (SIGPIPE, SIG_DFL);
	signal (SIGALRM, SIG_DFL);
	if (queue->q.type == QUE_BATCH) signal (SIGTERM, SIG_IGN);
	else signal (SIGTERM, SIG_DFL);
#if	SGI | SYS52 | UTS | OSF
	signal (SIGUSR1, SIG_DFL);
	signal (SIGUSR2, SIG_DFL);
#if OSF
        signal (SIGCHLD, SIG_DFL);

#else
	signal (SIGCLD, SIG_DFL);
	signal (SIGPWR, SIG_DFL);
#endif

#else
#if	BSD42 | BSD43 | ULTRIX
	signal (SIGURG, SIG_DFL);
	signal (SIGSTOP, SIG_DFL);
	signal (SIGTSTP, SIG_DFL);
	signal (SIGCONT, SIG_DFL);
	signal (SIGCHLD, SIG_DFL);
	signal (SIGTTIN, SIG_DFL);
	signal (SIGTTOU, SIG_DFL);
	signal (SIGIO, SIG_DFL);
	signal (SIGXCPU, SIG_DFL);
	signal (SIGXFSZ, SIG_DFL);
	signal (SIGVTALRM, SIG_DFL);
	signal (SIGPROF, SIG_DFL);
#else
#if	UNICOS
	signal (SIGUSR1, SIG_DFL);
	signal (SIGUSR2, SIG_DFL);
	signal (SIGCLD, SIG_DFL);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
	/*
	 *  ***TRIAGE***
	 *
	 *  Extreme schedule constraints have forced us to leave
	 *  network queues unimplemented (software implementation
	 *  by triage).  When the day arrives that network queues
	 *  are a reality, then the obvious code section below
	 *  should be changed to read:
	 */  
	    /*
	     *  Report our "process-family" to the NQS daemon.
	     */
	    /*	interclear();                 */
	    /*	interw32i ((long) getppid()); */ /* Shepherd process-id */
	    /*	interw32i ((long) getpid());  */ /* Our process-id */
	    /*	inter (PKT_FAMILY);           */

	/*
	 *  WITHOUT the surrounding "if" statement that PREVENTS
	 *  network queue requests from sending PKT_FAMILY packets
	 *  to the local NQS daemon.
	 */

	if (queue->q.type != QUE_NET) {		/* THIS IF STATEMENT SHOULD */
						/* EVENTUALLY BE REMOVED */
						/* (SEE ABOVE COMMENTS) */
		/*
		 *  Report our "process-family" to the NQS daemon.
		 */
		interclear();
		interw32i ((long) getppid());	/* Shepherd process-id */
		interw32i ((long) jid);		/* Our job id */
#ifdef SDSC
		if (j_req != NULL) {
		    interw32i ((long) j_req->part_id);	   /* Our part_id */
		} else {
		    interw32i ((long) -1);
		}
#endif		    
		inter (PKT_FAMILY);
	}					/* (SEE ABOVE COMMENTS) */
	/*
	 *  Batch and device requests are not allowed to retain their
	 *  connection to the NQS daemon event/request pipe.  Only pipe
	 *  and network queue servers need to be able to talk directly to
	 *  the NQS daemon.
	 */
	if (queue->q.type == QUE_BATCH || queue->q.type == QUE_DEVICE) {
		close (Write_fifo);
	}
	/*
	 *  Batch requests are handled differently....
	 */
	if (queue->q.type == QUE_BATCH) {
		/*
		 *  Change directory to the user's home directory.
		 */
		if (chdir (passwd->pw_dir) == -1) {
			/*
			 *  We were unable to chdir() to the user's
			 *  home directory.
			 */
			serexit (RCM_UNABLETOEXE,
				 "Unable to chdir() to user home directory.");
		}
		/*
		 *  Set the file creation mask to the value that it
		 *  had when the user first submitted the request, with
		 *  exception that the umask will NOT be allowed to
		 *  disable WRITE access for the request owner.
		 *
		 *  We do this so that any stdout or stderr output file
		 *  created at this point, will be created such that it
		 *  will be writeable by the request owner.  This is
		 *  required if the request is to be restartable, and
		 *  a stdout or stderr file exists from a previously
		 *  interrupted execution of the batch request.
		 *
		 *  The umask will be set precisely later on.
		 */
		umask (rawreq->v.bat.umask & 0577);	/* Set partial umask */
		if ((rawreq->v.bat.stdout_acc & OMD_SPOOL) == 0) {
			/*
			 *  The standard-output file is to be accessed
			 *  directly.  We had to wait until after the chdir()
			 *  because the pathname could be relative.
			 */
			while ((standout = open (rawreq->v.bat.stdout_name,
				O_WRONLY | filemask | O_APPEND, 0777)) == -1 &&
				errno == EINTR)
				;
			if (standout == -1) {
				/*
				 *  Unable to create standard output file.
				 */
				if (errno == ENFILE) {
					/*
					 *  The system file table is full.
					 */
					serexit (RCM_ENFILERUN, (char *) 0);
				}
				if (errno == ENOSPC) {
					/*
					 *  Insufficient space.
					 */
					serexit (RCM_ENOSPCRUN, (char *) 0);
				}
				serexit (mergertcm (RCM_UNCRESTDOUT,
						    errnototcm()), (char *) 0);
			}
		}
		if ((rawreq->v.bat.stderr_acc & OMD_SPOOL) == 0 &&
		    (rawreq->v.bat.stderr_acc & OMD_EO) == 0) {
			/*
			 *  The standard-error file is to be accessed
			 *  directly.  We had to wait until after the
			 *  chdir() because the pathname may have been
			 *  specified as a relative pathname.
			 */
			while ((standerr = open (rawreq->v.bat.stderr_name,
				O_WRONLY | filemask | O_APPEND, 0777)) == -1 &&
				errno == EINTR)
				;
			if (standerr == -1) {
				/*
				 *  Unable to create standard error file.
				 */
				if (errno == ENFILE) {
					/*
					 *  The system file table is full.
					 */
					serexit (RCM_ENFILERUN, (char *) 0);
				}
				if (errno == ENOSPC) {
					/*
					 *  Insufficient space.
					 */
					serexit (RCM_ENOSPCRUN, (char *) 0);
				}
				serexit (mergertcm (RCM_UNCRESTDERR,
						    errnototcm()), (char *) 0);
			}
		}
		else if (rawreq->v.bat.stderr_acc & OMD_EO) {	/* -eo */
			/*
			 *  The stderr output of the request is to be directed
			 *  to the stdout file.
			 */
			standerr = fcntl (standout, F_DUPFD, 0);
			fcntl (standerr, F_SETFL, O_APPEND);
		}
		/*
		 *  Set the file creation mask to the value that it
		 *  had when the user first submitted the request.
		 */
		umask (rawreq->v.bat.umask);	/* Set precise umask */
		if (restartflag) {
			/*
			 *  This batch request is being restarted.
			 *  Write an appropriate message on the stdout
			 *  file indicating this fact.
			 */
			time (&timeval);	/* Get current time */
			sprintf (restart_msg,
			       "\n\n%%NQS(WARN): Request restarted at %s.\n\n",
				fmttime (&timeval));
			write (standout, restart_msg, strlen (restart_msg));
			if ((rawreq->v.bat.stderr_acc & OMD_EO) == 0) {
				/*
				 *  Write an appropriate message on the
				 *  stderr file as well.
				 */
				write (standerr, restart_msg,
				       strlen (restart_msg));
			}
		}
		/*
		 *  Mark the request as executing, if the request is
		 *  being executed for the first time.
		 *
		 *  WARNING:  We must do this now, BEFORE rearranging
		 *	      the file descriptor associations for the
		 *	      shell (see below).  Otherwise, file
		 *	      descriptor #3 will no longer be open
		 *	      as the request message/completion pipe
		 *	      back to the shepherd process.
		 */
		serexecute ();
		/*
		 *  Force the shell script file to be on file descriptor
		 *  #0.  Force the stdout and stderr files to be on file
	 	 *  descriptors #1 and #2 respectively.
		 */
		if (script != 0) {
			close (0);
			fcntl (script, F_DUPFD, 0);
			close (script);
		}
		if (standout != 1) {
			close (1);
			fcntl (standout, F_DUPFD, 1);
			close (standout);
		}
		if (standerr != 2) {
			close (2);
			fcntl (standerr, F_DUPFD, 2);
			close (standerr);
		}
		/*
		 *  Close all remaining file descriptors.
		 */
#if	BSD42 | BSD43 | ULTRIX
		i = getdtablesize();		/* #of file descrs per proc */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
		i = _NFILE;			/* #of file descrs per proc */
#else
BAD SYSTEM TYPE
#endif
#endif
		while (--i >= 3) close (i);	/* Close all other descrs */
		/*
		 *  Exec the proper shell.
		 */
		execve (server, argv, envp);
		/*
		 *  Fall through if execve() fails.
		 *
		 *  We have severed all connections to the shepherd
		 *  process that created us.  We must invoke a setuid
		 *  root program (shlexefai), to signal the shepherd
		 *  that the execve() of the shell, failed.
		 */
#ifndef	NQS_LIBEXE
The symbol:  NQS_LIBEXE  MUST be defined from the Makefile.
#else
		sprintf (buffer, "%s/shlexefai", NQS_LIBEXE);
#endif
		/*
		 *  Format errno as argv [1] for shlexefai.
		 */
		strcpy (argv0buffer, "shlexefai");
		sprintf (minusname, "%1d", errno);
		/*
		 *  Write shell execve() diagnostic on the stderr
		 *  file of the batch request.
		 */
		fprintf (stderr, "FATAL:  Execve() of shell: %s failed.\n",
			 server);
		fprintf (stderr, "INFO:   %s.\n", asciierrno());
		fflush (stdout);
		fflush (stderr);
		/*
		 *  Execve() shlexefai to inform the shepherd about
		 *  the shell execve() failure.
		 */
		argv [0] = argv0buffer;
		argv [1] = minusname;
		argv [2] = (char *) 0;
		envp [0] = (char *) 0;
		execve (buffer, argv, envp);
		/*
		 *  We are really having a bad day!  Nothing works!
		 *  Commit seppuku.  We cannot even display a diagnostic
		 *  to the NQS log file because our connection with the
		 *  NQS log process has been severed.
		 */
		kill (getpid(), SIGKILL);
	}
	/*
	 *  If the request is a device request, then the request must
	 *  be marked as executing.
	 */
	if (queue->q.type == QUE_DEVICE) serexecute ();
	/*
	 *  We are spawning a device, network, or pipe queue request.
	 *  Execve() the appropriate server.
	 */
	execve (server, argv, envp);
	/*
	 *  Execve() failed.
	 */
	serexit (RCM_SEREXEFAI, asciierrno());
}


/*** mkspool
 *
 *
 *	int mkspool():
 *
 *	Make a standard error or standard output file for a batch
 *	request in the NQS output spooling directory.
 *
 *	WARNING!!!!!!!!!!!!!!!!
 *		The EXTREMELY serious student will note that this
 *		function is invoked while the calling shell process
 *		is still running as root.
 *
 *		This is done because someday, NQS may be linked
 *		with the Newcastle distributed file access library.
 *		Since the Newcastle library converts ALL file paths
 *		to ABSOLUTE paths on the appropriate machine when
 *		invoking the requisite system call, NQS must still
 *		be running as root to safely create the spool output
 *		files in the spooling directory, since the spooling
 *		directory resides BELOW the NQS private directory,
 *		which only allows root access.
 *
 *	Returns:
 *	      >=0: if successful, as the opened file descriptor
 *		   for the stderr file;
 *	       -1: if an error occurs in which case errno is set.
 */
int mkspool (rawreq, namefn, passwd, filemask)
struct rawreq *rawreq;			/* Rawreq structure for request */
char *(*namefn)();			/* Ptr to fn returning ptr to char */
struct passwd *passwd;			/* Password file entry for owner */
int filemask;				/* File create/truncate flag */
{
	register int fd;		/* File descriptor */
	register char *path;		/* Ptr to temp file path */
	register int cmask;		/* umask */

	/*
	 *  Generate name of temporary spooled output file.
	 */
	path = (*namefn) (rawreq->orig_seqno, rawreq->orig_mid);
	/*
	 *  Set the file creation mask to the value that it
	 *  had when the user first submitted the request, with
	 *  the exception that the umask will NOT be allowed to
	 *  disable WRITE access for the request owner.
	 *
	 *  We do this so that the output return algorithm on
	 *  retry will not have to worry about being rebuffed
	 *  at the output file destination by a lack of write-
	 *  access.  (Note that the output return algorithm
	 *  replicates the permissions of the original file
	 *  for the destination file.)
	 */
	cmask = umask (rawreq->v.bat.umask & 0577);	/* Set partial umask */
	if ((fd = open (path, filemask | O_WRONLY | O_APPEND, 0777)) == -1) {
		return (-1);
	}
	umask (cmask);			/* Restore the file create mask */
	/*
	 *  Make sure that the user pays for the spooled disk space.
	 *  (The spool file was created while we were running as root).
	 */
	chown (path, passwd->pw_uid, passwd->pw_gid);
	return (fd);
}


/*** seracknowledge
 *
 *
 *	void seracknowledge():
 *
 *	Catch SIGPIPE request state set to executing acknowledgement
 *	from the server shepherd process.
 */
static void seracknowledge()
{
	Stateset = 1;			/* Request state has been set */
}

 
/*** serexecute
 *
 *
 *	void serexecute():
 *
 *	Tell the shepherd process to set the request state to
 *	executing.
 */
void serexecute ()
{
#if OSF
	void (*prevsigfn)();		/* Previous signal function */
#else
	int (*prevsigfn)();		/* Previous signal function */
#endif
	struct servermssg msg_packet;	/* Message packet */

	msg_packet.rcm = RCM_EXECUTING;
	msg_packet.id = jid;
	msg_packet.mssg [0] = '\0';
	/*
	 *  Set signal handler to catch acknowledge signal from the
	 *  server shepherd process.
	 */
	Stateset = 0;			/* Clear semaphore */
	prevsigfn = signal (SIGPIPE, seracknowledge);
	/*
	 *  WARNING:	The file descriptor below (#3) must agree with
	 *		../src/nqs_spawn and ../lib/serexit.c.
	 */
	if (write (3, (char *) &msg_packet, sizeof (msg_packet)) !=
		sizeof (msg_packet)) {
		/*
		 * We cannot communicate with the shepherd through
		 * the pipe. Exit so the shepherd's read() will return.
		 */
		exit (0);
	}
	/*
	 *  Loop waiting for acknowlege signal from shepherd process.
	 */
	while (!Stateset) pause();	/* Wait for signal */
	signal (SIGPIPE, prevsigfn);	/* Restore SIGPIPE handler */
}
