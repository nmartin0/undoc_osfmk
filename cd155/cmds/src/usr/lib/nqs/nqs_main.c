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
 
/*++ nqs_main.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_main.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module is the main NQS module and processes requests from
 *	NQS client processes.
 *
 *
 *	This module accepts the request packet types:
 *
 *		PKT_ABOQUE:
 *			Abort all running reqs in the named queue.
 *		PKT_ADDFOR:
 *			Add a form to the NQS forms list.
 *		PKT_ADDNQSMAN:
 *			Add an account to the NQS manager account
 *			access list.
 *		PKT_ADDQUECOM:
 *			Add a queue to a queue complex.
 *		PKT_ADDQUEDES:
 *			Add a pipe queue destination for the named
 *			queue.
 *		PKT_ADDQUEDEV:
 *			Add a queue/device mapping.
 *		PKT_ADDQUEGID:
 *			Add a gid to the access list for a queue.
 *		PKT_ADDQUEUID:
 *			Add a uid to the access list for a queue.
 *		PKT_CREDEV:
 *			Create a new device.
 *		PKT_CREBATQUE:
 *			Create a new batch queue.
 *		PKT_CRECOM:
 *			Create a queue complex.
 *		PKT_CREDEVQUE:
 *			Create a new device queue.
 *		PKT_CREPIPQUE:
 *			Create a new pipe queue.
 *		PKT_DELCOM:
 *			Delete a queue complex.
 *		PKT_DELDEV:
 *			Delete a device.
 *		PKT_DELFOR:
 *			Delete a form from the NQS forms list.
 *		PKT_DELNQSMAN:
 *			Delete an account from the NQS manager
 *			account access list.
 *		PKT_DELQUE:
 *			Delete a queue.
 *		PKT_DELQUEDES:
 *			Delete a destination for the named queue.
 *		PKT_DELQUEDEV:
 *			Delete a queue/device mapping.
 *		PKT_DELQUEGID:
 *			Delete a gid from the access list for a queue.
 *		PKT_DELQUEUID:
 *			Delete a uid from the access list for a queue.
 *		PKT_DELREQ:
 *			Delete the specified request.
 *		PKT_DISDEV:
 *			Disable a device.
 *		PKT_DISQUE:
 *			Disable a queue.
 *		PKT_ENADEV:
 *			Enable a device.
 *		PKT_ENAQUE:
 *			Enable a queue.
 *		PKT_FAMILY:
 *			Receive "process-family" of req server
 *			report.
 *		PKT_LOCDAE:
 *			Lock the NQS daemon in memory.  Make the NQS
 *			daemon immune to swapping.
 *		PKT_MODREQ:
 *			Modify queued req parameters.
 *		PKT_MOVQUE:
 *			Move all requests on a queue to another queue.
 *		PKT_MOVREQ:
 *			Move a request between local queues.
 *		PKT_PURQUE:
 *			Purge queued and waiting reqs from a queue.
 *		PKT_QUEREQ:
 *			Queue a new request.
 *		PKT_QUEREQVLPQ:
 *			Queue a request via a local pipe queue.
 *		PKT_REMQUECOM:
 *			Remove a queue from a queue complex.
 *		PKT_REQCOM:
 *			An NQS request has completed execution.
 *		PKT_RMTACCEPT:
 *			Remote destination has tentatively accepted
 *			the request.
 *		PKT_RMTARRIVE:
 *			The specified request previously queued in
 *			the pre-arrive state is now in the arriving
 *			state.
 *		PKT_RMTDEPART:
 *			Remote destination has committed the request
 *			for arrival.
 *		PKT_RMTENADES:
 *			Enable a pipe queue destination.
 *		PKT_RMTENAMAC:
 *			Enable a remote machine.
 *		PKT_RMTFAIDES:
 *			Mark a pipe queue destination as failed.
 *		PKT_RMTFAIMAC:
 *			Mark a remote machine as failed.
 *		PKT_RMTQUEREQ:
 *			Queue an NQS request via a remote pipe queue.
 *		PKT_RMTRECEIVED:
 *			The specified request that was previously in
 *			the arriving state has been completely received
 *			on the local machine.
 *		PKT_RMTSCHDES:
 *			Schedule a pipe-queue destination for retry.
 *		PKT_RMTSCHMAC:
 *			Schedule a machine destination for retry.
 *		PKT_RMTSTASIS:
 *			Request is no longer in the pre-depart state.
 *		PKT_SCHEDULEREQ:
 *			Schedule a request for later retry.
 *		PKT_SENSEDAEMON:
 *			Not handled by this module.
 *		PKT_SETBATLIM:
 *			Set the maximum global batch requests allowed
 *			to run at one time.
 *		PKT_SETCOMLIM:
 *			Set a queue complex run limit.
 *		PKT_SETDEB:
 *			Set the debug level.
 *		PKT_SETDEFBATPR:
 *			Set the default intra-queue batch-req priority.
 *		PKT_SETDEFBATQU:
 *			Set the default batch queue.
 *		PKT_SETDEFDESTI
 *			Set default maximum amount of time that a
 *			destination is allowed to be in the retry
 *			state before being marked as failed.
 *		PKT_SETDEFDESWA:
 *			Set the default number of seconds to wait before
 *			retrying failed destinations.
 *		PKT_SETDEFDEVPR:
 *			Set the default intra-queue device-req priority.
 *		PKT_SETDEFPRIFO:
 *			Set the default print forms.
 *		PKT_SETDEFPRIQU:
 *			Set the default print queue.
 *		PKT_SETDEVFOR:
 *			Set the device forms in the specified device.
 *		PKT_SETDEVSER:
 *			Set the device server and arguments for the
 *			specified device.
 *		PKT_SETFOR:
 *			Set the NQS forms list to the single specified
 *			form.
 *		PKT_SETLIFE:
 *			Set the lifetime of pipe and network queue reqs
 *			in seconds.
 *		PKT_SETLOGFIL:
 *			Switch NQS message output to the new log file.
 *			ALL messages of severity:  INFO, LOG, DEBUG, WARN,
 *			ERROR, and FATAL are written to this file.
 *			Note that messages of severity ERROR and FATAL
 *			are also written to the crash file.
 *		PKT_SETMAXCOP:
 *			Set the maximum number of print copies.
 *		PKT_SETMAXOPERE:
 *			Set max device open retries.
 *		PKT_SETMAXPRISI:
 *			Set the maximum size of an NQS print file.
 *		PKT_SETNDFBATQU:
 *			Set no default batch queue.
 *		PKT_SETNDFPRIFO:
 *			Set no default print forms.
 *		PKT_SETNDFPRIQU:
 *			Set no default print queue.
 *		PKT_SETNETCLI:
 *			Configure/set the NQS network client.
 *		PKT_SETNETDAE:
 *			Configure/set the NQS network daemon.
 *		PKT_SETNETSER:
 *			Configure/set the NQS network server.
 *		PKT_SETNONETDAE:
 *			Set no NQS network daemon.
 *		PKT_SETNOPIPONL:
 *			Set no restrictions on queue entry.
 *		PKT_SETNOQUEACC:
 *			Set queue access to reject all gid's, uid's.
 *		PKT_SETNQSMAI:
 *			Set the NQS mail account name.
 *		PKT_SETNQSMAN:
 *			Set the NQS manager account list to the single
 *			account of root, with full NQS privileges.
 *		PKT_SETOPEWAI:
 *			Set the number of seconds to wait between failed
 *			device opens.
 *		PKT_SETPIPCLI:
 *			Set pipe client and arguments.
 *		PKT_SETPIPONL:
 *			Set pipeonly entry attribute for queue.
 *		PKT_SETPPCORE:
 *			Set the per-process core file size limit
 *			for a queue.
 *		PKT_SETPPCPUT:
 *			Set the per-process cpu time limit
 *			for a queue.
 *		PKT_SETPPDATA:
 *			Set the per-process data segment size limit
 *			for a queue.
 *		PKT_SETPPMEM:
 *			Set the per-process memory size limit
 *			for a queue.
 *		PKT_SETPPNICE:
 *			Set the per-process nice value 
 *			for a queue.
 *		PKT_SETPPPFILE:
 *			Set the per-process permanent file size limit
 *			for a queue.
 *		PKT_SETPPQFILE:
 *			Set the per-process quick file size limit
 *			for a queue.
 *		PKT_SETPPSTACK:
 *			Set the per-process stack segment size limit
 *			for a queue.
 *		PKT_SETPPTFILE:
 *			Set the per-process temporary file size limit
 *			for a queue.
 *		PKT_SETPPWORK:
 *			Set the per-process working set size limit
 *			for a queue.
 *		PKT_SETPRCPUT:
 *			Set the per-request cpu time limit
 *			for a queue.
 *		PKT_SETPRDRIVES:
 *			Set the per-request tape drive limit
 *			for a queue.
 *		PKT_SETPRMEM:
 *			Set the per-request memory size limit
 *			for a queue.
 *		PKT_SETPRNCPUS:
 *			Set the per-request limit on number of processors
 *			for a queue.
 *		PKT_SETPRPFILE:
 *			Set the per-request permanent file size limit
 *			for a queue.
 *		PKT_SETPRQFILE:
 *			Set the per-request quick file size limit
 *			for a queue.
 *		PKT_SETPRTFILE:
 *			Set the per-request temporary file size limit
 *			for a queue.
 *		PKT_SETQUEDES:
 *			Set the destination set for the named queue to
 *			the single specified destination.
 *		PKT_SETQUEDEV:
 *			Set the queue/device mapping set for the named
 *			queue to the single specified device.
 *		PKT_SETQUEPRI:
 *			Set the inter-queue priority for the named
 *			queue.
 *		PKT_SETQUERUN:
 *			Set the run-limit for a batch or pipe queue.
 *		PKT_SETSHSFIX:
 *			Set fixed shell-choice strategy.
 *		PKT_SETSHSFRE:
 *			Set free shell-choice strategy.
 *		PKT_SETSHSLOG:
 *			Set login shell-choice strategy.
 *		PKT_SETUNRQUEAC:
 *			Set queue access to accept all gid's, uid's.
 *		PKT_SHUTDOWN:
 *			Shutdown the NQS daemon.
 *		PKT_STAQUE:
 *			Start an NQS queue.
 *		PKT_STOQUE:
 *			Stop an NQS queue.
 *		PKT_UNLDAE:
 *			Unlock the NQS daemon from memory.  Let the
 *			NQS daemon be subject to swapping just like
 *			every other process.
 *
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
 * REVISION HISTORY: ($Revision: 1.9 $ $Date: 1995/02/24 23:42:47 $ $State: Exp $)
 * $Log: nqs_main.c,v $
 * Revision 1.9  1995/02/24  23:42:47  kremenek
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
 * Revision 1.8  1995/01/24  01:11:00  davidl
 * Cleaned up console_write() so it works the way it was (apparently)
 * originally intended to work.  Also cleaned up some of the calls to
 * console_write() (for example, added missing close parenthesis).
 *
 *  Reviewer: doyle
 *  Risk: Low
 *  Benefit or PTS #: 11648
 *  Testing: Developer testing, EATs
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnqs/bytezero.c
 *             cmds_libs/src/usr/lib/nqs/netdaemon.c
 *             cmds_libs/src/usr/lib/nqs/nqs_abort.c
 *             cmds_libs/src/usr/lib/nqs/nqs_boot.c
 *             cmds_libs/src/usr/lib/nqs/nqs_main.c
 *
 * Revision 1.7  1994/11/19  02:53:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/10/07  00:46:54  kremenek
 *  Reviewer:  davidl doyle
 *  Risk: Low
 *  Benefit or PTS #: 5706
 *  Testing: EATS
 *  Module(s): cmds_libs/src/usr/lib/nqs/netdaemon.c, nqs_abort.c,
 *             nqs_boot.c, nqs_mai.c, nqs_main.c
 *
 * Revision 1.5  1993/11/02  00:57:20  mwan
 * R1.2 mods
 *
 * Revision 1.4  1993/05/24  22:55:20  mwan
 * Fix PTS 4590 - check if the new log_file exists before writing over it.
 *
 * Revision 1.2  1992/10/09  22:25:42  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:13  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:06:47  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_main.c	1.2 (nqs_main.c OSF/1 NQS2.0 GJK) 6/25/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>			/* Password file */
#include "nqs.h"			/* Include NQS constants/types	*/
#include "nqspacket.h"			/* NQS local message packet types */
#include "nqsvars.h"			/* NQS global variables and */
					/* directories */
#include "informcc.h"			/* NQS information codes */
#include "transactcc.h"			/* NQS transaction completion codes */
#if	UNICOS | SGI | SYS52 | UTS | OSF
#include <sys/lock.h>			/* Definitions for plock() calls*/
#include <sys/times.h>			/* Get struct tms definition */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>
#include <sys/resource.h>		/* Get struct rusage definition */
#else
BAD SYSTEM TYPE
#endif
#endif

#ifdef SDSC
#include <sys/stat.h>                   /* Include after nqs.h which */
                                        /* includes sys/types.h */
#include <syslog.h>
#include <values.h>
#endif



/*
 *	UNIX external definitions:
 *
 *	Defined in signal.h:	extern int (* signal ())();
 */

extern int errno;			/* In case not declared in errno.h */

/*
 *	External functions:
 */
extern unsigned alarm();		/* Get/set(or clear) alarm */
extern struct passwd *fetchpwuid();	/* Fetch passwd entry for user-id */
extern int intern32i();			/* Return number of 32-bit integers */
					/* in received message packet */
extern int internstr();			/* Return number of strings in the */
					/* received message packet */
extern long interr32i();		/* Return the n-th 32-bit precision */
					/* integer from the received message */
					/* packet as a signed long integer */
extern int interr32sign();		/* Return sign of the n-th 32-bit */
					/* precision integer from the */
					/* received message packet */
extern unsigned long interr32u();	/* Return the n-th 32-bit precision */
					/* integer from the received message */
					/* packet as an unsigned long integer*/
extern int interlstr();			/* Return the length of the n-th */
					/* string from the received message */
					/* packet */
extern int interread();			/* Read another message packet */
extern char *interrstr();		/* Return the n-th string from the */
					/* received message packet */
extern void nqs_aboque();		/* Abort queue */
extern void nqs_abort();		/* Abort NQS execution */
extern void nqs_boot();			/* Boot/initialize NQS */
extern void nqs_complt();		/* Send completion code to local */
					/* client process */
extern long nqs_delreq();		/* Delete a request */
extern void nqs_family();		/* Report process family */
extern void nqs_reqcom();		/* Req complete */
extern long nqs_modreq();		/* Modify a request */
extern long nqs_movque();		/* Move a queue of requests */
extern long nqs_movreq();		/* Move a request */
extern long nqs_quereq();		/* Queue a request */
extern void nqs_valarm();		/* Catch SIGALRM signal for virtual */
					/* timer (see nqs_vtimer.c) AND */
					/* enable virtual timers. */
extern void nsq_spawn();		/* A batch/device/pipe req was just */
					/* submitted, and therefore, a req */
					/* MAY need to be spawned.  This */
					/* function is in nqs_insque.c */
extern long pip_reqreceived();		/* Do update for PKT_RMTRECEIVED */
extern long pip_rmtaccept();		/* Do update for PKT_RMTACCEPT */
extern long pip_rmtarrive();		/* Do update for PKT_RMTARRIVE */
extern long pip_rmtdepart();		/* Do update for PKT_RMTDEPART */
extern long pip_rmtstasis();		/* Do update for PKT_RMTSTASIS */
extern long pip_schedulereq();		/* Do update for PKT_SCHEDULEREQ */
extern char *strcpy();			/* String copy function */
extern int strlen();			/* String length */
extern void udb_genparams();		/* Update the general NQS operating */
					/* parameters as recorded in the NQS */
					/* database */
extern void udb_netprocs();		/* Update the network processes */
					/* parameters as recorded in the NQS */
					/* database */
extern void udb_setfor();		/* Update the NQS database to record */
					/* the single specified valid form */
extern void udb_setnqsman();		/* Update the NQS database to record */
					/* the single specified manager */
extern long upc_addquecom();		/* Add a queue to a queue complex */
extern long upc_crecom();		/* Create a queue complex */
extern long upc_delcom();		/* Delete a queue complex */
extern long upc_remquecom();		/* Remove queue from a queue complex */
extern long upc_setcomlim();		/* Set a runlimit for a queue complex */
extern long upd_addquedes();		/* Add a queue destination */
extern long upd_delquedes();		/* Delete a queue destination */
extern long upd_destination();		/* Set destination state */
extern long upd_machine();		/* Set machine destination state */
extern long upd_setquedes();		/* Set pipe-queue destination set */
extern long upf_addfor();		/* Add form to the NQS forms set */
extern long upf_delfor();		/* Delete form from the NQS forms set */
extern long upm_addnqsman();		/* Add NQS manager to NQS manager set */
extern long upm_delnqsman();		/* Delete NQS manager from the NQS */
					/* manager set */
extern long upp_setdefbatque();		/* Set default batch queue */
extern long upp_setdefprifor();		/* Set default print forms */
extern long upp_setdefprique();		/* Set default print queue */
extern long upp_setgblbatlim();		/* Set global batch run limit */
extern long upp_setlogfil();		/* Set NQS log file */
extern long upq_aboque();		/* Abort running reqs in queue */
extern long upq_creque();		/* Create a new queue */
extern long upq_delque();		/* Delete a queue */
extern long upq_disque();		/* Disable a queue */
extern long upq_enaque();		/* Enable a queue */
extern long upq_purque();		/* Purge reqs from a queue */
extern long upq_setcpulim();		/* Set a cpu time limit for a queue */
extern long upq_setentatr();		/* Set queue entry attribute */
extern long upq_setniclim();		/* Set nice value for a queue */
#ifdef SDSC
extern long upq_setncpuslim();		/* Set ncpus value for a queue */
extern long upq_setnodegrp();		/* Set nodegrp value for a queue */
#endif
extern long upq_setpipcli();		/* Set pipe client */
extern long upq_setquepri();		/* Set inter-queue queue priority */
#ifdef SDSC
extern long upq_setquequeued();         /* Set queue queued-limit */
#endif
extern long upq_setquerun();		/* Set queue run-limit */
extern long upq_setquolim();		/* Set a quota limit for a queue */
extern long upq_staque();		/* Start a queue */
extern long upq_stoque();		/* Stop a queue */
extern long ups_shutdown();		/* Shutdown */
extern long upv_addquedev();		/* Add a queue/device mapping */
extern long upv_credev();		/* Create a new device */
extern long upv_deldev();		/* Delete a device */
extern long upv_delquedev();		/* Delete a queue/device mapping */
extern long upv_disdev();		/* Disable a device */
extern long upv_enadev();		/* Enable a device */
extern long upv_setdevfor();		/* Set device forms */
extern long upv_setdevser();		/* Set device server */
extern long upv_setquedev();		/* Set queue/device mapping set */


/*** main
 *
 *
 *	int main():
 *	Start of the NQS local daemon.
 */
int main (argc, argv)
int argc;
char *argv[];
{
	void badpacket();		/* Display bad packet message */
	int getpchar();			/* Return next input pipe char */
	void showbytes();		/* Show array of bytes */
	int validstr();			/* Was string in packet valid? */

	int integers;			/* Number of 32-bit precision signed-*/
					/* integer values present in the */
					/* received message packet. */
	int strings;			/* Number of character/byte strings */
					/* in the received message packet. */
	int pid;			/* Pid of requester */
	uid_t ruid;			/* Real user-id of requester */
	int i;				/* Loop variable */
	int i1;				/* Integer argument #1. */
	int i2;				/* Integer argument #2. */
	int i3;				/* Integer argument #3. */
	int i4;				/* Integer argument #4. */
	long l1;			/* Long integer argument #1. */
	long l2;			/* Long integer argument #2. */
	short s1;			/* Short integer argument #1. */
	short s2;			/* Short integer argument #2. */
	unsigned long ul1;		/* Unsigned long argument #1. */
#ifdef SDSC
        struct stat stat_buf;                   /* Stat() buffer */
#endif

	Argv0 = argv[0];		/* Save pointer to daemon argv[0] */
	Argv0size = strlen (Argv0);	/* and size for later use */
	/*
	 *  Boot/initialize NQS
	 */	
	nqs_boot();

	/*
	 *  Loop forever to read requests.
	 */
	if (Debug) {
		printf ("D$main(): Looping to read request packets.\n");
		fflush (stdout);
	}
	for (;;) {
		/*
		 *  Loop to read packets until packet error, or no more
		 *  packets and we are shutting down.
		 */

		switch (interread (getpchar)) {

		case -1:
			/*
			 *  A shutdown request was received earlier.
			 *  The protection on the NQS local daemon request
			 *  pipe has been set to deny write access to
			 *  ALL processes.  Furthermore, no process
			 *  has the request pipe open for writing at
			 *  this time.  It is therefore safe to
			 *  shutdown now.
			 */
			close (Read_fifo);	/* Close read FIFO file dscr */
			printf ("I$NQS shutdown complete.\n");
#ifdef SDSC
                        (void)syslog(LOG_INFO, "NQS shutdown complete.");
                        (void)syslog(LOG_INFO | LOG_DAEMON,
                        "NQS shutdown complete.");
                        console_write("NQS shutdown complete. ");
#endif
			exit (0);
		case -2:		/* Bad packet received.  Abort */
					/* execution. */
			printf ("F$Inconsistent message ");
			printf ("packet received by NQS local daemon.\n");
			printf ("I$No recovery possible.\n");
			errno = 0;	/* Not a system call error. */
			nqs_abort();	/* Abort NQS execution. */
		}
		/*
		 *  Otherwise, the packet is self-consistent.
		 *  We have a request to process.
		 */
		integers = intern32i();	/* Get number of 32-bit precision */
					/* integers in message packet */
		if (integers < 3) {
			/*
			 *  The message packet at a minimum must contain
			 *  a packet-type value, client process-id, and
			 *  client process real user-id, as the last three
			 *  (3) integers in the message packet.
			 */
			printf ("E$Received message packet ");
			printf ("authentication error.\n");
			printf ("I$Message packet ignored.\n");
			fflush (stdout);
			continue;	/* Loop to read next message packet */
		}
		integers -= 3;		/* Delete the 3 integer values of */
					/* packet type, client process-id,*/
					/* and client process real user-id*/
		pid = interr32i (integers + 2);
					/* Get local client process-id. */
		ruid = interr32i (integers + 3);
					/* Get local client process real-uid */
		strings = internstr();	/* Get number of strings in packet */
		/*
		 *  The packet has been minimally authenticated.
		 */
		alarm (0);		/* Disable any virtual alarm timers. */
					/* This is absolutely critical so that*/
					/* the alarm functions do not run */
					/* asynchronously at a time when the */
					/* NQS data structures are possibly */
					/* being updated. */
		if (Debug) {
			printf ("D$main(): Received packet ");
			printf ("from local process: %1d\n", pid);
			printf ("D$main(): Client process ");
			printf ("real user-id = %1d\n", ruid);
			printf ("D$main(): Packet type is: %1ld\n",
				interr32i (integers + 1));
			if (integers != 0 || strings != 0) {
				printf ("D$main(): Packet contents are as ");
				printf ("follows:\n");
				fflush (stdout);
				for (i = 1; i <= strings; i++) {
					printf ("D$main(): String [%1d] = ", i);
					showbytes (interrstr (i),
						   interlstr (i));
					fflush (stdout);
				}
				for (i = 1; i <= integers; i++) {
					printf ("D$main(): Integer ");
					if (interr32sign (i)) {
						printf ("[%1d] = %1ld\n",
							i, interr32i (i));
					}
					else {
						printf ("[%1d] = %1lu\n",
							i, interr32u (i));
					}
				}
			}
			fflush (stdout);
		}
		/*
		 *  Based upon the packet type, we perform a case statement.
		 */
		switch ((int) interr32i (integers + 1)) {
		case PKT_ABOQUE:	/* Abort all running reqs in a queue */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME)) {
				i1 = interr32i (1);
				if (i1 >= 0) {
					nqs_complt (
						upq_aboque (
							interrstr (1), i1
						),
						pid);
				}
				else badpacket (pid, ruid, 1);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ADDFOR:	/* Add an NQS form */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_FORMNAME)) {
				nqs_complt (upf_addfor (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ADDNQSMAN:	/* Add an NQS manager account */
			if (strings == 0 && integers == 3 &&
			    !interr32sign (1)) {
				nqs_complt (
					upm_addnqsman (
						(uid_t) interr32i (1),
						(mid_t) interr32i (2),
						(int) interr32i (3)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ADDQUECOM:	/* Add a queue to a queue complex */
			if (strings == 2 && 
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_QCOMPLXNAME)) {
				nqs_complt (
					upc_addquecom (
						interrstr (1), interrstr (2) ),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ADDQUEDES:	/* Add a queue destination */
			if (strings == 2 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_QUEUENAME)) {
				nqs_complt (
					upd_addquedes (
						interrstr (1), interrstr (2),
						(mid_t) interr32i (1)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ADDQUEDEV:	/* Add a queue/device mapping */
			if (strings == 2 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_DEVNAME)) {
				nqs_complt (
					upv_addquedev (
						interrstr (1), interrstr (2)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ADDQUEGID:	/* Add a gid to queue's access list */
			if (strings == 1 && integers == 1 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				nqs_complt (
					upq_addqueacc (
						interrstr (1),
						interr32i (1) | MAKEGID
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ADDQUEUID:	/* Add a uid to queue's access list */
			if (strings == 1 && integers == 1 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				nqs_complt (
					upq_addqueacc (
						interrstr (1),
						interr32i (1)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_CREBATQUE:	/* Create a new NQS batch queue */
			if (strings == 1 && integers == 3 &&
			    validstr (1, MAX_QUEUENAME)) {
				i1 = interr32i (1);
				i2 = interr32i (2);
				i3 = (interr32i (3) != 0);
				if (i1 < 0 || i1 > MAX_QPRIORITY || i2 < 0) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_creque (
						interrstr (1), QUE_BATCH,
						i1, i2, i3, NULL
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_CRECOM:	/* Create a queue complex */
			if (strings == 1 && 
			    validstr (1, MAX_QCOMPLXNAME)) {
				nqs_complt( upc_crecom( interrstr (1) ), pid );
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_CREDEV:	/* Create a new NQS device */
			if (strings == 4 && integers == 0 &&
			    validstr (1, MAX_DEVNAME) &&
			    validstr (2, MAX_FORMNAME) &&
			    validstr (3, MAX_PATHNAME) &&
			    validstr (4, MAX_SERVERNAME)) {
				nqs_complt (
					upv_credev (
						interrstr (1), interrstr (2),
						interrstr (3), interrstr (4)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_CREDEVQUE:	/* Create a new NQS device queue */
			if (strings == 1 && integers == 2 &&
			    validstr (1, MAX_QUEUENAME)) {
				i1 = interr32i (1);
				i2 = (interr32i (2) != 0);
				if (i1 < 0 || i1 > MAX_QPRIORITY) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_creque (
						interrstr (1), QUE_DEVICE,
						i1, 0, i2, NULL
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_CREPIPQUE:	/* Create a new NQS pipe queue */
			if (strings == 2 && integers == 3 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_SERVERNAME)) {
				i1 = interr32i (1);
				i2 = interr32i (2);
				i3 = (interr32i (3) != 0);
				if (i1 < 0 || i1 > MAX_QPRIORITY || i2 < 0) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_creque (
						interrstr (1), QUE_PIPE,
						i1, i2, i3, interrstr (2)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELCOM:	/* Delete a queue complex */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QCOMPLXNAME)) {
				nqs_complt (upc_delcom (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELDEV:	/* Delete a device */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_DEVNAME)) {
				nqs_complt (upv_deldev (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELFOR:	/* Delete an NQS form */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_FORMNAME)) {
				nqs_complt (upf_delfor (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELNQSMAN:	/* Delete an NQS manager account */
			if (strings == 0 && integers == 3 &&
			    !interr32sign (1)) {
				nqs_complt (
					upm_delnqsman (
						(uid_t) interr32i (1),
						(mid_t) interr32i (2),
						(int) interr32i (3)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELQUE:	/* Delete a queue */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_delque (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELQUEDES:	/* Delete a pipe queue destination */
			if (strings == 2 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_QUEUENAME)) {
				nqs_complt (
					upd_delquedes (
						interrstr (1), interrstr (2),
						(mid_t) interr32i (1)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELQUEDEV:	/* Delete a queue/device mapping */
			if (strings == 2 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_DEVNAME)) {
				nqs_complt (
					upv_delquedev (
						interrstr (1), interrstr (2)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELQUEGID:  /* Delete a gid from queue's access list */
			if (strings == 1 && integers == 1 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				nqs_complt (
					upq_delqueacc (
						interrstr (1),
						interr32i (1) | MAKEGID
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELQUEUID:  /* Delete a uid from queue's access list */
			if (strings == 1 && integers == 1 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				nqs_complt (
					upq_delqueacc (
						interrstr (1),
						interr32i (1)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DELREQ:	/* Delete an NQS req */
			if (strings == 0 && integers == 5) {
				i1 = interr32i (1);
				l2 = interr32i (2);
				i4 = interr32i (4);
				if (i1 < 0 || l2 < 0 || l2 > MAX_SEQNO_USER ||
				    i4 < 0) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					nqs_delreq ((uid_t) i1, l2,
						    (mid_t) interr32i (3), i4,
						    (int) interr32i (5)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DISDEV:	/* Disable a device */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_DEVNAME)) {
				nqs_complt (upv_disdev (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_DISQUE:	/* Disable a queue */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_disque (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ENADEV:	/* Enable a device */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_DEVNAME)) {
				nqs_complt (upv_enadev (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_ENAQUE:	/* Enable a queue */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_enaque (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_FAMILY:	/* Report server "process-family" */
#ifdef SDSC
			if (strings == 0 && integers == 3) {
				nqs_family ((int) interr32i (1),
					    (int) interr32i (2),
					    (int) interr32i (3));
#else
			if (strings == 0 && integers == 2) {
				nqs_family ((int) interr32i (1),
					    (int) interr32i (2));
#endif
			}
			else badpacket (pid, ruid, 0);
			break;
		case PKT_LOCDAE:	/* Lock NQS local daemon in memory. */
			if (strings == 0 && integers == 0) {
#if	UNICOS | SGI | SYS52 | UTS | OSF
				if (Plockdae == 0) {
					/*
					 *  The daemon is not presently locked
					 *  in memory.
					 */
					if (plock (PROCLOCK) == 0) {
						nqs_complt (TCML_COMPLETE, pid);
						Plockdae = 1;
						udb_genparams();
					}
					else nqs_complt (TCML_PLOCKFAIL, pid);
				}
				else nqs_complt (TCML_COMPLETE, pid);
#else
#if	BSD42 | BSD43 | ULTRIX
				nqs_complt (TCML_PLOCKFAIL, pid);
#else
BAD SYSTEM TYPE
#endif
#endif
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_MODREQ:	/* Modify req */
			if (strings == 0 && integers == 7) {
				i1 = interr32i (1);
				l2 = interr32i (2);
				if (i1 < 0 || l2 < 0 || l2 > MAX_SEQNO_USER) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					nqs_modreq ((uid_t) i1, l2,
						    (mid_t) interr32i (3),
						    interr32i (4),
						    interr32i (5),
						    interr32i (6),
						    interr32i (7)),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_MOVQUE:	/* Move all requests on a queue to */
					/* another queue */
			if (strings == 2 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_QUEUENAME)) {
				nqs_complt (nqs_movque ( interrstr (1),
					interrstr (2)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_MOVREQ:	/* Move req between local queues */
			if (strings == 1 && integers == 2 &&
			    validstr (1, MAX_QUEUENAME)) {
				l1 = interr32i (1);
				l2 = interr32i (2);
				if (l1 < 0 || l1 > MAX_SEQNO_USER) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					nqs_movreq (l1, (mid_t)l2,
						    interrstr(1)),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_PURQUE:	/* Purge queue */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_purque (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_QUEREQ:	/* Queue a NQS req */
			if (strings == 1 && integers == 0 &&
			    interlstr (1) > 0) {
				l1 = nqs_quereq (0, interrstr (1), 0L,
						(mid_t) 0, (char *) 0,
						(mid_t) 0);
				/*
				 *  Signal the client that we have made
				 *  our decision concerning the queueing
				 *  of the request.
				 */
				if (kill (pid, SIGALRM) == -1) {
					/*
					 *  What?  Unable to signal client?
					 */
					switch (errno) {
					case EINVAL:
						printf ("E$Invalid pid given");
						printf (" to nqs_main().\n");
						fflush (stdout);
						break;
					case EPERM:
						printf ("F$NQS not running ");
						printf ("as root in ");
						printf ("nqs_main().\n");
						nqs_abort();
					}
					/*
					 *  If the error is ESRCH, then we
					 *  don't get worried, it simply means
					 *  that the submitting process has
					 *  exited, or more seriously -- that
					 *  we were given a wrong process-id.
					 */
				}
				if ((l1 & XCI_FULREA_MASK) == TCML_SUBMITTED) {
					/*
					 *  The req was queued successfully.
					 *  We may need to spawn a req here.
					 */
					nsq_spawn();	/* MAYBE Spawn a req */
				}
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_QUEREQVLPQ:	/* Queue an NQS request via a local */
					/* pipe queue. */
			if (strings == 1 && integers == 2 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (
					nqs_quereq (
						1, (char *) 0, interr32i (1),
						(mid_t) interr32i (2),
						interrstr (1), (mid_t) 0
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_REMQUECOM:	/* Remove a queue from a queue */
					/* complex */
			if (strings == 2 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_QCOMPLXNAME)) {
				nqs_complt (
					upc_remquecom( interrstr(1),
						interrstr(2)),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_REQCOM:	/* Request-complete packet */
			if (strings == 2 && integers == 3 &&
#if	UNICOS | SGI | SYS52 | UTS | OSF
			    interlstr (1) == sizeof (struct tms)) {
#else
#if	BSD42 | BSD43 | ULTRIX
			    interlstr (1) == sizeof (struct rusage)) {
#else
BAD SYSTEM TYPE
#endif
#endif
				/*
				 *  We invoke nqs_reqcom() with a
				 *  pointer to the tms (or rusage)
				 *  structure passed in the request-
				 *  completion packet.
				 *
				 *  Pid has the process-id of the
				 *  exited NQS shepherd process for
				 *  the completed req request reporting
				 *  CPU usage times.
				 */
				nqs_reqcom ((long) interr32i (1),
					    (mid_t) interr32i (2),
					    (int) interr32i (3),
#if	UNICOS | SGI | SYS52 | UTS | OSF
					    (struct tms *) interrstr (1),
#else
#if	BSD42 | BSD43 | ULTRIX
					    (struct rusage *) interrstr (1),
#else
BAD SYSTEM TYPE
#endif
#endif
					    interrstr (2));
			}
			else badpacket (pid, ruid, 0);
			break;
		case PKT_RMTACCEPT:	/* Remote destination has */
					/* tentatively accepted the request */
			if (strings == 0 && integers == 2) {
				nqs_complt (
					pip_rmtaccept (interr32i (1),
						       interr32i (2)
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTARRIVE:	/* The specified request previously */
					/* queued in the pre-arrive state is */
					/* now in the arriving state */
			if (strings == 0 && integers == 2) {
				nqs_complt (
					pip_rmtarrive (interr32i (1),
						       interr32i (2)
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTDEPART:	/* Remote destination has committed */
					/* the request for arrival */
			if (strings == 0 && integers == 2) {
				nqs_complt (
					pip_rmtdepart (interr32i (1),
						       interr32i (2)
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTENADES:	/* Enable a pipe queue destination */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (
					upd_destination (
						interrstr (1), interr32i (1),
						0, 0L
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTENAMAC:	/* Enable a remote machine */
			if (strings == 0 && integers == 1) {
				nqs_complt (upd_machine (interr32i (1), 0, 0L),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTFAIDES:	/* Mark a pipe queue destination as */
					/* failed */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (
					upd_destination (
						interrstr (1), interr32i (1),
						1, 0L
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTFAIMAC:	/* Mark a remote machine as failed */
			if (strings == 0 && integers == 1) {
				nqs_complt (upd_machine (interr32i (1), 1, 0L),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTQUEREQ:	/* Queue an NQS request via a remote */
					/* pipe queue. */
			if (strings == 1 && integers == 3 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (
					nqs_quereq (
						2, (char *) 0, interr32i (1),
						(mid_t) interr32i (2),
						interrstr (1), interr32i (3)
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTRECEIVED:	/* The specified request that was */
					/* previously in the arriving state */
					/* has been completely received on */
					/* the local machine */
			if (strings == 0 && integers == 2) {
				nqs_complt (
					pip_reqreceived (interr32i (1),
							 interr32i (2)
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTSCHDES:	/* Schedule a pipe-queue destination */
			if (strings == 1 && integers == 2 &&
			    validstr (1, MAX_QUEUENAME) && !interr32sign (2)) {
				nqs_complt (
					upd_destination (
						interrstr (1), interr32i (1),
						2, interr32i (2)
					), pid);
			} else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTSCHMAC:	/* Schedule a machine for retry */
			if (integers == 2 && !interr32sign (2)) {
				nqs_complt (upd_machine (interr32i (1), 2,
							 interr32i (2)),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_RMTSTASIS:	/* Request was in the pre-depart */
					/* state, but the transaction */
					/* resolution was such that the */
					/* request has become free to be */
					/* delivered to another machine, and */
					/* is now in the stasis state */
			if (strings == 0 && integers == 2) {
				nqs_complt (
					pip_rmtstasis (interr32i (1),
						       interr32i (2)
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SCHEDULEREQ:	/* Schedule a request retry */
			if (strings == 0 && integers == 3 &&
			    !interr32sign (3)) {
				nqs_complt (
					pip_schedulereq (
						interr32i (1), interr32i (2),
						interr32i (3)
					), pid);
			} else badpacket (pid, ruid, 1);
			break;
		case PKT_SETBATLIM:	/* Set global batch run limit */
			if (strings == 0 && integers == 1 ) {
				nqs_complt (
					upp_setgblbatlim( interr32i(1)),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
#ifdef SDSC
		case PKT_SETSOFTULIM:	/* Set global soft user_limit */
			if (strings == 0 && integers == 1 ) {
				nqs_complt (
					upp_setsoftulim( interr32i(1)),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;

		case PKT_SETHARDULIM:	/* Set global hard user_limit */
			if (strings == 0 && integers == 1 ) {
				nqs_complt (
					upp_sethardulim( interr32i(1)),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;

#endif
		case PKT_SETCOMLIM:	/* Set queue complex run limit */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QCOMPLXNAME)) {
				nqs_complt (
					upc_setcomlim( interrstr(1),
						interr32i(1)),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEB:	/* Set debug level */
			if (strings == 0 && integers == 1) {
				Debug = interr32i (1);
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEFBATPR:	/* Set default intra-queue batch-req */
					/* Priority */
			if (strings == 0 && integers == 1) {
				i1 = interr32i (1);
				if (i1 < 0 || i1 > MAX_RPRIORITY) {
					badpacket (pid, ruid, 1);
				}
				else {
					Defbatpri = i1;
					udb_genparams();
					nqs_complt (TCML_COMPLETE, pid);
				}
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEFBATQU:	/* Set default batch queue */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upp_setdefbatque (interrstr (1)),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEFDESTI:	/* Set default destination */
					/* retry state time tolerance */
			if (strings == 0 && integers == 1 &&
			    !interr32sign (1)) {
				Defdesrettim = interr32i (1);
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEFDESWA:	/* Set default failed destination */
					/* wait retry time */
			if (strings == 0 && integers == 1 &&
			    !interr32sign (1)) {
				Defdesretwai = interr32i (1);
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEFDEVPR:	/* Set default intra-queue device-req */
					/* priority */
			if (strings == 0 && integers == 1) {
				i1 = interr32i (1);
				if (i1 < 0 || i1 > MAX_RPRIORITY) {
					badpacket (pid, ruid, 1);
				}
				else {
					Defdevpri = i1;
					udb_genparams();
					nqs_complt (TCML_COMPLETE, pid);
				}
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEFPRIFO:	/* Set default print forms */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_FORMNAME)) {
				nqs_complt (upp_setdefprifor (interrstr (1)),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEFPRIQU:	/* Set default print queue */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upp_setdefprique (interrstr (1)),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEVFOR:	/* Set device forms */
			if (strings == 2 && integers == 0 &&
			    validstr (1, MAX_DEVNAME) &&
			    validstr (2, MAX_FORMNAME)) {
				nqs_complt (
					upv_setdevfor (
						interrstr (1), interrstr (2)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETDEVSER:	/* Set device server */
			if (strings == 2 && integers == 0 &&
			    validstr (1, MAX_DEVNAME) &&
			    validstr (2, MAX_SERVERNAME)) {
				nqs_complt (
					upv_setdevser (
						interrstr (1), interrstr (2)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETFOR:	/* Set the NQS forms list */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_FORMNAME)) {
				udb_setfor (interrstr (1));
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETLIFE:	/* Set lifetime of NQS pipe and */
					/* network reqs */
			if (strings == 0 && integers == 1 &&
			    !interr32sign (1)) {
				Lifetime = interr32i (1);
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETLOGFIL:	/* Switch to new logfile */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_PATHNAME)) {
#ifdef SDSC
			    if (stat (interrstr(1), &stat_buf) != -1)
			        nqs_complt (TCML_ALREADEXI, pid);
			    else
#endif
				nqs_complt (upp_setlogfil (interrstr(1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETMAXCOP:	/* Set max number of print copies */
			if (strings == 0 && integers == 1 &&
			    !interr32sign (1)) {
				Maxcopies = interr32i (1);
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETMAXOPERE:	/* Set max device-open retry count */
			if (strings == 0 && integers == 1 &&
			    !interr32sign (1)) {
				Maxoperet = interr32i (1);
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETMAXPRISI:	/* Set max print file size */
			if (strings == 0 && integers == 1 &&
			    !interr32sign (1)) {
				Maxprint = interr32i (1);
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNDFBATQU:	/* Set no default batch queue */
			if (strings == 0 && integers == 0) {
				nqs_complt (upp_setdefbatque (""), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
 		case PKT_SETNDFPRIFO:	/* Set no default print forms */
			if (strings == 0 && integers == 0) {
				nqs_complt (upp_setdefprifor (""), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNDFPRIQU:	/* Set no default print queue */
			if (strings == 0 && integers == 0) {
				nqs_complt (upp_setdefprique (""), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNETCLI:	/* Configure/set the NQS network */
					/* client */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_SERVERNAME)) {
				strcpy (Netclient, interrstr (1));
				udb_netprocs();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNETDAE:	/* Configure/set the NQS network */
					/* daemon */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_SERVERNAME)) {
				strcpy (Netdaemon, interrstr (1));
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNETSER:	/* Configure/set the NQS network */
					/* server */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_SERVERNAME)) {
				strcpy (Netserver, interrstr (1));
				udb_netprocs();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNONETDAE:	/* Set no NQS network daemon */
			if (strings == 0 && integers == 0) {
				Netdaemon [0] = '\0';
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNOPIPONL:	/* Set no origination restrictions */
					/* on queue entry */
			if (strings == 1 && validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_setentatr (interrstr (1), 0),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNOQUEACC:	/* Set queue to reject all gids, uids */
			if (strings ==  1 && validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_setnoqueacc (interrstr (1)),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNQSMAI:	/* Set NQS mail account user-id and */
					/* name */
			if (strings == 0 && integers == 1 &&
			    !interr32sign (1)) {
				i1 = interr32i (1);
				if (fetchpwuid (i1) == NULL) {
					/*
					 *  No such account exists on the
					 *  local machine.
					 */
					nqs_complt (TCML_NOSUCHACC, pid);
				}
				else {
					Mail_uid = i1;
					udb_genparams();
					nqs_complt (TCML_COMPLETE, pid);
				}
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETNQSMAN:	/* Set the NQS manager set to the */
					/* single account of root with full */
					/* NQS privileges. */
			if (strings == 0 && integers == 0) {
				udb_setnqsman();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETOPEWAI:	/*Set the failed device open wait time*/
			if (strings == 0 && integers == 1) {
				i1 = interr32i (1);
				if (i1 < 0) badpacket (pid, ruid, 1);
				else {
					Opewai = i1;
					udb_genparams();
					nqs_complt (TCML_COMPLETE, pid);
				}
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPIPCLI:	/* Set pipe client */
			if (strings == 2 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_SERVERNAME)) {
				nqs_complt (
					upq_setpipcli (
						interrstr (1), interrstr (2)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPIPONL:	/* Set pipeonly entry attr for queue */
			if (strings == 1 && validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_setentatr (interrstr (1), 1),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPPCORE:	/* Set core file size limit for queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PPCORE
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
#ifdef SDSC
                case PKT_SETQUEQDLIM:   /* Set queued-limit for a batch queue */
                        if (strings == 1 && integers == 3 &&
                            !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
                                i1 = interr32u (1);
                                s1 = interr32i (2);
                                s2 = interr32i (3);
                                if (s2 == 1)
                                   /* infinite limit is being set */
                                   i1 = 0;
				else if (i1 == 0) badpacket (pid, ruid, 1);
                                if (i1 > MAXSHORT || invalidunits (s1) ||
                                    s2 < 0 || s2 > 1) {
                                        badpacket (pid, ruid, 1);
                                }
                                else nqs_complt (
                                        upq_setquequeued (
                                                interrstr (1), i1
                                        ),
                                        pid);
                        }
                        else badpacket (pid, ruid, 1);
                        break;
#endif
		case PKT_SETPPCPUT:	/* Set per-process cpu time */
					/* limit for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				s1 = (short) interr32i (2);
				s2 = (short) interr32i (3);
				if (s1 < 0 || s1 > 999 ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setcpulim (
						interrstr (1), interr32u (1),
						s1, s2, LIM_PPCPUT
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPPDATA:	/* Set data size limit for queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PPDATA
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPPMEM:	/* Set per-process memory size */
					/* limit for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PPMEM
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPPNICE:	/* Set nice value for queue */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME)) {
				i1 = (int) interr32i (1);
				if (i1 < MIN_REQNICE || i1 > MAX_REQNICE) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setniclim (
						interrstr (1), i1
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPPPFILE:	/* Set per-process permanent file */
					/* size limit for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PPPFILE
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPPSTACK:	/* Set stack size limit for queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PPSTACK
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPPTFILE:	/* Set per-process temporary file */
					/* size limit for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PPTFILE
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPPWORK:	/* Set working set size limit */
					/* for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PPWORK
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPRCPUT:	/* Set per-request cpu time limit */
					/* for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				s1 = (short) interr32i (2);
				s2 = (short) interr32i (3);
				if (s1 < 0 || s1 > 999 ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setcpulim (
						interrstr (1), interr32u (1),
						s1, s2, LIM_PRCPUT
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPRMEM:	/* Set per_request memory limit */
					/* for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PRMEM
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
#ifdef SDSC
		case PKT_SETPRNCPUS:	/* Set ncpu value for queue */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME)) {
				i1 = (int) interr32i (1);
				if (i1 < MIN_NCPUS || i1 > MAX_NCPUS) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setncpuslim (
						interrstr (1), i1
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;

		case PKT_SETNODEGRP:	/* Set node group value for queue */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME)) {
				i1 = (int) interr32i (1);
				if (i1 < MIN_NODEGRP || i1 > MAX_NODEGRP) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setnodegrp (
						interrstr (1), i1
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
#endif
		case PKT_SETPRPFILE:	/* Set per-request permanent file */
					/* size limit for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PRPFILE
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETPRTFILE:	/* Set per-request temporary file */
					/* size limit for a queue */
			if (strings == 1 && integers == 3 &&
			    !interr32sign (1) && validstr (1, MAX_QUEUENAME)) {
				ul1 = interr32u (1);
				s1 = interr32i (2);
				s2 = interr32i (3);
				if (ul1 > 2147483647 || invalidunits (s1) ||
				    s2 < 0 || s2 > 1) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquolim (
						interrstr (1),
						ul1, s1, s2, LIM_PRTFILE
					), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETQUEDES:	/* Set destination set for the */
					/* named queue to the single */
					/* specified destination. */
			if (strings == 2 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_QUEUENAME)) {
				nqs_complt (
					upd_setquedes (
						interrstr (1), interrstr (2),
						(mid_t) interr32i (1)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETQUEDEV:	/* Set device set for queue to the */
					/* single named device */
			if (strings == 2 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME) &&
			    validstr (2, MAX_DEVNAME)) {
				nqs_complt (
					upv_setquedev (
						interrstr (1), interrstr (2)
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETQUEPRI:	/* Set inter-queue queue priority */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME)) {
				i1 = interr32i (1);
				if (i1 < 0 || i1 > MAX_QPRIORITY) {
					badpacket (pid, ruid, 1);
				}
				else nqs_complt (
					upq_setquepri (
						interrstr (1), i1
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
 		case PKT_SETQUERUN:	/* Set run-limit for a batch or */
					/* pipe queue */
			if (strings == 1 && integers == 1 &&
			    validstr (1, MAX_QUEUENAME)) {
				i1 = interr32i (1);
				if (i1 <= 0) badpacket (pid, ruid, 1);
				else nqs_complt (
					upq_setquerun (
						interrstr (1), i1
					),
					pid);
			}
			else badpacket (pid, ruid, 1);
			break;
 		case PKT_SETSHSFIX:	/* Set fixed shell-choice strategy */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_SERVERNAME)) {
				Shell_strategy = SHSTRAT_FIXED;
				strcpy (Fixed_shell, interrstr (1));
				udb_genparams();
				nqs_complt (TCML_COMPLETE, pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SETSHSFRE:	/* Set free shell-choice strategy */
			Shell_strategy = SHSTRAT_FREE;
			Fixed_shell [0] = '\0';
			udb_genparams();
			nqs_complt (TCML_COMPLETE, pid);
			break;
		case PKT_SETSHSLOG:	/* Set login shell-choice strategy */
			Shell_strategy = SHSTRAT_LOGIN;
			Fixed_shell [0] = '\0';
			udb_genparams();
			nqs_complt (TCML_COMPLETE, pid);
			break;
		case PKT_SETUNRQUEAC:	/* Set queue to accept all gids, uids */
			if (strings ==  1 && validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_setunrqueacc (interrstr (1)),
					    pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_SHUTDOWN:	/* Shutdown request. */
			if (strings == 0 && integers == 1) {
				i1 = interr32i (1);
				if (i1 < 0) badpacket (pid, ruid, 1);
				else  {
					nqs_complt (
					ups_shutdown (
						pid, ruid, i1
					),
					pid);
				}
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_STAQUE:	/* Start a queue */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_staque (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_STOQUE:	/* Stop a queue */
			if (strings == 1 && integers == 0 &&
			    validstr (1, MAX_QUEUENAME)) {
				nqs_complt (upq_stoque (interrstr (1)), pid);
			}
			else badpacket (pid, ruid, 1);
			break;
		case PKT_UNLDAE:	/* Unlock NQS local daemon from memory*/
			if (strings == 0 && integers == 0) {
#if	UNICOS | SGI | SYS52 | UTS | OSF
				if (Plockdae) {
					/*
					 *  The daemon is presently locked
					 *  in memory.
					 */
					if (plock (UNLOCK) == 0) {
						nqs_complt (TCML_COMPLETE, pid);
						Plockdae = 0;
						udb_genparams();
					}
					else nqs_complt (TCML_PLOCKFAIL, pid);
				}
				else nqs_complt (TCML_COMPLETE, pid);
#else
#if	BSD42 | BSD43 | ULTRIX
				nqs_complt (TCML_PLOCKFAIL, pid);
#else
BAD SYSTEM TYPE
#endif
#endif
			}
			else badpacket (pid, ruid, 1);
			break;
		default:
			badpacket(pid, ruid, 1);
			break;
		}
		nqs_valarm ();		/* Re-enable any virtual timers */
					/* (see nqs_vtimer.c()). It is */
					/* now safe to do so since the NQS */
					/* data structures will remain */
					/* stable until the next message */
					/* packet is received. */
	}
}


/*** getpchar
 *
 *
 *	int getpchar():
 *
 *	Return the next character from the NQS FIFO "mailbox" pipe.
 *	Return -1 if no processes in the system have the request
 *		  FIFO mailbox named-pipe open and a shutdown
 *		  request was received earlier.
 *
 *	If a character is returned, then the value as a signed
 *	integer MUST be positive, and hold eight (8 - count them),
 *	bits of data!
 */
static int getpchar ()
{
	static char buffer [MAX_PACKET];
					/* Fifo receive buffer */
	static int bufptr = 0;		/* Fifo input buffer index ptr */
	static int n_read = 0;		/* # of chars returned from read() */
	register int no_packet_yet;	/* Flag used in read loop. */

	if (bufptr < n_read) {
		/*
		 *  Return the next character read from the input buffer.
		 */
		return (buffer [bufptr++] & 0377);
	}
	/*
	 *  Otherwise, we need to fetch another buffer from the NQS
 	 *  FIFO "mailbox" request pipe.  Read until an error other
	 *  than EINTR or a request arrives on the input pipe.
	 */
	bufptr = 1;			/* Reset input buffer ptr */
	no_packet_yet = 1;
	do  {
		if ((n_read = read (Read_fifo, buffer, MAX_PACKET)) == -1) {
			if (errno != EINTR) {
				printf ("F$Error reading NQS request pipe.\n");
				nqs_abort();
			}
		}
		else no_packet_yet = 0;	/* Successfully got a packet */
	} while (no_packet_yet);
	if (n_read == 0) {
		/*
		 *  No more requests.  A shutdown request was received
		 *  earlier AND all processes with active request traffic
		 *  have exited.
		 */
		return (-1);		/* No more characters will be read */
	}
	return (buffer [0] & 0377);	/* Return the next character */
}


/*** badpacket
 *
 *
 *	void badpacket():
 *
 *	Write a message to the NQS log process concerning the bad packet
 *	received.
 */
static void badpacket (pid, ruid, report_protocol_error)
int pid;				/* Process id */
uid_t ruid;				/* Real user-id */
int report_protocol_error;		/* Boolean report protocol error */
					/* in packet, if non-zero. */
{
	if (report_protocol_error) nqs_complt (TCML_PROTOFAIL, pid);
	printf ("E$Bad packet received from process %1d.\n", pid);
	printf ("I$Sender process real UID = %1u.\n", ruid);
	printf ("I$Bad packet ignored.\n");
	fflush (stdout);
}


/*** invalidunits
 *
 *
 *	int invalidunits():
 *	Return non-zero if the specified quota limit units are invalid.
 */
static int invalidunits (units)
short units;				/* Quota limit units */
{
	switch (units) {
	case QLM_BYTES:
	case QLM_WORDS:
	case QLM_KBYTES:
	case QLM_KWORDS:
	case QLM_MBYTES:
	case QLM_MWORDS:
	case QLM_GBYTES:
	case QLM_GWORDS:
		return (0);		/* Units are valid */
	}
	return (-1);			/* Units are invalid */
}


/*** showbytes
 *
 *
 *	void showbytes():
 *	Show byte/string datum as received in message packet.
 */
static void showbytes (bytes, length)
register char *bytes;			/* Array of bytes. */
register unsigned length;		/* Length of byte array. */
{
	register short ch;		/* Byte/character */
	register int col;		/* Column counter */
	short truncate_flag;		/* String truncated flag */

	col = 0;			/* # of columns written */
	truncate_flag = 0;
	if (length > 300) {
		truncate_flag = 1;	/* Truncate to 300 chars on */
		length = 300;		/* display */
	}
	while (length--) {
		if (col >= 24) {
			putchar ('\n');
			printf ("D$main(): ");
			col = 0;
		}
		ch = (*bytes & 0377);	/* Mask to 8 bits */
		bytes++;		/* Increment to next byte */
		if (ch < ' ' || ch > 127) {
			/*
			 *  This character is unprintable, or is a tab
			 *  character.
			 */
			printf ("[\\%03o]", ch);
			col += 6;
		}
		else {
			putchar (ch);
			col += 1;
		}
	}
	putchar ('\n');
	if (truncate_flag) {
		/*
		 *  String truncated.
		 */
		printf ("String diagnostic output truncated.\n");
	}
}


/*** validstr
 *
 *
 *	int validstr():
 *	Validate string.
 *
 *	Returns:
 *		1: if the specified string parameter in the received
 *		   message packet is not null, and is of length less
 *		   than, or equal to size_limit.
 *		0: otherwise.
 */
static int validstr (str_n, size_limit)
register int str_n;			/* Message packet string [n] */
register int size_limit;		/* Maximum size limit */
{
	register int size;

	size = interlstr (str_n);
	if (size && size <= size_limit) return (1);
	return (0);
}
