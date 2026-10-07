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
 * $Log: msg08.h,v $
 * Revision 1.3  1994/11/18  21:10:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:10  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:38  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:12  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:37:59  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:32:17  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:52:36  gm]
 * 
 * Revision 1.2  90/03/13  21:23:06  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:25:26  tom]
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
