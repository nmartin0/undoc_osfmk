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
 * $Log: ulimit.h,v $
 * Revision 1.3  1994/11/18  21:11:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:27  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:22  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:47  nandy
 * Initial revision
 *
 * Revision 1.5  92/04/13  14:40:02  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.5.4.2  91/06/13  10:17:27  hankin
 * 	Added UL_GETNOFILES define for ulimit.
 * 	[91/06/10  16:11:28  hankin]
 * 
 * Revision 1.5  90/10/07  20:38:44  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:02:17  gm]
 * 
 * Revision 1.4  90/09/13  12:32:42  devrcs
 * 	Put prototype back in. Remove include of types.h.
 * 	[90/08/30  09:44:48  tom]
 * 
 * Revision 1.3  90/08/24  13:53:41  devrcs
 * 	fix prototype for XPG3
 * 	[90/08/09  13:12:14  mbrown]
 * 
 * 	add UL_GETBREAK for max break size
 * 	[90/08/08  10:35:01  mbrown]
 * 
 * Revision 1.2  90/03/13  21:23:56  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:38:42  tom]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)ulimit.h	1.9  com/inc,3.1,8943 9/15/89 10:02:22 */
/*
 * COMPONENT_NAME: (LIBCSYS) Standard C Library System Functions
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

#ifndef _ULIMIT_H_
#define _ULIMIT_H_

#include <standards.h>

#ifdef _XOPEN_SOURCE

#ifdef _NO_PROTO
extern long ulimit();
#else
extern long ulimit(int, ...);
#endif

#define UL_GETFSIZE	1	/* get max file size limit */
#define UL_SETFSIZE	2	/* set max file size limit */

#endif /* _XOPEN_SOURCE */

#ifdef _OSF_SOURCE
#define UL_GETBREAK	3	/* get max break (data space) limit */
#define UL_GETNOFILES	4	/* get max break (data space) limit */
#endif /* _OSF_SOURCE */


#endif /* _ULIMIT_H_ */ 
