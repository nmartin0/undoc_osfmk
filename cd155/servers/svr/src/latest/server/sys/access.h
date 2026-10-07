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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)access.h	1.10  com/inc/sys,3.1,8943 10/11/89 17:21:52 */
/*
 * COMPONENT_NAME: SYSLFS - Logical File System
 *
 * ORIGINS: 27, 3
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * HISTORY
 * $Log: access.h,v $
 * Revision 1.4  1994/11/18  20:39:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:23:25  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:14  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:38:53  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:27  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:01:51  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:41:24  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:48:36  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:36:24  gm]
 * 
 * Revision 1.2  90/03/13  20:48:57  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/09  14:39:58  tom]
 * 
 * $EndLog$
 */

#ifndef _SYS_ACCESS_H_
#define _SYS_ACCESS_H_

#include <standards.h>

/* POSIX does not define access.h, however, certain values in access.h
 * are required to be included by unistd.h when _POSIX_SOURCE is defined.
 * Therefore, these values are confined within POSIX ifdefs.
 */

#ifdef _POSIX_SOURCE
/*
 *  BSD defines
 */
#define	F_OK	00		/* E_ACC does file exist */
#define	X_OK	01		/* X_ACC is it executable by caller */
#define	W_OK	02		/* W_ACC writable by caller */
#define	R_OK	04		/* R_ACC readable by caller */

#endif /* _POSIX_SOURCE */

#ifdef _OSF_SOURCE

#define R_ACC	04	/* read */
#define W_ACC	02	/* write */
#define X_ACC	01	/* execute (search) */
#define E_ACC	00	/* check existence of file */
#define NO_ACC	00	/* no access rights */

#endif /* _OSF_SOURCE */
#endif /* _SYS_ACCESS_H_ */
