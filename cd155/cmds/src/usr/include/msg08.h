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
/* @(#)msg08.h	1.4  com/inc,3.1,8943 3/4/89 15:22:34 */
/* msg08.h	5.1 - 86/12/09 - 05:59:32 */
/* msg08.h	5.1 86/12/09 05:59:32 */
#ifndef _MSG08_H_
#define _MSG08_H_   

/************************************************************************
*  msg08.h - Structure Declarations for Message Table (used by msg07.h) *
************************************************************************/

/*** Format of message definition ***/
typedef struct {
       short helpindx;       /* help index #               */
       char sect_id[3];      /* Messages Manual section ID */
       char msg_id[3];       /* Messages Manual message ID */
       char *msgtextp;       /* ptr to message text        */
       } msg__msg;

/*** Format of text insert definition ***/
typedef char *msg__ins;

/*** Format of "Message Table" ***/
typedef struct {
       msg__msg *msgtabp;    /* ptr to message definition table        */
       msg__ins *instabp;    /* ptr to text insert definition table    */
       int  num_msg;         /* # of messages defined                  */
       int  num_ins;         /* # of text inserts defined              */
       char uniq[4];         /* unique ID ("MtAb") to verify table ptr */
       } msg__table;

#endif /* _MSG08_H_ */
