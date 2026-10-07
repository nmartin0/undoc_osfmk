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
 * $Log: s5fblk.h,v $
 * Revision 1.5  1994/11/18  20:38:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:17:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:11:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:28:26  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:37:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:35:34  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:33:30  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:31:30  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:55:23  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/06/27  15:39:16  sp
 * Incorporated from 1.0.2
 * 
 * Revision 1.3  90/10/07  14:40:37  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:24:48  gm]
 * 
 * Revision 1.2  90/04/27  19:20:49  devrcs
 * 	Prevent multiple includes of this file.
 * 	[90/04/17  14:52:07  morris]
 * 
 * 	Initial version from system V.2.
 * 	[90/04/02  12:38:46  morris]
 * 
 * $EndLog$
 */
#ifndef _S5_FBLK_H_
#define _S5_FBLK_H_

/* @(#)s5fblk.h	6.1 */
struct	fblk
{
	int	df_nfree;
	daddr_t	df_free[NICFREE];
};
#endif  _S5_FBLK_H_
