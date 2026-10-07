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
 
/*++ nqs_udb.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_udb.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS update operating configuration/database files.
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
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:53:24 $ $State: Exp $)
 * $Log: nqs_udb.c,v $
 * Revision 1.4  1994/11/19  02:53:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:57:43  mwan
 * R1.2 mods
 *
 * Revision 1.2  1992/10/09  22:26:30  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:39  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:08:31  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_udb.c	1.2 (nqs_udb.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include "nqs.h"
#include "nqsxvars.h"			/* External global variables */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>			/* File control */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include <signal.h>


/*
 *	External functions:
 */
extern long allodb();			/* Allocate database descr */
extern void rewritedb();		/* Rewrite a database descr */
extern void freedb();			/* Free a database descr */
extern long lseek();			/* System call returns other than int */
extern struct gendescr *nextdb();	/* Return next allocated entry */
					/* from a database file */
extern void nqs_abort();		/* Abort NQS execution */
extern void seekdbb();			/* Buffered seek on a database file */
extern int setmtime();			/* Synchronously set a file */
					/* modification time */
extern int strcmp();			/* String comparison */
extern char *strcpy();			/* Copy string */
extern char *strncpy();			/* Anchored string copy */
extern long telldb();			/* Return address of last */
					/* descriptor read */

/*** udb_addfor
 *
 *
 *	void udb_addfor():
 *	Add a form to the NQS forms database.
 *
 *	WARNING:
 *		It is assumed that the forms do not already
 *		exist in the forms database.
 */
void udb_addfor (forms)
register char *forms;			/* Forms name to be added */
{
	struct gendescr newdescr;

	strcpy (newdescr.v.form.forms, forms);
	allodb (Formsfile, &newdescr, DSC_FORM);
}


/*** udb_addnqsman
 *
 *
 *	void udb_addnqsman():
 *	Add an NQS manager account to the NQS database.
 *
 *	WARNING:
 *		If is assumed that the specified account
 *		is not already present in the NQS manager
 *		access set.
 */
void udb_addnqsman (account_uid, account_mid, privilege_bits)
uid_t account_uid;			/* Manager account user-id */
mid_t account_mid;			/* Manager account machine-id */
int privilege_bits;			/* Privilege bits */
{
	struct gendescr newdescr;

	newdescr.v.mgr.privileges = privilege_bits;
	newdescr.v.mgr.manager_uid = account_uid;
	newdescr.v.mgr.manager_mid = account_mid;
	allodb (Mgrfile, &newdescr, DSC_MGR);
}


/*** udb_addquedes
 *
 *
 *	void udb_addquedes():
 *	Add a pipe queue destination to the NQS database.
 */
void udb_addquedes (queue, map)
struct queue *queue;			/* Ptr to local queue */
struct qdestmap *map;			/* Queue/destination map */
{
	struct gendescr descr;
	register struct pipeto *pipeto;	/* Destination */

	pipeto = map->pipeto;
	descr.v.map.qtodevmap = 0;	/* Queue/destination mapping */
	strcpy (descr.v.map.v.qdestmap.lqueue, queue->q.namev.name);
	strcpy (descr.v.map.v.qdestmap.rqueue, pipeto->rqueue);
	descr.v.map.v.qdestmap.rhost_mid = pipeto->rhost_mid;
	map->mapno = allodb (Qmapfile, &descr, DSC_QMAP);
}


/*** udb_addquedev
 *
 *
 *	void udb_addquedev():
 *	Add a queue/device mapping to the NQS database.
 */
void udb_addquedev (map)
struct qdevmap *map;		/* Queue to device and vice versa map */
{
	struct gendescr descr;

	descr.v.map.qtodevmap = 1;	/* Queue/device mapping */
	strcpy (descr.v.map.v.qdevmap.qname, map->queue->q.namev.name);
	strcpy (descr.v.map.v.qdevmap.dname, map->device->name);
	map->mapno = allodb (Qmapfile, &descr, DSC_QMAP);
}


/*** udb_crecom
 *
 *
 *	void udb_crecom():
 *	Create a queue complex.
 */
void udb_crecom (qcomplex)
struct qcomplex *qcomplex;		/* Ptr to queue complex structure */
{
	struct gendescr descr;

	bytezero( (char *)&descr, sizeof(struct gendescr) );
	descr.v.qcom.runlimit = qcomplex->runlimit;
	strcpy(descr.v.qcom.name, qcomplex->name);
	qcomplex->qcomplexno = allodb (Qcomplexfile, &descr, DSC_QCOMPLEX);
}


/*** udb_credes
 *
 *
 *	void udb_credes():
 *	Add a queue destination descriptor to the NQS database.
 */
void udb_credes (dest)
struct pipeto *dest;			/* Pipe queue destination structure */
{
	struct gendescr descr;

	descr.v.dest.status = dest->status;
	descr.v.dest.retry_at = dest->retry_at;
	descr.v.dest.retrytime = dest->retrytime;
	descr.v.dest.retrydelta = dest->retrydelta;
	strcpy (descr.v.dest.rqueue, dest->rqueue);
	descr.v.dest.rhost_mid = dest->rhost_mid;
	dest->pipetono = allodb (Pipeqfile, &descr, DSC_PIPEQDEST);
}


/*** udb_credev
 *
 *
 *	void udb_credev():
 *	Add a device to the NQS database.
 */
void udb_credev (device)
struct device *device;			/* Device structure */
{
	struct gendescr descr;

	descr.v.dev.status = device->status;
	strcpy (descr.v.dev.statmsg, device->statmsg);
	descr.v.dev.orig_seqno = 0;
	descr.v.dev.uid = 0;
	descr.v.dev.orig_mid = 0;
	descr.v.dev.reqname [0] = '\0';
	descr.v.dev.qname [0] = '\0';
	strcpy (descr.v.dev.forms, device->forms);
	strcpy (descr.v.dev.dname, device->name);
	strcpy (descr.v.dev.server, device->server);
	strcpy (descr.v.dev.fullname, device->fullname);
	device->deviceno = allodb (Devicefile, &descr, DSC_DEVICE);
}


/*** udb_creque
 *
 *
 *	void udb_creque():
 *	Add a queue to the NQS database.
 */
void udb_creque (queue)
struct queue *queue;			/* Queue structure */
{
/*
 *	Uncomment the declaration of: i, if your compiler is not capable
 *	of assigning entire structures.
 *
 *	register int i;
 */
	struct gendescr descr;

	descr.v.que = queue->q;		/* Copy in queue description */
/*
 *	If your compiler is not capable of assigning entire structures,
 *	then you must uncomment the following code replacing the single
 *	assignment statement above.
 *
 *	descr.v.que.orderversion = queue->q.orderversion;
 *	descr.v.que.accessversion = queue->q.accessversion;
 *	descr.v.que.status = queue->q.status;
 *	descr.v.que.type = queue->q.type;
 *	descr.v.que.priority = queue->q.priority;
 * #if	UNICOS | SGI | SYS52 | UTS | OSF
 *	descr.v.que.ru_stime = queue->q.ru_stime;
 *	descr.v.que.ru_utime = queue->q.ru_utime;
 * #else
 * #if	BSD42 | BSD43 | ULTRIX
 *	descr.v.que.ru_stime_usec = queue->q.ru_stime_usec;
 *	descr.v.que.ru_stime = queue->q.ru_stime;
 *	descr.v.que.ru_utime_usec = queue->q.ru_utime_usec;
 *	descr.v.que.ru_utime = queue->q.ru_utime;
 * #else
 * BAD SYSTEM TYPE
 * #endif
 * #endif
 *	descr.v.que.departcount = queue->q.departcount;
 *	descr.v.que.runcount = queue->q.runcount;
 *	descr.v.que.stagecount = queue->q.stagecount;
 *	descr.v.que.queuedcount = queue->q.queuedcount;
 *	descr.v.que.waitcount = queue->q.waitcount;
 *	descr.v.que.holdcount = queue->q.holdcount;
 *	descr.v.que.arrivecount = queue->q.arrivecount;
 *	if (queue->q.type == QUE_BATCH) {
 *	    strcpy (descr.v.que.namev.name, queue->q.namev.name);
 *	    for (i = 0; i < BSET_CPUVECSIZE; i++) {
 *		descr.v.que.v1.batch.cpuset [i] = queue->q.v1.batch.cpuset [i];
 *		descr.v.que.v1.batch.cpuuse [i] = queue->q.v1.batch.cpuuse [i];
 *	    }
 *	    descr.v.que.v1.batch.ncpuset = queue->q.v1.batch.ncpuset;
 *	    descr.v.que.v1.batch.ncpuuse = queue->q.v1.batch.ncpuuse;
 *	    for (i = 0; i < BSET_RS1VECSIZE; i++) {
 *		descr.v.que.v1.batch.rs1set [i] = queue->q.v1.batch.rs1set [i];
 *		descr.v.que.v1.batch.rs1use [i] = queue->q.v1.batch.rs1use [i];
 *	    }
 *	    descr.v.que.v1.batch.nrs1set = queue->q.v1.batch.nrs1set;
 *	    descr.v.que.v1.batch.nrs1use = queue->q.v1.batch.nrs1use;
 *	    for (i = 0; i < BSET_RS2VECSIZE; i++) {
 *		descr.v.que.v1.batch.rs2set [i] = queue->q.v1.batch.rs2set [i];
 *		descr.v.que.v1.batch.rs2use [i] = queue->q.v1.batch.rs2use [i];
 *	    }
 *	    descr.v.que.v1.batch.nrs2set = queue->q.v1.batch.nrs2set;
 *	    descr.v.que.v1.batch.nrs2use = queue->q.v1.batch.nrs2use;
 *	    descr.v.que.v1.batch.runlimit = queue->q.v1.batch.runlimit;
 *	    descr.v.que.v1.batch.explicit = queue->q.v1.batch.explicit;
 *	    descr.v.que.v1.batch.infinite = queue->q.v1.batch.infinite;
 *	    descr.v.que.v1.batch.ppcoreunits = queue->q.v1.batch.ppcoreunits;
 *	    descr.v.que.v1.batch.ppdataunits = queue->q.v1.batch.ppdataunits;
 *	    descr.v.que.v1.batch.pppfileunits = queue->q.v1.batch.pppfileunits;
 *	    descr.v.que.v1.batch.prpfileunits = queue->q.v1.batch.prpfileunits;
 *	    descr.v.que.v1.batch.ppqfileunits = queue->q.v1.batch.ppqfileunits;
 *	    descr.v.que.v1.batch.prqfileunits = queue->q.v1.batch.prqfileunits;
 *	    descr.v.que.v1.batch.pptfileunits = queue->q.v1.batch.pptfileunits;
 *	    descr.v.que.v1.batch.prtfileunits = queue->q.v1.batch.prtfileunits;
 *	    descr.v.que.v1.batch.ppmemunits = queue->q.v1.batch.ppmemunits;
 *	    descr.v.que.v1.batch.prmemunits = queue->q.v1.batch.prmemunits;
 *	    descr.v.que.v1.batch.ppstackunits = queue->q.v1.batch.ppstackunits;
 *	    descr.v.que.v1.batch.ppworkunits = queue->q.v1.batch.ppworkunits;
 *	    descr.v.que.v1.batch.ppcorecoeff = queue->q.v1.batch.ppcorecoeff;
 *	    descr.v.que.v1.batch.ppdatacoeff = queue->q.v1.batch.ppdatacoeff;
 *	    descr.v.que.v1.batch.pppfilecoeff = queue->q.v1.batch.pppfilecoeff;
 *	    descr.v.que.v1.batch.prpfilecoeff = queue->q.v1.batch.prpfilecoeff;
 *	    descr.v.que.v1.batch.ppqfilecoeff = queue->q.v1.batch.ppqfilecoeff;
 *	    descr.v.que.v1.batch.prqfilecoeff = queue->q.v1.batch.prqfilecoeff;
 *	    descr.v.que.v1.batch.pptfilecoeff = queue->q.v1.batch.pptfilecoeff;
 *	    descr.v.que.v1.batch.prtfilecoeff = queue->q.v1.batch.prtfilecoeff;
 *	    descr.v.que.v1.batch.ppmemcoeff = queue->q.v1.batch.ppmemcoeff;
 *	    descr.v.que.v1.batch.prmemcoeff = queue->q.v1.batch.prmemcoeff;
 *	    descr.v.que.v1.batch.ppstackcoeff = queue->q.v1.batch.ppstackcoeff;
 *	    descr.v.que.v1.batch.ppworkcoeff = queue->q.v1.batch.ppworkcoeff;
 *	    descr.v.que.v1.batch.ppcpusecs = queue->q.v1.batch.ppcpusecs;
 *	    descr.v.que.v1.batch.ppcpums = queue->q.v1.batch.ppcpums;
 *	    descr.v.que.v1.batch.prcpusecs = queue->q.v1.batch.prcpusecs;
 *	    descr.v.que.v1.batch.prcpums = queue->q.v1.batch.prcpums;
 *	    descr.v.que.v1.batch.ppnice = queue->q.v1.batch.ppnice;
 *	    descr.v.que.v1.batch.prdrives = queue->q.v1.batch.prdrives;
 *	    descr.v.que.v1.batch.prncpus = queue->q.v1.batch.prncpus;
 *	}
 *	else if (queue->q.type == QUE_DEVICE) {
 *	    strcpy (descr.v.que.namev.name, queue->q.namev.name);
 *	}
 *	else if (queue->q.type == QUE_NET) {
 *	    descr.v.que.namev.to_destination = queue->q.namev.to_destination;
 *	    descr.v.que.v1.network.runlimit = queue->q.v1.network.runlimit;
 *	    strcpy (descr.v.que.v1.network.server, queue->q.v1.network.server);
 *	    descr.v.que.v1.network.retry_at = queue->q.v1.network.retry_at;
 *	    descr.v.que.v1.network.retrytime = queue->q.v1.network.retrytime;
 *	    descr.v.que.v1.network.retrydelta = queue->q.v1.network.retrydelta;
 *	}
 *	else {
 *	    strcpy (descr.v.que.namev.name, queue->q.namev.name);
 *	    descr.v.que.v1.pipe.runlimit = queue->q.v1.pipe.runlimit;
 *	    strcpy (descr.v.que.v1.pipe.server, queue->q.v1.pipe.server);
 *	}
 */
	queue->queueno = allodb (Queuefile, &descr, DSC_QUEUE);
}


/*** udb_delcom
 *
 *
 *	void udb_delcom():
 *	Delete a queue complex.
 */
void udb_delcom (qcomplex)
struct qcomplex *qcomplex;		/* Ptr to queue complex structure */
{
	freedb (Qcomplexfile, qcomplex->qcomplexno);
}


/*** udb_deldes
 *
 *
 *	void udb_deldes():
 *	Delete a queue destination descriptor from the NQS database.
 */
void udb_deldes (dest)
struct pipeto *dest;			/* Pipe queue destination structure */
{
	freedb (Pipeqfile, dest->pipetono);
}


/*** udb_deldev
 *
 *
 *	void udb_deldev():
 *	Delete a device descriptor from the NQS database.
 */
void udb_deldev (device)
struct device *device;			/* Device to be deleted */
{
	freedb (Devicefile, device->deviceno);
}



/*** udb_delfor
 *
 *
 *	void udb_delfor():
 *	Delete a form from the NQS forms database.
 */
void udb_delfor (forms)
char *forms;				/* Forms to be deleted */
{
	register struct gendescr *descr;

	seekdbb (Formsfile, 0L);	/* Seek to the beginning of the NQS */
					/* forms list file */
	/*
	 *  Search to delete entry.
	 */
	descr = nextdb (Formsfile);
	while (descr != (struct gendescr *) 0) {
		if (strcmp (descr->v.form.forms, forms) == 0) {
			/*
			 *  The forms have been found.
			 */
			freedb (Formsfile, telldb (Formsfile));
							/* Free the entry */
			descr = (struct gendescr *) 0;	/* Set to exit */
		}
		else descr = nextdb (Formsfile);
	
	}
}


/*** udb_delnqsman
 *
 *
 *	void udb_delnqsman():
 *	Delete an NQS manager from the NQS manager database.
 */
void udb_delnqsman (account_uid, account_mid)
uid_t account_uid;			/* Manager account user-id */
mid_t account_mid;			/* Manager account machine-id */
{
	register struct gendescr *descr;

	seekdbb (Mgrfile, 0L);		/* Seek to the beginning of the NQS */
					/* manager access list file */
	/*
	 *  Search to delete entry.
	 */
	descr = nextdb (Mgrfile);
	while (descr != (struct gendescr *) 0) {
		if (descr->v.mgr.manager_uid == account_uid &&
		    descr->v.mgr.manager_mid == account_mid) {
			/*
			 *  The account-name has been found.
			 */
			freedb (Mgrfile, telldb (Mgrfile));/* Free the entry */
			descr = (struct gendescr *) 0;	   /* Set to exit */
		}
		else descr = nextdb (Mgrfile);
	
	}
}


/*** udb_delque
 *
 *
 *	void udb_delque():
 *
 *	Delete a queue descriptor and any associated queue-ordering
 *	or queue access file from the NQS database.
 */
void udb_delque (queue)
struct queue *queue;				/* Queue to be deleted */
{
	char path [MAX_PATHNAME+1];
	sprintf (path, "%s/q%08x%04x", Nqs_qorder, queue->queueno,
		 queue->q.orderversion);
	unlink (path);				/* Delete queue-order file */
	if (queue->q.type != QUE_NET) {
		/*
		 *  Delete any existing queue access file for the queue.
		 *  This is quite critical, and code in upq_delque()
		 *  (../src/nqs_upq.c) relies on this.
		 */
		sprintf (path, "%s/q%08x%04x", Nqs_qaccess, queue->queueno,
		 	 queue->q.accessversion);
		unlink (path);			/* Delete queue access file */
	}
	freedb (Queuefile, queue->queueno);	/* Release queue descriptor */
}


/*** udb_delquedes
 *
 *
 *	void udb_delquedes():
 *	Delete a pipe queue destination from the NQS database.
 */
void udb_delquedes (map)
struct qdestmap *map;			/* Queue/dest mapping to be deleted */
{
	freedb (Qmapfile, map->mapno);
}


/*** udb_delquedev
 *
 *
 *	void udb_delquedev():
 *	Delete a queue/device mapping from the NQS database.
 */
void udb_delquedev (map)
struct qdevmap *map;			/* Queue/device map to be deleted */
{
	freedb (Qmapfile, map->mapno);
}


/*** udb_destination
 *
 *
 *	void udb_destination():
 *	Update the destination state stored in the NQS database.
 */
void udb_destination (dest)
struct pipeto *dest;			/* Pipe structure */
{
	struct gendescr descr;

	descr.v.dest.status = dest->status;
	descr.v.dest.retry_at = dest->retry_at;
	descr.v.dest.retrytime = dest->retrytime;
	descr.v.dest.retrydelta = dest->retrydelta;
	strcpy (descr.v.dest.rqueue, dest->rqueue);
	descr.v.dest.rhost_mid = dest->rhost_mid;
	rewritedb (Pipeqfile, dest->pipetono, &descr, DSC_PIPEQDEST);
}


/*** udb_device
 *
 *
 *	void udb_device():
 *	Update the device state stored in the NQS database.
 */
void udb_device (device)
struct device *device;
{
	struct gendescr descr;
	register struct request *request;

	descr.v.dev.status = device->status;
	strcpy (descr.v.dev.statmsg, device->statmsg);
	request = device->curreq;
	if (request != (struct request *) 0) {
		descr.v.dev.orig_seqno = request->v1.req.orig_seqno;
		descr.v.dev.uid = request->v1.req.uid;
		descr.v.dev.orig_mid = request->v1.req.orig_mid;
		strcpy (descr.v.dev.reqname, request->v1.req.reqname);
		strcpy (descr.v.dev.qname, device->curque->q.namev.name);
	}
	else {
		descr.v.dev.orig_seqno = 0;
		descr.v.dev.uid = 0;
		descr.v.dev.orig_mid = 0;
		descr.v.dev.reqname [0] = '\0';
		descr.v.dev.qname [0] = '\0';
	}
	strcpy (descr.v.dev.forms, device->forms);
	strcpy (descr.v.dev.dname, device->name);
	strcpy (descr.v.dev.server, device->server);
	strcpy (descr.v.dev.fullname, device->fullname);
	rewritedb (Devicefile, device->deviceno, &descr, DSC_DEVICE);
}


/*** udb_genparams
 *
 *
 *	void udb_genparams():
 *	Update the general parameters record in the NQS database.
 */
void udb_genparams ()
{
	struct gendescr gendescr;

	/*
	 *  Completely format the general NQS operating parameters
	 *  record.
	 */
	gendescr.v.par.paramtype = PRM_GENPARAMS;
	gendescr.v.par.v.genparams.mail_uid = Mail_uid;
	gendescr.v.par.v.genparams.maxgblacclimit = Maxgblacclimit;
	gendescr.v.par.v.genparams.maxgblbatlimit = Maxgblbatlimit;
	gendescr.v.par.v.genparams.maxgblnetlimit = Maxgblnetlimit;
	gendescr.v.par.v.genparams.maxgblpiplimit = Maxgblpiplimit;
	gendescr.v.par.v.genparams.plockdae = Plockdae;
	gendescr.v.par.v.genparams.shell_strategy = Shell_strategy;
	gendescr.v.par.v.genparams.termsignal = Termsignal;
#ifdef SDSC
	gendescr.v.par.v.genparams.maxsoftulimit = Maxsoftulimit;
	gendescr.v.par.v.genparams.maxhardulimit = Maxhardulimit;
#else
	gendescr.v.par.v.genparams.sreserved1 = 0;
	gendescr.v.par.v.genparams.sreserved2 = 0;
#endif
	gendescr.v.par.v.genparams.sreserved3 = 0;
	gendescr.v.par.v.genparams.sreserved4 = 0;
	gendescr.v.par.v.genparams.debug = Debug;
	gendescr.v.par.v.genparams.defbatpri = Defbatpri;
	gendescr.v.par.v.genparams.defdevpri = Defdevpri;
	gendescr.v.par.v.genparams.maxcopies = Maxcopies;
	gendescr.v.par.v.genparams.maxextrequests = Maxextrequests;
	gendescr.v.par.v.genparams.maxoperet = Maxoperet;
	gendescr.v.par.v.genparams.maxprint = Maxprint;
	gendescr.v.par.v.genparams.opewai = Opewai;
	gendescr.v.par.v.genparams.ireserved1 = 0;
	gendescr.v.par.v.genparams.ireserved2 = 0;
	gendescr.v.par.v.genparams.ireserved3 = 0;
	gendescr.v.par.v.genparams.ireserved4 = 0;
	gendescr.v.par.v.genparams.defdesrettim = Defdesrettim;
	gendescr.v.par.v.genparams.defdesretwai = Defdesretwai;
	gendescr.v.par.v.genparams.defnetrettim = Defnetrettim;
	gendescr.v.par.v.genparams.defnetretwai = Defnetretwai;
	gendescr.v.par.v.genparams.lifetime = Lifetime;
	gendescr.v.par.v.genparams.lreserved1 = 0;
	gendescr.v.par.v.genparams.lreserved2 = 0;
	gendescr.v.par.v.genparams.lreserved3 = 0;
	gendescr.v.par.v.genparams.lreserved4 = 0;
	strcpy (gendescr.v.par.v.genparams.defprifor, Defprifor);
	strcpy (gendescr.v.par.v.genparams.defbatque, Defbatque);
	strcpy (gendescr.v.par.v.genparams.defprique, Defprique);
	strcpy (gendescr.v.par.v.genparams.fixed_shell, Fixed_shell);
	strcpy (gendescr.v.par.v.genparams.netdaemon, Netdaemon);
	if (Udbgenparams == -1) {
		/*
		 *  No general parameters file record has ever been
		 *  allocated.
		 */
		Udbgenparams = allodb (Paramfile, &gendescr, DSC_PARAM);
	}
	else rewritedb (Paramfile, Udbgenparams, &gendescr, DSC_PARAM);
	/*
	 *  Signal the NQS network daemon that the NQS general operating
	 *  parameters have been modified, and that it should update
	 *  its internal configuration variables as necessary.
	 */
	if (Netdaepid > 0) kill (Netdaepid, SIGINT);
}


/*** udb_netprocs
 *
 *
 *	void udb_netprocs():
 *	Update the network processes record in the NQS database.
 */
void udb_netprocs()
{
	struct gendescr gendescr;

	/*
	 *  Completely format the network processes record.
	 */
	gendescr.v.par.paramtype = PRM_NETPROCS;
	strcpy (gendescr.v.par.v.netprocs.netclient, Netclient);
	strcpy (gendescr.v.par.v.netprocs.netserver, Netserver);
	if (Udbnetprocs == -1) {
		/*
		 *  No network processes record has been allocated.
		 */
		Udbnetprocs = allodb (Paramfile, &gendescr, DSC_PARAM);
	}
	else rewritedb (Paramfile, Udbnetprocs, &gendescr, DSC_PARAM);
	/*
	 *  Signal the NQS network daemon that the NQS general operating
	 *  parameters have been modified, and that it should update
	 *  its internal configuration variables as necessary.
	 */
	if (Netdaepid > 0) kill (Netdaepid, SIGINT);
}


/*** udb_qaccess
 *
 *
 *	void udb_qaccess():
 *
 *	Update the access file for the specified queue
 *	in the NQS database.
 */
void udb_qaccess (queue)
struct queue *queue;
{
	void udb_queue();				/* In this file */

	unsigned long buffer [QAFILE_CACHESIZ];		/* Output buffer */
	char path [MAX_PATHNAME+1];
	register int index;
	register int fd;
	register struct acclist *acclistp;
	register int newversion;
	int oldversion;

	oldversion = queue->q.accessversion;
	if (queue->q.status & QUE_BYGIDUID) {
		/*
		 *  Create a new queue access file.
		 */
		if (oldversion == 32767) newversion = 0;
		else newversion = oldversion + 1;
		queue->q.accessversion = newversion;
		sprintf (path, "%s/q%08x%04x", Nqs_qaccess, queue->queueno,
			 newversion);
		if ((fd = open (path, O_WRONLY | O_CREAT |
				      O_TRUNC, 0644)) == -1) {
			printf ("E$Unable to open/create ");
			printf ("queue access file in udb_qaccess().\n");
			fflush (stdout);
		}
		else {
			/*
			 *  The new queue access file was created and opened
			 *  successfully.
			 */
			index = 0;
			switch (queue->q.type) {
			case QUE_BATCH:
				acclistp = queue->v1.batch.acclistp;
				break;
			case QUE_DEVICE:
				acclistp = queue->v1.device.acclistp;
				break;
			case QUE_PIPE:
				acclistp = queue->v1.pipe.acclistp;
				break;
			}
			while (acclistp != (struct acclist *) 0) {
				index = udb_qapack (fd, buffer, index,
					acclistp);
				acclistp = acclistp->next;
			}
			udb_qapack (fd, buffer, index, (struct acclist *) 0);
			close (fd);
		}
	}
	/*
	 *  Delete the old queue access file for this queue.
	 *  If we have gone to unrestricted, there is no new access file.
	 */
	sprintf (path, "%s/q%08x%04x", Nqs_qaccess, queue->queueno, oldversion);
	unlink (path);			/* Delete old queue access file */
	/*
	 *  Now, update queue state.
	 */
	udb_queue (queue);		/* Update queue header */
}


/*** udb_qorder
 *
 *
 *	void udb_qorder():
 *
 *	Update the req ordering file for the specified queue
 *	in the NQS database.
 */
void udb_qorder (queue)
struct queue *queue;
{
	void udb_queue();				/* In this file */
	struct qentry buffer [QOFILE_CACHESIZ];		/* Output buffer */
	char path [MAX_PATHNAME+1];
	register int index;
	register int fd;
	register struct request *request;
	register int newversion;
	int oldversion;

	oldversion = queue->q.orderversion;
	if (queue->q.departcount || queue->q.runcount || queue->q.stagecount ||
	    queue->q.queuedcount || queue->q.waitcount || queue->q.holdcount ||
	    queue->q.arrivecount) {
		/*
		 *  Create a new queue ordering file.
		 */
		if (oldversion == 32767) newversion = 0;
		else newversion = oldversion + 1;
		queue->q.orderversion = newversion;
		if (queue->q.type == QUE_NET) {
			sprintf (path, "%s/n%08x%04x", Nqs_qorder,
				 queue->queueno, newversion);
		}
		else {
			sprintf (path, "%s/q%08x%04x", Nqs_qorder,
				 queue->queueno, newversion);
		}
		if ((fd = open (path, O_WRONLY | O_CREAT |
				      O_TRUNC, 0644)) == -1) {
			printf ("E$Unable to open/create ");
			printf ("queue order file in udb_qorder().\n");
			fflush (stdout);
		}
		else {
			/*
			 *  The new queue-ordering file was created and opened
			 *  successfully.
			 */
			index = 0;
			request = queue->departset;
			while (request != (struct request *) 0) {
				index = udb_qopack (fd, buffer, index, request,
						    queue->q.type, 0);
				request = request->next;
			}
			request = queue->runset;
			while (request != (struct request *) 0) {
				index = udb_qopack (fd, buffer, index, request,
						    queue->q.type, 1);
				request = request->next;
			}
			request = queue->stageset;
			while (request != (struct request *) 0) {
				index = udb_qopack (fd, buffer, index, request,
						    queue->q.type, 0);
				request = request->next;
			}
			request = queue->queuedset;
			while (request != (struct request *) 0) {
				index = udb_qopack (fd, buffer, index, request,
						    queue->q.type, 0);
				request = request->next;
			}
			request = queue->waitset;
			while (request != (struct request *) 0) {
				index = udb_qopack (fd, buffer, index, request,
						    queue->q.type, 0);
				request = request->next;
			}
			request = queue->holdset;
			while (request != (struct request *) 0) {
				index = udb_qopack (fd, buffer, index, request,
						    queue->q.type, 0);
				request = request->next;
			}
			request = queue->arriveset;
			while (request != (struct request *) 0) {
				index = udb_qopack (fd, buffer, index, request,
						    queue->q.type, 0);
				request = request->next;
			}
			udb_qopack (fd, buffer, index, (struct request *) 0,
				    queue->q.type, 0);
			close (fd);
		}
	}
	udb_queue (queue);		/* Update queue header */
	/*
	 *  Delete the old queue ordering file for this queue.
	 */
	sprintf (path, "%s/q%08x%04x", Nqs_qorder, queue->queueno, oldversion);
	unlink (path);			/* Delete old queue-order file */
}


/*** udb_quecom
 *
 *
 *	void udb_quecom():
 *	Update the database for a queue complex.
 */
void udb_quecom (qcomplex)
struct qcomplex *qcomplex;		/* Ptr to queue complex structure */
{
	struct gendescr descr;
	register int	i;

	bytezero( (char *)&descr, sizeof(struct gendescr) );
	descr.v.qcom.runlimit = qcomplex->runlimit;
	strcpy(descr.v.qcom.name, qcomplex->name);
	for (i = 0; i < MAX_QSPERCOMPLX; i++) {
		if (qcomplex->queue[i] == (struct queue *)0) continue;
		strcpy(descr.v.qcom.queues[i],
			qcomplex->queue[i]->q.namev.name);
	}
	rewritedb (Qcomplexfile, qcomplex->qcomplexno, &descr, DSC_QCOMPLEX);
}


/*** udb_queue
 *
 *
 *	void udb_queue():
 *
 *	Update the queue state stored in the NQS database.
 *	This does NOT update the request ordering file for the queue.
 *	This function DOES clear the QUE_UPDATE bit.
 */
void udb_queue (queue)
struct queue *queue;
{
/*
 *	Uncomment the declaration of: i, if your compiler is not capable
 *	of assigning entire structures.
 *
 *	register int i;
 */
	struct gendescr descr;

	queue->q.status &= ~QUE_UPDATE;	/* Clear update bit */
	descr.v.que = queue->q;		/* Copy in queue description */
/*
 *	If your compiler is not capable of assigning entire structures,
 *	then you must uncomment the following code replacing the single
 *	assignment statement above.
 *
 *	descr.v.que.orderversion = queue->q.orderversion;
 *	descr.v.que.accessversion = queue->q.accessversion;
 *	descr.v.que.status = queue->q.status;
 *	descr.v.que.type = queue->q.type;
 *	descr.v.que.priority = queue->q.priority;
 * #if	UNICOS | SGI | SYS52 | UTS | OSF
 *	descr.v.que.ru_stime = queue->q.ru_stime;
 *	descr.v.que.ru_utime = queue->q.ru_utime;
 * #else
 * #if	BSD42 | BSD43 | ULTRIX
 *	descr.v.que.ru_stime_usec = queue->q.ru_stime_usec;
 *	descr.v.que.ru_stime = queue->q.ru_stime;
 *	descr.v.que.ru_utime_usec = queue->q.ru_utime_usec;
 *	descr.v.que.ru_utime = queue->q.ru_utime;
 * #else
 * BAD SYSTEM TYPE
 * #endif
 * #endif
 *	descr.v.que.departcount = queue->q.departcount;
 *	descr.v.que.runcount = queue->q.runcount;
 *	descr.v.que.stagecount = queue->q.stagecount;
 *	descr.v.que.queuedcount = queue->q.queuedcount;
 *	descr.v.que.waitcount = queue->q.waitcount;
 *	descr.v.que.holdcount = queue->q.holdcount;
 *	descr.v.que.arrivecount = queue->q.arrivecount;
 *	if (queue->q.type == QUE_BATCH) {
 *	    strcpy (descr.v.que.namev.name, queue->q.namev.name);
 *	    for (i = 0; i < BSET_CPUVECSIZE; i++) {
 *		descr.v.que.v1.batch.cpuset [i] = queue->q.v1.batch.cpuset [i];
 *		descr.v.que.v1.batch.cpuuse [i] = queue->q.v1.batch.cpuuse [i];
 *	    }
 *	    descr.v.que.v1.batch.ncpuset = queue->q.v1.batch.ncpuset;
 *	    descr.v.que.v1.batch.ncpuuse = queue->q.v1.batch.ncpuuse;
 *	    for (i = 0; i < BSET_RS1VECSIZE; i++) {
 *		descr.v.que.v1.batch.rs1set [i] = queue->q.v1.batch.rs1set [i];
 *		descr.v.que.v1.batch.rs1use [i] = queue->q.v1.batch.rs1use [i];
 *	    }
 *	    descr.v.que.v1.batch.nrs1set = queue->q.v1.batch.nrs1set;
 *	    descr.v.que.v1.batch.nrs1use = queue->q.v1.batch.nrs1use;
 *	    for (i = 0; i < BSET_RS2VECSIZE; i++) {
 *		descr.v.que.v1.batch.rs2set [i] = queue->q.v1.batch.rs2set [i];
 *		descr.v.que.v1.batch.rs2use [i] = queue->q.v1.batch.rs2use [i];
 *	    }
 *	    descr.v.que.v1.batch.nrs2set = queue->q.v1.batch.nrs2set;
 *	    descr.v.que.v1.batch.nrs2use = queue->q.v1.batch.nrs2use;
 *	    descr.v.que.v1.batch.runlimit = queue->q.v1.batch.runlimit;
 *	    descr.v.que.v1.batch.explicit = queue->q.v1.batch.explicit;
 *	    descr.v.que.v1.batch.infinite = queue->q.v1.batch.infinite;
 *	    descr.v.que.v1.batch.ppcoreunits = queue->q.v1.batch.ppcoreunits;
 *	    descr.v.que.v1.batch.ppdataunits = queue->q.v1.batch.ppdataunits;
 *	    descr.v.que.v1.batch.pppfileunits = queue->q.v1.batch.pppfileunits;
 *	    descr.v.que.v1.batch.prpfileunits = queue->q.v1.batch.prpfileunits;
 *	    descr.v.que.v1.batch.ppqfileunits = queue->q.v1.batch.ppqfileunits;
 *	    descr.v.que.v1.batch.prqfileunits = queue->q.v1.batch.prqfileunits;
 *	    descr.v.que.v1.batch.pptfileunits = queue->q.v1.batch.pptfileunits;
 *	    descr.v.que.v1.batch.prtfileunits = queue->q.v1.batch.prtfileunits;
 *	    descr.v.que.v1.batch.ppmemunits = queue->q.v1.batch.ppmemunits;
 *	    descr.v.que.v1.batch.prmemunits = queue->q.v1.batch.prmemunits;
 *	    descr.v.que.v1.batch.ppstackunits = queue->q.v1.batch.ppstackunits;
 *	    descr.v.que.v1.batch.ppworkunits = queue->q.v1.batch.ppworkunits;
 *	    descr.v.que.v1.batch.ppcorecoeff = queue->q.v1.batch.ppcorecoeff;
 *	    descr.v.que.v1.batch.ppdatacoeff = queue->q.v1.batch.ppdatacoeff;
 *	    descr.v.que.v1.batch.pppfilecoeff = queue->q.v1.batch.pppfilecoeff;
 *	    descr.v.que.v1.batch.prpfilecoeff = queue->q.v1.batch.prpfilecoeff;
 *	    descr.v.que.v1.batch.ppqfilecoeff = queue->q.v1.batch.ppqfilecoeff;
 *	    descr.v.que.v1.batch.prqfilecoeff = queue->q.v1.batch.prqfilecoeff;
 *	    descr.v.que.v1.batch.pptfilecoeff = queue->q.v1.batch.pptfilecoeff;
 *	    descr.v.que.v1.batch.prtfilecoeff = queue->q.v1.batch.prtfilecoeff;
 *	    descr.v.que.v1.batch.ppmemcoeff = queue->q.v1.batch.ppmemcoeff;
 *	    descr.v.que.v1.batch.prmemcoeff = queue->q.v1.batch.prmemcoeff;
 *	    descr.v.que.v1.batch.ppstackcoeff = queue->q.v1.batch.ppstackcoeff;
 *	    descr.v.que.v1.batch.ppworkcoeff = queue->q.v1.batch.ppworkcoeff;
 *	    descr.v.que.v1.batch.ppcpusecs = queue->q.v1.batch.ppcpusecs;
 *	    descr.v.que.v1.batch.ppcpums = queue->q.v1.batch.ppcpums;
 *	    descr.v.que.v1.batch.prcpusecs = queue->q.v1.batch.prcpusecs;
 *	    descr.v.que.v1.batch.prcpums = queue->q.v1.batch.prcpums;
 *	    descr.v.que.v1.batch.ppnice = queue->q.v1.batch.ppnice;
 *	    descr.v.que.v1.batch.prdrives = queue->q.v1.batch.prdrives;
 *	    descr.v.que.v1.batch.prncpus = queue->q.v1.batch.prncpus;
 *	}
 *	else if (queue->q.type == QUE_DEVICE) {
 *	    strcpy (descr.v.que.namev.name, queue->q.namev.name);
 *	}
 *	else if (queue->q.type == QUE_NET) {
 *	    descr.v.que.namev.to_destination = queue->q.namev.to_destination;
 *	    descr.v.que.v1.network.runlimit = queue->q.v1.network.runlimit;
 *	    strcpy (descr.v.que.v1.network.server, queue->q.v1.network.server);
 *	    descr.v.que.v1.network.retry_at = queue->q.v1.network.retry_at;
 *	    descr.v.que.v1.network.retrytime = queue->q.v1.network.retrytime;
 *	    descr.v.que.v1.network.retrydelta = queue->q.v1.network.retrydelta;
 *	}
 *	else {
 *	    strcpy (descr.v.que.namev.name, queue->q.namev.name);
 *	    descr.v.que.v1.pipe.runlimit = queue->q.v1.pipe.runlimit;
 *	    strcpy (descr.v.que.v1.pipe.server, queue->q.v1.pipe.server);
 *	}
 */
	rewritedb (Queuefile, queue->queueno, &descr, DSC_QUEUE);
}


/*** udb_reqpgrp
 *
 *
 *	void udb_reqpgrp():
 *	Update req process-group in a queue-ordering file.
 */
void udb_reqpgrp (queue, req)
struct queue *queue;			/* Queue containing the req */
register struct request *req;		/* Request structure for req */
{
	static int all_ones = ~0;	/* Integer with all bits 1 */

	char path [MAX_PATHNAME+1];	/* Name of queue-ordering file */
	struct qentry dummy;		/* Used for address computation */
	register long offset;
	register int fd;
	register struct request *walk_request;

	/*
	 *  Compute name of existing queue-ordering file for the specified
	 *  queue.
	 */
	sprintf (path, "%s/q%08x%04x", Nqs_qorder, queue->queueno,
		 queue->q.orderversion);
	if ((fd = open (path, O_WRONLY)) == -1) {
		printf ("E$Unable to open ");
		printf ("queue order file in udb_reqpgrp().\n");
		printf ("I$Process group not updated for request.\n");
		fflush (stdout);
	}
	else {
		/*
		 *  The queue-ordering file for the queue was successfully
		 *  opened for writing.  Find out the offset of the relevant
		 *  qentry structure for the request.
		 */
		offset = 0;		/* Offset of desired qentry */
		walk_request = queue->runset;
		while (walk_request != req) {
			walk_request = walk_request->next;
			offset += sizeof (struct qentry);
		}
		/*
		 *  We now know the offset of the qentry in the queue-ordering
		 *  file for the given request.
		 *
		 *  Seek to the precise location in the queue-ordering file
		 *  of the integer defining the process-group for the given
		 *  request.
		 */
		lseek (fd, (long) (offset + ((char *) &dummy.v.process_family
					  -  (char *) &dummy)), 0);
		/*
		 *  Write the process-group of the request to the file.
		 */
		dummy.v.process_family = Runvars [req->reqindex].process_family;
		write (fd, (char *) &dummy.v.process_family,
		       sizeof (dummy.v.process_family));
		/*
		 *  Now that the process-group has been written, seek to
		 *  the location of the valid process-group byte, and
		 *  write a non-zero value to indicate that the process-
		 *  group field is valid.
		 */
		lseek (fd, (long) (offset + ((char *) &dummy.reqname_v
						      [MAX_REQNAME]
					  -  (char *) &dummy)), 0);
		/*
		 *  Set the valid-byte.
		 */
		write (fd, (char *) &all_ones, 1);
		close (fd);
	}
}


/*** udb_setfor
 *
 *
 *	void udb_setfor():
 *
 *	Set the NQS forms list to the single specified form in the
 *	NQS database.  Delete all other pre-existing forms from the
 *	NQS forms list.
 */
void udb_setfor (forms)
char *forms;				/* Forms name */
{
	struct gendescr newdescr;
	register struct gendescr *descr;

	seekdbb (Formsfile, 0L);	/* Seek to the beginning of the NQS */
					/* forms list file */
	/*
	 *  Delete all existing entries.
	 */
	descr = nextdb (Formsfile);
	while (descr != (struct gendescr *) 0) {
		freedb (Formsfile, telldb (Formsfile));
		descr = nextdb (Formsfile);
	}
	/*
	 *  Add the new single entry.
	 */
	strcpy (newdescr.v.form.forms, forms);
	allodb (Formsfile, &newdescr, DSC_FORM);
}


/*** udb_setlogfil
 *
 *
 *	void udb_setlogfil():
 *	Set the log file name stored in the NQS database.
 */
void udb_setlogfil ()
{
	struct gendescr gendescr;

	/*
	 *  Completely format the logfile record in the NQS
	 *  database.
	 */
	gendescr.v.par.paramtype = PRM_LOGFILE;
	strcpy (gendescr.v.par.v.logfile, Logfilename);
	if (Udblogfile == -1) {
		/*
		 *  No logfile record has ever been allocated.
		 */
		Udblogfile = allodb (Paramfile, &gendescr, DSC_PARAM);
	}
	else rewritedb (Paramfile, Udblogfile, &gendescr, DSC_PARAM);
	/*
	 *  Signal the NQS network daemon that the NQS general operating
	 *  parameters have been modified, and that it should update
	 *  its internal configuration variables as necessary.
	 */
	if (Netdaepid > 0) kill (Netdaepid, SIGINT);
}


/*** udb_setnqsman
 *
 *
 *	void udb_setnqsman():
 *
 *	Set the NQS managers account access list to the single
 *	account of "root", with full NQS manager privileges
 */
void udb_setnqsman ()
{
	register struct gendescr *descr;

	seekdbb (Mgrfile, 0L);		/* Seek to the beginning of the NQS */
					/* manager access list file */
	/*
	 *  Delete all existing entries with the exception of "root", which
	 *  is guaranteed to be present by the upm_addnqsman ("root", ~0)
	 *  call in nqs_ldconf.c, and which cannot be deleted by the
	 *  upm_delnqsman() function in ../src/nqs_upm.c.
	 */
	descr = nextdb (Mgrfile);
	while (descr != (struct gendescr *) 0) {
		if (descr->v.mgr.manager_uid != 0 ||
		    descr->v.mgr.manager_mid != Locmid) {
			freedb (Mgrfile, telldb (Mgrfile));
		}
		descr = nextdb (Mgrfile);
	}
}


/*** udb_useqno
 *
 *
 *	void udb_useqno():
 *
 *	Update the next available batch and device request sequence number
 *	stored in the NQS database.
 */
void udb_useqno (seqno)
long seqno;				/* Next available sequence number */
{
	/*
	 *  Please read the comments in ../src/nqs_ldconf.c so that
	 *  you will know when to do away with the awful mechanism
	 *  used to store the next available request sequence number....
	 */
	if (setmtime (Nqs_seqno, (unsigned long) seqno) == -1) {
		/*
		 *  Unable to update request seq#!
		 */
		printf ("F$Unable to update next available seq#.\n");
		nqs_abort();
	}
}


/*** udb_qapack
 *
 *
 *	int udb_qapack():
 *	Pack/write request into queue access file.
 *
 *	Returns:
 *		Next available index in buffer.
 */
static int udb_qapack (fd, buffer, index, acclistp)
register int fd;					/* File descr */
register unsigned long buffer [QAFILE_CACHESIZ];	/* Output buffer */
register int index;					/* Index into buffer */
register struct acclist *acclistp;			/* Structure to write */
{
	int i;					/* Array index */
	
	if (acclistp != (struct acclist *) 0) {
		for (i = 0; i < ACCLIST_SIZE; i++) {
			if (acclistp->entries [i] != 0) {
				buffer [index] = acclistp->entries [i];
				index++;
			}
			/*
			 * QAFILE_CACHESIZ is not constrained to be a
			 * multiple of ACCLIST_SIZE, thus the complexity.
			 */
			if (acclistp->entries [i] == 0 ||
				index == QAFILE_CACHESIZ) {
				index *= sizeof (unsigned long);
				if (write (fd, (char *) buffer, index)
						!= index) {
					printf ("E$Write() error ");
					printf ("in udb_qapack().\n");
					fflush (stdout);
				}
				index = 0;
			}
		}
		return (index);
	}
	index *= sizeof (unsigned long);
	if (write (fd, (char *) buffer, index)
		!= index) {
		printf ("E$Write() error in udb_qapack().\n");
		fflush (stdout);
	}
	index = 0;
	return (index);
}


/*** udb_qopack
 *
 *
 *	int udb_qopack():
 *	Pack/write request into queue ordering file.
 *
 *	Returns:
 *		Next available index in buffer.
 */
static int udb_qopack (fd, buffer, index, request, queuetype, req_is_running)
register int fd;				/* File descr */
register struct qentry buffer [QOFILE_CACHESIZ];/* Output buffer */
register int index;				/* Output buffer index */
register struct request *request;		/* Request to write */
int queuetype;					/* Type of containing queue */
short req_is_running;				/* Boolean true if req is */
						/* presently running */
{
	if (request != (struct request *) 0) {
		buffer [index].uid = request->v1.req.uid;
		buffer [index].priority = request->v1.req.priority;
		buffer [index].orig_mid = request->v1.req.orig_mid;
		buffer [index].orig_seqno = request->v1.req.orig_seqno;
		buffer [index].size = 0;
		if (queuetype == QUE_DEVICE) {
			buffer [index].size = request->v1.req.v2.dev.size;
		}
		strncpy (buffer [index].reqname_v, request->v1.req.reqname,
			 MAX_REQNAME+1);
		if (req_is_running) {
			if (Runvars [request->reqindex].process_family != 0) {
				/*
				 *  The process-family for this request is
				 *  known.  Mark the valid byte and write
				 *  the process-family.
				 */
				buffer [index].reqname_v [MAX_REQNAME] = ~0;
				buffer [index].v.process_family
				= Runvars [request->reqindex].process_family;
			}
			else {
				/*
				 *  The process-family for this request is
				 *  not yet known.  Clear the valid byte.
				 */
				buffer [index].reqname_v [MAX_REQNAME] = '\0';
			}
		}
		else buffer [index].v.start_time = request->start_time;
		index++;
	}
	if (index == QOFILE_CACHESIZ ||
	   (request == (struct request *) 0 && index)) {
		index *= sizeof (struct qentry);
		if (write (fd, (char *) buffer, index) != index) {
			printf ("E$Write() error in udb_qopack().\n");
			fflush (stdout);
		}
		index = 0;
	}
	return (index);
}
