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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: device_reply_hdlr.h,v $
 * Revision 1.4  1994/11/18  20:47:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:40:41  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:00:10  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:53:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:36  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:51:27  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/03/09  13:31:13  durriya
 * 	91/12/27  17:26:19  jose
 * 	Defined the REPLY_PORT_ALIAS to be true
 *
 * Revision 2.2  91/08/31  14:25:41  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:05:59  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/06/02  15:27:28  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:13:26  rpd]
 * 
 */

/*
 * Handler for device read and write replies.
 */
#ifndef	DEVICE_REPLY_HDLR_H
#define	DEVICE_REPLY_HDLR_H

#define	REPLY_PORT_ALIAS	1

#include <mach/kern_return.h>

typedef kern_return_t	(*kr_fn_t)();

extern void	reply_hash_enter();	/* mach_port_t, char *, kr_fn_t, kr_fn_t */
extern void	reply_hash_remove();	/* mach_port_t */

extern void	device_reply_hdlr();

#endif	DEVICE_REPLY_HDLR_H

