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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: device_utils.h,v $
 * Revision 1.7  1994/11/18  20:47:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/28  22:58:24  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.5  1994/06/01  00:47:49  mjl
 * Correct typo in #endif comment line.
 *
 * Revision 1.4  1993/07/14  18:40:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:00:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:26:41  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:53:48  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:42:40  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:51:34  cfj
 * Bump major revision number.
 *
 * Revision 2.10  93/10/20  15:31:47  dnoveck
 *      DEV_BSIZE elimination: Change blk{size,shift} to mrec{size,shift}.
 *
 * Revision 2.8  1992/07/14  14:52:03  rabii
 *
 * Revision 2.9  1993/01/29  13:53:58  durriya
 * 	add a field return_short_reads to devinfo_t                 (durriya)
 *
 * Revision 2.8  92/07/14  14:52:03  rabii
 * 	Added more fields to devinfo_t struct (used to be named dev_rawinfo_t).
 * 	[92/07/10            roy]
 * 
 * Revision 2.7  92/03/15  14:33:01  roy
 * 	remove extra ; from end of udecl_simple_lock_data in (srl)
 * 	char_device typedef
 * 
 * Revision 2.6  92/03/09  13:32:03  durriya
 * 	add c_lock to char_device_t - grV3.5 merge (durriya)
 * 
 * Revision 2.5  91/11/13  15:50:04  rabii
 * 	undef blksize macro from ufs/fs.h; collides with bsd/mach_init.c 
 * 	compiles
 * 
 * Revision 2.4  91/10/14  18:32:53  roy
 * 	Change to char_device for pgrp signalling (from Locus).
 * 
 * 
 * Revision 2.3  91/10/14  18:16:48  roy
 * 	Revision 2.2.1.2  91/10/02  16:38:20  roy
 * 		Changed dev_blkinfo to dev_rawinfo.
 * 
 * 	Revision 2.2.1.1  91/09/26  19:08:34  roy
 * 		Added dev_blkinfo and explicit device type support.
 * 
 * Revision 2.2  91/08/31  14:25:57  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/27  15:38:13  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.0  91/01/17  12:06:02  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.4  91/03/20  15:05:09  dbg
 * 	Distinguish block and character devices.
 * 	[91/02/21            dbg]
 * 
 * Revision 2.3  90/06/02  15:27:32  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:13:46  rpd]
 * 
 * Revision 2.2  89/09/15  15:29:30  rwd
 * 	Undefine KERNEL while including device_types.h
 * 	[89/09/11            rwd]
 * 
 */
/*
 * Support routines for device interface in out-of-kernel kernel.
 */
#ifndef	_UXKERN_DEVICE_UTILS_H_
#define	_UXKERN_DEVICE_UTILS_H_

#include <sys/param.h>
#include <sys/types.h>
#include <kern/queue.h>

#include <uxkern/import_mach.h>

#ifdef	KERNEL
#define	KERNEL__
#undef	KERNEL
#endif	KERNEL
#include <device/device_types.h>
#ifdef	KERNEL__
#undef	KERNEL__
#define	KERNEL	1
#endif	KERNEL__
#ifdef _KERNEL
#include <kern/lock.h>
#include <sys/unix_defs.h>
#endif /* _KERNEL */

extern mach_port_t	device_server_port;

/*
 * Types of devices that may be entered into the dev table.
 */
#define BLOCK_DEV	0x1
#define CHAR_DEV	0x2

/*
 * Character device support.
 */
typedef struct char_device {
	mach_port_t	c_device_port;	/* port to device */
	mach_port_t	c_select_port;	/* reply port for select */
	struct queue_entry c_read_selq;	/* Queue of threads waiting on read */
	int		c_read_sel_state;
					/* message sent/recvd for read-select */
#define CD_SEL_MSENT 1
#define CD_SEL_MRCVD 2
	struct queue_entry c_write_selq;/* Queue of threads waiting on write */
	int		c_write_sel_state;
					/* message sent/recvd for select */
	int		c_mode;		/* IO modes: */
#define	C_NBIO		0x01		/* non-blocking IO */
#define	C_ASYNC		0x02		/* asynchronous notification */
	pid_t		c_pgrp;
#ifdef	_KERNEL
	udecl_simple_lock_data(, c_lock)
#endif
} char_device_t;


/*
 * Info for block-oriented devices, such as disks.  Note that these devices
 * may be accessed either as BLOCK_DEV's or CHAR_DEV's.  Such devices are
 * adressed in term of mach records (address is an recnum_t).  The size of
 * a mach record is determined by the kernel driver and is typically the
 * physical sector size of the device.  The server drivers receive disk
 * address from their clients in terms of disk granules and convert these
 * to mach records.
 *
 * the return_short_reads filed indicates that if the kernel
 * driver returns with less than requested bytes on a read, don't loop in 
 * disk_read. This is a hack to allow tape drives to return to caller when
 * we hit a tape mark, but kernel disk drivers that may return with less than 
 * requested require repeated calls to the kernel driver until the request is
 * satisfied.
 */
typedef struct devinfo {
	mach_port_t	devport;	/* device port */
	int		mrecsize;	/* device block size */
	int		mrecmask;	/* mask for rounding */
	int		mrecshift;	/* shift for multiply/divide */
        boolean_t       return_short_reads; /* TRUE <=> we should  return short
                                             * reads to our caller if the 
                                             * driver does. is true for tape 
                                             * drives 
                                             */
} devinfo_t;

extern void			dev_utils_init();
extern void			dev_enter(/* dev_t, int, char * */);
extern void			dev_remove(/* dev_t, int */);
extern char			*dev_lookup(/* dev_t, int */);
extern int			dev_error_to_errno(/* int */);

#endif	/* _UXKERN_DEVICE_UTILS_H_ */
