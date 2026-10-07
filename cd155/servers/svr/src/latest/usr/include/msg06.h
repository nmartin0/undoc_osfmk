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
 * $Log: msg06.h,v $
 * Revision 1.3  1994/11/18  21:10:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:35  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:09  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:37:55  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:32:13  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:52:30  gm]
 * 
 * Revision 1.2  90/03/13  21:23:04  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:25:18  tom]
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
/* @(#)msg06.h	1.4  com/inc,3.1,8943 3/4/89 15:22:10 */
/* msg06.h	5.1 - 86/12/09 - 05:59:23 */
/* msg06.h	5.1 86/12/09 05:59:23 */
#ifndef  _MSG06_H_
#define  _MSG06_H_

/***********************************************************************
*  msg06.h - Message Services External Variable Declarations           *
***********************************************************************/

    extern int   msgvi1;           /* integer variable 1              */
    extern int   msgvi2;           /* integer variable 2              */
    extern long  msgvl1;           /* long integer variable 1         */
    extern long  msgvl2;           /* long integer variable 2         */
    extern char  *msgvc1;          /* pointer to variable string 1    */
    extern char  *msgvc2;          /* pointer to variable string 2    */
    extern char  *msgvc3;          /* pointer to variable string 3    */
    extern int   msgvt1;           /* index to variable text insert 1 */
    extern int   msgvt2;           /* index to variable text insert 2 */
    extern int   msgvt3;           /* index to variable text insert 3 */
    extern char  *msgpath;         /* pointer to optional path prefix */

#endif /* _MSG06_H_ */
