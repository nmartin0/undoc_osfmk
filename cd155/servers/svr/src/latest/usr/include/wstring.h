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
 * $Log: wstring.h,v $
 * Revision 1.3  1994/11/18  21:11:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:18:03  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:56  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:58  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:22:12  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:40:36  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/07  20:39:24  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:03:16  gm]
 * 
 * Revision 1.3  90/04/27  23:08:22  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:50:36  tom]
 * 
 * Revision 1.2  90/03/13  21:24:16  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:42:27  tom]
 * 
 * $EndLog$
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
