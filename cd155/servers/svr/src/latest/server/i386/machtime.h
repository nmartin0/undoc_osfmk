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
 * Revision 1.4  1994/11/18  20:31:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:56:19  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:38  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:45  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:56  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:20  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/03/09  12:04:20  durriya
 * 	[Revision 3.3  92/02/12  10:58:35  bernadat]
 * 	Fixed CLOCKS_PER_SEC to be 100
 *
 * Revision 2.3  91/12/16  18:41:30  roy
 * 	91/10/17  15:45:09  condict
 * 	Change CLOCKS_PER_SEC to 1000.
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

