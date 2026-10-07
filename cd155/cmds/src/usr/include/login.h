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
