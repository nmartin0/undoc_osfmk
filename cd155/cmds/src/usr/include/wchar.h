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
/* @(#)wchar.h  1.4  com/inc,3.1,9013 1/14/90 16:52:16 */
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

#ifndef _WCHAR_H_
#define _WCHAR_H_

#ifdef _KJI

#include <sys/types.h>

extern wchar_t *strtows(char *, wchar_t *);
extern char    *wstrtos(wchar_t *, char *);

#define wstrcat NCstrcat
#define wstrlen NCstrlen
#define wstrdup NCstrdup
#define wstrncat NCstrncat
#define wstrcmp NCstrcmp
#define wstrncmp NCstrncmp
#define wstrcpy NCstrcpy
#define wstrncpy NCstrncpy

#endif /* KJI */
#endif /* _WCHAR_H_ */
