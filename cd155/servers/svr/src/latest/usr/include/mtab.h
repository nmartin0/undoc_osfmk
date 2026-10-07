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
 * $Log: mtab.h,v $
 * Revision 1.3  1994/11/18  21:10:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:13  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:41  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:15  nandy
 * Initial revision
 *
 * Revision 2.3  92/04/13  14:38:02  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.3  90/10/07  20:32:21  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:52:42  gm]
 * 
 * Revision 2.2  90/01/02  19:15:20  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)mtab.h	5.1 (Berkeley) 5/30/85
 */

/*
 * Mounted device accounting file.
 */
struct mtab {
	char	m_path[32];		/* mounted on pathname */
	char	m_dname[32];		/* block device pathname */
	char	m_type[4];		/* read-only, quotas */
};
