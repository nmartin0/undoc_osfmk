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
 
/*++ nqs_ldconf.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_ldconf.c,v $
 *
 * DESCRIPTION:
 *
 *	Build the NQS operating configuration from the NQS configuration
 *	database.
 *
 *	WARNING/NOTE:
 *		This module invokes the function:
 *
 *			fetchpwuid()
 *
 *		which opens the account/password database on the local
 *		system, and keeps it open.  All subsequent calls to
 *		fetchpwuid() and fetchpwnam() will use the account/-
 *		password database that existed at the time NQS was
 *		booted.
 *
 *		This side-effect is bad in that NQS will NOT notice
 *		NEW (different i-node) /etc/passwd files created AFTER
 *		NQS was booted.  (However, if changes are made DIRECTLY
 *		to the /etc/passwd file--that is, the /etc/passwd file
 *		is not UNLINKED and replaced with a new one, then NQS
 *		WILL notice the changes.)
 *
 *		This side-effect is good in that NQS always has access
 *		to SOME version of the local account/password database,
 *		thus preventing a system-wide shortage in the file-
 *		table from affecting NQS when trying to access the
 *		account/password database.
 *
 *		This side-effect is also beneficial to the extent that
 *		NQS does not have to reopen the account/password data-
 *		base every single time that an account entry must be
 *		gotten from a user-id, or username.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 7, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1995/02/24 23:42:28 $ $State: Exp $)
 * $Log: nqs_ldconf.c,v $
 * Revision 1.5  1995/02/24  23:42:28  kremenek
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 5134
 *  Testing: Developer testing
 *  Module(s):      ./cmds_libs/src/usr/bin/qmgr/mgr_cmd.c
 *                 ./cmds_libs/src/usr/bin/qmgr/mgr_main.c
 *                 ./cmds_libs/src/usr/bin/qmgr/mgr_packet.c
 *                 ./cmds_libs/src/usr/bin/qmgr/qmgr.hlp
 *                 ./cmds_libs/src/usr/ccs/lib/libnqs/listq.c
 *                 ./cmds_libs/src/usr/include/nqs/nqspacket.h
 *                 ./cmds_libs/src/usr/include/nqs/nqsstruct.h
 *                 ./cmds_libs/src/usr/lib/nqs/macs_sched.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_ldconf.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_main.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_nsq.c
 *                 ./cmds_libs/src/usr/lib/nqs/nqs_upq.
 *
 * Revision 1.4  1994/11/19  02:52:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:57:15  mwan
 * R1.2 mods
 *
 * Revision 1.2  1992/10/09  22:25:36  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:09  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:06:22  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_ldconf.c	1.4 (nqs_ldconf.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include "nqs.h"
#include "nqsmail.h"			/* Mail log file structures */
#include "nqsxvars.h"			/* External global variables */
#include "transactcc.h"			/* Transaction completion codes */
#include <pwd.h>			/* Password file stuff */
#include <sys/stat.h>			/* Include after nqs.h which */
					/* includes sys/types.h */

#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <sys/lock.h>
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif

/*
 *
 *	External functions and variables.
 */
extern int errno;			/* System call error record */
extern struct passwd *fetchpwuid();	/* Fetch password entry for user-id */
extern char *fmtmidname();		/* Format name for machine-id */
extern void ldparam();			/* Load general operating parameters */
extern struct gendescr *nextdb();	/* Get next allocated descriptor from */
					/* an NQS database file */
extern void nqs_abort();		/* Abort NQS execution */
extern int nqs_enfile();		/* Diagnose ENFILE or EMFILE */
extern int set_mtime();			/* Set file modification time */
					/* synchronously */
extern void sync();			/* Schedule all dirty I/O buffers */
					/* for writing */
extern long telldb();			/* Tell offset of last read record */
extern long upc_addquecom();		/* Add a queue to a queue complex */
extern long upc_crecom();		/* Create a queue complex */
extern void udb_genparams();		/* Update NQS general operating */
					/* parameters in database */
extern void udb_netprocs();		/* Update network processes defn */
					/* record in the NQS database */
extern void udb_setlogfil();		/* Update logfile record in the */
					/* parameters database */
extern long upd_addquedes();		/* Add a queue destination */
extern struct pipeto *upd_credes();	/* Create a queue destination */
extern void upd_deldes();		/* Delete a queue destination */
extern long upm_addnqsman();		/* Add an NQS manager */
extern long upp_setlogfil();		/* Set NQS logfile */
extern long upq_creque();		/* Create a queue */
extern long upq_lowaddacc();		/* Add queue access */
extern long upv_addquedev();		/* Add a queue-to-device mapping */
extern long upv_credev();		/* Create a device */


/*** nqs_ldconf
 *
 *
 *	void nqs_ldconf():
 *
 *	Build the NQS operating configuration from the NQS configuration
 *	database.
 *
 *	WARNING:  It is assumed that the database files have just been
 *		  opened, and that the file pointer for all of the
 *		  appropriate database files is therefore at offset 0.
 */
void nqs_ldconf()
{
	void initseqno();			/* Initialize seq# */
	void ldcomplex();

	struct rawreq rawreq;			/* For ATOMICBLKSIZ checking */
						/* purposes */
	struct stat stat_buf;			/* Stat() buffer */
	register int i;				/* Loop counter and scratch */
	register struct pipeto *dest;		/* Destination set walking */
	register int residual;			/* Residual byte count */

	/*
	 *  Check to see that a single instance of any NQS database
	 *  structure in the queue or device descriptor file will fit
	 *  within a single block of size ATOMICBLKSIZ.  This constraint
	 *  must be satisfied so that updates to the NQS status and
	 *  configuration database files will happen atomically.
	 */
	i = sizeof (struct gendescr);
	residual = i % sizeof (ALIGNTYPE);
	if (residual) i += sizeof (ALIGNTYPE) - residual;
	if (i > ATOMICBLKSIZ) {
		printf ("F$Configuration database ");
		printf ("descr size exceeds ATOMICBLKSIZ.\n");
		errno = 0;			/* Not a system call error */
		nqs_abort();
	}
	/*
	 *  Check to see that mail request log files will not exceed a
	 *  size of ATOMICBLKSIZ.  This is important.  Without this
	 *  constraint, detection of un-updated mail request log file
	 *  information (because of a system crash for example), is much
	 *  more difficult.
	 */
	i = sizeof (struct requestlog);
	residual = i % sizeof (ALIGNTYPE);
	if (residual) i += sizeof (ALIGNTYPE) - residual;
	if (i > ATOMICBLKSIZ) {
		printf ("F$Mail request log structure");
		printf ("size exceeds ATOMICBLKSIZ.\n");
		errno = 0;			/* Not a system call error */
		nqs_abort();
	}
	/*
	 *  Check to see that the portions of an NQS control file that
	 *  must fit in the first block of the file, do so, for atomic
	 *  indivisible Qmod updates.
	 */
	if (((char *) &rawreq.v.bat.stderr_mid) -
	    ((char *) &rawreq.magic1) > ATOMICBLKSIZ ||
	    ((char *) &rawreq.v.dev) -
	    ((char *) &rawreq.magic1) +
	    sizeof (struct rawdevreq) > ATOMICBLKSIZ) {
		printf ("F$Control-file header exceeds ATOMICBLKSIZ\n");
		printf ("F$requirement.\n");
		errno = 0;			/* Not a system call error */
		nqs_abort();
	}
	/*
	 *  Check to see that maximum number of externally queued requests
	 *  cannot exceed the total number of requests that can be queued
	 *  at this machine.
	 */
	if (MAX_EXTREQUESTS > MAX_TRANSACTS) {
		printf ("F$MAX_EXTREQUESTS > MAX_TRANSACTS.\n");
		errno = 0;			/* Not a system call error */
		nqs_abort();
	}
	/*
	 *  Mark every entry in the Runvars array as unallocated.
	 */
	for (i = 0; i < MAX_GBLRUNLIMIT; i++) {
		Runvars [i].allocated = 0;
	}
	/*
	 *  Record the fact there are no NQS daemon spawned server
	 *  processes running at this moment.
	 */
	Gblbatcount = 0;
	Gblnetcount = 0;
	Gblpipcount = 0;
	/*
	 *  Record that at the present time, no requests are queued, and
	 *  therefore the number of requests that have been seen to arrive
 	 *  from external sources (at this juncture in the NQS boot sequence)
	 *  is therefore zero (0).
	 */
	Extreqcount = 0;
	/*
	 *  Record that no device descriptions have been loaded yet.
	 */
	Noofdevices = 0;
	/*
	 *  Enforce the presence of the system "root" account as
	 *  a fully privileged NQS manager account.
	 */
	upm_addnqsman ((uid_t) 0, (mid_t) Locmid, ~0);	/* All privileges on */
	/*
	 *  Build the name-ordered device set.
	 */
	Devset = (struct device *)0;
	while (nextdev() == 0)
		;
	/*
	 *  Build the name-ordered queue set.
	 */
	Net_queueset = (struct queue *)0;	/* No network queues */
	Nonnet_queueset = (struct queue *)0;	/* No non-net queues either */
	Pribatqueset = (struct queue *) 0;	/* Priority ordered batch */
						/* queue set is empty */
	Prinetqueset = (struct queue *) 0;	/* Priority ordered network */
						/* queue set is empty */
	Pripipqueset = (struct queue *) 0;	/* Priority ordered pipe */
						/* queue set is empty */
	while (nextque() == 0)
		;

	/*
	 *  Build the queue complex structures and link associated batch queues
	 *  to them.
	 */
	Qcomplexset = (struct qcomplex *)0;
	ldcomplex() ;

	/*
	 *  Build the pipe queue destination set.
	 */
	Pipetoset = (struct pipeto *) 0;	/* No destinations */
	while (nextdest() == 0)
		;
	/*
	 *  Build the device to device-queue, device-queue to device,
	 *  and pipe-queue to destination mappings.
	 */
	while (nextqmap() == 0)
		;
	/*
	 *  Discard unreferenced pipe queue destinations (this can happen
	 *  if the system crashes during the middle of a database update).
	 */
	dest = Pipetoset;
	while (dest != (struct pipeto *) 0) {
		if (dest->refcount == 0) {
			/*
			 *  This destination is not referenced by any
			 *  pipe queue.  Delete it.
			 */
			upd_deldes (dest);	/* Delete both in memory and */
		}				/* in NQS database image */
		dest = dest->next;		/* Step to next destination */
	}
	/*
	 *  Read and process the general NQS operating parameters
	 *  from the NQS parameter file.
	 */
	ldparam();				/* Load general parameters */
	/*
	 *  Verify several of the parameters, and do some general
	 *  housekeeping.
	 */
	if (Logfilename [0] != '\0') {
		if (upp_setlogfil (Logfilename) != TCML_COMPLETE) {
			/*
			 *  Unable to open the configured logfile.
			 */
			printf ("F$Unable to open logfile: %s.\n",
				Logfilename);
			nqs_abort();
		}
	}
	if (fetchpwuid ((int) Mail_uid) == (struct passwd *) 0) {
		/*
		 *  Egads!  No password file entry for Mail user-id!
		 */
		printf ("F$No password entry for NQS ");
		printf ("daemon mail user-id: %1d.\n", (int) Mail_uid);
		printf ("I$Password file must either be changed, or the\n");
		printf ("I$NQS params file must be removed before\n");
		printf ("I$NQS is rebooted.\n");
		errno = 0;			/* Not a system call error */
		nqs_abort();
	}
	if (Maxextrequests > MAX_EXTREQUESTS) {
		/*
		 *  NQS has been recompiled with a smaller Maxextrequests
		 *  limit.
		 */
		printf ("W$NQS has been recompiled with a smaller\n");
		printf ("W$maximum external request limit less than\n");
		printf ("W$the limit defined in the NQS database.\n");
		fflush (stdout);
		Maxextrequests = MAX_EXTREQUESTS;
	}
#if	SGI | SYS52 | UNICOS | UTS | OSF
	if (Plockdae) {
		/*
		 *  Lock ourselves in memory.
		 */
		if (plock (PROCLOCK) != 0) {
			/*
			 *  The plock() call failed!
			 */
			printf ("F$Plock() failure in nqs_ldconf.c.\n");
			nqs_abort();
		}
	}
#else
#if	BSD42 | BSD43 | ULTRIX
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Force the general operating parameters record, logfile record,
	 *  and network server definition record to be the first three (3)
	 *  records in the NQS parameters file (for efficiency reasons).
	 *  After these records comes a sequence of N network peer
	 *  records.
	 */
	if (Udbgenparams == -1) udb_genparams();/* Update database */
	if (Udblogfile == -1) udb_setlogfil();
	if (Udbnetprocs == -1) udb_netprocs();
	/*
	 *  Load (or initialize) the request sequence number file.
	 *  This is not an easy thing to do.  Read on.
	 *
	 *  Unfortunately, as of March 28, 1986, very few UNIX implementations
	 *  provided a synchronous write file I/O mode, where when the write(2)
	 *  system call returned, the file system had been PHYSICALLY updated!
	 *  This is why we jump through all of the crazy hoops below....
	 *
	 *  Note also that as soon as synchronous write I/O is supported (in
	 *  the sense described below, I strongly recommend that the heinous
	 *  method used to store the next available request sequence number
	 *  be abolished.  (Please note the existence of the long integers
	 *  RESERVED in a PRM_GENPARAMS record in the general NQS
	 *  operating parameters file--one of them is reserved for the
	 *  storage of the next available request sequence number--someday.)
	 *
	 *  We are a bit more paranoid in the storage of the next available
	 *  request sequence number, than we are with other NQS database
	 *  values....
	 *
	 *  It might seem reasonable for NQS to simply store the value of
	 *  the next available sequence number in a disk file (like most
	 *  everything else).  However, the request sequence number is a
	 *  very critical quantity.  No two requests submitted temporally
	 *  close together, must ever have the same sequence number for
	 *  rather obvious reasons.
	 *
	 *  Since the system could crash at any time, we need an absolutely
	 *  iron-clad way of making sure that the permanent memory of the
	 *  next available sequence number (as stored on disk), is physically
	 *  updated whenever the next available request sequence number is
	 *  changed.
	 *
	 *  However, the standard UNIX file system does NOT physically
	 *  perform disk I/O, until no more disk cache buffers are available
	 *  in the system to perform some new I/O request.
	 *
	 *  Furthermore, some UNIX implementations provide enormous buffer
	 *  caches containing upwards of 1000 disk cache buffers.  It can be a
	 *  very long time indeed before the action defined by a write(2)
	 *  system call takes place on the physical disk medium.  If few people
	 *  are logged onto the system, or I/O traffic is low, many minutes
	 *  can elapse before a given disk cache buffer allocated by a write(2)
	 *  system call is physically written to the disk.
	 *
	 *  There are however three exceptions to this situation.  First, many
	 *  UNIX implementations configure a 'sync' process that runs every
	 *  N seconds or minutes (or sometimes every N hours), that schedules
	 *  all written disk cache buffers for writing to the disk.  Shortly
	 *  thereafter, all of the allocated disk cache buffers are written
	 *  to disk.
	 *
	 *  Second, the link(2) system call in all UNIX implementations known
	 *  to the author (UNICOS, UTS, System V, Berkeley UNIX, and the UNIX
	 *  implementation running on the Silicon Graphics IRIS machines),
	 *  synchronously updates the target inode of the link(2) system call.
	 *  By synchronous, I mean that the link(2) system call does not return
	 *  until the modified inode of the target file (the file whose link
	 *  count is being increased), has been physically written to disk.
	 *  In this way, we can store a small amount of information in a file
	 *  inode structure and then make a link to the same file, forcing the
	 *  synchronous update the file inode.
	 *
	 *  The link(2) call is synchronous for at least the good reason that
	 *  the link count for an inode will always be greater than or equal to
	 *  the actual number of links to the file.  (The link count can be
	 *  greater than the number of actual links to the file if the system
	 *  crashes before the disk cache buffers associated with the new
	 *  directory entry are written to disk.)  The constraint on the link
	 *  count values of being greater than or equal to the actual number
	 *  of links to the file makes it possible for programs like 'fsck' to
	 *  work, rebuilding the file system after a system crash.
	 *	
	 *  A third method of avoiding the standard UNIX disk buffering
	 *  mechanism has been implemented in the UNICOS version of UNIX.
	 *  UNICOS provides an I/O mode that can be used on ordinary files to
	 *  read and write 4K bytes at a time, bypassing the standard buffer-
	 *  ing mechanism of the UNIX kernel.
	 *
	 *  Of these three alternatives, only one is portable and synchronous
	 *  to the extent that we do not have to wait N seconds after the
	 *  completion of the system call for permanent memory to record the
	 *  transaction state.
	 *
	 *  Thus, we heinously store the next available sequence number in
	 *  the MODIFICATION time of a separate file named:
	 *
	 *		../database/seqno
	 *
	 *  To synchronously update the inode of ../database/seqno, we
	 *  UNLINK a synchronous linkname reserved for the purpose of
	 *  updating ../database/seqno, call utime(../database/seqno,...)
	 *  as necessary, and then LINK the reserved link pathname to
	 *  ../database/seqno to force the synchronous writing of the
	 *  sequence number (as stored in the inode) to the disk medium.
	 *  It's a really awful thing to do, but it works.
	 *
	 */
	Seqno_user = 0;			/* Start at zero */
	if (stat (Nqs_seqno, &stat_buf) != -1) {
		/*
		 *  The stat() call was successful.  The ../database/seqno
		 *  file exists.
		 */
		if ((stat_buf.st_mode & 0777) == 0) {
			/*
			 *  The sequence number file was created by a
			 *  previous incarnation of NQS that crashed
			 *  before it could set the modification time
			 *  to contain the sequence number....
			 */
			initseqno ();	/* Initialize seq# file */
		}
		else Seqno_user = stat_buf.st_mtime;
	}
	else {
		/*
		 *  The sequence number file does not exist.  Create it
		 *  from scratch, setting the initial request sequence
		 *  number to zero (0).  Note that until we set the
		 *  modification time, the permissions of the file are
		 *  all turned off!  This is very important if we crash
		 *  AFTER creating the sequence number file, but BEFORE
		 *  setting the modification time....
		 */
		if (creat (Nqs_seqno, 0000) == -1) {
			/*
			 *  We were unsuccessful in our efforts to create
			 *  the sequence number file.
			 */
			printf ("F$Unable to create request seq# file.\n");
			nqs_abort();
		}
		/*
		 *  The sequence number file has been successfully created.
		 */
		sync();			/* Sync that sucker!  Make */
					/* SURE that the directory block */
					/* referring to the sequence */
					/* file inode gets written out. */
					/* This brings all system I/O */
					/* to a dead halt--but only */
					/* once--when NQS is first */
					/* run on this system.  All */
					/* future NQS reboots will not */
					/* need to execute this call. */
					/* Once the sequence file is */
					/* initialized or updated, the */
					/* sequence file must not */
					/* disappear!  This is */
					/* CRITICAL.  If you think you */
					/* can take this out, then you */
					/* don't understand the */
					/* seriousness of the problem */
		/*
		 *  Now, appropriately initialize the sequence number
		 *  file.
		 */
		initseqno ();
	}
}


/*** get_access
 *
 *
 *	void get_access():
 *
 *	Using the queue access file, get and configure
 *	the access list for a queue.
 *
 *	Returns: void
 */
static void get_access (queue)
struct queue *queue;
{
    void insuffmem();	/* Insufficient memory diagnostic */
    int fd;				/* File descriptor */
    char path [MAX_PATHNAME+1];		/* Queue access file */
    unsigned long buffer [QAFILE_CACHESIZ];
					/* Holds read() input */
    int done;				/* Boolean */
    int cachebytes;			/* Cache size in bytes */
    int bytes;				/* Bytes read this time */
    int entries;			/* Entries read this time */
    int i;				/* Loop variable */
    long upd;				/* Result of updating */
    
    if (queue->q.type == QUE_NET) {
	return;
    }
    if (queue->q.status & QUE_BYGIDUID) {
	sprintf (path, "%s/q%08x%04x", Nqs_qaccess,
		queue->queueno, queue->q.accessversion);
	if ((fd = open (path, O_RDONLY)) == -1) {
	    printf ("I$Cannot open queue access file for %s.\n",
				queue->q.namev.name);
	    fflush (stdout);
	    if (nqs_enfile ()) nqs_abort ();	/* File system problem */
	}
	else {
	    done = 0;
	    cachebytes = QAFILE_CACHESIZ * sizeof (unsigned long);
	    while ((bytes = read (fd, (char *) buffer, cachebytes)) > 0 &&
		   !done) {
		entries = bytes / sizeof (unsigned long);
		for (i = 0; i < entries && !done; i++) {
		    if (buffer [i] == 0) {		/* Zero means the */
							/* list has ended */
			done = 1;
		    }
		    else {
			/*
			 * Add this gid or uid to the access list
			 * using the low level function.
			 */
			upd = upq_lowaddacc (queue, buffer [i]);
			if (upd == TCML_INSUFFMEM) {
			    insuffmem ();
			    nqs_abort ();		/* Exit */
			}
			else if (upd == TCML_ALREADACC) {
			    printf ("I$Access set duplicates for queue %s;\n",
				    queue->q.namev.name);
			    if (buffer [i] & MAKEGID) {
				printf ("I$Group-id %d.\n",
					buffer [i] & ~MAKEGID);
			    }
			    else printf ("I$User-id %d.\n", buffer [i]);
			    fflush (stdout);
			}
			else if (upd != TCML_COMPLETE) {
			    printf ("I$Access set load error for queue %s.\n",
				    queue->q.namev.name);
			    fflush (stdout);
			    nqs_abort ();
			}
		    }
		}
	    }
	}
    }
}


/*** initseqno
 *
 *
 *	void initseqno():
 *	Initialize the already created sequence number file.
 */
static void initseqno ()
{
	if (setmtime (Nqs_seqno, (unsigned long) Seqno_user) == -1 ||
	    chmod (Nqs_seqno, 0777) == -1) {
		/*
		 *  We were unable to synchronously set the modification
		 *  time of the sequence number file:  ../database/seqno,
		 *  and/or to change its protection bits to indicate
		 *  valid initialization.
		 */
		printf ("F$Unable to initialize request seq# file.\n");
		nqs_abort();
	}
}


/*** insuffmem
 *
 *
 *	void insuffmem():
 *	Issue an "Insufficient memory" diagnostic and flush.
 */
static void insuffmem()
{
	printf ("F$Insufficient memory.\n");
	fflush (stdout);
}


/*** ldcomplex
 *
 *
 *	void ldcomplex():
 *
 *	Load the queue complex structures from the database.
 */

static void ldcomplex()
  {
  	struct	gendescr *descr;
  	long	result;
  	register int	i;
  
   	for (;;) {
  		descr = nextdb(Qcomplexfile);
  		if (descr == (struct gendescr *)0) break;
  		result = upc_crecom(descr->v.qcom.name);
  		if (result != TCML_COMPLETE) {
  			printf("F$nqs_ldconf: error creating queue complex; ");
  			printf("tcm = %#lo\n", result);
  			fflush(stdout);
  			errno = 0;		 /* Not a system call error  */
  			nqs_abort();
  		}
  		New_qcomplex->qcomplexno = telldb (Qcomplexfile);	/* CERN Boissat */
		result = upc_setcomlim(descr->v.qcom.name,
  			descr->v.qcom.runlimit);
  		if (result != TCML_COMPLETE) {
  			printf("F$nqs_ldconf: error setting queue complex ");
  			printf("runlimit; tcm = %#lo\n", result);
  			fflush(stdout);
  			errno = 0;		 /* Not a system call error  */
  			nqs_abort();
  		}
  		for (i = 0; i < MAX_QSPERCOMPLX; i++) {
  			if (descr->v.qcom.queues[i][0] == '\0') continue;
  			result = upc_addquecom(descr->v.qcom.queues[i],
  				descr->v.qcom.name);
  			if (result == TCML_COMPLETE) continue;
  			printf("E$nqs_ldconf: error adding queue ");
  			printf("to queue complex; tcm = %#lo\n",result);
  			fflush(stdout);
  		}
  	}
  }

 /*** nextcomplex
  *
  *
  *    int nextcomplex():
  *
  *    Get and configure the next queue complex from the next complex
  *    descriptor in the complex description file.
  *
  *    Returns:
  *            0: if successful;
  *           -1: if no more complex descriptors exist in the
  *               database;
  */
 static int nextcomplex()
 {
       register struct gendescr *gendescr;
       register long result;
       int queno;

       gendescr = nextdb (Qcomplexfile);
       if (gendescr == (struct gendescr *)0) return (-1);
       /*
        *  We successfully read a generic descriptor containing a
        *  complex descriptor.
        */
       result = upc_crecom (gendescr->v.qcom.name);
       if (result == TCML_COMPLETE) {
               /*
                *  The device was successfully created.
                *  Finish initialization and add the rest of the
                *  queues to the complex.
                */
		result= upc_setcomlim(gendescr->v.qcom.name,
               		gendescr->v.qcom.runlimit);
               New_qcomplex->qcomplexno = telldb (Qcomplexfile);
               New_qcomplex->runlimit = gendescr->v.qcom.runlimit;
               for (queno = 1; gendescr->v.qcom.queues[queno][0] != '\0';
                       queno++) {
                 result = upc_addquecom (gendescr->v.qcom.name,
                       gendescr->v.qcom.queues[queno]);
                 if (result == TCML_COMPLETE)
                       continue;
                 else if (result == TCML_ALREADEXI) {
                       printf ("F$Queue already exists in complex.\n");
                       printf ("I$Qcomplex = %s.\n", gendescr->v.qcom.name);
                       errno = 0;      /* Not a system error */
                 } else if (result == TCML_INSUFFMEM) {
                       insuffmem();            /* Insufficient memory */
                 } else if (result == TCML_NOSUCHCOM) {
                       printf ("F$Qcomplex does not exist.\n");
                       printf ("I$Qcomplex = %s.\n", gendescr->v.qcom.name);
                       errno = 0;      /* Not a system error */
                 } else if (result == TCML_NOSUCHQUE) {
                       printf ("F$Queue participant does not exist.\n");
                       printf ("I$Qcomplex = %s.\n", gendescr->v.qcom.name);
                       errno = 0;              /* Not a system call error */
                 } else if (result == TCML_TOOMANCOM) {
                       printf ("F$Too many complexes listed for queue.\n");
                       printf ("I$Queue = %s.\n", gendescr->v.qcom.queues[0]);
                       errno = 0;              /* Not a system call error */
                 } else if (result == TCML_TOOMANQUE) {
                       printf ("F$Too many complexes listed for queue.\n");
                       printf ("I$Queue = %s.\n", gendescr->v.qcom.queues[0]);
                       errno = 0;              /* Not a system call error */
                 } else if (result == TCML_WROQUETYP) {
                       printf ("F$Wrong queue type for a complex.\n");
                       printf ("I$Queue = %s.\n", gendescr->v.qcom.queues[0]);
                       errno = 0;              /* Not a system call error */
                 }
                 nqs_abort();
               }
               return (0);
       }
       else if (result == TCML_ALREADEXI) {
               printf ("F$Multiple database entries for a complex.\n");
               printf ("I$Qcomplex = %s.\n", gendescr->v.qcom.name);
               errno = 0;              /* Not a system call error */
       }
       else if (result == TCML_NOSUCHQUE) {
               printf ("F$Queue participant does not exist.\n");
               printf ("I$Qcomplex = %s.\n", gendescr->v.qcom.name);
               errno = 0;              /* Not a system call error */
       }
       else if (result == TCML_TOOMANCOM) {
               printf ("F$Too many complexes listed for queue.\n");
               printf ("I$Queue = %s.\n", gendescr->v.qcom.queues[0]);
               errno = 0;              /* Not a system call error */
       }
       else if (result == TCML_WROQUETYP) {
               printf ("F$Wrong queue type for a complex.\n");
               printf ("I$Queue = %s.\n", gendescr->v.qcom.queues[0]);
               errno = 0;              /* Not a system call error */
       }
       else if (result == TCML_INSUFFMEM) {
               insuffmem();            /* Insufficient memory */
       }
       nqs_abort();                    /* Abort NQS execution */
 }


/*** nextdest
 *
 *
 *	int nextdest():
 *
 *	Get the next queue to remote queue and host pipe mapping
 *	descriptor from the pipeto queue mapping file.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if no more destination descriptors exist in the
 *		   database;
 */
static int nextdest()
{
	register struct gendescr *gendescr;
	register struct pipeto *dest;

	gendescr = nextdb (Pipeqfile);
	if (gendescr == (struct gendescr *)0) return (-1);
	/*
	 *  We successfully read a generic descriptor containing a
	 *  pipe queue destination descriptor.
	 */
	dest = upd_credes(gendescr->v.dest.rqueue, gendescr->v.dest.rhost_mid);
	if (dest == (struct pipeto *) 0) insuffmem();	/* Insufficient mem */
	/*
	 *  The destination was successfully created.
	 *  Finish initialization.
	 */
	dest->pipetono = telldb (Pipeqfile);
	dest->status = gendescr->v.dest.status;
	dest->retry_at = gendescr->v.dest.retry_at;
	dest->retrytime = gendescr->v.dest.retrytime;
	dest->retrydelta = gendescr->v.dest.retrydelta;
	return (0);
}


/*** nextdev
 *
 *
 *	int nextdev():
 *
 *	Get and configure the next device from the next device
 *	descriptor in the device description file.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if no more device descriptors exist in the
 *		   database;
 */
static int nextdev()
{
	register struct gendescr *gendescr;
	register long result;

	gendescr = nextdb (Devicefile);
	if (gendescr == (struct gendescr *)0) return (-1);
	/*
	 *  We successfully read a generic descriptor containing a
	 *  device descriptor.
	 */
	result = upv_credev (gendescr->v.dev.dname, gendescr->v.dev.forms,
			     gendescr->v.dev.fullname, gendescr->v.dev.server);
	if (result == TCML_COMPLETE) {
		/*
	 	 *  The device was successfully created.
	 	 *  Finish initialization.
	 	 */
		New_device->deviceno = telldb (Devicefile);
		New_device->status = gendescr->v.dev.status;
		strcpy (New_device->statmsg, gendescr->v.dev.statmsg);
		return (0);
	}
	else if (result == TCML_ALREADEXI) {
		printf ("F$Multiple database entries for a device.\n");
		printf ("I$Device = %s.\n", gendescr->v.dev.dname);
		errno = 0;		/* Not a system call error */
	}
	else if (result == TCML_INSUFFMEM) {
		insuffmem();		/* Insufficient memory */
	}
	else if (result == TCML_NOSUCHFORM) {
		printf ("W$Device uses forms which do not exist.\n");
		printf ("I$Device = %s.\n", gendescr->v.dev.dname);
		printf ("I$Forms = %s.\n", gendescr->v.dev.forms);
		fflush (stdout);
		return (0);
	}
	else if (result == TCML_TOOMANDEV) {
		printf ("F$Too many device defined.\n");
		printf ("I$MAX_NOOFDEVICES must be increased.\n");
		errno = 0;		/* Not a system call error */
	}
	nqs_abort();			/* Abort NQS execution */
}


/*** nextqmap
 *
 *
 *	int nextqmap():
 *
 *	Get the next queue/device mapping descriptor from the
 *	queue/device mapping file.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if no more qmap descriptors exist in the
 *		   database;
 */
static int nextqmap()
{
	register struct gendescr *gendescr;
	register long result;

	gendescr = nextdb (Qmapfile);
	if (gendescr == (struct gendescr *)0) return (-1);
	/*
	 *  We successfully read a generic descriptor containing a
	 *  queue-to-device or queue-to-destination mapping (qmap)
	 *  descriptor.
	 */
	if (gendescr->v.map.qtodevmap) {
		/*
		 *  We have a queue-to-device mapping.
		 */
		result = upv_addquedev (gendescr->v.map.v.qdevmap.qname,
					gendescr->v.map.v.qdevmap.dname);
		if (result == TCML_COMPLETE) {
			/*
			 *  The mapping was successfully created.
			 *  Finish initialization.
			 */
			New_qdevmap->mapno = telldb (Qmapfile);
			return (0);
		}
		/*
		 *  An error occurred trying to create the mapping.
		 *  Only possible error codes are TCML_ALREADEXI,
		 *  TCML_INSUFFMEM, TCML_NOSUCHDEV, TCML_NOSUCHQUE,
		 *  and TCML_WROQUETYP.
		 */
		if (result == TCML_INSUFFMEM) {
			insuffmem();	/* Insufficient mem */
		}
		else if (result == TCML_NOSUCHDEV) {
			printf ("F$No such device for ");
			printf ("database queue/device mapping.\n");
			printf ("I$Device = %s.\n",
				gendescr->v.map.v.qdevmap.dname);
			errno = 0;		/* Not a system call error */
			fflush (stdout);
		}
		else {
			if (result == TCML_ALREADEXI) {
				printf ("F$Multiple database definitions for");
				printf (" mapping.\n");
				printf ("I$Device = %s.\n",
					gendescr->v.map.v.qdevmap.dname);
			}
			else {
				if (result == TCML_NOSUCHQUE) {
					printf ("F$No such queue ");
				}
				else if (result == TCML_WROQUETYP) {
					printf ("F$Wrong queue type ");
				}
				printf("for database queue/device mapping.\n");
			}
			printf ("I$Queue = %s.\n",
				gendescr->v.map.v.qdevmap.qname);
			fflush (stdout);
			errno = 0;		/* Not a system call error */
		}
		nqs_abort();			/* Abort NQS execution */
	}
	/*
	 *  We have a queue-to-destination mapping.
	 */
	result = upd_addquedes (gendescr->v.map.v.qdestmap.lqueue,
				gendescr->v.map.v.qdestmap.rqueue,
				gendescr->v.map.v.qdestmap.rhost_mid);
	if (result == TCML_COMPLETE) {
		/*
		 *  The destination was successfully created.
		 *  Finish initialization.
		 */
		New_qdestmap->mapno = telldb (Qmapfile);
		return (0);
	}
	/*
	 *  An error occurred trying to create the mapping.
	 *  Only possible error codes are TCML_ALREADEXI,
	 *  TCML_INSUFFMEM, TCML_NOSUCHQUE, and TCML_WROQUETYP.
	 */
	if (result == TCML_INSUFFMEM) insuffmem(); /* Insufficient mem */
	else {
		if (result == TCML_ALREADEXI) {
			printf ("F$Multiple database definitions for ");
			printf ("destination.\n");
			printf ("I$Destination = %s@%s.\n",
				gendescr->v.map.v.qdestmap.rqueue,
				fmtmidname (
					gendescr->v.map.v.qdestmap.rhost_mid
				));
		}
		else {
			if (result == TCML_NOSUCHQUE) {
				printf ("F$No such queue ");
			}
			else if (result == TCML_WROQUETYP) {
				printf ("F$Wrong queue type ");
			}
			printf ("for database destination.\n");
		}
		printf ("I$Local queue = %s.\n",
			gendescr->v.map.v.qdestmap.lqueue);
		fflush (stdout);
		errno = 0;		/* Not a system call error */
	}
	nqs_abort();			/* Abort NQS execution */
}


/*** nextque
 *
 *
 *	int nextque():
 *
 *	Get and configure the next queue from the next queue
 *	descriptor in the queue description file.
 *
 *	Returns:
 *		0: if successful;
 *	       -1: if no more queue descriptors exist in the
 *		   database;
 */
static int nextque()
{
	register struct gendescr *gendescr;
	register long result;
	register int i;

	gendescr = nextdb (Queuefile);
	if (gendescr == (struct gendescr *) 0) return (-1);
	/*
	 *  We successfully read a generic descriptor containing a
	 *  queue descriptor.
	 */
	switch (gendescr->v.que.type) {
	case QUE_BATCH:
		result = upq_creque (gendescr->v.que.namev.name,
				     gendescr->v.que.type,
				     gendescr->v.que.priority,
				     gendescr->v.que.v1.batch.runlimit,
				    (gendescr->v.que.status & QUE_PIPEONLY),
				     (char *) 0);
		break;
	case QUE_DEVICE:
		result = upq_creque (gendescr->v.que.namev.name,
				     gendescr->v.que.type,
				     gendescr->v.que.priority,
				     0,
				    (gendescr->v.que.status & QUE_PIPEONLY),
				     (char *) 0);
		break;
	case QUE_NET:
		result = upq_creque (gendescr->v.que.namev.to_destination,
				     gendescr->v.que.type,
				     gendescr->v.que.priority,
				     gendescr->v.que.v1.network.runlimit,
				    (gendescr->v.que.status & QUE_PIPEONLY),
				     gendescr->v.que.v1.network.server);
		break;
	case QUE_PIPE:
		result = upq_creque (gendescr->v.que.namev.name,
				     gendescr->v.que.type,
				     gendescr->v.que.priority,
				     gendescr->v.que.v1.pipe.runlimit,
				    (gendescr->v.que.status & QUE_PIPEONLY),
				     gendescr->v.que.v1.pipe.server);
		break;
	}
	if (result == TCML_COMPLETE) {
		/*
		 *  The queue was successfully created.
		 *  Finish initialization.
		 */
		New_queue->queueno = telldb (Queuefile);
		/*
		 * Get from disk everything that NQS had saved when it
		 * last ran.  Exception: don't bother getting
		 * the counts of requests in various states
		 * (departcount, runcount, queuedcount, ...).
		 * Those counts must be reconstructed by the
		 * rebuild module: nqs_rbuild.c.
		 */
		New_queue->q.orderversion = gendescr->v.que.orderversion;
		New_queue->q.accessversion = gendescr->v.que.accessversion;
		New_queue->q.status = gendescr->v.que.status;
		New_queue->q.type = gendescr->v.que.type;
		New_queue->q.priority = gendescr->v.que.priority;
#if	UNICOS | SGI | SYS52 | UTS | OSF
		New_queue->q.ru_stime = gendescr->v.que.ru_stime;
		New_queue->q.ru_utime = gendescr->v.que.ru_utime;
#else
#if	BSD42 | BSD43 | ULTRIX
		New_queue->q.ru_stime_usec = gendescr->v.que.ru_stime_usec;
		New_queue->q.ru_stime = gendescr->v.que.ru_stime;
		New_queue->q.ru_utime_usec = gendescr->v.que.ru_utime_usec;
		New_queue->q.ru_utime = gendescr->v.que.ru_utime;
#else
BAD SYSTEM TYPE
#endif
#endif
		if (gendescr->v.que.type == QUE_BATCH) {
	    		strcpy (New_queue->q.namev.name,
				gendescr->v.que.namev.name);
			for (i = 0; i < BSET_CPUVECSIZE; i++) {
		    		New_queue->q.v1.batch.cpuset [i] =
					gendescr->v.que.v1.batch.cpuset [i];
		    		New_queue->q.v1.batch.cpuuse [i] = 0;
			}
#ifdef SDSC
			New_queue->q.v1.batch.node_group = 
			    gendescr->v.que.v1.batch.node_group;
#else
			New_queue->q.v1.batch.ncpuset = 0;
#endif
			New_queue->q.v1.batch.ncpuuse = 0;
			for (i = 0; i < BSET_RS1VECSIZE; i++) {
		    		New_queue->q.v1.batch.rs1set [i] =
					gendescr->v.que.v1.batch.rs1set [i];
		    		New_queue->q.v1.batch.rs1use [i] = 0;
			}

#ifdef SDSC
                        New_queue->q.v1.batch.sPerQueMaxQueuedULimit =
                            gendescr->v.que.v1.batch.sPerQueMaxQueuedULimit;
#else
                        New_queue->q.v1.batch.nrs1set = 0;
#endif
			New_queue->q.v1.batch.nrs1use = 0;
			for (i = 0; i < BSET_RS2VECSIZE; i++) {
		    		New_queue->q.v1.batch.rs2set [i] =
					gendescr->v.que.v1.batch.rs2set [i];
		    		New_queue->q.v1.batch.rs2use [i] = 0;
			}
			New_queue->q.v1.batch.nrs2set = 0;
			New_queue->q.v1.batch.nrs2use = 0;
	    		New_queue->q.v1.batch.runlimit =
				gendescr->v.que.v1.batch.runlimit;
	    		New_queue->q.v1.batch.explicit =
				gendescr->v.que.v1.batch.explicit;
	    		New_queue->q.v1.batch.infinite =
				gendescr->v.que.v1.batch.infinite;
	    		New_queue->q.v1.batch.ppcoreunits =
				gendescr->v.que.v1.batch.ppcoreunits;
	    		New_queue->q.v1.batch.ppcorecoeff =
				gendescr->v.que.v1.batch.ppcorecoeff;
	    		New_queue->q.v1.batch.ppdataunits =
				gendescr->v.que.v1.batch.ppdataunits;
	    		New_queue->q.v1.batch.ppdatacoeff =
				gendescr->v.que.v1.batch.ppdatacoeff;
	    		New_queue->q.v1.batch.pppfileunits =
				gendescr->v.que.v1.batch.pppfileunits;
	    		New_queue->q.v1.batch.pppfilecoeff =
				gendescr->v.que.v1.batch.pppfilecoeff;
	    		New_queue->q.v1.batch.prpfileunits =
				gendescr->v.que.v1.batch.prpfileunits;
	    		New_queue->q.v1.batch.prpfilecoeff =
				gendescr->v.que.v1.batch.prpfilecoeff;
	    		New_queue->q.v1.batch.ppqfileunits =
				gendescr->v.que.v1.batch.ppqfileunits;
	    		New_queue->q.v1.batch.ppqfilecoeff =
				gendescr->v.que.v1.batch.ppqfilecoeff;
	    		New_queue->q.v1.batch.prqfileunits =
				gendescr->v.que.v1.batch.prqfileunits;
	    		New_queue->q.v1.batch.prqfilecoeff =
				gendescr->v.que.v1.batch.prqfilecoeff;
	    		New_queue->q.v1.batch.pptfileunits =
				gendescr->v.que.v1.batch.pptfileunits;
	    		New_queue->q.v1.batch.pptfilecoeff =
				gendescr->v.que.v1.batch.pptfilecoeff;
	    		New_queue->q.v1.batch.prtfileunits =
				gendescr->v.que.v1.batch.prtfileunits;
	    		New_queue->q.v1.batch.prtfilecoeff =
				gendescr->v.que.v1.batch.prtfilecoeff;
	    		New_queue->q.v1.batch.ppmemunits =
				gendescr->v.que.v1.batch.ppmemunits;
	    		New_queue->q.v1.batch.ppmemcoeff = 
				gendescr->v.que.v1.batch.ppmemcoeff;
	    		New_queue->q.v1.batch.prmemunits =
				gendescr->v.que.v1.batch.prmemunits;
	    		New_queue->q.v1.batch.prmemcoeff =
				gendescr->v.que.v1.batch.prmemcoeff;
	    		New_queue->q.v1.batch.ppstackunits =
				gendescr->v.que.v1.batch.ppstackunits;
	    		New_queue->q.v1.batch.ppstackcoeff =
				gendescr->v.que.v1.batch.ppstackcoeff;
	    		New_queue->q.v1.batch.ppworkunits =
				gendescr->v.que.v1.batch.ppworkunits;
	    		New_queue->q.v1.batch.ppworkcoeff =
				gendescr->v.que.v1.batch.ppworkcoeff;
	    		New_queue->q.v1.batch.ppcpusecs =
				gendescr->v.que.v1.batch.ppcpusecs;
	    		New_queue->q.v1.batch.ppcpums =
				gendescr->v.que.v1.batch.ppcpums;
	    		New_queue->q.v1.batch.prcpusecs =
				gendescr->v.que.v1.batch.prcpusecs;
	    		New_queue->q.v1.batch.prcpums =
				gendescr->v.que.v1.batch.prcpums;
	    		New_queue->q.v1.batch.ppnice =
				gendescr->v.que.v1.batch.ppnice;
	    		New_queue->q.v1.batch.prdrives =
				gendescr->v.que.v1.batch.prdrives;
	    		New_queue->q.v1.batch.prncpus =
				gendescr->v.que.v1.batch.prncpus;
		}
		else if (gendescr->v.que.type == QUE_DEVICE) {
	    		strcpy (New_queue->q.namev.name,
				gendescr->v.que.namev.name);
		}
		else if (gendescr->v.que.type == QUE_NET) {
	    		New_queue->q.namev.to_destination =
				gendescr->v.que.namev.to_destination;
	    		New_queue->q.v1.network.runlimit =
				gendescr->v.que.v1.network.runlimit;
	    		strcpy (New_queue->q.v1.network.server,
				gendescr->v.que.v1.network.server);
			New_queue->q.v1.network.retry_at =
				gendescr->v.que.v1.network.retry_at;
			New_queue->q.v1.network.retrytime =
				gendescr->v.que.v1.network.retrytime;
			New_queue->q.v1.network.retrydelta =
				gendescr->v.que.v1.network.retrydelta;
		}
		else {
	    		strcpy (New_queue->q.namev.name,
				gendescr->v.que.namev.name);
	    		New_queue->q.v1.pipe.runlimit =
				gendescr->v.que.v1.pipe.runlimit;
	    		strcpy (New_queue->q.v1.pipe.server,
				gendescr->v.que.v1.pipe.server);
		}
		/*
		 * Get the list of groups and users allowed to access
		 * this queue.
		 */
		switch (New_queue->q.type) {
		case QUE_BATCH:
			New_queue->v1.batch.acclistp = (struct acclist *) 0;
			break;
		case QUE_DEVICE:
			New_queue->v1.device.acclistp = (struct acclist *) 0;
			break;
		case QUE_NET:
			break;
		case QUE_PIPE:
			New_queue->v1.pipe.acclistp = (struct acclist *) 0;
			break;
		}
		get_access (New_queue);
		return (0);
	}
	/*
	 *  An error occurred trying to create the queue.
	 *  Only possible error codes are TCML_ALREADEXI and
	 *  TCML_INSUFFMEM.
	 */
	if (result == TCML_ALREADEXI) {
		printf ("F$Multiple database definitions for queue.\n");
		printf ("I$Queue = %s.\n", gendescr->v.que.namev.name);
		fflush (stdout);
		errno = 0;		/* Not a system call error */
	}
	else insuffmem();		/* Insufficient memory */
	nqs_abort();			/* Abort NQS execution */
}
