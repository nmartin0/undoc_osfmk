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
 
/*++ nqs_upp.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_upp.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS general parameters state update module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	June 16, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 02:53:28 $ $State: Exp $)
 * $Log: nqs_upp.c,v $
 * Revision 1.5  1994/11/19  02:53:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/07  00:52:30  kremenek
 *  Reviewer: davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 8997
 *  Testing: EATS
 *  Module(s):  cmds_libs/src/usr/lib/nqs/nqs_upp.c
 *
 * Revision 1.3  1993/11/02  00:57:46  mwan
 * R1.2 mods
 *
 * Revision 1.2  1992/10/09  22:26:41  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:47  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:09:56  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "nqs.h"
#include "nqsxvars.h"
#include "transactcc.h"		/* Transaction completion codes */

extern int errno;		/* System call error number */

extern struct gendescr *nextdb();
				/* Return next allocated entry */
				/* from a database file */
extern struct queue *nqs_fndnnq();
				/* Find non-network queue by name */
extern void seekdbb();		/* Buffered seek on a database file */
extern int strcmp();		/* String comparison */
extern char *strcpy();		/* String copy */
extern void udb_genparams();	/* Update NQS database with general NQS */
				/* operating parameters */
extern void udb_setlogfil();	/* Set NQS log file in the NQS database */
extern int upf_valfor();	/* Boolean function: valid form? */

/*** upp_setgblbatlim
 *
 *
 *	long upp_setgblbatlim():
 *
 *	Set the global runlimit for batch requests.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 */
long upp_setgblbatlim (limit)
long limit;
{
	long prevlim;			/* Previous batch request runlimit */

	/*
	 *  Set the new global batch run limit and call the spawning
	 *  routine to possibly spawn new batch requests if the limit has
	 *  increased.
	 */
	prevlim = Maxgblbatlimit;
	Maxgblbatlimit = limit;
	if (Booted) {
		udb_genparams();
	}
	if (limit > prevlim) {
		bsc_spawn();
	}
	return (TCML_COMPLETE);		/* Successful completion */
}

#ifdef SDSC

/*** upp_setsoftulim
 *
 *
 *	long upp_setsoftulim():
 *
 *	Set the the soft user_limit.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 */
long upp_setsoftulim (limit)
long limit;
{
	long prevlim;			/* Previous user_limit*/

	/*
	 *  Set the new user_limit and call the spawning
	 *  routine to possibly spawn new batch requests if the limit has
	 *  increased.
	 */
	prevlim = Maxsoftulimit;
	Maxsoftulimit = limit;
	if (Booted) {
		udb_genparams();
	}
	if (limit > prevlim) {
		bsc_spawn();
	}
                    
	fprintf (stderr, "I$soft user limit set to %d\n",  
	Maxsoftulimit);
	fflush (stderr);
	return (TCML_COMPLETE);		/* Successful completion */
}


/*** upp_sethardulim
 *
 *
 *	long upp_sethardulim():
 *
 *	Set the the hard user_limit.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful.
 */
long upp_sethardulim (limit)
long limit;
{
	long prevlim;			/* Previous user_limit*/

	/*
	 *  Set the new user_limit and call the spawning
	 *  routine to possibly spawn new batch requests if the limit has
	 *  increased.
	 */
	prevlim = Maxhardulimit;
	Maxhardulimit = limit;
	if (Booted) {
		udb_genparams();
	}
	if (limit > prevlim) {
		bsc_spawn();
	}
        fprintf (stderr, "I$hard user limit set to %d\n", 
	Maxhardulimit);
	fflush (stderr);
	return (TCML_COMPLETE);		/* Successful completion */
}

#endif

/*** upp_setdefbatque
 *
 *
 *	long upp_setdefbatque():
 *	Set the default batch-request queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHQUE:	if the named queue does not exist.
 */
long upp_setdefbatque (defbatque)
char *defbatque;		/* Name of the new default batch-req queue */
{
	register struct queue *queue;

	if (defbatque [0] != '\0') {		/* Setting a default */
		queue = nqs_fndnnq (defbatque);
		if (queue == (struct queue *)0) {
			return (TCML_NOSUCHQUE);/* No such queue */
		}
	}
	strcpy (Defbatque, defbatque);		/* Save new batch queue name */
	udb_genparams();			/* Update NQS database */
	return (TCML_COMPLETE);			/* Success */
}


/*** upp_setdefprifor
 *
 *
 *	long upp_setdefprifor():
 *	Set the default print-forms value.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if successful;
 *		TCML_NOSUCHFORM: if the named form is not defined in the
 *				 local NQS forms set.
 */
long upp_setdefprifor (defprifor)
char *defprifor;			/* Name of default print-forms */
{
	if (defprifor [0] != '\0') {	/* Setting a default */
		if (!upf_valfor (defprifor)) {
			return (TCML_NOSUCHFORM);	/* No such form */
		}
	}
	strcpy (Defprifor, defprifor);	/* Save default print-form */
	udb_genparams();		/* Update NQS database */
	return (TCML_COMPLETE);		/* Success */
}


/*** upp_setdefprique
 *
 *
 *	long upp_setdefprique():
 *	Set the default print-request queue.
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_NOSUCHQUE:	if the named queue does not exist.
 */
long upp_setdefprique (defprique)
char *defprique;		/* Name of the new default print-req queue */
{
	register struct queue *queue;

	if (defprique [0] != '\0') {	/* Setting a default */
		queue = nqs_fndnnq (defprique);
		if (queue == (struct queue *)0) {
			return (TCML_NOSUCHQUE);	/* No such queue */
		}
	}
	strcpy (Defprique, defprique);	/* Save new print-queue name */
	udb_genparams();		/* Update NQS database */
	return (TCML_COMPLETE);		/* Success */
}


/*** upp_setlogfil
 *
 *
 *	long upp_setlogfil():
 *
 *	Switch error and message logging to a new logfile.
 *	If the new logfile cannot be opened for writing, then
 *	do not switch!
 *
 *	A message is left on the old logfile if the switch is
 *	successful (or unsuccessful).
 *
 *	Returns:
 *		TCML_COMPLETE:	if successful;
 *		TCML_LOGFILERR:	if unsuccessful.
 */
long upp_setlogfil (newlogname)
register char *newlogname;		/* Name of new logfile. */
{
	void brokenpipe();		/* SIGPIPE catcher function */

#if OSF
	void (*sigpipe_sav)();		/* Save old SIGPIPE handler */
#else
	int (*sigpipe_sav)();		/* Save old SIGPIPE handler */
#endif
	char buffer [1];		/* Status buffer */

	sigpipe_sav = signal (SIGPIPE, brokenpipe);
					/* Save caller's SIGPIPE handler */
					/* and instate our own */
	errno = 0;			/* Clear system call error */
	/*
	 * If we make SURE that "$$" comes immediately after a new line,
	 * then we can safely do a blocking read on the pipe, for
	 * the log daemon is sure to write something to us.
	 */
	printf ("\n$$%s\n", newlogname);
	fflush (stdout);
	if (errno && errno != ENOTTY && errno != 45) {
		signal (SIGPIPE, sigpipe_sav); /* Restore SIGPIPE handler */
		return (TCML_LOGFILERR);
	}
	/*
	 *  Successful write to the NQS log process.
	 */
	if (read (Fromlog, buffer, 1) != 1) {	/* Blocking read on pipe */
		/*
		 *  Status readback from log process failed.
		 */
		signal (SIGPIPE, sigpipe_sav); /* Restore SIGPIPE handler */
		return (TCML_LOGFILERR);
	}
	signal (SIGPIPE, sigpipe_sav);	/* Restore SIGPIPE handler */
	if (buffer [0]) {
		/*
		 *  Success!  The new logfile has been opened.
		 */
		strcpy (Logfilename, newlogname);
					/* Remember new logfile name */
		udb_setlogfil();	/* Update log file name in */
					/* NQS database */
		return (TCML_COMPLETE);
	}
	return (TCML_LOGFILERR);	/* Failure */
}


/*** brokenpipe
 *
 *
 *	void brokenpipe ():
 *
 *	This function serves as the signal catcher for SIGPIPE signals
 *	sent in case the NQS log process is not running anymore.
 *
 *	This function sets errno = EPIPE
 */
static void brokenpipe ()
{
	signal (SIGPIPE, brokenpipe);
	errno = EPIPE;
}

