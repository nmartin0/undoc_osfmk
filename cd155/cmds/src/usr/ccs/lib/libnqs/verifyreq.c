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
 
/*++ verifyreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/verifyreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Verify a rawreq structure.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 28, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:28:04 $ $State: Exp $)
 * $Log: verifyreq.c,v $
 * Revision 1.3  1994/11/19  02:28:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:00:08  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:21  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:57:16  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"

extern int secgrfir();			/* Check for 2nd limit > 1st limit */
extern int verifyhdr();			/* Verify common request header */


/*** verifyreq
 *
 *
 *	int verifyreq():
 *	Verify a rawreq structure.
 *
 *	Returns:
 *		0: if the rawreq structure is valid.
 *	       -1: otherwise.
 */
int verifyreq (rawreq)
struct rawreq *rawreq;			/* Raw request structure */
{
	register int i;			/* Loop index var */

	if (verifyhdr (rawreq) == -1) {
		/*
		 *  There is something wrong in the common part
		 *  of the rawreq structure.
		 */
		return (-1);
	}
	if (rawreq->type == RTYPE_DEVICE) {
		/*
		 *  The request is a device-oriented request.
		 */
		if (rawreq->v.dev.forms [MAX_FORMNAME] != '\0' ||
		    rawreq->v.dev.reserved1 != 0 ||
		    rawreq->v.dev.reserved2 != 0 ||
		    rawreq->v.dev.copies < 0 ||
		    rawreq->v.dev.copies > MAX_COPIES) {
			/*
			 *  Values out of bounds.
			 */
			return (-1);
		}
		/*
		 *  Verify device preference list.
		 */
		for (i = 0; i < MAX_DEVPREF; i++) {
			if (rawreq->v.dev.devprefname [i][MAX_DEVNAME]!='\0') {
				/*
				 *  Device preference name is NOT null
				 *  terminated.
				 */
				return (-1);
			}
		}
	}
	else {
		/*
		 *  The request is not device-oriented.
		 *  Verify quota limits and CPU requirements.
		 *  The following conditional is broken up to
		 *  be kind to little compilers.
		 */
		if (vfyquolim (&rawreq->v.bat.ppcoresize) == -1 ||
		    vfyquolim (&rawreq->v.bat.ppdatasize) == -1 ||
		    vfyquolim (&rawreq->v.bat.pppfilesize) == -1 ||
		    vfyquolim (&rawreq->v.bat.prpfilespace) == -1 ||
		    vfyquolim (&rawreq->v.bat.ppqfilesize) == -1 ||
		    vfyquolim (&rawreq->v.bat.prqfilespace) == -1 ||
		    vfyquolim (&rawreq->v.bat.pptfilesize) == -1 ||
		    vfyquolim (&rawreq->v.bat.prtfilespace) == -1 ||
		    vfyquolim (&rawreq->v.bat.ppmemsize) == -1) {
			/*
			 *  Values out of bounds.
			 */
			return (-1);
		}
		if (vfyquolim (&rawreq->v.bat.prmemsize) == -1 ||
		    vfyquolim (&rawreq->v.bat.ppstacksize) == -1 ||
		    vfycpulim (&rawreq->v.bat.ppcputime) == -1 ||
		    vfycpulim (&rawreq->v.bat.prcputime) == -1 ||
		    vfyquolim (&rawreq->v.bat.ppworkset) == -1 ||
		    rawreq->v.bat.ppnice < MIN_REQNICE ||
		    rawreq->v.bat.ppnice > MAX_REQNICE ||
		    rawreq->v.bat.prdrives < 0 ||
		    rawreq->v.bat.prncpus < 0) {
			/*
			 *  Values out of bounds.
			 */
			return (-1);
		}
		/*
		 *  Verify predecessor list.
		 */
		for (i = 0; i < MAX_PREDECESSOR; i++) {
			if (rawreq->v.bat.predecessors[i][MAX_REQNAME]!='\0') {
				/*
				 *  Predecessor name is NOT null terminated.
				 */
				return (-1);
			}
		}
		/*
		 *  Verify everything else.
		 */
		if (rawreq->v.bat.shell_name [MAX_SHELLNAME] != '\0' ||
		    rawreq->v.bat.stderr_name [MAX_REQPATH] != '\0' ||
		    rawreq->v.bat.stdlog_name [MAX_REQPATH] != '\0' ||
		    rawreq->v.bat.stdout_name [MAX_REQPATH] != '\0' ||
		    rawreq->v.bat.instacount < 0 ||
		    rawreq->v.bat.instacount > MAX_INSTAPERREQ ||
		    rawreq->v.bat.oustacount < 0 ||
		    rawreq->v.bat.oustacount > MAX_OUSTAPERREQ) {
			/*
			 *  Values out of bounds.
			 */
			return (-1);
		}
	}
	return (0);			/* Rawreq checks out */
}


/*** vfyquolim
 *
 *
 *	int vfyquolim():
 *	Verify a quota limit.
 *
 *	Returns:
 *		0: if the specified quota limit was ok.
 *	       -1: if the quota limit was corrupt.
 */
static int vfyquolim (quota)
struct quotalimit *quota;
{
	switch (quota->max_units) {
	case QLM_BYTES:
	case QLM_WORDS:
	case QLM_KBYTES:
	case QLM_KWORDS:
	case QLM_MBYTES:
	case QLM_MWORDS:
	case QLM_GBYTES:
	case QLM_GWORDS:
		switch (quota->warn_units) {
		case QLM_BYTES:
		case QLM_WORDS:
		case QLM_KBYTES:
		case QLM_KWORDS:
		case QLM_MBYTES:
		case QLM_MWORDS:
		case QLM_GBYTES:
		case QLM_GWORDS:
			if (quota->max_quota <= 2147483647 &&
			    quota->warn_quota <= 2147483647) {
				/*
				 *  Quota limit coefficients are limited
				 *  to values in the range [0..2^31-1].
				 */
				if (!secgrfir (quota->max_quota,
					       quota->max_units,
					       quota->warn_quota,
					       quota->warn_units)) {
					/*
					 *  The warn limit values must
					 *  be <= the maximum limit value.
					 */
					return (0);	/* Valid limit */
				}
			}
		}
	}
	return (-1);			/* Bad quota limit */
}


/*** vfycpulim
 *
 *
 *	int vfycpulim():
 *	Verify a CPU time limit.
 *
 *	Returns:
 *		0: if the specified CPU time limit was ok.
 *	       -1: if the CPU time limit was corrupt.
 */
static int vfycpulim (cpulimit)
struct cpulimit *cpulimit;
{
	if (cpulimit->max_ms >= 0 &&
	    cpulimit->max_ms <= 999 &&
	    cpulimit->warn_ms >= 0 &&
	    cpulimit->warn_ms <= 999 &&
	   (cpulimit->max_seconds > cpulimit->warn_seconds ||
	   (cpulimit->max_seconds == cpulimit->warn_seconds &&
	    cpulimit->max_ms >= cpulimit->warn_ms))) {
		/*
		 *  Valid CPU time limit.
		 */
		return (0);
	}
	return (-1);			/* Bad CPU limit */
}
