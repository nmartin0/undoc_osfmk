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
 
/*++ mkreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/mkreq.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains 4 functions which respectively:
 *
 *	  o Create a request control file (mkctrl),
 *	  o Create a request data file (mkdata),
 *	  o Unlink all files associated with a request from the
 *	    NQS new request staging directory (zapreq), and
 *	  o Submit a request to the NQS queueing system, (quereq).
 *
 *	for LOCALLY submitted requests ONLY.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.6 $ $Date: 1994/11/19 02:27:17 $ $State: Exp $)
 * $Log: mkreq.c,v $
 * Revision 1.6  1994/11/19  02:27:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/11/02  01:24:04  mwan
 * 1.2 mods
 *
 * Revision 1.4  1993/07/21  18:50:50  mwan
 * T11 - Fix NFS file access problem from qsub.
 *
 * Revision 1.2  1992/10/09  20:17:24  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:15  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:41  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)mkreq.c	50.2 (mkreq.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>		/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include <pwd.h>		/* Password file formats */
#include NETDB			/* Network database types */
#include <signal.h>		/* Signal definitions */
#include "nqs.h"		/* NQS constants and data types */
#include "nqspacket.h"		/* NQS local message packets */
#include "nqsxdirs.h"		/* NQS directories */
#include "informcc.h"		/* NQS information completion codes and masks */
#include "transactcc.h"		/* NQS transaction completion codes */
#include "mkreqcc.h"		/* Make request completion codes */

/*
 *	External functions.
 */
extern void closedb();		/* Close an NQS database file */
extern void closepwdb();	/* Close account/password database */
extern FILE *fdopen();		/* Associate file stream */
extern struct passwd *fetchpwuid();
				/* Fetch passwd entry for user-id */
extern char *getenv();		/* Get value of environment var */
extern gid_t getgid();		/* Get effective group-id */
extern int getpid();		/* Get process PID */
extern uid_t getuid();		/* Get effective user-id */
extern char *getwdir();		/* Get current working directory */
extern long inter();		/* Send queue req packet to the NQS */
				/* daemon */
extern void interclear();	/* Clear inter-process communication packet */
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
#if	BSD42 | BSD43
extern int interconn();		/* Make up for lack of named pipes. */
#else
BAD SYSTEM TYPE
#endif
#endif
extern void interwstr();	/* Write string for message packet */
extern struct gendescr *nextdb();
				/* Get next allocated descriptor from */
				/* an NQS database file */
extern int nmap_get_mid();	/* Get machine-id of local machine */
extern struct confd *opendb();	/* Open NQS database file */
extern void pack6name();	/* Compute name on 6-bit alphabet */
extern int readhdr();		/* Read request common control file header */
#if	BSD42 | BSD43 | ULTRIX
extern char *index();		/* Locate character in string */
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
extern char *strchr();		/* Locate character in string */
#else
BAD SYSTEM TYPE
#endif
#endif
extern char *strncpy();		/* Anchored string copy */
extern time_t time();		/* Get time in seconds */


/*
 *	Variables global to all functions within this module.
 */
static char ctrlname [26+1];	/* Synthesized pathname for control file */
				/* (11 character control-file name + */
				/*  14 character directory name + */
				/*   1 character for "/") */
static short n_datafiles = 0;	/* Number of data files for request. */
static int ctrlfile = -1;	/* Control file file-descriptor */

/*** mkctrl
 *
 *
 *	FILE *mkctrl():
 *
 *	WARNING/NOTE:
 *		The effective user-id of the caller MUST
 *		be the owner user-id of the NQS directory
 *		hierarchy, or root.
 *
 *	This function creates a control file in the NQS local
 *	new request directory opened for reading AND writing by
 *	the caller, and initializes an associated raw request
 *	structure.
 *
 *	The original caller's working directory is returned
 *	by this function in the parameter:  workdir, and the
 *	caller's working directory is changed to the NQS new
 *	(not local new) request directory.
 *
 *	The file creation mask (umask), of the calling process
 *	is set to zero.
 *
 *	Lastly, the effective user-id and group-id of the caller
 *	are set to their real values by this function.
 *
 *	Returns:
 *		MKREQ_SUCCESS	  Control file successfully created.
 *		MKREQ_NOCREATE	  Unable to open/create control file.
 *		MKREQ_NOMID	  Unable to get machine-id of local host.
 *		MKREQ_BADREQTYPE  Bad request type specified.
 *		MKREQ_NOUSERNAME  Could not get username for user-id.
 *		MKREQ_NOCWD	  Unable to get current working directory.
 *		MKREQ_CWDNEWLINE  Current working directory contained
 *				  a new line character.
 *		MKREQ_NOCHDIRNEW  Unable to chdir() to the NQS new request
 *				  directory.
 *		MKREQ_NOSETUGID	  Unable to set [uid,gid].
 *		MKREQ_NOPARMFILE  Unable to open NQS parameters file.
 *		MKREQ_INVALIDACC  Invalid account ID.
 *
 */
int mkctrl (rawreq, reqtype, ptrfile, workdir, local_mid)
register struct rawreq *rawreq;	/* Pointer to raw request structure. */
int reqtype;			/* Request type: RTYPE_BATCH, RTYPE_DEVICE */
FILE **ptrfile;			/* Ptr to FILE structure for data file */
char **workdir;			/* Ptr to where to return ptr to the */
				/* caller's original working directory */
mid_t *local_mid;		/* Machine-id of local host */
{
	void set_default_quota();
				/* Initialize a default quota limit */

	char path [MAX_PATHNAME+1];
	struct confd *paramfile;/* NQS general parameters file */
	struct gendescr *descr;	/* Database descriptor */
	struct passwd *passwd;	/* Password entry for caller */
	char *userqueue;	/* User-defined destination queue */
	register int i;		/* Loop var */
#ifdef SDSC
        int     euid;
        int     ruid;
#endif

	*ptrfile = (FILE *) 0;			/* No file yet */
#ifdef SDSC
	/* set the euid to the real user so that it can access NFS mounted
	 * wdir 
	 */
        ruid = getuid();
        euid = geteuid();
	if (seteuid(ruid) == -1)
		return (MKREQ_NOCWD);
#endif
	if ((*workdir = getwdir()) == (char *) 0) {
		/*
		 *  Unable to determine the caller's working directory.
		 */
		return (MKREQ_NOCWD);
	}
#ifdef SDSC
	/* set the euid to the original value. */

        if (seteuid(euid) == -1)
                return (MKREQ_NOCWD);
#endif


#if	BSD42 | BSD43 | ULTRIX
	if (index (*workdir, '\n') != (char *) 0) {
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if (strchr (*workdir, '\n') != (char *) 0) {
#else
BAD SYSTEM TYPE
#endif
#endif
		/*
		 *  A newline was located in the current working directory.
		 */
		return (MKREQ_CWDNEWLINE);
	}
	if (chdir (Nqs_requests) == -1) {
		/*
		 *  Unable to chdir() to the NQS new request directory.
		 */
		return (MKREQ_NOCHDIRNEW);
	}
	/*
	 *  The caller would like us to initialize the request
	 *  structure.
	 */
	if (nmap_get_mid ((struct hostent *) 0, local_mid) != NMAP_SUCCESS) {
		/*
		 *  We could not get the machine-id of the local
		 *  machine.
		 */
		return (MKREQ_NOMID);
	}
	if (reqtype != RTYPE_BATCH && reqtype != RTYPE_DEVICE) {
		/*
		 *  Invalid request type.
		 */
		return (MKREQ_BADREQTYPE);
	}
	passwd = fetchpwuid ((int) getuid());
	closepwdb();				/* Close account/password */
						/* database */
	if (passwd == (struct passwd *) 0) {
		/*
		 *  Could not get username from effective
		 *  user-id.
		 */
		return (MKREQ_NOUSERNAME);
	}
	/*
	 *  Open the NQS general parameters file.
	 */
	sprintf (path, "%s/%s", Nqs_root, Nqs_params);
	if ((paramfile = opendb (path, O_RDONLY)) == (struct confd *) 0) {
		/*
		 *  Unable to open the NQS general parameters file.
		 */
		return (MKREQ_NOPARMFILE);
	}
	/*
	 * Get a fifo pipe to the local daemon.
	 * (On systems with named pipes, we do this automatically
	 * inside inter().)
	 */
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
#if	BSD42 | BSD43
	if (interconn () < 0) {
		return (MKREQ_NOLOCALDAE);
	}
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  We must now give up our set[uid,gid] privileges because the
	 *  control file MUST be created with the correct user and group
	 *  id of the caller.
	 */
#if UNICOS & NEWPWD
	if (acctid (0,passwd->pw_acid[0]) == -1) {
		/*
		 *  Invalid account
		 */
		return (MKREQ_INVALIDACC);	
	}
#endif
	if (setuid (getuid()) == -1 || setgid (getgid()) == -1) {
		/*
		 *  Unable to set[uid,gid] to real values.
		 */
		return (MKREQ_NOSETUGID);
	}
	rawreq->magic1 = REQ_MAGIC1;		/* Request magic# */
	rawreq->trans_id = 0;			/* No transaction descriptor */
#ifdef SDSC
	rawreq->period_inx = 0;
	rawreq->cube_account = 0; 		/* default account */
	rawreq->major_nodes = 0;
#else  
	rawreq->reserved1 = 0;			/* Set RESERVED fields to */
	rawreq->reserved2 = 0;			/* zero (0) */
	rawreq->reserved3 = 0;
#endif
	rawreq->node_group = 0;
	rawreq->reserved5 = 0;
	rawreq->reserved6 = 0;
	strncpy (rawreq->trans_quename, "", MAX_QUEUENAME+1);
						/* No transport queue */
						/* (set ALL zero */
	time (&rawreq->create_time);		/* Time at which req */
						/* was created */
	rawreq->enter_time=rawreq->create_time;	/* Time at which req */
						/* entered the queue */
	strncpy (rawreq->quename, "", MAX_QUEUENAME+1);
						/* Containing queue is */
						/* unknown (set ALL zero) */
	rawreq->type = reqtype;			/* Set req type */
	rawreq->orig_uid = getuid();		/* Real user-id */
	rawreq->orig_mid = *local_mid;		/* Machine-id of owner */
	rawreq->tcm = TCML_UNDEFINED;		/* No TCM yet assigned */
	rawreq->orig_seqno = -1;		/* No seq# yet assigned */
	rawreq->rpriority = -1;			/* Let NQS determine */
						/* priority by default */
	rawreq->flags = RQF_RECOVERABLE;	/* By default, request */
						/* is restartable */
	rawreq->flags = RQF_RESTARTABLE;	/* By default, request */
						/* is restartable */
	rawreq->start_time = 0;			/* As soon as possible */
	rawreq->ndatafiles = 0;			/* Number of data files=0 */
	rawreq->reqname [0] = '\0';		/* No reqname yet */
	strncpy (rawreq->username, passwd->pw_name, MAX_ACCOUNTNAME+1);
						/* Preserve user name */
	strncpy (rawreq->mail_name, passwd->pw_name, MAX_ACCOUNTNAME+1);
						/* and for sending mail */
	rawreq->mail_mid = *local_mid;		/* Machine-id for mail */
	if (reqtype == RTYPE_DEVICE) {
		/*
		 *  The request is going to be a device oriented request.
		 */
		if ((userqueue = getenv ("QPR_QUEUE")) == (char *) 0) {
			/*
			 *  No user defined print queue exists.
			 *  Scan the NQS general parameters file for the
			 *  default print-queue name.
			 */
			descr = nextdb (paramfile);
			while (descr != (struct gendescr *)0) {
				if (descr->v.par.paramtype == PRM_GENPARAMS) {
				    strncpy (rawreq->quename,
					    descr->v.par.v.genparams.defprique,
					    MAX_QUEUENAME+1);
					break;
				}
				descr = nextdb (paramfile);
			}
		}
		else {
			/*
			 *  The user has defined a default queue in their
			 *  environment appropriate for the request type.
			 */
			strncpy (rawreq->quename, userqueue, MAX_QUEUENAME+1);
		}
		strncpy (rawreq->v.dev.forms, "", MAX_FORMNAME+1);
						/* No forms; set all zero */
		rawreq->v.dev.copies = 1;	/* Default 1 copy */
		rawreq->v.dev.reserved1 = 0;	/* These MUST be zero */
		rawreq->v.dev.reserved2 = 0;	/* at present */
		for (i = 0; i < MAX_DEVPREF; i++) {
			strncpy (rawreq->v.dev.devprefname [i], "",
				 MAX_DEVNAME+1);
			rawreq->v.dev.devprefmid [i] = *local_mid;
		}
		rawreq->v.dev.size = 0;		/* Print size unknown */
	}
	else {
		/*
		 *  The request is going to be a batch request.
		 */
		if ((userqueue = getenv ("QSUB_QUEUE")) == (char *) 0) {
			/*
			 *  No user defined batch queue exists.
			 *  Scan the NQS general parameters file for the
			 *  default batch-queue name.
			 */
			descr = nextdb (paramfile);
			while (descr != (struct gendescr *)0) {
				if (descr->v.par.paramtype == PRM_GENPARAMS) {
				    strncpy (rawreq->quename,
					    descr->v.par.v.genparams.defbatque,
					    MAX_QUEUENAME+1);
					break;
				}
				descr = nextdb (paramfile);
			}
		}
		else {
			/*
			 *  The user has defined a default queue in their
			 *  environment appropriate for the request type.
			 */
			strncpy (rawreq->quename, userqueue, MAX_QUEUENAME+1);
		}
		/*
		 *  Get/set umask.
		 *
		 *  WARNING:  It is critical that the file-creation mask
		 *	      be set to zero so that the 744 protections
		 *	      of the control file, and the 500 protections
		 *	      on any data files are exactly what we say
		 *	      they should be.
		 */
		rawreq->v.bat.umask = umask (0);
		strncpy (rawreq->v.bat.shell_name, "", MAX_SHELLNAME+1);
						/* No shell specified; set */
						/* all zero */
		/*
		 *  Below, set default so Qsub only has to turn bits on,
		 *  not off.
		 */
		rawreq->v.bat.explicit = 0;
		rawreq->v.bat.infinite = LIM_PPCORE | LIM_PPDATA | LIM_PPPFILE |
			LIM_PRPFILE | LIM_PPQFILE | LIM_PRQFILE | LIM_PPTFILE |
			LIM_PRTFILE | LIM_PPMEM | LIM_PRMEM | LIM_PPSTACK |
			LIM_PPWORK | LIM_PPCPUT | LIM_PRCPUT | LIM_PPNICE |
			LIM_PRDRIVES | LIM_PRNCPUS;
		/*
		 *  It is necessary to place values into the limits
		 *  fields that will not cause an error when read
		 *  by ../lib/readreq.c.
		 */
		set_default_quota (&rawreq->v.bat.ppcoresize);
		set_default_quota (&rawreq->v.bat.ppdatasize);
		set_default_quota (&rawreq->v.bat.pppfilesize);
		set_default_quota (&rawreq->v.bat.prpfilespace);
		set_default_quota (&rawreq->v.bat.ppqfilesize);
		set_default_quota (&rawreq->v.bat.prqfilespace);
		set_default_quota (&rawreq->v.bat.pptfilesize);
		set_default_quota (&rawreq->v.bat.prtfilespace);
		set_default_quota (&rawreq->v.bat.ppmemsize);
		set_default_quota (&rawreq->v.bat.prmemsize);
		set_default_quota (&rawreq->v.bat.ppstacksize);
		set_default_quota (&rawreq->v.bat.ppworkset);
		rawreq->v.bat.ppcputime.max_seconds = 0;
		rawreq->v.bat.ppcputime.max_ms = 0;
		rawreq->v.bat.ppcputime.warn_seconds = 0;
		rawreq->v.bat.ppcputime.warn_ms = 0;
		rawreq->v.bat.prcputime.max_seconds = 0;
		rawreq->v.bat.prcputime.max_ms = 0;
		rawreq->v.bat.prcputime.warn_seconds = 0;
		rawreq->v.bat.prcputime.warn_ms = 0;
		rawreq->v.bat.ppnice = 0;
		rawreq->v.bat.prdrives = 0;
		rawreq->v.bat.prncpus = 0;
		/*
		 *  Clear request precedence fields.
		 */
		for (i = 0; i < MAX_PREDECESSOR; i++) {
			strncpy (rawreq->v.bat.predecessors [i], "",
				 MAX_REQNAME+1);
		}
		/*
		 *  Initialize the output return fields appropriately.
		 */
		rawreq->v.bat.stderr_acc = OMD_SPOOL;
		rawreq->v.bat.stdlog_acc = OMD_SPOOL;
		rawreq->v.bat.stdout_acc = OMD_SPOOL;
		rawreq->v.bat.stderr_mid = *local_mid;
		rawreq->v.bat.stdlog_mid = *local_mid;
		rawreq->v.bat.stdout_mid = *local_mid;
		strncpy (rawreq->v.bat.stderr_name, "", MAX_REQPATH+1);
		strncpy (rawreq->v.bat.stdlog_name, "", MAX_REQPATH+1);
		strncpy (rawreq->v.bat.stdout_name, "", MAX_REQPATH+1);
		rawreq->v.bat.instacount = 0;
		rawreq->v.bat.oustacount = 0;
		rawreq->v.bat.instahiermask = 0;
		rawreq->v.bat.oustahiermask = 0;
		for (i = 0; i < MAX_INSTAPERREQ; i++) {
			rawreq->v.bat.instamid[i] = 0;
		}
		for (i = 0; i < MAX_OUSTAPERREQ; i++) {
			rawreq->v.bat.oustamid[i] = 0;
		}
	}
	closedb (paramfile);
	/*
	 *  Now, construct the name of the control file as it will exist
	 *  in the NQS new request staging directory hierarchy.
	 *
	 *  The name is constructed by using:
	 *
	 *	o The 32-bit time value as the the number of seconds since
	 *	  0:00:00.00 January 1, 1970 GMT.
	 *	o The low-order 30-bits of the calling process' process-id.
	 *
	 *  For a collision to occur, two or more processes must attempt
	 *  to create a control file in the same 1-second time quantum which
	 *  also have the same process-ids (taken modulo 2^30).
	 *
	 *  Since the submission of a batch request to the NQS daemon cannot
	 *  complete (the calling process is blocked), until the NQS daemon
	 *  queues the request and unlinks the request files from the new
	 *  request directory hierarchy, the submission of a steady stream
	 *  of reqs (even when submitted from the SAME process), cannot run
	 *  into trouble.
	 */
	pack6name (ctrlname, (char *) 0, -1, (char *) 0,
		  (long) time ((time_t *) 0), 6,
		  (long) getpid(), 5, 0, 0);
	if ((ctrlfile=open (ctrlname, O_RDWR | O_CREAT | O_EXCL, 0744)) == -1) {
		/*
		 *  Unable to create control file.
		 */
		return (MKREQ_NOCREATE);
	}
	if ((*ptrfile = fdopen (ctrlfile, "w+")) == (FILE *) 0) {
		return (MKREQ_NOCREATE);
	}
	return (MKREQ_SUCCESS);
}


/*** mkdata
 *
 *
 *	int mkdata():
 *
 *	Create a data file in the new request staging directory hierarchy
 *	opened for writing for the caller.
 *	
 *	WARNING/NOTE:
 *
 *		The current directory of the caller MUST BE
 *		the NQS new request staging directory!
 *
 *		It is critical that the effective user-id and
 *		effective group-id of the caller be the values
 *		for the req owner.  This is crucial for the
 *		NQS utilities which allow users to modify and
 *		delete their requests.  The owner of the control
 *		and data files describing a req MUST be the
 *		mapped submitter's user-id.
 *
 *	Returns:
 *		0 if the data file successfully created and opened
 *		  for writing, in which case *ptrfile is returned
 *		  as the open FILE structure providing access to
 *		  the newly created data file.
 *	       -1 if open error or fdopen() error.
 *	       -2 if too many data files would exist.
 */
int mkdata (ptrfile)
FILE **ptrfile;			/* Ptr to FILE structure for data file */
{
	char path [MAX_PATHNAME+1];		/* Pathname of data file */
	int fd;

	*ptrfile = (FILE *) 0;			/* No file yet */
	if (n_datafiles >= 32767) {
		/*
		 *  A maximum of 32767 data files may be created.
		 */
		return (-2);
	}
	pack6name (path, (char *) 0, -1, ctrlname, 0L, 0, 0L, 0,
		  (int) n_datafiles, 3);
	if ((fd = open (path, O_WRONLY | O_CREAT | O_EXCL, 0500)) == -1) {
		/*
		 *  Unable to create data file.
		 */
		return (-1);
	}
	if ((*ptrfile = fdopen (fd, "w")) == (FILE *) 0) return (-1);
	n_datafiles++;			/* One more data file */
	return (0);			/* Success */
}


/*** zapreq
 *
 *
 *	void zapreq():
 *
 *	Unlink the control file, and ALL data files associated with
 *	the request that was under construction (or was successfully
 *	submitted), from the NQS new request staging directory.
 *
 *	WARNING/NOTE:
 *
 *		The current directory of the caller MUST BE
 *		the NQS new request staging directory!
 */
void zapreq()
{
	register int i;
	char path [MAX_PATHNAME+1];	/* Pathname of data file */

	if (ctrlfile != -1) {
		/*
		 *  We have a control file and possible data file(s).
		 */
		unlink (ctrlname);	/* Unlink the control file name */
		/*
		 *  Loop to discard all data files associated with the req
		 *  that was under construction.
		 */
		for (i=0; i < n_datafiles; i++) {
			pack6name (path, (char *) 0, -1, ctrlname, 0L, 0,
				   0L, 0, (int) i, 3);
			unlink (path);	/* Discard a control file */
		}
	}
}


/*** quereq
 *
 *
 *	long quereq():
 *
 *	Send a queue request to the NQS daemon.
 *	This function should be invoked when ALL of the files
 *	describing the req have been constructed in the NQS
 *	new request staging directory.
 *
 *	ANOTHER WARNING/NOTE:
 *
 *		The current directory of the caller MUST BE
 *		the NQS new request staging directory!
 *
 *	Returns:
 *	      >=0: if successful, in which case the value returned
 *		   is the sequence number assigned to the request
 *		   (*explain refers to an explanatory text string
 *		   saying that the request was successfully queued.
 *	       -1: if an error occurred submitting the req and the
 *		   local NQS daemon was running.  An example of such
 *		   an error would be submitting the req to a non-existent
 *		   queue (*explain references diagnostic text).  ALL
 *		   request files describing the request are deleted.
 *	       -2: if the local NQS daemon is not running (*explain
 *		   references diagnostic text).  ALL request files 
 *		   describing the request are deleted.
 */
long quereq (transaction_code)
long *transaction_code;			/* Transaction completion code */
{
	struct rawreq rawreq;		/* Raw request structure for just */
					/* submitted request */

	/*
	 *  Ignore all of the usual signals to kill a process at this
	 *  point.  We want to make sure that we survive to tell the
	 *  user the request-id of the queue request.
	 */
	signal (SIGHUP, SIG_IGN);
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGTERM, SIG_IGN);
	/*
	 *  Now, let's talk to the daemon.
	 */
	interclear();			/* Clear inter-process communication */
					/* message contents */
	interwstr (ctrlname);		/* Squirrel away the name of the */
					/* control file */
	if (inter (PKT_QUEREQ) == TCML_COMPLETE) {
		/*
		 *  The NQS daemon has responded to our request.
		 *  We are not yet sure of any more than that.  If
		 *  the req has been successfully submitted, then the
		 *  NQS daemon has linked the control file and ALL data
		 *  files for the req into its own private control and
		 *  data directories.
		 */
		lseek (ctrlfile, 0L, 0);	/* Seek to beginning of file */
		if (readhdr (ctrlfile, &rawreq) == -1 ||
		   (rawreq.tcm & XCI_FULREA_MASK) != TCML_SUBMITTED) {
			if ((rawreq.tcm & XCI_FULREA_MASK) == TCML_UNDEFINED) {
				/*
				 *  The NQS daemon could not open the control
				 *  file at all (and therefore could not set
				 *  a request completion code).
				 */
				rawreq.tcm = TCML_UNAFAILURE; /* Very bad news */
			}
			close (ctrlfile);		/* Close control file */
			zapreq();			/* Delete the request*/
			*transaction_code = rawreq.tcm;	/* Return code */
			return (-1);			/* Req not submitted */
		}
		/*
		 *  The request was successfully queued.
		 */
		close (ctrlfile);		/* Close control file */
		if (rawreq.orig_seqno > MAX_SEQNO_USER) {
			/*
			 *  The NQS daemon failed to set a valid
			 *  sequence number.
			 */
			zapreq();			/* Delete the request*/
			*transaction_code = TCML_UNAFAILURE;
			return (-1);			/* Req not submitted */
		}
		/*
		 *  Otherwise, the request was successfully submitted.
		 */
		*transaction_code = rawreq.tcm;	/* Completion code */
		return (rawreq.orig_seqno);	/* Success */
	}
	/*
	 *  The local NQS daemon is not running; therefore the request
	 *  cannot be queued and must be destroyed.
	 */
	zapreq();				/* Destroy the request */
	*transaction_code = TCML_NOLOCALDAE;	/* No daemon present */
	close (ctrlfile);			/* Close control file */
	return (-2);
}


/*** set_default_quota
 *
 *
 *	void set_default_quota():
 *
 *	Initialize a quota limit field to an appropriate default
 *	value that is valid when read by ../lib/readreq.c.
 */
static void set_default_quota (quotafield)
struct quotalimit *quotafield;			/* Field to be initialized */
{
	quotafield->max_units = QLM_BYTES;	/* Default units */
	quotafield->warn_units = QLM_BYTES;
	quotafield->max_quota = 0;		/* Default quota */
	quotafield->warn_quota = 0;
}

#if	SGI | SYS52 | UNICOS |  BSD42 | BSD43 | ULTRIX | OSF

 /*** mklink
  *
  *
  *    int mkdata():
  *
  *    Link a data file into the new request staging directory hierarchy
  *
  *    Returns:
  *            0 if the data file successfully linked
  *           -1 if symlink error.
  *           -2 if too many data files would exist.
  */
 int mklink (datapath)
 char *datapath;                       /* Ptr to pathname of data file */
 {
       char path [MAX_PATHNAME+1];             /* Pathname of data file */
       int i;

       if (n_datafiles >= 32767) {
               /*
                *  A maximum of 32767 data files may be created.
                */
               return (-2);
       }
       pack6name (path, (char *) 0, -1, ctrlname, 0L, 0, 0L, 0,
                 (int) n_datafiles, 3);
       i = umask(~0500);
       if (symlink (datapath, path) < 0) {
               /*
                *  Unable to symbolically link file
                */
               (void) umask (i);

               return (-1);
       }
       (void) umask(i);
       n_datafiles++;                  /* One more data file */
       return (0);                     /* Success */
 }

#endif


