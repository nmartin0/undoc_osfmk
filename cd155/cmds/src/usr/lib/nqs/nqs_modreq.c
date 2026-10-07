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
 
/*++ nqs_modreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_modreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Modify a queued NQS request.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 8, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:53:02 $ $State: Exp
$)
 * $Log: nqs_modreq.c,v $
 * Revision 1.4  1994/11/19  02:53:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/12/16  23:26:17  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:25:46  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:15  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:07:02  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_modreq.c	1.2 (nqs_modreq.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#if	UNICOS
#include <fcntl.h>
#include <sys/category.h>
#include <sys/resource.h>
#else
#if	SGI | SYS52 | UTS | OSF
#include <fcntl.h>
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/file.h>
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#include "nqs.h"			/* NQS constants and data types */
#include "transactcc.h"			/* Transaction completion codes */
#include "nqsxvars.h"			/* Global vars */

extern int errno;
extern struct request *nqs_fndreq();	/* Find request by sequence# & mid */
extern void pack6name();		/* Compute name on 6-bit alphabet */
extern int readreq();			/* Read the request header portion */
					/* of a control file for an NQS req */
extern int writereq();			/* Write/rewrite request header */
extern long limit();

/*** nqs_modreq
 *
 *
 *	long nqs_modreq():
 *	Modify a queued NQS request.
 *
 *	Returns:
 *		TCML_COMPLETE:	 if the request was successfully
 *				 modified.
 *		TCML_NOSUCHREQ:	 if the specified request does not
 *				 exist on this machine.
 *		TCML_NOTREQOWN:	 if the mapped user-id does not
 *				 match the current mapped user-id
 *				 of the request owner.
 *		TCML_REQRUNNING: if the specified request is running.
 */
long nqs_modreq (mapped_uid, orig_seqno, orig_mid, modtype, param1, param2, inf)
uid_t mapped_uid;			/* Mapped owner user-id */
long orig_seqno;			/* Original request sequence# */
mid_t orig_mid;				/* Original machine-id of req */
long modtype;				/* Type of modification */
long param1;				/* First modification parameter */
long param2;				/* Second modification parameter */
int inf;				/* Set to infinite flag */
{
	struct request *predecessor;	/* Predecessor in req set in queue */
	int state;			/* Request queue state: RQS_ */
	struct request *req;		/* Ptr to request structure for */
					/* located req, if found */
	char path[MAX_PATHNAME+1];	/* pathname of control file */
	int cfd;			/* file descriptor for control file */
	struct rawreq rawreq;		/* raw request structure */
	long	jid;			/* UNICOS job ID */
	long	lim;			/* New limit */

	/*
	 *  Locate the request.
	 */
	state = RQS_STAGING | RQS_QUEUED | RQS_WAITING | RQS_HOLDING
		| RQS_ARRIVING | RQS_RUNNING;

	predecessor = (struct request *) 0;
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor,
			       &state)) == (struct request *) 0) {
		/*
		 *  The request was not found in any of the local queues.
		 */
		return (TCML_NOSUCHREQ);
	}
	if (mapped_uid && req->v1.req.uid != mapped_uid) {
		/*
		 *  This request cannot be affected by the client.
		 */
		return (TCML_NOTREQOWN);
	}
#if	UNICOS
#else
#if	SGI | SYS52 | UTS | BSD42 | BSD43 | ULTRIX | OSF
	if (state == RQS_RUNNING) {
		return (TCML_REQRUNNING);
	}
#else
BAD SYSTEM TYPE
#endif
#endif

	/*
	 *  Make the requested modification.
	 */
	pack6name (path, Nqs_control,
		  (int) (orig_seqno % MAX_CTRLSUBDIRS), (char *) 0,
		  (long) orig_seqno, 5, (long) orig_mid, 6, 0, 0);
	if ((cfd = open( path, O_RDWR )) < 0) {
		printf("E$nqs_mov(): Unable to open control file (%s).", path);
		printf("  Errno = %d\n", errno);
		fflush(stdout);
		return (TCML_UNAFAILURE);
	}
	if (readreq(cfd, &rawreq) < 0) {
		close(cfd);
		printf("E$nqs_mov(): Read of request header failed.");
		printf("  Errno = %d\n", errno);
		return (TCML_UNAFAILURE);
	}

	/*
	 *  Set the new limits.
	 */
	rawreq.v.bat.explicit |= modtype;
	if (inf) {				/* if infinite flag set */
                                                        /* CERN Boissat begin */
		rawreq.v.bat.infinite |= modtype;
		switch (modtype) { /* Modify  per-process limits */
		case LIM_PRMEM:
			rawreq.v.bat.infinite |= LIM_PPMEM;
			break;
                case LIM_PPCPUT:
			rawreq.v.bat.infinite |= LIM_PPCPUT;
			break;
		}

                                                       /* CERN Boissat end */
	} else {
		rawreq.v.bat.infinite &= ~modtype; 	/* CERN Boissat */
		switch (modtype) {
		case LIM_PRMEM:
			rawreq.v.bat.prmemsize.max_quota = param1;
			rawreq.v.bat.prmemsize.warn_quota = param1;
			rawreq.v.bat.prmemsize.max_units = param2;
			rawreq.v.bat.prmemsize.warn_units = param2;
			break;
		case LIM_PPNICE:
			lim = rawreq.v.bat.ppnice;	/* get previous value */
			rawreq.v.bat.ppnice = param1;
			break;
		case LIM_PRCPUT:
			rawreq.v.bat.prcputime.max_seconds = param1;
			rawreq.v.bat.prcputime.warn_seconds = param1;
			rawreq.v.bat.prcputime.max_ms = param2;
			rawreq.v.bat.prcputime.warn_ms = param2;
			if (rawreq.v.bat.ppcputime.max_seconds > param1) {
                                                        /* CERN Boissat begin */
/*
       		                rawreq.v.bat.explicit |= LIM_PPCPUT;
               		        rawreq.v.bat.infinite &= ~LIM_PPCPUT;
				rawreq.v.bat.ppcputime.max_seconds = param1;
                                rawreq.v.bat.ppcputime.warn_seconds = param1;
                                rawreq.v.bat.ppcputime.max_ms = param2;
                                rawreq.v.bat.ppcputime.warn_ms = param2;
*/
				rawreq.v.bat.ppcputime.max_seconds = param1;
				rawreq.v.bat.ppcputime.warn_seconds = param1;
				rawreq.v.bat.ppcputime.max_ms = param2;
				rawreq.v.bat.ppcputime.warn_ms = param2;
			}
			break;
		default:
			close(cfd);
			return (TCML_NOSUCHQUO);
		}
	}
	if (writereq(cfd, &rawreq) < 0) {
		printf("E$nqs_mov(): Write of request header failed.");
		printf("  Errno = %d\n", errno);
		close(cfd);
		return (TCML_UNAFAILURE);
	}
	close(cfd);

#if	UNICOS
	if (state != RQS_RUNNING) {
		return (TCML_COMPLETE);
	}
	jid = Runvars [req->reqindex].process_family;
	switch (modtype) {
	case LIM_PRMEM:
		if (inf) {
			lim = 0;
		} else {
			switch (param2) {
			case QLM_BYTES:
				lim = param1 / MEM_LIMIT_GRAN;
				break;
			case QLM_WORDS:
				lim = (param1 * BYTES_PER_WORD) /
					MEM_LIMIT_GRAN;
				break;
			case QLM_KBYTES:
				lim = (param1 * (1<<10)) / MEM_LIMIT_GRAN;
				break;
			case QLM_KWORDS:
				lim = (param1 * BYTES_PER_WORD * (1<<10)) /
					MEM_LIMIT_GRAN;
				break;
			case QLM_MBYTES:
				lim = (param1 * (1<<20)) / MEM_LIMIT_GRAN;
				break;
			case QLM_MWORDS:
				lim = (param1 * BYTES_PER_WORD * (1<<20)) /
					MEM_LIMIT_GRAN;
				break;
			case QLM_GBYTES:
				lim = (param1 * (1<<30)) / MEM_LIMIT_GRAN;
				break;
			case QLM_GWORDS:
				lim = ((param1 * (1<<30)) / MEM_LIMIT_GRAN) *
					BYTES_PER_WORD;
				break;
			}
		}
		limit(C_JOB, jid, L_MEM, lim);
		break;
	case LIM_PPNICE:
		nicem (C_JOB, jid, param1 - lim);
		break;
	case LIM_PRCPUT:
		if (inf) {
			lim = 0;
		} else {
			lim = param1 * CPU_LIMIT_GRAN;
		}
		limit (C_JOB, jid, L_CPU, lim);
		break;
	}
#endif
	return (TCML_COMPLETE);
}
