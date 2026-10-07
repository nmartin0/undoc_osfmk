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
 * $Log: strlog.h,v $
 * Revision 1.4  1994/11/18  20:41:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:28:18  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:37:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:44:09  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:41:43  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:40:46  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/17  11:19:04  sjs
 * fixed RCS messages
 *
 * Revision 2.2  91/10/14  13:09:05  sjs
 * Check in
 * 
 * Revision 3.0  91/09/27  11:52:33  emcmanus
 * Installed for streams.
 * 
 * Revision 1.6  90/10/31  14:07:05  devrcs
 * 	Post-file-rearrangement pathnames.
 * 	[90/10/11  08:34:16  tmt]
 * 
 * 	Checkin prior to file rearrangement.
 * 	[90/10/08  13:40:47  tmt]
 * 
 * Revision 1.5  90/10/07  14:44:46  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:31:03  gm]
 * 
 * Revision 1.4  90/02/23  00:31:11  devrcs
 * 	Integrated new PSE release from Mentat.
 * 	[90/02/14  21:01:23  nolting]
 * 
 * 	Integrated new PSE release from Mentat.
 * 	[90/02/05  07:55:07  nolting]
 * 
 * Revision 1.3  90/01/02  20:22:52  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:28:05  gm
 * 	Added streams support.
 * 	[89/12/25            gm]
 * 
 * $EndLog$
 */
/** Copyright (c) 1989  Mentat Inc.
 ** strlog.h 1.2, last change 21/20/89
 **/

#ifndef	_STRLOG_H
#define	_STRLOG_H

/*
 * STREAMS logging.
 */

struct log_ctl {
	short	mid;
	short	sid;
	char	level;
	short	flags;
	long	ltime;
	long	ttime;
	int	seq_no;
};

#define	SL_FATAL	0x1	/* Fatal error */
#define	SL_NOTIFY	0x2	/* Notify the system administrator */
#define	SL_ERROR	0x4	/* Pass message to error logger */
#define	SL_TRACE	0x8	/* Pass message to tracer */

struct trace_ids {
	short	ti_mid;
	short	ti_sid;
	char	ti_level;
	short	ti_flags;
};

#define	I_TRCLOG	1
#define	I_ERRLOG	2

#define	LOGMSGSZ	128
#endif
