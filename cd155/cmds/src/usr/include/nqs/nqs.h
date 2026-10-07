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
 
/*++ nqs.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqs.h,v $
 *
 * DESCRIPTION:
 *
 *	NQS daemon constant definitions file.
 *
 *	WARNINGS:
 *	  Seven system types are defined:  BSD42, BSD43, SGI, SYS52 (OSF),
 *	  ULTRIX, UNICOS, and UTS.  Accordingly, the flag: SYSTEM_TYPE
 *	  in the Makefile must be appropriately defined.
 *
 *	  The #define type:  ALIGNTYPE  should be configured for the
 *	  appropriate supporting hardware (see below).
 *
 *	  See the file:
 *
 *		/usr/include/ioblksiz.h		(NPSN-UNIX systems)
 *		../npsn_compat/ioblksiz.h	(Non-NPSN UNIX systems)
 *
 *	  for the definition of the constant:  ATOMICBLKSIZ, which may
 *	  need to be changed depending upon the configuration of the
 *	  supporting system.
 *
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
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 02:44:02 $ $State: Exp
$)
 * $Log: nqs.h,v $
 * Revision 1.5  1994/11/19  02:44:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/11/02  00:06:11  mwan
 * R1.2 mods.
 *
 * Revision 1.3  1992/12/16  23:23:28  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:17:54  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:44  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:44:10  hender
 * Sterling version 4/22/87
 * 
 *
 */

#define	mid_t	long

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs.h	1.4 (nqs.h OSF/1 NQS2.0 GJK) 7/7/92";
#define SCCS
#endif
#endif


/*
 *
 *	Configurable parameters.  These #defines may be changed on a
 *	per-system basis as necessary.
 *
 *
 *	See also:
 *		/usr/include/ioblksiz.h		(NPSN UNIX systems)
 *		../npsn_compat/ioblksiz.h	(Non-NPSN UNIX systems)
 *
 *
 *	WARNING #1:
 *
 *		Once the NQS configuration database files
 *		have been created, the value of: ATOMICBLKSIZ
 *		as defined in ioblksiz.h (see above comments)
 *		must NEVER change!  To change ATOMICBLKSIZ
 *		after any of the NQS configuration database
 *		files have been created means certain death.
 *		If it becomes necessary to change ATOMICBLKSIZ,
 *		then shutdown NQS, rebuild NQS, delete all of
 *		the NQS configuration database files in the
 *		directory:
 *
 *			/usr/spool/nqs/root/database
 *
 *		restart NQS, and then redefine the NQS
 *		configuration via the qmgr program.
 *
 *		ALL other files can remain untouched, including
 *		the control and data files describing existing
 *		NQS requests.  Any queued requests on the local
 *		host will NOT be damaged by this operation.
 *
 *
 *	We also define which set of resource limits can be enforced by
 *	the supporting UNIX operating system here (see VALID_LIMITS),
 *	and the queue access set uid/gid cluster size (ACCLIST_SIZE).
 *
 *	Lastly, we define several key parameters controlling the
 *	maximum number of running NQS processes, and various transaction
 *	quota parameters.  These parameters are:
 *
 *
 *		MAX_GBLACCLIMIT:
 *			Global maximum number of simultaneous
 *			network connections that will be
 *			accepted by the local NQS network
 *			daemon FROM the progeny of all NQS
 *			daemons at all remote machines.
 *
 *			This value acts only as a ceiling.
 *			Lower values can be configured via
 *			the Qmgr program.
 *
 *		MAX_GBLBATLIMIT:
 *			Global maximum number of simultaneous
 *			running batch requests
 *
 *			This value acts only as a ceiling.
 *			Lower values can be configured via
 *			the Qmgr program.
 *
 *		MAX_GBLPIPLIMIT:
 *			Global maximum number of simultaneous
 *			running pipe queue servers.
 *
 *			This value acts only as a ceiling.
 *			Lower values can be configured via
 *			the Qmgr program.
 *
 *		MAX_GBLNETLIMIT:
 *			Global maximum number of simultaneous
 *			running network queue servers.
 *
 *			This value acts only as a ceiling.
 *			Lower values can be configured via
 *			the Qmgr program.
 *
 *		MAX_NOOFDEVICES:
 *			Global maximum number of NQS devices.
 *
 *		MAX_TRANSACTS:
 *			The maximum number of concurrent
 *			transactions.  Every request on the
 *			system has a transaction descriptor
 *			associated with it.  Thus, the value
 *			of this parameter sets a ceiling on
 *			the maximum number of requests that
 *			can be queued on the local host.
 *
 *			See WARNINGS #2, #3 and #4 if this
 *			value ever needs to be changed.
 *
 *		MAX_EXTREQUESTS:
 *			The maximum number of requests that
 *			can be queued at any given time on
 *			this machine, that have arrived from
 *			other remote machines.
 *
 *			See WARNINGS #2, #3 and #4 if this
 *			value ever needs to be changed.
 *
 *
 *
 *	There are two rules (or warnings), that the NQS system maintainer
 *	must observe when changing any of these parameters:
 *
 *
 *	    WARNING #2:
 *
 *		The following relation must hold:
 *
 *			MAX_EXTREQUESTS <= MAX_TRANSACTS.
 *
 *
 *	    WARNING #3:
 *
 *		Any change of:  MAX_TRANSACTS  must be CAREFULLY
 *		considered.  If there are any NQS requests present
 *		on the local host, then the value of MAX_TRANSACTS
 *		can only be INCREASED.  The presence of one or more
 *		requests on the local host DISALLOWS the possibility
 *		of MAX_TRANSACTS being DECREASED.
 *		  
 *		If MAX_TRANSACTS must be INCREASED, then NQS must be
 *		recompiled when the editing of this file is complete.
 *		When recompilation is complete, NQS must be shutdown,
 *		and reinstalled (while running as root) by the command:
 *
 *			make -f Makefile install
 *
 *		Then, the additional 'make' command of:
 *
 *			make -f Makefile transactions
 *
 *		must also be issued (again while running as root).
 *		At this point, the NQS daemon can be restarted and
 *		the effort is complete.
 *
 *		ALL other files can remain untouched, including the
 *		control and data files describing existing NQS
 *		requests.  Any queued requests on the local host
 *		will NOT be damaged by an INCREASE of MAX_TRANSACTS.
 *
 *		If it becomes necessary to reduce MAX_TRANSACTS,
 *		then the system maintainer(s) must follow the steps
 *		for INCREASING the value of MAX_TRANSACTS, with the
 *		exception that all requests queued on the local
 *		system, and all transaction descriptors on the local
 * 		system must be deleted, prior to the issuance of the
 *		command:
 *
 *			make -f Makefile transactions
 *
 *		Reducing the value of MAX_TRANSACTS essentially
 *		requires that NQS be reinstalled almost from scratch.
 *
 *
 *	    WARNING #4:
 *
 *		Any change of MAX_EXTREQUESTS must also be carefully
 *		considered.  Any INCREASE in this value is always
 *		perfectly safe.  There is a danger however in
 *		DECREASING this value.
 *
 *		One can only reduce MAX_EXTREQUESTS if one knows that
 *		the number of requests currently queued on the local
 *		system is LESS THAN or EQUAL TO the reduced value of
 *		MAX_EXTREQUESTS.  If this condition is not true, then
 *		NQS will be unable to requeue all of the requests upon
 *		rebooting.
 *
 *
 */
#define	ALIGNTYPE	long		/* Machine-alignment type */
#include IOBLKSIZ			/* See warning #1 */
#if	BSD42 | BSD43 | ULTRIX		/* Berkeley UNIX and ULTRIX */
#define	VALID_LIMITS	(LIM_PPCORE | LIM_PPDATA | LIM_PPPFILE | \
			 LIM_PPSTACK | LIM_PPWORK | LIM_PPCPUT | LIM_PPNICE)
#define ACCLIST_SIZE	13
#define	MAX_GBLACCLIMIT	10
#define	MAX_GBLBATLIMIT 10
#define	MAX_GBLPIPLIMIT 5
#define	MAX_GBLNETLIMIT	10
#define	MAX_NOOFDEVICES	40
#define	MAX_TRANSACTS	200		/* See WARNINGS #2, #3, #4 at the */
#define	MAX_EXTREQUESTS	160		/* beginning of this file before */
					/* changing any of these parameters */
#else
#if	SGI				/* Silicon Graphics Iris UNIX */
#define	VALID_LIMITS	(LIM_PPPFILE | LIM_PPNICE)
#define ACCLIST_SIZE	5
#define	MAX_GBLACCLIMIT	4
#define	MAX_GBLBATLIMIT 4
#define	MAX_GBLPIPLIMIT 2
#define	MAX_GBLNETLIMIT	4
#define	MAX_NOOFDEVICES	4
#define	MAX_TRANSACTS	200		/* See WARNINGS #2, #3, #4 at the */
#define	MAX_EXTREQUESTS	160		/* beginning of this file before */
					/* changing any of these parameters */
#else
#if	SYS52 | OSF				/* AT&T UNIX */
#ifdef SDSC
#define	VALID_LIMITS	(LIM_PPCORE | LIM_PPDATA | LIM_PPPFILE | \
			 LIM_PPSTACK | LIM_PPWORK | LIM_PPCPUT | LIM_PPNICE | \
			 LIM_PRCPUT | LIM_PRNCPUS)
#else
#define VALID_LIMITS    (LIM_PPPFILE | LIM_PPNICE)
#endif
#define ACCLIST_SIZE	13
#define	MAX_GBLACCLIMIT	10
#ifdef SDSC
#define	MAX_GBLBATLIMIT 64
#else
#define	MAX_GBLBATLIMIT 10
#endif
#define	MAX_GBLPIPLIMIT 5
#define	MAX_GBLNETLIMIT	10
#define	MAX_NOOFDEVICES	10
#define	MAX_TRANSACTS	500		/* See WARNINGS #2, #3, #4 at the */
#define	MAX_EXTREQUESTS	400		/* beginning of this file before */
					/* changing any of these parameters */
#else
#if	UTS				/* Amdahl Unix */
#define	VALID_LIMITS	(LIM_PPPFILE | LIM_PPNICE)
#define ACCLIST_SIZE	13
#define	MAX_GBLACCLIMIT	20
#define	MAX_GBLBATLIMIT 20
#define	MAX_GBLPIPLIMIT 10
#define	MAX_GBLNETLIMIT	20
#define	MAX_NOOFDEVICES	20
#define	MAX_TRANSACTS	1000		/* See WARNINGS #2, #3, #4 at the */
#define	MAX_EXTREQUESTS	800		/* beginning of this file before */
					/* changing any of these parameters */
#else
#if	UNICOS				/* Cray UNIX */
#define	VALID_LIMITS	(LIM_PPPFILE | LIM_PRMEM | LIM_PPMEM | \
			LIM_PRCPUT | LIM_PPCPUT | LIM_PPNICE)
#define ACCLIST_SIZE	13
#define	MAX_GBLACCLIMIT	20
#define	MAX_GBLBATLIMIT 20
#define	MAX_GBLPIPLIMIT 10
#define	MAX_GBLNETLIMIT	10
#define	MAX_NOOFDEVICES	10
#define	MAX_TRANSACTS	500		/* See WARNINGS #2, #3, #4 at the */
#define	MAX_EXTREQUESTS	250		/* beginning of this file before */
					/* changing any of these parameters */
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#endif
#endif

#ifdef SDSC
#define MAX_SOFTULIMIT 32000
#define MAX_HARDULIMIT 32000
#endif

#if OSF
#include	"sys_types_OSF.h"
#else
#include	<sys/typesgjk.h>	 	/* Get time_t definition */ /* in OSF sys_types_OSF.h */
#endif

#include	NMAP			/* Network mapping definitions */
#include	<quolim.h>		/* Quota limit definitions */

/*
 *	More configurable parameters.
 */
#define	ABORT_WAIT	60		/* Default time in secs to wait after */
					/* sending SIGTERM to all processes */
					/* for each req running in a queue */
					/* for which a PKT_ABOQUE packet */
					/* (see ../h/nqspacket.h) has */
					/* been received.  At the end of this */
					/* time, any remaining processes for */
					/* the queue in question receive a */
					/* SIGKILL signal.... */
#define	BSET_CPUVECSIZE	8		/* Size of bit-vector for CPU */
					/* resource allocation for a batch */
					/* queue (cardinality of set is */
					/* BSET_CPUVECSIZE * sizeof (ulong) */
					/* in bits) */
					/* *** WARNING *** */
					/* If this value is changed, then */
					/* the queues file will have to be */
					/* rebuilt.... */
#define	BSET_RS1VECSIZE	4		/* Size of bit-vector for RESERVED */
#define	BSET_RS2VECSIZE	4		/* resource allocation (types 1, 2) */
					/* for a batch queue (set cardinality*/
					/* is BSET_RS1VECSIZE * sizeof(ulong)*/
					/* in bits) */
					/* *** WARNING *** */
					/* If these values are changed, then */
					/* the queues file will have to be */
					/* rebuilt.... */
#define	EXPIRATION_SCAN	3600		/* Number of seconds to wait between */
					/* scanning for expired requests */
#define	MAX_COMPLXSPERQ	4		/* Maximum number of queue complexes */
					/* of which a given batch queue can */
					/* be a member */
#define	MAX_CTRLSUBDIRS	30		/* Number of ctrl file subdirs */
					/* *** WARNING *** */
					/* If this parameter is changed */
					/* AFTER NQS has already been */
					/* installed, then NQS must be */
					/* REINSTALLED.  Don't change this */
					/* unless you are compelled to do so.*/
#define	MAX_DATASUBDIRS	30		/* Number of data file subdirs */
					/* *** WARNING *** */
					/* (See WARNING for MAX_CTRLSUBDIRS) */
#define	MAX_ENVIRONMENT	5120		/* The maximum #of chars allowed */
					/* in the environment */
#define	MAX_ENVIRONVARS	400		/* Maximum #of environment vars for */
					/* a request server */
					/* WARNING: This value MUST be >= */
					/* MAX_QDESTS+6. */
#define	MAX_MACHINENAME	255		/* Longest acceptable machine-name */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#define	MAX_NICE	19		/* Maximum nice-value for local host */
#else
#if	BSD42 | BSD43 | ULTRIX
#define	MAX_NICE	20		/* Maximum nice-value for local host */
#else
BAD SYSTEM TYPE
#endif
#endif
#define	MIN_NICE	(-20)		/* Minimum nice-value for local host */
#define	MAX_OUTPSUBDIRS	30		/* Number of output file subdirs */
					/* *** WARNING *** */
					/* (See WARNING for MAX_CTRLSUBDIRS) */
#define	MAX_PATHNAME	255		/* Longest pathname supported by NQS */
					/* with the exception of MAX_REQPATH*/
#define	MAX_QCOMPLXNAME	15		/* Maximum length of a queue complex */
					/* name */
					/* *** WARNING *** */
					/* If this parameter is changed, */
					/* then the queue complex file will */
					/* have to be rebuilt */
#define	MAX_QDESTS	100		/* Maximum number of queue destina- */
					/* tions allowed for a pipe queue. */
					/* WARNING: This value MUST be <= */
					/* MAX_ENVIRONVARS. */
					/* WARNING: The queue destination */
					/* set for a queue server is passed */
					/* via the environment set.  This */
					/* value must not be so large as to */
					/* exceed the maximum environment */
					/* size of 5,120 bytes on many */
					/* UNIX systems.  Each destination */
					/* entry in the environment for a */
					/* for a pipe-queue server takes */
					/* up to 42-bytes. */
#define	MAX_QPRIORITY	63		/* The maximum priority that can be */
					/* assigned to an NQS queue.  This */
					/* value MUST be in the range: */
					/* [0..Max_int] */
#define	MAX_QRUNLIMIT	MAX_GBLRUNLIMIT	/* Maximum number of reqs allowed to */
					/* run simultaneously in the same */
					/* queue */
#define	MAX_QSPERCOMPLX	15		/* Maximum number of queues per */
					/* batch queue complex */
					/* *** WARNING *** */
					/* If this parameter is changed, */
					/* then the queue complex file will */
					/* have to be rebuilt */
#define	MAX_SERVERARGS	63		/* Maximum number of server arguments,*/
					/* including the server pathname. */
					/* WARNING: This constant MUST be */
					/* >= 2.  Code in nqs_reqser.c depends*/
					/* on this boundary condition */
#define	MAX_SERVERNAME	127		/* Maximum length of NQS server name */
#define	MAX_SERVMESSAGE	100		/* Maximum length of an NQS server */
					/* message */
#define	MAX_STATMSG	41		/* Maximum length of an NQS device */
					/* server status message */
#define	MAX_TDSCSUBDIRS	30		/* Number of transaction descriptor */
					/* subdirectories */
					/* *** WARNING *** */
					/* (See WARNING for MAX_CTRLSUBDIRS) */
#define MAKEGID		020000000000L	/* Store gids with this bit on */
#define	QAFILE_CACHESIZ	512		/* This many gids and uids are */
					/* read or written at a time */
					/* for a queue access file */
#define	QOFILE_CACHESIZ	100		/* This many queue entries are */
					/* are read or written at a time */
					/* for a queue ordering file */
#define	RESOURCE_WAIT	120		/* Number of seconds to wait for a */
					/* resource between attempts to get */
					/* the resource (see */
					/* ../src/netclient.c) */
#define	SHUTDOWN_WAIT	60		/* Default time in secs to wait after */
					/* sending SIGTERM to all processes */
					/* for each req running in any queue */
					/* upon reception of a PKT_SHUTDOWN */
					/* packet (see ../h/nqspacket.h). */
					/* At the end of this time, all */
					/* remaining processes of any */
					/* remaining NQS reqs receive a */
					/* SIGKILL signal. */
#define SOCKBUFSIZ	4096		/* When writing to a socket, */
					/* buffer to 4096 characters */
#define	TERMSIG_DEFAULT	9		/* Default signal sent to the pgrp */
					/* of an exiting request (SIGKILL) */
#if	BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
/* #define void int */			/* Uncomment if the compiler does */
					/* not support "void" function values */
#else
#if	BSD42 | UNICOS			/* While void is supported by the */
#define	void	int			/* Berkeley 4.2 and CRAY UNICOS */
					/* C compilers, ptrs to fns */
					/* returning void don't always seem */
					/* to work quite right.  This fixes */
					/* things for everyone. */
#else
BAD SYSTEM TYPE
#endif
#endif
/*
 *	These #defines should NEVER be changed.
 *
 *	ALL of the #defines below configure maximums which MUST be of
 *	identical value on ALL NQS machines.  If the maximum values
 *	are not identical, then it becomes impossible to safely pass
 *	requests between NQS machines.
 *
 *	Furthermore, some of these #defines should NEVER be changed
 *	for implementation reasons (like MAX_GBLRUNLIMIT).  Changing
 *	the definition of MAX_GBLRUNLIMIT will simply cause NQS to
 *	break.
 *
 *	I repeat.  Do not change any of these #defines.
 */
#define	MAX_ACCOUNTNAME	15		/* Maximum number of chars in an */
					/* account-name (7 extra for UNIX */
					/* implementations) */
#define	MAX_COPIES	32767		/* The maximum #of output copies for */
					/* a device request.  This value */
					/* MUST be in the range: [0..Max_int]*/
#define	MAX_DEVNAME	15		/* Longest shorthand device name */
#define	MAX_DEVPREF	4		/* Maximum number of devices in a */
					/* device request preference list */
#define	MAX_FORMNAME	15		/* Longest forms name */
#define	MAX_GBLRUNLIMIT	(MAX_GBLBATLIMIT+MAX_GBLNETLIMIT+MAX_GBLPIPLIMIT+ \
			 MAX_NOOFDEVICES)
					/* Maximum number of entries needed */
					/* in the Runvars[] array */
					/* *** WARNING *** */
					/* Thou shalt not change this unless */
					/* the implementation of NQS is */
					/* appropriately changed! */
#define	MAX_INSTAPERREQ	29		/* Maximum number of stage-in events */
					/* per-request.  The present */
					/* implementation will break if */
					/* this number is larger than 32.... */
#define	MAX_NETPASSWORD	15		/* Maximum size of an NQS network */
					/* connection password */
#define	MAX_OUSTAPERREQ	29		/* Maximum number of stage-out files */
					/* per-request.  The present */
					/* implementation will break if */
					/* this number is larger tnan 29.... */
#define	MAX_PACKET	4096		/* The maximum number of bytes that */
					/* can be contained in an NQS */
					/* inter-process communication */
					/* packet.  This value CANNOT and */
					/* MUST not be greater than 8k */
					/* (8192).  See ../lib/interwrite.c */
					/* and ../lib/interread.c.  */
					/* For System V based implementa- */
					/* tions of NQS, this value also */
					/* CANNOT be greater than the named- */
					/* pipe buffersize on the supporting */
					/* local system (usually 5120 bytes),*/
					/* in order to write packets to the */
					/* local NQS daemon in an indivisi- */
					/* ble fashion.  */
					/* For similar reasons, Berkeley */
					/* implementations of NQS cannot */
					/* have a MAX_PACKET value any */	
					/* than 4096.  */
					/* It is also EXTREMELY important */
					/* that this size be large enough */
					/* to accomodate the largest */
					/* message packet that might be sent.*/
					/* To summarize, do NOT ever change */
					/* this number! */
#define	MAX_PKTINTEGERS	150		/* The maximum number of integers */
					/* that can be contained in an NQS */
					/* inter-process communication */
					/* message packet.  This value */
					/* CANNOT be greater than 255. */
#define	MAX_PKTSTRINGS	150		/* The maximum number of strings */
					/* that can be contained in an NQS */
					/* inter-process communication */
					/* message packet.  This value */
					/* CANNOT be greater than 255. */
#define	MAX_PREDECESSOR	4		/* Maximum number of request */
					/* predecessors */
#define	MAX_QUEUENAME	15		/* Longest queue name */
#ifdef SDSC
#define MAX_ACCNAME     15              /* Longest account name */
#define MIN_NCPUS       1               /* min ncpus per request */
#define MAX_NCPUS       32000           /* just a large number. take it out
					 * when we find a way to get mesh sz */
#endif
#define	MAX_REQNAME	15		/* Longest request name */
#define	MIN_REQNICE	(-127)		/* Minimum request nice value */
#define	MAX_REQNICE	127		/* Maximum request nice value */
#ifdef SDSC
#define MIN_NODEGRP     0		/* min node group value */
#define MAX_NODEGRP	63		/* max node group value */
#endif
#define	MAX_REQPATH	1023		/* Longest batch request output file */
					/* path name or staged-file path name */
					/* excluding any explicit machine */
					/* name specification of the form: */
					/* <machine-name>:<path/filename> */
#define	MAX_RPRIORITY	63		/* Intra-queue req request priorities */
					/* are in the range [0..63], 63 */
					/* being the highest.  */
#define	MAX_SEQNO_USER	99999		/* Seq# for batch and device requests*/
					/* are in [0..MAX_SEQNO_USER] */
#define	MAX_SHELLNAME	63		/* Maximum length of a shell spec */
					/* on a Qsub or Qmod command line */
#define	TRANS_TIMEOUT	240		/* Number of seconds allowed to */
					/* complete a remote transaction */


/*
 *	ALL of the remaining #defines in this header file are not
 *	configurable, unless of course the affected implementation
 *	of NQS is changed appropriately.
 *
 *
 *	File descriptors.
 */
#define	STDIN		0		/* Standard-input is fd 0 */
#define	STDOUT		1		/* Standard-output is fd 1 */
#define	STDERR		2		/* Standard-error is fd 2 */


/*
 *	Queue status flags.
 */
#define	QUE_RUNNING	0000001		/* Queue is running */
					/* Otherwise queue is stopped */
#define	QUE_ENABLED	0000002		/* Queue is enabled to receive new */
					/* requests.  Otherwise queue is */
					/* closed to new requests. */
#define	QUE_NETRETRY	0000004		/* Network queue while technically */
					/* in the retry state, is waiting */
					/* to make a retry attempt in the */
					/* future */
#define	QUE_UPDATE	0100000		/* The queue image in the NQS data- */
					/* base needs to be updated for */
					/* this queue. */
#define	QUE_PIPEONLY	0040000		/* Reqs may only be added to this */
					/* queue from another pipe queue. */
#define QUE_BYGIDUID	0020000		/* Access restricted by gid, uid */


/*
 *	Queue type flags.
 */
#define	QUE_BATCH	1		/* Queue is for batch requests */
#define	QUE_DEVICE	2		/* Queue is for device requests */
#define	QUE_NET		3		/* Queue is a network machine queue */
#define	QUE_PIPE	4		/* Queue is a pipe queue */


/*
 *	Device status flags.
 */
#define	DEV_ACTIVE	0000001		/* Device active and handling request,*/
					/* Otherwise device is inactive, or */
					/* failed */
#define	DEV_ENABLED	0000002		/* Device is enabled, otherwise */
					/* device is disabled. */
#define	DEV_FAILED	0000004		/* If device fails, then this bit */
					/* is set and the DEV_ACTIVE and */
					/* DEV_ENABLED bits are cleared */

/*
 *	Pipe queue destination status flags.
 */
#define	DEST_ENABLED	0000002		/* Pipe queue dest enabled */
#define	DEST_FAILED	0000004		/* Pipe queue dest failed */
#define	DEST_RETRY	0000010		/* Pipe queue dest in retry mode */


/*
 *	NQS Queue manager Qmgr) privilege flags.
 */
#define	QMGR_OPER_PRIV	0000001		/* Account has operator privileges */
#define	QMGR_MGR_PRIV	0000002		/* Account has manager privileges */


/*
 *	Batch request shell script choice modes.
 *	(See nqs_spawn.c and shserver.c)
 */
#define	SHSTRAT_FREE	0		/* Let the user's login shell choose */
					/* which shell (e.g. csh, sh, ksh), */
					/* should be used to run the batch */
					/* request script */
#define	SHSTRAT_FIXED	1		/* Run batch request scripts with */
					/* site designated batch request */
					/* shell */
#define	SHSTRAT_LOGIN	2		/* Run batch request scripts with */
					/* the user's login shell */


/*
 *	NQS general parameter record TYPES (NOT values).
 */
#define	PRM_GENPARAMS	0		/* General NQS operating parameters */
#define	PRM_LOGFILE	1		/* Logfile parameter */
#define	PRM_NETPROCS	2		/* Network client, Network server */
#define	PRM_NETPEER	3		/* Network peer configuration */


/*
 *	NQS database descriptor types:
 */
#define	DSC_QUEUE	0		/* Queue descriptor */
#define	DSC_DEVICE	1		/* Device descriptor */
#define	DSC_QMAP	2		/* Queue/device mapping descriptor */
#define	DSC_PIPEQDEST	3		/* Pipe queue destination descriptor */
#define	DSC_PARAM	4		/* General parameter descriptor */
#define	DSC_MGR		5		/* NQS manager account-name descriptor*/
#define	DSC_FORM	6		/* NQS forms descriptor */
#define DSC_QCOMPLEX	7		/* Queue complex descriptor */

/*
 *	The following show flags are for NAS NQS.
 *	Some of these flags the same value as the original
 *	flags.  However, these flags would never be used in
 *	the same case so no question as to the proper meaning
 *	would be inferred
 */

#define SHO_ACCT        00000000002L    /* NOT USED AT THIS TIME Show acct */
#define SHO_GROUP       00000000004L    /* NOT USED AT THIS TIME Show group */
#define SHO_USER        00000000001L    /* Show user */
#define SHO_FULL        00000000400L    /* Show full detail  */
#define SHO_EXTEND      00000001000L    /* Show Extended format vendor ind. */
#define SHO_HOST        00001000000L    /* Show host */
#define SHO_QUE         00002000000L    /* Show queues  */
#define SHO_REQ         00004000000L    /* Show requests */
#define SHO_TARGET      00010000000L    /* Show target user */
#define SHO_NOHDR       00020000000L    /* Show NO headers OR trailers */
#define SHO_LIMITS      00040000000L    /* Show limits / resources */
#define SHO_BATCH       00100000000L    /* Show only batch queues */
#define SHO_PIPE        00200000000L    /* Show only pipe queues */
#define SHO_DEVICE      00400000000L    /* Show only device queues */
#define SHO_CMPLX       01000000000L    /* Show complex */
#ifdef SDSC
#define SHO_TSCHED      02000000000L    /* Show tennis court schedule */
#define SHO_VERBOSE     04000000000L    /* Show verbose output (132 characters per line) */
#endif

/*
 *	Show queue and show limit operation flags.
 */
#define	SHO_H_ACCESS	00000001L	/* Show queue access by user & group */
#define	SHO_H_DEST	00000002L	/* Show destinations for pipe queue */
#define SHO_H_LIM	00000004L	/* Show queue quota limits */
#define	SHO_H_MAP	00000010L	/* Show devices for device queue */
#define	SHO_H_RUNL	00000020L	/* Show run limit */
#define	SHO_H_SERV	00000040L	/* Show server for queue */
#define	SHO_H_TIME	00000100L	/* Show CPU usage times */

#define SHO_R_ALLUID	00000200L	/* Show requests belonging to all */
#define	SHO_R_LONG	00000400L	/* Long display of requests */
#define	SHO_R_MED	00001000L	/* Medium length display of requests */
#define	SHO_R_SHORT	00002000L	/* Short display of requests */

#define	SHO_RS_EXIT	00004000L	/* Select departing/exiting  requests */
#define	SHO_RS_RUN	00010000L	/* Select running/routing  requests */
#define	SHO_RS_STAGE	00020000L	/* Select staging-in requests */
					/* RESERVED FOR FUTURE USE */
#define	SHO_RS_QUEUED	00040000L	/* Select queued requests */
#define	SHO_RS_WAIT	00100000L	/* Select waiting requests */
#define	SHO_RS_HOLD	00200000L	/* Select holding requests */
#define	SHO_RS_ARRIVE	00400000L	/* Select arriving requests */

#define SHO_SHS		01000000L	/* Show shell strategy (used only by */
					/* qlimit; qstat does not use this) */

/*
 *	Request selection type:
 */
#define	SEL_REQNAME	0000001		/* Selection entry is by request-name */
#define	SEL_REQID	0000002		/* Selection entry is by request-id */
#define	SEL_FOUND	0100000		/* Selection entry has been found */



/*
 *	The following request flags are present both in the flags field
 *	of a rawreq structure, and in the status field of a request or
 *	subrequest structure.
 */
#define	RQF_OPERHOLD	0000001		/* An operator hold has been placed */
					/* on the request */
#define	RQF_USERHOLD	0000002		/* A user hold has been placed on */
					/* the request */
#define	RQF_EXTERNAL	0000004		/* Request arrived from another */
					/* remote machine */
#define	RQF_BEGINMAIL	0000010		/* Send mail when the request begins */
					/* execution */
#define	RQF_ENDMAIL	0000020		/* Send mail when the request ends */
					/* execution */
#define	RQF_RESTARTMAIL	0000040		/* Send mail on request restart */
#define	RQF_TRANSMAIL	0000100		/* Send mail when the request */
					/* completes transit to a new queue */
					/* from a pipe queue */
#define RQF_EXPORTVARS	0000200		/* Export all environment variables */
					/* from host shell */
#define	RQF_RESTARTABLE	0000400		/* Request is restartable */
#define RQF_RECOVERABLE 0001000		/* Request is recoverable */
#define RQF_SAVED	0002000		/* Request has been "checkpointed" by */
					/* operator */	
#define RQF_SUSPENDED	0004000		/* Request has been suspended by */
					/* operator */
/*
 *	The following request flags are present only in the status field
 *	of a request or subrequest structure.
 */
#define	RQF_EXPIRED	0000200000	/* The request is residing in a pipe */
					/* or network queue, and has passed  */
					/* its delivery expiration time.  The*/
					/* next routing or delivery attempt */
					/* must succeed, or the request will*/
					/* be deleted */
#define	RQF_INSTAFILES	0000400000	/* The batch request has files */
					/* that need to be staged-in */
#define	RQF_PREARRIVE	0001000000	/* The request is in the pre-arrive */
					/* state */
#define	RQF_PREDEPART	0002000000	/* The request is in the pre-depart */
					/* state */
#define	RQF_SIGQUEUED	0004000000	/* A signal is queued for this */
					/* running request whose process-*/
					/* group is not yet known */
#define	RQF_SIGREQUEUE	0010000000	/* Request should be requeued if */
					/* killed by a signal */
#define	RQF_SUBREQUEST	0020000000	/* This request structure is really */
					/* a subrequest */
#define	RQF_WASEXE	0040000000	/* During rebuild, it was detected */
					/* that this request had been */
					/* executing at the time of a */
					/* system crash, or an orderly */
					/* shutdown */
#define RQF_RECOVERED	0100000000	/* Request has been recovered after */
					/* an NQS restart following an    */
					/* orderly shutdown or crash 	    */
#define RQF_SIGSAVE	0200000000	/* Request should be "checkpointed" */
					/* if killed by a signal */
#define RQF_SIGKILL	0400000000	/* Request should be killed */

/*
 *	Transaction types.
 *	WARNING:  These values must be in [0..3].
 */
#define	TRA_UNALLOCATED	0		/* Transaction descriptor is */
					/* unallocated */
#define	TRA_REQSTATE	1		/* Transaction descriptor is */
					/* allocated, and describes the */
					/* state of a request */


/*
 *	NQS request queue states (RQS_).
 */
#define	RQS_DEPARTING	0000001		/* Departing/staging-out state */
#define	RQS_RUNNING	0000002		/* Running state */
#define	RQS_STAGING	0000004		/* Staging-in state */
#define	RQS_QUEUED	0000010		/* Queued state */
#define	RQS_WAITING	0000020		/* Waiting state */
#define	RQS_HOLDING	0000040		/* Holding state */
#define	RQS_ARRIVING	0000100		/* Arriving state */


/*
 *	NQS request transaction states (RTS_).
 *	WARNING:  These values must be in [0..7].
 */
#define	RTS_STASIS	0		/* This request is not doing any- */
					/* thing interesting at the moment */
#define	RTS_PREARRIVE	1		/* Preparing for arrive state */
#define	RTS_ARRIVE	2		/* Arriving state */
#define	RTS_PREDEPART	3		/* Preparing for depart state */
#define	RTS_DEPART	4		/* Departing state */
#define	RTS_STAGEIN	5		/* Staging-in state */
#define	RTS_EXECUTING	6		/* Executing state */
#define	RTS_STAGEOUT	7		/* Staging-out state */


/*
 *	NQS request type specifiers.
 */
#define	RTYPE_BATCH	1		/* Batch request */
#define	RTYPE_DEVICE	2		/* Device oriented request */


/*
 *	NQS request output mode specifiers.
 */
#define	OMD_SPOOL	0000001		/* Create stdout/stderr on machine */
					/* that executes the batch req, and */
					/* then spool the stdout/stderr to */
					/* the final destination */
#define	OMD_EO		0000002		/* Direct the stderr output to the */
					/* stdout file for the batch request. */
#define	OMD_NOSPOOL	0000004		/* Create stdout/stderr on named */
					/* machine and access it directly, */
					/* no matter what the network cost. */
					/* The output file is NOT spooled. */


/*
 *	NQS request output mode modifiers.
 */
#define	OMD_M_KEEP	0000010		/* The final destination of the */
					/* output file is somewhere on the */
					/* execution machine. */
					/* This flag exists so that a user */
					/* can specify that an output file */
					/* is to be kept on the execution */
					/* machine, without knowing the name */
					/* of the execution machine. */


/*
 *	NQS request header magic number(s).
 */
#define	REQ_MAGIC1	1618033989	/* Magic number used to verify */
					/* valid request control file (at */
					/* least to verify the first block */
					/* as valid) */


/*
 *	NQS request quota limits and nice value defined flags.
 *	VALID_LIMITS, the constant that defines which of the following
 *	are meaningful on a given operating system, appears at
 *	the top of this file.
 */
#define LIM_PPCORE	0000001		/* Request per-process */
					/* core file size limit exists */
#define	LIM_PPDATA	0000002		/* Request per-process */
					/* data size limit exists */
#define	LIM_PPPFILE	0000004		/* Request per-process permanent */
					/* file size limit exists */
#define	LIM_PRPFILE	0000010		/* Request per-request permanent */
					/* file space quota limit exists */
#define	LIM_PPQFILE	0000020		/* Request per-process quick */
					/* file size limit exists */
#define	LIM_PRQFILE	0000040		/* Request per-request quick */
					/* file space quota limit exists */
#define	LIM_PPTFILE	0000100		/* Request per-process temporary */
					/* file size limit exists */
#define	LIM_PRTFILE	0000200		/* Request per-request temporary */
					/* file space quota limit exists */
#define	LIM_PPMEM	0000400		/* Request per-process */
					/* memory size limit exists */
#define	LIM_PRMEM	0001000		/* Request per-request */
					/* memory space quota limit exists */
#define LIM_PPSTACK	0002000		/* Request per-process */
					/* stack size limit exists */
#define	LIM_PPWORK	0004000		/* Request per-process */
					/* working set quota exists */
#define	LIM_PPCPUT	0010000		/* Request per-process */
					/* CPU time limit exists */
#define	LIM_PRCPUT	0020000		/* Request per-request */
					/* CPU time quota limit exists */
#define	LIM_PPNICE	0040000		/* Request per-process */
					/* nice value exists */
#define LIM_PRDRIVES	0100000		/* Request per-process */
					/* limit on tape drives exists */
#define LIM_PRNCPUS	0200000		/* Request per-process limit */
					/* on number of cpus exists */

/*
 *	Additions for NAS NQS
 */
#define VERSION	2			/* version number for NQS */
#define	SUCCESS	0			
#define FAILURE	-1

/*
 *	All NQS constants are now defined.
 *	Include the NQS data structure definitions file.
 */
#include "nqsstruct.h"			/* Data structure definitions */

