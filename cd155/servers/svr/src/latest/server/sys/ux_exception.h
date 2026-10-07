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
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 **********************************************************************
 * HISTORY
 * $Log: ux_exception.h,v $
 * Revision 1.4  1994/11/18  20:42:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:30:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:40:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:40  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:43:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:20  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/03/09  12:20:08  durriya
 * 	[92/02/28  00:11:27  condict]
 * 	Add emul_uacc_* vars, for EFAULT recovery in the emulator.
 *
 * Revision 2.2  91/08/31  14:16:51  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:09:22  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/06/02  15:26:20  rpd
 * 	Cleaned up conditionals; removed MACH, CMU, CMUCS, MACH_NO_KERNEL.
 * 	[90/04/28            rpd]
 * 	Converted to new IPC.
 * 	Added EXC_UNIX_ABORT.
 * 
 * 	Out-of-kernel version.
 * 	[89/01/06            dbg]
 * 
 * Revision 2.1  89/08/04  14:46:33  rwd
 * Created.
 * 
 * Revision 2.2  88/08/24  02:52:12  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:27:27  mwyoung]
 * 
 *
 * 29-Sep-87  David Black (dlb) at Carnegie-Mellon University
 *	Created.
 *
 **********************************************************************
 */

#ifndef	_SYS_UX_EXCEPTION_H_
#define	_SYS_UX_EXCEPTION_H_

/*
 *	Codes for Unix software exceptions under EXC_SOFTWARE.
 */


#define EXC_UNIX_BAD_SYSCALL	0x10000		/* SIGSYS */

#define EXC_UNIX_BAD_PIPE	0x10001		/* SIGPIPE */

#define EXC_UNIX_ABORT		0x10002		/* SIGABRT */

#ifdef	KERNEL
#include <uxkern/import_mach.h>

/*
 *	Kernel data structures for Unix exception handler.
 */

/*
 *	Addresses in the text segment of the emulator.  This is for doing
 *	error recovery when the emulator gets an exception while accessing a
 *	user address, allowing the emulator to return EFAULT instead of
 *	the process core dumping.  These addresses are set by an RPC
 *	(bsd_emul_uacc_addrs) from the emulator during its bootstrap:
 */
int			emul_uacc_start, emul_uacc_end, emul_uacc_err;

struct mutex		ux_handler_init_lock;
mach_port_t		ux_exception_port;

#endif	KERNEL
#endif	_SYS_UX_EXCEPTION_H_
