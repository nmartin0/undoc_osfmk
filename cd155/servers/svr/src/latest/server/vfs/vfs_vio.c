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
 * HISTORY:
 * $Log: vfs_vio.c,v $
 * Revision 1.13  1995/03/04  23:18:31  sean
 * PTS #:		9441  (multiple test cases cause server panic at bcopy()+70)
 * Description:	The code path for the ufs S_fsvr_write_at_offset, can sometimes
 * 		deallocate the write buffer twice.  In when that happens
 * 		another thread which allocates memory between the two
 * 		deallocation is vunerable to panic.
 * Reviewer(s):	Bob Y., John L.
 * Risk:		Med
 * Testing:	vsx eats (full suite of 12 hours!)
 * Fix:		vio_wip_done deallocates the buffer only if there was
 * 		not an error, or if there is no way to report the error
 * 		back to ux_server_loop().  If ux_server_loop() is
 * 		informed of an error, then deallocation occurs there.
 *  Module(s):	server/ufs/ufs_vnops.c
 * 		server/vfs/vfs_vio.c
 *
 * Revision 1.12  1994/11/18  20:50:50  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/07/08  20:27:27  chrisp
 * Add missing ")" in call to vio_device_read_overwrite_finish() from
 * vio_device_read() inside #if VIO_SIMULATE_READ_OVERWRITE.
 *
 *  Reviewer: None
 *  Risk: None
 *  Benefit or PTS #: 10057
 *  Testing: Compiles for 386s
 *  Module(s): vfs_vio.c
 *
 * Revision 1.10  1994/06/28  23:24:16  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.9  1994/04/22  22:01:53  brad
 * Fixed typo if VIO_SIMULATE_READ_OVERWRITE turned on.  Bug introduced
 * in fix for 8784.
 *
 * Revision 1.8  1994/04/05  04:14:18  brad
 * Merged revisions 1.5.2.2 and 1.5.2.3 from the R1.2 branch.
 *
 * Revision 1.5.2.3  1994/04/05  04:06:27  brad
 * Add correct deallocation of buffer allocated by kernel to previous
 * checkin.  Same reviewer/testing/etc.
 *
 * Revision 1.5.2.2  1994/04/05  03:22:48  brad
 * Fixed error handling in vio_write() when error returned from
 * vio_device_write().  Also return EIO if kernel returns a bad count,
 * as well as display a warning.
 *
 *  Reviewer: Paul Roy @ OSF
 *  Risk: Low
 *  Benefit or PTS #: 8784
 *  Testing: PFS developer tests, fileio and PFS EATs on 64 nodes
 *  Module(s): server/vfs/vfs_vio.c
 *
 * Revision 1.7  1994/03/25  18:43:41  brad
 * Merged revision 1.5.2.1 from the R1.2 branch.
 *
 * Revision 1.5.2.1  1994/03/25  18:26:25  brad
 * Fixed improper calculation of read size if fragment being reallocated
 * at the end of a disk partition in realloccg_nbc().  Possible cause of
 * vio_device_read_synchronous panics.  Also changed
 * vio_device_read_synchronous panic in vfs_vio.c to a less severe warning,
 * and fixed kernel/driver error->errno mapping.
 *
 *  Reviewer: Bob Godley
 *  Risk: Low
 *  Benefit or PTS #: 8426
 *  Testing: Ran fileio/PFS EATs
 *  Module(s): server/ufs/ufs_alloc.c server/vfs/vfs_vio.c
 *
 * Revision 1.6  1994/01/11  18:26:14  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: less lint complaints
 *  Testing: compiled
 *  Module(s):
 * 	nfs/nfs_vnops.c
 * 	vfs/fifo_vnops.c
 * 	vfs/vfs_cache.c
 * 	vfs/vfs_flock.c
 * 	vfs/vfs_vnops.c
 * 	vfs/vfs_bio.c
 * 	vfs/vfs_subr.c
 * 	vfs/vfs_vio.c
 * 	vfs/spec_vnops.c
 * 	vfs/vfs_syscalls.c
 * 	vfs/vfs_lookup.c
 *
 * Revision 1.5  1993/11/30  01:02:14  brad
 * Fixed problem with improper write conflict detection being done on Fast
 * Path writes that are not sector-aligned.  This problem was causing PFS
 * data corruption.
 *
 *  Reviewer: Dave Minturn, Paul Roy (OSF)
 *  Risk: Medium
 *  Benefit or PTS #: 7239
 *  Testing: Ran ProSolver test from David Scott that originally (but very
 *     infrequently) reproduced the problem.  Also ran an engineering test
 *     created to reproduce the problem ... both ran many times successfully,
 *     where before they had failed.  Also ran PFS EATs.
 *  Module(s): server/ufs/ufs_vnops.c, server/vfs/vfs_vio.c
 *
 * Revision 1.4  1993/07/14  18:46:53  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/07/09  18:34:23  brad
 * Turned VIO_SIMULATE_READ_OVERWRITE off.
 *
 * Revision 1.1.1.3  1993/07/01  21:10:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1993/06/23  01:11:00  dbm
 * Added missing parameter to vio_read_complete call in vio_read().
 *
 * Revision 1.1.1.2  1993/05/20  19:55:24  cfj
 * 05-18-93 code drop from Locus.
 *
 * Revision 2.7  93/10/20  15:32:07  dnoveck
 *      DEV_BSIZE elimination: Change to accept disk addresses from the
 *      outside in terms of disk granules and convert to mach records for
 *      the kernel.
 *
 * Revision 2.4  93/04/14  10:04:39  roy
 * 	Turn on simulated read_overwrite temporarily.
 * 	[93/04/14            roy]
 * 
 * Revision 2.3  93/04/13  18:06:01  roy
 * 	Put simulated device_read_overwrite code under an ifdef.
 * 	Minor change to macros for event numbers.
 * 	[93/04/09            roy]
 * 
 * Revision 2.2  93/03/30  16:11:22  roy
 * 	Initial revision.
 * 	[93/03/08            roy]
 * 
 */

#include <sys/types.h>
#include <uxkern/import_mach.h>
#include <uxkern/device_reply_hdlr.h>
#include <uxkern/device_utils.h>
#include <device/device.h>
#include <kern/zalloc.h>
#include <sys/user.h>
#include <sys/time.h>
#include <sys/vnode.h>

/*
 * Read-in-progress structure.
 */
typedef struct vio_rip {
	struct mutex	lock;		/* protects this structure */
	queue_chain_t	chain;          /* next and prev links for free list */
	devinfo_t	*devinfo;	/* devinfo pointer */		
	short		ops_expected;   /* number of I/O's needed to complete */
	short		ops_completed;  /* number of I/O's completed so far */
	kern_return_t	error;		/* error code */
	mach_port_t	reply_port;     /* port used for async. i/o replies */
} vio_rip_t;

/* 
 * Write buffer structure.  A write-in-progress structure has a queue
 * of zero or more of these.
 */
typedef struct vio_wb {
	queue_chain_t	chain;          /* next and prev links */
	char		*buf;		/* pointer to buffer */
	unsigned int	bufsize;	/* size of buffer */
} vio_wb_t;

/*
 * Write-in-progress structure.
 */
typedef struct vio_wip {
	struct mutex	lock;		/* protects this structure */
	queue_chain_t	chain;          /* next and prev links for vnode list */
	struct vnode	*vp;		/* vnode for this request */
	devinfo_t	*devinfo;	/* devinfo pointer */		
	char		*buf;		/* pointer to buffer */
	unsigned int	bufsize;	/* size of buffer */
        queue_head_t	buf_list;	/* list of additional buffers */
	daddr_t		blkno;          /* logical block number of request */
	int		numblks;        /* number of logical blocks */
	short		ops_expected;   /* number of I/O's needed to complete */
	short		ops_completed;  /* number of I/O's completed so far */
	int		write_sync:1,	/* thread waiting for writes to */
					/*   complete? */
	                write_conflict:1; /* thread(s) waiting due to write */
					  /*   conflict? */
	kern_return_t	error;		/* error code */
	mach_port_t	reply_port;     /* port used for async. i/o replies */
} vio_wip_t;

/*
 * Data descriptor for read-ahead cache.
typedef struct vio_data {
} vio_data_t;
 */


#define	INITIAL_RIP_DESCS	16	/* pre-allocated rip descriptors */
#define	INITIAL_WIP_DESCS	16	/* pre-allocated wip descriptors */

int		vio_rip_cnt = 0;    	/* total num rip descriptors */
int		vio_wip_cnt = 0;  	/* total num wip descriptors */
int		vio_ripfree_cnt = 0;	/* num rip descs on free list */
int		vio_wipfree_cnt = 0;	/* num wip descs on free list */
int		vio_wb_inuse = 0;	/* num wb descs in use */

struct mutex	vio_ripfree_lock;	/* synchronize rip free list */
struct mutex	vio_wipfree_lock;	/* synchronize wip free list */
queue_head_t  	vio_ripfree_head; 	/* head of rip free list */
queue_head_t  	vio_wipfree_head; 	/* head of wip free list */
zone_t		vio_rip_zone;
zone_t		vio_wip_zone;
zone_t		vio_wb_zone;

/*
 * Used to determine if a write-in-progress structure overlaps 
 * with (bno, numb).
 */
#define vio_wip_overlap(wip, bno, numb) 			        \
  (((bno <= wip->blkno) && (bno + numb > wip->blkno)) || 	   	\
  ((bno > wip->blkno) && (bno < wip->blkno + wip->numblks)))		\

#define	VIO_LOCK_INIT(rip_or_wip)	mutex_init(&rip_or_wip->lock)
#define	VIO_LOCK(rip_or_wip)		mutex_lock(&rip_or_wip->lock)
#define	VIO_UNLOCK(rip_or_wip)		mutex_unlock(&rip_or_wip->lock)

#define	VIO_WFREE_LOCK_INIT()		mutex_init(&vio_wipfree_lock)
#define	VIO_WFREE_LOCK()		mutex_lock(&vio_wipfree_lock)
#define	VIO_WFREE_UNLOCK()		mutex_unlock(&vio_wipfree_lock)

#define	VIO_RFREE_LOCK_INIT()		mutex_init(&vio_ripfree_lock)
#define	VIO_RFREE_LOCK()		mutex_lock(&vio_ripfree_lock)
#define	VIO_RFREE_UNLOCK()		mutex_unlock(&vio_ripfree_lock)

/*
 * Event numbers used for assert_wait/thread_wakeup.  For wip's, a thread
 * may block either because it's waiting for synchronous completion of all
 * device writes or, because the wip represents a write that conflicts
 * with the operation (read or write) it's attempting to carry out.
 */
#define VIO_WRITE_SYNC_EVENT(wip)	((int)wip)
#define VIO_WRITE_CONFLICT_EVENT(wip)	(((int)wip)+sizeof(int))
#define VIO_READ_EVENT(rip)		((int)rip)

/*
 * Debugging statistics.
 */
#define	VIO_DEBUG	1

#if	VIO_DEBUG
#define	debug_incr_counter(x)	(x)++
#else
#define	debug_incr_counter(x)	
#endif

#if	VIO_DEBUG
int		vio_num_write_conflicts = 0;
int		vio_num_write_conflicts_multi = 0;
#endif	/* VIO_DEBUG */

/*
 * Whether to use a simulated version of device_read_overwrite().
 */
#define	VIO_SIMULATE_READ_OVERWRITE	0


/* internal routines */
vio_rip_t	*vio_rip_alloc();
vio_wip_t	*vio_wip_alloc();
void		vio_write_conflict_detect();
kern_return_t	vio_device_read_synchronous();
#if	VIO_SIMULATE_READ_OVERWRITE
kern_return_t	vio_device_read_overwrite_request();
#endif
kern_return_t	vio_device_read_overwrite_finish();
kern_return_t	vio_device_write_finish();
kern_return_t	vio_wip_done();

/*
 * Read-ahead provided by this module.
 * not-yet-impl.
 */
#define	VIO_READ_AHEAD	0


/*
 * Initialize the VIO module.
 */
void
vio_init()
{
	register vio_rip_t	*rip;
	register vio_wip_t	*wip;
	int			i;

	VIO_RFREE_LOCK_INIT();
	VIO_WFREE_LOCK_INIT();
	queue_init(&vio_ripfree_head);
	queue_init(&vio_wipfree_head);

	/* expandable zones by default */
	vio_rip_zone = zinit((vm_size_t)sizeof(vio_rip_t),
			     (vm_size_t)sizeof(vio_rip_t) * INITIAL_RIP_DESCS,
			     vm_page_size,
			     "vio rip desc");
	vio_wip_zone = zinit((vm_size_t)sizeof(vio_wip_t),
			     (vm_size_t)sizeof(vio_wip_t) * INITIAL_WIP_DESCS,
			     vm_page_size,
			     "vio wip desc");
	vio_wb_zone = zinit((vm_size_t)sizeof(vio_wb_t),
			     (vm_size_t)sizeof(vio_wb_t) * 16,
			     vm_page_size,
			     "vio wb desc");

	for (i = 0; i < INITIAL_RIP_DESCS; i++) {
		/*
		 * Allocate a rip and put it on the free list.
		 */
		rip = vio_rip_alloc();
		VIO_RFREE_LOCK();
		vio_rip_cnt++;
		vio_ripfree_cnt++;					
		queue_enter(&vio_ripfree_head, rip, vio_rip_t *, chain);
		VIO_RFREE_UNLOCK();
	}

	for (i = 0; i < INITIAL_WIP_DESCS; i++) {
		/*
		 * Allocate a wip and put it on the free list.
		 */
		wip = vio_wip_alloc();
		VIO_WFREE_LOCK();
		vio_wip_cnt++;
		vio_wipfree_cnt++;					
		queue_enter(&vio_wipfree_head, wip, vio_wip_t *, chain);
		VIO_WFREE_UNLOCK();
	}
}


/*
 * Allocate a read-in-progress structure.
 */
vio_rip_t *
vio_rip_alloc()
{
	vio_rip_t		*rip;

	rip = (vio_rip_t *) zalloc(vio_rip_zone);
	ASSERT(rip != NULL);
	VIO_LOCK_INIT(rip);
#ifdef	REPLY_PORT_ALIAS
	reply_hash_enter(&rip->reply_port, (char *) rip, 
			 vio_device_read_overwrite_finish, 
			 vio_device_write_finish);
#else
	rip->reply_port = mach_reply_port();
	reply_hash_enter(rip->reply_port, (char *) rip,
			 vio_device_read_overwrite_finish, 
			 vio_device_write_finish);
#endif
	return(rip);
}


/*
 * Allocate a write-in-progress structure.
 */
vio_wip_t *
vio_wip_alloc()
{
	vio_wip_t		*wip;

	wip = (vio_wip_t *) zalloc(vio_wip_zone);
	ASSERT(wip != NULL);
	VIO_LOCK_INIT(wip);
	queue_init(&wip->buf_list);
#ifdef	REPLY_PORT_ALIAS
	reply_hash_enter(&wip->reply_port, (char *) wip, 
			 vio_device_read_overwrite_finish, 
			 vio_device_write_finish);
#else
	wip->reply_port = mach_reply_port();
	reply_hash_enter(wip->reply_port, (char *) wip,
			 vio_device_read_overwrite_finish, 
			 vio_device_write_finish);
#endif
	return(wip);
}


/*
 * Prepare the file system for a read request covering the logical
 * range (blkno, numblks) within the file 'vp'.  
 * This routine:
 *	- allocates and fills in a data structure for the read
 *	- waits for conflicting write operations to complete
 *
 * The caller is returned an opague "tag" representing this request.
 * This tag may subsequently be provided to vio_device_read() 
 * to associate multiple device read operation with a single logical
 * request.  The tag must then subsequently be provided to vio_read_complete() 
 * in order to wait for all outstanding device operations to complete.
 *
 * Note:  a 'blkno' arg equal to -1 is a special value indicating that
 * the caller has already synchronized appropriately with outstanding
 * write-behinds, and hence shouldn't be done by this routine.
 */
void *
vio_read_setup(vp, devinfo, blkno, numblks)
	struct vnode		*vp;
	devinfo_t		*devinfo;
	daddr_t 		blkno;
	unsigned int		numblks;
{
	vio_rip_t		*rip;

	/*
	 * Allocate a read-in-progress structure and fill it in.
	 */
	VIO_RFREE_LOCK();
	if (!(queue_empty(&vio_ripfree_head))) {
		queue_remove_first(&vio_ripfree_head, rip, vio_rip_t *, chain);
		vio_ripfree_cnt--;					
		VIO_RFREE_UNLOCK();
	} else {
		vio_rip_cnt++;
		VIO_RFREE_UNLOCK();
		/* printf("vfs_vio:  allocating rip. num=%d\n", vio_rip_cnt); */
		rip = vio_rip_alloc();
	}

	rip->devinfo = devinfo;
	rip->ops_expected = -1;		/* invalid value */
	rip->ops_completed = 0;
	rip->error = KERN_SUCCESS;

	/* 
	 * Wait for any write conflicts to clear, but only if 
	 * the caller hasn't already synchronized appropriately 
	 * (indicated by blkno == -1).
	 */
	if (blkno != -1) {
		VN_BUFLISTS_LOCK(vp);
		vio_write_conflict_detect(vp, blkno, numblks, TRUE);
		VN_BUFLISTS_UNLOCK(vp);
	}

	return((void *) rip);
}

/*
 * Given a tag, specify that the read is complete.  
 * 'numops' tells this routine how many read operations were initiated,
 * and hence how many it should wait for.
 *
 * After invocation, callers of this routine may NOT use the tag again.
 */
kern_return_t
vio_read_complete(tag, numops)
	void			*tag;
	unsigned int		numops;
{
	vio_rip_t		*rip = (vio_rip_t *) tag;
	int			error;

	if (numops != 0) {
		VIO_LOCK(rip);
		if (rip->ops_completed != numops) {
			/* Wait until all ops have completed. */
			rip->ops_expected = numops;
			assert_wait(VIO_READ_EVENT(rip), FALSE);
			VIO_UNLOCK(rip);
			thread_block();
			ASSERT(rip->ops_expected == rip->ops_completed);
		} else
			VIO_UNLOCK(rip);
		/*
		 * Only one thread may be waiting for the ops to complete.
		 *	=> no need to hold rip lock.
		 */
		error = rip->error;
		ASSERT(error != -1);	/* sanity check */
		rip->error = -1;

	} else {
		ASSERT(rip->ops_completed == 0);
		error = KERN_SUCCESS;
	}
		
	VIO_RFREE_LOCK();
	vio_ripfree_cnt++;
	queue_enter(&vio_ripfree_head, rip, vio_rip_t *, chain);
	VIO_RFREE_UNLOCK();

	return(error);
}

/*
 * Perform a synchronous file read operation.  A new buffer containing 
 * the data will be pointed to by *buf upon successfully returning.
 *
 * (blkno, numblks) is the logical range within the file and is used
 * to synchronize with other active requests.  (secno, numdg) is
 * the physical range on the device and is used in the I/O operation.
 *
 * The 'synchonize' parameter indicates whether to synchronize with other 
 * I/O operations.
 */
kern_return_t
vio_read(vp, devinfo, blkno, numblks, dgran, numdg, synchronize, buf)
	struct vnode		*vp;
	devinfo_t		*devinfo;			
	daddr_t 		blkno;
	unsigned int		numblks;
	daddr_t 		dgran;
	unsigned int		numdg;
	boolean_t		synchronize;
	char			**buf;
{
	void			*tag;
	int			error;

	/*
	 * Prepare for a read.  A 'blkno' arg of -1 tells vio_read_setup
	 * not to synchronize.
	 */
	tag = vio_read_setup(vp, devinfo, synchronize ? blkno : -1, numblks);
	
	error = vio_device_read_synchronous(tag, dgran, numdg, buf);

	(void) vio_read_complete(tag, 0);

	return(error);
}


#if	VIO_SIMULATE_READ_OVERWRITE
/*
 * XXX Temporary hack until device_read_overwrite is really implemented.
 */
kern_return_t
vio_device_read_overwrite_request(devport, reply_port, mode, recnum, count,
				  data)
	mach_port_t		devport;
	mach_port_t		reply_port;
	dev_mode_t		mode;
	recnum_t		recnum;
	mach_msg_type_number_t	count;
	io_buf_ptr_t		data;
{
	unsigned int		amount;
	io_buf_ptr_t		return_data = NULL;
	kern_return_t		err;

	err = device_read(devport, mode, recnum, count, &return_data, &amount);

	if (err != KERN_SUCCESS) {
		printf("Error: vio_device_read_overwrite_request: err=0x%x\n",
		       err);
		return(dev_error_to_errno(err));
	}
	if (count != amount || return_data == NULL) {
		printf("Error: vio_device_read_overwrite_request: \n");
		printf("       block=%d count=%d amount=%d data=%x\n",
		       recnum, count, amount, return_data);
		err = EIO;
		goto out;
	}

	bcopy((char *)return_data, (char *) data, count);
	
out:
	if (return_data != NULL) {
		if (err = vm_deallocate(mach_task_self(),
					(vm_address_t) return_data, 
					(vm_size_t) count))
			panic("read_overwrite.vm_deallocate err=0x%x\n", err);
	}
	return(err);
}
#endif	/* VIO_SIMULATE_READ_OVERWRITE */


/*
 * Perform a sychronous device read operation into the supplied 
 * buffer.
 *
 * The operation is associated with the logical read request 
 * indicated by 'tag'.
 */
kern_return_t
vio_device_read_synchronous(tag, dgran, numdg, buf)
	void			*tag;
	daddr_t 		dgran;
	unsigned int		numdg;
	char			**buf;
{
	vio_rip_t		*rip = (vio_rip_t *) tag;
	struct devinfo		*devinfo = rip->devinfo;
	unsigned int		amount, count;
	kern_return_t		err;
	recnum_t		recno;


	/*
	 * Adapt Unix device addressing to the Mach mode.
	 */
	recno = dgran >> (devinfo->mrecshift - DISK_GSHIFT);
	count = dgtob(numdg);
	if (dgran & ((devinfo->mrecsize >> DISK_GSHIFT) - 1))
		panic("vio_device_read_synchronous: sec bounding");
	if (dgtob(numdg) & (devinfo->mrecsize - 1))
		panic("vio_device_read_synchronous: sec size");

	/*
	 * Perform the synchronous read. 
	 */
	*buf = NULL;

	ux_server_thread_blocking();
	err = device_read(rip->devinfo->devport, D_READ, recno,
			  count, buf, &amount);
	ux_server_thread_unblocking();

	if (err != KERN_SUCCESS) {
		printf("Error: vio_device_read_synchronous: err=0x%x\n", err);
		return(dev_error_to_errno(err));
	}
	if (count != amount || *buf == NULL) {
		printf("Error: vio_device_read_synchronous: \n");
		printf("       block=%d count=%d amount=%d data=%x\n",
		       dgran, count, amount, *buf);
		if (*buf != NULL) {
			(void) vm_deallocate(mach_task_self(),
					     (vm_offset_t)*buf, count);
			*buf = NULL;
		}
		return(EIO);
	}

	return(KERN_SUCCESS);
}


/*
 * Perform an asychronous device read operation into the supplied 
 * buffer.
 *
 * The operation is associated with the logical read request 
 * indicated by 'tag'.
 */
kern_return_t
vio_device_read(tag, dgran, numdg, buf)
	void			*tag;
	daddr_t 		dgran;
	unsigned int		numdg;
	char			*buf;
{
	vio_rip_t		*rip = (vio_rip_t *) tag;
	kern_return_t		err;
	recnum_t		recno;
	struct devinfo		*devinfo = rip->devinfo;


	ASSERT(buf != NULL);

	u.u_ru.ru_inblock++;		/* pay for reads */

	recno = dgran >> (devinfo->mrecshift - DISK_GSHIFT);
	if (dgran & ((devinfo->mrecsize >> DISK_GSHIFT) - 1))
		panic("vio_device_read: sec bounding");
	if (dgtob(numdg) & (devinfo->mrecsize - 1))
		panic("vio_device_read: sec size");

	/*
	 * Perform the asynchronous read. 
	 */
	ux_server_thread_blocking();

#if	VIO_SIMULATE_READ_OVERWRITE
	/* this is synchronous */
	vio_device_read_overwrite_request(devinfo->devport, 
					  rip->reply_port, 
					  D_READ, recno, dgtob(numdg),
					  buf);
	err = vio_device_read_overwrite_finish(tag, KERN_SUCCESS, 
					      dgtob(numdg));
#else
	err = device_read_overwrite_request(rip->devinfo->devport, 
					    rip->reply_port, 
					    D_READ, recno, dgtob(numdg), 
					    buf);
#endif
	ux_server_thread_unblocking();
	if (err != KERN_SUCCESS) {
		/*
		 * Read request failed => no aysnc. reply expected.
		 */
		printf("Error: vio_device_read: block=%d size=%d ret=0x%x\n", 
		       dgran, dgtob(numdg), err);
		err = dev_error_to_errno(err);
	}
	
	return(err);
}


/*
 * Called upon completion of a device_read_overwrite operation.
 */
kern_return_t
vio_device_read_overwrite_finish(tag, return_code, count)
	void			*tag;
	kern_return_t		return_code;
	unsigned int		count;
{
	vio_rip_t		*rip = (vio_rip_t *) tag;

	/*
	 * Key Assumption:
	 * For disk devices, all read/write requests within the range
	 * of the device (i.e., end of partition is not encountered)
	 * should either:
	 * - read/write the entire amount, or
	 * - read/write less than the entire amount AND return a non-zero
	 *   error code.
	 *
	 * Depending on this guarantee is important because it allows us
	 * to avoid allocating a per-device request structure to remember
	 * the original request amount.  Also, it avoids the need to
	 * loop in this layer.
	 */
	VIO_LOCK(rip);
	if (return_code != KERN_SUCCESS) {
		printf("Error: vio_device_read_overwrite_finish: ret=0x%x\n",
		       return_code);
		rip->error = dev_error_to_errno(return_code);
	} else if ((int)count <= 0) 
		panic("vio_overwrite_finish: bad count=%d, err=\n",
		      count);

	if (++rip->ops_completed == rip->ops_expected)
		/*
		 * All ops have completed:  
		 * - there must me a thread waiting, so wake it up.
		 */
		thread_wakeup(VIO_READ_EVENT(rip));

	VIO_UNLOCK(rip);
	return(KERN_SUCCESS);
}


/* 
 * Detect write conflicts.  'wait' arg specifies whether to 
 * wait for conflicts to clear or just return.
 *
 * This routine must be called with the VN_BUFLISTS_LOCK held.
 * Note, however, that during execution the lock may be temporarily
 * released and reacquired.
 */
void
vio_write_conflict_detect(vp, blkno, numblks, wait)
	struct vnode		*vp;
	daddr_t 		blkno;
	unsigned int		numblks;
	boolean_t		wait;
{
	vio_wip_t		*tempwip;
	boolean_t		conflict = FALSE;

	/*
	 * XXX Currently, only wait==TRUE is supported.
	 */
	ASSERT(wait == TRUE);

 try_again:
	for (tempwip = (vio_wip_t *) queue_first(&vp->v_wip_list);
	     !queue_end(&vp->v_wip_list, (queue_entry_t) tempwip);
	     tempwip = (vio_wip_t *) queue_next(&tempwip->chain)) {
		VIO_LOCK(tempwip);
		if ((vio_wip_overlap(tempwip, blkno, numblks))) {
			VN_BUFLISTS_UNLOCK(vp);
			debug_incr_counter(vio_num_write_conflicts);
			if (conflict)
			    debug_incr_counter(vio_num_write_conflicts_multi);
			tempwip->write_conflict = 1;
			assert_wait(VIO_WRITE_CONFLICT_EVENT(tempwip), FALSE);
			VIO_UNLOCK(tempwip);
			thread_block();
			/*
			 * May not use tempwip after unblocking because it
			 * may have been deallocated.
			 *
			 * It's possible that this write conflicted
			 * with multiple write-pending ops.
			 */
			conflict = TRUE;
			VN_BUFLISTS_LOCK(vp);
			goto try_again; 	
		} 
		VIO_UNLOCK(tempwip);
	}
}


/*
 * Prepare the file system for a write request covering the logical
 * range (blkno, numblks) within the file 'vp'.
 * This routine:
 *	- allocates and fills in a data structure for the write
 *	- inserts it in the write-in-progress list
 *	- removes conflicting read-ahead operations
 *
 * The caller is returned an opague "tag" representing this request.
 * This tag may subsequently be provided to vio_device_write() to
 * associate multiple device write operation with a single logical
 * request.  The caller must eventually either wait for all device operations
 * to complete, via vio_write_wait(), or simply specify how many operations 
 * have been initiated, via vio_device_writes_initiated().
 */
void *
vio_write_setup(vp, devinfo, buf, bufsize, blkno, numblks)
	struct vnode		*vp;
	devinfo_t		*devinfo;			
	char			*buf;
	unsigned int		bufsize;
	daddr_t 		blkno;
	unsigned int		numblks;
{
	vio_wip_t		*newwip;

	/*
	 * Allocate a write-in-progress structure and fill it in.
	 */
	VIO_WFREE_LOCK();
	if (!(queue_empty(&vio_wipfree_head))) {
		queue_remove_first(&vio_wipfree_head, newwip, vio_wip_t *, 
				   chain);
		vio_wipfree_cnt--;					
		VIO_WFREE_UNLOCK();
	} else {
		vio_wip_cnt++;
		VIO_WFREE_UNLOCK();
		/* printf("vfs_vio:  allocating wip. num=%d\n", vio_wip_cnt); */
		newwip = vio_wip_alloc();
	}
	ASSERT(queue_empty(&newwip->buf_list));

	newwip->vp = vp;
	newwip->devinfo = devinfo;
	newwip->buf = buf;
	newwip->bufsize = bufsize;
	newwip->blkno = blkno;
	newwip->numblks = numblks;
	newwip->ops_expected = -1;		/* invalid value */
	newwip->ops_completed = 0;
	newwip->write_sync = newwip->write_conflict = 0;
	newwip->error = KERN_SUCCESS;

	/*
	 * Wait on conflicting writes that are pending, and then insert
	 * the new wip on the vnode's wip queue.  
	 *
	 * Note that performing the wait+insert sequence atomically really
	 * shouldn't be necessary if the higher layers guarantee that
	 * there never will be simultaneous, conflicting writes to the 
	 * same data.  However, the extra measure of caution is implemented
	 * anyway for now.  This decision should be revisited.
	 */
	VN_BUFLISTS_LOCK(vp);
	vio_write_conflict_detect(vp, blkno, numblks, TRUE);
	queue_enter(&vp->v_wip_list, newwip, vio_wip_t *, chain);
	VN_BUFLISTS_UNLOCK(vp);

	VN_OUTPUT_LOCK(vp);
	vp->v_numoutput++;	/* track write requests in progress */
	VN_OUTPUT_UNLOCK(vp);

#if	VIO_READ_AHEAD
	/*
	 * Abort any conflicting read-ahead operations.
	 */
	vio_read_conflict_remove(vp, blkno, numblks);
#endif

	return((void *) newwip);
}


/*
 * Perform postprocessing on a wip descriptor for which all device writes
 * have completed.  An error code indicating the collective success or
 * or failure of the device writes is returned.
 */
kern_return_t
vio_wip_done(wip)
	vio_wip_t		*wip;
{
	struct vnode		*vp = wip->vp;
	vio_wb_t		*wb;
	int			error, wakeup = 0;

	/*
	 * - Unlink the wip from the vnode wip queue.  This prevents any
	 *   additional threads from blocking on this wip due to a write 
	 *   conflict.  
	 * - Wakeup any threads blocked on the wip due to a write conflict.
	 *   All threads blocked are guaranteed not to access the wip 
	 *   again when they wake up (see vio_write_conflict_detect).
	 * - Free the buffer.
	 * - Perform write-in-progress bookkeeping.
	 * - Free the wip.
	 */
	VN_BUFLISTS_LOCK(vp);
	queue_remove(&vp->v_wip_list, wip, vio_wip_t *, chain);
	VN_BUFLISTS_UNLOCK(vp);

	VIO_LOCK(wip);	      	
	if (wip->write_conflict)
		thread_wakeup(VIO_WRITE_CONFLICT_EVENT(wip));

	/*
	 * Free the buffer and any additional buffers that may have
	 * been associated with this wip.
	 *
	 * If an error is to be returned, then ux_server_loop()
	 * will deallocate the buffer.  If the request is asynchronous
	 * or error-free, then deallocation occurs here.
	 */
	if (wip->buf != NULL && !(wip->write_sync && wip->error)) {
		if ((error = vm_deallocate(mach_task_self(), 
					   (vm_address_t) wip->buf, 
					   (vm_size_t) wip->bufsize))
		    != KERN_SUCCESS) 
			panic("vio_writes_initiated.vm_deallocate 0x%x\n", 
			      error);
	} 
	while (!(queue_empty(&wip->buf_list))) {
		queue_remove_first(&wip->buf_list, wb, vio_wb_t *, chain);
		if ((error = vm_deallocate(mach_task_self(), 
					   (vm_address_t) wb->buf, 
					   (vm_size_t) wb->bufsize))
		    != KERN_SUCCESS) 
			panic("vio_writes_initiated.vm_deallocate2 0x%x\n", 
			      error);
		vio_wb_inuse--;
		zfree(vio_wb_zone, wb);
	}
	ASSERT(queue_empty(&wip->buf_list));

	error = wip->error;
	ASSERT(error != -1);	/* sanity */
	wip->error = -1;
	VIO_UNLOCK(wip);

	/*
         * Track write requests in progress.
	 */
	VN_OUTPUT_LOCK(vp);
	ASSERT(vp->v_numoutput > 0);
	vp->v_numoutput--;
	if ((vp->v_outflag & VOUTWAIT) && vp->v_numoutput <= 0) {
		vp->v_outflag &= ~VOUTWAIT;
		wakeup++;
	}
	VN_OUTPUT_UNLOCK(vp);
	if (wakeup)
		thread_wakeup((int)&vp->v_numoutput);

	VIO_WFREE_LOCK();
	vio_wipfree_cnt++;
	queue_enter(&vio_wipfree_head, wip, vio_wip_t *, chain);
	VIO_WFREE_UNLOCK();

	return(error);
}


void
vio_write_wait_error(tag, numops, error)
	void		*tag;
	unsigned int	numops;
	kern_return_t	error;
{
	vio_wip_t	*wip = (vio_wip_t *)tag;

	wip->error = error;
	(void)vio_write_wait(tag, numops);
	return;
}


/*
 * Given a tag, wait for all associated write operations to complete.
 * 'numops' tells this routine how many write operations were initiated.
 * It can tolerate a numops value of 0.
 *
 * After invocation, callers of this routine may NOT use the tag again.
 */
kern_return_t
vio_write_wait(tag, numops)
	void			*tag;
	unsigned int		numops;
{
	vio_wip_t		*wip = (vio_wip_t *) tag;

	VIO_LOCK(wip);
	ASSERT(!wip->write_sync);
	wip->write_sync = 1;
	if (wip->ops_completed != numops) {
		/* Wait until all ops have completed. */
		wip->ops_expected = numops;
		assert_wait(VIO_WRITE_SYNC_EVENT(wip), FALSE);
		VIO_UNLOCK(wip);
		thread_block();
		ASSERT(wip->ops_expected == wip->ops_completed);
	} else
		VIO_UNLOCK(wip);

	/*
	 * All ops have completed:  perform wip postprocessing.
	 */
	return(vio_wip_done(wip));
}



/*
 * For an active write request, specify the number of device write 
 * operations initiated.  This interface is used by threads not wanting
 * to wait for the device operations to complete (cf. vio_write_wait).
 * It can tolerate a numops value of 0.
 *
 * This allows the vio subsystem to know when all initiated I/O's are 
 * completed, thus allowing it to perform postprocessing on the wip.
 *
 * After invocation, callers of this routine may NOT use the tag again.
 */
void
vio_device_writes_initiated(tag, numops)
	void			*tag;
	unsigned int		numops;
{
	vio_wip_t		*wip = (vio_wip_t *) tag;

	VIO_LOCK(wip);
	ASSERT(!wip->write_sync);
	if (wip->ops_completed != numops) {
		/*
		 * Not all ops have completed.  Simply specify
		 * the number of ops initiated and leave
		 * it to vio_device_write_finish to free the buffer.
		 */
		wip->ops_expected = numops;
		VIO_UNLOCK(wip);
		return;
	}
	/*
	 * All ops have completed:  perform wip postprocessing.
	 * Must free the wip lock first to avoid deadlock with the
	 * VN_BUFLISTS_LOCK.
	 */
	VIO_UNLOCK(wip);  	
	(void) vio_wip_done(wip);
}


/*
 * Given a tag, associate a new buffer with it.  When all operations
 * are complete this buffer will be deallocated by the vio module.
 */
void
vio_assoc_write_buf(tag, buf, bufsize)
	void			*tag;
	char			*buf;
	unsigned int		bufsize;
{
	vio_wip_t		*wip = (vio_wip_t *) tag;
	vio_wb_t		*newwb;

	/*
	 * Allocate a write buffer structure and fill it in.
	 */
	newwb = (vio_wb_t *) zalloc(vio_wb_zone);
	ASSERT(newwb != NULL);
	vio_wb_inuse++;
	newwb->buf = buf;
	newwb->bufsize = bufsize;

	VIO_LOCK(wip);
	queue_enter(&wip->buf_list, newwb, vio_wb_t *, chain);
	VIO_UNLOCK(wip);
}

/*
 * Perform a file write operation from the supplied buffer.
 *
 * (blkno, numblks) is the logical range within the file and is used
 * to synchronize with other active requests.  (dgran, numdg) is
 * the physical range on the device and is used in the I/O operation.
 *
 * XXX The 'synchonize' parameter is obsolete and should be deleted.
 *
 * The 'synchronous' parameter specifies whether the write should be 
 * performed synchronously.  In either case, the buffer is deallocated 
 * when the device operation is complete.
 */
kern_return_t
vio_write(vp, devinfo, buf, bufsize, blkno, numblks, dgran, numdg,
	  synchronize, synchronous)
	struct vnode		*vp;
	devinfo_t		*devinfo;			
	char			*buf;
	unsigned int		bufsize;
	daddr_t 		blkno;
	unsigned int		numblks;
	daddr_t 		dgran;
	unsigned int		numdg;
	boolean_t		synchronize;
	boolean_t		synchronous;
{
	void			*tag;
	int			error;

	ASSERT(synchronize == TRUE);

	tag = vio_write_setup(vp, devinfo, buf, bufsize, blkno, numblks);
	
	if (error = vio_device_write(tag, buf, dgran, numdg)) {
		vio_device_writes_initiated(tag, 0);
		return(error);
	}

	if (synchronous) 
		return(vio_write_wait(tag, 1));
	else {
		vio_device_writes_initiated(tag, 1);
		return(KERN_SUCCESS);
	}
}


/*
 * Perform a device write operation from the supplied buffer.
 * The operation is associated with the logical write request 
 * indicated by 'tag'.
 *
 * Note: the device write operation is performed asynchronously.
 */
kern_return_t
vio_device_write(tag, buf, dgran, numdg)
	void			*tag;
	char			*buf;
	daddr_t 		dgran;
	unsigned int		numdg;
{
	vio_wip_t		*wip = (vio_wip_t *) tag;
	kern_return_t		err;

	recnum_t		recno;
	struct devinfo		*devinfo = wip->devinfo;

	/*
	 * Adapt Unix device addressing to the Mach mode.
	 */
	recno = dgran >> (devinfo->mrecshift - DISK_GSHIFT);
	if (dgran & ((devinfo->mrecsize >> DISK_GSHIFT) - 1))
		panic("vio_device_write: sec bounding");
	if (dgtob(numdg) & (devinfo->mrecsize - 1))
		panic("vio_device_write: sec size");

	u.u_ru.ru_oublock++;		/* pay for writes */

	err = device_write_request(wip->devinfo->devport, wip->reply_port, 
				   D_WRITE, recno,
				   (io_buf_ptr_t) buf, 
				   dgtob(numdg));
	if (err != KERN_SUCCESS) {
		/*
		 * Write request failed => no aysnc. reply expected.
		 */
		printf("Error: vio disk write request, ");
		printf("block=%d size=%d err=0x%x\n", dgran,
			dgtob(numdg), err);
		err = dev_error_to_errno(err);
	}

	return(err);
}


/*
 * Called upon completion of a device write operation.
 */
kern_return_t
vio_device_write_finish(tag, return_code, count)
	void			*tag;
	kern_return_t		return_code;
	unsigned int		count;
{
	vio_wip_t		*wip = (vio_wip_t *) tag;

	VIO_LOCK(wip);
	if (return_code != KERN_SUCCESS) {
		printf("Error: vio_device_write_finish: ret=0x%x\n",
		       return_code);
		wip->error = dev_error_to_errno(return_code);
	} else if ((int)count <= 0) 
		panic("vio_device_write_finish: bad count=%d, err=\n", count);

	/* printf("write_finish: ops_completed=%d  ops_expected=%d\n", 
	       wip->ops_completed+1, wip->ops_expected); */

	if (++wip->ops_completed == wip->ops_expected) {
		/*
		 * All ops have completed.  If there's a thread waiting for
		 * completion, then wake it up so that it may perform
		 * wip postprocessing.  Otherwise, we know there won't 
		 * eventually be a thread waiting for completion because 
		 * ops_expected must have been set to a valid value by
		 * vio_device_writes_initiated, so we must perform wip
		 * postprocessing now.
		 */
		if (wip->write_sync) {
			thread_wakeup(VIO_WRITE_SYNC_EVENT(wip));
			VIO_UNLOCK(wip);
		} else {
			VIO_UNLOCK(wip);  	
			(void) vio_wip_done(wip);
		}
	} else
		VIO_UNLOCK(wip);
	
	return(KERN_SUCCESS);
}


#ifdef not_needed

/*
 * Called to synchronize with operations in progress or cached data
 * associated with a vnode.  
 */
void
vio_synchronize(vp, blkno, numblks)
	struct vnode		*vp;
	daddr_t 		blkno;
	unsigned int		numblks;
{
	void			*tag;

	/*
	 * Synchronize with write-behinds.  The easiest way to do this
	 * is to prepare for a write and then indicate that no writes
	 * actually occurred.
	 */
	tag = vio_write_setup(vp, NULL, NULL, 0, blkno, numblks);
	vio_device_writes_initiated(tag, 0);
}

/*
 * Wait for all write-behinds associated with a file to complete.
 */
void
vio_fsync(vp)
	struct vnode		*vp;
{
	/*
	 * Wait for all write requests in progress to complete.
	 */
	VN_OUTPUT_LOCK(vp);
	while (vp->v_numoutput) {
		vp->v_outflag |= VOUTWAIT;
		assert_wait((int)&vp->v_numoutput, FALSE);
		VN_OUTPUT_UNLOCK(vp);
		thread_block();
		VN_OUTPUT_LOCK(vp);
	}
	VN_OUTPUT_UNLOCK(vp);
}
#endif  /* not_needed */

#if	VIO_READ_AHEAD

/*
 * Perform a device read-ahead operation.  
 *
 * (blkno, numblks) represents the logical range within the file 
 * and (dgran, numdg) represents the physical range on the device.
 * This routine:
 *	- allocates and fills in a data structure for the read
 *	- inserts it in the read-ahead cache, aborting if it
 *	  overlaps with an already-cached entry
 *	- waits for conflicting write operations to complete
 *	- performs the read and returns.
 *
 * Note: the read-ahead is performed asynchronously.
 */
kern_return_t
vio_read_ahead(vp, devinfo, blkno, numblks, dgran, numdg)
	struct vnode		*vp;
	devinfo_t		*devinfo;			
	daddr_t 		blkno;
	unsigned int		numblks;
	daddr_t 		dgran;
	unsigned int		numdg;
{
}


/*
 * Search the read-ahead cache for a buffer satisfying 
 * (vp, lblkno, numblks).  If found, return a pointer to it, 
 * else return NULL.
 *
 * This routine will wait for read-aheads in progress to
 * complete, if necessary.
 */
void *
vio_read_search(vp, blkno, numblks)
	struct vnode		*vp;
	daddr_t 		blkno;
	unsigned int		numblks;
{
}


/*
 * Abort any conflicting read-ahead operations.
 */
void
vio_read_conflict_remove(vp, blkno, numblks)
	struct vnode		*vp;
	daddr_t 		blkno;
	unsigned int		numblks;
{
}

#endif 	/* VIO_READ_AHEAD */	
