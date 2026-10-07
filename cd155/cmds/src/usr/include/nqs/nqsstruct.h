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
 
/*++ nqsstruct.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqsstruct.h,v $
 *
 * DESCRIPTION:
 *
 *	NQS daemon data structures definitions file.
 *	This file is included by the ../h/nqs.h file.
 *
 *	WARNINGS:
 *	  This file MUST NEVER be included directly by ANY NQS source
 *	  file.
 *
 *	  Seven system types are defined:  BSD42, BSD43, SGI, SYS52,
 *	  ULTRIX, UNICOS, and UTS.  Accordingly, the flag: SYSTEM_TYPE
 *	  in the Makefile must be appropriately defined.
 *
 *	  The #define type:  ALIGNTYPE  should be configured in
 *	  ../h/nqs.h as appropriate.
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 29, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.6 $ $Date: 1995/02/24 23:39:36 $ $State: Exp $)
 * $Log: nqsstruct.h,v $
 * Revision 1.6  1995/02/24  23:39:36  kremenek
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
 * Revision 1.5  1994/11/19  02:44:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/11/02  00:06:28  mwan
 * R1.2 mods.
 *
 * Revision 1.3  1993/07/13  18:18:26  mwan
 * T11 - fixed PTS 5012
 *
 * Revision 1.2  1992/10/09  22:18:12  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:53  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:44:49  hender
 * Sterling version 4/22/87
 *
 *	Change donated by Intergraph:	Bill Mar	12/05/89
 *	- Added qcomplex to device portion of struct queue.
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)nqsstruct.h	1.4 (nqsstruct.h OSF/1 NQS2.0 GJK) 7/7/92";
#define SCCS
#endif

/*
 *	NQS raw request structure for a device specific request.
 *
 *	WARNING:  The format of this structure cannot be changed
 *		  if there are any requests queued on the system.
 */
struct rawdevreq {
	char forms [MAX_FORMNAME+1];	/* Forms to use for request */
					/* (Null if default forms ok) */
	int copies;			/* Number of copies to produce */
	long reserved1;			/* RESERVED for future use */
	long reserved2;			/* RESERVED for future use */
	unsigned long size;		/* Output size in bytes */
	char devprefname [MAX_DEVPREF][MAX_DEVNAME+1];
	mid_t devprefmid [MAX_DEVPREF];	/* Device preference set */
					/* RESERVED for future use */
					/* *** WARNING *** */
					/* ALL of the fields from: magic1 */
					/* in struct rawreq through all of */
					/* the fields in a struct rawdevreq */
					/* MUST make a structure that is */
					/* <= ATOMICBLKSIZ bytes in size */
					/* (for the future implementation */
					/* of Qmod).  This requirement is */
					/* checked in nqs_ldconf.c. */
};

/*
 *	NQS raw request structure for a batch request (non-device).
 *	WARNING:  The format of this structure cannot be changed
 *		  if there are any requests queued on the system.
 */
struct rawbatreq {
	short umask;			/* Umask */
	char shell_name[MAX_SHELLNAME+1];
					/* Name of shell to be used */
					/* when executing shell script */
	long explicit;			/* LIM_ quota/nice explicit or */
					/* default bit flags */
	long infinite;			/* LIM_ quota/nice infinite or */
					/* finite bit flags */
	struct quotalimit ppcoresize;	/* Per-process core file size limits */
	struct quotalimit ppdatasize;	/* Per-process data size limits */
	struct quotalimit pppfilesize;	/* Per-process permanent file size */
					/* limits */
	struct quotalimit prpfilespace;	/* Per-request permanent file space */
					/* limits */
	struct quotalimit ppqfilesize;	/* Per-process quick file size */
					/* limits */
	struct quotalimit prqfilespace;	/* Per-request quick file space */
					/* limits */
	struct quotalimit pptfilesize;	/* Per-process temporary file size */
					/* limits */
	struct quotalimit prtfilespace;	/* Per-request temporary file space */
					/* limits */
	struct quotalimit ppmemsize;	/* Per-process memory size limits */
	struct quotalimit prmemsize;	/* Per-request memory size limits */
	struct quotalimit ppstacksize;	/* Per-process stack size limits */
	struct quotalimit ppworkset;	/* Per-process working set limits */
	struct cpulimit ppcputime;	/* Per-process CPU time limits */
	struct cpulimit prcputime;	/* Per-request CPU time limits */
	short ppnice;			/* Per-process nice value */
	short prdrives;			/* Per-request tape drives limit */
	short prncpus;			/* Per-request # of cpus limit */
	char predecessors [MAX_PREDECESSOR][MAX_REQNAME+1];
	short stderr_acc;		/* Stderr access mode bits [OMD_] */
	short stdlog_acc;		/* RESERVED for future use */
	short stdout_acc;		/* Stdout access mode bits [OMD_] */
		/* *** WARNING *** */
		/* ALL of the fields from: magic1 in struct rawreq  */
		/* through the field: stdout_acc in this structure  */
		/* MUST make a structure <= ATOMICBLKSIZ bytes in size */
		/* (for the future implementation of Qmod).  This */
		/* requirement is checked in nqs_ldconf.c. */
	mid_t stderr_mid;		/* Stderr machine-id destination */
					/* (ignored if stderr_acc has */
					/* OMD_M_KEEP bit set) */
			/* *** WARNING *** */
			/* Never move this field relative to the */
			/* stdout_acc field unless ../src/nqs_ldconf.c */ 
			/* is updated appropriately 		*/
	mid_t stdlog_mid;		/* RESERVED for future use */
	mid_t stdout_mid;		/* Stdout machine-id destination */
					/* (ignored if stdout_acc has */
					/* OMD_M_KEEP bit set) */
	char stderr_name[MAX_REQPATH+1];/* Stderr path name at machine*/
	char stdlog_name[MAX_REQPATH+1];/* Stdlog path name at machine*/
	char stdout_name[MAX_REQPATH+1];/* Stdout path name at machine*/
	short instacount;		/* Number of files to be staged-in */
	short oustacount;		/* Number of files to be staged-out */
	long instahiermask;		/* Stage-in recursive hierarchy */
	long oustahiermask;		/* Stage-out recursive hierarchy */
	mid_t instamid[MAX_INSTAPERREQ];/* Machine-ids of stage-in files */
	mid_t oustamid[MAX_OUSTAPERREQ];/* Machine-ids of stage-out files */
};					/* List of predecessor requests */

/*
 *	NQS general raw request structure format.
 *	WARNING:  The format of this structure cannot be changed
 *		  if there are any requests queued on the system.
 */
struct rawreq {				/* Raw request structure */
	long magic1;			/* Magic# [REQ_MAGIC1] */
	short trans_id;			/* Transaction-id */
#ifdef SDSC
	short period_inx;			/* period index */
	long major_nodes;		/* number of nodes in the major part */
        long cube_account;  		/* account id */
	long node_group;		/* scheduling group */
#else
	short reserved1;		/* RESERVED for future use; must */
	long reserved2;			/* presently be set to zero (0); */
	mid_t reserved3;		/* All of these reserved fields */
	mid_t reserved4;		/* will someday be used to store */
#endif
	long reserved5;			/* the transaction state info now */
	time_t reserved6;		/* stored in a transaction descr. */
					/* See ../lib/transact.c */
	char trans_quename [MAX_QUEUENAME+1];
		/* If the request is queued for transport to another */
		/* machine, or is presently being transported to */
		/* another machine, then this field contains the */
		/* name of the destination queue at the receiving */
		/* machine; otherwise this field is */
		/* completely filled with '\0' chars */
	time_t create_time;		/* Time at which the request was */
					/* first created */
	time_t enter_time;		/* Time at which request entered the */
					/* queue in which it now resides */
	char quename [MAX_QUEUENAME+1]; /* Name of queue request is in. */
					/* If this request is queued for */
					/* transport to another machine, or */
					/* is presently being transported to */
					/* another machine, then this field */
					/* contains the name of the local */
					/* pipe queue that routed this */
					/* request.  If this request was */
					/* directly queued for transport */
					/* to a remote machine/queue */
					/* without ever passing though a */
					/* local pipe queue, then this */
					/* field is filled entirely with */
					/* null characters. */
	short type;			/* Request type [RTYPE_BATCH, */
					/* RTYPE_DEVICE] */
	uid_t orig_uid;			/* Original user-id of submitter */
					/* on originating machine */
	mid_t orig_mid;			/* Originating machine-id */
	long tcm;			/* Submitter looks here to see */
					/* if qsub went ok. Holds TCML_??? */
	long orig_seqno;		/* Request sequence number */
	short rpriority;		/* Relative intra-queue priority */
	short flags;			/* Request flags [RQF_OPERHOLD, */
					/* RQF_USERHOLD, RQF_EXTERNAL, */
					/* RQF_BEGINMAIL, RQF_ENDMAIL, */
					/* RQF_TRANSMAIL, RQF_RESTARTABLE] */
	time_t start_time;		/* Start time */
	short ndatafiles;		/* Number of data files for request */
	char reqname [MAX_REQNAME+1];	/* Request-name */
	char username [MAX_ACCOUNTNAME+1];
					/* Submitting user account name */
	char mail_name [MAX_ACCOUNTNAME+1];
					/* Account name for mail */
	mid_t mail_mid;			/* Machine destination for ALL mail */
	union {				/* concerning the request. */
		struct rawbatreq bat;	/* Batch request data */
		struct rawdevreq dev;	/* Device request data */
	} v;
};


/*
 *	NQS transaction structure.
 */
struct transact {
	short tra_type;			/* Transaction type: TRA_??? */
	short state;			/* Request transaction state */
	time_t update;			/* Time of last state update */
	long orig_seqno;		/* Request sequence# for this */
					/* transaction */
	mid_t orig_mid;			/* Request machine-id */
	union {
		mid_t peer_mid;		/* Peer machine-id when request */
					/* is in a transport state */
		long events31;		/* File staging events [0..31] */
					/* (31=stderr, 30=stdout) */
					/* When all bits of events31 are */
					/* zero, then all file staging */
					/* transactions are complete for */
					/* this particular phase of the */
					/* request execution */
					/* See ../lib/transact.c for */
					/* much more information */
	} v;
};


/*
 *	NQS memory-resident request structure.
 *
 *	Requests are described by request structures which appear in linked
 *	lists for each queue.
 *
 *	Note that request structures ending with the variant: subreq are
 *	are present only in network queues, while full request structures
 *	are only present in non-network queues.
 */
struct request {
#ifdef SDSC
	long status;			/* Request status flags: RQF_??? */
#else
	short status;			/* Request status flags: RQF_??? */
#endif
	short reqindex;			/* Index into Runvars[] of request */
					/* info when this (sub)request is */
					/* actually running */
	time_t start_time;		/* Start time in secs from 1/1/70; */
					/* This time can be adjusted by any */
					/* of the request schedulers: */
					/* nqs_bsched(), nqs_dsched(), or */
					/* nqs_psched(). */
	struct request *next;		/* Pointer to the next request */
					/* structure in this queue */
	struct queue *queue;		/* Pointer to queue in which this */
					/* structure resides */
	union {
	    struct subrequestvariant {
		short event;		/* Subtransaction event# [0..31] */
		struct request *sibling;/* Pointer to the next sibling */
					/* subrequest for the parent request */
		struct request *parent;	/* Pointer to the parent request */
	    } subreq;			/* Subrequest */
					/* WARNING: When a subrequest request*/
					/* structure is allocated, only */
					/* enough memory is allocated to */
					/* hold the request structure fields */
					/* up to this point.  NO FIELD BELOW */
					/* THIS POINT MUST EVER BE USED IF */
					/* THIS STRUCTURE DENOTES A SUB- */
					/* REQUEST!!! */
	    struct {
		char reqname [MAX_REQNAME+1];
					/* Request name */
		short priority;		/* Assigned priority has computed */
					/* by nqs_xsched (x=[b,d,n,p]) */
		uid_t uid;		/* Submitter's mapped uid */
		mid_t orig_mid;		/* Mid of submitter's machine */
		long orig_seqno;	/* Sequence number for this request */
		time_t state_time;	/* Time at which request entered the */
					/* queue in which it now resides, or */
					/* last changed state (depending on */
					/* the state transitions) */
		short trans_id;		/* Transaction-id [0..MAX_TRANSACTS] */
					/* (0 = no transaction state) */
		short ndatafiles;	/* Number of data files for req */
		union {
		    struct {		/* For device requests only */
			char forms [MAX_FORMNAME+1];
					/* Forms to use */
					/* Forms is null if default forms ok */
			int copies;	/* Number of copies to make */
			unsigned long size;
					/* Size of device output in bytes */
		    } dev;		/* Device request specific */
#ifdef SDSC
		    struct {		/* Queued "batch" request only */
#else
		    union {		/* Queued "batch" request only */
#endif
					/* The fields: rfile, rcpu, rmem, and*/
					/* and basepri can be used in any way*/
					/* seen fit by the NQS batch request */
					/* scheduler */
			long rfile;	/* File space requirements/limits */
			long rcpu;	/* CPU time requirements/limits */
			long rmem;	/* Memory requirements/limits */
#ifdef SDSC
			int ncpus;	/* number of cpus */
			short period_inx; /* period index */
			long major_nodes;  /* no. of nodes in the major part */
			long node_group;   /* scheduling node_group */
#endif
			int basepri;	/* This field could be used as */
					/* follows:  Each time a request is */
					/* "passed-over by the batch request */
					/* scheduler, the basepriority of */
					/* the req (set to 0 by nqs_xsched() */
					/* (x=[b,d,n,p]) on creation), can be*/
					/* increased at the scheduler's */
					/* discretion.  In this way, we can */
					/* prevent requests that require many*/
					/* resources to not be blocked */
					/* forever from execution by smaller */
			long events31;	/* File staging events [0..31] */
					/* (31=stderr, 30=stdout) */
					/* When all bits of events31 are */
					/* zero, then all file staging-in */
					/* or staging-out transactions are */
					/* complete as indicated by the */
					/* respective presence or absence */
					/* or RQF_INSTAFILES in the request */
					/* status flags */
		    } bat;		/* Batch request specific */
		} v2;			/* Request scheduling and event info */
	    } req;			/* Request */
	} v1;
};


/*
 *	Running NQS request description array element.
 */
struct running {
	short allocated;		/* Boolean records whether or not */
					/* this entry in the Runvars[] */
					/* array is allocated */
	short queued_signal;		/* If RQF_SIGQUEUED is set in the */
					/* status field for the request, */
					/* then this field contains the */
					/* queued signal */
	int process_family;		/* This field contains the process */
					/* group of the server for the */
					/* request, and hence all processes */
					/* in the request, unless one of the */
					/* request processes successfully */
					/* performs a setpgrp() call (setpgid() for OSF/1).  If */
					/* this field is zero, and the */
					/* shepherd_pid is non-zero, then */
					/* the request has been spawned but */
					/* no server has been created yet */
					/* for the request. */
	int shepherd_pid;		/* This field contains the PID of */
					/* the request shepherd process (>0) */
#ifdef SDSC
	int part_id;			/* partition id */
#endif
};


/*
 *	NQS destination "pipeto" queue descriptor.
 */
struct pipeto {
	long pipetono;			/* NQS pipe queue destination number */
					/* = offset in NQS pipe descr file */
	short refcount;			/* Reference count */
	short status;			/* Current pipe destination status */
					/* bits: DEST_ */
	time_t retry_at;		/* If in retry mode, then this value */
					/* determines when the destination */
					/* should be reenabled */
	time_t retrytime;		/* If in retry mode, then this value */
					/* records the time at which the */
					/* destination first failed */
	long retrydelta;		/* RESERVED for future use */
	char rqueue [MAX_QUEUENAME+1];	/* Name of destination queue */
	mid_t rhost_mid;		/* Machine-id of remote destination */
	struct pipeto *next;		/* Ptr to next dest "pipeto" descr */
};


/*
 *	NQS device-queue to device and device to device-queue mapping
 *	structure.
 */
struct qdevmap {
	long mapno;			/* Queue/device mapping number */
					/* = offset in queue/device/dest map */
					/* descriptor file */
	struct device *device;		/* Ptr to device used by queue */
	struct queue *queue;		/* Ptr to queue served by device */
	struct qdevmap *nextqd;		/* Ptr to next queue-to-device */
					/* mapping for this queue */
	struct qdevmap *nextdq;		/* Ptr to next device-to-queue */
};					/* mapping for this device */


/*
 *	NQS pipe-queue to destination mapping structure.
 */
struct qdestmap {
	long mapno;			/* Queue/dest mapping number */
					/* = offset in queue/device/dest map */
					/* descriptor file */
	struct pipeto *pipeto;		/* Ptr to destination structure */
	struct qdestmap *next;		/* Ptr to next queue/dest mapping */
};					/* for this pipe queue */


/*
 *	NQS queue complex descriptor structure.
 */
struct qcomplxdescr {
	short runlimit;			/* Queue complex run limit */
	char name [MAX_QCOMPLXNAME+1];	/* Name of queue complex */
	char queues [MAX_QSPERCOMPLX][MAX_QUEUENAME+1];
					/* Names of participant queues */
};


/*
 *	NQS queue complex structure.
 */
struct qcomplex {
	long qcomplexno;		/* Queue complex number */
					/* = offset in queue complex */
					/* descriptor file */
	short runlimit;			/* Queue complex runlimit */
	short runcount;			/* Number of queues running in */
					/* this complex */
	char name [MAX_QCOMPLXNAME+1];	/* Name of queue complex */
	struct queue *queue [MAX_QSPERCOMPLX];
					/* List of participants */
	struct qcomplex *next;		/* Next queue complex */
};


/*
 *	NQS queue name descriptor structure.
 */
union quenamedescr {
	char name [MAX_QUEUENAME+1];	/* Name of non-network queue */
	mid_t to_destination;		/* Machine-id of network queue */
};


/*
 *	NQS queue descriptor database structure.
 */
struct quedescr {
	short orderversion;		/* Current queue ordering version */
	short accessversion;		/* Current queue access verison */
	short status;			/* Queue status bits */
	short type;			/* Queue type=[QUE_PIPE, QUE_DEVICE, */
					/*	       QUE_BATCH, QUE_NET] */
	short priority;			/* Priority for this queue */
	union quenamedescr namev;	/* Name of non-network queue; */
					/* Machine-id of network queue */
#if	UNICOS | SGI | SYS52 | UTS | OSF
	time_t ru_stime;		/* Number of system space and user */
	time_t ru_utime;		/* space clock ticks consumed by */
					/* processes of all reqs that have */
					/* run in this queue.   NOTE THAT */
					/* this value will NOT record the */
					/* CPU time consumed by ORPHANED */
					/* child processes for any request. */
#else
#if	BSD42 | BSD43 | ULTRIX
	long ru_stime_usec;		/* Number of microseconds and seconds */
	unsigned long ru_stime;		/* of system space and user space */
	long ru_utime_usec;		/* clock ticks consumed by the */
	unsigned long ru_utime;		/* processes of all reqs that have */
					/* run in this queue.   NOTE THAT */
					/* this value will NOT record the */
					/* CPU time consumed by ORPHANED */
					/* child processes for any request. */
#else
BAD SYSTEM TYPE
#endif
#endif
	short departcount;		/* Number of reqs in the queue that */
					/* are presently departing if this */
					/* queue is a pipe queue;  number */
					/* of reqs in the queue that are */
					/* presently staging-out files */
					/* if this queue is a batch queue */
	short runcount;			/* Number of reqs in the queue that */
					/* are presently running */
	short stagecount;		/* Number of requests in the queue */
					/* that are presently staging-in */
					/* files for later execution */
					/* RESERVED FOR FUTURE USE SO DATA- */
					/* BASE MODIFICATIONS CAN BE AVOIDED*/
					/* WHEN STAGING-IN IS IMPLEMENTED IN*/
					/* A FUTURE RELEASE OF NQS */
	short queuedcount;		/* Number of reqs in the queue that */
					/* are not running, but could be */
					/* chosen for execution by the */
					/* scheduler */
	short waitcount;		/* Number of reqs in the queue that */
					/* will be eligible for execution */
					/* after a certain date/time. */
	short holdcount;		/* Number of reqs in the queue that */
					/* are on hold */
	short arrivecount;		/* Number of reqs in the queue that */
					/* are arriving from network queues */
	union {
		struct {
			unsigned long cpuset [BSET_CPUVECSIZE];
					/* Bitmask of CPUs allocated to this */
					/* queue */
			unsigned long cpuuse [BSET_CPUVECSIZE];
					/* Bitmask of CPUs presently in use */
					/* by this queue */
#ifdef SDSC
			short node_group; /* scheduling node group */
#else
			short ncpuset;	/* CPUs in set */
#endif
			short ncpuuse;	/* CPUs in use */
			unsigned long rs1set [BSET_RS1VECSIZE];
			unsigned long rs1use [BSET_RS1VECSIZE];
#ifdef SDSC
                        short sPerQueMaxQueuedULimit;
#else
                        short nrs1set;  /* RESERVED for future use */
#endif
			short nrs1use;	/* RESERVED for future use */
			unsigned long rs2set [BSET_RS1VECSIZE];
			unsigned long rs2use [BSET_RS1VECSIZE];
			short nrs2set;	/* RESERVED for future use */
			short nrs2use;	/* RESERVED for future use */
			short runlimit;	/* Max #of requests allowed to run */
					/* in this queue at any given time */
			long explicit;	/* LIM_ quota/nice explicit or */
					/* default bit flags */
			long infinite;	/* LIM_ quota/nice infinite or */
					/* finite bit flags */
			/*
			 *  We separate units from their coefficients
			 *  since many architectures align integers
			 *  on addresses that are a multiple of the
			 *  integer type size.  Thus, it is often a
			 *  better use of memory if integers of the
			 *  same size appear one after another, instead
			 *  of using the pattern the pattern:
			 *
			 *	short limit_type_units;
			 *	unsigned long limit_type_coefficient;
			 *		:
			 *		:
			 *	       etc.
			 */
			short ppcoreunits;
					/* Per-process core file size */
					/* limit units */
			short ppdataunits;
					/* Per-process data segment size */
					/* limit units */
			short pppfileunits;
					/* Per-process permanent file size */
					/* limit units */
			short prpfileunits;
					/* Per-request permanent file size */
					/* limit units */
			short ppqfileunits;
					/* Per-process quick file size */
					/* limit units */
			short prqfileunits;
					/* Per-request quick file size */
					/* limit units */
			short pptfileunits;
					/* Per-process temporary file size */
					/* limit units */
			short prtfileunits;
					/* Per-request temporary file size */
					/* limit units */
			short ppmemunits;
					/* Per-process memory size limit */
					/* units */
			short prmemunits;
					/* Per-request memory size limit */
					/* units */
			short ppstackunits;
					/* Per-process stack segment size */
					/* limit units */
			short ppworkunits;
					/* Per-process working set size */
					/* limit units */
			unsigned long ppcorecoeff;
					/* Per-process core file size limit */
					/* coefficient */
			unsigned long ppdatacoeff;
					/* Per-process data segment size */
					/* limit coefficient */
			unsigned long pppfilecoeff;
					/* Per-process permanent file size */
					/* limit coefficient */
			unsigned long prpfilecoeff;
					/* Per-request permanent file space */
					/* limit coefficient */
			unsigned long ppqfilecoeff;
					/* Per-process quick file size */
					/* limit coefficient */
			unsigned long prqfilecoeff;
					/* Per-request quick file space */
					/* limit coefficient */
			unsigned long pptfilecoeff;
					/* Per-process temporary file size */
					/* limit coefficient */
			unsigned long prtfilecoeff;
					/* Per-request temporary file space */
					/* limit coefficient */
			unsigned long ppmemcoeff;
					/* Per-process memory size limit */
					/* coefficient */
			unsigned long prmemcoeff;
					/* Per-request memory size limit */
					/* coefficient */
			unsigned long ppstackcoeff;
					/* Per-process stack segment size */
					/* limit coefficient */
			unsigned long ppworkcoeff;
					/* Per-process working set size */
					/* limit coefficient */
			unsigned long ppcpusecs;
					/* Per-process CPU limit seconds */
			short ppcpums;	/* Milliseconds of above */
			unsigned long prcpusecs;
					/* Per-request CPU limit seconds */
			short prcpums;	/* Milliseconds of above */
			short ppnice;	/* Per-process nice value */
			short prdrives;	/* Per-request tape drives limit */
			short prncpus;	/* Per-request # of cpus limit */
		} batch;		/* Batch queue specific */
		struct {
			short runlimit;	/* Max #of requests allowed to run */
					/* in this queue at any given time */
			char server [MAX_SERVERNAME+1];
					/* Server and arguments */
			time_t retry_at;/* If this network queue is in */
					/* retry mode, then this field */
					/* contains the time at which the */
					/* next retry should be made (see */
					/* QUE_NETRETRY) */
			time_t retrytime;
					/* If this network queue is in */
					/* retry mode, then this field */
					/* contains the time of the first */
					/* unsuccessful transaction */
			long retrydelta;/* RESERVED for future use */
		} network;		/* Network queue specific */
		struct {
			short runlimit;	/* Max #of requests allowed to run */
					/* in this queue at any given time */
			char server [MAX_SERVERNAME+1];
					/* Server and arguments */
		} pipe;			/* Pipe queue specific */
	} v1;
};


/*
 *	NQS queue header structure.
 */
struct queue {
	long queueno;			/* NQS queue number */
					/* = offset in NQS queue descr file */
	struct request *departset;	/* Requests that are departing if */
					/* this queue is a pipe queue; order-*/
					/* ed by queue depart time; requests */
					/* that are presently staging */
					/* output if this queue is a batch */
					/* queue; ordered by execution */
					/* (run) phase completion */
	struct request *runset;		/* Requests (or subrequests) that */
					/* are running; ordered by start of */
					/* execution */
	struct request *stageset;	/* Requests that are staging-in */
					/* files;  ordered by time of */
					/* stage-in startup (applicable */
					/* only to batch queues and */
					/* RESERVED FOR FUTURE USE) */
	struct request *queuedset;	/* Requests (or subrequests) */
					/* eligible for execution; ordered */
					/* by scheduled priority and */
					/* enter_time */
	struct request *waitset;	/* Requests (or subrequests) not */
					/* eligible for execution until a */
					/* certain date/time;  ordered by */
					/* -a time and scheduled priority */
	struct request *holdset;	/* Requests (or subrequests) on */
					/* hold; ordered by scheduled */
					/* priority and enter_time*/
	struct request *arriveset;	/* For network queues, this set */
					/* contains the set of subrequests */
					/* arriving from batch and pipe */
					/* queues; ordered by their temporal */
					/* subrequest creation time. */
					/* For non-network queues, this */
					/* contains the set of requests */
					/* arriving from remote network */
					/* queues, ordered by temporal */
					/* queueing time on the local host */
	time_t sigkill;			/* If non-zero, then this field */
					/* specifies that a SIGKILL signal */
					/* should be sent to all processes */
					/* for each req running in the queue */
					/* when this time (as specified in */
					/* seconds from 0:00:00 Jan 1, 1970 */
					/* GMT, occurs */
	union {
		struct {
			struct acclist *acclistp;
					/* Ptr to access list */
			struct queue *nextpriority;
					/* Next batch queue in the priority */
					/* ordered batch queue set -- sorted*/
					/* in order of decreasing priority */
			struct qcomplex *qcomplex [MAX_COMPLXSPERQ];
					/* Queue complex membership */
					/* (Ptrs can be NIL */
		} batch;		/* Batch queue specific */
		struct {
			struct acclist *acclistp;
					/* Ptr to access list */
			struct qdevmap *qdmaps;
			struct qcomplex *qcomplex [MAX_COMPLXSPERQ];
					/* Queue complex membership */
					/* (Ptrs can be NIL */
					/* Ptr to queue-to-device maps */
		} device;		/* Device queue specific */
		struct {
			struct queue *nextpriority;
					/* Next network queue in the priority*/
					/* ordered network queue set --sorted*/
					/* in order of decreasing priority */
		} network;
		struct {
			struct acclist *acclistp;
					/* Ptr to access list */
			struct qdestmap *destset;
					/* Linked list of mappings for the */
					/* possible destination queues */
			struct queue *nextpriority;
					/* Next pipe queue in the priority */
					/* ordered pipe queue set -- sorted*/
					/* in order of decreasing priority */
		} pipe;			/* Pipe queue specific */
	} v1;
	struct quedescr q;		/* Additional queue description */
	struct queue *next;		/* Next queue in queue set */
};


/*
 *	Queue access list structure.
 */
struct acclist {
	struct acclist *next;			/* For a singly linked list */
	unsigned long entries [ACCLIST_SIZE];	/* Gids and uids go here */
};


/*
 *	NQS device structure.
 */
struct device {
	long deviceno;			/* NQS device number */
					/* = offset in device descr file */
	short status;			/* Device status bits */
	char statmsg [MAX_STATMSG+1];	/* Device status message */
	char name [MAX_DEVNAME+1];	/* Shorthand device name */
	char *fullname;			/* Full pathname for device which */
					/* is dynamically allocated */
	char forms [MAX_FORMNAME+1];	/* Current forms in device */
	char server [MAX_SERVERNAME+1];	/* Command line specifying server */
					/* and server arguments */
	struct request *curreq;		/* Current request being handled */
					/* (Will be NULL if no current req) */
	struct queue *curque;		/* Ptr to queue struct that the */
					/* current request is in (NULL if */
					/* no current request) */
	struct qdevmap *dqmaps;		/* Ptr to device-to-queue maps */
	struct device *next;		/* Next device in device set */
};


/*
 *	NQS queue file format structure.
 */
struct qentry {
	uid_t uid;			/* Mapped user-id of owner */
	short priority;			/* Assigned request priority as */
					/* determined by nqs_xsched; */
					/* (x=[b,d,n,p]) */
	mid_t orig_mid;			/* Machine-id of originating machine */
	long orig_seqno;		/* Req sequence number */
	unsigned long size;		/* Size of device output in bytes */
					/* (Set to 0 for non-device queues) */
	union {
		long process_family;	/* If the req is running, then this */
					/* field contains the "process- */
					/* group of the req server", as */
					/* reported by the shepherd process. */
					/* The validity of this field is */
					/* determined by the value of the */
					/* last byte in the reqname_v */
					/* array (see below). */
		time_t start_time;	/* Time after which the req should */
					/* be started.  This field is only */
					/* valid if the req is waiting or */
	} v;				/* holding. */
	char reqname_v [MAX_REQNAME+1];	/* NOT necessarily null terminated */
};					/* reqname AND validity byte. */
					/* Reqname_v [MAX_REQNAME] does NOT */
					/* always store a null byte!!!!!!! */
					/* If the request described by this */
					/* qentry is running, and */
					/* reqname_v [MAX_REQNAME] == '\0', */
					/* then the process_family field in */
					/* the union: v, is NOT defined */
					/* (the req has been spawned, but no*/
					/* server has been created yet for */
					/* the req).  If the req described */
					/* by this qentry is running, and */
					/* reqname_v [MAX_REQNAME] != '\0', */
					/* then the process_family field */
					/* IS defined. */
					/* Both the name of the req, and */
					/* the validity byte are crammed */
					/* into the same character array, */
					/* so that machines like the Cray */
					/* will not use another 64-bit */
					/* integer just to store the */
					/* validity byte. */


/*
 *	NQS device descriptor database structure.
 */
struct devdescr {
	short status;			/* Device status bits */
	char statmsg [MAX_STATMSG+1];	/* Device status message */
	long orig_seqno;		/* Request sequence# */
	uid_t uid;			/* Mapped request owner user-id */
	mid_t orig_mid;			/* Mid of machine that originated */
					/* the request */
	char reqname [MAX_REQNAME+1];	/* Null terminated name of current */
					/* request */
	char qname [MAX_QUEUENAME+1];	/* Null terminated name of queue */
					/* containing the current request */
	char forms [MAX_FORMNAME+1];	/* Null terminated current name */
					/* of forms in device */
	char dname [MAX_DEVNAME+1];	/* Null terminated device name */
	char server [MAX_SERVERNAME+1];	/* Null terminated name of server */
					/* and related arguments.  */
	char fullname [MAX_PATHNAME+1];	/* Null terminated full device name. */
};					/* This last field is actually */
					/* implemented as a varying length */
					/* character string in the device */
					/* descriptor file. */
					/* See ../lib/sizedb.c */


/*
 *	NQS device-queue to device and pipe-queue to destination mapping
 *	structure (used in the Nqs_qmaps file).
 */
struct qmapdescr {
	short qtodevmap;		/* Boolean TRUE if descr describes */
					/* a device queue to device mapping */
	union {
		struct {		/* Device queue to device mapping */
			char qname [MAX_QUEUENAME+1];
			char dname [MAX_DEVNAME+1];
					/* Null terminated queue and */
		} qdevmap;		/* device names */
		struct {		/* Pipe queue to destination mapping */
			char lqueue [MAX_QUEUENAME+1];
			char rqueue [MAX_QUEUENAME+1];
					/* Null terminated local pipe queue */
					/* and destination queue names */
			mid_t rhost_mid;/* Machine-id of destination */
		} qdestmap;
	} v;
};


/*
 *	NQS pipe-queue queue@machine-name structure (used in the Nqs_pipeto
 *	file).
 */
struct pipetodescr {
	short status;			/* Status bits for pipe destination */
	time_t retry_at;		/* Retry time */
	time_t retrytime;		/* Time of first failure */
	long retrydelta;		/* RESERVED for future use */
	char rqueue[MAX_QUEUENAME+1];	/* Null terminated remote queue name*/
	mid_t rhost_mid;		/* Machine-id of remote host */
};


/*
 *	NQS general parameter structure (used in the Nqs_params file).
 */
struct paramdescr {
	short paramtype;		/* Parameter-type:  See PRM_ */
	union {
		struct {
			uid_t mail_uid;		/* For an explanation of */
			short maxgblacclimit;	/* these variables, the  */
			short maxgblbatlimit;	/* reader is referred to */
			short maxgblnetlimit;	/* ../h/nqsvars.h and */
			short maxgblpiplimit;	/* ../lib/ldparam.c */
			short plockdae;
			short shell_strategy;
			short termsignal;
#ifdef SDSC
			short maxsoftulimit;	/* soft_user_limit */
			short maxhardulimit;	/* hard_user_limit */
#else
			short sreserved1;	/* RESERVED for future use */
			short sreserved2;	/* RESERVED for future use */
#endif
                        short sreserved3;       /* RESERVED for future use */
			short sreserved4;	/* RESERVED for future use */
			int debug;
			int defbatpri;
			int defdevpri;
			int maxcopies;
			int maxextrequests;
			int maxoperet;
			int maxprint;
			int opewai;
			int ireserved1;		/* RESERVED for future use */
			int ireserved2;		/* RESERVED for future use */
			int ireserved3;		/* RESERVED for future use */
			int ireserved4;		/* RESERVED for future use */
			long defdesrettim;
			long defdesretwai;
			long defnetrettim;
			long defnetretwai;
			long lifetime;
			long lreserved1;	/* RESERVED for future use */
			long lreserved2;	/* RESERVED for future use */
			long lreserved3;	/* RESERVED for future use */
			long lreserved4;	/* RESERVED for future use */
			char defprifor [MAX_FORMNAME+1];
			char defbatque [MAX_QUEUENAME+1];
			char defprique [MAX_QUEUENAME+1];
			char fixed_shell [MAX_SERVERNAME+1];
			char netdaemon [MAX_SERVERNAME+1];
		} genparams;			/* PRM_GENPARAMS */
		char logfile [MAX_PATHNAME+1];	/* PRM_LOGFILE */
		struct {
			char netserver [MAX_SERVERNAME+1];
			char netclient [MAX_SERVERNAME+1];
		} netprocs;		/* PRM_NETPROCS */
		struct {
			mid_t peer_mid;	/* Machine-id of network peer */
			short status;	/* Status bits */
			short connport;	/* Network connect to peer port# */
					/* if non-standard */
			short priority;	/* Priority of this network queue */
					/* (used only when the number */
					/* of network queues that want to */
					/* run exceeds the number that */
					/* can run simultaneously; see */
					/* Gblnetcount) */
			short acceptlimit;
					/* Maximum number of simultaneous */
					/* network connections that will be */
					/* accepted by the local NQS network*/
					/* daemon FROM the progeny of the */
					/* NQS daemon at the machine */
					/* identified by peer_mid; */
					/* (-1 if no specific limit for */
					/* this network peer) */
			short connectlimit;
					/* Maximum number of simultaneous */
					/* network connections that can be */
					/* created by the progeny of the */
					/* local NQS daemon TO the machine */
					/* identified by peer_mid.  The */
					/* value of connectlimit = the */
					/* runlimit of the network queue */
					/* for the machine identified by */
					/* peer_mid; (-1 if no specific */
					/* limit for this network peer) */
			long retrydelta;/* RESERVED for future use */
			char acptpasswd [MAX_NETPASSWORD+1];
					/* They must send it to us */
			char connpasswd [MAX_NETPASSWORD+1];
					/* We must send it to them */
		} netpeer;		/* PRM_NETPEER */
	} v;
};


/*
 *	NQS manager access list structure (used in Nqs_mgracct file).
 */
struct mgrdescr {
	uid_t manager_uid;		/* Manager user-id at machine */
	mid_t manager_mid;		/* Machine-id of manager account */
	short privileges;		/* Privilege bits */
};


/*
 *	NQS forms list structure (used in Nqs_forms file).
 */
struct formdescr {
	char forms [MAX_FORMNAME+1];	/* Null terminated NQS forms name */
};
 
 
/*
 *	NQS database descriptor structure.
 */
struct gendescr {
	short size;			/* Size of descriptor entry in chars */
					/* Negative values indicate unused */
	union {
		struct devdescr dev;	/* Device descriptor */
		struct formdescr form;	/* Forms descriptor */
		struct mgrdescr mgr;	/* NQS manager (operator) descriptor */
		struct paramdescr par;	/* General parameter descriptor */
		struct pipetodescr dest;/* Pipe queue destination descriptor */
		struct qmapdescr map;	/* Device-queue/device and pipe- */
					/* queue/destination mapping descr */
		struct quedescr que;	/* Queue descriptor */
		struct qcomplxdescr qcom;
					/* Queue complex descriptor */
	} v;
};


/*
 *	NQS queue/device and configuration files raw block structure.
 */
struct rawblock {
	union {
		ALIGNTYPE alignment;	/* Align the block */
		char chars [ATOMICBLKSIZ];/* Raw chars */
	} v;
};


/*
 *	NQS configuration file (i.e. queue, device, queue/device mapping,
 *	or pipe to remote queue/machine mapping file) handle.
 */
struct confd {
	short fd;			/* File descriptor */
	long size;			/* Size of file in chars */
	long vposition;			/* Virtual current file position */
	long rposition;			/* Real current file position */
					/* MUST be a multiple of ATOMICBLKSIZ*/
	long lastread;			/* Position of descr last read */
	struct rawblock *cache;		/* File block cache */
};


/*
 *	Server-to-shepherd process completion message structure.
 */
struct servermssg {
	long rcm;			/* Req/server completion code */
	long id;			/* identifier for request */
	char mssg [MAX_SERVMESSAGE+1];	/* Additional server message */
};


/*
 *	NQS request set search list structure.
 */
struct reqset {
	short selecttype;		/* Selection type SEL_ and SEL_FOUND */
					/* bits */
	uid_t uid;			/* Mapped user-id */
	union {
		char reqname [MAX_REQNAME+1];
					/* By reqname if SEL_REQNAME */
		struct {		/* By seq# and mid# if SEL_REQID */
			mid_t orig_mid;	/* Machine-id */
			long orig_seqno;/* Req sequence# */
		} reqid;
	} v;
	struct reqset *next;		/* Ptr to next req set selector */
};
