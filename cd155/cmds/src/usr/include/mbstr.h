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
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)mbstr.h	1.3  com/inc,3.1,8943 7/11/89 18:19:06 */
/*
 * NAME: mbstr.h
 */                                                                   

#ifndef _MBSTR_H_
#define _MBSTR_H_

#include <sys/types.h>

#define MBMAXLEN	2

#ifdef   _NO_PROTO
extern size_t mbslen();
extern char *mbscat();
extern char *mbsncat();
extern int mbscmp();
extern int mbsncmp();
extern char *mbscpy();
extern char *mbsncpy();
extern char *mbschr();
extern char *mbsrchr();
extern char *mbspbrk();
extern int mbstoint();
#else  /*_NO_PROTO */
extern size_t mbslen(const char *);
extern char *mbscat(char *, char *);
extern char *mbsncat(char *, const char *, size_t);
extern int mbscmp(char *, char *);
extern int mbsncmp(char *, char *, size_t);
extern char *mbscpy(char *, char *);
extern char *mbsncpy(char *, char *, size_t);
extern char *mbschr(const char *, const int);
extern char *mbsrchr(char *, int);
extern char *mbspbrk(char *, char *);
extern int mbstoint(char *);
#endif /*_NO_PROTO */

#endif  /* _MBSTR_H_ */
