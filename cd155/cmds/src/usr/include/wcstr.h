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
/* @(#)wcstr.h	1.4  com/inc,3.1,8943 9/25/89 12:00:39 */
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wcstr.h
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

#ifndef _WCSTR_H_
#define _WCSTR_H_

#include <sys/types.h>

#define MBMAXLEN	2

#ifdef   _NO_PROTO
extern wchar_t *wcscat();
extern wchar_t *wcsncat();
extern int wcscmp();
extern int wcsncmp();
extern wchar_t *wcscpy();
extern wchar_t *wcsncpy();
extern wchar_t *wcschr();
extern wchar_t *wcsrchr();
extern size_t wcslen();
extern wchar_t *wcspbrk();
extern size_t wcsspn();
extern size_t wcscspn();
extern wchar_t *wcswcs();
#else  /*_NO_PROTO */
extern wchar_t *wcscat(wchar_t *, wchar_t *);
extern wchar_t *wcsncat(wchar_t *, wchar_t *, size_t);
extern int wcscmp(wchar_t *, wchar_t *);
extern int wcsncmp(wchar_t *, wchar_t *, size_t);
extern wchar_t *wcscpy(wchar_t *, wchar_t *);
extern wchar_t *wcsncpy(wchar_t *, wchar_t *, size_t);
extern size_t wcslen(wchar_t *);
extern wchar_t *wcschr(wchar_t *, wchar_t);
extern wchar_t *wcsrchr(wchar_t *, wchar_t);
extern wchar_t *wcspbrk(wchar_t *, wchar_t *);
extern size_t wcsspn(wchar_t *, wchar_t *);
extern size_t wcscspn(wchar_t *, wchar_t *);
extern wchar_t *wcswcs(wchar_t *, wchar_t *);
#endif /*_NO_PROTO */
#endif /* _WCSTR_H_ */
