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
 * $Log: login.h,v $
 * Revision 1.3  1994/11/18  21:09:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:57  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:26  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:05  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:36:20  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:29:18  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:48:08  gm]
 * 
 * Revision 1.2  90/03/13  21:22:36  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:22:27  tom]
 * 
 * $EndLog$
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)login.h	1.4  com/inc,3.1,8943 10/2/89 23:21:44 */
/*
 * COMPONENT_NAME: (login.h) 
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#ifndef _LOGIN_H_    
#define _LOGIN_H_
/* prolog */

/* modes for checking the user account and identification */
#define	S_LOGIN		0x1		/* check for logins */
#define S_RLOGIN	0x2		/* check for remote logins */
#define	S_SU		0x4		/* check for su */
#define S_TSM		0x8		/* check for Terminal state manager */
#define S_DAEMON	0x16		/* check for allowing batch programs */

#define	S_PRIMARY	0x1		/* primary authentication requested */
#define	S_SECONDARY     0x2		/* secondary authentication requested */


#include <lastlog.h>			/* for the berkeley lastlog struct */
					/* and LL_LINELEN and LL_HOSTLEN   */

#endif /* _LOGIN_H_ */
