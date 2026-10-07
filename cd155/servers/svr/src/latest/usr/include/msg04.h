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
 * $Log: msg04.h,v $
 * Revision 1.3  1994/11/18  21:10:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:06  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:33  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:06  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:37:51  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:32:09  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:52:24  gm]
 * 
 * Revision 1.2  90/03/13  21:23:01  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:25:03  tom]
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
/* @(#)msg04.h	1.4  com/inc,3.1,8943 3/4/89 15:21:43 */
/* msg04.h	5.1 - 86/12/09 - 05:59:15 */
/* msg04.h	5.1 86/12/09 05:59:15 */
#ifndef  _MSG04_H_
#define  _MSG04_H_ 

/***********************************************************************
*  msg04.h - Message Services Error Return Code Definitions            *
***********************************************************************/

#define  MSG_COMP -9001    /* component file could not be located     */
#define  MSG_INVL -9002    /* component file has invalid format       */
#define  MSG_MTCH -9003    /* component file contains wrong component */
#define  MSG_NONE -9004    /* req. msg/insert/help not in comp. file  */
#define  MSG_REFN -9005    /* text reference to invalid descriptor    */
/* (Above errors cause msg # 090-001 or 090-002 to be substituted.)   */

#define  MSG_TABI -9009    /* req. msg/insert not in message table    */

#define  MSG_CPID -9010    /* component ID is not six characters long */
#define  MSG_INDX -9011    /* index is not in the range 1-999         */
#define  MSG_TABP -9012    /* MSGFLTAB flag specified with help req.  */
#define  MSG_ALLO -9013    /* unable to allocate minimum work area    */
#define  MSG_SREG -9014    /* segment reg. not avail. for file mapping*/
#define  MSG_BADP -9015    /* msg table ptr doesn't point to msg table*/
#define  MSG_EXEC -9016    /* execl to /qmsg upd pgm (msgoutq) failed */
#define  MSG_QMSG -9017    /* can't open /qmsg, or its fmt is invalid */

#endif /* _MSG04_H_ */

