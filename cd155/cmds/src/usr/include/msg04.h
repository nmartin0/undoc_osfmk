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

