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
 * $Log: cmn_err.h,v $
 * Revision 1.4  1994/11/18  20:40:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:23:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:32:51  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:41:27  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:50  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/17  11:18:51  sjs
 * fixed RCS messages
 *
 * Revision 2.2  91/10/14  13:01:29  sjs
 * Check in
 * 
 * Revision 3.0  91/09/27  11:52:16  emcmanus
 * Installed for streams.
 * 
 * Revision 1.4  90/10/31  14:06:18  devrcs
 * 	Post-file-rearrangement pathnames.
 * 	[90/10/11  08:33:51  tmt]
 * 
 * 	Add varargs prototype. Checkin prior to file rearrangement.
 * 	[90/10/08  13:34:31  tmt]
 * 
 * Revision 1.3  90/10/07  14:44:01  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:29:54  gm]
 * 
 * Revision 1.2  90/06/29  13:37:27  devrcs
 * 	Invented the file based on V.4 STREAMS programmer's guide.
 * 	[90/06/24  18:47:22  nolting]
 * 
 * $EndLog$
 * 
 */

#ifndef	_CMN_ERR_H
#define	_CMN_ERR_H

/*
 * STREAMS common error codes.
 */

#ifdef	_NO_PROTO

extern	void	cmn_err();

#else

extern	void	cmn_err(int, char *, ...);

#endif

/*
 *	level definitions for cmn_err()
 */
#define	CE_CONT		0x0001		/* simple printf */
#define CE_NOTE		0x0002		/* "NOTICE:" */
#define	CE_WARN		0x0003		/* "WARNING:" */
#define	CE_PANIC	0x0004		/* "PANIC:" */
#endif
