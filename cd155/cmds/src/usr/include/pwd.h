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
/* @(#)pwd.h	1.11  com/inc,3.1,8943 7/14/89 08:36:10 */
/*
 * COMPONENT_NAME: pwd.h
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
/*
 * OSF/1 Release 1.0
 */

#ifndef _PWD_H_
#define _PWD_H_

#include <standards.h>
#include <sys/types.h>

/* The POSIX standard requires that certain elements be included in pwd.h. 
 * It also requires that when _POSIX_SOURCE is defined, only those standard
 * specific elements are made available.
 * This header includes all the POSIX required entries.
 */

#ifdef _POSIX_SOURCE

#ifdef _NO_PROTO

	extern struct passwd *getpwuid();
	extern struct passwd *getpwnam();

/* _POSIX_REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
	extern int getpwuid_r();
	extern int getpwnam_r();
#endif

#else /* _NO_PROTO */

	extern struct passwd *getpwuid(uid_t );
	extern struct passwd *getpwnam(char *);

/* _POSIX_REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
	extern int getpwuid_r(uid_t, struct passwd *, char *, int);
	extern int getpwnam_r(char *, struct passwd *, char *, int);
#endif

#endif /* _NO_PROTO */

struct passwd {
	char	*pw_name;
	char	*__pw_passwd;
	uid_t	pw_uid;
	gid_t	pw_gid;
        int     __pw_quota;
        char    *__pw_comment;
	char    *__pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

#ifdef _OSF_SOURCE
#define pw_passwd __pw_passwd
#define pw_quota __pw_quota
#define pw_comment __pw_comment
#define pw_gecos __pw_gecos
#endif /* _OSF_SOURCE */
#endif /* _POSIX_SOURCE */

#ifdef _OSF_SOURCE 

#define pw_etc pw_gecos

#ifdef _NO_PROTO

extern struct passwd *getpwent();
extern void setpwent();
extern void endpwent();
extern void setpwfile();

/* _POSIX_REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
extern int  getpwent_r();
#endif

#else /* _NO_PROTO */

extern struct passwd *getpwent (void);
extern void setpwent (void);
extern void endpwent (void);
extern void setpwfile (const char*);

/* _POSIX_REENTRANT_FUNCTIONS */
#if defined(_THREAD_SAFE) || defined(_REENTRANT)
extern int  getpwent_r(struct passwd *, char *, int);
#endif

#endif  /* _NO_PROTO */

#endif /* _OSF_SOURCE */ 

#endif /* _PWD_H_ */
