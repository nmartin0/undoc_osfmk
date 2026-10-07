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
 * $Log: wchar.h,v $
 * Revision 1.3  1994/11/18  21:11:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:57  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:50  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:22:06  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:40:29  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/07  20:39:15  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:03:03  gm]
 * 
 * Revision 1.3  90/04/27  23:08:18  devrcs
 * 	Updated to latest AIX code.
 * 	[90/04/16  13:50:31  tom]
 * 
 * Revision 1.2  90/03/13  21:24:12  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:41:31  tom]
 * 
 * $EndLog$
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
