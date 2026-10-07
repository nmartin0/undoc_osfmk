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
 
/*++ mgr_movset.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qmgr/mgr_movset.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager "move set" execution module.
 *
 *	Author:
 *	------
 *	David G. Evans, Cray Research Inc.
 *	August 11th, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 01:35:41 $ $State: Exp $)
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 01:35:41 $ $State: Exp $)
 * $Log: mgr_movset.c,v $
 * Revision 1.5  1994/11/19  01:35:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/04/06  22:16:58  mwan
 * Update R1_3 to R1_2 WW15
 *
 *  Reviewer: kremenek
 *  Risk: M
 *  Benefit or PTS #: 7738,8087,8346, 8325,8599,8576,8600,8601,8088,8597,8876,8886
 *  Testing:
 *  Module(s): mgr_movset.c qmgr.hlp
 *
 * Revision 1.3  1993/11/02  01:07:57  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  22:30:00  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:28:28  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:30  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:11:48  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)mgr_movset.c	1.4 (mgr_movset.c OSF/1 NQS2.0 GJK) 6/12/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>		
#include "nqs.h"			/* NQS types and definitions */
#include "nqsmgr.h"			/* Token types and error messages */
#include "transactcc.h"			/* Transaction completion codes */

/*
 *	Declaration of transaction modules.
 */
extern long  movreq();			/* Move a request to a new queue */
extern long  modreq();			/* Modify a request */
/*
 *	Miscellaneous funtions
 */
extern char  *deq_set();		/* Dequeue text */
extern mid_t deq_mid();			/* Dequeue machine-id */
extern void  diagnose();		/* Diagnose completion code */
extern int   localmid();		/* Get local machine id */
extern int   scancpulim();		/* Parse a cpu time limit */
extern int   scanquolim();		/* Parse a quota limit */	
extern void  show_failed_prefix();	/* Show TCML_FAILURE prefix */


/*** modreq_set
 *
 *
 *	void modreq_set():
 *
 *	Modfify a request identifier. The set of modifiers is defined
 *	by the scan_reqmodset() function which is used to scan/parse
 *	the qualifier set specification.
 */
void modreq_set(req_seqno, orig_hostid, whomuid)
int req_seqno;				/* Request sequence number */
mid_t orig_hostid;			/* Originating machine identifier */
uid_t whomuid; 		                /* Calling user, else zero for */ 
                                        /*  NQS operator/manager call  */ 

{
	void modreq_problem();		/* Problem modifying a request */
	
	char *param_string;		/* Dequeued parameter string */
	long lim_type;			/* Request limit type - LIM_xxx */
	long param_value1;
	long param_value2;
	short param_infinite;
 
	short mssgflag = 0;		/* BOOLEAN TRUE if a message has /*
					/* been delivered to the user */
	register short exitflag = 0;	/* Exit flag */
	int qual_token;			/* Qualifier token */
	int comcode;			/* Completion code */

	while ((param_string = deq_set()) != NULL && !exitflag) {
		/*
		 *	Get qualifier type
		 */	
		qual_token = (int) deq_qual_type();
		/*
		 *	Analyze parameter value 
		 */
		switch (qual_token) {
		case 1:			/* Nice Value */
			param_value1 = atol (param_string);	
			if (param_value1 < (long) MIN_REQNICE || 
				param_value1 > (long) MAX_REQNICE) {
				errormessage (EM_NICEOUTOFBOU);
				modreq_problem ( req_seqno, orig_hostid);
				/* CERN Boissat break; */
				return;
				}
			param_value2 = 0L;
			param_infinite = 0;
			lim_type = (long) LIM_PPNICE;
			break;
		case 2:			/* CPU time limit */
			if (cput_valid (param_string, &param_value1,
				&param_value2, &param_infinite)
				 == -1) {
				 modreq_problem ( req_seqno, orig_hostid);
                                /* CERN Boissat break; */
				return;
			}
			lim_type = (long) LIM_PRCPUT;
			break;
		case 3:			/* Memory Limit */	
			if (quota_valid (param_string, &param_value1,
				&param_value2, &param_infinite)
				 == -1) {
                                 modreq_problem ( req_seqno, orig_hostid);
                                /* CERN Boissat break; */
                                return;
                        }
			lim_type = (long) LIM_PRMEM;
			break;
		}	
		/*
		 *	Send packet to NQS daemon
		 */
		comcode = (int) (modreq ( whomuid, req_seqno, orig_hostid,
 			lim_type, param_value1, param_value2, param_infinite));
		switch (comcode) {
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			modreq_problem (req_seqno,orig_hostid);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_NOSUCHREQ:
		case TCML_REQRUNNING:
			modreq_problem (req_seqno,orig_hostid);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		default:
			modreq_problem (req_seqno,orig_hostid);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** modreq_problem
 *
 *
 *	void movreq_problem()
 *	Indicate problem modifying a request.
 */
static void modreq_problem (seq_num, orig_hostid)
int seq_num;			/* Sequence number */
mid_t orig_hostid;			/* Request host machine id */
{
	show_failed_prefix();
	printf ("Problem modifying request: %ld.%s.\n",
		seq_num, getmacnam (orig_hostid));	/* CERN Boissat orig_hostid); */
}


/*** movreq_set
 *
 *
 *	void movreq_set():
 *
 *	Move reqset.  The set of reqids  (request ids) to move is
 *      defined by the scan_reqset() function which is used to      
 *	scan/parse a reqid set specification.
 */
void movreq_set (dest_qname)
char *dest_qname;			/* Destination queue name */
{
	void movreq_problem();		/* Problem during move of reqid set */
	
	long comcode;			/* Completion code */
	register short exitflag;	/* Exit flag */
	register short mssgflag;	/* BOOLEAN TRUE if a message has */
					/* been delivered to the user */
	register mid_t orig_hostid;	/* Host machine-id of request */
	long seq_num;			/* Sequence number of request */

	/*
	 *  Loop to move requests to a specified queue
	 */

	mssgflag = 0;
	exitflag = 0;
	while (deq_set() != NULL && !exitflag) {
		/*      
		 * Perform the operation
		 */
		seq_num = deq_uid_gid();		/* Get sequence no. */
		if ((orig_hostid = deq_mid()) == -1)
			 orig_hostid = (mid_t) localmid;	/* Get host id */
		/*
		 *  The request exists, as far as we can tell ...
		 */
#ifdef SDSC
                comcode = movreqSDSC ( seq_num, orig_hostid, dest_qname);
#else
		comcode = movreq ( seq_num, orig_hostid, dest_qname);
                /* it is NOT called and it doesn't work ! */
#endif
		switch (comcode) {
		case TCML_INTERNERR:
		case TCML_NOESTABLSH:
		case TCML_NOLOCALDAE:
		case TCML_PROTOFAIL:
			movreq_problem (seq_num,orig_hostid,dest_qname);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		case TCML_COMPLETE:
			break;
		case TCML_NOSUCHQUE:
		case TCML_NOSUCHREQ:
		case TCML_QUEDISABL:
		case TCML_REQRUNNING:
		case TCML_WROQUETYP:
			movreq_problem (seq_num,orig_hostid,dest_qname);
			diagnose (comcode);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
			break;
		default:
			movreq_problem (seq_num,orig_hostid,dest_qname);
			diagnose (TCML_UNDEFINED);
			mssgflag = 1;
			exitflag = 1;		/* Exit */
		}
	}
	if (!mssgflag) diagnose (TCML_COMPLETE);
}


/*** movreq_problem
 *
 *
 *	void movreq_problem()
 *	Indicate problem moving a request to another queue.
 */
static void movreq_problem (seq_num, orig_hostid, dest_qname)
int seq_num;			/* Sequence number */
mid_t orig_hostid;		/* Request host machine id */
char *dest_qname;		/* Destination queue name */
{
	show_failed_prefix();
	printf ("Problem moving request: %d.%s; to queue %s.\n",
		seq_num, getmacnam (orig_hostid), dest_qname);	/* CERN Boissat orig_hostid, dest_qname); */
}
 

/*** cput_valid
 *
 *
 *	int cput_valid ():
 *
 *	Validate a CPU time limit quota.
 *
 *	Returns:
 *		 0: if scanning was successful.
 *		-1: otherwise.
 */
static int cput_valid (cput_string, cpu_secs, cpu_msecs, infinite)
register char *cput_string;
long *cpu_secs;
short *cpu_msecs;
short *infinite;
{
	struct cpulimit cpulimit;	/* CERN Boissat *cpulimit; */

	*infinite = 0;					/* Finite value */
	if (*cput_string == 'u') {
		*cpu_secs = 0;			/* Give these fields */
		*cpu_msecs = 0;			/* values that will */
						/* pass integrity checks */
						/* checks */
		*infinite = 1;			/* Infinite */
	}
	else {
		switch (scancpulim (cput_string, &cpulimit, 1)) { /* /* CERN Boissat */
		case 0:
			*cpu_secs = cpulimit.max_seconds;	/* CERN Boissat cpulimit->max_seconds; */
			*cpu_msecs = cpulimit.max_ms;		/* CERN Boissat
cpulimit->max_ms; */
			break;
		case -1:
			errormessage (EM_INVCPULIMSYN);
			return (-1);
		case -2:
			errormessage (EM_BADCPULIMVAL);
			return (-1);
		case -3:
			errormessage (EM_CPULIMEXP);
			return (-1);
		}
	}
	return(0);			/* Scanning successful */
}


/*** quota_valid
 *
 *
 *	int quota_valid
 *
 *	Validate a quota limit having nothing to do
 *		 with "nice" or cpu limits.
 *
 *	Returns:
 *		 0: if scanning was successful, and privileges are ok;
 *		-1: otherwise.
 */
static int quota_valid (quota_string, quota_value1, quota_value2, infinite)
register char *quota_string;
long *quota_value1;
long *quota_value2;
short *infinite;
{
	struct quotalimit quotalimit;	/* CERN Boissat *quotalimit; */

	*infinite = 0;			/* Finite */
	if (*quota_string == 'u') {
		*quota_value1 = 0;		/* Give these fields */
		*quota_value2 = 0;		/* values that will */
						/* pass integrity */
						/* checks */
		*infinite = 1;			/* Infinite */
	}
	else {
		switch (scanquolim (quota_string, &quotalimit, 1)) { /* CERN Boissat */
		case 0:
			*quota_value1 = quotalimit.max_quota;	/* CERN Boissat quotalimit->max_quota; */
			*quota_value2 = quotalimit.max_units;	/* CERN Boissat
quotalimit->max_units; */
			break;
		case -1:
			errormessage (EM_INVQUOLIMSYN);
			return (-1);
		case -2:
			errormessage (EM_BADQUOLIMVAL);
			return (-1);
		case -3:
			errormessage (EM_QUOLIMEXP);
			return (-1);
		}
	}
	return (0);			/* Scanning successful */
}				

