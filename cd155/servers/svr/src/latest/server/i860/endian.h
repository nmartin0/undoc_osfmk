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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: endian.h,v $
 * Revision 1.5  1994/11/18  20:31:46  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:58:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:17:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:09:18  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:29:51  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:21:46  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/11/13  12:21:14  rabii
 * 	Initial checkin
 *
 * Revision 2.2  91/08/31  13:33:37  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:56:46  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/31  13:54:21  devrcs
 * 	Add inline byteswappers when compiling under GCC.
 * 	[90/10/23  08:21:35  tmt]
 * 
 * Revision 1.5  90/10/07  13:37:44  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:31:06  gm]
 * 
 * Revision 1.4  90/08/09  13:17:32  devrcs
 * 	Add new NTOHL, etc macros. Make file re-entrant.
 * 	[90/07/24  14:57:45  tmt]
 * 
 * Revision 1.3  90/07/17  11:28:00  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:17:58  kevins]
 * 
 * 	From TAHOE for the programs that care.
 * 	[89/10/21            rvb]
 * 
 * $EndLog$
 */
 
/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)endian.h	7.3 (Berkeley) 6/29/88
 */

#ifndef _MACHINE_ENDIAN_H_
#define _MACHINE_ENDIAN_H_

/*
 * Definitions for byte order,
 * according to byte significance from low address to high.
 */
#define	LITTLE_ENDIAN	1234	/* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321	/* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412	/* LSB first in word, MSW first in long (pdp) */

#define	BYTE_ORDER	LITTLE_ENDIAN	/* byte order on i860 */

/*
 * Macros for network/external number representation conversion.
 */

unsigned short	ntohs(), htons();
unsigned long	ntohl(), htonl();

#define NTOHL(x)	(x) = ntohl((u_long)x)
#define NTOHS(x)	(x) = ntohs((u_short)x)
#define HTONL(x)	(x) = htonl((u_long)x)
#define HTONS(x)	(x) = htons((u_short)x)

#endif /* _MACHINE_ENDIAN_H_ */
