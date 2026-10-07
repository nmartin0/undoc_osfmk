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
 * $Log: unctrl.h,v $
 * Revision 1.3  1994/11/18  21:11:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:37  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:32  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:28  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:50  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:40:06  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:38:48  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:02:23  gm]
 * 
 * Revision 1.2  90/03/13  21:23:58  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:38:58  tom]
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
/* @(#)unctrl.h	1.4  com/inc,3.1,8943 3/4/89 15:32:17 */
/* unctrl.h	5.1 - 86/12/09 - 06:06:38 */
/*
 * unctrl.h
 *
*/
#ifndef _UNCTRL_H_
#define _UNCTRL_H_

#ifndef unctrl
extern char	*_unctrl[];

# define	unctrl(ch)	(_unctrl[(unsigned) ch])
#endif

#endif /* _UNCTRL_H_ */
