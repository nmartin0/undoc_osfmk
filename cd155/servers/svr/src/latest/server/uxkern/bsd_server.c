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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: bsd_server.c,v $
 * Revision 1.4  1994/11/18  20:47:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:39:38  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:58:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:52:52  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:41:53  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:50:01  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:23:56  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/27  15:37:02  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.0  91/01/17  12:05:46  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.3  91/03/20  15:04:47  dbg
 * 	Include sys/time.h before undefining KERNEL to avoid search for
 * 	<time.h>.
 * 	[91/02/22            dbg]
 * 
 * Revision 2.2  90/06/19  23:15:37  rpd
 * 	Added uxkern/bsd_2_user.c, uxkern/bsd_2_server.c.
 * 
 */
/*
 * Wrapper to compile bsd servers without KERNEL defined.
 */
#include <sys/time.h>

#undef	KERNEL
#include <uxkern/bsd_1_server.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/bsd_2_user.c>
#undef	msgh_request_port
#undef	msgh_reply_port
#include <uxkern/bsd_2_server.c>
