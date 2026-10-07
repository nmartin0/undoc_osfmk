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
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: server_mach_msg.h,v $
 * Revision 1.4  1994/11/18  20:49:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:44:16  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:06:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:56:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:44  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:55:30  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1992/03/15  14:33:55  roy
 * 	Created (srl).
 *
 */

/*
 * This redefines mach_msg so that the server has a chance to account
 * for the fact that a thread is about to block on a remote message.
 */

#ifndef	_UXKERN_SERVER_MACH_MSG_H_
#define	_UXKERN_SERVER_MACH_MSG_H_

#ifndef _EMULATOR
#define mach_msg(_p1,_p2,_p3,_p4,_p5,_p6,_p7) \
	server_mach_msg(_p1,_p2,_p3,_p4,_p5,_p6,_p7)
#endif

#endif	/* _UXKERN_SERVER_MACH_MSG_H_ */
