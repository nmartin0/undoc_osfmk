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
 
/*++ shoqbydesc.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/shoqbydesc.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the function shoqbydesc().
 *	Shoqbydesc() is responsible for displaying information
 *	about the local queue whose descriptor it is called with.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:56 $ $State: Exp $)
 * $Log: shoqbydesc.c,v $
 * Revision 1.4  1994/11/19  02:27:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/11/02  18:33:19  kremenek
 *  Reviewer: doyle davidl
 *  Risk: low
 *  Benefit or PTS #: 11272
 *  Testing: EAT
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/shoqbydesc.c
 *
 * Revision 1.2  1992/10/09  21:59:44  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:07  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:49:43  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"		/* NQS definitions */
#if	UNICOS
#include "scpnqs.h"
#include <sys/param.h>
#include <sys/jtab.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/sysmacros.h>
#else
#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
#include "nqsxdirs.h"		/* NQS directories */
#define	DESTSET_MARGIN	13	/* Used in shoquedesmap() */
#define	DEVSET_MARGIN	12	/* Used in shoquedevmap() */
#define	OUTPUT_WIDTH	78	/* Number of output columns - 1 */

/*
 *	External functions.
 */
extern char *fmttime();		/* Format time long integer to local time */
				/* string */
extern char *getgrpnam();	/* Get group name given gid */
extern int gethostname();	/* Get name of host system */
extern char *getmacnam();	/* Get machine name from machine-id */
extern int getrreq();		/* Get a control file request header */
extern uid_t getuid();		/* Get real user-id */
extern char *getusenam();	/* Get user name for user-id */
extern long lseek();		/* Seek on a file */
extern struct gendescr *nextdb();
				/* Read the next allocated descriptor */
				/* from an NQS database file */
extern struct confd *opendb();	/* Open an NQS database file */
extern int openqacc();		/* Open a queue access file */
extern int openqord();		/* Open a queue ordering file */
extern void seekdbb();		/* Buffered seek on an NQS database file */
extern int strlen();		/* String length */
extern int strcmp();		/* String comparison */
extern int strncmp();		/* Anchored string comparison */
extern long telldb();		/* Return position of last read on an NQS */
				/* database file */
#if	UNICOS
extern int scpflag;		/* UNICOS called by SCP flag */
extern struct jtab jtab[];	/* Job table */
#else
#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif

static int acccolumn = 0;	/* Used by shoqueacc(), shogrp(), shousr() */


/*** shoqbydesc
 *
 *
 *	int shoqbydesc():
 *
 *	Display information about a local NQS queue.  The caller
 *	of this function must be running with the current directory
 *	of:  Nqs_root.
 *
 *	Returns:
 *		0: If information about the queue and zero or more
 *		   requests residing in the queue was displayed.
 *	       -1: If no output was produced.
 *	       -2: If we were unable to open the queue request-ordering
 *		   file for the queue; no output was produced.
 *	       -3: If the queue was deleted while we were trying
 *		   to open the queue-ordering file.
 */
int shoqbydesc_NOT_USED_NOW (file, que_descr, flags, whomuid, requests, dae_present,
		qmapfile, pipeqfile, qcomplexfile)
struct confd *file;			/* NQS queue file containing.... */
struct gendescr *que_descr;		/* the queue descriptor */
long flags;				/* Display flags */
uid_t whomuid;				/* Zero in on this user */
struct reqset *requests;		/* Optional request selection set */
short dae_present;			/* Boolean non-zero if the local NQS */
					/* daemon is present and running */
struct confd *qmapfile;			/* NQS queue/device/destination */
					/* mapping database file */
struct confd *pipeqfile;		/* NQS pipe-queue destination */
					/* database file */
struct confd *qcomplexfile;		/* NQS queue complex definition file */
{
	void shogap ();			/* Show the gap between selected reqs */
	void shokey ();			/* Show column headers */
	void shoquehdr();		/* Show a queue header */
	void shorequest();		/* Show a request */

	char hostname [256];		/* Name of local host */
	struct qentry cache [QOFILE_CACHESIZ];
					/* Queue ordering file cache buffer */
	int cacheindex;			/* Current buffer cache index */
	int cachesize;			/* Number of queue entries in read */
					/* cache buffer */
	short header;			/* ~0 if queue header displayed */
	int nth_request;		/* Rank of this request */
	int gaplow;			/* Lowest rank among unshown reqs */
	int gaphigh;			/* Highest rank among unshown reqs */
	int fd;				/* Queue ordering file descriptor */
	int i;				/* Loop var */

	gethostname (hostname, 255);	/* Get name of local host and make */
	hostname [255] = '\0';		/* sure that it is null-terminated */
	header = 0;			/* Queue header has not been shown */
	nth_request = 0;		/* No requests yet displayed */
	gaplow = 1;			/* Set up boundary conditions */
	gaphigh = 0;			/* Set up boundary conditions */
	if (flags & (SHO_RS_EXIT | SHO_RS_RUN | SHO_RS_STAGE | SHO_RS_QUEUED |
			SHO_RS_WAIT | SHO_RS_HOLD | SHO_RS_ARRIVE)) {
		/*
		 *  Open the request queue ordering file.
		 */
		if (!dae_present) {
			/*
			 *  Override database if daemon not present.
			 */
			que_descr->v.que.queuedcount
				+= que_descr->v.que.runcount;
			que_descr->v.que.runcount = 0;
		}
		fd = openqord (file, que_descr);
		/*
		 *  fd >= 0 if the queue has requests, and we successfully
		 *	    opened the queue ordering file.
		 *  fd = -1 if the queue has no requests.
		 *  fd = -2 if the queue has requests, but an error prevented
		 *	    us from opening the queue ordering file for
		 *	    the queue.
		 *  fd = -3 if the queue was deleted.
		 */
		if (fd < -1) return (fd);	/* Error or queue was deleted */
	}
	else {
		/*
		 *  We are only interested in the queue header.
		 */
		if (!dae_present) {
			/*
			 *  Override database if daemon not present.
			 */
			que_descr->v.que.queuedcount
				+= que_descr->v.que.runcount;
			que_descr->v.que.runcount = 0;
		}
		shoquehdr (file, que_descr, dae_present, flags, hostname,
			   qmapfile, pipeqfile, qcomplexfile);
						/* Display queue header only */
		return (0);			/* Output was produced */
	}
	/*
	 *  The queue ordering file does exist.
	 *  We use it to try to find requests that match our requirements.
	 */
	if (requests == (struct reqset *) 0) {
		/*
		 *  Definitely display queue header no matter what.
		 */
		header = 1;			/* Mark header as displayed */
		shoquehdr (file, que_descr, dae_present, flags, hostname,
			   qmapfile, pipeqfile, qcomplexfile);
	}
	if (fd == -1) {
		/*
		 *  No requests are in the queue.
		 */
		if (header) return (0);		/* Output was produced */
		return (-1);			/* No output produced */
	}
	cachesize = 0;				/* Mark read cache as invalid*/
	cacheindex = 0;
	if (requests == (struct reqset *) 0) {
		if (flags & SHO_R_SHORT) shokey (que_descr);
	}
	if ((flags & SHO_RS_EXIT) && que_descr->v.que.type == QUE_BATCH) {
		/*
		 *  Show completed requests staging output files.
		 */
		for (i = 0; i < que_descr->v.que.departcount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "EXITING");
				shorequest (&cache [cacheindex],
					    SHO_RS_EXIT, que_descr,
					    nth_request, flags, hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "EXITING");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	else if ((flags & SHO_RS_EXIT) && que_descr->v.que.type==QUE_PIPE) {
		/*
		 *  Show departing requests.
		 */
		for (i = 0; i < que_descr->v.que.departcount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "DEPARTING");
				shorequest (&cache [cacheindex],
					    SHO_RS_EXIT, que_descr,
					    nth_request, flags, hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "DEPARTING");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	else lseek (fd, (long) (que_descr->v.que.departcount *
			sizeof (struct qentry)), 0);
	if ((flags & SHO_RS_RUN) && que_descr->v.que.type != QUE_PIPE) {
		/*
		 * Show running requests.
		 */
		for (i = 0; i < que_descr->v.que.runcount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "RUNNING");
				shorequest (&cache [cacheindex], SHO_RS_RUN,
					    que_descr, nth_request, flags,
					    hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "RUNNING");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	else if ((flags & SHO_RS_RUN) && que_descr->v.que.type == QUE_PIPE) {
		for (i = 0; i < que_descr->v.que.runcount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "ROUTING");
				shorequest (&cache [cacheindex], SHO_RS_RUN,
					    que_descr, nth_request, flags,
					    hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "ROUTING");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	else {
		cacheindex += que_descr->v.que.runcount;
		if (cacheindex > cachesize) {
			lseek (fd, (long) (cacheindex - cachesize)
					 * sizeof (struct qentry), 1);
		}
	}
	if ((flags & SHO_RS_STAGE) && que_descr->v.que.type == QUE_BATCH) {
		/*
		 * Show staging requests.
		 */
		for (i = 0; i < que_descr->v.que.stagecount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "STAGING");
				shorequest (&cache [cacheindex],
					    SHO_RS_STAGE, que_descr,
					    nth_request, flags, hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "STAGING");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	else {
		cacheindex += que_descr->v.que.stagecount;
		if (cacheindex > cachesize) {
			lseek (fd, (long) (cacheindex - cachesize)
					 * sizeof (struct qentry), 1);
		}
	}
	if (flags & SHO_RS_QUEUED) {	/* Show queued requests */
		for (i = 0; i < que_descr->v.que.queuedcount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "QUEUED");
				shorequest (&cache [cacheindex],
					    SHO_RS_QUEUED, que_descr,
					    nth_request, flags, hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "QUEUED");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	else {
		cacheindex += que_descr->v.que.queuedcount;
		if (cacheindex > cachesize) {
			lseek (fd, (long) (cacheindex - cachesize)
					 * sizeof (struct qentry), 1);
		}
	}
	if (flags & SHO_RS_WAIT) {	/* Show waiting requests */
		for (i = 0; i < que_descr->v.que.waitcount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "WAITING");
				shorequest (&cache [cacheindex],
					    SHO_RS_WAIT, que_descr,
					    nth_request, flags, hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "WAITING");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	else {
		cacheindex += que_descr->v.que.waitcount;
		if (cacheindex > cachesize) {
			lseek (fd, (long) (cacheindex - cachesize)
					 * sizeof (struct qentry), 1);
		}
	}
	if (flags & SHO_RS_HOLD) {	/* Show requests holding */
		for (i = 0; i < que_descr->v.que.holdcount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "HOLDING");
				shorequest (&cache [cacheindex],
					    SHO_RS_HOLD, que_descr,
					    nth_request, flags, hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "HOLDING");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	else {
		cacheindex += que_descr->v.que.holdcount;
		if (cacheindex > cachesize) {
			lseek (fd, (long) (cacheindex - cachesize)
					 * sizeof (struct qentry), 1);
		}
	}
	if (flags & SHO_RS_ARRIVE) {	/* Show arriving requests */
		for (i = 0; i < que_descr->v.que.arrivecount; i++) {
			nth_request++;
			if (cacheindex >= cachesize) {
				cachesize = read (fd, (char *) cache,
						  sizeof (cache));
				cachesize /= sizeof (struct qentry);
				cacheindex = 0;
			}
			if (selrequest (&cache [cacheindex], flags,
					whomuid, requests)) {
				if (!header) {
					/*
					 *  Display the queue header.
					 *  Mark header as displayed.
					 */
					header = 1;
					shoquehdr (file, que_descr, dae_present,
						   flags, hostname, qmapfile,
						   pipeqfile, qcomplexfile);
					if (flags & SHO_R_SHORT) {
						shokey (que_descr);
					}
				}
				shogap (gaplow, gaphigh, "ARRIVING");
				shorequest (&cache [cacheindex],
					    SHO_RS_ARRIVE, que_descr,
					    nth_request, flags, hostname);
				gaphigh = nth_request;
				gaplow = nth_request + 1;
			}
			else gaphigh++;
			cacheindex++;
		}
		shogap (gaplow, gaphigh, "ARRIVING");
		gaphigh = nth_request;
		gaplow = nth_request + 1;
	}
	close (fd);			/* Close queue ordering file */
	if (header) return (0);		/* There was output */
	return (-1);			/* No output at all */
}


/*** get_units
 *
 *
 *	char *get_units():
 *	Get quota units.
 */
static char *get_units (units)
register short units;
{
	switch (units) {
	case QLM_BYTES:
		return ("bytes");
	case QLM_WORDS:
		return ("words");
	case QLM_KBYTES:
		return ("kilobytes");
	case QLM_KWORDS:
		return ("kilowords");
	case QLM_MBYTES:
		return ("megabytes");
	case QLM_MWORDS:
		return ("megawords");
	case QLM_GBYTES:
		return ("gigabytes");
	case QLM_GWORDS:
		return ("gigawords");
	}
	return ("");		/* Unknown units! */
}


/*** selrequest
 *
 *
 *	int selrequest():
 *	Select a request.
 *
 *	Returns:
 *		1: if the request was selected for display;
 *		0: if not.
 */
static int selrequest (qentry, flags, whomuid, requests)
register struct qentry *qentry;		/* Request queue entry */
long flags;				/* Display mode flags */
uid_t whomuid;				/* Whom we are asking about */
register struct reqset *requests;	/* Specific request select list */
{
	if (requests == (struct reqset *) 0 ) {	
		if (flags & SHO_R_ALLUID) return (1);
		else {
			if (qentry->uid == whomuid) {
				return (1);	/* Request will be shown */
			}
			return (0);		/* Request will not be shown */
		}
	}
	/*
	 *  Scan specific request set.
	 */
	do {
		if ((flags & SHO_R_ALLUID) || qentry->uid == whomuid) {
			/*
			 *  Might be one of the requests we are looking for.
			 */
			if (requests->selecttype == SEL_REQNAME) {
				/*
				 *  Selection is by request name.
				 */
				if (strncmp (requests->v.reqname,
					     qentry->reqname_v,
					     MAX_REQNAME) == 0) {
					return (1);	/* A match */
				}
			}
			else {
				/*
				 *  Selection is by request-id.
				 */
				if (requests->v.reqid.orig_mid
					== qentry->orig_mid &&
				    requests->v.reqid.orig_seqno
					== qentry->orig_seqno) {
					return (1);	/* A match */
				}
			}
		}
		requests = requests->next;	/* Get next request */
						/* in selection set */
	} while (requests != (struct reqset *) 0);	/* until end of set */
	return (0);			/* It wasn't one of the requests */
					/* we are looking for. */
}


/*** shocpulim
 *
 *
 *	void shocpulim():
 *	Display a CPU time limit quota.
 */
static void shocpulim (cpu, explicit, infinite, que_descr)
register struct cpulimit *cpu;
long explicit;
long infinite;
struct gendescr *que_descr;
{
	if (infinite == 0) {
		printf ("= [%1lu.%1d, %1lu.%1d]",
			 cpu->max_seconds, cpu->max_ms,
			 cpu->warn_seconds, cpu->warn_ms);
		if (explicit == 0) printf ("<DEFAULT>");
	}
	else {
		/*
		 *  The limit is infinite, or is not specified.
		 */
		if (que_descr->v.que.type == QUE_BATCH || explicit) {
			printf ("= UNLIMITED ");
			if (explicit == 0) printf ("<DEFAULT>");
		}
		else printf ("= UNSPECIFIED");
	}
	putchar ('\n');
}


/*** shogap
 *
 *
 *	void shogap():
 *	Display a gap in between requests we are interested in.
 */
static void shogap (gaplow, gaphigh, state)
int gaplow;
int gaphigh;
char * state;
{
	if (gaplow > gaphigh) return;
	if (gaplow == gaphigh) printf ("<1 request %s>\n", state);
	else printf ("<%d requests %s>\n", gaphigh - gaplow + 1, state);
}


/*** shogrp
 *
 *
 *	void shogrp():
 *	Display a group name.
 */
static void shogrp (gid)
unsigned long gid;
{
	if (acccolumn >= 4) {
		acccolumn = 0;
		printf ("\n    ");
	}
	else printf ("%s\t", getgrpnam ((int) gid));
	acccolumn++;
}


/*** shokey
 *
 *
 *	void shokey():
 *	Display column headings.
 */
static void shokey (que_descr)
register struct gendescr *que_descr;
{
#if UNICOS
	if (scpflag) return;
#endif
	if (que_descr->v.que.type == QUE_DEVICE) {
		printf ("         REQUEST NAME        REQUEST ID            ");
		printf ("USER  PRI    STATE     SIZE\n");
		return;
	}
#if UNICOS
	printf ("REQUEST NAME      JID   STATE  NICE  SIZE    CPU  ");
	printf (" CPLIM  OWNER    NQSID\n");
	printf ("--------------- ------ -------- --- ------ ------ ");
	printf ("------- -------- -----\n");
#else
	printf ("         REQUEST NAME        REQUEST ID            ");
	printf ("USER  PRI    STATE     PGRP\n");
#endif
}


/*** shoomd
 *
 *
 *	void shoomd():
 *	Display an output mode.
 */
static void shoomd (mode)
char mode;
{
	if (mode & OMD_SPOOL) {
		printf ("SPOOL\n");
	}
	else if (mode & OMD_EO) {
		printf ("EO\n");
	}
	else printf ("NOSPOOL\n");
}


/*** shoqueacc
 *
 *
 *	void shoqueacc():
 *	Display the groups and users allowed to access a queue.
 */
static void shoqueacc (file, que_descr)
struct confd *file;
struct gendescr *que_descr;
{
	void shousr();			/* Show user */
	
	int fd;				/* File descriptor */
	unsigned long buffer [QAFILE_CACHESIZ];
	int done;			/* Boolean */
	int cachebytes;			/* Try to read this much */
	int bytes;			/* Did read this much */
	int entries;			/* Did read this many */
	int i;				/* Loop variable */


	fd = openqacc (file, que_descr);
	/*
	 * Openqacc returns 0 if the queue is restricted and the file
	 * is open; -1 if the queue is unrestricted; -2 if the queue
	 * queue is restricted but the file is not open, and -3
	 * if the queue appears to have been deleted.
	 */
	if (fd < -1) {
		printf ("  Failed to open queue access file.\n");
		return;
	}
	if (fd == -1) {
		printf ("  Unrestricted access\n");
		return;
	}
	printf ("  Groups with access:  ");
	acccolumn = 2;
	done = 0;
	cachebytes = QAFILE_CACHESIZ * sizeof (unsigned long);
	while ((bytes = read (fd, (char *) buffer, cachebytes)) > 0) {
		entries = bytes / sizeof (unsigned long);
		for (i = 0; i < entries; i++) {
			/* Zero comes only at the end */
			if (buffer [i] == 0) {
				done = 1;
				break;
			}
			if ((buffer [i] & MAKEGID) != 0) /* A group */
				shogrp (buffer [i] & ~MAKEGID);
		}
		if (done) break;
	}
	putchar ('\n');
	lseek (fd, 0, 0);			/* Back to the beginning */
	printf ("  Users with access:  ");
	acccolumn = 2;
	done = 0;
	shousr ((unsigned long) 0);		/* Root always has access */
	while ((bytes = read (fd, (char *) buffer, cachebytes)) > 0) {
		entries = bytes / sizeof (unsigned long);
		for (i = 0; i < entries; i++) {
			/* Zero comes only at the end */
			if (buffer [i] == 0) {
				done = 1;
				break;
			}
			if ((buffer [i] & MAKEGID) == 0) /* Not a group */
				shousr (buffer [i]);
		}
		if (done) break;
	}
	putchar ('\n');
}


/*** shoquedesmap
 *
 *
 *	void shoquedesmap():
 *	Display the destination set for a pipe queue.
 */
static void shoquedesmap (que_descr, qmapfile, pipeqfile)
struct gendescr *que_descr;		/* The queue descriptor */
struct confd *qmapfile;			/* Queue/device/destination mapping */
					/* database file */
struct confd *pipeqfile;		/* Pipe-destination database file */
{
	register struct gendescr *dbds1;/* NQS database file generic descr */
	register struct gendescr *dbds2;/* NQS database file generic descr */
	register short column;		/* Current output column */
	register short fldsiz;		/* Size of output field */
	short preamble;			/* Display flag */
	short first;			/* Display flag */
	char *rhostname;		/* Remote host destination name */
	
	if (telldb (qmapfile) != -1) {
		/*
		 *  We are not necessarily at the start of the
		 *  queue/device/destination descriptor database
		 *  file....
		 */
		seekdbb (qmapfile, 0L);	/* Seek to the beginning */
	}
	dbds1 = nextdb (qmapfile);
	preamble = 0;		/* "Destset = [" not displayed yet */
	first = 1;		/* If destination displayed, it will */
				/* be the first in the set */
	while (dbds1 != (struct gendescr *) 0) {
		if (!dbds1->v.map.qtodevmap &&
		    strcmp (dbds1->v.map.v.qdestmap.lqueue,
			    que_descr->v.que.namev.name) == 0) {
			/*
			 *  We have located a pipe-queue/destination mapping
			 *  for the pipe queue.
			 *
			 *  Now, locate the database description for the
			 *  pipe queue destination referenced in the mapping.
			 */
			if (telldb (pipeqfile) != -1) {
				/*
				 *  We are not necessarily at the start of the
				 *  pipe queue descriptor database file....
				 */
				seekdbb (pipeqfile, 0L);/* Seek to start */
			}
			dbds2 = nextdb (pipeqfile);
			while (dbds2 != (struct gendescr *) 0 &&
			      (strcmp (dbds1->v.map.v.qdestmap.rqueue,
				       dbds2->v.dest.rqueue) ||
			       dbds1->v.map.v.qdestmap.rhost_mid !=
			       dbds2->v.dest.rhost_mid)) {
				/*
				 *  We have not yet located the pipe queue
				 *  destination referred to in the mapping
				 *  descriptor.
				 */
				dbds2 = nextdb (pipeqfile);
			}
			if (dbds2 != (struct gendescr *) 0) {
			    /*
			     *  We have located the pipe-queue
			     *  descriptor for the mapping.
			     */
			    if (!preamble) {
				/*
				 *  Display destination set
				 *  header.
				 */
				preamble = 1;
				printf ("  Destset = {");
				column = DESTSET_MARGIN; /* From 0 */
			    }
			    rhostname = getmacnam (dbds2->v.dest.rhost_mid);
			    if (!(dbds2->v.dest.status & DEST_ENABLED) &&
				 (dbds2->v.dest.status & DEST_RETRY)) {
				/*
				 *  The destination is in the retry
				 *  state.  Show the time of the
				 *  next earliest retry, and the
				 *  time of the first failure.
				 */
				if (column != DESTSET_MARGIN) {
					printf (",\n%*s", DESTSET_MARGIN, "");
				}
				printf ("%s@%s [RETRY]\n",
					dbds2->v.dest.rqueue, rhostname);
				printf ("%*s<Unreachable since: %s>\n",
					DESTSET_MARGIN+4, "",
					fmttime (&dbds2->v.dest.retrytime));
				printf ("%*s<Next retry at: %s>\n%*s",
					DESTSET_MARGIN+4, "",
					fmttime (&dbds2->v.dest.retry_at),
					DESTSET_MARGIN, "");
				column = DESTSET_MARGIN;
			    }
			    else {
				/*
				 *  The destination is either enabled,
				 *  or has been marked as failed.
				 */
				fldsiz = strlen (dbds2->v.dest.rqueue)
				       + strlen (rhostname) + 1;
				if (dbds2->v.dest.status & DEST_FAILED) {
					/*
					 *  This destination has been
					 *  marked as failed.
					 */
					fldsiz += 9;	/* _[FAILED] */
				}
				if (!first) {
					putchar (',');
					fldsiz += 2;	/*+2 for ', '*/
				}
				if (column + fldsiz >= OUTPUT_WIDTH) {
					printf ("\n%*s", DESTSET_MARGIN, "");
					column = fldsiz + DESTSET_MARGIN;
					if (!first) column -= 2;
				}
				else {
					if (!first) putchar (' ');
					column += fldsiz;
				}
				printf ("%s@%s", dbds2->v.dest.rqueue,
					rhostname);
				if (dbds2->v.dest.status & DEST_FAILED) {
					printf (" [FAILED]");
				}
			    }
			    first = 0;		/* Not the first */
			}
		}
		dbds1 = nextdb (qmapfile);
	}
	if (preamble) printf ("};\n");
}


/*** shoquehdr
 *
 *
 *	void shoquehdr():
 *	Display the queue header.
 */
static void shoquehdr (file, que_descr, dae_present, flags, hostname,
		       qmapfile, pipeqfile, qcomplexfile)
struct confd *file;			/* NQS queue file containing.... */
struct gendescr *que_descr;		/* The Queue-descriptor */
short dae_present;			/* Non-zero if the local NQS daemon */
					/* is present and running */
long flags;				/* Display mode flags */
char *hostname;				/* Name of local host */
struct confd *qmapfile;			/* Queue/device/destination mapping */
					/* database file */
struct confd *pipeqfile;		/* Pipe-destination database file */
struct confd *qcomplexfile;		/* Queue complex definition file */
{
	void shoquelims();		/* Show queue limits */
	void shoquedevmap();		/* Show queue-to-device mappings */
	void shoquetime();		/* Show queue cpu usage */
	struct gendescr *qcom_descr;	/* A queue-complex-descriptor */
	register char *namep, *qname;
	register int i, found;

#if	UNICOS
	if (scpflag) {
		struct	qstat_q	scp;	/* structure for SCP info */

		if (que_descr->v.que.type == QUE_DEVICE) return;
		memset( (char *)&scp, 0, sizeof(scp) );
		strncpy(scp.qname, que_descr->v.que.namev.name,
			sizeof(scp.qname));
		scp.limit = que_descr->v.que.v1.batch.runlimit;
		scp.running = que_descr->v.que.runcount;
		scp.queued = que_descr->v.que.queuedcount +
			que_descr->v.que.waitcount + que_descr->v.que.holdcount;
		if (que_descr->v.que.status & QUE_RUNNING) scp.status = 1;
		write(scpflag, (char *)&scp, sizeof(scp));
		return;
	}
#else
#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS  | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
	printf ("%s@%s;  type=", que_descr->v.que.namev.name, hostname);
	switch (que_descr->v.que.type) {
	case QUE_BATCH:
		printf ("BATCH");
		break;
	case QUE_DEVICE:
		printf ("DEVICE");
		break;
	case QUE_PIPE:
		printf ("PIPE");
		break;
	case QUE_NET:
		printf ("NETWORK");
		break;
	default:
		printf ("UNKNOWN");
		break;
	}
	printf (";  [");
	if (que_descr->v.que.status & QUE_ENABLED) {
		if (dae_present) printf ("ENABLED, ");
		else printf ("CLOSED, ");
	}
	else printf ("DISABLED, ");
	if (que_descr->v.que.status & QUE_RUNNING) {
		if (!dae_present) printf ("SHUTDOWN");
		else if (que_descr->v.que.runcount) printf ("RUNNING");
		else printf ("INACTIVE");
	}
	else if (que_descr->v.que.runcount && dae_present) printf ("STOPPING");
	else printf ("STOPPED");
	fputs ("];", stdout);
	if (que_descr->v.que.status & QUE_PIPEONLY) {
		fputs ("  PIPEONLY;", stdout);
	}
	printf ("  pri=%1d;", que_descr->v.que.priority);
	printf ("  lim=");
	switch (que_descr->v.que.type) {
	case QUE_BATCH:
		printf ("%1d\n", que_descr->v.que.v1.batch.runlimit);
		printf (" %2d exit;  ", que_descr->v.que.departcount);
		printf ("%2d run;  ", que_descr->v.que.runcount);
		printf ("%2d stage;  ", que_descr->v.que.stagecount);
		break;
	case QUE_DEVICE:
		printf ("1\n");
		break;
	case QUE_PIPE:
		printf ("%1d\n", que_descr->v.que.v1.pipe.runlimit);
		printf (" %2d depart;  ", que_descr->v.que.departcount);
		printf ("%2d route;  ", que_descr->v.que.runcount);
		break;
	case QUE_NET:
		printf ("%1d\n", que_descr->v.que.v1.network.runlimit);
		printf (" %2d run;  ", que_descr->v.que.runcount);
		break;
	}
	printf ("%2d queued;  ", que_descr->v.que.queuedcount);
	printf ("%2d wait;  ", que_descr->v.que.waitcount);
	printf ("%2d hold;  ", que_descr->v.que.holdcount);
	printf ("%2d arrive;\n", que_descr->v.que.arrivecount);
	if (flags & SHO_H_TIME) {
		shoquetime (que_descr);
	}
	if (flags & SHO_H_ACCESS && que_descr->v.que.type != QUE_NET) {
		shoqueacc (file, que_descr);
	}
	if (flags & SHO_H_SERV && que_descr->v.que.type == QUE_PIPE) {
		printf ("  Queue server: %s\n",
			que_descr->v.que.v1.pipe.server);
	}
	if (flags & SHO_H_MAP && que_descr->v.que.type == QUE_DEVICE) {
		shoquedevmap (que_descr, qmapfile);
	}
	else if (flags & SHO_H_DEST && que_descr->v.que.type == QUE_PIPE) {
		shoquedesmap (que_descr, qmapfile, pipeqfile);
	}
	if (que_descr->v.que.type == QUE_BATCH) {
		found = 0;
		if (telldb (qcomplexfile) != -1) {
			seekdbb (qcomplexfile, 0L);	/* Seek to beginning */
		}
		qname = que_descr->v.que.namev.name;
		while ((qcom_descr = nextdb( qcomplexfile )) !=
		      (struct gendescr *)0) {
			for (i = 0; i < MAX_QSPERCOMPLX; i++) {
				namep = qcom_descr->v.qcom.queues[i];
				if (*namep == '\0') continue;
				if (strcmp(namep, qname) != 0) continue;
				if (!found) {
					printf("  Queue Complex Membership:");
				}
				found = 1;
				printf("  %s;", qcom_descr->v.qcom.name);
			}
		}
		if (found) putchar('\n');
		if (flags & SHO_H_LIM) {
			shoquelims (que_descr);
		}
	}
	putchar ('\n');				/* Leave one blank line */
}


/*** shoquelims
 *
 *
 *	void shoquelims():
 *	Display the limits associated with a queue.
 */
static void shoquelims (que_descr)
struct gendescr * que_descr;		/* The queue */
{
static char *unlimited = "UNLIMITED";
static char *def = " <DEFAULT>";

#if	(VALID_LIMITS & LIM_PRDRIVES)
	printf ("  Per-request tape drives limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PRDRIVES) {
		fputs (unlimited, stdout);
	}
	else printf ("%1d", que_descr->v.que.v1.batch.prdrives);
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PRDRIVES)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPCORE)
	printf ("  Per-process core file size limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPCORE) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.ppcorecoeff,
			get_units (que_descr->v.que.v1.batch.ppcoreunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPCORE)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPDATA)
	printf ("  Per-process data size limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPDATA) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.ppdatacoeff,
			get_units (que_descr->v.que.v1.batch.ppdataunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPDATA)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPPFILE)
	printf ("  Per-process permanent file ");
	printf ("size limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPPFILE) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.pppfilecoeff,
			get_units (que_descr->v.que.v1.batch.pppfileunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPPFILE)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PRPFILE)
	printf ("  Per-request permanent file ");
	printf ("space limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PRPFILE) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.prpfilecoeff,
			get_units (que_descr->v.que.v1.batch.prpfileunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PRPFILE)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPMEM)
	printf ("  Per-process memory size limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPMEM) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s", que_descr->v.que.v1.batch.ppmemcoeff,
			get_units (que_descr->v.que.v1.batch.ppmemunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPMEM)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PRMEM)
	printf ("  Per-request memory size limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PRMEM) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s", que_descr->v.que.v1.batch.prmemcoeff,
			get_units (que_descr->v.que.v1.batch.prmemunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PRMEM)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPNICE)
	printf ("  Per-process execution nice value = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPNICE) {
		fputs (unlimited, stdout);
	}
	else printf ("%1d", que_descr->v.que.v1.batch.ppnice);
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPNICE)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PRNCPUS)
	printf ("  Per-request number of cpus limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PRNCPUS) {
		fputs (unlimited, stdout);
	}
	else printf ("%1d", que_descr->v.que.v1.batch.prncpus);
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PRNCPUS)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPQFILE)
	printf ("  Per-process quick file ");
	printf ("size limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPQFILE) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.ppqfilecoeff,
			get_units (que_descr->v.que.v1.batch.ppqfileunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPQFILE)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PRQFILE)
	printf ("  Per-request quick file ");
	printf ("space limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PRQFILE) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.prqfilecoeff,
			get_units (que_descr->v.que.v1.batch.prqfileunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PRQFILE)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPSTACK)
	printf ("  Per-process stack size limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPSTACK) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.ppstackcoeff,
			get_units (que_descr->v.que.v1.batch.ppstackunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPSTACK)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPCPUT)
	printf ("  Per-process CPU time limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPCPUT) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu.%1d", que_descr->v.que.v1.batch.ppcpusecs,
			que_descr->v.que.v1.batch.ppcpums);
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPCPUT)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PRCPUT)
	printf ("  Per-request CPU time limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PRCPUT) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu.%1d", que_descr->v.que.v1.batch.prcpusecs,
			que_descr->v.que.v1.batch.prcpums);
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PRCPUT)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPTFILE)
	printf ("  Per-process temporary file ");
	printf ("size limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPTFILE) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.pptfilecoeff,
			get_units (que_descr->v.que.v1.batch.pptfileunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPTFILE)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PRTFILE)
	printf ("  Per-request temporary file ");
	printf ("space limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PRTFILE) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.prtfilecoeff,
			get_units (que_descr->v.que.v1.batch.prtfileunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PRTFILE)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
#if	(VALID_LIMITS & LIM_PPWORK)
	printf ("  Per-process working set limit = ");
	if (que_descr->v.que.v1.batch.infinite & LIM_PPWORK) {
		fputs (unlimited, stdout);
	}
	else {
		printf ("%1lu %s",
			que_descr->v.que.v1.batch.ppworkcoeff,
			get_units(que_descr->v.que.v1.batch.ppworkunits));
	}
	if (!(que_descr->v.que.v1.batch.explicit & LIM_PPWORK)) {
		fputs (def, stdout);
	}
	putchar ('\n');
#endif
}


/*** shoquedevmap
 *
 *
 *	void shoquedevmap():
 *	Display the queue-to-device mappings for a queue.
 */
static void shoquedevmap (que_descr, qmapfile)
struct gendescr *que_descr;		/* The queue descriptor */
struct confd *qmapfile;			/* Queue/device/destination mapping */
					/* database file */
{
	register struct gendescr *dbdsc;/* NQS database file generic descr */
	register short column;		/* Current output column */
	register short fldsiz;		/* Current field size */
	short preamble;			/* Display flag */
	short first;			/* Display flag */
	
	if (telldb (qmapfile) != -1) {
		/*
		 *  We are not necessarily at the start of the database file.
		 */
		seekdbb (qmapfile, 0L);	/* Seek to the beginning */
	}
	dbdsc = nextdb (qmapfile);
	preamble = 0;		/* "Device set = [" not displayed */
	first = 1;		/* If device displayed, it will be */
				/* the first in the set */
	while (dbdsc != (struct gendescr *) 0) {
		if (dbdsc->v.map.qtodevmap) {
			if (strcmp (dbdsc->v.map.v.qdevmap.qname,
				    que_descr->v.que.namev.name) == 0) {
				if (!preamble) {
					/*
					 *  Display device set header.
					 */
					preamble = 1;
					printf ("  Devset = {");
					column = DEVSET_MARGIN;	/* From 0 */
				}
				fldsiz = strlen (dbdsc->v.map.v.qdevmap.dname);
				if (!first) {
					putchar (',');
					fldsiz += 2;	/* +2 for ', '*/
				}
				if (column + fldsiz >= OUTPUT_WIDTH) {
					printf ("\n%*s", DEVSET_MARGIN, "");
					column = fldsiz + DEVSET_MARGIN;
					if (!first) column -= 2;
				}
				else {
					if (!first) putchar (' ');
					column += fldsiz;
				}
				fputs (dbdsc->v.map.v.qdevmap.dname, stdout);
				first = 0;	/* Not the first */
			}
		}
		dbdsc = nextdb (qmapfile);
	}
	if (preamble) printf ("};\n");
}


/*** shoquetime
 *
 *
 *	void shoquetime():
 *	Display the cumulative cpu time used by requests in a queue.
 */
static void shoquetime (que_descr)
struct gendescr *que_descr;
{
	printf ("  Cumulative system space time = ");
#if	SGI | SYS52 | UNICOS | UTS | OSF
	printf ("%ld.%02d seconds\n", que_descr->v.que.ru_stime / CPU_MEAS_GRAN,
		((que_descr->v.que.ru_stime % CPU_MEAS_GRAN) * 100) /
		CPU_MEAS_GRAN);
#else
#if	BSD42 | BSD43 | ULTRIX
	printf ("%ld.%06ld seconds\n", que_descr->v.que.ru_stime,
		que_descr->v.que.ru_stime_usec);
#else
BSD SYSTEM TYPE
#endif
#endif
	printf ("  Cumulative user space time = ");
#if	SGI | SYS52 | UNICOS | UTS | OSF
	printf ("%ld.%02d seconds\n", que_descr->v.que.ru_utime / CPU_MEAS_GRAN,
		((que_descr->v.que.ru_utime % CPU_MEAS_GRAN) * 100) /
		CPU_MEAS_GRAN);
#else
#if	BSD42 | BSD43 | ULTRIX
	printf ("%ld.%06ld seconds\n", que_descr->v.que.ru_utime,
		que_descr->v.que.ru_utime_usec);
#else
BSD SYSTEM TYPE
#endif
#endif
}


/*** shoquolim
 *
 *
 *	void shoquolim():
 *	Display a quota limit.
 */
static void shoquolim (quota, explicit, infinite, que_descr)
register struct quotalimit *quota;
long explicit;
long infinite;
struct gendescr *que_descr;
{
	if (infinite == 0) {
		printf ("= [%1lu %s, %1lu %s]", quota->max_quota,
			get_units (quota->max_units), quota->warn_quota,
			get_units (quota->warn_units));
		if (explicit == 0) printf ("<DEFAULT>");
	}
	else {
		/*
		 *  The limit is infinite, or is not specified.
		 */
		if (que_descr->v.que.type == QUE_BATCH || explicit) {
			printf ("= UNLIMITED ");
			if (explicit == 0) printf ("<DEFAULT>");
		}
		/* Infinite, default limits are meaningless in a pipe queue */
		else printf ("= UNSPECIFIED");
	}
	putchar ('\n');
}


/*** shoreqlims
 *
 *
 *	void shoreqlims():
 *	Display the limits associated with a request.
 */
static void shoreqlims (que_descr, rawreqp)
struct gendescr *que_descr;		/* Needed to detect pipe queues */
struct rawreq *rawreqp;			/* The request as a rawreq */
{
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PRDRIVES)) {
		printf ("  Per-req. tape drives limit");
		if ((rawreqp->v.bat.infinite & LIM_PRDRIVES) == 0) {
			printf (" = %1d", rawreqp->v.bat.prdrives);
			if (!(rawreqp->v.bat.explicit & LIM_PRDRIVES)) {
				printf (" <DEFAULT>");
			}
		}
		else printf (" = UNSPECIFIED");
		putchar ('\n');
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPCORE)) {
		printf ("  Per-proc. core file size limit");
		shoquolim (&rawreqp->v.bat.ppcoresize,
			   rawreqp->v.bat.explicit & LIM_PPCORE,
			   rawreqp->v.bat.infinite & LIM_PPCORE,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPDATA)) {
		printf ("  Per-proc. data size limit");
		shoquolim (&rawreqp->v.bat.ppdatasize,
			   rawreqp->v.bat.explicit & LIM_PPDATA,
			   rawreqp->v.bat.infinite & LIM_PPDATA,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPPFILE)) {
		printf ("  Per-proc. permanent file ");
		printf ("size limit");
		shoquolim (&rawreqp->v.bat.pppfilesize,
			   rawreqp->v.bat.explicit & LIM_PPPFILE,
			   rawreqp->v.bat.infinite & LIM_PPPFILE,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PRPFILE)) {
		printf ("  Per-req. permanent file ");
		printf ("space limit");
		shoquolim (&rawreqp->v.bat.prpfilespace,
			   rawreqp->v.bat.explicit & LIM_PRPFILE,
			   rawreqp->v.bat.infinite & LIM_PRPFILE,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPMEM)) {
		printf ("  Per-proc. memory size limit");
		shoquolim (&rawreqp->v.bat.ppmemsize,
			   rawreqp->v.bat.explicit & LIM_PPMEM,
			   rawreqp->v.bat.infinite & LIM_PPMEM,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PRMEM)) {
		printf ("  Per-req. memory size limit");
		shoquolim (&rawreqp->v.bat.prmemsize,
			   rawreqp->v.bat.explicit & LIM_PRMEM,
			   rawreqp->v.bat.infinite & LIM_PRMEM,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPNICE)) {
		printf ("  Per-proc. execution nice priority");
		if ((rawreqp->v.bat.infinite & LIM_PPNICE) == 0) {
			printf (" = %1d", rawreqp->v.bat.ppnice);
			if (!(rawreqp->v.bat.explicit & LIM_PPNICE)) {
				printf (" <DEFAULT>");
			}
		}
		else printf (" = UNSPECIFIED");
		putchar ('\n');
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PRNCPUS)) {
		printf ("  Per-req. # of cpus limit");
		if ((rawreqp->v.bat.infinite & LIM_PRNCPUS) == 0) {
			printf (" = %1d", rawreqp->v.bat.prncpus);
			if (!(rawreqp->v.bat.explicit & LIM_PRNCPUS)) {
				printf (" <DEFAULT>");
			}
		}
		else printf (" = UNSPECIFIED");
		putchar ('\n');
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPQFILE)) {
		printf ("  Per-proc. quick file ");
		printf ("size limit");
		shoquolim (&rawreqp->v.bat.ppqfilesize,
			   rawreqp->v.bat.explicit & LIM_PPQFILE,
			   rawreqp->v.bat.infinite & LIM_PPQFILE,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PRQFILE)) {
		printf ("  Per-req. quick file ");
		printf ("space limit");
		shoquolim (&rawreqp->v.bat.prqfilespace,
			   rawreqp->v.bat.explicit & LIM_PRQFILE,
			   rawreqp->v.bat.infinite & LIM_PRQFILE,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPSTACK)) {
		printf ("  Per-proc. stack size limit");
		shoquolim (&rawreqp->v.bat.ppstacksize,
			   rawreqp->v.bat.explicit & LIM_PPSTACK,
			   rawreqp->v.bat.infinite & LIM_PPSTACK,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPCPUT)) {
		printf ("  Per-proc. CPU time limit");
		shocpulim (&rawreqp->v.bat.ppcputime,
			   rawreqp->v.bat.explicit & LIM_PPCPUT,
			   rawreqp->v.bat.infinite & LIM_PPCPUT,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PRCPUT)) {
		printf ("  Per-req. CPU time limit");
		shocpulim (&rawreqp->v.bat.prcputime,
			   rawreqp->v.bat.explicit & LIM_PRCPUT,
			   rawreqp->v.bat.infinite & LIM_PRCPUT,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPTFILE)) {
		printf ("  Per-proc. temporary file ");
		printf ("size limit");
		shoquolim (&rawreqp->v.bat.pptfilesize,
			   rawreqp->v.bat.explicit & LIM_PPTFILE,
			   rawreqp->v.bat.infinite & LIM_PPTFILE,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PRTFILE)) {
		printf ("  Per-req. temporary file ");
		printf ("space limit");
		shoquolim (&rawreqp->v.bat.prtfilespace,
			   rawreqp->v.bat.explicit & LIM_PRTFILE,
			   rawreqp->v.bat.infinite & LIM_PRTFILE,
			   que_descr);
	}
	if (que_descr->v.que.type == QUE_PIPE ||
	   (VALID_LIMITS & LIM_PPWORK)) {
		printf ("  Per-proc. working set limit");
		shoquolim (&rawreqp->v.bat.ppworkset,
			   rawreqp->v.bat.explicit & LIM_PPWORK,
			   rawreqp->v.bat.infinite & LIM_PPWORK,
			   que_descr);
	}
}


/*** shorequest
 *
 *
 *	void shorequest():
 *	Display a request.
 */
static void shorequest (qentry, reqstate, que_descr, nth_request, flags,
			hostname)
struct qentry *qentry;			/* Queue entry describing request */
int reqstate;				/* Req state=[HOLDING,RUNNING,QUEUED] */
struct gendescr *que_descr;		/* Queue file queue-descriptor */
int nth_request;			/* N-th request number in the queue */
long flags;				/* Display mode flags */
char *hostname;				/* Local host name */
{
	char reqname [MAX_REQNAME+1];	/* Name of request */
	char *status;			/* Request status */
	char *machinename;		/* Name of originating machine */
	char *username;			/* User name of request owner */
	struct rawreq rawreq;		/* Control file raw request */
	int cfd;			/* Control file file-descr */
	int chars;			/* String length */
	register int i;
#if UNICOS
	register int jid;		/* job id */
	register struct jtab *jtabp;	/* Job table entry pointer */
	struct	qstat_j	scp;		/* record structure for SCP */
#else
#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif

	machinename = getmacnam (qentry->orig_mid);
	username = getusenam (qentry->uid);
	switch (reqstate) {
	case SHO_RS_EXIT:
		if (que_descr->v.que.type == QUE_BATCH) {
			status = "EXITING";
		}
		else if (que_descr->v.que.type == QUE_PIPE) {
			status = "DEPARTING";
		}
		else status = "UNKNOWN";
		break;
	case SHO_RS_RUN:
		if (que_descr->v.que.type == QUE_PIPE) {
			status = "ROUTING";
		}
		else status = "RUNNING";
		break;
	case SHO_RS_STAGE:
		if (que_descr->v.que.type == QUE_BATCH) {
			status = "STAGING";
		}
		else status = "UNKNOWN";
		break;
	case SHO_RS_QUEUED:
		status = "QUEUED";
		break;
	case SHO_RS_WAIT:
		status = "WAITING";
		break;
	case SHO_RS_HOLD:
		status = "HOLDING";
		break;
	case SHO_RS_ARRIVE:
		status = "ARRIVING";
		break;
	default:
		status = "UNKNOWN";	/* We should never get here */
		break;
	}
#if UNICOS
        if (scpflag) {
                memset((char *)&scp, 0, sizeof(scp));
		/*
		 *  Qentry->reqname_v [MAX_REQNAME] is used as a flag,
		 *  telling whether the process family field is valid yet.
		 */
		jid = 0;		/* Job-id is unknown */
		if (que_descr->v.que.type != QUE_DEVICE &&
		    qentry->reqname_v [MAX_REQNAME]) {
			jid = qentry->v.process_family;
			jtabp = jtab;
			for (i = NJOB; --i >= 0; jtabp++) {
				if (jtabp->j_jid != jid) continue;
                        	scp.nice = jtabp->j_nice;
                        	scp.limit = jtabp->j_cpulimit / HZ;
				scp.cpu = (jtabp->j_ucputime + jtabp->j_scputime)
					 / HZ;
				scp.fl = jtabp->j_memuse +
					(ULSIZE * jtabp->j_nprocs);
			}
		}
		scp.jobid = jid;
                strncpy( scp.jobname, qentry->reqname_v, sizeof(scp.jobname) );
                strncpy( scp.user, username, sizeof(scp.user) );
                strncpy( scp.state, status, sizeof(scp.state) );
                scp.seqno = qentry->orig_seqno;
                cfd = getrreq ((long) qentry->orig_seqno, qentry->orig_mid,
			       &rawreq);
                if (cfd >= 0) {
                        if (!strncmp(rawreq.v.bat.stdout_name, PMAJOR,
       			     strlen(PMAJOR))) {
                                scp.scpflag = 1;
                        }
			if (jid == 0) {
				scp.nice = rawreq.v.bat.ppnice + 20;
				scp.limit = rawreq.v.bat.prcputime.max_seconds;
			}
                        close(cfd);
                }
                write( scpflag, (char *)&scp, sizeof(scp) );
                return;
        }
#else
#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
	strncpy (reqname, qentry->reqname_v, MAX_REQNAME);
	reqname [MAX_REQNAME] = '\0';	/* Force null termination */
	if (flags & SHO_R_SHORT) {
		if (que_descr->v.que.type == QUE_DEVICE) {
			printf (" %4d:%15s", nth_request, reqname);
			printf (" %9ld.%-15.15s", (long) qentry->orig_seqno,
				machinename);
			printf (" %-8.8s", username);
			printf (" %3d", qentry->priority);
			printf (" %8s", status);
			printf ("%9lu\n", qentry->size);
		} else {
#if	UNICOS
			printf ("%-15.15s", reqname);
			if (reqstate == SHO_RS_RUN &&
			    qentry->reqname_v [MAX_REQNAME]) {
				jid = qentry->v.process_family;
				jtabp = jtab;
				for (i = NJOB; --i >= 0; jtabp++) {
					if (jtabp->j_jid == jid) break;
				}
				/*
			 	*  Qentry->reqname_v [MAX_REQNAME] is used as
				*  a flag, telling whether the process family
				*  field is valid yet.
			 	*/
				printf ("%7ld", jid);
				if (i < 0) {
					printf(" COMPLETE");
					printf ("                          ");
				} else {
					printf (" %-8.8s", status);
					printf (" %3ld %6ld %6ld %7ld",
						jtabp->j_nice, jtabp->j_memuse +
						ULSIZE * jtabp->j_nprocs,
						(jtabp->j_ucputime +
							jtabp->j_scputime) / HZ,
						jtabp->j_cpulimit / HZ);
				}
			} else {
				printf ("       ");
				printf (" %-8.8s", status);
				printf ("                          ");
			}
			printf (" %-8.8s", username);
			printf (" %1ld.%.15s\n", (long) qentry->orig_seqno,
				machinename);
#else
			printf (" %4d:%15s", nth_request, reqname);
			printf (" %9ld.%-15.15s", (long) qentry->orig_seqno,
				machinename);
			printf (" %-8.8s", username);
			printf (" %3d", qentry->priority);
			printf (" %8s", status);
			if (reqstate == SHO_RS_RUN &&
			    qentry->reqname_v [MAX_REQNAME]) {
				printf("%9ld", qentry->v.process_family);
			}
			putchar('\n');
#endif
		}
	}
	else if (flags & SHO_R_MED || flags & SHO_R_LONG) { /* qstat -m or -l */
		printf ("  Request %4d:", nth_request);
		printf ("  Name=%s", reqname);
		printf ("  Id=%1ld.%s\n", (long) qentry->orig_seqno,
			machinename);
		printf ("     Owner=%s  Priority=%1d  %s  ", username,
			qentry->priority, status);
		if (reqstate == SHO_RS_RUN) {
			/*
		 	 *  The request is running or is being spawned.  Try
		 	 *  to show the process family (group).
		 	 */
			if (qentry->reqname_v [MAX_REQNAME]) {
				/*
				 *  The process-family field validity byte is
				 *  set.  Show the process-family.
				 */
				printf ("Pgrp=%ld  ",
					qentry->v.process_family);
			}
		}
		else if (reqstate == SHO_RS_WAIT) {
			/*
			 *  Show the -a time.
			 */
			printf ("%s  ", fmttime (&qentry->v.start_time));
		}
		if (que_descr->v.que.type == QUE_DEVICE) {
			printf ("Size=%lu", qentry->size);
		}
		putchar ('\n');
	}
	if (flags & SHO_R_LONG) {
		if ((cfd = getrreq ((long)qentry->orig_seqno, qentry->orig_mid,
				    &rawreq)) != -1) {
		    /*
		     *  We have a good control file.
		     */
		    printf ("  Created at %s\n",
			    fmttime (&rawreq.create_time));
		    if (que_descr->v.que.type == QUE_PIPE) {
			if (rawreq.start_time > 0) {
			    printf ("  Execute after %s\n",
				    fmttime (&rawreq.start_time));
			}
		    }
		    fputs ("  Mail = [", stdout);
		    if (rawreq.flags & RQF_BEGINMAIL) {
			if (rawreq.flags & RQF_ENDMAIL)
				fputs ("BEGIN, END]\n", stdout);
			else fputs ("BEGIN]\n", stdout);
		    }
		    else if (rawreq.flags & RQF_ENDMAIL) {
			fputs ("END]\n", stdout);
		    }
		    else fputs ("NONE]\n", stdout);
		    /*
		     *  Show where the mail will be sent.
		     */
		    printf ("  Mail address = %s@%s\n", rawreq.mail_name,
			    getmacnam (rawreq.mail_mid));
		    printf ("  Owner user name at originating ");
		    printf ("machine = %s\n", rawreq.username);
		    /*
		     *  Show request-type dependent information.
		     */
		    if (rawreq.type == RTYPE_DEVICE) {
			/*
			 *  The request is a device-oriented request.
			 */
			printf ("  Forms = ");
			if (rawreq.v.dev.forms [0] == '\0') printf ("DEFAULT");
			else fputs (rawreq.v.dev.forms, stdout);
			printf (";  Copies = %1d\n", rawreq.v.dev.copies);
		    }
		    else {
			/*
			 *  The request is not device-oriented.
			 *  The request is a batch request requiring
			 *  only the resource of a CPU.
			 */
			shoreqlims (que_descr, &rawreq);
			printf ("  Standard-error access mode = ");
			shoomd (rawreq.v.bat.stderr_acc);
			if ((rawreq.v.bat.stderr_acc & OMD_EO) == 0) {
			    if (rawreq.v.bat.stderr_acc & OMD_M_KEEP) {
				if (que_descr->v.que.type == QUE_BATCH) {
				    /*
				     *  Use the name of the local host.
				     */
				    machinename = hostname;
				}
				else machinename = "<execution-host>";
			    }
			    else {
				machinename=getmacnam(rawreq.v.bat.stderr_mid);
			    }
			    printf ("  Standard-error name = %s:%s\n",
				    machinename, rawreq.v.bat.stderr_name);
			}
			printf ("  Standard-output access mode = ");
			shoomd (rawreq.v.bat.stdout_acc);
			if (rawreq.v.bat.stdout_acc & OMD_M_KEEP) {
			    if (que_descr->v.que.type==QUE_BATCH) {
				/*
				 *  Use the name of the local
				 *  host.
				 */
				machinename = hostname;
			    }
			    else machinename = "<execution-host>";
			}
			else {
			    machinename = getmacnam (rawreq.v.bat.stdout_mid);
			}
			printf ("  Standard-output name = %s:%s\n",
				machinename, rawreq.v.bat.stdout_name);
			printf ("  Shell = ");
			if (rawreq.v.bat.shell_name [0] == '\0') {
			    printf ("DEFAULT\n");
			}
			else {	/* An explicit shell spec is given */
			    fputs (rawreq.v.bat.shell_name, stdout);
			    putchar ('\n');
			}
			printf ("  Umask = %3o\n", rawreq.v.bat.umask);
			putchar ('\n');	/* Blank line separator */
		    }
		    close (cfd);		/* Close the control file */
		}
		else if (reqstate == SHO_RS_RUN) {
		    printf ("\n  <exiting>\n\n");
		}
		else printf ("\n  <update in progress>\n\n");
	}
}


/*** shousr
 *
 *
 *	void shousr():
 *	Display a user name.
 */
static void shousr (uid)
unsigned long uid;
{
	if (acccolumn >= 4) {
		acccolumn = 0;
		printf ("\n    ");
	}
	printf ("%s\t", getusenam ((int) uid));
	acccolumn++;
}
