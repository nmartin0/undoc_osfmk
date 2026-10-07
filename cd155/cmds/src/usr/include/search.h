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
/* @(#)search.h	1.11  com/inc,3.1,9013 2/25/90 13:15:36 */
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
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
#ifndef _SEARCH_H_
#define _SEARCH_H_

#include <standards.h>
#include <sys/types.h>

#ifdef _XOPEN_SOURCE

/* HSEARCH(3C) */
typedef struct entry { char *key, *data; } ENTRY;
typedef enum { FIND, ENTER } ACTION;

/* TSEARCH(3C) */
typedef enum { preorder, postorder, endorder, leaf } VISIT;

#ifdef	_NONSTD_TYPES
extern char *lsearch();
extern char *lfind();
extern char *tsearch();
extern char *tfind();
extern char *tdelete();

#elif	defined	_NO_PROTO

extern void *lsearch();
extern void *lfind();
extern void *tsearch();
extern void *tfind();
extern void *tdelete();

#else	/* _NONSTD_TYPES, _NO_PROTO */

extern void *lsearch(void *key, void *base, size_t *nelp, size_t width, int (*compar)());
extern void *lfind(void *key, void *base, size_t *nelp, size_t width, int (*compar)());
extern void *tsearch(void *key, void **rootp, int (*compar)(void *,void *)); 
extern void *tfind(void *key, void **rootp, int (*compar)(void *,void *));
extern void *tdelete(void *key, void **rootp, int (*compar)(void *,void *));
#endif	/* _NONSTD_TYPES, _NO_PROTO */
 
#ifdef _NO_PROTO
extern int hcreate();
extern void hdestroy();
extern ENTRY *hsearch();
extern void twalk();

#else

extern int hcreate(unsigned nel);
extern void hdestroy(void);
extern ENTRY *hsearch(ENTRY item, ACTION action);
extern void twalk(void *root, void (*action)(void *,VISIT,int));

#endif /* _NO_PROTO */
#endif /* _XOPEN_SOURCE */

#endif /* _SEARCH_H_ */
