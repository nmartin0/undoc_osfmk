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
 * $Log: strstat.h,v $
 * Revision 1.4  1994/11/18  20:41:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:28:30  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:42  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:50  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:51  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/17  11:19:08  sjs
 * fixed RCS messages
 *
 * Revision 2.2  91/10/14  13:09:24  sjs
 * Check in
 * 
 * Revision 3.0  91/09/27  11:52:40  emcmanus
 * Installed for streams.
 * 
 * Revision 1.6  90/10/31  14:07:11  devrcs
 * 	Post-file-rearrangement pathnames.
 * 	[90/10/11  08:34:42  tmt]
 * 
 * 	Checkin prior to file rearrangement.
 * 	[90/10/08  13:41:02  tmt]
 * 
 * Revision 1.5  90/10/07  14:44:54  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:31:16  gm]
 * 
 * Revision 1.4  90/02/23  00:31:16  devrcs
 * 	No changes.
 * 	[90/02/18  19:22:18  nolting]
 * 
 * Revision 1.3  90/01/02  20:22:53  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:28:32  gm
 * 	Added streams support.
 * 	[89/12/25            gm]
 * 
 * $EndLog$
 */
/** Copyright (c) 1989  Mentat Inc.
 ** strstat.h 1.1, last change 8/8/89
 **/

#ifndef	_SRTSTAT_H
#define	_SRTSTAT_H

/* module statistics structure */
struct	module_stat {
	long	ms_pcnt;	/* count of calls to put proc */
	long	ms_scnt;	/* count of calls to service proc */
	long	ms_ocnt;	/* count of calls to open proc */
	long	ms_ccnt;	/* count of calls to close proc */
	long	ms_acnt;	/* count of calls to admin proc */
	char	* ms_xptr;	/* pointer to private statistics */
	short	ms_xsize;	/* length of private statistics buffer */
};
#endif
