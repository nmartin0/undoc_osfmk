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
 
/*++ nqs_enf.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_enf.c,v $
 *
 * DESCRIPTION:
 *
 *	This module enforces batch request resource limits.
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	February 17, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:52:54 $ $State: Exp
$)
 * $Log: nqs_enf.c,v $
 * Revision 1.4  1994/11/19  02:52:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/12/16  23:26:13  mwan
 * T6 update 2
 *
 * Revision 1.2  1992/10/09  22:25:22  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:02  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:05:44  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_enf.c	1.2 (nqs_enf.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <errno.h>
#include "nqs.h"			/* NQS constants and data types */
#include "informcc.h"			/* NQS information completion */
					/* codes and masks */
#include "requestcc.h"			/* NQS request completion codes */

#if	SGI | SYS52 | UTS
#else
#if	UNICOS
#include <sys/category.h>		/* For limit (2) */
#include <sys/resource.h>		/* For limit (2) */
#else
#if	BSD42 | BSD43 | ULTRIX | OSF
#include <sys/time.h>			/* So we can include sys/resource.h */
#include <sys/resource.h>		/* Get definition of struct rlimit */
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

/*
 *
 *	External functions and variables.
 */
extern char *asciierrno();		/* Return ASCII errno */
extern int errno;			/* System call error record */

#if	SGI | SYS52 | UTS | OSF
extern long ulimit();
#else
#if	UNICOS
extern long limit();
extern long ulimit();
#else
#if	BSD42 | BSD43 | ULTRIX
#else
BAD SYSTEM TYPE
#endif
#endif
#endif

#if	VALID_LIMITS & (LIM_PPCPUT | LIM_PRCPUT)
#if	BSD42 | BSD43 | ULTRIX | OSF
#if	CPU_LIMIT_GRAN > 2000
REWRITE THIS FUNCTION
/* See the note about cpulimhigh() below. */
#endif
/*** enf_bsdcpu
 *
 *
 *	void enf_bsdcpu():
 *
 *	Enforce within BSD4.2 a per process or per request
 *	cpu time limit.
 */
void enf_bsdcpu (cpulimitp, infinite, limit_type)
struct cpulimit *cpulimitp;		/* Pointer to what is to be set */
short infinite;				/* Boolean */
long limit_type;			/* Per_process or Per_request */
{
	int limresult;
	struct rlimit rlimit;
	
	if (limit_type == LIM_PPCPUT) {
		if (infinite) {
			/*
			 * It is impossible in NQS to have an infinite
			 * maximum and a finite warning.
			 */
			rlimit.rlim_max = RLIM_INFINITY;
			rlimit.rlim_cur = RLIM_INFINITY;
		}
		else {
			/*
			 * If seconds > 1000, ignore ms in order to avoid
			 * overflow.  (1000 is arbitrary. We know from
			 * cpulimhigh() that the limit can be enforced;
			 * we just have to avoid ever expressing the limit
			 * in units finer than those of the system call.)
			 */
			if (cpulimitp->max_seconds > 1000) {
				rlimit.rlim_max =
					cpulimitp->max_seconds * CPU_LIMIT_GRAN;
			}
			/*
			 * It is safe to temporarily express the limit
			 * in units 1000 times finer than the system call.
			 */
			else {
				rlimit.rlim_max =
					(((1000 * cpulimitp->max_seconds)
					+ cpulimitp->max_ms) *
					CPU_LIMIT_GRAN) / 1000;
			}
			/* Same reasoning as above. */
			if (cpulimitp->warn_seconds > 1000) {
				rlimit.rlim_cur =
					cpulimitp->warn_seconds *
					CPU_LIMIT_GRAN;
			}
			else {
				rlimit.rlim_cur =
					(((1000 * cpulimitp->warn_seconds)
					+ cpulimitp->warn_ms) *
					CPU_LIMIT_GRAN) / 1000;
			}
		}
		limresult = setrlimit (RLIMIT_CPU, &rlimit);
		if (limresult == -1L) {
			serexit (RCM_UNABLETOEXE | TCI_PP_CTLEXC,
				 asciierrno());
		}
	}
}
#else
#if	UNICOS
/*** enf_sy5cpu
 *
 *
 *	void enf_sy5cpu():
 *
 *	Enforce within UNICOS a per process or per request cpu time limit.
 */
void enf_sy5cpu (cpulimitp, infinite, limit_type)
struct cpulimit *cpulimitp;		/* Pointer to what is to be set */
short infinite;				/* Boolean */
long limit_type;			/* Per process or Per request */
{
	int category;
	long llimresult;

	if (limit_type == LIM_PPCPUT) {
		category = C_PROC;
	} else {
		category = C_JOB;
	}
	if (infinite) {
		/* Zero means infinity on UNICOS */
		llimresult = limit (category, 0, L_CPU, 0);
	}
	else {
		llimresult = limit (category, 0, L_CPU,
			(((1000 * cpulimitp->max_seconds) +
			cpulimitp->max_ms) * CPU_LIMIT_GRAN) / 1000);
	}
	if (llimresult == -1L) {
		if (limit_type == LIM_PPCPUT) {
			serexit (RCM_UNABLETOEXE | TCI_PP_CTLEXC,
				 asciierrno());
		}
		else {
			serexit (RCM_UNABLETOEXE | TCI_PR_CTLEXC,
				 asciierrno());
		}
	}
}
#else
#if	SGI | SYS52 | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#endif


#if	VALID_LIMITS & LIM_PPNICE
#if	BSD42 | BSD43 | ULTRIX
/*** enf_bsdnic
 *
 *
 *	void enf_bsdnic():
 *
 *	Enforce a nice value within BSD4.2.
 */
void enf_bsdnic (nice_value)
int nice_value;
{
	int limresult;
	
	limresult = setpriority (PRIO_PROCESS, getpid(), nice_value);
	if (limresult == -1) {
		serexit (RCM_UNABLETOEXE | TCI_PP_NELEXC, asciierrno());
	}
}
#else
#if	SGI | SYS52 | UTS | UNICOS | OSF
/*** enf_sy5nic
 *
 *
 *	void enf_sy5nic():
 *
 *	Enforce a nice value within Silicon Graphics' Unix, System V,
 *	UTS, or UNICOS.
 *
 */
void enf_sy5nic (nice_value)
int nice_value;
{
	int limresult;
	
	errno = 0;			/* nice(2) returning -1 doesn't */
					/* always mean failure */
	limresult = nice (0);		/* Get current nice value */
	if (errno != 0) {
		serexit (RCM_UNABLETOEXE | TCI_PP_NELEXC, asciierrno());
	}
	nice (nice_value - limresult);    /* Go to desired value */
	if (errno != 0) {
		serexit (RCM_UNABLETOEXE | TCI_PP_NELEXC, asciierrno());
	}
}
#else
BAD SYSTEM TYPE
#endif
#endif
#endif


/*
 * Below, we define THESE, THEM, and THOSE for C preprocessors that
 * do not allow line continuation.
 */
#define THESE	LIM_PPCORE | LIM_PPDATA | LIM_PPPFILE | LIM_PRPFILE
#define THEM	LIM_PPMEM | LIM_PRMEM | LIM_PPQFILE | LIM_PRQFILE 
#define THOSE	LIM_PPSTACK | LIM_PPTFILE | LIM_PRTFILE | LIM_PPWORK

#if	VALID_LIMITS & (THESE | THEM | THOSE)
#if	BSD42 | BSD43 | ULTRIX | OSF
/*** enf_bsdquo
 *
 *
 *	void enf_bsdquo():
 *
 *	Enforce within BSD4.2 one of the limits: per process core file
 *	size, per process data segment size, per process permanent file
 *	size, per request permanent file size, per process
 *	memory size, per request memory size, per process stack
 *	segment size, per process temporary file size, per request
 *	temporary file size, or per process working set size.
 *
 */
void enf_bsdquo (quotalimitp, infinite, limit_type)
struct quotalimit *quotalimitp;		/* Pointer to what is to be set */
short infinite;				/* Boolean */
long limit_type;			/* One of LIM_??? */
{
	int granularity;		/* Granularity of the sys call */
	int limresult;			/* Return value of limit setting */
	struct rlimit rlimit;		/* Holds the limit itself */

	if (infinite) {
		/*
		 * It is impossible in NQS to have an infinite maximum
		 * and a finite warning.
		 */
		rlimit.rlim_max = RLIM_INFINITY;
		rlimit.rlim_cur = RLIM_INFINITY;
	}
	else {
		if (limit_type & (LIM_PPCORE | LIM_PPPFILE)) {
			granularity = DISK_LIMIT_GRAN;
		}
		else {
			granularity = MEM_LIMIT_GRAN;
		}
		switch (quotalimitp->max_units) {
		case QLM_BYTES:
			rlimit.rlim_max = quotalimitp->max_quota / granularity;
			break;
		case QLM_WORDS:
			rlimit.rlim_max =
				(quotalimitp->max_quota * BYTES_PER_WORD) /
				granularity;
			break;
		case QLM_KBYTES:
			rlimit.rlim_max = (quotalimitp->max_quota * (1<<10)) /
				granularity;
			break;
		case QLM_KWORDS:
			rlimit.rlim_max =
				(quotalimitp->max_quota * (BYTES_PER_WORD) *
				(1<<10)) / granularity;
			break;
		case QLM_MBYTES:
			rlimit.rlim_max = (quotalimitp->max_quota * (1<<20)) /
				granularity;
			break;
		case QLM_MWORDS:
			rlimit.rlim_max =
				(quotalimitp->max_quota * (BYTES_PER_WORD) *
				(1<<20)) / granularity;
			break;
		case QLM_GBYTES:
			rlimit.rlim_max = (quotalimitp->max_quota * (1<<30)) /
				granularity;
			break;
		case QLM_GWORDS:
			rlimit.rlim_max =
				(quotalimitp->max_quota * (BYTES_PER_WORD) *
				(1<<30)) / granularity;
			break;
		}
		switch (quotalimitp->warn_units) {
		case QLM_BYTES:
			rlimit.rlim_cur = quotalimitp->warn_quota / granularity;
			break;
		case QLM_WORDS:
			rlimit.rlim_cur =
				(quotalimitp->warn_quota * BYTES_PER_WORD) /
				granularity;
			break;
		case QLM_KBYTES:
			rlimit.rlim_cur = (quotalimitp->warn_quota * (1<<10)) /
				granularity;
			break;
		case QLM_KWORDS:
			rlimit.rlim_cur =
				(quotalimitp->warn_quota * (BYTES_PER_WORD) *
				(1<<10)) / granularity;
			break;
		case QLM_MBYTES:
			rlimit.rlim_cur = (quotalimitp->warn_quota * (1<<20)) /
				granularity;
			break;
		case QLM_MWORDS:
			rlimit.rlim_cur =
				(quotalimitp->warn_quota * (BYTES_PER_WORD) *
				(1<<20)) / granularity;
			break;
		case QLM_GBYTES:
			rlimit.rlim_cur = (quotalimitp->warn_quota * (1<<30)) /
				granularity;
			break;
		case QLM_GWORDS:
			rlimit.rlim_cur =
				(quotalimitp->warn_quota * (BYTES_PER_WORD) *
				(1<<30)) / granularity;
			break;
		}
	}
	switch (limit_type) {
	case LIM_PPCORE:
		limresult = setrlimit (RLIMIT_CORE, &rlimit);
		if (limresult == -1) {
			serexit (RCM_UNABLETOEXE | TCI_PP_CFLEXC,
				 asciierrno());
		}
		break;
	case LIM_PPDATA:
		limresult = setrlimit (RLIMIT_DATA, &rlimit);
		if (limresult == -1) {
			serexit (RCM_UNABLETOEXE | TCI_PP_DSLEXC,
				 asciierrno());
		}
		break;
	case LIM_PPPFILE:
		limresult = setrlimit (RLIMIT_FSIZE, &rlimit);
		if (limresult == -1) {
			serexit (RCM_UNABLETOEXE | TCI_PP_PFLEXC,
				 asciierrno());
		}
		break;
	case LIM_PPSTACK:
		limresult = setrlimit (RLIMIT_STACK, &rlimit);
		if (limresult == -1) {
			serexit (RCM_UNABLETOEXE | TCI_PP_SSLEXC,
				 asciierrno());
		}
		break;
	case LIM_PPWORK:
		limresult = setrlimit (RLIMIT_RSS, &rlimit);
		if (limresult == -1) {
			serexit (RCM_UNABLETOEXE | TCI_PP_WSLEXC,
				 asciierrno());
		}
		break;
	}
}
#else
#if	SGI | SYS52 | UTS | UNICOS | OSF
/*** enf_sy5quo
 *
 *
 *	void enf_sy5quo():
 *
 *	Enforce within Silicon Graphics' Unix, System V, UTS, or
 *	UNICOS one of the limits: per process core file
 *	size, per process data segment size, per process permanent file
 *	size, per request permanent file size, per process
 *	memory size, per request memory size, per process stack
 *	segment size, per process temporary file size, per request
 *	temporary file size, or per process working set size.
 *
 */
void enf_sy5quo (quotalimitp, infinite, limit_type)
struct quotalimit *quotalimitp;		/* Pointer to what is to be set *
short infinite;				/* Is desired quota infinite? */
long limit_type;			/* One of LIM_??? */
{
	int llimresult;			/* Return value of limit setting */
	int category;			/* For UNICOS limit(2) */

	switch (limit_type) {
	case LIM_PPPFILE:
		if (infinite) {
			serexit (RCM_UNABLETOEXE | TCI_PP_PFLEXC,
				"Infinite per proc. permfile not supported."
				);
		}
		else {
			switch (quotalimitp->max_units) {
			case QLM_BYTES:
				llimresult = ulimit (2,
					quotalimitp->max_quota /
					DISK_LIMIT_GRAN);
				break;
			case QLM_WORDS:
				llimresult = ulimit (2,
					(quotalimitp->max_quota *
					BYTES_PER_WORD) / DISK_LIMIT_GRAN);
				break;
			case QLM_KBYTES:
				llimresult = ulimit (2,
					(quotalimitp->max_quota * (1<<10)) /
					DISK_LIMIT_GRAN);
				break;
			case QLM_KWORDS:
				llimresult = ulimit (2,
					(quotalimitp->max_quota *
					BYTES_PER_WORD * (1<<10)) /
					DISK_LIMIT_GRAN);
				break;
			case QLM_MBYTES:
				llimresult = ulimit (2,
					(quotalimitp->max_quota * (1<<20)) /
					DISK_LIMIT_GRAN);
				break;
			case QLM_MWORDS:
				llimresult = ulimit (2,
					(quotalimitp->max_quota *
					BYTES_PER_WORD * (1<<20)) /
						DISK_LIMIT_GRAN);
				break;
			case QLM_GBYTES:
				llimresult = ulimit (2,
					(quotalimitp->max_quota * (1<<30)) /
						DISK_LIMIT_GRAN);
				break;
			case QLM_GWORDS:
				/*
				 * For SGI, SYS52, and UTS,and OSF
				 * we have checked that
				 * the limit can be expressed in bytes. For
				 * UNICOS, we have not checked that the limit
				 * can be expressed in bytes.  Therefore,
				 * rearrange to avoid overflow.
				 */
				llimresult = ulimit (2,
					((quotalimitp->max_quota * (1<<30)) /
						DISK_LIMIT_GRAN) *
						BYTES_PER_WORD);
				break;
			}
			if (llimresult == -1L) {
				serexit (RCM_UNABLETOEXE | TCI_PP_PFLEXC,
					 asciierrno());
			}
			break;
		}
#if	UNICOS
	case LIM_PPMEM:
	case LIM_PRMEM:
		if (limit_type == LIM_PPMEM) {
			category = C_PROC;
		} else {
			category = C_JOB;
		}

		if (infinite) {
			/* Zero means infinity on UNICOS */
			llimresult = limit (category, 0, L_MEM, 0);
		}
		else {
			switch (quotalimitp->max_units) {
			case QLM_BYTES:
				llimresult = limit (category, 0, L_MEM,
					quotalimitp->max_quota /
					MEM_LIMIT_GRAN);
				break;
			case QLM_WORDS:
				llimresult = limit (category, 0, L_MEM,
					(quotalimitp->max_quota *
					BYTES_PER_WORD) /
					MEM_LIMIT_GRAN);
				break;
			case QLM_KBYTES:
				llimresult = limit (category, 0, L_MEM,
					(quotalimitp->max_quota * (1<<10)) /
					MEM_LIMIT_GRAN);
				break;
			case QLM_KWORDS:
				llimresult = limit (category, 0, L_MEM,
					(quotalimitp->max_quota *
					BYTES_PER_WORD * (1<<10)) /
					MEM_LIMIT_GRAN);
				break;
			case QLM_MBYTES:
				llimresult = limit (category, 0, L_MEM,
					(quotalimitp->max_quota * (1<<20)) /
					MEM_LIMIT_GRAN);
				break;
			case QLM_MWORDS:
				llimresult = limit (category, 0, L_MEM,
					(quotalimitp->max_quota *
					BYTES_PER_WORD * (1<<20)) /
					MEM_LIMIT_GRAN);
				break;
			case QLM_GBYTES:
				llimresult = limit (category, 0, L_MEM,
					(quotalimitp->max_quota * (1<<30)) /
					MEM_LIMIT_GRAN);
				break;
			case QLM_GWORDS:
				/*
				 * For SGI, SYS52, and UTS, and OSF
				 * we have checked that
				 * the limit can be expressed in bytes.
				 * For UNICOS, we have not checked that the
				 * limit can be expressed in BYTES.  Therefore,
				 * rearrange to avoid overflow.
				 */
				llimresult = limit (category, 0, L_MEM,
					((quotalimitp->max_quota * (1<<30)) /
					MEM_LIMIT_GRAN) *
					BYTES_PER_WORD);
				break;
			}
		}
		if (llimresult == -1L) {
			if (limit_type == LIM_PPMEM) {
				serexit (RCM_UNABLETOEXE | TCI_PP_MSLEXC,
					 asciierrno());
			}
			else {
				serexit (RCM_UNABLETOEXE | TCI_PR_MSLEXC,
					 asciierrno());
			}
		}
		break;
#endif
	}
}
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
