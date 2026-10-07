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
 
/*
 * ++ movreq.c - Network Queueing System
 * 
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/movreq.c,v $
 * 
 * DESCRIPTION:
 * 
 * Send a move request packet to the NQS daemon.
 * 
 * 
 * Author: ------- Brent A. Kingsbury, Sterling Software Incorporated. December
 * 2, 1985.
 * 
 * 
 * STANDARDS VIOLATIONS: None.
 * 
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:18 $ $State: Exp
 * $) $Log: movreq.c,v $
 * Revision 1.4  1994/11/19  02:27:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  01:24:10  mwan
 * 1.2 mods
 * Revision 1.2  1992/10/09  20:17:26  mwan T6 freeze
 * 
 * Revision 1.1  1992/09/24  16:49:22  rkl Initial revision
 * 
 * Revision 3.2  91/02/11  16:55:17  root Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:51:50  hender Sterling version 4/22/87
 * 
 * Updated according to Intergraph curtisoy of Bill Mar 12/05/89  TAC
 * 
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)movreq.c	1.2 (movreq.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nqspacket.h"		/* NQS local message packets */
#if	BSD43 | SGI | UTS | OSF
#include <sys/types.h>		/* Uid_t, gid_t */
#else
#if	BSD42 | SYS52 | ULTRIX | UNICOS
#else
BAD SYSTEM      TYPE
#endif
#endif
#include NMAP			/* Mid_t (all OS's) */
/* Uid_t and gid_t (if not BSD43) */

extern long     inter();	/* Send local packet */
extern void     interclear();	/* Clear packet */
extern void     interw32i();	/* Write packet integer datum */
extern void     interwstr();	/* Write packet string datum */

/*** movreqSDSC
 *
 *
 *	long movreqSDSC():
 *	Send a move request packet to the NQS daemon.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if the specified request was
 *				 successfully placed in the specified
 *				 queue.
 *		TCML_INTERNERR:	 if maximum packet size exceeded.
 *		TCML_NOESTABLSH: if unable to establish inter-
 *				 process communication.
 *		TCML_NOLOCALDAE: if the NQS daemon is not running.
 *		TCML_NOSUCHQUE:	 if the specified from or destination
 *				 queue does not exist.
 *		TCML_NOSUCHREQ:	 if the specified request does not
 *				 exist.
 *		TCML_NOTREQOWN:	 if the specified request is not
 *				 owned by the client.
 *		TCML_PROTOFAIL:	 if a protocol failure occurred.
 */
long 
movreqSDSC(orig_seqno, orig_mid, dest_queue_name)
   long            orig_seqno;	/* Original sequence# */
   mid_t           orig_mid;	/* Original machine-id of req */
   char           *dest_queue_name;	/* Name of destination queue for */
{				/* the request */
   interclear();
   interw32i((long) orig_seqno);
   interw32i((long) orig_mid);
   interwstr(dest_queue_name);
   return (inter(PKT_MOVREQ));
}
