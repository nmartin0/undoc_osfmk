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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)langinfo.h	1.4  com/inc,3.1,8943 9/15/89 09:58:11 */
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#ifndef _LANGINFO_H_
#define _LANGINFO_H_

#include <standards.h>
#include <nl_types.h>

/*
   X/Open compatible langinfo.h file used by nl_langinfo(3).

   The items referenced here may be resolved in one of three ways:

   1. value or sub-value of an environment variable

   2. value or sub-value of an NLFILE item

   3. value or sub-value of a default item
*/

#define D_T_FMT   0x0001 /* string for formatting date and time */
#define D_FMT     0x0002 /* string for formatting date */
#define T_FMT     0x0003 /* string for formatting time */
#define AM_STR    0x0004 /* string for a.m. */
#define PM_STR    0x0005 /* string for p.m. */
#define DAY_1     0x0101 /* name of the first day of the week (Sunday) */
#define DAY_2     0x0102 /* name of the second day of the week (Monday) */
#define DAY_3     0x0103 /* name of the third day of the week (Tuesday) */
#define DAY_4     0x0104 /* name of the fourth day of the week (Wednesday) */
#define DAY_5     0x0105 /* name of the fifth day of the week (Thursday) */
#define DAY_6     0x0106 /* name of the sixth day of the week (Friday) */
#define DAY_7     0x0107 /* name of the seventh day of the week (Saturday) */
#define ABDAY_1   0x0201 /* abbreviated first day of the week (Sun) */
#define ABDAY_2   0x0202 /* abbreviated second day of the week (Mon) */
#define ABDAY_3   0x0203 /* abbreviated third day of the week (Tue) */
#define ABDAY_4   0x0204 /* abbreviated fourth day of the week (Wed) */
#define ABDAY_5   0x0205 /* abbreviated fifth day of the week (Thu) */
#define ABDAY_6   0x0206 /* abbreviated sixth day of the week (Fri) */
#define ABDAY_7   0x0207 /* abbreviated seventh day of the week (Sat) */
#define MON_1     0x0301 /* name of the first month (January) */
#define MON_2     0x0302 /* name of the second month (February) */
#define MON_3     0x0303 /* name of the third month (March) */
#define MON_4     0x0304 /* name of the fourth month (April) */
#define MON_5     0x0305 /* name of the fifth month (May) */
#define MON_6     0x0306 /* name of the sixth month (June) */
#define MON_7     0x0307 /* name of the seventh month (July) */
#define MON_8     0x0308 /* name of the eighth month (August) */
#define MON_9     0x0309 /* name of the ninth month (September) */
#define MON_10    0x030a /* name of the tenth month (October) */
#define MON_11    0x030b /* name of the eleventh month (November) */
#define MON_12    0x030c /* name of the twelveth month (December) */
#define ABMON_1   0x0401 /* abbreviated first month (Jan) */
#define ABMON_2   0x0402 /* abbreviated second month (Feb) */
#define ABMON_3   0x0403 /* abbreviated third month (Mar) */
#define ABMON_4   0x0404 /* abbreviated fourth month (Apr) */
#define ABMON_5   0x0405 /* abbreviated fifth month (May) */
#define ABMON_6   0x0406 /* abbreviated sixth month (Jun) */
#define ABMON_7   0x0407 /* abbreviated seventh month (Jul) */
#define ABMON_8   0x0408 /* abbreviated eighth month (Aug) */
#define ABMON_9   0x0409 /* abbreviated ninth month (Sep) */
#define ABMON_10  0x040a /* abbreviated tenth month (Oct) */
#define ABMON_11  0x040b /* abbreviated eleventh month (Nov) */
#define ABMON_12  0x040c /* abbreviated twelveth month (Dec) */
#define RADIXCHAR 0x0501 /* radix character */
#define THOUSEP   0x0601 /* separator for thousands */
#define YESSTR    0x0701 /* affiramitive response for yes/no queries */
#define NOSTR     0x0801 /* negative response for yes/no queries */
#define CRNCYSTR  0x0901 /* currency symbol; - leading, + trailing */

#ifdef _NO_PROTO
extern char *nl_langinfo();
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern int nl_langinfo_r();
#endif
#else /* _NO_PROTO */
extern char *nl_langinfo(nl_item );
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
extern int nl_langinfo_r(nl_item, char* , int);
#endif
#endif /* _NO_PROTO */

#endif /* _LANGINFO_H_ */
