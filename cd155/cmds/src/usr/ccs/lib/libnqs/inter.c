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
 
/*++ inter.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/inter.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the entry point functions:
 *
 *		exiting(),
 *		interset(),
 *		inter(), and
 *		interconn()
 *
 *	which respectively:
 *
 *		1) Delete the inter-process communication file
 *		   established for ALL message packets with the
 *		   EXCEPTION of PKT_FAMILY, PKT_REQCOM, and
 *		   PKT_QUEREQ packets, and break the connection
 *		   which was established to the local NQS daemon.
 *		2) Allow the caller to specify the open file
 *		   descriptor that is to be used to communicate
 *		   requests to the local NQS daemon.
 *		3) Send message packets to the local NQS daemon.
 *		4) Return a file descriptor which is open for
 *		   writing on a fifo pipe to the local daemon.
 *
 *	Interconn() is not needed on systems with named pipes.
 *	On such systems, the function of interconn() is subsumed
 *	into inter().
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 2, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/12/07 22:12:20 $ $State: Exp $)
 * $Log: inter.c,v $
 * Revision 1.4  1994/12/07  22:12:20  davidl
 *  Code-reviewed and cleaned up all socket code in MACS and NQS:
 *
 *  * Added some bzero() calls for sockaddr_in structures before bind()
 *    and accept() operations.
 *
 *  * Changed some occurrences of gethostbyname(host) (which can cause
 *    failures on systems with multiple Ethernets) to
 *    gethostbyname("localhost").
 *
 *  * Removed some dead (never executed) code.
 *
 *  * Replaced a few hard-coded 0's with appropriate socket #defines.
 *
 *  Reviewer: doyle
 *  Risk: Medium (has not been tested with multiple Enets)
 *  Benefit or PTS #: 11659
 *  Testing: EATs (only)
 *  Module(s):
 *
 *   MACS:	cmds_libs/src/usr/ccs/lib/libmacs/open.c
 * 	cmds_libs/src/usr/lib/macs/macd.c
 * 	cmds_libs/src/usr/lib/macs/ports.c
 *
 *   NQS:	cmds_libs/src/usr/ccs/lib/libnqs/establish.c
 * 	cmds_libs/src/usr/ccs/lib/libnqs/inter.c
 * 	cmds_libs/src/usr/lib/nqs/netdaemon.c
 * 	cmds_libs/src/usr/lib/nqs/res_msg.c
 * 	cmds_libs/src/usr/lib/nqs/smd_msg.c
 *
 * Revision 1.3  1994/11/19  02:26:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:39  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:49  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:50:10  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)inter.c	1.3 (inter.c OSF/1 NQS2.0 GJK) 7/7/92";
#define SCCS
#endif

#define	TIMEOUT		120	/* Time to wait in seconds before assuming */
				/* the demise of the NQS daemon.... */
#include "nqs.h"		/* NQS definitions and types */
#include "nqsxdirs.h"		/* NQS external directories */
#include "nqspacket.h"		/* NQS local message packets */
#include "transactcc.h"		/* Transaction completion codes */
#if	SGI | SYS52 | UNICOS | UTS | OSF
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
#if	BSD42 | BSD43
#include "netpacket.h"			/* Network packet types */
#include NETDB				/* Network database header file; */
					/* Proto/Makefile will tell us */
					/* where to find netdb.h */
#include <sys/socket.h>			/* Socket stuff */
#include <netinet/in.h>			/* Internet stuff */
#else
BAD SYSTEM TYPE
#endif
#endif


/*
 *	UNIX external definitions:
 *
 *	Defined in signal.h:	extern int (* signal ())();
 */

extern int errno;		/* In case not declared in errno.h */

extern unsigned alarm();	/* Alarm() system call */
extern uid_t getuid();		/* Get real user UID */
#if	NET_ORDER
#else
extern unsigned short htons();		/* A macro if NET_ORDER, */
					/* a routine otherwise */
#endif
extern int interw32i();		/* Write 32-bit precision integer to packet */
extern int interfmt();		/* Format NQS inter-process communication */
				/* packet. */


/*
 *	Variables global within this module, and local to this
 *	module.
 */
static int fifopipe = -1;	/* Write FIFO pipe to the NQS daemon */
static int interproc = -1;	/* Inter-process communications FROM */
				/* the NQS daemon to the local client*/
				/* process. */
int sem_timeout;		/* Records the number of times that a*/
				/* SIGALRM signal has been caught */

/*** exiting
 *
 *
 *	void exiting():
 *
 *	Discard any connection to the local NQS daemon, and
 *	delete any inter-process communication file established
 *	for the caller.
 *
 *	NOTE/WARNING:
 *		The caller MUST be running with a current
 *		working directory of the NQS root directory.
 */
void exiting()
{
	char path [MAX_PATHNAME+1];	/* Name of inter-process comm. file */
					/* used to RECEIVE completion codes */
					/* from the local NQS daemon. */

	if (interproc != -1) {
		/*
		 *  There is an inter-process communications file present
		 *  to receive completion codes from the NQS daemon.
		 */
		sprintf (path, "%s/%1d", Nqs_inter, getpid());
		unlink (path);		/* Delete inter-process comm. file */
		close (interproc);	/* Close it. */
		interproc = -1;		/* Record file as closed */
	}
	close (fifopipe);		/* Break connection to daemon */
	fifopipe = -1;			/* Indicate that there is not conn. */
}




/*** interset
 *
 *
 *	void interset():
 *
 *	Set the file descriptor used to communicate with the NQS
 *	daemon to the specified value.  The file descriptor MUST
 *	already be open and connected to the NQS daemon request
 *	pipe.
 */
void interset (fd)
int fd;
{
	fifopipe = fd;		/* Set FIFO file-descriptor */
}


/*** inter
 *
 *
 *	long inter():
 *
 *	Send a message packet to the NQS daemon.  If the message packet
 *	is sent, then the caller's execution (depending on the packet-
 *	type) is delayed until a SIGALRM acknowledge signal is received
 *	from the NQS daemon.
 *
 *	NOTE/WARNING:
 *		If a PKT_QUEREQ is to be sent, then the working directory
 *		of the caller MUST be the NQS new request directory.
 *		Otherwise, the working directory of the caller MUST be
 *		the NQS root directory.
 *
 *	NOTE/WARNING:
 *		This function uses the signal:  SIGALRM  to communicate
 *		with the NQS daemon.  Thus, do not under any circumstances
 *		be expecting a SIGALRM signal to be sent from another
 *		process other than the NQS daemon when invoking this function.
 *
 *		It should be noted however, that the SIGALRM signal handler
 *		is restored upon completion to the value it had upon the
 *		invocation of this function.
 *
 *		Exceptions to the SIGALRM signal protocol are made for
 *		PKT_FAMILY and PKT_REQCOM packets, which do not use a
 *		SIGALRM signal as part of the protocol when communicating
 *		with the NQS daemon.
 *
 *	Returns:
 *
 *	    For PKT_FAMILY and PKT_REQCOM packets:
 *
 *
 *		TCML_COMPLETE:	 The message packet was successfully
 *				 sent.
 *
 *		TCML_INTERNERR:	 Indicates that the contents of the
 *				 message packet, as specified by the
 *				 caller, are too large to fit in an NQS
 *				 message packet, and can therefore not
 *				 be sent to the NQS daemon.
 *
 *		TCML_NOLOCALDAE: Indicates that the message packet could
 *				 not be delivered. Either there is no
 *				 local daemon present, or interconn()
 *				 was never called.
 *
 *
 *
 *	    For PKT_QUEREQ packets:
 *
 *
 *		TCML_COMPLETE:	 The message packet was successfully
 *				 sent.  It does NOT indicate that the
 *				 request was queued successfully.
 *
 *		TCML_INTERNERR:	 Indicates that the contents of the
 *				 message packet, as specified by the
 *				 caller, are too large to fit in an NQS
 *				 message packet, and can therefore not
 *				 be sent to the NQS daemon.
 *
 *		TCML_NOLOCALDAE: Indicates that the message packet could
 *				 not be delivered. Either there is no
 *				 local daemon present, or interconn()
 *				 was never called.
 *
 *		TCML_PROTOFAIL:	 Protocol failure error between the NQS
 *				 daemon, and the local client process.
 *
 *
 *
 *	    For all other packet types, this function may return:
 *
 *
 *		TCML_COMPLETE:	 The message packet was successfully
 *				 sent, and the action specified by the
 *				 message packet was successfully carried
 *				 out by the NQS daemon.
 *
 *		TCML_ALREADACC:	 The message packet specified a gid or uid
 *				 that already had the access asked for.
 *
 *		TCML_ALREADEXI:	 The message packet specified the creation
 *				 of an NQS object that conflicted with
 *				 an existing object.
 *
 *		TCML_DEVACTIVE:	 The message packet specified an operation
 *				 on a device that could not be carried out
 *				 because the device was active.
 *
 *		TCML_DEVENABLE:	 The message packet specified an operation
 *				 on a device that could not be carried out
 *				 because the device was enabled.
 *
 *		TCML_FIXBYNQS:	 The requested queue resource limit was
 *				 outside the legal range for this machine.
 *				 A limit was set that is inside the legal
 *				 range.
 * 
 *		TCML_GRANFATHER: The message packet was successfully
 *				 sent, and a limit or nice value was
 *				 successfully associated with a queue,
 *				 but a request in that queue was
 *				 given a grandfather clause. A grandfather
 *				 clause is a clause in an otherwise
 *				 limiting rule which exempts from the rule
 *				 those who have already begun the prohibited
 *				 action before the rule was established.
 *				 (This comment had to be written by a
 *				 disbarred California attorney.)
 *
 *		TCML_INSHUTDOWN: The message packet was a PKT_SENSEDAEMON
 *				 packet, and the local NQS daemon is in the
 *				 process of shutting down, though it has
 *				 not completed doing so.
 *
 *		TCML_INSUFFMEM:	 The message packet specified the creation
 *				 of an NQS object, but there was not
 *				 sufficient heap space to create the
 *				 object.
 *
 *		TCML_INSUFFPRV:	 The message packet specified an action
 *				 for which the client did not have the
 *				 privilege to request such an action.
 *
 *		TCML_INTERNERR:	 Indicates that the contents of the
 *				 message packet, as specified by the
 *				 caller, are too large to fit in an NQS
 *				 message packet, and can therefore not
 *				 be sent to the NQS daemon.
 *
 *		TCML_LOGFILERR:	 The message packet specified a new log
 *				 file that could not be opened or created
 *				 by the NQS daemon.
 *
 *		TCML_MAXQDESTS:	 The message specified the addition of
 *				 a new destination to the destination set
 *				 of a pipe queue, which would exceed the
 *				 maximum allowable cardinality of such
 *				 a set.
 *
 *		TCML_NOACCNOW:	 The message packet specified a gid or uid
 *				 to be deleted from an access set, and the
 *				 uid or gid was not present in the access
 *				 set.
 *
 *		TCML_NOESTABLSH: Unable to establish the inter-process
 *				 communication file to read completion
 *				 codes from the local NQS daemon.
 *
 *		TCML_NOLOCALDAE: Indicates that the message packet could
 *				 not be delivered. Either there is no
 *				 local daemon present, or interconn()
 *				 was never called.
 *
 *		TCML_NOSUCHACC:	 The message referred to an account that
 *				 does not exist.
 *
 *		TCML_NOSUCHDES:	 The message packet specified an operation
 *				 on the destination of a pipe queue, when
 *				 the specified destination did not exist.
 *
 *		TCML_NOSUCHDEV:	 The message packet specified an operation
 *				 on a device, when the specified device
 *				 did not exist.
 *
 *		TCML_NOSUCHFOR:	 The message packet specified an operation
 *				 on a formname that did not exist.
 *
 *		TCML_NOSUCHGRP:	 The message referred to a group that
 *				 does not exist.
 *
 *		TCML_NOSUCHMAN:	 The message packet specified an operation
 *				 on a manager account, and the manager
 *				 account did not exist.
 *
 *		TCML_NOSUCHMAP:	 The message packet specified an operation
 *				 on a queue/device mapping where the mapping
 *				 did not exist.
 *
 *		TCML_NOSUCHQUE:	 The message packet specified an operation
 *				 on a queue, where the specified queue did
 *				 not exist.
 *
 *		TCML_NOSUCHQUO:	 The message packet specified the setting
 *				 of a quota limit not supported on the
 *				 destination UNIX system.
 *
 *		TCML_NOSUCHREQ:	 The message packet specified an operation
 *				 upon a specific req, and the req was not
 *				 found in any of the NQS queues.
 *
 *		TCML_NOSUCHSIG:	 The message packet specified that a specific
 *				 req be signalled with a signal that is not
 *				 recognized by the local execution machine.
 *
 *		TCML_NOTREQOWN:	 The message packet specified an operation
 *				 on a req, where the client requester is
 *				 not the owner of the req, when client
 *				 ownership is required.
 *
 *		TCML_PLOCKFAIL:	 The message packet specified the locking
 *				 or unlocking of the NQS daemon in memory,
 *				 and the NQS daemon call to plock() failed
 *				 for some reason.
 *
 *		TCML_PROTOFAIL:	 Protocol failure error between the NQS
 *				 daemon, and the local client process.
 *
 *		TCML_QUEENABLE:	 The message packet specified an operation
 *				 on a queue that could not be carried out
 *				 because the queue was enabled.
 *
 *		TCML_QUEHASREQ:	 The message packet specified an operation
 *				 on a queue that could not be carried out
 *				 because the queue had at least one req
 *				 in it.
 *
 *		TCML_REQDELETE:	 The message packet specified the deletion
 *				 of a queued req, and the req was success-
 *				 fully deleted.
 *
 *		TCML_REQRUNNING: The message packet referred to a req which
 *				 is running.
 *
 *		TCML_REQSIGNAL:	 The message packet specified that a signal
 *				 be sent to the specified req (if running),
 *				 and all processes in the req were then
 *				 signalled.
 *
 *		TCML_ROOTINDEL:	 The message packet tried to delete root
 *				 from a queue access list or the list
 *				 of NQS managers.
 *
 *		TCML_SHUTERROR:	 The message packet specified that the NQS
 *				 daemon should shut down, but the NQS daemon
 *				 was unable to do so, because of some horri-
 *				 fying error condition.
 *
 *		TCML_TOOMANDEV:	 The message packet specified that a new
 *				 device be created, but the number of devices
 *				 already in existence was at the maximum
 *				 number allowed.
 *
 *		TCML_UNRESTR:	 The message packet tried to add or
 *				 delete gids or uids to/from the access
 *				 list of a queue with unrestricted access.
 *
 *		TCML_WROQUETYP:	 The message packet specified an operation
 *				 upon a queue that could not be carried out
 *				 because the queue was of the wrong type
 *				 for the operation.
 *
 */
long inter (packet_type)
int packet_type;			/* Packet type to send */
{
	void alarmsig();		/* Catch SIGALRM */
	void brokenpipe();		/* SIGPIPE catcher function */
#if OSF
	void (*sigalrm_sav)();		/* Save old SIGALRM handler */
	void (*sigpipe_sav)();		/* Save old SIGPIPE handler */
#else
	int (*sigalrm_sav)();		/* Save old SIGALRM handler */
	int (*sigpipe_sav)();		/* Save old SIGPIPE handler */
#endif
	unsigned origalarm;		/* Original alarm time */
	int packet_size;		/* Size of packet in bytes */
	struct stat statbuf;		/* Stat() return structure */
	int pid;			/* Process-id of caller */
	char path [MAX_PATHNAME+1];	/* Name of the inter-process */
					/* communication file for */
					/* the calling process. */
	char packet [MAX_PACKET];	/* The message packet */


	if (fifopipe == -1) {		/* If no connection, then... */
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
		/*
		 *  Establish the connection to the local NQS daemon.
		 */
		if (packet_type == PKT_QUEREQ) {
			/*
			 *  We are being called to send a queue request
			 *  message packet, and the caller therefore has
			 *  a current working directory of the NQS new
			 *  request directory.
			 */
			fifopipe = open (Nqs_fifo, O_NDELAY | O_WRONLY |
					 O_APPEND);
		}
		else {
			/*
			 *  We are being called to send some packet
			 *  other than a queue request packet.  In this
			 *  case, the caller MUST be running with a
			 *  current working directory of the NQS root
			 *  directory.
			 */
			fifopipe = open (Nqs_ffifo, O_NDELAY | O_WRONLY |
					 O_APPEND);
		}
		if (fifopipe == -1) {	/* The open call failed */
			/*
			 *  The "open" will fail if no daemon exists to
			 *  read the pipe, thus if we get here, then the
			 *  local NQS daemon is not running OR is trying
			 *  to shutdown with group and world write-access
			 *  on the pipe disabled.
			 */
			return (TCML_NOLOCALDAE);
					/* We were unable to send the */
		}			/* request packet */
		if (packet_type != PKT_SENSEDAEMON) {
			/*
			 *  Turn off the O_NDELAY bit so that writes when the
			 *  pipe:
			 *
			 *	fifopipe
			 *
			 *  is full, will block, until there is room to send
			 *  the message packet.
			 */
			if (fcntl (fifopipe, F_SETFL, O_APPEND) == -1) {
				/*
				 *  The "fcntl" call failed.
				 */
				exiting();
				return (TCML_NOLOCALDAE);
					/* We were unable to send the */
					/* request packet */
			}
		}
#else
#if	BSD42 | BSD43
		/*
		 * Berkeley clients who wish to communicate with the local
		 * daemon must call interconn() first.  
		 */
		return (TCML_NOLOCALDAE);
#else
BAD SYSTEM TYPE
#endif
#endif
	}
	if (packet_type != PKT_SENSEDAEMON) {
		pid = getpid();		/* Get caller's process-id */
		if (interproc == -1) {	/* Inter-proc comm file exist? */
			/*
			 *  No inter-process communications file exists.
			 */
			if (packet_type != PKT_FAMILY &&
			    packet_type != PKT_REQCOM &&
			    packet_type != PKT_QUEREQ) {
				/*
				 *  We must also establish the inter-process
				 *  communication file for this process used
				 *  to receive completion codes from the local
				 *  NQS daemon.
				 */
				sprintf (path, "%s/%1d", Nqs_inter, pid);
				if ((interproc = creat (path, 0600)) == -1) {
					/*
					 *  We were unable to create the inter-
					 *  process communications file used to
					 *  RECEIVE completion codes from the
					 *  NQS daemon.
					 */
					exiting();
					return (TCML_NOESTABLSH);
				}
			}
		}
	}
#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
	fstat (fifopipe, &statbuf);
	if ((statbuf.st_mode & 0000022) == 0) {
		/*
		 *  If write access is denied to group and others on
		 *  the fifo, then the NQS daemon is trying to shutdown.
		 *
		 *  The only packets now allowed through are PKT_REQCOM,
		 *  and PKT_FAMILY packets.  All other packet types are
		 *  NOT sent with the result that the pipe is closed, and
		 *  a status value of TCML_NOLOCALDAE is returned.
		 *
		 *  PKT_REQCOM packets are allowed through, and then the
		 *  pipe is closed, returning a status value of success
		 *  (TCML_COMPLETE).
		 *
		 *  PKT_FAMILY packets are allowed through, and the
		 *  pipe remains open, returning a status value of success
		 *  (TCML_COMPLETE).
		 *
		 *  It should be noted that the NQS daemon cannot shutdown
		 *  until ALL processes with the FIFO pipe open for writing
		 *  CLOSE the pipe.
		 *
		 *  We do not reject PKT_REQCOMs because we desperately
		 *  want request completion packets to reach the daemon.
		 *
		 *  We do not reject PKT_FAMILY packets because we need
		 *  to know which process family to kill, if a fast shutdown
		 *  is required.
		 */
		if (packet_type == PKT_SENSEDAEMON) {
			/*
			 *  We simply wanted to determine if the NQS daemon
			 *  was running on the local system.
			 */
			exiting();		/* Giveup all connections */
			return(TCML_INSHUTDOWN);/* Daemon is in shutdown */
						/* mode, but has not */
						/* finished shutting down */
						/* just yet */
		}
		if (packet_type != PKT_REQCOM && packet_type != PKT_FAMILY) {
			/*
			 *  Only PKT_REQCOM, and PKT_FAMILY packets
			 *  are allowed through at this point.
			 */
			exiting();
			return (TCML_NOLOCALDAE);
					/* Unable to send the packet */
		}
	}
	if (packet_type == PKT_SENSEDAEMON) {
		/*
		 *  We simply wanted to determine if the NQS daemon
		 *  was running on the local system.
		 */
		exiting();		/* Giveup all connections */
		return (TCML_COMPLETE);	/* Daemon is running */
	}
#else
#if	BSD42 | BSD43
#else
BAD SYSTEM TYPE
#endif
#endif
	/*
	 *  Add the packet type, process-id, and real user-id to the packet.
	 */
	interw32i ((long) packet_type);
	interw32i ((long) pid);
	interw32i ((long) getuid());
	if ((packet_size = interfmt (packet)) == -1) {
		/*
		 *  The packet contents create a packet that is too
		 *  large.  The packet cannot be sent.
		 */
		exiting();
		return (TCML_INTERNERR);	/* Packet too large */
	}
	sigpipe_sav = signal (SIGPIPE, brokenpipe);
					/* Save caller's SIGPIPE handler */
					/* and instate our own */
	if (packet_type != PKT_FAMILY &&
	    packet_type != PKT_REQCOM) {
		/*
		 *  The communication protocol used for packets other
		 *  than PKT_FAMILY and PKT_REQCOM requires that a
		 *  SIGALRM signal be received from the NQS daemon.
		 *  Thus, SIGALRM signals MUST be set to be caught,
		 *  prior to sending the message packet to the NQS daemon.
		 */
		origalarm = alarm (0);	/* Get original alarm setting */
					/* disabling any pending alarm. */
		sigalrm_sav = signal (SIGALRM, alarmsig);
					/* Get/Set SIGALRM handler */
		sem_timeout = 0;	/* Clear the alarm time-out flag */
	}
	if (write (fifopipe, packet, packet_size) != packet_size) {
		/*
		 *  An error occurred trying to send the message packet
		 *  to the daemon.
		 */
		exiting();
		return(TCML_NOLOCALDAE);/* Message packet not sent */
	}
	signal (SIGPIPE, sigpipe_sav);	/* Restore SIGPIPE handler */
	if (packet_type == PKT_FAMILY) {
		/*
		 *  PKT_FAMILY packets do not need to wait for
		 *  any SIGALRM synchronization.
		 */
		return (TCML_COMPLETE);	/* Successful completion */
	}
	else if (packet_type == PKT_REQCOM) {
		/*
		 *  A request completion message was sent;  close
		 *  the fifo pipe so that the daemon can shutdown
		 *  (if a shutdown operation is in progress).
		 *  This does not cause a problem here because
		 *  the only process that sends PKT_REQCOMS, is
		 *  a related forked version of NQS, which does
		 *  not have, or need an inter-process communication
		 *  file for receiving completion codes from the
		 *  local NQS daemon.
		 */
		close (fifopipe);	/* Close pipe to daemon. */
		fifopipe = -1;		/* Mark pipe as closed */
		return (TCML_COMPLETE);	/* Successful completion */
	}
	/*
	 *  Wait for the acknowledge signal from the local NQS daemon.
	 *  Note that we set a timeout here to catch a crashed NQS
	 *  daemon....
	 *
	 *  Note that SIGALRM signals are caught by the statements
	 *  executed prior to the write() statement above....
	 */
	alarm (TIMEOUT);		/* Set a timeout */
					/* to catch a crashed daemon */
	/*
	 *  Wait for timeout or acknowledge from the NQS daemon.
	 */
	while (sem_timeout == 0) {
#if  BSD43 | ULTRIX
		sigpause(0);
#else
		pause();
#endif
	}
	signal (SIGALRM, sigalrm_sav);	/* Restore SIGALRM state */
	if (alarm (origalarm)) {	/* Restore the original alarm */
		/*
		 *  An acknowledge signal was received from the NQS
		 *  daemon.
		 */
		if (packet_type == PKT_QUEREQ) {
			/*
			 *  For PKT_QUEREQs, the control file for
			 *  the request is used to hold the failed
			 *  completion code, or sequence number
			 *  for the request.  Let quereq() in mkreq.c
			 *  sort this out.
			 */
			return (TCML_COMPLETE);
		}
		/*
		 *  It is now necessary to fstat() the inter-process
		 *  communication file to get the completion code
		 *  sent from the NQS daemon.
		 *
		 *  See ../src/nqs_complt() for more information.
		 */
		fstat (interproc, &statbuf);
		if (packet_type == PKT_SHUTDOWN &&
		    statbuf.st_atime == TCML_COMPLETE) {
			/*
			 *  A shutdown message packet was just sent;
			 *  close the fifo pipe so that the daemon can
			 *  shutdown (if a shutdown operation is in
			 *  progress).  We do NOT however delete or
			 *  close the interprocess communications file
			 *  used to RECEIVE completion codes from the
			 *  local NQS daemon, because we need to know
			 *  if the shutdown was successful.
			 */
			close (fifopipe);	/* Close pipe to daemon. */
			fifopipe = -1;		/* Mark pipe as closed */
		}
		return ((long) statbuf.st_mtime);
					/* The modification time of the */
					/* file has the completion */
					/* code; heinous, but effective */
	}
	return (TCML_NOLOCALDAE);	/* Daemon not running or crashed */
}



#if	SGI | SYS52 | ULTRIX | UNICOS | UTS | OSF
#else
#if	BSD42 | BSD43
/*** interconn
 *
 *
 *	int interconn ():
 *
 *	Return the file descriptor of a fifo pipe to the local daemon.
 *	We will be doing the writing, and the local daemon the reading.
 *
 *	Interconn() works by contacting the netdaemon via the
 *	AF_INET domain, and asking the netdaemon to fork a
 *	child who will relay information to the local daemon.
 *
 *	We bind to a reserved port, hence our uid must be == root.
 *
 *	Returns:
 *		>= 0: if successful; the value returned is the
 *		      file descriptor associated with the pipe.
 *
 *		-1:   if unsuccessful, and retry doesn't make sense.
 *
 *		-2:   if unsuccessful, and retry makes sense.
 */
int interconn ()
{
	void brokenpipe ();
	
	char hostname [MAX_MACHINENAME + 1];	/* For gethostname() */
	struct sockaddr_in rsin;	/* Remote Internet socket address */
	struct sockaddr_in lsin;	/* Local Internet socket address */
	struct servent *servent;	/* NQS service entry */
	struct hostent *hostent;	/* Host entry for target */
	int socket_descr;		/* Socket descriptor */
	char packet [MAX_PACKET];	/* Network message packet */
	int packetsize;			/* Size of network packet */
	short tryport;			/* Try to bind to this port */
	short retry;			/* Boolean retry flag */
	int timeout;			/* Seconds before next connect() */
	int (*sigpipe_sav)();		/* Save old SIGPIPE handler */

	servent = getservbyname ("nqs", (char *) 0);
	if (servent == (struct servent *) 0) return (-1);
	/*
	 *  Determine an address at which we can contact ourselves.
	 */
	if (gethostname (hostname, MAX_MACHINENAME + 1) != 0) {
		return (-1);
	}
	hostent = gethostbyname ("localhost");
	if (hostent == (struct hostent *) 0) {
		return (-1);
	}
	/*
	 * Clobber whatever SIGPIPE handler our caller has,
	 * and put in one that always returns errno == EPIPE.
	 */
	sigpipe_sav = signal (SIGPIPE, brokenpipe);
	interclear ();
	interw32i (NPK_SERVERCONN);
	/*
	 *  Now that we have defined the network transaction packet
	 *  contents, format the packet.
	 */
	if ((packetsize = interfmt (packet)) == -1) {
		/*
		 *  The packet is too large.
		 */
		return (-1);
	}
	/*
	 *  Store in the remote socket address structure the address
	 *  at which the netdaemon is listening.
	 */
	bzero ((char *) &rsin, sizeof (struct sockaddr_in));
	bytecopy ((char *) &rsin.sin_addr.s_addr, (char *) hostent->h_addr,
		hostent->h_length);
	rsin.sin_family = hostent->h_addrtype;
	rsin.sin_port = servent->s_port;
	/*
	 *  Store the address of the local socket we will be using
	 *  in the local socket address structure.
	 */
	tryport = IPPORT_RESERVED -1;
	timeout = 1;
	bzero ((char *)&lsin, sizeof (struct sockaddr_in));
	lsin.sin_family = AF_INET;
	lsin.sin_addr.s_addr = INADDR_ANY;
	/*
	 *  Try to connect to the netdaemon.
	 *  If it is likely that an immediate retry would succeed, go
	 *  ahead and do the retry right now, rather than returning -2.
	 */
	do {
		retry = 0;		/* Adopt an optimistic attitude */
		socket_descr = socket (AF_INET, SOCK_STREAM, 0);
		if (socket_descr == -1) {
			if (errno == ENOBUFS) return (-2);
			else return (-1);
		}
		/*
		 *  Try to bind this socket to a reserved port.
		 *  Without a reserved port, the netdaemon would
		 *  suspect that we were trying to pull a fast one.
		 */
		lsin.sin_port = htons (tryport);
		while (bind (socket_descr, &lsin, sizeof (lsin)) < 0) {
			/*
			 *  The bind() failed.  Try another port.
			 */
			if (errno != EADDRINUSE && errno != EADDRNOTAVAIL) {
				return (-1);
			}
			tryport--;
			if (tryport <= IPPORT_RESERVED/2) {
				return (-2);
			}
			lsin.sin_port = htons(tryport);
		}
		/*
		 *  Connect to the netdaemon.
		 */
		if (connect (socket_descr, (char *) &rsin,
			     sizeof (rsin)) == -1) {
			/*
			 *  We were unable to connect to the netdaemon.
			 */
			switch (errno) {
			case EADDRINUSE:
				close (socket_descr);
				tryport--;
				retry = 1;	/* Retry */
				break;
			case ECONNREFUSED:
				if (timeout <= 16) {
					close (socket_descr);
					sleep (timeout);
					timeout *= 2;
					retry = 1;	/* Retry */
				}
				else {
					return (-2);	/* Retry, but not now */
				}
				break;
			case ETIMEDOUT:
				return (-2);		/* Retry, but not now */
			default:
				return (-1);
			}
		}
	} while (retry);
	if (write (socket_descr, packet, packetsize) != packetsize) {
		/*
		 * The netdaemon was probably unable to fork.
		 */
		return (-2);
	}
	signal (SIGPIPE, sigpipe_sav);		/* Restore SIGPIPE handler */
	fifopipe = socket_descr;
	return (socket_descr);
}
#else
BAD SYSTEM TYPE
#endif
#endif

/*** alarmsig
 *
 *
 *	void alarmsig():
 *
 *	This function serves as the signal catcher for SIGARLM signals
 *	occuring as the result of an alarm () call in the lock ()
 *	function above.
 *
 *	This function SETS the "sem_timeout" flag.
 */
static void alarmsig ()
{
	signal (SIGALRM, alarmsig);
	sem_timeout++;			/* Timeout occurred */
}


/*** brokenpipe
 *
 *
 *	void brokenpipe ():
 *
 *	This function serves as the signal catcher for SIGPIPE signals
 *	sent in case the write to the NQS daemon fails because the NQS
 *	daemon is not running any more.
 *
 *	This function sets errno = EPIPE
 */
static void brokenpipe ()
{
	signal (SIGPIPE, brokenpipe);
	errno = EPIPE;
}
