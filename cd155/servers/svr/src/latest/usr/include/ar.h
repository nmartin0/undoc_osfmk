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
/* @(#)ar.h	1.8  com/inc,3.1,8943 10/26/89 10:38:09 */
/* ar.h	5.1 - 86/12/09 - 06:03:39 */
/*
 * COMPONENT_NAME: CMDAR
 *
 * FUNCTIONS: none
 *
 * ORIGINS: 27, 3
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * HISTORY
 * $Log: ar.h,v $
 * Revision 1.3  1994/11/18  21:09:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:15  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:41  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:14:51  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:27  nandy
 * Initial revision
 *
 * Revision 2.4  92/04/13  14:34:30  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.4  90/10/07  20:25:32  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:42:32  gm]
 * 
 * Revision 2.3  90/03/13  21:21:15  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  17:47:32  tom]
 * 
 * Revision 2.2  90/01/02  19:15:04  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */


#ifndef _AR_H_
#define _AR_H_

/* NOTE: There used to be AIX specific archive information in this
         file, but it has been removed and replaced with the vanilla
         BSD structure.  If the AIX "ar" command is to be used,
         this file must be changed back.
*/

#define ARMAG   "!<arch>\n"
#define SARMAG  8

#define ARFMAG  "`\n"

struct ar_hdr {
        char    ar_name[16];
        char    ar_date[12];
        char    ar_uid[6];
        char    ar_gid[6];
        char    ar_mode[8];
        char    ar_size[10];
        char    ar_fmag[2];
};

#endif /* _AR_H_ */
