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
 * $Log: sp.h,v $
 * Revision 1.5  1994/11/18  20:36:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:14:08  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:36:15  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:02:33  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:31:24  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:38  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:08  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:48:13  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:06  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:38:09  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:19:45  gm]
 * 
 * Revision 1.4  90/07/27  09:03:46  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:17:27  tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1984, 1985, 1986, 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	sp.h	7.4 (Berkeley) 6/28/90
 */

/*
 * Definitions for Xerox NS style sequenced packet protocol
 */

struct sphdr {
	u_char	sp_cc;		/* connection control */
	u_char	sp_dt;		/* datastream type */
#define	SP_SP	0x80		/* system packet */
#define	SP_SA	0x40		/* send acknowledgement */
#define	SP_OB	0x20		/* attention (out of band data) */
#define	SP_EM	0x10		/* end of message */
	u_short	sp_sid;		/* source connection identifier */
	u_short	sp_did;		/* destination connection identifier */
	u_short	sp_seq;		/* sequence number */
	u_short	sp_ack;		/* acknowledge number */
	u_short	sp_alo;		/* allocation number */
};
