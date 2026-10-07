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
 
/*++ nqspacket.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqspacket.h,v $
 *
 * DESCRIPTION:
 *
 *
 *	NQS local message packet types definition file.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 24, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1995/02/24 23:39:07 $ $State: Exp $)
 * $Log: nqspacket.h,v $
 * Revision 1.5  1995/02/24  23:39:07  kremenek
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
 * Revision 1.4  1994/11/19  02:44:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:06:21  mwan
 * R1.2 mods.
 *
 * Revision 1.2  1992/10/09  22:18:09  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:51  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:44:41  hender
 * Sterling version 4/22/87
 * 
 *
 */


/*
 *	NQS local message packet types.
 */
#define	PKT_ABOQUE	0		/* Abort and requeue all running */
					/* requests in a particular queue */
#define	PKT_ADDFOR	1		/* Add a form to the forms list */
#define	PKT_ADDNQSMAN	2		/* Add NQS manager to managers list */
#define	PKT_ADDQUEDES	3		/* Add pipe queue destination */
#define	PKT_ADDQUEDEV	4		/* Add queue to device mappings */
#define	PKT_ADDQUEGID	5		/* Add a gid to queue access set */
#define	PKT_ADDQUEUID	6		/* Add a uid to queue access set */
#define	PKT_CREDEV	7		/* Create new device */
#define	PKT_CREBATQUE	8		/* Create new batch queue */
#define	PKT_CREDEVQUE	9		/* Create new device queue */
#define	PKT_CREPIPQUE	10		/* Create new pipe queue */
#define	PKT_DELDEV	11		/* Delete device */
#define	PKT_DELFOR	12		/* Delete a form from the forms list */
#define	PKT_DELNQSMAN	13		/* Delete NQS manager from */
					/* managers list */
#define	PKT_DELQUE	14		/* Delete queue */
#define	PKT_DELQUEDES	15		/* Delete destination for pipe queue */
#define	PKT_DELQUEDEV	16		/* Delete queue to device mapping */
#define	PKT_DELQUEGID	17		/* Delete a gid from queue access set */
#define	PKT_DELQUEUID	18		/* Delete a uid from queue access set */
#define	PKT_DELREQ	19		/* Delete/kill request from queue */
#define	PKT_DISDEV	20		/* Disable device */
#define	PKT_DISQUE	21		/* Disable queue */
#define	PKT_ENADEV	22		/* Enable device */
#define	PKT_ENAQUE	23		/* Enable queue */
#define	PKT_FAMILY	24		/* Report process family */
#define PKT_LOCDAE	25		/* Lock the NQS daemon in memory */
#define	PKT_MODREQ	26		/* Modify request */
#define	PKT_MOVREQ	27		/* Move req between local queues */
#define	PKT_PURQUE	28		/* Purge queue */
#define	PKT_QUEREQ	29		/* Queue a request packet */
#define	PKT_QUEREQVLPQ	30		/* Queue a request via a LOCAL pipe */
					/* queue */
#define	PKT_REQCOM	31		/* Request completion packet */
#define	PKT_RMTACCEPT	32		/* Remote destination has */
					/* tentatively accepted the request */
					/* (transaction state */
					/*  = RTS_PREDEPART) */
#define	PKT_RMTARRIVE	33		/* The specified tentatively queued */
					/* request (transaction state */
					/* = RTS_PERARRIVE), is now arriving */
					/* (transaction state = RTS_ARRIVE) */
#define	PKT_RMTDEPART	34		/* Remote destination has committed */
					/* the request for arrival */
					/* (transaction state = RTS_DEPART) */
#define	PKT_RMTENADES	35		/* Enable remote queue destination */
#define	PKT_RMTENAMAC	36		/* Enable remote machine in all */
					/* queue destinations */
#define	PKT_RMTFAIDES	37		/* Mark remote queue destination */
					/* as completely failed */
#define	PKT_RMTFAIMAC	38		/* Mark remote machine destination */
					/* as completely failed */
#define	PKT_RMTQUEREQ	39		/* Tentatively queue a request to */
					/* be received from a remote machine */
					/* (transaction state=RTS_PREARRIVE) */
#define	PKT_RMTRECEIVED	40		/* A request that was previously */
					/* in the arriving state (transaction*/
					/* state is RTS_ARRIVE), has been */
					/* completely received from the */
					/* originating machine */
#define	PKT_RMTSCHDES	41		/* Schedule a remote queue */
					/* destination for later retry */
					/* pipe queue destination; mark */
					/* destination for later retry */
#define	PKT_RMTSCHMAC	42		/* Schedule a remote machine for */
					/* later retry */
#define	PKT_RMTSTASIS	43		/* A request in a pipe queue that */
					/* was in a RTS_PREDEPART transaction*/
					/* state is now in a stasis state */
#define	PKT_SCHEDULEREQ	44		/* Set the in-memory version of the */
					/* start-after time for a request */
					/* WITHOUT modifying the start-after */
					/* time specified in the request */
					/* control file.  This packet type */
					/* is used by pipe and network queue */
					/* servers to reschedule requests */
					/* for later retry when necessary */
#define	PKT_SENSEDAEMON	45		/* Sense whether or not the local */
					/* NQS daemon is running; NO MESSAGE */
					/* packets are sent to the NQS daemon!*/
#define	PKT_SETDEB	46		/* Set debug level */
#define	PKT_SETDEFBATPR	47		/* Set default intra-queue batch */
					/* priority */
#define	PKT_SETDEFBATQU	48		/* Set default batch queue */
#define	PKT_SETDEFDESTI	49		/* Set default maximum number of */
					/* seconds that a destination can */
					/* remain in the retry state without */
					/* being marked as failed */
#define	PKT_SETDEFDESWA	50		/* Set default destination retry */
					/* wait time in seconds */
#define	PKT_SETDEFDEVPR	51		/* Set default intra-queue */
					/* device-request priority */
#define	PKT_SETDEFPRIFO	52		/* Set default print forms */
#define	PKT_SETDEFPRIQU	53		/* Set default print queue */
#define	PKT_SETDEVFOR	54		/* Set device forms */
#define	PKT_SETDEVSER	55		/* Set server for device */
#define	PKT_SETFOR	56		/* Set forms list */
#define	PKT_SETLIFE	57		/* Set lifetime of pipe queue requests*/
#define PKT_SETLOGFIL	58		/* New logfile request */
#define	PKT_SETMAXCOP	59		/* Set max number of print copies */
#define	PKT_SETMAXOPERE	60		/* Set max device open retries */
#define	PKT_SETMAXPRISI	61		/* Set max print file size */
#define	PKT_SETNDFBATQU	62		/* Set no default batch queue */
#define	PKT_SETNDFPRIFO	63		/* Set no default print form */
#define	PKT_SETNDFPRIQU	64		/* Set no default print queue */
#define	PKT_SETNETCLI	65		/* Set NQS network client */
					/* and arguments */
#define	PKT_SETNETDAE	66		/* Set NQS network daemon */
					/* and arguments */
#define	PKT_SETNETSER	67		/* Set NQS network server */
					/* and arguments */
#define	PKT_SETNONETDAE	68		/* Set no NQS network daemon */
#define	PKT_SETNOPIPONL	69		/* Set no_pipeonly entry attribute */
					/* for queue */
#define PKT_SETNOQUEACC	70		/* Set no queue access */
					/* (root still has access anyway) */
#define	PKT_SETNQSMAI	71		/* Set NQS mail account */
#define	PKT_SETNQSMAN	72		/* Set NQS manager account set */
					/* to contain only the account of */
					/* root */
#define	PKT_SETOPEWAI	73		/* Set time to wait between failed */
					/* device open operations before */
					/* retry */
#define	PKT_SETPIPCLI	74		/* Set pipe queue client */
					/* and arguments */
#define	PKT_SETPIPONL	75		/* Set pipeonly entry attribute */
					/* for queue */
#define PKT_SETPPCORE	76		/* Set request per-process core */
					/* file size limit for a queue */
#define	PKT_SETPPCPUT	77		/* Set request per-process CPU time */
					/* limit for a queue */
#define	PKT_SETPPDATA	78		/* Set request per-process data- */
					/* segment size limit for a queue */
#define	PKT_SETPPMEM	79		/* Set request per-process memory */
					/* size limit for a queue */
#define	PKT_SETPPNICE	80		/* Set request per-process nice */
					/* value limit for a queue */
#define	PKT_SETPPPFILE	81		/* Set request per-process permfile */
					/* size limit for a queue */
#define	PKT_SETPPQFILE	82		/* Set request per-process quick file */
					/* size limit for a queue */
#define PKT_SETPPSTACK	83		/* Set request per-process stack */
					/* size limit for a queue */
#define	PKT_SETPPTFILE	84		/* Set request per-process tempfile */
					/* size limit for a queue */
#define	PKT_SETPPWORK	85		/* Set request per-process working */
					/* set quota for a queue */
#define	PKT_SETPRCPUT	86		/* Set request per-request CPU time */
					/* limit for a queue */
#define	PKT_SETPRDRIVES	87		/* Set request per-request tape */
					/* drives limit for a queue */
#define	PKT_SETPRMEM	88		/* Set request per-request memory */
					/* size limit for a queue */
#define PKT_SETPRNCPUS	89		/* Set request per-request # of */
					/* cpus limit for a queue */
#define	PKT_SETPRPFILE	90		/* Set request per-request permfile */
					/* space limit for a queue */
#define	PKT_SETPRQFILE	91		/* Set request per-request quick file */
					/* space limit for a queue */
#define	PKT_SETPRTFILE	92		/* Set request per-request tempfile */
					/* space limit for a queue */
#define	PKT_SETQUEDES	93		/* Set pipe queue destination */
#define	PKT_SETQUEDEV	94		/* Set queue to device mapping */
#define	PKT_SETQUEPRI	95		/* Set priority of queue */
#define	PKT_SETQUERUN	96		/* Set a queue run-limit */
#define	PKT_SETSHSFIX	97		/* Set a fixed shell strategy */
					/* for batch request execution */
#define	PKT_SETSHSFRE	98		/* Set a free shell strategy */
					/* for batch request execution */
#define	PKT_SETSHSLOG	99		/* Set a login shell strategy */
					/* for batch request execution */
#define PKT_SETUNRQUEAC 100		/* Set queue access to unrestricted */
#define PKT_SHUTDOWN	101		/* Shutdown request */
#define	PKT_STAQUE	102		/* Start queue */
#define	PKT_STOQUE	103		/* Stop queue */
#define PKT_UNLDAE	104		/* Unlock the NQS daemon from memory */
#define PKT_ADDQUECOM	105		/* Add a queue to a queue complex */
#define PKT_CRECOM	106		/* Create a queue complex */
#define PKT_DELCOM	107		/* Delete a queue complex */
#define PKT_MOVQUE	108		/* Move all requests on a queue to */
					/* another queue */
#define PKT_REMQUECOM	109		/* Remove a queue from a queue */
					/* complex */
#define PKT_SETCOMLIM	110		/* Set a queue complex run limit */
#define PKT_SETCPUACC	111		/* Set CPU access for a queue */
#define PKT_SETBATLIM	112		/* Set batch run limit */

#ifdef SDSC
#define PKT_SETNODEGRP  113		/* Set the node_groep for a queue */
#define PKT_SETSOFTULIM 114		/* Set the soft user limit */
#define PKT_SETHARDULIM	115		/* Set the hard user limit */
#define PKT_SETQUEQDLIM 116             /* Set the queued per queue limit */
#endif
