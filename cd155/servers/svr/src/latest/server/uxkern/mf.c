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
 * HISTORY
 * $Log: mf.c,v $
 * Revision 1.18  1994/11/18  20:48:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/06/21  18:14:34  jlitvin
 * Fix typo found by lint - the variable vmp was never initialized in the
 * mf_sync_data() function.
 *
 *  Reviewer: dbm
 *  Risk: low
 *  Benefit or PTS #: 9411
 *  Testing: PFS EAT
 *  Module(s): server/uxkern/mf.c
 *
 * Revision 1.16  1994/03/09  00:52:26  dbm
 * Mainline update of R1.2 revision 1.14.2.1
 *
 * Revision 1.15  1994/01/12  17:46:16  jlitvin
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
 * Revision 1.14.2.1  1994/03/09  00:34:51  dbm
 * Added mf_sync_data() to support O_SYNC mode with mapped files.
 *  Reviewer: Brad Rullman
 *  Risk: Low
 *  Benefit or PTS #: 8420
 *  Testing: Specific test case using O_SYNC, PFS eats.
 *  Module(s):
 * 	mf.c
 *
 * Revision 1.14  1993/10/28  03:15:50  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.13  1993/08/19  00:47:16  cfj
 * Modified mf_trunc() so that if the call to vnode_pager_flush() returns an error, the
 * underlying file system is still updated.  Should avoid leaving blocks allocated to
 * deallocated inodes.
 *
 * Revision 1.12  1993/07/14  18:42:53  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:03:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.11  1993/06/01  22:23:18  nandy
 * mf_write() fixed to handle APPEND.
 *
 * Revision 1.10  1993/05/12  00:23:52  brad
 * Changed a DEBUG_PFS to DEBUG_PFSTOKEN.
 *
 * Revision 1.9  1993/05/07  18:45:15  nandy
 * Fixed a merge conflict
 *
 * Revision 1.8  1993/05/06  19:30:43  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:52:45  cfj
 * Initial 1.0.3 code drop
 * Revision 1.6  1993/04/09  23:21:47  cfj
 * Merge with T9.5.
 *
 * Revision 1.3.6.2  1993/04/09  22:13:48  cfj
 * In mf_clean(), break large lock_requests into smaller chunks.
 *
 * Revision 1.5  1993/04/03  03:12:16  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.3  1993/03/11  00:38:50  dbm
 * Put in fix for ^C on PFS files.  This resets the length of the PFS file
 * to VNOVAL if the process that is holding on to the token terminates
 * Aby Normally so that the size can be recomputed by the emulator.
 *
 * Revision 1.4  1993/03/08  18:25:44  nandy
 * Merged from T9 tree
 *
 * Revision 1.3.6.1  1993/03/06  23:43:19  nandy
 * Changes from LOCUS for the new locking mechanism.
 *
 * Revision 2.19  93/01/12  17:06:40  roy
 * 	Comment out an error check in mf_token_acquire_with_mo.
 * 	[93/01/11            roy]
 * 
 * Revision 1.1.2.1.2.1  1992/12/16  06:05:27  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  22:52:32  dbm
 * Added PFS token functionality.
 *
 * Revision 1.2  1992/11/30  22:55:13  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.18  92/11/17  19:53:25  loverso
 * 	Store temporary and cacheable flags in the vm_info struct so
 * 	that mf_uncache and mf_temporary will affect memory objects
 * 	that aren't active but will become so.  
 * 
 * 	Change mf_write to use user_bcopy2, which can handle exceptions,
 * 	for ENOSPC in the nfs server.  (mmp)
 * 	[92/11/15            roy]
 * 
 * Revision 2.17  92/11/05  17:27:32  roy
 * 	Make no-more-senders handling more robust.
 * 	[92/11/02            roy]
 * 
 * Revision 2.21  93/06/16  13:54:25  klh
 * 	Revision 2.25  93/06/02  17:22:44  rabii
 * 		mf_write now supports IO_APPEND.
 * 		[93/06/02            roy]
 * 
 * 	Revision 2.24  93/05/13  16:45:20  roy
 * 		Removed mf_get_size_for_pageout.
 * 		Clean and flush last page in mf_trunc.
 * 		[93/05/05            roy]
 * 
 * Revision 2.20  93/04/29  14:06:46  klh
 * 	Revision 2.23  93/04/14  09:26:57  roy
 * 		Up max size related to last change to 16M.
 * 		[93/04/14            roy]
 *
 * 	Revision 2.22  93/04/13  18:15:01  roy
 * 		Limit the max size passed to vnode_pager_flush in mf_clean and
 * 		mf_flush until XMM can gracefully handle unlimited sizes.
 * 		[93/04/13            roy]
 *
 * 	Revision 2.21  93/03/25  10:36:32  durriya
 * 		Return an error instead of panicing when inode_pager_setup fails.
 * 		[93/03/18            roy]
 *
 * 	Revision 2.20  93/03/22  23:58:15  condict
 * 		Changed cthread_yield to thread_yield.  (See kern/sched_prim.c)
 *
 * 	Revision 2.19  93/01/12  17:06:40  roy
 * 		Comment out an error check in mf_token_acquire_with_mo.
 * 		[93/01/11            roy]
 *
 * Revision 2.19  93/03/22  21:18:59  yazz
 * OSF lock changes.  Change cthread_yield() calls to thread_yield() and
 * add a missing ux_server_thread_blocking/unblocking() pair of calls.
 * 
 * Revision 2.18  92/11/17  19:53:25  loverso
 * 	Store temporary and cacheable flags in the vm_info struct so
 * 	that mf_uncache and mf_temporary will affect memory objects
 * 	that aren't active but will become so.  
 * 
 * 	Change mf_write to use user_bcopy2, which can handle exceptions,
 * 	for ENOSPC in the nfs server.  (mmp)
 * 	[92/11/15            roy]
 * 
 * Revision 2.17  92/11/05  17:27:32  roy
 * 	Make no-more-senders handling more robust.
 * 	[92/11/02            roy]
 * 
 * Revision 2.16  92/10/05  12:09:01  rabii
 * 	Redo locking so that tinfo struct uses its own lock.
 * 	[92/10/02            roy]
 * 
 * Revision 2.15  92/09/29  16:48:56  rabii
 * 	If a uemul_token_get_size RPC fails with MIG_SERVER_DIED
 * 	then don't call token_port_deallocate.
 * 	[92/09/28            roy]
 * 
 * 	mf_temporary must make the mem obj uncacheable so that disk blocks
 * 	of the corresponding file will be reclaimed when the file is removed.
 * 	[92/09/26            roy]
 * 
 * Revision 2.14  92/09/24  16:51:25  rabii
 * 	mf_token_not_found won't signal tok_released condition if it's
 * 	already been done.
 * 	[92/09/22            roy]
 * 
 * Revision 2.13  92/09/22  12:25:36  roy
 * 	Temporarily back out code for temporary files.
 * 	[92/09/22            roy]
 * 
 * Revision 2.12  92/09/20  11:24:36  roy
 * 	Move deallocation of revoke port from mf_token_release to
 * 	tinfo_decr_refcnt.
 * 	[92/09/18            roy]
 * 
 * 	This module no longer caches the file size.  Uses VOP_GETSIZE to 
 * 	retrieve the size from the underlying file system.  Also, knowledge
 * 	of temporary files is only retained in the vnode pager.
 * 	[92/09/15            roy]
 * 
 * Revision 2.11  92/09/11  09:29:41  rabii
 * 	Add assert to mf_token_reclaim.
 * 	[92/09/10            roy]
 * 
 * Revision 2.10  92/08/26  12:14:08  loverso
 * 	Cached state (file size, accessed/modified flags) propogated to 
 * 	the underlying file system as necessary when a token is released,
 * 	or the file truncated.  mf_get_info replaced with mf_update.  
 * 	[92/08/18            roy]
 * 
 * 	Remove printf's.  Added back in revision 2.8 comment and fixed
 * 	revision 2.9 comment.  
 * 	[92/08/18            roy]
 * 
 * Revision 2.9  92/07/29  17:22:14  rabii
 * 	fixed last mod (roy).
 * 
 * Revision 2.8  92/07/29  10:28:37  rabii
 * 	mf_read and mf_write now take uio args (roy).
 * 
 * Revision 2.7  92/07/14  14:51:55  rabii
 * 	Implemented mf_temporary.
 * 	[92/07/13            roy]
 * 
 * Revision 2.6  92/06/30  22:48:03  loverso
 * 	Added mf_get_info, removed mf_get_size, and added must_clean flag
 * 	to vm_info struct.
 * 	[92/06/05            roy]
 * 
 * Revision 2.5  92/05/24  13:59:18  pjg
 * 	Fix args to vnode_pager_flush from mf_set_size.
 * 	Call ux_server_thread_blocking/unblocking where appropriate.
 * 	[92/05/20            roy]
 * 
 * Revision 2.4  92/05/18  12:26:28  roy
 * 	Revision 2.3.1.4  92/05/08  12:26:03  roy
 * 	get_data_token now has condition_wait in a while loop.
 * 	[92/05/08            roy]
 * 
 * 	Revision 2.3.1.3  92/05/08  12:03:55  roy
 * 	Token data structures now move through a state machine.
 * 	[92/04/28            roy]
 * 
 * 	Revision 2.3.1.2  92/04/22  10:04:25  roy
 * 	Major changes to support token caching in the emulator.
 * 	[92/04/05            roy]
 * 
 * 	Revision 2.3.1.1  92/03/30  17:53:48  roy
 * 	Use VM_INHERIT_NONE in mf_get_window until VM_INHERIT_SHARE is
 * 	implemented across nodes.  Use new vnode pager interfaces.
 * 	[92/03/30            roy]
 * 
 * Revision 2.3  92/03/15  14:29:17  roy
 * 	92/03/09  10:40:12  roy
 * 	vnode_uncache_object now returns a void.
 * 
 * 	92/03/03  16:48:35  roy
 * 	Fix up mf_set_size.  Implement mf_uncache.
 * 
 * 	92/02/21  17:46:51  roy
 * 	Fix mf_read and mf_write to map on page boundaries.
 * 
 * Revision 2.2  91/12/10  22:14:46  roy
 * 	91/12/04  16:32:30  roy
 * 	Better file size handling.
 * 
 * 	91/12/03  10:52:42  roy
 * 	Added mf_clean, mf_read, mf_write.
 * 
 * 	91/11/19  20:12:36  roy
 * 	Size handling logic.
 * 
 * 	91/11/19  10:13:41  roy
 * 	Added mf_get_window in support of mapping files opened for
 * 	read access only.
 * 
 * Revision 2.1  91/11/16  17:26:40  roy
 * Created.
 *
 * $EndLog$
 */

#include <sys/kernel.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <sys/file.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <kern/mfs.h>
#include <uxkern/mf.h>
#include <kern/queue.h>
#include <kern/assert.h>
#include <sys/errno.h>
#ifdef PFS
#include <sys/estat.h>
#endif

/*
 * This is the mapped files module.  It provides interfaces for token
 * acquisition/release and access to mapped file data, including a
 * file's size.
 *
 * Tokens are used to control shared access to an open file's offset
 * (seek ptr) and a file's length.  As implemented, a token is a port
 * and a single token represents whether a client has access to a
 * open file's offset and read or write access to a file's data (a 
 * file's length is treated as part of its data).
 *
 * Clients may cache tokens to avoid communication with this module.
 * Callbacks are used to revoke client tokens.  Protection against
 * aborting clients is ensured via no-more-senders notification
 * on tokens.
 *
 * Acquiring a token with access to the offset only (no data access)
 * is not supported.  Acquiring offset-only access would only be used
 * by seek() and since it normally precedes read() or write(), acquiring
 * data access at the same time is a reasonable optimization.  Plus, the
 * token callback mechanism is simpler.
 */

zone_t		token_info_zone;

int		token_inuse = 0; 		/* debug */
int 		token_revoke_retries = 0;	
int 		token_revoke_failures = 0;	
int 		token_not_found_waiter = 0;	
int 		token_not_found_no_waiter = 0;	
int		token_no_senders_wait_cnt = 0; 	


extern mach_port_t	get_token_port();
extern void		token_port_allocate();
extern void		token_port_deallocate();
extern void		token_port_callback_failure();

/*
 * Routine to initialize the mapped files module.
 */
mf_init()
{
	/* Initialize an expandable token zone. */
	token_info_zone = zinit(sizeof(token_info_t), vm_page_size, 0,
				"token_info");
}

/*
 * Increment the reference count on a token info struct.
 * Must be called with the struct locked (or otherwise accessible
 * to only the calling thread).
 *
 * Obtaining a token info reference guarantees the caller that the 
 * structure will not disappear.  
 */
#define	tinfo_incr_refcnt(x)	(x)->refcnt++


/*
 * Decrement the reference count on a token info struct.
 * Must NOT be called with the struct locked.
 *
 * This may cause the token info struct to be freed, as well as
 * tear down of data structures referenced by the struct.
 */
void
tinfo_decr_refcnt(tip, num)
	token_info_t	*tip;
{
	mutex_lock(&tip->lock);
	tip->refcnt -= num;
	if (tip->refcnt == 0) {
		ASSERT(tip->vp);
		ASSERT(tip->fp);
		ASSERT(tip->revoke_port != MACH_PORT_NULL);
		FP_UNREF(tip->fp);
		tip->fp = NULL;
		vrele(tip->vp);
		tip->vp = NULL;
		mach_port_deallocate(mach_task_self(), tip->revoke_port);
		tip->revoke_port = MACH_PORT_NULL;
		tip->state = T_FREE;
		ZFREE(token_info_zone, tip);
		token_inuse--;
	} else
		mutex_unlock(&tip->lock);
}

/*
 * Do the work of getting a data token.  
 * Only called internal to this module.
 * 
 * XXX Should be made faster, especially for the case where the 
 * memory object isn't active.
 */
void
get_data_token(vmp, tip, flags)
	struct vm_info  *vmp;
	token_info_t	*tip;
	int		flags;
{
	token_info_t	*curtip;
	mach_port_t	port;
	int		err;
	/* int		retries = 0;   debug */

	/*
	 * We attempt to grant tokens to a file in FIFO order.
         * If a thread is acquiring or someone is already waiting
	 * to acquire, then just wait our turn.  
	 */
        mutex_lock(&vmp->vm_lock);
	if (vmp->wait_cnt != 0)
		goto wait;		/* there's at least one waiter */
        while (vmp->acquiring) {
                /*
		 * This code is in a while loop because under rare 
		 * circumstances condition_signal may wake up more than 
		 * one waiter.  For this same reason we are not guaranteed
		 * of absolute FIFO ordering.
                 */
	 wait:
		vmp->wait_cnt++;
                condition_wait(&vmp->may_acquire, &vmp->vm_lock);
		vmp->wait_cnt--;
        }

	ASSERT(!vmp->acquiring);
	vmp->acquiring = 1;		/* prevent others from acquiring */

 try_again:
        if (vmp->wa_granted || vmp->ra_granted) {
                /*
                 * Someone holds the token.  
                 * XXX Only a single reader or writer is currently supported,
                 * via use of the write token pointer.
                 */
                curtip = vmp->wtoken;
                if (curtip != NULL) {
			mutex_lock(&curtip->lock);
			ASSERT(curtip->state == T_ACQ);

			/* Get a reference to the token info struct. */
			tinfo_incr_refcnt(curtip);

                        /* 
                         * Not the server itself that holds the token.
                         * => send a revoke message and wait for a token
                         * release.  
                         */
                        ASSERT(curtip->revoke_port != MACH_PORT_NULL);

			port = get_token_port(curtip);
                        err = uemul_token_revoke(curtip->revoke_port, port);

			if (err != KERN_SUCCESS) {
				/*
				 * Deallocate the send right and tinfo
				 * reference.  Yield to another thread
				 * in order to give token release a chance.
				 */
				mutex_unlock(&curtip->lock);
				mutex_unlock(&vmp->vm_lock);
				token_port_callback_failure(port);
				tinfo_decr_refcnt(curtip, 1);

				thread_yield();  
				mutex_lock(&vmp->vm_lock);
				token_revoke_failures++;
				/* retries++; */
				goto try_again;
		       }

			/* reflect that a revoke is in progress */
			curtip->state = T_ACQ_AND_RIP;
			mutex_unlock(&curtip->lock);
                }

		/* 
		 * Wait to be signalled.
		 */
                condition_wait(&vmp->tok_released, &vmp->vm_lock);
	
                if (curtip != NULL) {
			/*
			 * It's possible the token_revoke message above 
			 * raced with a client acquiring the token, in which 
			 * case the emulator didn't yet know about the token.  
			 * Or, the revoke and release msgs crossed in the mail.
			 * So we have to retry (after yielding).
			 */
			mutex_lock(&curtip->lock);
			ASSERT(curtip->state == T_ACQ_AND_RIP ||
			       curtip->state == T_REL_AND_RIP);
			if (curtip->state == T_ACQ_AND_RIP) {
				curtip->state = T_ACQ;
				mutex_unlock(&curtip->lock);
				mutex_unlock(&vmp->vm_lock);
				tinfo_decr_refcnt(curtip, 1);

				thread_yield();
				mutex_lock(&vmp->vm_lock);
				token_revoke_retries++;
				/* retries++; */
				goto try_again;
			} else
				mutex_unlock(&curtip->lock);

			tinfo_decr_refcnt(curtip, 1);
		}

                ASSERT(!vmp->wa_granted && !vmp->ra_granted);
        }

	/* if (retries)	debug 
		printf("token_revoke: retried %d times\n", retries); */

        if (flags & TOK_DATA_WRITE) {
                vmp->wa_granted = 1;
                vmp->ra_granted = 0;
        } else {
                vmp->ra_granted = 1;
                vmp->wa_granted = 0;
        }

	/*
	 * For the case when it's not the server itself acquiring the 
	 * token (tip != NULL), record the token struct and set its state. 	
	 * Grab a ref to indicate it's "acquired".
	 */
        ASSERT(vmp->wtoken == NULL);
	if (tip != NULL) {
		ASSERT(tip->state == T_FREE);
		tip->state = T_ACQ;	
		/* tinfo struct not accessible so lock not needed */
		tinfo_incr_refcnt(tip);
		vmp->wtoken = tip;      
	}
	vmp->acquiring = 0;
        condition_signal(&vmp->may_acquire); 	/* in case someone is waiting */
        mutex_unlock(&vmp->vm_lock);
}

/*
 * Do the work of releasing a data token.  
 * Only called internal to this module.
 */
void
rel_data_token(vmp, flags)
	struct vm_info	*vmp;
	int		flags;
{
	token_info_t	*tip;

        mutex_lock(&vmp->vm_lock);
	if (flags & TOK_DATA_WRITE) 
		vmp->wa_granted = 0;
	else 
		vmp->ra_granted = 0;

	if ((tip = vmp->wtoken) != NULL) {
		/*
		 * Disconnect the token struct to prevent others from 
		 * accessing it.
		 */
		vmp->wtoken = NULL;

		/*
		 * For the token info struct, change the state to "released".
		 * And, signal the condition (if it wasn't already done
		 * by mf_token_not_found).
		 */
		mutex_lock(&tip->lock);
		ASSERT(tip->state == T_ACQ || tip->state == T_ACQ_AND_RIP);
		if (tip->state == T_ACQ)
			tip->state = T_REL;  
		else
			tip->state = T_REL_AND_RIP;  
		if (tip->state == T_REL_AND_RIP)
			condition_signal(&vmp->tok_released);
		mutex_unlock(&tip->lock);
		mutex_unlock(&vmp->vm_lock);
	} else {
		/*
		 * Signal a possible waiter.
		 */
		condition_signal(&vmp->tok_released);
		mutex_unlock(&vmp->vm_lock);
	}

	if (tip != NULL)
		/*
		 * Decrement the tinfo refcnt because it had been "acquired".
		 */
		tinfo_decr_refcnt(tip, 1);

}

/*
 * Acquire a token.  Upon entry to the routine a FP_REF is held.
 */
int
mf_token_acquire(fp, args)
	struct file 		*fp;
	void 			*args;	
{
	register struct args {
		int		flags;
		mach_port_t	revoke_port;
		mach_port_t	*token;
		off_t		*offset; /* (esize_t *) for PFS files. */
		int		*length; /* (esize_t *) for PFS files. */
	} *uap = (struct args *) args;

	int			flags = uap->flags;
        boolean_t               offset_needed;
	struct vnode 		*vp;	
	token_info_t		*tip = NULL;

        offset_needed = flags & TOK_OFFSET;
        flags = flags & ~TOK_OFFSET;

        /*
         * Check for bogus args.  
         */
        if (flags != TOK_DATA_READ && flags != TOK_DATA_WRITE)
		return (EINVAL);

        if (fp->f_type != DTYPE_VNODE)
		return (EINVAL);

        vp = (struct vnode *) fp->f_data;
	/*
	 * Allocate a token info structure and associate a port with it.
	 * XXX Should be caching token_info structs after use.
	 */
	ZALLOC(token_info_zone, tip, token_info_t *);
	if (tip == NULL) {
		panic("mf_token_acquire: No more entries");
	}
	token_inuse++;	       
	tip->state = T_FREE;	/* will change to T_ACQ in get_data_token */
	tip->refcnt = 0;
	mutex_init(&tip->lock);

	/*
	 * Allocate a port for the token structure, including a send
	 * right to return to the caller.  
	 */
	token_port_allocate(tip);
	tip->revoke_port = uap->revoke_port;
	tip->flags = flags;             
	tip->vp = vp;
        ASSERT(vp);
	VREF(vp);

	/*
	 * We always retain access to the file structure, even though it's 
	 * strictly only required if offset_needed==TRUE.
	 */
	FP_REF(fp);
	tip->fp = fp;

	/* 
	 * Get the data token.  
	 */
	get_data_token(vp->v_vm_info, tip, flags);

        if (offset_needed) {
                /*
                 * Access to the offset is synchronized via the data token
                 * (which we already have).
                 */
		tip->flags |= TOK_OFFSET;  /* record the fact */

		/* 
		 * Return the offset to the client.
		 */
#ifdef PFS
		if (VIO_IS_PFS(vp)) {
			esize_t	*ex_offset = (esize_t *)uap->offset;
			ex_offset->shigh = fp->pfs_offset.shigh;
			ex_offset->slow = fp->pfs_offset.slow;

#ifdef DEBUG_PFSTOKEN
			printf("mf_token_acquire: ex_offset = %d, %d\n",
				ex_offset->shigh, ex_offset->slow);
#endif
		} else 
#endif
		*uap->offset = fp->f_offset;   
	} else 
#ifdef PFS
		if (VIO_IS_PFS(vp)) {
			esize_t	*ex_offset = (esize_t *)uap->offset;
			ex_offset->shigh = VNOVAL;
			ex_offset->slow = VNOVAL;
		} else 
#endif
		*uap->offset = -1;      /* to catch bugs */

	/*
	 * Set more return args.
	 */
#ifdef PFS
	if (VIO_IS_PFS(vp)) {
		esize_t		*ex_length = (esize_t *)uap->length;
		ex_length->shigh = vp->v_vm_info->pfs_length.shigh;
		ex_length->slow  = vp->v_vm_info->pfs_length.slow;
#ifdef DEBUG_PFSTOKEN
		printf("mf_token_acquire: ex_length = %d, %d\n",
			ex_length->shigh, ex_length->slow);
#endif
	} else 
#endif
	*uap->length = mf_get_size_for_owner(vp);

	*uap->token = TINFO_TO_PORT(tip);
	return(ESUCCESS);
}

/*
 * Acquire a token and a memory object port.
 */
int
mf_token_acquire_with_mo(fp, args)
	struct file 		*fp;
	void 			*args;	
{
	register struct args {
		int		flags;
		mach_port_t	revoke_port;
		mach_port_t	*token;
		off_t		*offset;
		int		*length;
		mach_port_t	*mem_obj;
	} *uap = (struct args *) args;

	struct vnode 		*vp;
	int			error;

	/*
	 * Can't hand out a memory object to the emulator if the file 
	 * wasn't opened for write access.  
	 * XXX We currently are handing out the mem obj to the emulator in
	 * all cases.
	if ((fp->f_flag & FWRITE) == 0)
		return(EINVAL);
	 */

	if (error = mf_token_acquire(fp, args))
		return(error);

	/*
	 * Hold vm_info's lock while accessing its state.
	 * XXX inode_pager_setup should be changed to accept a 'temporary' arg.
	 */
	vp = (struct vnode *) fp->f_data;
	mutex_lock(&vp->v_vm_info->vm_lock);
	if ((*uap->mem_obj = inode_pager_setup(vp, FALSE, 
					       vp->v_vm_info->cacheable))
		    == MEMORY_OBJECT_NULL) 
		error = EINVAL;
	mutex_unlock(&vp->v_vm_info->vm_lock);

	return(error);
}

/*
 * Called by a client to release a token.  
 */
int
mf_token_release(tip, args)
	token_info_t		*tip;
	void 			*args;	
{
	register struct args {
		off_t		offset; /* (esize_t *) for PFS */
		int		length;	/* (esize_t *) for PFS */
		int		accessed;
		int		modified;
		off_t		min_offset;
		off_t		max_offset;
		int		num_rights;	
	} *uap = (struct args *) args;
	
	struct vm_info		*vmp;
	int 			flags;
	int			modified, length;
	int 			error;
#ifdef PFS
	esize_t *ex_offset;
	esize_t ex_length;
#endif

	flags = tip->flags;	      
	ASSERT(flags & (TOK_DATA_READ | TOK_DATA_WRITE));
	ASSERT(tip->fp);
	ASSERT(tip->vp);
	ASSERT(tip->revoke_port != MACH_PORT_NULL);
#ifdef DEBUG_PFSTOKEN
	if (VIO_IS_PFS(tip->vp)) {
		printf("mf_token_release, flags = %x\n",
			flags);
	}
#endif

	if (flags & TOK_OFFSET) 
                /*
                 * Update of fp->f_offset is protected because the 
                 * data token is held.
                 */
		
#ifdef PFS
		if (VIO_IS_PFS(tip->vp)) {
			ex_offset = (esize_t *)uap->offset;
			tip->fp->pfs_offset.shigh = ex_offset->shigh;
			tip->fp->pfs_offset.slow =  ex_offset->slow;
#ifdef DEBUG_PFSTOKEN
			printf("mf_token_release, ex_offset = %d, %d\n",
				ex_offset->shigh, ex_offset->slow);
#endif
		} else 
#endif
		tip->fp->f_offset = uap->offset;

	/*
	 * Reflect the new state and release the token.
	 * Only update state related to file modifications if 
	 * we're releasing a write token and the modified flag is set.
	 */
	vmp = tip->vp->v_vm_info;
	if ((flags & TOK_DATA_READ) == 0 && uap->modified) {
		/*
		 * Record the size (this shouldn't be shrinking
		 * the file).
		 */
		modified = 1;
#ifdef PFS
		if (VIO_IS_PFS(tip->vp)) {
			ex_length.shigh = ((esize_t *)uap->length)->shigh;
			ex_length.slow = ((esize_t *)uap->length)->slow;
#ifdef DEBUG_PFSTOKEN
			printf("mf_token_release, ex_length = %d, %d\n",
				ex_length.shigh, ex_length.slow);
#endif
		} else 
#endif
		length = uap->length;

		/*
		 * Record info used by mf_clean.
		 */
		vmp->must_clean = 1;
		if (uap->min_offset < vmp->min_offset)
			vmp->min_offset = uap->min_offset;
		if (uap->max_offset > vmp->max_offset)
			vmp->max_offset = uap->max_offset;
#ifdef PFS
	} else if ((flags & TOK_DATA_READ) && (uap->modified) && 
		   (VIO_IS_PFS(tip->vp))) {
		/*
		 * This case is only valid when the file length
		 * has been recomputed by the emulator when the
		 * token was acquired.
		 */
		ex_length.shigh = ((esize_t *)uap->length)->shigh;
		ex_length.slow = ((esize_t *)uap->length)->slow;
#endif
	} else {
		/* tell VOP_UPDATE not to update these values */
		modified = 0;
#ifdef PFS
		if (VIO_IS_PFS(tip->vp)) {
			ex_length.shigh = VNOVAL;
			ex_length.slow 	= VNOVAL;
		} else 
#endif
		length = -1;
	}
	
	/*
	 * Update flags and file size in the underlying file system.
	 */
	if (uap->accessed || modified)
#ifdef PFS
	if (VIO_IS_PFS(tip->vp)) {

		VOP_UPDATE(tip->vp, uap->accessed, modified, &ex_length, 0,
			   tip->fp->f_cred, error);
	} else
#endif
		VOP_UPDATE(tip->vp, uap->accessed, modified, length, 0,
			   tip->fp->f_cred, error);


	/*
	 * After rel_data_token is executed it's no longer possible
	 * to generate a new send right for the token.  Prior to
	 * then, however, a new send right could be generated by 
	 * callers of get_token_port.
	 */
	rel_data_token(vmp, flags);

	/*
	 * Deallocate the specified number of send rights.
	 */
	token_port_deallocate(tip, uap->num_rights);

	return(ESUCCESS);
}

/*
 * A token revoke did not succeed.  Force the thread waiting for the
 * token to be awoken.  The caller is providing a send right to this
 * routine.  This right is effectively a reference to the token info
 * struct.
 */
int
mf_token_not_found(tip)
	token_info_t		*tip;
{
	struct vm_info		*vmp;

	ASSERT(tip->vp);
	vmp = tip->vp->v_vm_info;

        mutex_lock(&vmp->vm_lock);  
	if (vmp->acquiring) {
		/*
		 * Signal the condition (if wasn't already done by 
		 * rel_data_token).
		 */
		mutex_lock(&tip->lock);
		ASSERT(tip->state == T_ACQ_AND_RIP || 
		       tip->state == T_REL_AND_RIP);
		if (tip->state == T_ACQ_AND_RIP)
			condition_signal(&vmp->tok_released);
		mutex_unlock(&tip->lock);
		mutex_unlock(&vmp->vm_lock);
		token_not_found_waiter++;
	} else {
		mutex_unlock(&vmp->vm_lock);
		token_not_found_no_waiter++;
	}

	/* 
	 * Deallocate the send right provided to us.
	 */
	token_port_deallocate(tip, 1);

	return(ESUCCESS);
}

/*
 * Called by a client to change a token.
 */
int
mf_token_change(tip, args)
	token_info_t		*tip;
	void 			*args;	
{
	register struct args {
		int		flags;
		mach_port_t	*token;
	} *uap = (struct args *) args;

	struct vnode		*vp;
	struct vm_info		*vmp;	

	vp = tip->vp;
	ASSERT(vp);
	vmp = vp->v_vm_info;

	if (!(uap->flags & TOK_DATA_WRITE) || 
	    !(tip->flags & TOK_DATA_READ))
		return(EINVAL);

	mutex_lock(&vmp->vm_lock);
	if (vmp->wa_granted || !vmp->ra_granted) {
		mutex_unlock(&vmp->vm_lock);
		return(EINVAL);
	}
	vmp->wa_granted = 1;
	vmp->ra_granted = 0;
	mutex_unlock(&vmp->vm_lock);
	tip->flags = tip->flags ^ (TOK_DATA_READ | TOK_DATA_WRITE);

	/* just return the same send right that was passed in */
	*uap->token = TINFO_TO_PORT(tip);
	return(ESUCCESS);
}
	
/*
 * Called by clients to obtain a mapped window into a file for which
 * they only have read access.
 */
int
mf_get_window(fp, args)
	struct file 		*fp;
	void 			*args;	
{
	register struct args {
		off_t		offset;
		int		size;
		vm_address_t	*addr;		/* IN/OUT */
	} *uap = (struct args *) args;

	struct vnode		*vp;
	memory_object_t		mem_obj;
	int			ret;

	ASSERT(fp != (struct file *) NULL);

	/*
	 * The memory object port acquired here will be deallocated
	 * (via inode_pager_release) at the time when the file 
	 * structure is deallocated.
	 *
	 * Hold vm_info's lock while accessing its state.  The lock is
	 * also protecting fp->mem_obj.
	 */
	vp = (struct vnode *) fp->f_data;
	mutex_lock(&vp->v_vm_info->vm_lock);
	if (fp->mem_obj == MEMORY_OBJECT_NULL) {
		if ((fp->mem_obj = inode_pager_setup(vp, FALSE, 
						     vp->v_vm_info->cacheable)) 
		    == MEMORY_OBJECT_NULL) {
			mutex_unlock(&vp->v_vm_info->vm_lock);
			return(EINVAL);
		}
	}
	mem_obj = fp->mem_obj;
	mutex_unlock(&vp->v_vm_info->vm_lock);

	/*
	 * Only read access is allowed. 
         * We would like to use VM_INHERIT_SHARE in the vm_map call
         * but it isn't yet implemented across nodes.
	 */
	ux_server_thread_blocking();   	/* may interact with vnode pager */
	if (ret = vm_map(u.u_procp->p_task, uap->addr, uap->size, 0, TRUE, 
		       mem_obj, uap->offset, FALSE, 
		       VM_PROT_READ, VM_PROT_READ, VM_INHERIT_NONE))
		panic("mf_get_window.vm_map 0x%x", ret);
	ux_server_thread_unblocking();

	return(ESUCCESS);
}

/*
 * Called only internal to this module.  Data token is held.
 */
int
mf_get_size_for_owner(vp)
	struct vnode	*vp;
{
	int 		size, error;

	/*
	 * Any cached size information has already been propogated
	 * back to the underlying file system (because that happens when
	 * a token is released).  Hence, use VOP_GETSIZE to find out
	 * the size rather than VOP_GETATTR (because the latter calls
	 * mf_update which would cause a deadlock).
	 */
	VOP_GETSIZE(vp, &size, 0, NULL, error);
	if (error) 
		panic("mf_get_size_for_owner 0x%x", error);
	return(size);
}

/*
 * Tell the mapped file module to update cached info to the underlying
 * file system.  
 */
int
mf_update(vp)
	struct vnode	*vp;
{
        struct vm_info  *vmp = vp->v_vm_info;

	ASSERT(VIO_IS_MAPPED(vp));

	/*
	 * Acquiring a write token guarantees that all remotely
	 * cached state is written back to this module, and to
	 * the underlying file system.
	 */
	get_data_token(vmp, NULL, TOK_DATA_WRITE);
	rel_data_token(vmp, TOK_DATA_WRITE);
	return(ESUCCESS);
}

/*
 * If the mapped file functionality has the real size, return it and the
 * file's offset.  
 */
int
mf_get_size_and_offset(fp, size, offset)
	struct file	*fp;
	int		*size;		/* out */
	off_t		*offset;	/* out */
{
	struct vnode	*vp = (struct vnode *) fp->f_data;
	struct vm_info	*vmp = vp->v_vm_info;

	ASSERT(VIO_IS_MAPPED(vp));

	/*
	 * Get a data read token so that we can call mf_get_size_for_owner.
	 * Access to the offset is synchronized via the data token as well.
	 */
	get_data_token(vmp, NULL, TOK_DATA_READ);

	*size = mf_get_size_for_owner(vp);
	*offset = fp->f_offset;   

	rel_data_token(vmp, TOK_DATA_READ);
	return(ESUCCESS);
}

/*
 * Called to either increase or decrease the size of the file.
 *
 * It will flush memory object data (if nec.), and write the size to the
 * underlying file system.
 *
 * If flushing is necessary, it synchronously waits for the flush to complete.
 */
int
mf_trunc(vp, newsize, flags)
	struct vnode		*vp;
	int			newsize;
	int			flags;
{
	struct vm_info		*vmp = vp->v_vm_info;
	int			offset, oldsize, size;
        memory_object_t 	object = MEMORY_OBJECT_NULL;
	int			error;

	ASSERT(VIO_IS_MAPPED(vp));

	/*
	 * Must have the data write token in order to guarantee atomicity.
	 */
	get_data_token(vmp, NULL, TOK_DATA_WRITE);

	/*
	 * If the file is shrinking, flush all pages within the no longer
	 * valid region.  
	 * 
	 * If not truncating to a page boundary, then the last page must 
	 * be cleaned AND flushed.  This means subsequent file growth 
	 * (e.g., due to truncate) will see zero's at the end of this page 
	 * rather than data that previously existed.  Cleaning this page
	 * is also required by callers of mf_trunc to ensure that a
	 * pageout doesn't erroneously grow the file beyond the new eof.
	 */
	oldsize = mf_get_size_for_owner(vp);

	if (newsize < oldsize || trunc_page(newsize) != newsize) {
		if ((object = vnode_pager_ref(vp)) != MEMORY_OBJECT_NULL) {
			if (newsize < oldsize) {
				/*
				 * The file is shrinking.
				 */
				offset = round_page(newsize);
				while (offset < oldsize) {
					/* 
					 * Flush all pages except the last.
					 * XXX Restrict flush size until 
					 * XMM can handle unlimited sizes.
					 */
					static vm_size_t max_size = 
						16*1024*1024;
					size = oldsize - offset;
					if (size > max_size)
						size = max_size;
					error = vnode_pager_flush(object, 
							     offset,
							     size,
							     1, FALSE, 
							     TRUE, 
							     VM_PROT_NO_CHANGE);
					if (error) {
						vnode_pager_unref(object);
						goto skip;
					}
					offset += size;
				}
			}

			if ((offset = trunc_page(newsize)) != newsize) {
				/* 
				 * Clean and flush the last page.
				 */
				error = vnode_pager_flush(object, offset,
							  vm_page_size,
							  1, TRUE, TRUE, 
							  VM_PROT_NO_CHANGE);
			}

			vnode_pager_unref(object);
		}

		/*
		 * Reset the range used for cleaning, if nec.
		 */
		if (newsize < vmp->max_offset)
			vmp->max_offset = newsize;
		if (newsize < vmp->min_offset)
			vmp->min_offset = newsize;
	}

 skip:
	/*
	 * Update size in the underlying file system.
	 */
	VOP_UPDATE(vp, 0, 0, newsize, (flags|IO_TRUNC), u.uu_procp->p_rcred, 
		   error);

	rel_data_token(vmp, TOK_DATA_WRITE);
	return(error);
}

/*
 * Write modified data from the mapped file back to its file system.
 * 'wait' arg says to wait until the writes are complete.
 */
void
mf_clean(vp, wait)
	struct vnode		*vp;
	boolean_t		wait;
{
	struct vm_info		*vmp = vp->v_vm_info;
	vm_offset_t		offset, max_offset;
        memory_object_t         object;
	vm_size_t		size;

	ASSERT(VIO_IS_MAPPED(vp));

	/*
	 * Only clean if the must_clean flag is set.  Get the token to 
	 * find out the state of the must_clean flag (and the range of 
	 * modifications).  
	 */
	get_data_token(vmp, NULL, TOK_DATA_READ);
	if (vmp->must_clean) {
		/*
		 * Do a minor bit of sanity checking.
		 */
		if (vmp->min_offset > vmp->max_offset) {
			printf("mf_clean error: min_off=0x%x > max_off=0x%x\n",
			       vmp->min_offset, vmp->max_offset);
			offset = max_offset = 0;
		} else {
			offset = trunc_page(vmp->min_offset);
			max_offset = round_page(vmp->max_offset);
		}

		/*
		 * Reset these values and release the token. 
		 * (XXX For now, play it conservative and don't release the
	         * token until after cleaning.)
                 */
		vmp->must_clean = 0;
		vmp->min_offset = INT_MAX;
		vmp->max_offset = 0;

		/*
		 * vnode_pager_flush() guarantees that modified data is
		 * written into the underlying file system.  However, there
		 * will be no effect if the file has previously been marked
		 * temporary.
                 *
                 * First, get a reference to the object to ensure it doesn't 
		 * disappear out from under us.
		 */
                if ((object = vnode_pager_ref(vp)) != MEMORY_OBJECT_NULL) {
			/*
			 * XXX Restrict flush size until XMM can handle
			 * unlimited sizes.
			 */
			static vm_size_t max_size = 16*1024*1024;
			while (offset < max_offset) {
				size = max_offset - offset;
				if (size > max_size)
					size = max_size;
				(void) vnode_pager_flush(object, offset, size,
							 wait ? 1 : 0, TRUE, 
							 FALSE, 
							 VM_PROT_NO_CHANGE);
				offset += size;
			}
                        vnode_pager_unref(object);
                }

		rel_data_token(vmp, TOK_DATA_READ); 
	} else
		rel_data_token(vmp, TOK_DATA_READ);

}

/*
 * mf_sync_data is called to force a flushing of the file data out to 
 * disk.  This is called after each update when in the file is 
 * opened with O_SYNC mode.
 */
int
mf_sync_data(tip, args)
	token_info_t		*tip;
	void 			*args;	
{
	register struct args {
		size_t		length;
		off_t		min_offset;
		off_t		max_offset;
	} *uap = (struct args *) args;
	
	struct vm_info		*vmp;
	int 			flags;
	int			modified, length, accessed;
	int 			error;
	vm_offset_t		offset, max_offset;
	memory_object_t		object;
	vm_size_t		size;

	/*
	 * Do an update so the i_truesize gets set.
	 */
	ASSERT(tip->vp);
	vmp = tip->vp->v_vm_info;
	length = uap->length;
	modified = 1;
	accessed = 0;
	VOP_UPDATE(tip->vp, accessed, modified, length, 0,
		   tip->fp->f_cred, error);

	/*
	 * Compute the new min and max offsets.
	 */
	vmp->must_clean = 1;
	if (uap->min_offset < vmp->min_offset)
		vmp->min_offset = uap->min_offset;
	if (uap->max_offset > vmp->max_offset)
		vmp->max_offset = uap->max_offset;

	offset = trunc_page(vmp->min_offset);
	max_offset = round_page(vmp->max_offset);
	/*
	 * Clear the vnode fields.
	 */
	vmp->must_clean = 0;
	vmp->min_offset = INT_MAX;
	vmp->max_offset = 0;
	/*
	 * vnode_pager_flush() guarantees that modified data is
	 * written into the underlying file system.  However, there
	 * will be no effect if the file has previously been marked
	 * temporary.
	 *
	 * First, get a reference to the object to ensure it doesn't
	 * disappear out from under us.
	 */
	if ((object = vnode_pager_ref(tip->vp)) != MEMORY_OBJECT_NULL) {
		/*
		 * XXX Restrict flush size until XMM can handle
		 * unlimited sizes.
		 */
		static vm_size_t max_size = 16*1024*1024;
		while (offset < max_offset) {
			size = max_offset - offset;
			if (size > max_size)
				size = max_size;
			(void) vnode_pager_flush(object, offset, size,
						 1, TRUE, FALSE,
						 VM_PROT_NO_CHANGE);
						 offset += size;
		}
		vnode_pager_unref(object);
	}
	return(ESUCCESS);
}

/*
 * Tell the mapped file cache to relinquish any cached data.  The caller
 * may choose to wait in order to definitively determine if the mapped
 * file cache did indeed relinquish its data (it's possible it couldn't
 * because the data is currently accessible - i.e., mapped).
 */
void
mf_uncache(vp, wait)
	struct vnode		*vp;
	boolean_t		wait;
{
        memory_object_t         object;
        boolean_t               cacheable = FALSE;

	ASSERT(VIO_IS_MAPPED(vp));

        /*
         * Get a reference to the object to ensure it doesn't disappear
         * out from under us.  Hold the vm_info lock while updating state
	 * and getting a vnode pager ref in order to synchronize with
	 * callers of inode_pager_setup.
	 */
	mutex_lock(&vp->v_vm_info->vm_lock);
	vp->v_vm_info->cacheable = 0;
        if ((object = vnode_pager_ref(vp)) != MEMORY_OBJECT_NULL) {
		mutex_unlock(&vp->v_vm_info->vm_lock);
                (void) vnode_pager_change_attributes(object, &cacheable, 
                                                     NULL, wait);
                vnode_pager_unref(object);
        } else
		mutex_unlock(&vp->v_vm_info->vm_lock);
}

/*
 * Mark a file as temporary.  This has the effect of making the kernel's
 * corresponding VM object uncacheable, and prevents dirty data from being
 * written out when the VM object is deactivated or at vnode_pager_flush time.
 */
void
mf_temporary(vp, wait)
	struct vnode		*vp;
	boolean_t		wait;
{
        memory_object_t         object;
        boolean_t               cacheable = FALSE, temporary = FALSE; /* XXX */

	/*
	 * XXX There is currently a problem with marking a memory object
	 * temporary:  doing so may result in the memory object being
	 * terminated (with corresponding loss of dirty data) while the
	 * the file is still open.  The solution is to make sure that
	 * a "reference is held" to the memory object (e.g., by keeping
	 * a one page mapping) as long as the corresponding file is open.
	 *
	 * In the meantime, the 'temporary' arg is FALSE, but the 'cacheable'
	 * arg must be FALSE so that when a file is removed (and no longer
	 * open) the object will be terminated and the file truncated.
	 */

	ASSERT(VIO_IS_MAPPED(vp));

        /*
         * Get a reference to the object to ensure it doesn't disappear
         * out from under us.  Hold the vm_info lock while updating state
	 * and getting a vnode pager ref in order to synchronize with
	 * callers of inode_pager_setup.
	 */
	mutex_lock(&vp->v_vm_info->vm_lock);
	vp->v_vm_info->cacheable = 0;
	/* vp->v_vm_info->temporary = 1;  XXX */
        if ((object = vnode_pager_ref(vp)) != MEMORY_OBJECT_NULL) {
		mutex_unlock(&vp->v_vm_info->vm_lock);
                (void) vnode_pager_change_attributes(object, &cacheable, 
                                                     &temporary, wait);
                vnode_pager_unref(object);
        } else
		mutex_unlock(&vp->v_vm_info->vm_lock);
}

/*
 * mf_read() and mf_write() are called to read/write vnode data
 * by the server itself.  For instance, they are called by vn_rdwr.
 *
 * XXX Should consider rewriting these routines to keep cached mapped 
 * windows in the vm_info structure. 
 */
int
mf_read(vp, uio, ioflag, cred)
	struct vnode		*vp;
	struct uio 		*uio;
	int			ioflag;
	struct ucred		*cred;
{
	register off_t		offset;
	register int		length;
	memory_object_t		mem_obj;
	vm_address_t		addr = 0;
	char			*xaddr;
	int			i, len, file_size, amount, map_length;
	off_t			map_offset;
	struct iovec		*iovp;
	int			error;

	ASSERT(VIO_IS_MAPPED(vp));

	offset = uio->uio_offset;
	length = uio->uio_resid;

	if (uio->uio_rw != UIO_READ)
		panic("mf_read mode");
	if (length == 0)
		return (0);
	if (offset < 0)
		return (EINVAL);

	/*
	 * Hold vm_info's lock while accessing its state.  
	 */
	mutex_lock(&vp->v_vm_info->vm_lock);
	if ((mem_obj = inode_pager_setup((struct vnode *) vp, FALSE, 
					 vp->v_vm_info->cacheable)) 
	    == MEMORY_OBJECT_NULL) {
		mutex_unlock(&vp->v_vm_info->vm_lock);
		return(EINVAL);
	}
	mutex_unlock(&vp->v_vm_info->vm_lock);

	map_offset = trunc_page(offset);
	map_length = round_page(length + offset) - map_offset;

	/*
	 * Must acquire a data read token.
	 */
	get_data_token(vp->v_vm_info, NULL, TOK_DATA_READ);

	/*
	 * Only read access is needed.
	 */
	ux_server_thread_blocking();   	/* may interact with vnode pager */
	if (error = vm_map(mach_task_self(), &addr, map_length, 0, TRUE, 
		       mem_obj, map_offset, FALSE, 
		       VM_PROT_READ, VM_PROT_READ, VM_INHERIT_NONE)) {
		panic("mf_read.vm_map 0x%x", error);
	}

	/*
	 * Copy data from the memory object, but restrict the
	 * amount by the end-of-file.
	 */
	file_size = mf_get_size_for_owner(vp);
	if (offset >= file_size)
		amount = 0;
	else
		amount = MIN(length, file_size - offset);

	/*
	 * Copy data from the memory object.
	 */
	uio->uio_resid -= amount;
	xaddr = (char *)addr + (offset - map_offset);
	for (i = 0, iovp = uio->uio_iov; i < uio->uio_iovcnt; i++, iovp++) {
		len = MIN(iovp->iov_len, amount);
		bcopy(xaddr, iovp->iov_base, len);
		if ((amount -= len) == 0)
			break;
		xaddr += len;
	}
	ux_server_thread_unblocking();

	/*
	 * Update accessed flag in the underlying file system.
	 */
	VOP_UPDATE(vp, 1, 0, -1, 0, cred, error);

	rel_data_token(vp->v_vm_info, TOK_DATA_READ);  	/* release the token */

	if (error = vm_deallocate(mach_task_self(), addr, map_length))
		panic("mf_read.vm_deallocate 0x%x", error);

	inode_pager_release(mem_obj);

	return(error);
}

int
mf_write(vp, uio, ioflag, cred)
	struct vnode		*vp;
	struct uio 		*uio;
	int			ioflag;
	struct ucred		*cred;
{
	register int		length, end_offset;
	memory_object_t		mem_obj;
	struct vm_info		*vmp = vp->v_vm_info;
	vm_address_t		addr = 0;
	char			*xaddr;
	int			i, file_size, map_length;
	off_t			map_offset;
	struct iovec		*iovp;
	int			error;
	int			copyerror = 0;

	ASSERT(VIO_IS_MAPPED(vp));

	if (uio->uio_rw != UIO_WRITE)
		panic("mf_write mode");
	if (uio->uio_resid == 0)
		return (0);
	if (uio->uio_offset < 0)
		return (EINVAL);

	/*
	 * Hold vm_info's lock while accessing its state.  
	 */
	mutex_lock(&vp->v_vm_info->vm_lock);
	if ((mem_obj = inode_pager_setup((struct vnode *) vp, FALSE, 
					 vp->v_vm_info->cacheable)) 
	    == MEMORY_OBJECT_NULL) {
		mutex_unlock(&vp->v_vm_info->vm_lock);
		return(EINVAL);
	}
	mutex_unlock(&vp->v_vm_info->vm_lock);

	/*
	 * Must acquire the data write token.
	 */
	get_data_token(vmp, NULL, TOK_DATA_WRITE);

	/*
	 * Get the file length and reset offset for APPEND.
	 */
	file_size = mf_get_size_for_owner(vp);
        if (ioflag & IO_APPEND) 
		uio->uio_offset = file_size;

	map_offset = trunc_page(uio->uio_offset);
	map_length = round_page(uio->uio_offset + uio->uio_resid) - map_offset;

	/*
	 * Read/write access is needed.
	 */
	ux_server_thread_blocking();   	/* may interact with vnode pager */
	if (error = vm_map(mach_task_self(), &addr, map_length, 0, TRUE, 
		       mem_obj, map_offset, FALSE, 
		       VM_PROT_READ|VM_PROT_WRITE, 
		       VM_PROT_READ|VM_PROT_WRITE, VM_INHERIT_NONE)) {
		panic("mf_write.vm_map 0x%x", error);
	}

	/*
	 * Copy data to the memory object.
	 */
	length = 0; 
	xaddr = (char *)addr + (uio->uio_offset - map_offset);
	for (i = 0, iovp = uio->uio_iov; i < uio->uio_iovcnt; i++, iovp++) {
		/*
		 * On return from user_bcopy2, count will be the number of
		 * bytes actually copied.
		 */
		int count = iovp->iov_len;

		copyerror = user_bcopy2(iovp->iov_base, xaddr, &count);
		length += count;
		if (copyerror)
			break;
		xaddr += iovp->iov_len;
	}
	ux_server_thread_unblocking();

	/*
	 * Update the length of the file and the modified flag.
	 */
	if (length) {
		uio->uio_resid -= length;

		end_offset = uio->uio_offset + length;
		if (end_offset > file_size)
			file_size = end_offset;
		else
			file_size = -1;    /* VOP_UPDATE will ignore size arg */

		VOP_UPDATE(vp, 0, 1, file_size, 0, cred, error);

		/*
		 * If any data got copied, return success.
		 * Record info used at clean time.  pageout logic relies 
		 * on the must_clean flag being updated after the mem obj 
		 * data is written.
		 */
		copyerror = 0;
		vmp->must_clean = 1;
		if (uio->uio_offset < vmp->min_offset)
			vmp->min_offset = uio->uio_offset;
		if (end_offset > vmp->max_offset)
			vmp->max_offset = end_offset;
	}

	rel_data_token(vmp, TOK_DATA_WRITE);  	/* release the token */

	if (error = vm_deallocate(mach_task_self(), addr, map_length))
		panic("mf_write.vm_deallocate 0x%x", error);

	inode_pager_release(mem_obj);

	return(copyerror ? copyerror : error);
}

/*
 * Given a token structure, perform the correct Mach port allocation
 * and initialization.  The token_info struct must NOT be accessible
 * to any threads other than the caller.
 */
void
token_port_allocate(tip)
	token_info_t	*tip;
{
	mach_port_t	port, dummy;
	int		error;

	/*
	 * Allocate a port for token.
	 */
	port = TINFO_TO_PORT(tip);
	if (error = mach_port_allocate_name(mach_task_self(),
					    MACH_PORT_RIGHT_RECEIVE, port))
		panic("token_port_allocate: can't alloc port "
				"ret=0x%x name=0x%x", error, port);

	/*
	 * Allocate a send right to send back to the requester. 
	 */
	if (error = mach_port_insert_right(mach_task_self(),
					   port, port, MACH_MSG_TYPE_MAKE_SEND))
		panic("token_port_allocate: can't acquire send rights 0x%x",
		      error);

	/*
	 * Arrange for a 'no-more-senders' notification
	 */
	if (error = mach_port_request_notification(mach_task_self(), port,
					   MACH_NOTIFY_NO_SENDERS, 0,
					   port, MACH_MSG_TYPE_MAKE_SEND_ONCE, 
					   &dummy))
		panic("token_port_allocate: request notification failed 0x%x",
		      error);

	tip->tok_magic = TOK_MAGIC;  	/* set magic cookie */
	tip->send_count = 1;		/* count of outstanding send rights */
	tip->mscount = 1;		/* count of send rights ever created */
	tip->seqno = 0;			
	tip->nms_waiting = 0;
	/*
	 * Bump the tinfo refcount to reflect the new send right.
	 * It's ok that no lock is protecting this call because the
	 * tip is not accessible yet.
	 */
	tinfo_incr_refcnt(tip);

	/*
	 * Tell the server to service the port 
	 */
	ux_server_add_port(port);
}


/*
 * Given a token structure, allocate a new send right to it.
 * Must be called with the structure locked.
 */
mach_port_t
get_token_port(tip)
	token_info_t	*tip;
{
	mach_port_t	port;
	int		error;

	port = TINFO_TO_PORT(tip);

	/*
	 * Allocate a send right to send back to the requester. 
	 */
	error = mach_port_insert_right(mach_task_self(),
		port, port, MACH_MSG_TYPE_MAKE_SEND);
	if (error != KERN_SUCCESS)
		/* XXX */
		panic("get_token_port: can't acquire send right 0x%x", error);  

	tip->send_count++;		/* count of outstanding send rights */
	tip->mscount++;			/* count of send rights ever created */
	
	/*
	 * Bump the tinfo refcount to reflect the new send right.
	 */
	tinfo_incr_refcnt(tip);
	return(port);
}

/*
 * Given a token info structure, deallocate the specified number of
 * send rights, and destroy the port if the send right count drops to 0.
 * Must be called WITHOUT the structure locked.
 */
void
token_port_deallocate(tip, num_rights)
	token_info_t  	*tip;
	int		num_rights;
{
	mach_port_t	port;
	int		error;

	mutex_lock(&tip->lock);
	port = TINFO_TO_PORT(tip);
	tip->send_count -= num_rights;
	ASSERT(tip->send_count >= 0);

	/*
	 * A couple races to be aware of:
	 * - the possibility of get_token_port executing and creating
	 *   a new send right.  Could only happen if the token is
	 *   still acquired, but note that token_port_deallocate is
	 *   always called after the token has been released.  
	 * - the possiblity of the mach_port_mod_refs resulting in a NMS
	 *   being generated.  Could happen if there was a previous
	 *   callback failure.
	 *   
	 * Although it's necessary for the tip->lock to be held while 
	 * decrementing the tip->send_count, it shouldn't be necessary
	 * to hold it while deallocating the port rights.  However, it is
	 * held in order to be extra conservative.
	 */
	if (tip->send_count == 0) {
		ux_server_remove_port(port);
		if (error = mach_port_destroy(mach_task_self(), port))
		    panic("token_port_deallocate: unable to destroy port 0x%x",
			  error);
		tip->tok_magic = 0;
	} else {
		if (error = mach_port_mod_refs(mach_task_self(), port,
					   MACH_PORT_RIGHT_SEND, -num_rights))
		    panic("token_port_deallocate: unable to dealloc send 0x%x",
			  error);
	}
	mutex_unlock(&tip->lock);

	/*
	 * Decrement the tinfo refcount now that send rights have disappeared.
	 */
	tinfo_decr_refcnt(tip, num_rights);
}

/*
 * Deallocate a send right associated with a failed callback message.
 *
 * It is called when an attempt to move a send right to a client fails.
 * The assumption at this point is that we are no longer able to keep
 * track of the count of outstanding send rights, but that a NMS message
 * will eventually be generated (because the process that we failed to
 * communicate with must be dead or dying).  NMS processing will then force
 * the send_count to 0, thus freeing up the remaining tip references.
 */
void
token_port_callback_failure(port)
	mach_port_t	port;
{
	int		error;

	/* printf("token callback failure port=0x%x token_inuse=%d\n", 
	   port, token_inuse); */
	
	if (error = mach_port_mod_refs(mach_task_self(), port,
				   MACH_PORT_RIGHT_SEND, -1))
		panic("token_port_callback_fail: unable to dealloc send 0x%x",
		      error);
}

/*
 * Internal routine to increment a token sequence number.
 * It is typically called at the end of a token operation in order 
 * to guarantee all token messages have been processed before a
 * no-more-senders is processed.
 */
void
token_port_incr_seqno(tip)
	token_info_t		*tip;
{
	int			nms_wakeup = 0;

	/*
	 * Increment the token's sequence number.  If a no-more-senders
	 * notification is waiting on the sequence number, wake it up.
	 */
	mutex_lock(&tip->lock);
	tip->seqno++;
	if (tip->nms_waiting) {
		tip->nms_waiting = 0;
		nms_wakeup++;
	}
	mutex_unlock(&tip->lock);
	if (nms_wakeup) 
		thread_wakeup((int)&tip->seqno);
}

/*
 * Called by token ops to get a tinfo struct reference.
 */
void
token_port_ref(tip)
	token_info_t		*tip;
{
	mutex_lock(&tip->lock);
	tinfo_incr_refcnt(tip);
	mutex_unlock(&tip->lock);
}

/*
 * Called by token ops to release a tinfo struct reference.
 * This will also allow a token no-senders op to proceed.
 */
void
token_port_unref(tip)
	token_info_t		*tip;
{
	token_port_incr_seqno(tip);
	tinfo_decr_refcnt(tip, 1);
}

/*
 * Called on behalf of a token no-more-senders message.
 */
void
token_port_no_senders(port, tip, mscount, seqno)
	mach_port_t 	port;
	token_info_t	*tip;
	u_long		mscount;
	u_long		seqno;
{
	int		num_rights, error;
	mach_port_t	dummy;
	boolean_t	waited = FALSE;  		/* debug */
	boolean_t	waited_more_than_once = FALSE; 	/* debug */

	/*
	 * Wait for any outstanding messages on this port to be
	 * received.
	 *
	 * XXX Should be able to remove this logic.  Its idea was to
	 * prevent a NMS from racing ahead of the processing of other
	 * messages targeted at a token port.  But, that's only 
	 * mf_token_release, mf_token_change, and mf_token_not_found,
	 * each of which carry a token send right in the message 
	 * body thus guaranteeing that a NMS will not arrive until
	 * after they've been processed.  It's also possible for
	 * another NMS msg to follow on the heels of this msg but
	 * the tip->lock prevents it from racing ahead.
	 */
	mutex_lock(&tip->lock);
	while (seqno > tip->seqno) {
		/* printf("token no-more-senders: seqno=%d > tip->seqno=%d\n", 
		       seqno, tip->seqno);
		printf("  tip=0x%x waited=%s\n", tip, 
		       waited == TRUE ? "TRUE" : "FALSE");
		 */
		if (waited == TRUE)
			waited_more_than_once = TRUE;
		waited = TRUE;
		token_no_senders_wait_cnt++;	/* debug */
		tip->nms_waiting = 1;
		assert_wait((int)&tip->seqno, FALSE);
		mutex_unlock(&tip->lock);
		thread_block();
		mutex_lock(&tip->lock);
	}
	tip->seqno++;		/* must incr seqno for NMS too */

	/* printf("token no-senders: tip=0x%x state=%d reestablish=%s sendc=%d token_inuse=%d\n", tip, tip->state, mscount < tip->mscount ? "TRUE" : "FALSE", 
	   tip->send_count, token_inuse); */

	/*
	 * If the token has not been released, then do it.
	 * No more send rights can be created for the tip after
	 * rel_data_token.
	 */
	if (tip->state == T_ACQ || tip->state == T_ACQ_AND_RIP) {
		mutex_unlock(&tip->lock);  
		ASSERT(tip->vp);
#ifdef PFS
		/* 
		 * We need to reset the cached size of the PFS file
		 * back to NULL, (-1,-1), so that it can be recomputed the
		 * next time a token is acquired by another client.
		 * This keeps the token size and stripe-file sizes
		 * in sync with each other.
		 */
		if (VIO_IS_PFS(tip->vp)) {
#ifdef DEBUG_PFSTOKEN
			printf("token_port_no_senders: resetting PFS length\n");
#endif
			tip->vp->v_vm_info->pfs_length.shigh = VNOVAL;
			tip->vp->v_vm_info->pfs_length.slow  = VNOVAL;
		}
#endif
		rel_data_token(tip->vp->v_vm_info, tip->flags);
		mutex_lock(&tip->lock);  
	} 

	/*
	 * Receipt of a NMS message indicates that we may have lost
	 * track of tip->send_count (see token_port_callback_failure).
	 * Hence, force the send_count to 0.  But, only do this if we
	 * haven't created new send rights since the NMS msg was generated.
	 */
	if (mscount < tip->mscount) {
		if (error = mach_port_request_notification(mach_task_self(), 
					    port, MACH_NOTIFY_NO_SENDERS, 
					    tip->mscount, port,
					    MACH_MSG_TYPE_MAKE_SEND_ONCE, 
					    &dummy))
			panic("token_port_no_senders: request_not failed 0x%x",
			      error);
		mutex_unlock(&tip->lock);  
	} else {
		num_rights = tip->send_count;
		mutex_unlock(&tip->lock);  
		if (num_rights > 0)
			token_port_deallocate(tip, num_rights);
	}
}
