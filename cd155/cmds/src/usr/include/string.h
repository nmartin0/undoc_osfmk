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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
/* @(#)string.h	1.19  com/inc,3.1,9013 2/25/90 13:16:11 */

/*
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <standards.h>

/*
 *
 *      The ANSI standard requires that certain values be in string.h.
 *      It also requires that if _ANSI_C_SOURCE is defined then ONLY these
 *      values are present. This header includes all the ANSI required entries.
 *
 */

#ifdef   _ANSI_C_SOURCE

/*
 *      The following definitions (NULL, size_t) are included in <sys/types.h>.
 *      They are also included here to comply with ANSI standards.
 */

#ifndef NULL
#define NULL    0
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long   size_t;
#endif

#ifdef	_NONSTD_TYPES
extern char	*memchr();
extern char	*memcpy();
extern char	*memset();
extern int	strcspn();
extern int	strlen();
extern int	strspn();
#elif	defined	_NO_PROTO
extern void	*memchr();
extern void	*memcpy();
extern void	*memset();
extern size_t 	strcspn();
extern size_t	strlen();
extern size_t	strspn();
#else	/* _NONSTD_TYPES, _NO_PROTO */
extern void	*memchr(const void *s, int c, size_t n);
extern void	*memcpy(void *s1, const void *s2, size_t n);
extern void	*memset(void *s, int c, size_t n);
extern size_t 	strcspn(const char *s1, const char *s2);
extern size_t	strlen(const char *s);
extern size_t	strspn(const char *s1, const char *s2);
#endif	/* _NONSTD_TYPES, _NO_PROTO */


#ifdef   _NO_PROTO
extern void 	*memmove();
extern char 	*strcpy();
extern char 	*strdup();
extern char 	*strncpy();
extern char 	*strcat();
extern char	*strncat();
extern int 	memcmp();
extern int	strcmp();
extern int	strcoll();
extern int	strncmp();
extern size_t 	strxfrm();
extern char	*strchr();
extern char	*strpbrk();
extern char	*strrchr();
extern char	*strstr();
extern char 	*strtok();
extern char 	*strerror();

extern char 	*strtok_r();			/* _POSIX_REENTRANT_FUNCTIONS */

#else  /*_NO_PROTO */
extern void 	*memmove(void *s1, const void *s2, size_t n);
extern char 	*strcpy(char *s1, const char *s2);
extern char 	*strdup(char *s);
extern char 	*strncpy(char *s1, const char *s2, size_t n);
extern char 	*strcat(char *s1, const char *s2);
extern char	*strncat(char *s1, const char *s2, size_t n);
extern int 	memcmp(const void *s1, const void *s2,size_t n);
extern int	strcmp(const char *s1, const char *s2);
extern int	strcoll(const char *s1, const char *s2);
extern int	strncmp(const char *s1,const char *s2,size_t n);
extern size_t 	strxfrm(char *s1, const char *s2, size_t n);
extern char	*strchr(const char *s, int c);
extern char	*strpbrk(const char *s1, const char *s2);
extern char	*strrchr(const char *s, int c);
extern char	*strstr(const char *s1, const char *s2);
extern char 	*strtok(char *s1, const char *s2);
extern char 	*strerror(int errnum);
extern char 	*strtok_r(char *s1, char *s2, char **savept); 
						/* _POSIX_REENTRANT_FUNCTIONS */
#endif /*_NO_PROTO */
#endif /*_ANSI_C_SOURCE */

#ifdef	_XOPEN_SOURCE

#ifdef	_NONSTD_TYPES
extern char	*memccpy();
#elif	defined	_NO_PROTO
extern void	*memccpy();
#else	/* _NONSTD_TYPES, _NO_PROTO */
extern void	*memccpy(void *target, const void *source, int c, size_t n);
#endif	/* _NONSTD_TYPES, _NO_PROTO */

#endif	/* _XOPEN_SOURCE */


#ifdef _OSF_SOURCE

#ifdef _NO_PROTO
extern int  strcasecmp();
extern int  strncasecmp();
extern char *index();
extern char *rindex();
#else
extern int  strcasecmp(const char*, const char*);
extern int  strncasecmp(const char*, const char*, int);
extern char *index(const char*, int);
extern char *rindex(const char*, int);
#endif  /* _NO_PROTO */

#endif /* _OSF_SOURCE */

#endif /* _STRING_H_ */
