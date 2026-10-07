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
/*
 * HISTORY
 * $Log: gmon.h,v $
 * Revision 1.3  1994/11/18  21:09:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:24  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:32  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:56  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:36  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:35:41  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:27:39  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:45:44  gm]
 * 
 * Revision 1.2  90/03/13  21:22:22  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:21:25  tom]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)gmon.h       1.1  com/inc,3.1,8943 10/13/89 16:05:27 */
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: gmon.h
 *
 * ORIGINS: 26
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ifndef _GMON_H_
#define _GMON_H_

/*
 *      @(#)gmon.h      5.1 (Berkeley) 5/30/85
 */

struct phdr {
    char        *lpc;
    char        *hpc;
    int         ncnt;
    caddr_t     *monitor_load_address;
};

    /*
     *  histogram counters are unsigned shorts (according to the kernel).
     */
#define HISTCOUNTER     unsigned short

    /*
     *  fraction of text space to allocate for histogram counters
     *  here, 1/2
     */
#define HISTFRACTION    2

    /*
     *  Fraction of text space to allocate for from hash buckets.
     *  The value of HASHFRACTION is based on the minimum number of bytes
     *  of separation between two subroutine call points in the object code.
     *  Given MIN_SUBR_SEPARATION bytes of separation the value of
     *  HASHFRACTION is calculated as:
     *
     *          HASHFRACTION = MIN_SUBR_SEPARATION / (2 * sizeof(short) - 1);
     *
     *  For the VAX, the shortest two call sequence is:
     *
     *          calls   $0,(r0)
     *          calls   $0,(r0)
     *
     *  which is separated by only three bytes, thus HASHFRACTION is
     *  calculated as:
     *
     *          HASHFRACTION = 3 / (2 * 2 - 1) = 1
     *
     *  Note that the division above rounds down, thus if MIN_SUBR_FRACTION
     *  is less than three, this algorithm will not work!
     */
#define HASHFRACTION    1

    /*
     *  percent of text space to allocate for tostructs
     *  with a minimum.
     */
#define ARCDENSITY      2
#define MINARCS         50

struct tostruct {
    char                *selfpc;
    long                count;
    unsigned short      link;
};

    /*
     *  a raw arc,
     *      with pointers to the calling site and the called site
     *      and a count.
     */
struct rawarc {
    unsigned long       raw_frompc;
    unsigned long       raw_selfpc;
    long                raw_count;
};

    /*
     *  general rounding functions.
     */
#define ROUNDDOWN(x,y)  (((x)/(y))*(y))
#define ROUNDUP(x,y)    ((((x)+(y)-1)/(y))*(y))

#endif /* _GMON_H_ */
