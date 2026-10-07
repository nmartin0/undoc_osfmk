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
 * $Log: msg01.h,v $
 * Revision 1.3  1994/11/18  21:10:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:30  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:03  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:37:46  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:32:05  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:52:17  gm]
 * 
 * Revision 1.2  90/03/13  21:22:59  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:24:53  tom]
 * 
 * $EndLog$
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
/* @(#)msg01.h	1.4  com/inc,3.1,8943 3/4/89 15:21:10 */
/* msg01.h	5.1 - 86/12/09 - 05:59:02 */
/* msg01.h	5.1 86/12/09 05:59:02 */
#ifndef  _MSG01_H_
#define _MSG01_H_

/***********************************************************************
*  msg01.h - Message Services Argument Flag Bit Definitions            *
***********************************************************************/

#define  MSGFLTIM 0x0001    /* output time with the message           */
#define  MSGFLSEV 0x0002    /* output severity code with the message  */
#define  MSGFLERR 0x0004    /* output error code with the message     */
#define  MSGFLFIL 0x0008    /* output to specified file               */
#define  MSGFLMSG 0x0010    /* retrieve msg text (instead of insert)  */
#define  MSGFLHLP 0x0020    /* retrieve help text (instead of insert) */
#define  MSGFLTAB 0x0040    /* msg table ptr instead of comp ID ptr   */

#endif /* _MSG01_H_ */
