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
 * $Log: timod.h,v $
 * Revision 1.5  1994/11/18  20:39:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:22:25  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:19:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:08:59  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:39:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:37:30  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:36:29  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:40:19  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1991/10/17  11:12:32  sjs
 * fixed RCS comments
 *
 * Revision 2.2  91/10/14  12:59:04  sjs
 * Check in
 * 
 * Revision 3.0  91/09/27  11:51:49  emcmanus
 * Installed for streams.
 * 
 * Revision 1.8  90/10/31  14:05:33  devrcs
 * 	Post-file-rearrangement pathnames.
 * 	[90/10/11  08:31:20  tmt]
 * 
 * 	Checkin prior to file rearrangement.
 * 	[90/10/08  13:41:34  tmt]
 * 
 * Revision 1.7  90/10/07  14:45:04  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:31:30  gm]
 * 
 * Revision 1.6  90/06/29  13:38:29  devrcs
 * 	No changes.
 * 	[90/06/26  21:30:23  gm]
 * 
 * Revision 1.5  90/06/22  20:49:39  devrcs
 * 	No changes.
 * 
 * Revision 1.4  90/05/24  23:05:15  devrcs
 * 	Changed _TIMOD_ to _TIMOD_H
 * 	[90/05/18  13:04:27  nolting]
 * 
 * Revision 1.3  90/04/27  19:23:04  devrcs
 * 	Moved the up-to-date "timod.h" from usr/include/tli to kernel/streams/h
 * 	[90/04/24  14:04:28  tung]
 * 
 * 	Added extra values for XTI
 * 	[90/04/19  12:34:20  tung]
 * 
 * Revision 1.3  90/02/27  21:16:38  devrcs
 * 	Added OSF Copyright marker.
 * 	[90/02/27  20:14:24  gm]
 * 
 * Revision 1.2  90/02/23  00:36:51  devrcs
 * 	created
 * 	[90/02/20  10:25:32  tung]
 * 
 * $EndLog$
 */

/** Copyright (c) 1989  Mentat Inc.
 ** timod.h 1.2, last change 4/14/90
 **/

#ifndef _TIMOD_H
#define	_TIMOD_H

/* Ioctl values for timod requests */
#define	TI_BIND			1
#define	TI_GETINFO		2
#define	TI_OPTMGMT		3
#define	TI_UNBIND		4

/* Extra values for XTI */
#define	TI_XTI_HELLO		5	/* This is an XTI stream, not a TLI stream */
#define	TI_XTI_GET_STATE	6	/* Retrieve state saved for XTI */
#define	TI_XTI_CLEAR_EVENT	7	/* Clear disconnect or orderly release indication */

/* Return structure for TI_XTI_GET_STATE request */
typedef struct xti_state_s {
	unsigned int	xtis_qlen;	/* Saved qlen parameter from t_bind */
} XTIS, * XTISP;

#endif	/* _TIMOD_H */
