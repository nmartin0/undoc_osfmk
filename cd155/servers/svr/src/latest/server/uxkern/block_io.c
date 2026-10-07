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
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: block_io.c,v $
 * Revision 1.11  1995/03/02  19:32:56  stans
 *  Lint picking
 *
 *  Reviewer:lenb & suri
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing:WW07 sats
 *
 * Revision 1.10  1994/12/02  21:20:09  yazz
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 11518 C-0
 *  Testing: Specific Testcase in PTS report.  Controlc EAT.
 *  Module(s):
 * 	server/i860/ipi_ops.c
 * 	server/uxkern/block_io.c
 * 	server/uxkern/device_misc.c
 * 	server/uxkern/disk_io.c
 * 	server/uxkern/raw_hippi.c
 * Make each user process open() of a device result in a microkernel
 * device_open() call, so MK device drivers can enforce 1-at-a-time usage.
 * Ensure that the number of MK device_open() & device_close() calls match.
 *
 * Revision 1.9  1994/11/18  20:46:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/06/28  23:17:10  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.7  1994/01/12  17:47:37  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.6  1993/07/19  22:59:28  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.5  1993/07/14  18:39:22  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:57:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/12  20:50:05  cfj
 * Fix swapon to block special devices.
 *
 * Revision 1.3  1993/05/06  19:24:14  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:52:35  dleslie
 * Copy of NX branch back into main trunk
 * Revision 1.1.1.1  1993/05/03  17:50:22  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1  1992/11/05  23:41:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:49:44  cfj
 * Bump major revision number.
 *
 * Revision 2.12  1992/10/22  15:53:51  dbm
 * Added PFS functionality.
 *
 * Revision 2.16  93/10/20  15:31:41  dnoveck
 *      DEV_BSIZE elimination: Accept disk addresses from the outside
 *      in terms of disk granules and convert to mach records for the
 *      kernel.  Chnage blk{size,shift} to mrec{size,shift}.  Use btodg
 *      instead of btodb.
 *
 * Revision 2.19  93/07/13  16:03:06  slively
 *      Revision 2.15  93/06/29  16:17:08  rabii
 *      Lite server mods (rabii)
 *
 * Revision 2.11  1992/07/14  14:51:02  rabii
 * 	Use dev_get_status to find out device sector size.
 * Revision 2.13  1993/01/29  13:54:19  durriya
 *
 * Revision 2.18  93/06/25  11:23:38  slively
 * Backout the LITE server changes.  Remove #if UFS and related code.
 * 
 * Revision 2.17  93/06/23  09:34:43  slively
 * Removed rcsmerge garbage that was left in this file after last update and
 * install.
 * 
 * Revision 2.16  93/06/22  20:02:04  slively
 * Support for LITE server, #if UFS sections and the include <ufs.h>.
 * 
 * Revision 2.15  93/06/22  18:10:15  yazz
 * Incorporate changes from Intel that pertain to device driver specifications.
 * 
 * Revision 2.14  93/05/18  14:42:04  loverso
 * 	Fix bdev_size to use DEV_GET_SIZE comamnd. Also fix bdev_ioctl
 * 	to handle DEV_GET_SIZE correctly
 * 	[93/05/12            durriya]
 * 
 * Revision 2.13  93/01/29  13:54:19  durriya
 * 	Initialise return_short_reads to FALSE in bdev_open           (durriya)
 *
 * Revision 2.12  93/01/08  14:34:38  durriya
 * 	add node as arg to bdev_close and bdev_ioctl. Also pass node to
 * 	dev_* calls                                             durriya
 * 
 * Revision 2.11  92/07/14  14:51:02  rabii
 * 	Use dev_get_status to find out device sector size.  
 * 	dev_rawinfo -> devinfo name change.
 * 	[92/07/12            roy]
 * 
 * Revision 2.10  92/06/09  16:42:05  pjg
 * 	Fix conflict in defn of "p_flag" btwn i386at/disk.h and user.h/proc.h
 * 	(loverso).
 * 
 * Revision 2.9  92/05/26  13:00:21  pjg
 * 	Removed ifdef's around include of i386at/disk.h
 * 
 * Revision 2.8  92/05/24  13:54:28  pjg
 * 	92/03/24  21:03:51  barbou
 * 	ifdef'ed some i386 code.
 * 
 * Revision 2.7  92/05/01  09:44:09  rabii
 * 	Modified to call device open on the proper node directly (rabii)
 * 
 * Revision 2.6  92/03/09  12:52:29  durriya
 * 	92/02/27  21:08:42  jose
 * 	Made all synchronous operations use the synchronous mach calls
 * 	(device_read & device_write) instead of sleeping.
 * 	Added bdev_ioctl and bdev_size.
 * 
 * 	91/12/27  17:26:13  jose
 * 	Changed interface to reply_hash_enter for port aliasing
 * 
 * Revision 2.5  91/12/17  11:48:19  roy
 * 	91/12/17  11:45:52  roy
 * 	Fix args to bdev_open.
 * 
 * 	91/11/06  14:53:10  emcmanus
 * 	Report failed write requests via syslog.  Optionally limit the number
 * 	of concurrent write requests to avoid taxing the poor microkernel.
 * 
 * 	91/10/30  09:16:55  sp
 * 	General cleanup and 'fix' bdev_ioctl.
 * 
 * 	91/10/17  18:33:44  barbou
 * 	Provide some backward compatibility with the original OSF/1.0.2 buffer
 * 	management (needed for LVM).
 * 
 * Revision 2.4  91/11/25  16:14:36  rabii
 * 	Added support for remote devices
 * 
 * Revision 2.3  91/10/14  18:05:49  roy
 * 	Revision 2.2.1.2  91/10/02  16:37:07  roy
 * 		Changed dev_blkinfo to devinfo.
 * 
 * 	Revision 2.2.1.1  91/09/26  19:10:55  roy
 * 		Modified to use dev_blkinfo as well as new interface to 
 * 		the dev table.
 * 
 * Revision 2.2  91/08/31  14:21:26  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/27  15:31:16  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.2  91/04/12  14:17:48  condict
 * Add include of sys/ucred.h; uncomment all functions and declarations except
 * bio_init(), bfree() and allocbuf().
 * 
 * Revision 3.1  91/03/08  16:05:47  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.0  91/01/17  12:05:41  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.5  91/03/20  15:04:41  dbg
 * 	Move bdev_open and bdev_close here from device_misc.c.  Use new
 * 	macros to distinguish block and character device numbers.
 * 
 * 	Remove vm_pageable calls from buffers.
 * 	[90/11/07            dbg]
 * 
 * Revision 2.4  90/09/09  14:13:31  rpd
 * 	Increase debugging info.
 * 	[90/09/05            rwd]
 * 
 * Revision 2.3  90/06/19  23:14:16  rpd
 * 	Fixed bio_strategy to not call bio_read_reply/bio_write_reply.
 * 	Fixed bio_read_reply/bio_write_reply to protect ALL buffer
 * 	munging with SPLBIO.
 * 	[90/06/11            rpd]
 * 
 * Revision 2.2  90/06/02  15:26:39  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:06:28  rpd]
 * 
 */

/*
 * Block IO using MACH KERNEL interface.
 */

#include <ufs.h>
#include <sys/param.h>
#include <sys/buf.h>
#include <sys/errno.h>
#include <sys/synch.h>
#include <kern/sched_prim.h>

#include <uxkern/import_mach.h>
#include <uxkern/device_reply_hdlr.h>
#include <uxkern/device_utils.h>

#include <device/device.h>

#include <sys/ioctl.h>
#include <sys/disklabel.h>
#include <sys/syslog.h>
#include <sys/time.h>
#include <sys/vnode.h>
#include <ufs/inode.h>


kern_return_t	bio_read_reply();
kern_return_t	bio_write_reply();
mach_port_t	node_to_master_device_port();

bio_strategy(bp)
	struct buf *	bp;
{
	devinfo_t	        *devinfo;
	io_buf_ptr_t		data;
	mach_msg_type_number_t	data_count;
	kern_return_t    	error;
	daddr_t			dgran;
	recnum_t		record;

	/*
	 * Find the request port for the device.
	 */
	devinfo = (devinfo_t *) dev_lookup(bp->b_dev, bp->b_devnode, 
                                           BLOCK_DEV);
	if (devinfo == NULL)
		panic("bio_strategy null port");

	/*
	 * Adapt Unix device addressing to the Mach mode.
	 */
	dgran = bp->b_blkno;
	record = dgran >> (devinfo->mrecshift - DISK_GSHIFT);
	if (dgran & ((devinfo->mrecsize >> DISK_GSHIFT) - 1)) {
		bp->b_error = EIO;
		bp->b_flags |= B_ERROR;
		biodone(bp);
		return;
	}

	/*
	 * The LVM allocates some bufs dynamically. These are not initialized
	 * by bio_init() and thus, have no reply port. Instead of adding some
	 * initialization code to all the allocations in the LVM, let's 
	 * provide a reply port here if there isn't already one. 
	 */
	if (bp->b_reply_port == MACH_PORT_NULL) {
		/* 
		 * naive usage of buf: don't screw everything up by doing
		 * some obscure optimization in bio_read_reply()
		 */
		/* allocate one reply port per buffer (the Accent way...) */
#ifdef	REPLY_PORT_ALIAS
		reply_hash_enter(&bp->b_reply_port,
				 (char *)bp,
				 bio_read_reply,
				 bio_write_reply);
	}
#else
		bp->b_reply_port = mach_reply_port();
		reply_hash_enter(bp->b_reply_port,
				 (char *)bp,
				 bio_read_reply,
				 bio_write_reply);
	}
#endif
	/*
	 * Start the IO.  In case of error we can call 
	 * read_reply/write_reply which don't use
	 * interrupt_enter/interrupt_exit.
	 */
	if (bp->b_flags & B_READ) {
		if (bp->b_flags & B_ASYNC) {
			error = device_read_request(devinfo->devport,
						    bp->b_reply_port,
						    D_NOWAIT,
						    record,
						    (unsigned)bp->b_bcount);
			if (error != KERN_SUCCESS)
				read_reply(bp, error, (char *)0, 0, FALSE);
		} else { /* B_SYNC */
			ux_server_thread_blocking();
			error = device_read(devinfo->devport,
					    D_NOWAIT,
					    record,
					    (unsigned)bp->b_bcount,
					    (io_buf_ptr_t *)&data,
					    &data_count);
			ux_server_thread_unblocking();
			read_reply(bp, error, data, data_count, FALSE);
		}
	} else { /* B_WRITE */
		if (bp->b_flags & B_ASYNC) {
			error = device_write_request(devinfo->devport,
						     bp->b_reply_port,
						     D_NOWAIT,
						     record,
						     bp->b_un.b_addr,
						     bp->b_bcount);
			if (error != KERN_SUCCESS)
				write_reply(bp, error, 0, FALSE);
		} else { /* B_SYNC */
			ux_server_thread_blocking();
			error = device_write(devinfo->devport,
					     D_NOWAIT,
					     record,
					     bp->b_un.b_addr,
					     bp->b_bcount,
					     (int *) &data_count);
			ux_server_thread_unblocking();
			write_reply(bp, error, data_count, FALSE);
		}
	}
}


#include <kern/parallel.h>

int copy_data = 1;

kern_return_t
bio_read_reply(bp_ptr, return_code, data, data_count)
	char *		bp_ptr;
	kern_return_t	return_code;
	char		*data;
	unsigned int	data_count;
{
	(void)read_reply(bp_ptr, return_code, data, data_count, TRUE);
}

read_reply(bp_ptr, return_code, data, data_count, from_device_hdlr)
	char *		bp_ptr;
	kern_return_t	return_code;
	char		*data;
	unsigned int	data_count;
	boolean_t	from_device_hdlr;
{
	register struct buf *bp = (struct buf *)bp_ptr;
	vm_offset_t dealloc_addr;
	vm_size_t dealloc_size = 0;

	if (from_device_hdlr)
		interrupt_enter(SPLBIO);
	if (return_code != D_SUCCESS) {
		bp->b_flags |= B_ERROR;
		bp->b_error = EIO;
	} else {
		if (data_count < bp->b_bcount) {
			bp->b_flags |= B_ERROR;
			bp->b_resid = bp->b_bcount - data_count;
		}
		if (copy_data || bp->b_optimize_mem == FALSE) {
			/* 
			 * OSF/1 sources may assume that the data will be stored
			 * at the address they provided (bp->b_un.b_addr).
			 * The optimization below (which avoids an extra copy of
			 * the data allocated by Mach to the user buffer) would 
			 * break this code.
			 * This flag forces the data to be copied to the user
			 * buffer.
			 * A better solution would be  to modify the OSF/1 code
			 * to take into account the optimization below, but this
			 * may concern huge pieces of code (e.g. the LVM).
			 */
			dealloc_addr = (vm_offset_t)data;
			dealloc_size = data_count;
			(void) bcopy(data, bp->b_un.b_addr, data_count);
		} else {
			/*
			 * Deallocate old memory.  Actually do it later,
			 * after we have lowered IPL.
			 */
			if (bp->b_bufsize > 0) {
				dealloc_addr = (vm_offset_t) bp->b_un.b_addr;
				dealloc_size = (vm_size_t) bp->b_bufsize;
			}
			bp->b_un.b_addr = data;
			bp->b_bufsize = round_page(data_count);
		}
	}
	biodone(bp);
	if (from_device_hdlr)
		interrupt_exit(SPLBIO);

	if (dealloc_size != 0)
	    (void) vm_deallocate(mach_task_self(), dealloc_addr, dealloc_size);
}

kern_return_t
bio_write_reply(bp_ptr, return_code, bytes_written)
	char *		bp_ptr;
	kern_return_t	return_code;
	int		bytes_written;
{
	write_reply(bp_ptr, return_code, bytes_written, TRUE);
}

write_reply(bp_ptr, return_code, bytes_written, from_device_hdlr)
	char *		bp_ptr;
	kern_return_t	return_code;
	int		bytes_written;
	boolean_t	from_device_hdlr;
{
	register struct buf *bp = (struct buf *)bp_ptr;

	if (from_device_hdlr)
		interrupt_enter(SPLBIO);
	if (return_code != D_SUCCESS) {
#if     UFS
	    extern struct vnodeops ufs_vnodeops;
#endif  /* UFS */
#ifdef	PFS
	    extern struct vnodeops pfs_vnodeops;
#endif	PFS
	    bp->b_flags |= B_ERROR;
	    bp->b_error = EIO;
#if UFS
#ifdef	PFS
	    if ((bp->b_rvp->v_op == &ufs_vnodeops) ||
	        (bp->b_rvp->v_op == &pfs_vnodeops)) {
#else	/* PFS */
	    if (bp->b_rvp->v_op == &ufs_vnodeops) {
#endif  PFS
		struct inode *ip = VTOI(bp->b_rvp);
		fserr(ip->i_fs, "write error");
	    } else log(LOG_ERR, "write error");
#else   /* UFS */
        log(LOG_ERR, "write error");
#endif  /* UFS */
	} else if (bytes_written < bp->b_bcount) {
	    bp->b_flags |= B_ERROR;
	    bp->b_resid = bp->b_bcount - bytes_written;
	}
	biodone(bp);
	if (from_device_hdlr)
		interrupt_exit(SPLBIO);
}

/*
 * Open block device.
 */
int
bdev_open(dev, mode, flag, node)
	dev_t			dev;
	int			mode;
	int			flag;
	int			node;
{
	char			name[32];
	kern_return_t		rc;
	devinfo_t		*devinfo;
	mach_port_t		devport;
	int			devstat[DEV_GET_SIZE_COUNT];
	int			i;
	mach_msg_type_number_t	devstat_count;

	rc = bdev_name_string(dev, name);
	if (rc != 0)
		return (rc);

	/* fix modes */
	mode = 0;	/* XXX */
	rc = device_open(node_to_master_device_port(node),
			 mode,
			 name,
			 &devport);
	if (rc != D_SUCCESS)
		return (dev_error_to_errno(rc));

	/*
	 * See whether we had the device open already.
	 */
	if (dev_lookup(dev, node, BLOCK_DEV)) {
		(void)device_close(devport);	/* match extra open w/ close */
		return (0);
	}

	devinfo = (devinfo_t *) malloc(sizeof(devinfo_t));
	devinfo->devport = devport;
	devinfo->return_short_reads = FALSE;

	devstat_count = DEV_GET_SIZE_COUNT;
	rc = device_get_status(devport,
			       DEV_GET_SIZE,
			       (dev_status_t)&devstat,
			       &devstat_count);
	if (rc != D_SUCCESS)
		panic("bdev_open.device_get_status failure 0x%x\n", rc);

	devinfo->mrecsize = devstat[DEV_GET_SIZE_RECORD_SIZE];
	devinfo->mrecmask = ~(devinfo->mrecsize - 1);

	for (devinfo->mrecshift = 0, i = devinfo->mrecsize; i > 1; i >>= 1)
		devinfo->mrecshift++;

	dev_enter(dev, node, BLOCK_DEV, (char *) devinfo);

	return (0);
}

int
bdev_close(dev, node, flag)
	dev_t		dev;
        node_t          node;
	int		flag;
{
	devinfo_t	*devinfo;
	int		error;

	devinfo = (devinfo_t *) dev_lookup(dev, node, BLOCK_DEV);
	if (devinfo == NULL)
	    return (ENXIO);	/* shouldn't happen */

	dev_remove(dev, node, BLOCK_DEV);
	error = dev_error_to_errno(device_close(devinfo->devport));
	(void) mach_port_deallocate(mach_task_self(), devinfo->devport);
	free((char *) devinfo);
	return (error);
}

bdev_dump()
{
	printf("bdev_dump()----------\n"); return(0);
}

bdev_size(dev, node)
{
	int size, error;
	dev_status_data_t dev_status;

	error = bdev_ioctl(dev, node, DEV_GET_SIZE, (caddr_t)&dev_status,
                           0);
	if (error)
		return(-1);
	size = btodg(dev_status[DEV_GET_SIZE_DEVICE_SIZE]);
	return size;
}

bdev_ioctl(dev, node, cmd, data, flag)
dev_t	dev;
node_t  node;
int	cmd;
caddr_t	data;
int	flag;
{
	unsigned int	count;
	register int	error;
	devinfo_t	*devinfo;

	if (cmd == DIOCGPART)
		return(EINVAL);

	devinfo = (devinfo_t *) dev_lookup(dev, node, BLOCK_DEV);
	if (devinfo == NULL)
	    return (ENXIO);	/* shouldn't happen */

	if (cmd == DEV_GET_SIZE) {
		count = DEV_GET_SIZE_COUNT;
		error = device_get_status(devinfo->devport,
					  cmd,
					  (int *) data,
					  &count);
		goto done;
	}

	count = (cmd & ~(IOC_INOUT|IOC_VOID)) >> 16; /* bytes */
	count = (count + 3) >> 2;		     /* ints */
	if (count == 0)
	    count = 1;

	if (cmd & (IOC_VOID|IOC_IN)) {
	    error = device_set_status(devinfo->devport,
				      cmd,
				      (int *)data,
				      count);
	    if (error)
		return (dev_error_to_errno(error));
	}
	if (cmd & IOC_OUT) {
	    error = device_get_status(devinfo->devport,
				      cmd,
				      (int *)data,
				      &count);
	}
      done:
	if (error)
	     return (dev_error_to_errno(error));
	else
	     return (0);
}

