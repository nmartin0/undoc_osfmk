/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the  rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: errno.h,v $
 * Revision 1.6  1994/11/18  20:31:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/05/24  18:54:23  richardg
 *  Reviewer: Jerrie Coffman
 *  Risk: low
 *  Benefit or PTS #: added D_NO_SPACE for tapes.
 *  Testing:
 *  Module(s):
 *
 * Revision 1.4  1993/06/30  22:22:17  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:20:36  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:20:24  dleslie
 * First R1_0 release
 *
 * Revision 2.3  91/05/14  15:48:09  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/02/05  17:09:37  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:29:22  mrt]
 * 
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	8/89
 *
 *	Old names for new error codes, for compatibility.
 */

#ifndef	_ERRNO_
#define	_ERRNO_

#include <device/device_types.h>	/* the real error names */

#define	EIO		D_IO_ERROR
#define	ENXIO		D_NO_SUCH_DEVICE
#define	EINVAL		D_INVALID_SIZE	/* XXX */
#define	EBUSY		D_ALREADY_OPEN
#define	ENOTTY		D_INVALID_OPERATION
#define ENOMEM		D_NO_MEMORY
#define ENOSPC		D_NO_SPACE

#endif	_ERRNO_
