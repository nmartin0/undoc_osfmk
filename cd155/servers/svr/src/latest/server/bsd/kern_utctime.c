/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/************************************************************************
 *									*
 *			Copyright (c) 1989, 90 by			*
 *		Digital Equipment Corporation, Maynard, MA		*
 *			All rights reserved.				*
 *									*
 *   This software is furnished under a license and may be used and	*
 *   copied  only  in accordance with the terms of such license and	*
 *   with the  inclusion  of  the  above  copyright  notice.   This	*
 *   software  or  any  other copies thereof may not be provided or	*
 *   otherwise made available to any other person.  No title to and	*
 *   ownership of the software is hereby transferred.			*
 *									*
 *   This software is  derived  from  software  received  from  the	*
 *   University    of   California,   Berkeley,   and   from   Bell	*
 *   Laboratories.  Use, duplication, or disclosure is  subject  to	*
 *   restrictions  under  license  agreements  with  University  of	*
 *   California and with AT&T.						*
 *									*
 *   The information in this software is subject to change  without	*
 *   notice  and should not be construed as a commitment by Digital	*
 *   Equipment Corporation.						*
 *									*
 *   Digital assumes no responsibility for the use  or  reliability	*
 *   of its software on equipment which is not supplied by Digital.	*
 *									*
 ************************************************************************/
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: kern_utctime.c,v $
 * Revision 1.7  1995/02/01  21:28:15  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:27:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/26  12:42:52  paul
 *  Reviewer: none (cfg for changes to 1.2 tree)
 *  Risk: low
 *  Benefit or PTS #: 7698
 *  Testing:
 *  Module(s):
 * 	uxkern/misc.c
 * 	uxkern/rpm_clock.c
 * 	bsd/kern_time.c
 * 	bsd/kern_utctime.c
 * 	afs/afs_osi.c
 *
 *
 * Merge of bug fixes for RPM from 1.2 tree - Moved the actual sending of the
 * RPM offset outside of a TIME lock.
 *
 * Revision 1.4  1994/01/13  17:53:05  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.3.4.1  1994/01/18  20:26:43  paul
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #:7698
 *  Testing:
 *  Module(s):
 *       uxkern/misc.c
 *       uxkern/rpm_clock.c
 *       bsd/kern_time.c
 *       afs/afs_osi.c
 *
 * Changes to fix bug 7698 - Time Lock hang
 *    Moved the RPM's offsett transmittal RPC outside of the TIME lock to
 *    prevent deadlocks.
 *
 *  kern_utctime.c:
 *         Added a call to rpm_transmit_offset in utc_adjtime
 *
 * Revision 1.3  1993/07/14  17:48:12  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:47:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:16:05  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:24  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:07:19  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/03/09  14:20:46  durriya
 * 	Revision 3.2  91/12/18  17:15:05  sp
 * 	Include sys/synch.h to get spl macros
 *
 * Revision 2.2  91/12/16  13:06:15  roy
 * 	Initial check-in.
 * 
 * Revision 3.1  91/10/23  16:37:53  condict
 * Remove unnecessary get_time call.  Time var is now updated correctly.
 * 
 * Revision 3.0  91/10/17  15:44:01  condict
 * Original version from OSF/1 Release 1.0.2 (utc system calls)
 * 
 * Revision 1.4  90/10/07  13:18:07  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:51:30  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:57:22  gm]
 * 
 * Revision 1.3  90/08/24  11:17:25  devrcs
 * 	Changes for new system call interface.
 * 	[90/08/19  16:42:38  gmf]
 * 
 * Revision 1.2  90/08/09  13:14:16  devrcs
 * 	New file for DCE time syscalls. Added priviledge check for utc_adjtime.
 * 
 * Revision 0.0  90/01/18            Joe Comuzzi
 * 	Added functions to return maximum drift rate and resolution.
 * 	[90/07/31  16:34:36  brezak]
 * 	[90/01/18            Joe Comuzzi]
 * 
 * Revision 0.0  90/03/30            Joe Comuzzi
 * 	    Changed external interface to nanoseconds, eliminated type-puns in
 * 	    external interface.
 * 	[90/03/30            Joe Comuzzi]
 * 
 * Revision 0.0  90/04/06            Joe Comuzzi
 * 	    Fixed Daylight time bug.
 * 	[90/04/06            Joe Comuzzi]
 * 
 * Revision 0.0  90/06/26            Joe Comuzzi
 * 	    Modified to work in Mach
 * 	[90/06/26            Joe Comuzzi]
 * 
 * Revision 0.0  90/07/16            Joe Comuzzi
 * 	    Fixed Big/Little Endian problems
 * 	[90/07/16            Joe Comuzzi]
 * 
 * $EndLog$
 */
#include <cputypes.h>
#include <sys/secdefines.h>

#include <sys/param.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/utctime.h>

#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif

#include <machine/reg.h>
#include <machine/cpu.h>
#if	SEC_BASE
#include <sys/security.h>
#endif

/* 
 * Get UTC Time of day and adjust clock routines.
 *
 * These routines provide the kernel entry points to get and set
 * utc timestamps, used by DECdts - the time syncronization service.
 */

/*
 * Variables shared with other kernel modules
 */

int inaccinfinite = 1;			/* Set in kern_time.c when old
                                           interface is used */

/*
 * Variables defined and used by other kernel modules
 */

extern int tickdelta;			/* Amount to change tick by each 
					   tick - used by kern_clock.c */
extern long timedelta;			/* Amount of time remaining to be
					   adjusted - used by kern_clock.c */
extern struct cpusw *cpup;		/* Pointer to cpu specific info */
extern int fixtick;			/* Amount to bump clock once per sec
					   used by kern_clock.c */
extern int maxdriftrecip;               /* Clock drift recipricol */

#if BYTE_ORDER == BIG_ENDIAN
static struct int64 diff_base_times = { /* Constant difference */
    0x01b21dd2, 0x13814000
};
#else /* BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == PDP_ENDIAN */
static struct int64 diff_base_times = { /* Constant difference */
    0x13814000, 0x01b21dd2		/* between base times */
};
#endif

/*
 * Variables used in inaccuracy calculation
 */

static struct int64 inacc;		/* Base inaccuracy */
static long int tickadjcount;		/* Number of ticks for which
					   adjustment will occur */
static struct timeval driftbase;	/* Time at which last base
					   inaccuracy was calculated */
static struct timeval adjbase;		/* Time at which last adjustment
					   was started */
static struct int64 leaptime;		/* Time of next leap second
					   after driftbase + inacc */

/*
 * Variables which have the current and next TDF info
 */

#define NO_TDF	8192			/* Used as out-of-band value to
                                           indicate not yet initialized */
static int curtdf = NO_TDF;		/* Current time differential factor */
static int nextdf = NO_TDF;		/* Next time differential factor */
static int tdftime = 0x7fffffff;	/* Time of next TDF change */

/*
 * These macros are a collection of 64-bit integer stuff used by
 * kern_utctime.c. It has been isolated to assist in migration to 64-bit
 * platforms. Obvisously for 64-bit machines, these macros could be greatly
 * simplified. Alternatively most of this 64-bit arithmatic could be
 * eliminted if the inaccuracy was incremented on each hard clock tick.
 */

/*
 * 64 Bit integer type
 *	This is defined in utctime.h
 *
 *      Little Endian                         Big Endian
 * 
 *   struct int64 {			struct int64 {
 * 	  unsigned long lo;			unsigned long hi;
 *	  unsigned long hi;			unsigned long lo;
 *   };					};
 */

/*
 * UTC_ADD32P - macro to add a positive 32-bit integer to a 64-bit integer
 */
#define UTC_ADD32P(I,extint)						\
{									\
    int _ADD32P_intermediate = ((extint)->lo&0x80000000);		\
									\
    (extint)->lo += (I) ;						\
    if (_ADD32P_intermediate)						\
    {									\
	if ((long int) (extint)->lo >= 0) 				\
	    (extint)->hi++ ;						\
    }									\
}
/* #define UTC_ADD32P(I,extint)	(extint) += (I) */

/*
 * UTC_MUL32P - macro to multiply two positive 32-bit numbers and return
 *	        a 64-bit result.
 */
#define UTC_MUL32P(I, J, extint)						\
{									\
    unsigned long int _MUL32P_u1 = (I) >> 16;				\
    unsigned long int _MUL32P_u2 = (I) & 0xffff;			\
    unsigned long int _MUL32P_v1 = (J) >> 16;				\
    unsigned long int _MUL32P_v2 = (J) & 0xffff;			\
    register unsigned long int _MUL32P_temp;				\
									\
    _MUL32P_temp = _MUL32P_u2 * _MUL32P_v2;				\
    (extint)->lo = _MUL32P_temp & 0xffff;				\
    _MUL32P_temp = _MUL32P_u1 * _MUL32P_v2 + (_MUL32P_temp >> 16);	\
    (extint)->hi = _MUL32P_temp >> 16;					\
    _MUL32P_temp = _MUL32P_u2 * _MUL32P_v1 + (_MUL32P_temp & 0xffff);	\
    (extint)->lo += (_MUL32P_temp & 0xffff) << 16;			\
    (extint)->hi += _MUL32P_u1 * _MUL32P_v1 + (_MUL32P_temp >> 16);	\
}

/* #define UTC_MUL32P(I,extint)	(extint) = (I) * (J) */


/* 
 * UTC_ADD64 - macro to add two 64-bit integers.
 */

#define UTC_ADD64(ext1,ext2)						\
    (ext2)->hi += (ext1)->hi;						\
    if (!(((ext1)->lo&0x80000000) ^ ((ext2)->lo&0x80000000)))		\
    {									\
	(ext2)->lo += (ext1)->lo; 					\
	if ((long int) (ext1)->lo < 0)					\
	    (ext2)->hi++; 						\
    }									\
    else								\
    {									\
	(ext2)->lo += (ext1)->lo;					\
	if ((long int) (ext2)->lo >= 0) 				\
	    (ext2)->hi++ ;						\
    }
/* #define UTC_ADD64(ext1,ext2)	(ext2) += (ext1) */

/* 
 * UTC_SUB64 - macro to subtract two 64-bit integers.
 */

#define UTC_SUB64(ext1,ext2)						\
    (ext2)->hi += ~((ext1)->hi);					\
    if (!((~((ext1)->lo)&0x80000000) ^ ((ext2)->lo&0x80000000)))	\
    {									\
	(ext2)->lo += ~((ext1)->lo); 					\
	if ((long int) (ext1)->lo >= 0)					\
	    (ext2)->hi++; 						\
    }									\
    else								\
    {									\
	(ext2)->lo += ~((ext1)->lo);					\
	if ((long int) (ext2)->lo >= 0) 				\
	    (ext2)->hi++ ;						\
    }									\
    if (0 == ++((ext2)->lo))						\
	(ext2)->hi++;
/* #define UTC_SUB64(ext1,ext2)	(ext2) -= (ext1) */

/*
 * Routine to compute the number of ticks which have occured since
 * the last time an adjustment was started.
 */

static long int baseticks(curtime, basetime)

struct timeval *curtime;	/* Current time */
struct timeval *basetime;	/* Time last adjustment was started */

{
	struct timeval elapsed;	/* Elapsed time since last adjustment was
				   started */

	elapsed.tv_sec = curtime->tv_sec - basetime->tv_sec;
	elapsed.tv_usec = curtime->tv_usec - basetime->tv_usec;

	/* Calculate the number of ticks, if we must approximate
	   round down so we'll overestimate the inaccuracy */

	/* If it won't overflow, compute using micro-seconds */
	if (elapsed.tv_sec < 4000)
		return(((unsigned)elapsed.tv_sec * K_US_PER_SEC +
			elapsed.tv_usec) / (tick + tickdelta));
/* Change the previous lines when kernel is switched to nano-seconds
 *		return(((unsigned)elapsed.tv_sec * (K_NS_PER_SEC / 1000) +
 *			elapsed.tv_nsec / 1000) /
 *		       ((tick + tickdelta + 999) / 1000));
 */
	/* If it won't overflow, compute using milli-seconds */

	if (elapsed.tv_sec < 4000000) {
 		return(((unsigned)elapsed.tv_sec * (K_US_PER_SEC / 1000) +
			elapsed.tv_usec / 1000) /
		       ((tick + tickdelta + 999) / 1000));
/* Change the previous lines when kernel is switched to nano-seconds
 *		return(((unsigned)elapsed.tv_sec * (K_NS_PER_SEC / 1000000) +
 *			elapsed.tv_nsec / 1000000) /
 *		       ((tick + tickdelta + 999999) / 1000000));
 */
	} else {
		/* Compute using seconds */

		struct int64 temp;

		UTC_MUL32P(K_US_PER_SEC / (tick + tickdelta),
/* Change the previous line when kernel is switched to nano-seconds
 *		UTC_MUL32P(K_NS_PER_SEC / (tick + tickdelta), */
		       elapsed.tv_sec, &temp);

		if (temp.hi == 0 && (long)temp.lo > 0)
			return(temp.lo);
	}

	return(0x7fffffff);
}

/*
 * Routine to calculate the drift and resolution contribution
 * to inaccuracy.
 *
 *	Return drift in 100ns units, if infinite (or just too large) return
 *      negative.
 */

static long int driftfactor(curtv, driftbase)

struct timeval *curtv;		/* Current time */
struct timeval *driftbase;	/* Last time base inaccuracy was
				   calculated */

{
	struct timeval	elapsed;	/* elapsed time since last time
					   base inaccuracy was
					   calculated */
	register long int drift;	/* Contribution to inaccuracy of
					   drift and resolution */

	/* Compute the amount of inaccuracy since the last base time by
	   computing the elapsed time, and scaling by the max drift rate */

	elapsed.tv_sec = curtv->tv_sec - driftbase->tv_sec;
	elapsed.tv_usec = curtv->tv_usec - driftbase->tv_usec +
                          tick + fixtick;

	/* If drift computation won't overflow, compute using micro-seconds */

	if (elapsed.tv_sec < 2000) {
		drift = (elapsed.tv_sec * K_US_PER_SEC +
			 elapsed.tv_usec +
			 maxdriftrecip - 1) /
			maxdriftrecip * (K_100NS_PER_SEC / K_US_PER_SEC);
/* Change the previous lines when kernel is switched to nano-seconds
 *		drift = (elapsed.tv_sec * (K_NS_PER_SEC / 1000) +
 *			 (elapsed.tv_nsec + 999) / 1000 +
 *			 maxdriftrecip - 1) /
 *			maxdriftrecip *
 *                      (K_100NS_PER_SEC / (K_NS_PER_SEC / 1000));
 */
	} else {
		/* If drift computation won't overflow compute using
		   milli-seconds */

		if (elapsed.tv_sec < 2000000) {
			drift = ((elapsed.tv_sec * (K_US_PER_SEC / 1000) +
				  (elapsed.tv_usec + 999) / 1000 +
				  maxdriftrecip - 1) / maxdriftrecip) *
				 (1000 * (K_100NS_PER_SEC / K_US_PER_SEC));
/* Change the previous lines when kernel is switched to nano-seconds
 *			drift = ((elapsed.tv_sec * (K_NS_PER_SEC / 1000000) +
 *			  	  (elapsed.tv_nsec + 999999) / 1000000 +
 *				  maxdriftrecip - 1) / maxdriftrecip) *
 *				 (1000 * (K_100NS_PER_SEC / (K_NS_PER_SEC / 1000)));
 */
		} else {
			return(-1);
		}
	}

	/* Add in resolution */

	drift += (tick + fixtick) * (K_100NS_PER_SEC/K_US_PER_SEC);
/* Change the previous line when kernel is switched to nano-seconds
 *	drift += ((tick + fixtick) + (K_NS_PER_SEC/K_100NS_PER_SEC) - 1)/
 *               (K_NS_PER_SEC/K_100NS_PER_SEC);
 */

	return(drift);
}

/*
 * Kernel mode get utc timestamp routine
 */

getutc(utc)

struct utc *utc;	/* 128 bit UTC timestamp */

{
	struct int64	atime;		/* Current time */
	struct int64	saveinacc;	/* Last base inaccuracy */
	struct int64	saveleaptime;	/* Time of next potential leap
					   second correction */
	struct timeval	savedriftbase;	/* Time of last base inaccuracy */
	struct timeval	atv;		/* Current time as a timeval */
        int             savetdf;	/* Current time differential factor */
	struct int64	hightime;	/* high end of current time interval */
	struct int64	correc;		/* amount to reduce inaccuracy
					   due to current adjustment */
	int		saveinfinite;	/* Inaccuracy is currently
					   infinite */
	long int	savetickadjcnt;	/* Current number of ticks for
					   which adjustment will occur */
	long int	saveticks;	/* Count of ticks since last
					   adjust started */
	long int	drift;		/* Drift contribution to
					   inaccuracy */
	int		s;		/* Temp for saving IPL */

	s = splhigh();
	TIME_WRITE_LOCK();
	atv = time;
        savetdf = (time.tv_sec > tdftime) ? nextdf : curtdf;
	savedriftbase = driftbase;
	saveinacc = inacc;
	saveinfinite = inaccinfinite;
	saveleaptime = leaptime;
	savetickadjcnt = tickadjcount;
	saveticks = baseticks(&time, &adjbase);
	TIME_WRITE_UNLOCK();
	splx(s);

	/* Convert the time in seconds and micro-seconds to a 64-bit integer
	   number of 100 nanoseconds. */

	UTC_MUL32P(atv.tv_sec, K_100NS_PER_SEC, &atime);
	UTC_ADD32P(atv.tv_usec * (K_100NS_PER_SEC / K_US_PER_SEC), &atime);
/* Change the previous line when kernel is switched to nano-seconds
 *	UTC_ADD32P(atv.tv_nsec / (K_NS_PER_SEC / K_100NS_PER_SEC), &atime);
 */

	/* Compensate for different base times */

	UTC_ADD64(&diff_base_times, &atime);

	/* Compute drift and resolution contribution to inaccuracy */

	drift = driftfactor(&atv, &savedriftbase);

	/* If not infinite, add in the drift contribution and reduce
	   the inaccuracy by the adjustment amount. Finally check for
	   leap seconds */

	if (drift >= 0 && !saveinfinite) {

		UTC_ADD32P(drift, &saveinacc);

		/* Calculate the amount to reduce inaccuracy from an
		   adjustment */

		if (savetickadjcnt != 0) {
			UTC_MUL32P((saveticks < savetickadjcnt) ?
				saveticks : savetickadjcnt,
			       tickdelta * (K_100NS_PER_SEC / K_US_PER_SEC),
/* Change the previous line when kernel is switched to nano-seconds
 *			       tickdelta / (K_NS_PER_SEC / K_100NS_PER_SEC), 
 */
			       &correc);

			UTC_SUB64(&correc, &saveinacc);
		}

		/* Compute upper end of interval */

		hightime = atime;
		UTC_ADD64(&saveinacc, &hightime);

		/* If upper end of interval is past a potential leap
		   second, add an extra second of inaccuracy */

		if ((hightime.hi > saveleaptime.hi) ||
		    (hightime.hi == saveleaptime.hi &&
		     hightime.lo >= saveleaptime.lo))
			UTC_ADD32P(K_100NS_PER_SEC, &saveinacc);

	}

	/* If inaccuracy has become infinite, just return
	   max value */

	if (drift < 0 || saveinfinite || saveinacc.hi & 0xffff0000) {
		saveinacc.lo = 0xffffffff;
		saveinacc.hi = 0xffff;
	}

	/* If DTS has not yet supplied a time differential factor
           pick up the kernels and use it (this could be off by an hour). */

	if (savetdf == NO_TDF)
	    savetdf = -tz.tz_minuteswest;

#if BYTE_ORDER == LITTLE_ENDIAN
	utc->time = atime;
	utc->inacclo = saveinacc.lo;
	utc->inacchi = saveinacc.hi;
	utc->tdf = savetdf;
	utc->vers = K_UTC_VERSION;
	utc->big_endian = 0;
#endif
#if BYTE_ORDER == BIG_ENDIAN
	utc->time = atime;
	utc->inacchi = saveinacc.hi;
	utc->inaccmid = saveinacc.lo >> 16;
	utc->inacclo = saveinacc.lo;
	utc->tdfhi = savetdf >> 4;
	utc->tdflo = savetdf & 0xf;
	utc->vers = K_UTC_VERSION;
	utc->big_endian = 1;
#endif
#if BYTE_ORDER == PDP_ENDIAN
	utc->timelolo = atime.lo & 0xffff;
	utc->timelohi = atime.lo >> 16;
	utc->timehilo = atime.hi & 0xffff;
	utc->timehihi = atime.hi >> 16;
	utc->inacchi = saveinacc.hi;
	utc->inaccmid = saveinacc.lo >> 16;
	utc->inacclo = saveinacc.lo;
	utc->tdf = savetdf;
	utc->vers = K_UTC_VERSION;
	utc->big_endian = 0;		/* This is little-endian */
#endif

}

/*
 * User mode get utc timestamp routine
 *
 *	call the kernel routine and copy the results out.
 */

utc_gettime(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		struct utc *utc;	/* user mode UTC time stamp */
	} *uap = (struct args *) args;
	struct utc autc;

	getutc(&autc);

	return (copyout((caddr_t)&autc, (caddr_t)(uap->utc),
			    sizeof (autc)));
}

/*
 * set/adjust utc timestamp routine
 *
 */

utc_adjtime(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		enum adjmode	modeflag;	/* Set/adjust flag
						    0 - Set time
						    1 - Adjust time
						    2 - End adjustment
						    3 = Get clock resolution
						    4 = Get clock max drift
						    5 = Trim clock frequency
						    6 = Get trimmed frequency */
		union adjunion	*argblk;	/* Pointer to argument block
						   for sub-function:
						    0 - *adjargs union member
						    1 - *adjargs union member
						    2 - IGNORED
						    3 - *resolution
						    4 - *maxdriftrecip
						    5 - *trimargs union member
						    6 - *frequency */
							
	} *uap = (struct args *) args;
	struct adjargs aargs;			/* Copy of adj arguments */
	struct trimargs targs;			/* Copy of trim arguments */
	/*
	 * For function 0 and 1:
	 *	struct timespec	a_adjustment; 	Amount to adjust or change
	 *	struct timespec	a_comptime;	Time which corresponds to base
	 *					inaccuracy
	 *	struct int64	a_baseinacc;	Base inaccuracy
	 *	struct int64	a_leaptime;	Time of next potential leap
	 *					second
	 *	long int	a_adjrate;	Rate at which to adjust
	 *					    1000 = .1% (.0768% on PMAX)
	 *					    100 = 1% (.999% on PMAX)
	 *					    10 = 10%, etc.
	 *					    Ignored for set time
	 *	long int	a_curtdf;	Current timezone
	 *	long int	a_nextdf;	Next timezone (Daylight time)
	 *	long int	a_tdftime;	Time of next timezone change
	 * For function 3:
	 *	long int	*resolution;	Resolution of clock in nanosecs
	 * For function 4:
	 *	long int	*maxdrift;	Maximun drift rate of clock
	 * For function 5:
	 *	long int	t_frequency;	New frequency trim of clock
	 *	long int	t_maxdrift;	New maximun drift rate of clock
	 * For function 6:
	 *	long int	*frequency;	Current frequency trim of clock
	 */
	long resolution;                /* Resolution of clock */
	long frequency;			/* frequency trim of clock */
	long ndelta;			/* New delta value */
	int s;				/* Temp for saving IPL */
	int newinfinite = 0;		/* Flag for new inaccinfinite */
	long int n;			/* Random temp */
	int error;

	/*
	 * Validate privleges
	 */
#if     SEC_BASE
	if (!privileged(SEC_SYSATTR, EPERM))
		return (EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	/*
	 * Validate arguments
	 */

	switch (uap->modeflag) {

	    case settime:
	    case adjusttime:

		error = copyin((caddr_t)(uap->argblk),
				   (caddr_t)&aargs, sizeof (aargs));
		if (error)
			return (error);
		if (aargs.a_adjrate == 0)
			aargs.a_adjrate = 100;	/* Default to 1% */
		if (aargs.a_adjrate < 0 ||
		    aargs.a_adjrate > tick/2 )
			return (EINVAL);
		if (aargs.a_adjustment.ts_nsec > K_NS_PER_SEC ||
		    aargs.a_adjustment.ts_nsec < -K_NS_PER_SEC)
			return (EINVAL);
		if (aargs.a_adjustment.ts_nsec < 0) {
			aargs.a_adjustment.ts_nsec += K_NS_PER_SEC;
			aargs.a_adjustment.ts_sec--;
		}
		if (uap->modeflag == adjusttime &&
				     (aargs.a_adjustment.ts_sec > 2000 ||
				      aargs.a_adjustment.ts_sec < -2000))
			return (EINVAL);
		if (aargs.a_comptime.ts_sec == 0) {
			newinfinite = 1;
		}
		break;

	    case endadjust:

		break;

	    case getresolution:

		resolution = (tick + fixtick) * (K_NS_PER_SEC/K_US_PER_SEC);
/* Change the previous line when kernel is switched to nano-seconds
 *              resolution = tick + fixtick;
 */
		error = copyout((caddr_t)&resolution, 
				    (caddr_t)(uap->argblk),
				    sizeof (long));
		return (error);

	    case getdrift:

		error = copyout((caddr_t)&maxdriftrecip,
				    (caddr_t)(uap->argblk),
				    sizeof (long));
		return (error);

	    case setfreq:

		error = copyin((caddr_t)(uap->argblk),
				   (caddr_t)&targs, sizeof (targs));
		if (error)
			return (error);
		/* Maximum drift rate must be less than 1%! */
		if (targs.t_maxdrift <= 100)
			return (EINVAL);
		break;

	    case getfreq:

		frequency = (tick * hz + fixtick) *
			    (K_NS_PER_SEC/K_US_PER_SEC);
/* Change the previous lines when kernel is switched to nano-seconds
 *              frequency = tick * hz + fixtick;
 */

		error = copyout((caddr_t)&frequency,
				    (caddr_t)(uap->argblk),
				    sizeof (long));
		return (error);

	    default:

		return (EINVAL);
	}

	s = splhigh();
	TIME_WRITE_LOCK();
	switch (uap->modeflag) {
	    case settime:	/* Set time */
		boottime.tv_sec += aargs.a_comptime.ts_sec - time.tv_sec;
   		time.tv_sec += aargs.a_adjustment.ts_sec;
		time.tv_usec += aargs.a_adjustment.ts_nsec /
                               ( K_NS_PER_SEC / K_US_PER_SEC );
		if (time.tv_usec >= K_US_PER_SEC) {
			time.tv_usec -= K_US_PER_SEC;
			time.tv_sec++;
		}

		tickdelta = 0;
		timedelta = 0;
		adjbase = time;
		driftbase.tv_sec = aargs.a_comptime.ts_sec;
		driftbase.tv_usec = aargs.a_comptime.ts_nsec /
                                    ( K_NS_PER_SEC / K_US_PER_SEC );
		inacc = aargs.a_baseinacc;
		leaptime = aargs.a_leaptime;
		tickadjcount = 0;
		inaccinfinite = newinfinite;
		curtdf = aargs.a_curtdf;
		nextdf = aargs.a_nextdf;
		tdftime = aargs.a_tdftime;
		resettodr();
		break;
		
	    case adjusttime:	/* Adjust time */

/* When the kernel is switched to nano-seconds, the variables timedelta and
 * tickdelta will probably be changed. A simple change of units will not
 * surfice since then the range will be too small. This code (as will the code
 * that supports adjtime) will need to be reworked. */

		ndelta = aargs.a_adjustment.ts_sec * K_US_PER_SEC +
			 aargs.a_adjustment.ts_nsec /
                         ( K_NS_PER_SEC / K_US_PER_SEC );

		tickdelta = tick / aargs.a_adjrate;
		if (ndelta % tickdelta)
			ndelta = ndelta / tickdelta * tickdelta;
		timedelta = ndelta;
		adjbase = time;
		driftbase.tv_sec = aargs.a_comptime.ts_sec;
		driftbase.tv_usec = aargs.a_comptime.ts_nsec /
                                    ( K_NS_PER_SEC / K_US_PER_SEC );
		inacc = aargs.a_baseinacc;
		leaptime = aargs.a_leaptime;
		tickadjcount = ((ndelta >= 0) ? ndelta : -ndelta) / tickdelta;
		inaccinfinite = newinfinite;
		curtdf = aargs.a_curtdf;
		nextdf = aargs.a_nextdf;
		tdftime = aargs.a_tdftime;
		break;

	    case endadjust:	/* End adjustment */

		timedelta = 0;
		n = baseticks(&time, &adjbase);
		if (n < tickadjcount)
			tickadjcount = n;
		break;

	    case setfreq:	/* Trim frequency of clock */
		/* The units of frequency are nanoseconds/second, we compute
                   a new tick so that after hz ticks, the clock has advanced
		   targs.t_frequency nanoseconds! */
		/* For micro-second version we compute using nanoseconds and
                   round to get as close as we can */

		tick = targs.t_frequency / hz /
                        (K_NS_PER_SEC/K_US_PER_SEC);
		fixtick = K_US_PER_SEC - tick * hz;
/* When kernel is switch to nanoseconds, replace preceeding statements
 * with:
 *		tick = targs.t_frequency / hz;
 *		fixtick = K_NS_PER_SEC - tick * hz;
 */
		maxdriftrecip = targs.t_maxdrift;
		
	}
        TIME_WRITE_UNLOCK();
	splx(s);

#if defined(__i860__) && defined(TNC)
	rpm_transmit_offset();
#endif

	return (0);
}
