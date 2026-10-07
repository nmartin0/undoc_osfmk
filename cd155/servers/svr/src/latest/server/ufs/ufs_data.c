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
 * HISTORY:
 * $Log: ufs_data.c,v $
 * Revision 1.8  1995/03/03  02:05:10  stans
 *  Lint picking
 *
 *  Reviewer:suri
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing:WW07 sats
 *
 * Revision 1.7  1994/11/18  20:45:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/28  23:11:41  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.5  1993/10/08  01:19:31  cfj
 * Rework lines where there was a cast on the left side of the equals sign
 * so that the PGI 4.5 compiler will compile the module.
 *
 * Revision 1.4  1993/07/14  18:38:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:53:19  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:30:46  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:50:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:39:26  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:48:09  cfj
 * Bump major revision number.
 *
 * Revision 2.8  93/10/20  15:31:10  dnoveck
 *      DEV_BSIZE elimination: Change to accept disk addresses from
 *      the outside in terms of disk granules and convert to mach
 *      records for the kernel.
 *
 * Revision 2.7  1992/08/13  19:20:00  rabii
 * 	Added back missing comment line for 2.5 (rabii)
 *
 * Revision 2.6  92/07/14  14:53:41  rabii
 * 	Changed calling sequence to data_read/data_write.
 * 	[92/07/10            roy]
 * 
 * Revision 2.5  92/07/08  12:06:52  rabii
 * 	Modified data_read to only issue device_read(s) for multiples of
 * 	blksize (roy)
 * 
 * Revision 2.4  92/05/24  14:47:28  pjg
 * 	Added comments related to NCPUS == 1 in the read/write finish routines.
 * 	[92/05/20            roy]
 * 
 * Revision 2.3  92/03/15  14:41:32  roy
 * 	Added calls to ux_server_thread_blocking/unblocking.
 * 
 * Revision 2.2  91/12/10  21:37:09  roy
 * 	91/10/07  20:04:44  roy
 * 	Much improved error handling; better isolation from 
 * 	cache module.
 * 
 * 	91/10/02  16:35:35  roy
 * 	Cache interface converted to use device port instead
 * 	of dev_t.
 * 
 * 	91/09/23  10:24:23  roy
 * 	Initial revision for OSF/1.
 * 
 * Revision 2.1  91/09/23  10:24:06  roy
 * Created.
 * 
 */

/*
 * Routines implementing file data abstraction.
 */

/*
 * Note on the synchronization design in this layer:
 *
 * This layer only guarantees the following:
 *   data_read() and data_write() invocations for a data range that
 *   overlaps with a prior, completed data_read() or data_write()
 *   invocation, are guaranteed to be executed after the prior invocation.
 *
 * This implies that all other ordering guarantees (to clients) must 
 * be provided at a higher level.  For example, if a client task
 * writes and then reads the same data, and in-order execution is desired,
 * it is the responsibility of a higher layer in the file server to 
 * ensure that the data_read() invocation to this layer is not executed 
 * until the data_write() invocation has completed.
 *
 * There are two aspects that require us to synchronize at this layer:
 * read-ahead and write-behind.  
 * (1) Read-ahead.  Incoming reads and writes to this layer must 
 * synchronize with asynchronously initiated read-ahead operations.  
 * The possibility of conflict arises here because the higher layer has 
 * no knowledge of asynchronous read-aheads.
 *
 * (2) Write-behind.  This layer may notify the high layer that a
 * write operation has completed when in reality the disk operation
 * has not yet occurred.  Because of this, this layer must 
 * synchronize incoming requests with outstanding write-behind operations
 * to guarantee in-order execution.
 * 
 * Synchronization Design:
 * Conceptually, there are two lists of outstanding operations, one for
 * read-aheads and one for pending writes (write-behinds).  The 
 * implementation of these lists is fully abstracted by the underlying
 * cache_* layer.  These lists allow incoming requests to be synchronized
 * with the outstanding operations.
 *
 * Read Algorithm:
 * - Try to satisfy the request from the read-ahead list.
 * - If can, get the data and return.
 * - If can't, wait until all conflicting write-behinds finish.
 * - Perform disk read (synchronously).
 * - Kick off read-ahead if needed.
 * - Return data.
 * 
 * Write Algorithm:
 * - Wait for any conflicting write operations to complete.
 * - Insert an entry in the write list (in order to block later
 *   incoming conflicting writes).
 * - Abort any conflicting read-aheads in progress (don't want stale
 *   data in the read-ahead cache).
 * - Perform the disk write (asynchronously).
 * - Return.
 *
 * Write Completion Algorithm:
 * - Remove entry from the write list.
 * - Wakeup any waiters.
 * 
 * Read-Ahead Algorithm:
 * - Insert an entry in the read-ahead list.
 * - Give up if there are any conflicting writers.
 * - Perform the disk operation (asynchronously).
 * - Return.
 * 
 * Read-Ahead Completion Algorithm:
 * - Mark data in the corresponding entry in the read-ahead list
 *   as valid (unless there was an error or a writer invalidated
 *   the entry).
 * - Wakeup any waiters.
 */

#include <sys/types.h>
#include <uxkern/import_mach.h>
#include <uxkern/device_utils.h>
#include <device/device.h>

/* statistics */
int			data_read_sync_num = 0;
int			data_read_try_again_set = 0;
int			data_read_try_again_got = 0;
int			data_read_async_num = 0;
int			data_write_sync_num = 0;
int			data_write_async_num = 0;

#define TRY_AGAIN	0xab0c0fd9

void 
data_init()
{
	/*
	 * Initialize the file data cache, providing it with the
	 * functions to be called upon completion of disk reads
	 * and writes.  (The callback functions are associated with
	 * the cache buffers.)
	 */
	kern_return_t	data_read_finish(), data_write_finish();

	cache_init(data_read_finish, data_write_finish);
}


/*
 * Internal routine used for read-ahead.
 */
void 
data_read_async(devinfo, blkno, numblks)
	devinfo_t	*devinfo;
	daddr_t 	blkno;
	int		numblks;
{
	register void	*tag;
	mach_port_t	reply_port;
	kern_return_t 	err;
	recnum_t	recno;

	data_read_async_num++;			/* statistics */

	/*
	 * Adapt Unix device addressing to the Mach mode.
	 */
	recno = blkno >> (devinfo->mrecshift - DISK_GSHIFT);
	if (blkno & ((devinfo->mrecsize >> DISK_GSHIFT) - 1))
		panic("data_read_async: sec bounding");
	if (dgtob(numblks) & (devinfo->mrecsize - 1))
		panic("data_read_async: sec size");

	/*
	 * Prepare the cache for a read operation.  If tag == NULL then 
	 * just give up.
	 */
	if ((tag = (void *) cache_read_setup(devinfo, blkno, numblks, 
					     &reply_port)) == NULL)
		return;

	/*
	 * If there is a conflicting write outstanding then abort this
	 * read-ahead operation.  The error code is set to TRY_AGAIN in
	 * case another reader has already had a cache hit on this buffer.
	 */
	if (cache_write_conflict_detect(devinfo, blkno, numblks, FALSE)) {
		/* need better err code */
		data_read_try_again_set++;
		cache_read_abort(tag, TRY_AGAIN);
		return;
	}

	/*
	 * Perform the asynchronous read. 
	 * Data returned is a page size multiple and is guaranteed
	 * to be zero'd beyond the 'size' bytes requested.
	 */
	if ((err = device_read_request(devinfo->devport, reply_port, D_READ, 
			 (recnum_t) recno, dgtob(numblks)))
	    != KERN_SUCCESS) {
		/*
		 * Read request failed => no aysnc. reply expected.
		 */
		printf("Error: disk read request, block=%d size=%d err=0x%x\n", 
		       blkno, dgtob(numblks), err);
		cache_read_abort(tag, err);
	        return;
	}
}


int 
data_read(devinfo, blkno, numblks, rablkno, ranumblks, data)
	devinfo_t	*devinfo;
	daddr_t 	blkno;
	int 		numblks;
	daddr_t 	rablkno;
	int 		ranumblks;
	vm_address_t 	*data;    /* out */
{
	register int	size;
	void		*tag;
	unsigned int	count;
	kern_return_t 	err;
	recnum_t	recno;

	data_read_sync_num++;			/* statistics */

	/* Debug(printf("data_read: blkno=%d, size=%d, rablkno=%d, rasize=%d\n",
		     blkno, size, rablkno, rasize)); */

	/*
	 * Adapt Unix device addressing to the Mach mode.
	 */
	recno = blkno >> (devinfo->mrecshift - DISK_GSHIFT);
	size = dgtob(numblks);
	if (blkno & ((devinfo->mrecsize >> DISK_GSHIFT) - 1))
		panic("data_read: sec bounding");
	if (size & (devinfo->mrecsize - 1))
		panic("data_read: sec size");

 try_again:
	/*
	 * See if the request can be satisfied by the cache.
	 */
	if ((tag=(void *)cache_read_search(devinfo, blkno, numblks)) == NULL) {
		/*
		 * No buffer present in cache.  Do the following:
		 * - wait for any write conflicts to clear
		 * - perform the read operation
		 * - kick off a read-ahead if necessary.
		 */
		cache_write_conflict_detect(devinfo, blkno, numblks, TRUE);

		/*
		 * 'data' returned is a page size multiple and is guaranteed
		 * to be zero'd beyond the 'size' bytes requested.
		 */
		ux_server_thread_blocking();
		err = device_read(devinfo->devport, D_READ, recno,
				  size, (io_buf_ptr_t *) data, &count);
		ux_server_thread_unblocking();

		if (err != KERN_SUCCESS || size != count) {
			printf("Error: disk read, ");
			printf("block=%d size=%d count=%d err=0x%x\n", 
			       blkno, size, count, err);
			if (*data != NULL) {
				(void) vm_deallocate(mach_task_self(), 
						     *data, count);
				*data = NULL;
			}
		        if (err == KERN_SUCCESS)
				err = KERN_FAILURE;
			return(dev_error_to_errno(err));
		}

		if (rablkno) 
			data_read_async(devinfo, rablkno, ranumblks);
		
		return(KERN_SUCCESS);
	} else {
		/*
		 * Got a buffer.  Kick off a read-ahead first, if necessary.
		 */
		if (rablkno) 
			data_read_async(devinfo, rablkno, ranumblks);

		/*
		 * We have a buffer, now ask for the data (may involve 
		 * waiting until the data is valid).  Note that this
		 * routine also "returns" the tag to the cache.  
		 */

		if ((err = cache_get_data(tag, blkno, numblks, data))
		    == TRY_AGAIN) {
			/*
			 * The data we hit on was aborted;
			 * see data_read_async().
			 */
			data_read_try_again_got++;
			rablkno = 0;
			goto try_again;
		} else
			return(dev_error_to_errno(err));
	}
}


/*
 * Callback routine on completion of asynchronous disk reads.
 */
kern_return_t 
data_read_finish(tag, return_code, data, count)
	void		*tag;
	kern_return_t	return_code;
	vm_address_t 	data;
	unsigned int 	count;
{
#if	NCPUS == 1
	/*
	 * We don't need to take the master mutex at this point because
	 * the underlying code is using cthreads mutex's (which maintain
	 * their purported semantics even if NCPUS == 1).
	 */
#endif
	if (return_code != KERN_SUCCESS) {
		printf("Error: disk read completion, err=0x%x\n", return_code);
		if (data != NULL)
			(void) vm_deallocate(mach_task_self(), data, count);
		cache_read_abort(tag, return_code);
	} else
		cache_read_finish(tag, data, count);

	return(KERN_SUCCESS);
}


int
data_write(devinfo, blkno, data, numblks, synchronous)
	devinfo_t	*devinfo;
	daddr_t 	blkno;
	int 		numblks;
	vm_address_t 	data;
	boolean_t	synchronous;	
{
	register int	size;
	mach_port_t	reply_port;
	void		*tag;
	unsigned int	count;
	kern_return_t 	err;
	recnum_t	recno;


	if (synchronous)
		data_write_sync_num++;	   	/* statistics */
	else
		data_write_async_num++;	   	/* statistics */

	/*
	 * Adapt Unix device addressing to the Mach mode.
	 */
	recno = blkno >> (devinfo->mrecshift - DISK_GSHIFT);
	size = dgtob(numblks);
	if (blkno & ((devinfo->mrecsize >> DISK_GSHIFT) - 1))
		panic("data_write: sec bounding");
	if (size & (devinfo->mrecsize - 1))
		panic("data_write_async: sec size");

	/*
	 * Prepare the cache for a write operation
	 * (synchronizing with any pending write conflicts).
	 */
	if ((tag = (void *)cache_write_setup(devinfo, blkno, numblks, data,
							&reply_port)) == NULL) {
		printf("Error: disk data write can't setup - not written!!\n");
		return(dev_error_to_errno(KERN_FAILURE));
	}

	/*
	 * Abort any conflicting read-ahead operations.
	 */
	cache_read_conflict_remove(devinfo, blkno, numblks);

        /*
	 * Perform the write either synchronously or asynchronously. 
	 */
	if (synchronous) {
		ux_server_thread_blocking();
		err = device_write(devinfo->devport, D_WRITE, recno, 
				   (io_buf_ptr_t) data, size, (int *)&count);
		ux_server_thread_unblocking();

		if (err != KERN_SUCCESS) {
			printf("Error: disk write, ");
			printf("block=%d size=%d err=0x%x\n", blkno, size, err);
			cache_write_abort(tag);
		} else
			cache_write_finish(tag, count);
	} else {
		err = device_write_request(devinfo->devport, reply_port, 
					   D_WRITE, recno, 
					   (io_buf_ptr_t) data, size);
		if (err != KERN_SUCCESS) {
			/*
			 * Write request failed => no aysnc. reply expected.
			 */
			printf("Error: disk write request, ");
			printf("block=%d size=%d err=0x%x\n", blkno, size, err);
			cache_write_abort(tag);
		}
	}
	return(dev_error_to_errno(err));
}
	

/*
 * Callback routine on completion of asynchronous disk writes.
 */
kern_return_t 
data_write_finish(tag, return_code, count)
	void		*tag;
	kern_return_t	return_code;
	unsigned int 	count;
{
#if	NCPUS == 1
	/*
	 * We don't need to take the master mutex at this point because
	 * the underlying code is using cthreads mutex's (which maintain
	 * their purported semantics even if NCPUS == 1).
	 */
#endif
	if (return_code != KERN_SUCCESS) {
		printf("Error: disk write completion, err=0x%x\n", return_code);
		cache_write_abort(tag);
	} else
		cache_write_finish(tag, count);

	return(KERN_SUCCESS);
}
