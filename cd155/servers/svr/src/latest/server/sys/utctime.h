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
 *			Copyright (c) 1990 by				*
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
 *   The information in this software is subject to change  without	*
 *   notice  and should not be construed as a commitment by Digital	*
 *   Equipment Corporation.						*
 *									*
 *   Digital assumes no responsibility for the use  or  reliability	*
 *   of its software on equipment which is not supplied by Digital.	*
 *									*
 ************************************************************************/
/*
 * HISTORY
 * $Log: utctime.h,v $
 * Revision 1.4  1994/11/18  20:42:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:30:50  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:43:08  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:15  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/12/16  12:50:49  roy
 * 	Initial check-in.
 *
 * Revision 3.0  91/10/17  15:44:06  condict
 * Original version from OSF/1 Release 1.0.2.
 * 
 * Revision 1.4  90/10/07  14:56:34  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:48:55  gm]
 * 
 * Revision 1.3  90/08/24  12:28:03  devrcs
 * 	unsinged -> unsigned in big endian declarations
 * 	[90/08/21  15:47:08  sharpe]
 * 
 * Revision 1.2  90/08/09  13:29:25  devrcs
 * 	New file.
 * 
 * 	<<<replace with log message for ./kernel/sys/utctime.h>>>
 * 	[90/07/31  17:46:59  brezak]
 * 
 * $EndLog$
 */

#ifndef _UTCTIME_H
#define _UTCTIME_H

#ifndef BYTE_ORDER
#include <machine/endian.h>
#endif

/*
 * Structure definitions for utctime used by DECdts
 */

#if BYTE_ORDER == BIG_ENDIAN
struct int64 {
	unsigned long hi;
	unsigned long lo;
};
#else /* BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == PDP_ENDIAN */
struct int64 {
	unsigned long lo;
	unsigned long hi;
};
#endif 

/*
 * Define the utc timestamp structure - This is defined to be in the
 * natural byte order for this endian processor. See the comments in
 * the DTSS API (Application Programmer Interface). Note for PDP-11's
 * straight Little Endian is used! This is somewhat unnatural on the PDP's
 * but since there are only two form of the timestamp permitted, it must
 * be converted into one of the two forms. (Besides is anyone really
 * interested in a PDP?)
 * This structure MUSt be 128 bits long.
 */

#if BYTE_ORDER == LITTLE_ENDIAN
struct utc {
	struct int64	time;
	unsigned long int inacclo;
	unsigned short int inacchi;
	int tdf : 12;
	unsigned vers : 3;
	int big_endian : 1;	/* This field must be the most
				   significant bit of the last byte */
};
#endif
#if BYTE_ORDER == BIG_ENDIAN
struct utc {
	struct int64	time;
	unsigned short int inacchi;
	unsigned short int inaccmid;
	unsigned short int inacclo;
	char tdfhi;
	int big_endian : 1;	/* This field must be the most
				   significant bit of the last byte */
	unsigned vers : 3;
	unsigned int tdflo : 4;
};
#endif
#if BYTE_ORDER == PDP_ENDIAN
struct utc {
	unsigned short int timelolo;
	unsigned short int timelohi;
	unsigned short int timehilo;
	unsigned short int timehihi;
	unsigned short int inacclo;
	unsigned short int inaccmid;
	unsigned short int inacchi;
	int tdf : 12;
	unsigned vers : 3;
	int big_endian : 1;	/* This field must be the most
				   significant bit of the last byte */
};
#endif

/* For a 64-bit little endian:
struct utc {
	long int time;
	long int inacc : 48;
	int tdf : 12;
	unsigned vers : 3;
	int big_endian : 1;	   This field must be the most
				   significant bit of the last byte
}; */
/* For a 64-bit big endian?
struct utc {
	long int time;
	int big_endian : 1;	   This field must be the most
				   significant bit of the last byte
	unsigned vers : 3;
	unsigned int tdflo : 4;
	int tdfhi : 8;
	long int inacc : 48;
}; */

/*
 * Define random other constants
 */
#define K_UTC_VERSION	(1)
#define K_100NS_PER_SEC (10000000)
#define K_US_PER_SEC    (1000000)
#define K_NS_PER_SEC    (1000000000)

enum adjmode {
	settime,  	/* Set time */
	adjusttime,	/* Adjust time */
	endadjust,	/* End adjust time */
	getresolution,  /* Get resolution of clock */
	getdrift,       /* Get drift reciprocal */
	setfreq,	/* Set (tweek) the clock frequency */
	getfreq,	/* Get the clock frequency */
	lastmode	/* Used as limit test */
};

struct timespec {
	long int	ts_sec;
	long int	ts_nsec;
};

union adjunion {
    struct adjargs {
	struct timespec	a_adjustment; 	/* Amount to adjust or change */
	struct timespec	a_comptime;	/* Time which corresponds to base
					   inaccuracy */
	struct int64	a_baseinacc;	/* Base inaccuracy */
	struct int64	a_leaptime;	/* Time of next potential leap
					   second */
	long int	a_adjrate;	/* Rate at which to adjust
						1000 = .1% (.0768% on PMAX)
						100 = 1% (.999% on PMAX)
						10 = 10%, etc.
					   Ignored for set time */
	long int	a_curtdf;	/* Current timezone */
	long int	a_nextdf;	/* Next timezone (eg. Daylight time) */
	long int	a_tdftime;	/* Time of next timezone change */
    } adjargs;				/* Adustment args */
    long int		resolution;	/* Resolution of clock in nanosecs */
    unsigned long int	*maxdrift;	/* Maximun drift rate of clock */
    struct trimargs {
	long int	t_frequency;	/* New frequency trim of clock */
	long int	t_maxdrift;	/* New maximun drift rate of clock */
    } trimargs;
    long int		frequency;	/* Current frequency trim of clock */
};

#endif /* _UTCTIME_H */
