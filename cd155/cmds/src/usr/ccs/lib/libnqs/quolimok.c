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
 
/*++ quolimok.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/quolimok.c,v $
 *
 * DESCRIPTION:
 *
 *	Given the numerical value of a resource limit, report whether
 *	that limit would be too high or too low to enforce on this
 *	machine.  If the limit would be out of bounds, suggest
 *	a value that would be in bounds.
 *
 *
 *	Author:
 *	-------
 *	Robert W. Sandstrom, Sterling Software Incorporated.
 *	January 22, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:27:34 $ $State: Exp $)
 * $Log: quolimok.c,v $
 * Revision 1.4  1994/11/19  02:27:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/19  17:48:34  mwan
 * Before T10 frozen. Fixed several PTS
 *
 * Revision 1.2  1992/10/09  20:18:20  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:37  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:53:37  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)quolimok.c	1.2 (quolimok.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include "nqs.h"		/* for struct quotalimit, etc. */
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>		/* So we can include sys/resource.h */
#include <sys/resource.h>	/* for RLIM_INFINITY */
#if	RLIM_INFINITY != 0x7FFFFFFF
REWRITE THIS FILE
#endif
#if	MEM_LIMIT_GRAN != 1	/* BSD4.2, BSD4.3, and ULTRIX code assumes */
				/* MEM_LIMIT_GRAN == 1 */
REWRITE THIS FILE
#endif
#if	DISK_LIMIT_GRAN != 1	/* BSD4.2, BSD4.3, and ULTRIX code assumes */
				/* DISK_LIMIT_GRAN == 1 */
REWRITE THIS FILE
#endif
#else
#if	SGI | SYS52 | UNICOS | UTS | OSF
#else
BAD SYSTEM TYPE
#endif
#endif

/*** cpulimhigh
 *
 *
 *	int cpulimhigh ():
 *
 *	Given a cpu time limit, report whether the implied limit
 *	could be enforced on this machine.  In determining whether
 *	the limit is too high, we must remember that some
 *	apparently finite limit values might have been usurped
 *	by the system call in question to mean "infinity".
 *
 *	If the limit is ok, return 0. If the limit is too big,
 *	return 1 and assign the largest enforceable limit 
 *	to the designated variables.
 */
int cpulimhigh (secs, ms, infinite, new_secsp, new_msp)
unsigned long secs;			/* Seconds part */
short ms;				/* Milliseconds part */
short infinite;				/* Boolean */
unsigned long *new_secsp;		/* Pointer to altered seconds value */
short *new_msp;				/* Pointer to altered milliseconds */
{
#if	SGI | SYS52 | UNICOS |  UTS | OSF
#else
#if	BSD42 | BSD43 | ULTRIX
 	if (infinite) return (0);
	if (secs >= (RLIM_INFINITY - ((ms * CPU_LIMIT_GRAN) / 1000))
			/ CPU_LIMIT_GRAN) {
		*new_secsp = (RLIM_INFINITY / CPU_LIMIT_GRAN) -1;
		*new_msp = 0;
		return (1);
	}
#else
BAD SYSTEM TYPE
#endif
#endif
	return (0);
}


/*** cpulimlow
 *
 *
 *	int cpulimlow ():
 *
 *	Given a cpu time limit, report whether the implied limit
 *	could be enforced on this machine.  In determining whether
 *	the limit is too low, we must remember that some
 *	apparently finite limit values might have been usurped
 *	by the system call in question to mean "infinity".
 *
 *	The limit shall be deemed to be too small if it is
 *	a small number and attempting to enforce it would
 *	result in an infinite limit.
 *
 *	If the limit is ok, return 0. If the limit is too small,
 *	return 1 and assign the smallest enforceable limit
 *	to the designated variables.
 */
int cpulimlow (secs, ms, new_secsp, new_msp)
unsigned long secs;			/* Seconds part */
short ms;				/* Milliseconds part */
unsigned long *new_secsp;		/* Pointer to altered seconds value */
short *new_msp;				/* Pointer to altered milliseconds */
{
#if	BSD42 | BSD43 | SGI | SYS52 | ULTRIX | UTS | OSF
#else
#if	UNICOS
	/*
	 *  We cannot enforce  a limit of "0".  If the limit is "0",
	 *  change it to something low and enforceable.
	 */
	if ((((secs * 1000) + ms) * CPU_LIMIT_GRAN) / 1000 < 1) {
		if (CPU_LIMIT_GRAN > 1000) {
			*new_secsp = 0;
			*new_msp = 1;
			return (1);
		}
		else if (CPU_LIMIT_GRAN == 1) {
			*new_secsp = 1;
			*new_msp = 0;
			return (1);
		}
		else {
			*new_secsp = 0;
			*new_msp = (1000 + (CPU_LIMIT_GRAN-1)) / CPU_LIMIT_GRAN;
			return (1);
		}
	}
#else
BAD SYSTEM TYPE
#endif
#endif
	return (0);
}


/*** quolimhigh
 *
 *
 *	int quolimhigh():
 *
 *	Given the coefficient and the units of a memory or disk
 *	resource limit, report whether the implied limit could be
 *	enforced on this machine. In determining whether the limit
 *	is too high, we must remember that some apparently finite
 *	limit values might have been usurped by the system call in
 *	question to mean "infinity". 
 *
 *	Presently, we assume that on BSD4.2, BSD4.3, and ULTRIX, that
 *	MEM_LIMIT_GRAN and DISK_LIMIT_GRAN are 1.  If either is not, the
 *	code breaks.  We also pretend that for SYS52, SGI, and UTS, a limit
 *	is too high if the limit expressed in bytes doesn't fit in 31 bits.
 *	This treats some legal limits as illegal.
 *
 *	If the limit could be enforced, return 0.
 *	If the limit could not be enforced, return 1
 *	and assign the largest limit that could be enforced
 *	to the designated variables.
 *
 *	We assume that the actual parameter "coeff" < 2**31.
 *	This is in line with scanquolim().  Limit_type is kept as
 *	a formal parameter for two reasons:
 *	1) For symmetry with quolimlow().
 *	2) To remind porters to consider it.
 */
int quolimhigh (coeff, units, infinite, limit_type, new_coeffp, new_unitsp)
unsigned long coeff;			/* Coefficient of limit in question */
short units;				/* Units of limit in question */
short infinite;				/* Boolean */
int limit_type;				/* One of LIM_??? */
unsigned long *new_coeffp;		/* Pointer to altered coefficient */
short *new_unitsp;			/* Pointer to altered units */
{
#if	BSD42 | BSD43 | ULTRIX
	/*
	 * If the limit would look like infinity to the system call,
	 * or if it is bigger than the number that would look like
	 * infinity, change it and return 1.
	 */
	if (infinite) return (0);
	switch (units) {
	case QLM_BYTES:
		if (coeff >= RLIM_INFINITY) {
			*new_coeffp = RLIM_INFINITY - 1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_WORDS:
		if (coeff >= RLIM_INFINITY / (BYTES_PER_WORD)) {
			*new_coeffp = RLIM_INFINITY -1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_KBYTES:
		if (coeff >= RLIM_INFINITY / (1<<10)) {
			*new_coeffp = RLIM_INFINITY - 1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_KWORDS:
		if (coeff >= RLIM_INFINITY / ((1<<10) * BYTES_PER_WORD)) {
			*new_coeffp = RLIM_INFINITY -1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_MBYTES:
		if (coeff >= RLIM_INFINITY / (1<<20)) {
			*new_coeffp = RLIM_INFINITY - 1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_MWORDS:
		if (coeff >= RLIM_INFINITY / ((1<<20) * BYTES_PER_WORD)) {
			*new_coeffp = RLIM_INFINITY -1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_GBYTES:
		if (coeff >= RLIM_INFINITY / (1<<30)) {
			*new_coeffp = RLIM_INFINITY - 1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_GWORDS:
		/*
		 * Zero gigawords is an exception.
		 */
		if (coeff == 0) return (0);
		/*
		 * In the case of gigawords, we have to check
		 * for overflow.
		 */
		if (BYTES_PER_WORD > 1) {
			*new_coeffp = RLIM_INFINITY -1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		if (coeff >= (RLIM_INFINITY / (1<<30)) / BYTES_PER_WORD) {
			*new_coeffp = RLIM_INFINITY -1;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	}
#else
#if	 SGI | SYS52 |  UTS | OSF
	/*
	 *  Unlike BSD4.2, BSD4.3, and ULTRIX, we do not have to watch out
	 *  for infinity here.  However, if the limit, when expressed in bytes,
	 *  would be > 2**31 -1, then change it and return 1.
	 */
#ifdef SDSC
	if (infinite) return (0);
#else
	if (infinite) {
		*new_coeffp = 2147483647L;
		*new_unitsp = (short) QLM_BYTES;
		return (1);
	}
#endif
	switch (units) {
	case QLM_BYTES:
		return (0);
	case QLM_WORDS:
		if (coeff > 2147483647L / BYTES_PER_WORD) {
			*new_coeffp = 2147483647L;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_KBYTES:
		if (coeff > 2147483647L / (1<<10)) {
			*new_coeffp = 2147483647L;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_KWORDS:
		if (coeff > 2147483647L / ((1<<10) * BYTES_PER_WORD)) {
			*new_coeffp = 2147483647L;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_MBYTES:
		if (coeff > 2147483647L / (1<<20)) {
			*new_coeffp = 2147483647L;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_MWORDS:
		if (coeff > 2147483647L / ((1<<20) * BYTES_PER_WORD)) {
			*new_coeffp = 2147483647L;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_GBYTES:
		if (coeff > 2147483647L / (1<<30)) {
			*new_coeffp = 2147483647L;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	case QLM_GWORDS:
		if (coeff == 0) return (0);
		/*
		 * In the case of gigawords, we have to check
		 * for overflow.
		 */
		if (BYTES_PER_WORD > 1) {
			*new_coeffp = 2147483647L;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		if (coeff > (2147483647L / (1<<30)) / BYTES_PER_WORD) {
			*new_coeffp = 2147483647L;
			*new_unitsp = (short) QLM_BYTES;
			return (1);
		}
		return (0);
	}
#else
#if	UNICOS
	/*
	 * 64 bits and the fact that the granularity of the system
	 * calls can't go finer than words make this the easy case.
	 */
	if (infinite && limit_type == LIM_PPPFILE) {
		*new_coeffp = 2147483647L;
		*new_unitsp = (short) QLM_BYTES;
		return (1);
	}
	return (0);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
}


/*** quolimlow
 *
 *
 *	int quolimlow ():
 *
 *	Given the coefficient and the units of a memory or disk
 *	resource limit, report whether the implied limit could be
 *	enforced on this machine. In determining whether the limit
 *	is too low, we must remember that some apparently finite
 *	limit values might have been usurped by the system call in
 *	question to mean "infinity". 
 *
 *	The limit shall be deemed to be too small if it is
 *	a small number and attempting to enforce it would
 *	result in an infinite limit, or would cause a batch request
 *	to fail in a way inscrutable to the user.
 *
 *	If the limit is ok, return 0. If the limit is too small,
 *	return 1 and assign the smallest enforceable limit
 *	to the designated variables.
 *
 *	We assume that the actual parameter "coeff" < 2**31.
 *	This is in line with scanquolim().
 */
int quolimlow (coeff, units, limit_type, new_coeffp, new_unitsp)
unsigned long coeff;			/* Coefficient of limit in question */
short units;				/* Units of limit in question */
int limit_type;				/* One of LIM_??? */
unsigned long *new_coeffp;		/* Pointer to altered coefficient */
short *new_unitsp;			/* Pointer to altered units */
{
	if (limit_type == LIM_PPPFILE) {
		/*
		 * Ulimit(2) claims to support the setting of values
		 * as low as zero bytes.  Here, however, we prevent
		 * the setting of any limit lower than 2048 bytes.
		 * The child of the shepherd process writes to the
		 * shepherd AFTER the call to ulimit().  A ulimit
		 * of less than 2048 bytes might result in the
		 * shepherd failing to get a completion message,
		 * or mail failing to get through.
		 */
		switch (units) {
		case QLM_BYTES:
			if (coeff < 2048) {
				*new_coeffp = 2048;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		case QLM_WORDS:
			if ((coeff * BYTES_PER_WORD) < 2048) {
				*new_coeffp = 2048;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		case QLM_KBYTES:
			if (coeff < 2) {
				*new_coeffp = 2048;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		case QLM_KWORDS:
			if ((coeff * BYTES_PER_WORD) < 2) {
				*new_coeffp = 2048;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		case QLM_MBYTES:
		case QLM_MWORDS:
		case QLM_GBYTES:
		case QLM_GWORDS:
			if (coeff == 0) {
				*new_coeffp = 2048;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		}
	}
	/*
	 *  Below, we handle limits OTHER than LIM_PPPFILE.
	 */
#if	BSD42 | BSD43 | ULTRIX | SGI | SYS52 | UTS | OSF
	/*
	 *  No problem of zero meaning infinity here.
	 */
	return (0);
#else
#if	UNICOS
	if (limit_type == LIM_PPMEM || limit_type == LIM_PRMEM) {
		/*
		 * The limit "0" cannot be enforced.  Change limits
		 * that would be called as "0" to 1.
		 */
		switch (units) {
		case QLM_BYTES:
			if (coeff / MEM_LIMIT_GRAN < 1) {
				*new_coeffp = 1 * MEM_LIMIT_GRAN;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		case QLM_WORDS:
			if ((coeff * BYTES_PER_WORD) / MEM_LIMIT_GRAN < 1) {
				*new_coeffp = 1 * MEM_LIMIT_GRAN;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		case QLM_KBYTES:
			if ((coeff * 1024) / MEM_LIMIT_GRAN < 1) {
				*new_coeffp = 1 * MEM_LIMIT_GRAN;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		case QLM_KWORDS:
			if ((coeff * BYTES_PER_WORD * 1024) / MEM_LIMIT_GRAN <
				1) {
				*new_coeffp = 1 * MEM_LIMIT_GRAN;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		case QLM_MBYTES:
		case QLM_MWORDS:
		case QLM_GBYTES:
		case QLM_GWORDS:
			if (coeff == 0) {
				*new_coeffp = 1 * MEM_LIMIT_GRAN;
				*new_unitsp = QLM_BYTES;
				return (1);
			}
			return (0);
		}
	}
#else
BAD SYSTEM TYPE
#endif
#endif
}
