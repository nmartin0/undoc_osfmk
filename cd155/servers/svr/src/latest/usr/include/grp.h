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
 * $Log: grp.h,v $
 * Revision 1.3  1994/11/18  21:09:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:34  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:59  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:39  nandy
 * Initial revision
 *
 * Revision 2.8  92/04/13  14:35:45  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.8  90/10/31  15:15:17  devrcs
 * 	Move #include <limits.h> under _OSF_SOURCE.
 * 	[90/10/06  11:58:36  rabin]
 * 
 * Revision 2.7  90/10/07  20:27:43  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:45:50  gm]
 * 
 * 	Cleanup _OSF_SOURCE in struct group.
 * 	[90/09/25  16:25:04  brezak]
 * 
 * Revision 2.6  90/09/23  16:42:53  devrcs
 * 	protected reentrant function declarations
 * 	[90/09/10  13:40:23  rabin]
 * 
 * Revision 2.5  90/08/09  14:26:18  devrcs
 * 	add correct prototypes for reentrant functions
 * 	[90/08/07  10:35:39  sp]
 * 
 * 	Changes for reentrant C library
 * 	[90/08/02  09:56:02  sp]
 * 
 * Revision 2.4  90/03/13  21:22:24  mbrown
 * 	Protect NGROUPS from being redefined.
 * 	[90/02/27  15:13:11  tom]
 * 
 * 	AIX merge first cut.
 * 	[90/02/12  17:56:43  tom]
 * 
 * Revision 2.3  90/01/02  19:15:17  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */
/* @(#)grp.h	1.7  com/inc,3.1,8943 8/8/89 15:24:07 */
/* grp.h	5.1 - 86/12/09 - 06:04:45 */
/*
 * COMPONENT_NAME: grp.h
 *                                                                    
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   

#ifndef _GRP_H_
#define _GRP_H_

#include <standards.h>
#include <sys/types.h>

/* The POSIX standard requires that certain elements be included in grp.h. 
 * It also requires that when _POSIX_SOURCE is defined, only those standard
 * specific elements are made available.
 * This header includes all the POSIX required entries.
 */

#ifdef _POSIX_SOURCE

struct  group {			 /* see getgrent(3) */
        char    *gr_name;
        char    *__gr_passwd;
        gid_t   gr_gid;
        char    **gr_mem;
};
#ifdef _OSF_SOURCE
#define gr_passwd __gr_passwd
#endif

#ifdef _NO_PROTO

extern struct group *getgrgid();
extern struct group *getgrnam();

/* _POSIX_REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
extern int getgrgid_r();
extern int getgrnam_r();
#endif

#else /* _NO_PROTO */

extern struct group *getgrgid(gid_t );
extern struct group *getgrnam(char *);

/* _POSIX_REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
extern int getgrgid_r(gid_t, struct group *, char *, int);
extern int getgrnam_r(char *, struct group *, char *, int);
#endif

#endif /* _NO_PROTO */

#endif /* _POSIX_SOURCE */

#ifdef _OSF_SOURCE

#include <limits.h>

#ifndef NGROUPS			/* could get it from param.h */
#define NGROUPS NGROUPS_MAX       /** as many as there are **/
#endif

#ifdef _NO_PROTO

extern struct group *getgrent();
extern void setgrent();
extern void endgrent();

/* _REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
extern int getgrent_r();
#endif

#else	/* _NO_PROTO */

extern struct group *getgrent(void);
extern void setgrent(void);
extern void endgrent(void);

/* _REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
extern int getgrent_r(struct group *, char *, int);
#endif

#endif /* _NO_PROTO */

#endif /* _OSF_SOURCE */

#endif /* _GRP_H_ */
 
