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
 
/*++ tcmmsgs.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/tcmmsgs.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Diagnose a transaction completion code WITHOUT
 *	diagnosing any TCI_ information bits that might
 *	be present in the transaction code.
 *
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	April 28, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:28:00 $ $State: Exp $)
 * $Log: tcmmsgs.c,v $
 * Revision 1.3  1994/11/19  02:28:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  21:59:56  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:13  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:37  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>		/* FILE definition */
#include "informcc.h"		/* NQS information completion codes */
#include "transactcc.h"		/* NQS transaction completion codes */


/*** tcmmsgs
 *
 *
 *	void tcmmsgs():
 *
 *	Diagnose a transaction completion code WITHOUT
 *	diagnosing any TCI_ information bits that might
 *	be present in the transaction code.
 */
void tcmmsgs (code, stream, prefix)
register long code;			/* Completion code */
register FILE *stream;			/* Output stream */
register char *prefix;			/* Output prefix */
{
	static char retry_later[] = "Retry later.\n";
	static char seek_help[] = "Seek staff support.\n";

	static char *generic_messages [] = {
		"Access denied",
		(char *) 0,		/* TCML_ACCESSDEN */
					/* TCMP_ACCESSDEN */
		"Already has access",
		(char *) 0,		/* TCML_ALREADACC */
		"Already exists",
		(char *) 0,		/* TCML_ALREADEXI */
		"Corrupt request control/data file(s) detected",
		seek_help,		/* TCML_BADCDTFIL */
					/* TCMP_BADCDTFIL */
		"Client machine-id unknown",
		seek_help,		/* TCMP_CLIMIDUNKN */
		"Transaction complete",
		(char *) 0,		/* TCML_COMPLETE */
					/* TCMP_COMPLETE */
		"Network connection lost",
		retry_later,		/* TCMP_CONNBROKEN */
		"Network connection timeout",
		retry_later,		/* TCMP_CONNTIMOUT */
		"Subtransaction complete",
		(char *) 0,		/* TCMP_CONTINUE */
		"CPU already allocated to another queue",
		(char *) 0,		/* TCML_CPUALRESVD */
		"Device is active",
		(char *) 0,		/* TCML_DEVACTIVE */
		"Device is enabled",
		(char *) 0,		/* TCML_DEVENABLE */
		"File access denied",
		(char *) 0,		/* TCML_EACCESS */
					/* TCMP_EACCESS */
		"File size limit exceeded",
		(char *) 0,		/* TCML_EFBIG */
					/* TCMP_EFBIG */
		"Operation on directory is prohibited",
		(char *) 0,		/* TCML_EISDIR */
					/* TCMP_EISDIR */
		"Symbolic link translation limit exceeded",
		(char *) 0,		/* TCML_ELOOP */
					/* TCMP_ELOOP */
		"Insufficient file descriptors",
		retry_later,		/* TCML_ENFILE */
					/* TCMP_ENFILE */
		"Insufficient buffer space",
		retry_later,		/* TCML_ENOBUFS */
					/* TCMP_ENOBUFS */
		"Component in file path does not exist",
		(char *) 0,		/* TCML_ENOENT */
					/* TCMP_ENOENT */
		"Insufficient memory",
		retry_later,		/* TCML_ENOMEM */
					/* TCMP_ENOMEM */
		"File system resource shortage",
		retry_later,		/* TCML_ENOSPC */
					/* TCMP_ENOSPC */
		"Invalid file path",
		(char *) 0,		/* TCML_ENOTDIR */
					/* TCMP_ENOTDIR */
		"Device error",
		retry_later,		/* TCML_ENXIO */
					/* TCMP_ENXIO */
		"Permission denied",
		(char *) 0,		/* TCML_EPERM */
					/* TCMP_EPERM */
		"Fifo error",
		retry_later,		/* TCML_EPIPE */
					/* TCMP_EPIPE */
		"Attempt to modify a readonly file system",
		(char *) 0,		/* TCML_EROFS */
					/* TCMP_EROFS */
		"Recoverable transaction failure",
		retry_later,		/* TCML_ERRORRETRY */
					/* TCMP_ERRORRETRY */
		"Connect(2) timeout",
		retry_later,		/* TCML_ETIMEDOUT */
		"Text file operation denied",
		retry_later,		/* TCML_ETXTBSY */
					/* TCMP_ETXTBSY */
		"Non-recoverable transaction failure",
		(char *) 0,		/* TCML_FATALABORT */
					/* TCMP_FATALABORT */
		"Limit set to enforceable value",
		"other than originally requested.\n",
					/* TCML_FIXBYNQS */
		"Transaction successful",
		"but one or more requests were given a grandfather clause.\n",
					/* TCML_GRANFATHER */
		"NQS is shutting down",
		(char *) 0,		/* TCML_INSHUTDOWN */
		"Insufficient space to queue request",
		retry_later,		/* TCML_INSQUESPA */
					/* TCMP_INSQUESPA */
		"Insufficient memory",
		(char *) 0,		/* TCML_INSUFFMEM */
		"Insufficient privilege",
		(char *) 0,		/* TCML_INSUFFPRV */
		"Internal NQS error",
		seek_help,		/* TCML_INTERNERR */
					/* TCMP_INTERNERR */
		"Cannot open or create new logfile",
		(char *) 0,		/* TCML_LOGFILERR */
		"Maximum network connection limit reached",
		retry_later,		/* TCMP_MAXNETCONN */
		"Maximum destination set cardinality reached",
		(char *) 0,		/* TCML_MAXQDESTS */
		"Machine-id conflict between client and peer",
		seek_help,		/* TCMP_MIDCONFLCT */
		"Local network database error",
		seek_help,		/* TCML_NETDBERR */
					/* TCMP_NETDBERR */
		"Networking not supported by implementation",
		(char *) 0,		/* TCML_NETNOTSUPP */
		"Network password verification failure",
		seek_help,		/* TCMP_NETPASSWD */
		"No account authorization",
		(char *) 0,		/* TCML_NOACCAUTH */
					/* TCMP_NOACCAUTH */
		"Does not have access now",
		(char *) 0,		/* TCML_NOACCNOW */
		"Unable to make connection with NQS daemon",
		retry_later,		/* TCML_NOESTABLSH */
					/* TCMP_NOESTABLSH */
		"NQS local daemon is not present",
		retry_later,		/* TCML_NOLOCALDAE */
					/* TCMP_NOLOCALDAE */
		"Insufficient processes to perform transaction",
		retry_later,		/* TCML_NOMOREPROC */
					/* TCMP_NOMOREPROC */
		"NQS net daemon is not present",
		retry_later,		/* TCMP_NONETDAE */
		"Non-secure network port verification error",
		seek_help,		/* TCMP_NONSECPORT */
		"No network port available",
		retry_later,		/* TCML_NOPORTAVAI */
		"No such account",
		(char *) 0,		/* TCML_NOSUCHACC */
		"No such CPU",
		(char *) 0,		/* TCML_NOSUCHCPU */
		"No such destination",
		(char *) 0,		/* TCML_NOSUCHDES */
		"No such device",
		(char *) 0,		/* TCML_NOSUCHDEV */
		"No such device forms",
		(char *) 0,		/* TCML_NOSUCHFORM */
					/* TCMP_NOSUCHFORM */
		"No such group",
		(char *) 0,		/* TCML_NOSUCHGRP */
		"No such machine",
		(char *) 0,		/* TCML_NOSUCHMAC */
		"No such manager/operator",
		(char *) 0,		/* TCML_NOSUCHMAN */
		"No such mapping",
		(char *) 0,		/* TCML_NOSUCHMAP */
		"No such queue",
		(char *) 0,		/* TCML_NOSUCHQUE */
					/* TCMP_NOSUCHQUE */
		"No such quota supported",
		(char *) 0,		/* TCML_NOSUCHQUO */
		"No such request",
		(char *) 0,		/* TCML_NOSUCHREQ */
					/* TCMP_NOSUCHREQ */
		"No such signal",
		(char *) 0,		/* TCML_NOSUCHSIG */
					/* TCMP_NOSUCHSIG */
		"Not request owner",
		(char *) 0,		/* TCML_NOTREQOWN */
					/* TCMP_NOTREQOWN */
		"Resolved output filename for batch request",
		"exceeds the maximum supported path length.\n",
					/* TCML_PATHLEN */
					/* TCMP_PATHLEN */
		"Request arriving",
		(char *) 0,		/* TCML_PEERARRIVE */
		"Request departing",
		(char *) 0,		/* TCML_PEERDEPART */
		"NQS daemon call to plock() failed",
		(char *) 0,		/* TCML_PLOCKFAIL */
		"NQS protocol failure",
		seek_help,		/* TCML_PROTOFAIL */
					/* TCMP_PROTOFAIL */
		"Queue is disabled",
		retry_later,		/* TCML_QUEDISABL */
					/* TCMP_QUEDISABL */
		"Queue is enabled",
		(char *) 0,		/* TCML_QUEENABLE */
		"Queue has request(s)",
		(char *) 0,		/* TCML_QUEHASREQ */
		"Explicit request quota limits exceed maximums",
		(char *) 0,		/* TCML_QUOTALIMIT */
					/* TCMP_QUOTALIMIT */
		"Attempt to queue already existing request",
		(char *) 0,		/* TCML_REQCOLLIDE */
					/* TCMP_REQCOLLIDE */
		"Request deleted",
		(char *) 0,		/* TCML_REQDELETE */
					/* TCMP_REQDELETE */
		"Request is running",
		(char *) 0,		/* TCML_REQRUNNING */
					/* TCMP_REQRUNNING */
		"Request was running and has been signalled",
		(char *) 0,		/* TCML_REQSIGNAL */
					/* TCMP_REQSIGNAL */
		"Root cannot be deleted",
		(char *) 0,		/* TCML_ROOTINDEL */
		"Request refers to unknown machines",
		(char *) 0,		/* TCMP_RRFUNKNMID */
		"Local machine-id unknown",
		seek_help,		/* TCML_SELMIDUNKN */
					/* TCMP_SELMIDUNKN */
		"Attempt to create self-referential destination",
		(char *) 0,		/* TCML_SELREFDES */
		"Shutdown error",
		(char *) 0,		/* TCML_SHUTERROR */
		"Request successfully submitted",
		(char *) 0,		/* TCML_SUBMITTED */
					/* TCMP_SUBMITTED */
		"Too many devices",
		(char *) 0,		/* TCML_TOOMANDEV */
		"Unanticipated transaction failure",
		(char *) 0,		/* TCML_UNAFAILURE */
					/* TCMP_UNAFAILURE */
		"Undefined transaction code",
		seek_help,		/* TCML_UNDEFINED */
					/* TCMP_UNDEFINED */
		"Access is currently unrestricted",
		(char *) 0,		/* TCML_UNRESTR */
		"Wrong queue type for transaction",
		(char *) 0,		/* TCML_WROQUETYP */
					/* TCMP_WROQUETYP */
		"Queue complex does not exist",
		(char *) 0,		/* TCML_NOSUCHCOM */
		"Too many queues defined for a queue complex",
		(char *) 0,		/* TCML_TOOMANQUE */
		"Too many queue complexes defined for a queue",
		(char *) 0,		/* TCML_TOOMANCOM */
		"Queue is already a member of the queue complex",
		(char *) 0,		/* TCML_ALREADCOM */
		"Queue is not a member of the queue complex",
		(char *) 0		/* TCML_NOTMEMCOM */
	};

	register short reason;		/* Reason bits of completion code */

	reason = (code & (XCI_REASON_MASK & ~XCI_PEER_MASK));
	if (reason > (TCM_MAXTCM & (XCI_REASON_MASK & ~XCI_PEER_MASK)) ||
	   !(code & XCI_TRANSA_MASK)) {
		/*
		 *  The transaction code is invalid.
		 */
		reason = (TCML_UNDEFINED & XCI_REASON_MASK);
	}
	reason *= 2;
	fputs (prefix, stream);
	fputs (generic_messages [reason++], stream);
	if (reason != (TCMP_CONNBROKEN & ~XCI_PEER_MASK) &&
	    reason != (TCMP_CONNTIMOUT & ~XCI_PEER_MASK) &&
	    reason != (TCML_NOSUCHMAC & ~XCI_PEER_MASK) &&
	    reason != (TCML_UNRESTR & ~XCI_PEER_MASK)) {
		/*
		 *  The transaction code diagnostic will also have
		 *  an origin suffix tag added.
		 */
		if (code & XCI_PEER_MASK) {
			/*
			 *  The transaction code originated at the transaction
			 *  peer machine.
			 */
			fputs (" at transaction peer.\n", stream);
		}
		else {
			/*
			 *  The transaction code originated at the local host.
			 */
			fputs (" at local host.\n", stream);
		}
	}
	else fputs (".\n", stream);	/* No origin suffix tag */
	if (generic_messages [reason] != (char *) 0) {
		fputs (prefix, stream);
		fputs (generic_messages [reason++], stream);
	}
	fflush (stream);		/* For logfile */
}
