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
 
