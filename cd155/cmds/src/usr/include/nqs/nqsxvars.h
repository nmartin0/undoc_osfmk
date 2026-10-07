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
 
/*++ nqsxvars.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqsxvars.h,v $
 *
 * DESCRIPTION:
 *
 *
 *	NQS global (and external) variable definitions file.
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
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:44:15 $ $State: Exp $)
 * $Log: nqsxvars.h,v $
 * Revision 1.4  1994/11/19  02:44:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:06:34  mwan
 * R1.2 mods.
 *
 * Revision 1.2  1992/10/09  22:18:19  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:57  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:45:08  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqsxdirs.h"			/* NQS global directory defintions */

extern char *Argv0;			/* Pointer to argv[0] string for */
					/* the NQS daemon */
extern int Argv0size;			/* Size of the argv[0] space for */
					/* the NQS daemon */
extern short Booted;			/* Boolean NQS booted flag.  If */
					/* non-zero, then reqs can be */
					/* spawned, etc.  Otherwise, NQS */
					/* is in the process of initializing */
					/* itself. */
extern int Debug;			/* NQS debug level */
extern int Defbatpri;			/* Default intra-queue batch-req */
					/* priority */
extern char Defbatque [MAX_QUEUENAME+1];/* Default batch queue name */
extern long Defdesrettim;		/* Default maximum number of seconds */
					/* that a destination can exist in a */
					/* retry state without being marked */
					/* as failed */
extern long Defdesretwai;		/* Default number of seconds to wait */
					/* between failed destination */
					/* connection attempts (cannot be 0) */
extern int Defdevpri;			/* Default intra-queue device-request*/
					/* priority */
extern long Defnetrettim;		/* Default maximum number of seconds */
					/* that a network queue can exist in */
					/* a failed retry state without being*/
					/* marked as failed */
extern long Defnetretwai;		/* Default number of seconds to wait */
					/* between failed network host */
					/* connection attempts (cannot be 0) */
extern char Defprifor [MAX_FORMNAME+1];	/* Default print form name */
extern char Defprique [MAX_QUEUENAME+1];/* Default print queue name */
extern int Extreqcount;			/* Number of requests that have */
					/* arrived at this machine from */
					/* other remote machines */
extern char Fixed_shell [MAX_SERVERNAME+1];
					/* Name of shell to be used if the */
					/* shell strategy is fixed */
extern int Fromlog;			/* Pipe from NQS log message process */
extern short Gblbatcount;		/* Total number of batch requests */
					/* running at the moment */
extern short Gblnetcount;		/* Total number network requests */
					/* running at the moment */
extern short Gblpipcount;		/* Total number of pipe requests */
					/* running at the moment */
extern long Lifetime;			/* Lifetime in seconds of requests */
					/* in pipe queues.  Also the */
					/* lifetime of requests in the */
					/* arriving state */
extern mid_t Locmid;			/* Machine-id of local host */
extern int Logdaepid;			/* Process-id of NQS log daemon */
					/* (0 if no daemon present) */
extern char Logfilename[MAX_PATHNAME+1];/* Logfile name */
extern uid_t Mail_uid;			/* User-id of NQS mail account */
extern int Maxcopies;			/* Maximum number of print copies */
extern int Maxextrequests;		/* Maximum number of requests */
					/* that arrived from remote machines */
					/* that can be simultaneously queued */
					/* on this machine */
extern short Maxgblacclimit;		/* Maximum number of simultaneous */
					/* connections that will be */
					/* accepted by the local NQS network */
					/* daemon from the progeny of remote */
					/* NQS daemons */
extern short Maxgblbatlimit;		/* Maximum number of batch requests */
					/* that can simultaneously run on */
					/* the local host */
extern short Maxgblnetlimit;		/* Maximum number of network requests */
					/* that can simultaneously run on */
					/* the local host */
extern short Maxgblpiplimit;		/* Maximum number of pipe requests */
					/* that can simultaneously run on */
					/* the local host */
extern int Maxoperet;			/* Maximum device-open retries */
extern int Maxprint;			/* Maximum number of bytes in a */
					/* print file */
#ifdef SDSC
extern int Maxsoftulimit;               /* soft user limit */
extern int Maxhardulimit;               /* hard user limit */
#endif
extern char Netclient[MAX_SERVERNAME+1];/* Empties network queues */
extern char Netdaemon[MAX_SERVERNAME+1];/* Spawns copies of Netserver */
extern int Netdaepid;			/* Process-id of NQS network daemon */
					/* (0 if no daemon present) */
extern char Netserver[MAX_SERVERNAME+1];/* Exec'd over child of Netdaemon */
extern struct queue *Net_queueset;	/* Network queue set */
extern struct qcomplex *New_qcomplex;	/* Most recently created queue */
					/* complex during the NQS boot */
					/* rebuild sequence.  (See */
					/* nqs_ldconf.c and nqs_updcom.c.) */
extern struct device *New_device;	/* Most recently created device      */
					/* during NQS boot rebuild sequence. */
					/* See nqs_ldconf.c and nqs_upddev.c.*/
extern struct qdestmap *New_qdestmap;	/* Most recently created queue/dest */
					/* mapping during the NQS boot */
					/* rebuild sequence.  (See */
					/* nqs_ldconf.c and nqs_upddev.c.) */
extern struct qdevmap *New_qdevmap;	/* Most recently created queue/device*/
					/* mapping during the NQS boot */
					/* rebuild sequence.  (See */
					/* nqs_ldconf.c and nqs_upddev.c.) */
extern struct queue *New_queue;		/* Most recently created queue  */
					/* during NQS boot rebuild sequence. */
					/* See nqs_ldconf.c and nqs_updque.c.*/
extern struct queue *Nonnet_queueset;	/* Non-networked queue set */
extern short Noofdevices;		/* Number of devices presently in */
					/* existence */
extern int Opewai;			/* Number of seconds to wait between */
					/* failed device opens */
extern struct pipeto *Pipetoset;	/* Destination set for all local */
					/* pipe queues */
extern short Plockdae;			/* Plock() status of daemon */
extern struct queue *Pribatqueset;	/* Priority ordered batch queue set */
extern struct queue *Prinetqueset;	/* Priority ordered network queue set*/
extern struct queue *Pripipqueset;	/* Priority ordered pipe queue set */
extern struct qcomplex *Qcomplexset;	/* Queue complex set */
extern struct running Runvars [MAX_GBLRUNLIMIT];
					/* Variables for running requests */
extern long Seqno_user;			/* Next available seq# for batch and */
					/* device requests */
extern short Shell_strategy;		/* Batch request shell script choice */
					/* strategy = [SHSTRAT_FREE, */
					/* SHSTRAT_FIXED] */
extern short Shutdown;			/* Non-zero if shutdown in progress */
extern short Termsignal;		/* Terminating signal sent to the */
					/* process group of an exiting */
					/* request */
extern long Udbgenparams;		/* Offset of general parameters */
					/* record in the params file */
extern long Udblogfile;			/* Offset of the logfile record */
					/* in the params file */
extern long Udbnetprocs;		/* Offset of the network processes */
					/* record in the params file */
extern struct confd *Devicefile;	/* Device descriptor file */
extern struct confd *Netqueuefile;	/* Network queue descriptor file */
extern struct confd *Qcomplexfile;	/* Queue complex descriptor file */
extern struct confd *Queuefile;		/* Queue descriptor file */
extern struct confd *Qmapfile;		/* Queue/device/destination mapping */
					/* file */
extern struct confd *Pipeqfile;		/* Pipe queue destination file */
extern struct confd *Paramfile;		/* General parameters file */
extern struct confd *Mgrfile;		/* NQS manager access list file */
extern struct confd *Formsfile;		/* NQS forms list file */
extern struct device *Devset;		/* Device set */
extern int Read_fifo;			/* Read FIFO descriptor used to */
					/* receive new request packets */
extern int Write_fifo;			/* Write FIFO descriptor.  This file */
					/* descriptor has the FIFO pipe */
					/* open for writing to prevent the */
					/* NQS daemon from exiting when	*/
					/* no processes have the request */
					/* pipe open.  IT IS CLOSED when */
					/* a shutdown request is received */
					/* so that the daemon can shutdown */
					/* when the last requesting proc */
					/* closes the FIFO request pipe */
					/* or exits.  Furthermore, this file */
					/* descriptor is used by pipe queue */
					/* servers and shepherd processes to */
					/* send appropriate message packets */
					/* to the local NQS daemon */
