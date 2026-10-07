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
 
/*++ delreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/delreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Delete a queued NQS request, or signal a running NQS request.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 3, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:26:29 $ $State: Exp $)
 * $Log: delreq.c,v $
 * Revision 1.4  1994/11/19  02:26:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/19  17:48:28  mwan
 * Before T10 frozen. Fixed several PTS
 *
 * Revision 1.2  1992/10/09  20:15:40  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:23  root
 * Version 2.0 Source
 * 
 * Revision 2.3  87/08/25  16:52:18  hender
 * 1. deleted last reminate of nmap; 2. uid_t and gid_t are now defined
 *    in /usr/netinclude/sys/types.h (4.3)
 * ,
 * 
 * Revision 2.2  87/04/22  14:47:28  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)delreq.c	1.3 (delreq.c OSF/1 NQS2.0 GJK) 6/11/92";
#define SCCS
#endif

#include "nqspacket.h"			/* NQS local message packets */
#if	BSD43 | UTS | SGI | OSF

#if OSF
#include "sys_types_OSF.h"      /* Uid_t, gid_t */ /* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>                  /* Uid_t, gid_t */
#endif

#else
#if	BSD42 | SYS52 | ULTRIX | UNICOS 
#else
BAD SYSTEM TYPE
#endif
#endif
#include NMAP				/* Mid_t (all OS's) */
					/* Uid_t and gid_t (if not BSD43) */
#include "transactcc.h"

extern long inter();			/* Send local packet */
extern void interclear();		/* Clear packet */
extern void interw32i();		/* Write packet integer datum */

/*** delreq
 *
 *
 *	long delreq():
 *	Delete a queued NQS request, or signal a running NQS request.
 *
 *	Returns:
 *		TCML_INTERNERR:	 if maximum packet size exceeded.
 *		TCML_NOESTABLSH: if unable to establish inter-
 *				 process communication.
 *		TCML_NOLOCALDAE: if the NQS daemon is not running.
 *		TCML_NOSUCHREQ:	 if the specified request does not
 *				 exist.
 *		TCML_NOSUCHSIG:	 if the request was running or in
 *				 transit, and the specified signal
 *				 to send is not recognized by
 *				 the execution machine.
 *		TCML_NOTREQOWN:	 if the specified request is not
 *				 owned by the client.
 *		TCML_PROTOFAIL:	 if a protocol failure occurred.
 *		TCML_REQDELETE:	 if the request was queued, and was
 *				 deleted.
 *		TCML_REQRUNNING: if the request is running, and has
 *				 not been signalled.
 *		TCML_REQSIGNAL:	 if the request was running, and
 *				 has been signalled.
 */
long delreq (mapped_uid, orig_seqno, orig_mid, sig, reqquestate)
uid_t mapped_uid;			/* Mapped owner user-id */
long orig_seqno;			/* Original sequence# */
mid_t orig_mid;				/* Original machine-id of req */
int sig;				/* Signal to send to the processes */
					/* in the req, if running.  0 if no*/
					/* signals are to be sent to */
					/* running request processes */
int reqquestate;			/* Request queue state */
{
	interclear();
	interw32i ((long) mapped_uid);
	interw32i ((long) orig_seqno);
	interw32i ((long) orig_mid);
	interw32i ((long) sig);
	interw32i ((long) reqquestate);
	return (inter (PKT_DELREQ));
}


 
/*** diagqdel
 *
 *
 *      diagdel():
 *      Diagnose delreq() completion code.
 */
int diagdel (code, reqid)
long code;                              /* Transaction completion code */
char *reqid;                            /* Character version of request-id */
{
#ifdef SDSC
	int result;
	result = 0;
#endif
/*        printf("%d code \n",code); */
        switch (code) {
        case TCML_INTERNERR:
                printf ("Internal error.\n");
                exiting();              /* Delete communication file */
                exit (-1);
        case TCML_NOESTABLSH:
                printf ("Unable to establish inter-process communications ");
                printf ("with NQS daemon.\n");
                printf ("Seek staff support.\n");
                exiting();              /* Delete communication file */
                exit (-1);
        case TCML_NOLOCALDAE:
                printf ("The NQS daemon is not running.\n");
                printf ("Seek staff support.\n");
                exiting();              /* Delete communication file */
                exit (-1);
        case TCML_NOSUCHREQ:
                printf ("Request %s does not exist.\n", reqid);
#ifdef SDSC
		result = -1;
#endif
                break;
        case TCML_NOSUCHSIG:
                printf ("Request %s is running, and the ", reqid);
                printf ("specified signal is\n");
                printf ("not recognized by the execution machine.\n");
#ifdef SDSC
		result = -1;
#endif
                break;
        case TCML_NOTREQOWN:
                printf ("Not owner of request %s.\n", reqid);
#ifdef SDSC
		result = -1;
#endif
                break;
        case TCML_PEERDEPART:
                printf ("Request %s is presently being routed by a ", reqid);
                 printf ("pipe queue,\n");
                printf ("and this NQS implementation does not support the ");
                printf ("deletion of a\n");
                printf ("request under such conditions.\n");
#ifdef SDSC
		result = -1;
#endif
                break;
        case TCML_PROTOFAIL:
                printf ("Protocol failure in inter-process communications ");
                printf ("with NQS daemon.\n");
                printf ("Seek staff support.\n");
                exiting();              /* Delete communication file */
                exit (-1);
        case TCML_REQDELETE:
                printf ("Request %s has been deleted.\n", reqid);
                break;
        case TCML_REQRUNNING:
                printf ("Request %s is running.\n", reqid);
#ifdef SDSC
		result = -1;
#endif
                break;
        case TCML_REQSIGNAL:
                printf ("Request %s is running, and has ", reqid);
                printf ("been signalled.\n");
                break;
        default:
                printf ("Unexpected completion code from NQS daemon.\n");
                exiting();              /* Delete communication file */
                exit (-1);
        }
#ifdef SDSC
	return (result);
#endif
}


