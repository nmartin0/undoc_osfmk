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
/* @(#)wstring.h        1.5  com/inc,3.1,9013 1/14/90 16:52:28 */
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _WSTRING_H_
#define _WSTRING_H_

#include <wchar.h>

extern double	 wstrtod(wchar_t *, wchar_t **);
extern double	 watof(wchar_t *);
extern int	 watoi(wchar_t *);
extern long	 watol(wchar_t *);
extern long	 wstrtol(wchar_t *, wchar_t **, int);

#ifdef _KJI
extern wchar_t
	*wstrdup(wchar_t *),
	*wstrcpy(wchar_t *, wchar_t *),
	*wstrncpy(wchar_t *, wchar_t *, int),
	*wstrcat(wchar_t *, wchar_t *),
	*wstrncat(wchar_t *, wchar_t *, int),
	*wstrchr(wchar_t *, int),
	*wstrrchr(wchar_t *, int),
	*wstrpbrk(wchar_t *, wchar_t *),
	*wstrtok(wchar_t *, wchar_t *);

extern size_t
	wstrlen(wchar_t *),
	wstrspn(wchar_t *, wchar_t *),
	wstrcspn(wchar_t *, wchar_t *);

extern int
	wstrcmp(wchar_t *, wchar_t *),
	wstrncmp(wchar_t *, wchar_t *, int);
#endif /* _KJI */

#endif /* _WSTRING_H_ */
