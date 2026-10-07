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
