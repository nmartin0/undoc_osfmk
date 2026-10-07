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
 
/*++ quereqvlpq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/quereqvlpq.c,v $
 *
 * DESCRIPTION:
 *
 *	Send a "queue request in a local queue via a local
 *	pipe queue" packet to the NQS daemon.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	December 18, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:33 $ $State: Exp $)
 * $Log: quereqvlpq.c,v $
 * Revision 1.3  1994/11/19  02:27:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:18:17  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:36  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:53:33  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include NMAP				/* uid_t and mid_t */
#include "nqspacket.h"			/* NQS local message packets */

extern long inter();			/* Send local packet */
extern void interclear();		/* Clear packet */
extern void interw32i();		/* Write packet integer datum */
extern void interwstr();		/* Write packet string datum */

/*** quereqvlpq
 *
 *
 *	long quereqvlpq():
 *
 *	Send a "queue request in a local queue via a local
 *	pipe queue" packet to the NQS daemon.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if the specified request was
 *				 successfully queued in the local
 *				 destination queue.
 *		TCML_INTERNERR:	 if maximum packet size exceeded.
 *		TCML_NOESTABLSH: if unable to establish inter-
 *				 process communication.
 *		TCML_NOLOCALDAE: if the NQS daemon is not running.
 *		TCML_NOSUCHQUE:	 if the specified destination
 *				 queue does not exist.
 *		TCML_NOSUCHREQ:	 if the specified request does not
 *				 exist.
 *		TCML_PROTOFAIL:	 if a protocol failure occurred.
 */
long quereqvlpq (orig_seqno, orig_mid, destqueue)
long orig_seqno;			/* Original sequence# */
mid_t orig_mid;				/* Original machine-id of req */
char *destqueue;			/* Name of local queue where */
{					/* request should be queued */
	interclear();
	interw32i ((long) orig_seqno);
	interw32i ((long) orig_mid);
	interwstr (destqueue);
	return (inter (PKT_QUEREQVLPQ));
}
