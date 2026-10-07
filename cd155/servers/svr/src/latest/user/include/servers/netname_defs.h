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
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: netname_defs.h,v $
 * Revision 1.5  1994/11/18  20:53:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:51:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:30:08  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:18:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  23:02:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:50:11  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:03:01  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:58:17  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1992/01/05  19:13:01  roy
 * 	Get rid of obsolete history.
 *
 * Revision 2.2  92/01/05  19:10:44  roy
 * 	Created.
 */

/*
 *	File:	netname_defs.h
 *	Author: Dan Julin, Carnegie Mellon University
 *	Date:	Dec. 1986
 *
 * 	Definitions for the mig interface to the network name service.
 */

#ifndef	_NETNAME_DEFS_
#define	_NETNAME_DEFS_

#define NETNAME_SUCCESS		(0)
#define	NETNAME_PENDING		(-1)
#define NETNAME_NOT_YOURS	(1000)
#define NAME_NOT_YOURS		(1000)
#define NETNAME_NOT_CHECKED_IN	(1001)
#define NAME_NOT_CHECKED_IN	(1001)
#define NETNAME_NO_SUCH_HOST	(1002)
#define NETNAME_HOST_NOT_FOUND	(1003)
#define	NETNAME_INVALID_PORT	(1004)

typedef char netname_name_t[80];

#endif NETNAME_DEFS_
