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
 * $Log: machtime.h,v $
 * Revision 1.5  1994/11/18  20:31:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  17:59:08  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:18:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:09:58  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:00  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:03  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:08  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.4  1992/05/01  10:29:34  rabii
 * 	CLOCKS_PER_SEC 1000 --> 100 as requested by stans (rabii)
 *
 * Revision 2.3  92/01/09  16:52:38  roy
 * 	91/12/20  10:16:19  stans
 * 	CLOCKS_PER_SEC 100 --> 1000
 * 
 * Revision 2.2  91/11/13  12:22:46  rabii
 * 	Initial checkin
 * 
 * Revision 2.2  91/08/31  13:35:14  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:57:53  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.2  90/10/31  13:56:43  devrcs
 * 	Created new header to separate machine-dependent time.h values from
 * 	machine-dependent limits.h values.
 * 	[90/10/06  11:57:40  rabin]
 * 
 * $EndLog$
 */

/*
 * This header contains machine-dependent time definitions
 */

#ifndef _MACH_MACHTIME_H_
#define _MACH_MACHTIME_H_

#define CLOCKS_PER_SEC	100		/* ANSI C clock ticks per sec */

#endif /* _MACH_MACHTIME_H_ */

