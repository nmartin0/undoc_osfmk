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
 * SSD HISTORY
 * $Log: xmm_svm.c,v $
 * Revision 1.15  1994/12/13  20:59:27  cfleck
 *  Reviewer: (Stephan, sz@esdc)
 *  Risk: low
 *  Benefit or PTS #: 11545
 *  Description:
 * 	This problem was caused by a pager lock request that arrives while XMM
 * 	is in the process of responding to a data request.  This lock request
 * 	is given priority in the request queue since it comes from the pager.
 * 	The pager lock request is handled correctly, but afterward XMM checks
 * 	the queue and finds the data request (one being processed already)
 *         on the queue and begins processing it again..  The fix for this is to
 *         remove the request before satisfying the request with
 * 	K_DATA_UNAVAILABLE, K_LOCK_REQUEST, etc, since these calls could block.
 *    	You have everything you need at this point anyway to satisfy the
 * 	request.
 *  Testing: Small version of ORNL MCAT (MUNOPS type test). 5 service nodes
 *           running service node SATs, plus parallel SATs in compute.
 *  Module(s): ./norma/xmm_svm.c
 *
 * Revision 1.14  1994/11/18  20:57:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/08/31  21:25:17  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.11.2.1  1994/08/08  23:54:32  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.12  1994/08/08  19:34:23  andyp
 * PTS #:	10338, 10339
 * Mandatory?:	Yes
 * Description: Don't issue proxy_lock_completed() until all expected
 * 	proxy_data_write_completed()'s have been received.
 * 	Added XMM function entry logging to the norma log ("show norma").
 * 	Added NORMA_LOG_ONLY bootmagic to log exactly one module id.
 * 	Upped the priority of the dipc_emmi_reply_threads above
 * 	that of ordinary dipc_kobj_server_threads.
 * Reviewer(s): rkl
 * Risk:	Low (compared to getting sporadic 0's or truncated files)
 * Testing:	sats, devloper tests, test cases pass.
 * Module(s):
 * 	M intel/pmap.c
 * 	M norma/xmm.c
 * 	M norma/xmm_buffer.c
 * 	M norma/xmm_copy.c
 * 	M norma/xmm_export.c
 * 	M norma/xmm_import.c
 * 	M norma/xmm_interpose.c
 * 	M norma/xmm_invalid.c
 * 	M norma/xmm_object.c
 * 	M norma/xmm_server.c
 * 	M norma/xmm_split.c
 * 	M norma/xmm_svm.c
 * 	M norma/xmm_user.c
 * 	A norma/xmm_dipc.h
 * 	M norma2/dipc_kserver.c
 * 	M norma2/norma_log.c
 * 	M norma2/norma_log.h
 *
 * Revision 1.11  1994/07/12  19:25:42  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.10  1994/03/29  13:22:49  fritz
 * Change XMM's page state admin code to two level. Fixes PTS #7142.
 *
 * Revision 1.9  1994/02/14  13:21:45  fritz
 * changes in XMM's page state admin code to reduce wired memory
 * requirements. Old code worked with bit fields in structures; new
 * code uses macros and requires 2 bytes of wired memory per page
 * instead of 8. Also get rid of obsolete "reply" field in gather structure.
 *
 *  Risk: medium
 *  Benefit or PTS #: cut down XMM wired memory consumption by 75%.
 *  Testing: various SVM/XMM related stress tests (overnight)
 *  Module(s): norma/xmm_svm.c - straight forward changes in many places.
 *
 * Revision 1.8  1994/02/10  10:14:26  fritz
 * Fix page leak due to incorrect reference counts to xmm objects.
 * (became appearent in case of multi-page memory_object_lock_request calls.
 * This happened for programs as simple as "write 64K to UFS file")
 * This was the underlying reason for panis in krealloc().
 *
 *  Reviewer: terry
 *  Risk: low
 *  Benefit or PTS #: 7142
 *  Testing: Run simple test program and verify xmm cleanup using ddb.
 *  Module(s): norma/xmm_svm.c
 *
 * Revision 1.7  1993/09/28  18:07:00  andyp
 * Update for the 1.2 release.
 *
 *
 *	- Added PENDING_TRACKING fields to follow data_write_completed
 *	activity through the layers of an XMM stack.
 *	- Added {k,m}_db_print methods.
 *	[alanl@osf.org]
 *
 * Revision 1.6  1993/07/22  02:22:02  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.5  1993/06/30  22:52:49  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:41:06  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:47:02  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:32  dleslie
 * Patch release of April 23
 *
 * Revision 1.3  1993/04/23  17:07:30  andyp
 * Added some code from [wyu@stps09.intel.com] that gets rid of
 * the "Text file busy" problem.  Cosmetic changes applied to
 * the RCS log.
 *
 * Revision 1.2  1993/04/12  03:19:38  SSD
 * 2nd code drop for pager flow control fixes.
 *
 * END SSD HISTORY
 */

/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_svm.c,v
 * Revision 1.2.6.6  1993/05/04  22:25:29  alanl
 * 	Occasionally, k_svm_lock_request would add requests without
 * 	updating the lookup hint.  Also, when pushing a request
 * 	from the request chain onto the equal list, k_svm_lock_request
 * 	wouldn't clear the request's chain pointers.  These fixes
 * 	originate with wyu@stps09.intel.com.  [wyu, alanl]
 * 	[1993/05/04  20:25:24  alanl]
 *
 * Revision 1.2.6.5  1993/04/26  14:14:41  mmp
 * 	Once again remove the useless cleanup code in xmm_ksvm_terminate
 * 	that crept back in with the previous submit.
 * 	[1993/04/26  14:14:18  mmp]
 * 
 * Revision 1.2.6.4  1993/04/15  22:47:51  alanl
 * 	Paging flow control (NORMA_VM).  Added support for
 * 	data_write_completed (interpose only), including
 * 	pending structures and lookup routines.  Changed
 * 	parameters to set_ready.  Logic for object termination
 * 	and debug code for tracking termination.  Support
 * 	lock_request operations on large ranges by using
 * 	scatter/gather.  [sjs]
 * 	Optimizations for long request chains.  [alanl]
 * 	On kobj termination, remove any pending requests
 * 	in the mobj.  [alanl]
 * 	[1993/04/15  22:13:19  alanl]
 * 
 * Revision 1.2.6.3  1993/03/18  21:22:47  mmp
 * 	Remove the code in xmm_ksvm_terminate that tried to clean up
 * 	if k_count > 0.  (Didn't make it into the previous submission.)
 * 	[1993/03/18  21:22:00  mmp]
 * 
 * Revision 1.2.6.2  1993/03/18  21:15:15  mmp
 * 	Change termination logic.  The release flag will never be set to
 * 	true from the kernel side, so remove the code that checked it from
 * 	m_svm_terminate, m_svm_change_completed, and m_svm_lock_completed.
 * 	There is now a terminated flag in the KOBJ, which is set in
 * 	m_svm_terminate.  Whichever one of terminate, change_completed,
 * 	or lock_completed decrements k_count to zero will call
 * 	xmm_ksvm_terminate if the terminated flag is set.  Remove code
 * 	in xmm_ksvm_terminate that tried to clean up if k_count > 0
 * 	since it is now always called when k_count==0.
 * 	[1993/03/18  21:14:15  mmp]
 * 
 * Revision 1.2  1992/11/25  01:17:32  robert
 * 	integrate chnages for norma_14 below
 * 
 * 	Alan Langerman (alanl) at Open Software Foundation 16-Nov-92
 * 	Printf in k_svm_data_error no longer needed.
 * 	[1992/11/20  00:56:34  robert]
 * 
 * 	fix history
 * 	[1992/11/09  22:31:29  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:52:55  robert]
 * 
 * Revision 0.0  92/10/14            sjs
 * 	Deal with write tokens that disappear when a kobj is terminated.
 * 	Dont prematurely terminate kobjs - the release flag for them is
 * 	strictly advisory due to outstanding requests the server layer
 * 	does not know about.  Introduced new K_RELEASE method to do
 * 	final clean up. Added pretty print routines for debugging.
 * 	[92/10/14            sjs]
 * 
 * Revision 0.0  92/09/30            dlb
 * 	Turn off xmm_buffer.
 * 
 * 	Revision 1.1  1992/11/05  21:00:48  robert
 * 	Initial revision
 * 	[92/09/30            dlb]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.4.3.9  92/09/15  17:35:31  jeffreyh
 * 	Ignore COPY_TEMPORARY for shared objects in k_svm_set_ready
 * 	pending real implementation of this.
 * 	[92/08/14            dlb]
 * 
 * 	Changed satisfy_request routines to allow error code to be passed
 * 	through.
 * 	[92/07/17            sjs]
 * 
 * 	Extend svm_state in chunks in m_svm_extend.  Add count of kobjs
 * 	to mobj, and use to pass access through if there's only one kernel.
 * 	Rewrite m_svm_data_supply() to include initial lock logic.
 * 	[92/07/06            dlb]
 * 
 * Revision 2.4.3.8  92/06/24  18:03:24  jeffreyh
 * 	Massive changes for XMM Framework Cleanup:
 * 	  Lots of calling sequence changes.  No more xmm_kobj_link().
 * 	  Reworked ksvm and msvm objects.  ksvm is now used only for
 * 	  communication with kernels, msvm only for communication with
 * 	  managers.  ksvm object is now created explicitly by
 * 	  xmm_ksvm_create instead of implicitly by m_svm_init.
 * 	  Only this module knows about their relationship.
 * 	  Add SHOULD_TERMINATE state for use in waiting for pending
 * 	  replies.  Add missing initializations.  Reorganize termination
 * 	  logic, including discarding pending changes and requests.
 * 	  Add logic to avoid switching order of terminate and change_
 * 	  completed if terminate came first.  Don't pass user-specified
 * 	  copy strategies to kernel through this layer.
 * 	[92/06/24            dlb]
 * 
 * 	Add k_count to objects (mobj and kobj); use to synchronize terminate
 * 	against pending replies from kernels.  Add release
 * 	logic to lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	use_old_pageout --> use routine. Implement data_initialize
 * 	logic in m_svm_data_write_return, including MOBJ size check.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.4.3.7  92/05/27  10:09:52  jeffreyh
 * 	Correct typos.
 * 
 * Revision 2.4.3.6  92/05/27  00:56:36  jeffreyh
 * 	Rework storage of prot and lock to cut down on memory usage.
 * 	Also add CALLED case to m_svm_init and cleaned up code.
 * 	[92/05/26            dlb]
 * 
 * 	Fix m_svm_process_pager_request to check for pending operation
 * 	counts and move them to the next request.  Also apply some cleanup
 * 	and optimization to the VM_PROT_NO_CHANGE code.
 * 	[92/05/26            dlb]
 * 
 * Revision 2.4.3.5.1.1  92/05/06  17:42:52  jeffreyh
 * 	Handle VM_PROT_NO_CHANGE in m_svm_process_pager_request.
 * 	Some of these changes are brute force and will benefit from
 * 	optimizations.
 * 	[92/04/29            dlb]
 * 
 * Revision 2.4.3.5  92/04/08  15:46:48  jeffreyh
 * 	Put markers in for new data_supply functionality.
 * 	[92/04/08  15:12:47  jeffreyh]
 * 
 * Revision 2.4.3.4  92/03/28  10:13:31  jeffreyh
 * 	Changed k_svm_data_write_return to use the passed in release
 * 	flag, rather than counting on the MOBJ being set up.  Fixes
 * 	problem of reply going to m_o_data_return rather than
 * 	m_o_data_write.
 * 	[92/03/26            sjs]
 * 	Fix k_svm_set_ready() logic to process changes after the
 * 	object is set ready if no reply has been requested.
 * 	[92/03/25            dlb]
 * 
 * 	Changed M_CHANGE_COMPLETED call to conditionally provide a
 * 	release flag - svm_set_ready() will typically not do a release
 * 	of the object.
 * 	[92/03/25            sjs]
 * 	Changed data_write to data_write_return and deleted data_return
 * 	 method.  Changed the mobj to handle multiple change requests and
 * 	 provided logic to contact all kernels and synchronize requests
 * 	 when a reply exists.  Changed delete and change_completed; all
 * 	 in support of m_o_change_attributes().
 * 	[92/03/20            sjs]
 * 
 * Revision 2.4.3.3  92/02/21  11:28:23  jsb
 * 	Replaced xmm_reply_allocate_mobj with m_svm_do_request, which now takes
 * 	a reference to mobj. m_svm_lock_completed now deallocates reply as well
 * 	as reference to mobj.
 * 	[92/02/18  17:31:27  jsb]
 * 
 * 	Cosmetic changes, including vm_page_size -> PAGE_SIZE.
 * 	[92/02/18  08:01:18  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Added MOBJ_STATE_TERMINATED to detect init/terminate race.
 * 	Added memory_object parameter to xmm_svm_create, and memory_object
 * 	field to struct mobj, so that m_svm_terminate can call
 * 	xmm_object_release on memory_object. Move M_TERMINATE call
 * 	to new routine xmm_svm_destroy, which is called from xmm_object
 * 	module only when there are no references to xmm object.
 * 	This fixes race between xmm_object_by_memory_object (where someone
 * 	decides to use our existing svm stack) and m_svm_terminate
 * 	(where we used to tear down the stack as soon as all the kernels
 * 	we knew about had terminated the object).
 * 	[92/02/16  15:50:31  jsb]
 * 
 * 	Changed copy strategy management to handle (naively)
 * 	MEMORY_OBJECT_COPY_TEMPORARY (by passing it up unchanged).
 * 	This will break in its current form when we enable VM_INHERIT_SHARE.
 * 	(Added appropriate checks to panic in this case.) Removed dead
 * 	routines xmm_svm_{set_access,initialize}. Changed debugging printfs.
 * 	[92/02/11  11:32:58  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	Use xmm_buffer layer to buffer data writes of migrating pages.
 * 	General cleanup.
 * 	[92/02/09  13:58:24  jsb]
 * 
 * Revision 2.4.3.1  92/01/21  21:54:57  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:46:55  jsb]
 * 
 * 	Fixes from OSF.
 * 	[92/01/17  14:15:53  jsb]
 * 
 * Revision 2.4.1.1  92/01/15  12:17:45  jeffreyh
 * 	Deallocate memory_object_name port when not propagating
 *	termination. (dlb)
 * 
 * Revision 2.4  91/08/03  18:19:47  jsb
 * 	Added missing type cast.
 * 	[91/07/17  14:07:46  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:40  jsb
 * 	Now allow objects to grow in size (as temporary objects do).
 * 	Merged user_t and kobj structures. Do garbage collection.
 * 	Now pass up all set_attribute calls, not just first.
 * 	Use zone for request structures.
 * 	[91/06/29  15:43:16  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:43  jsb
 * 	First checkin.
 * 	[91/06/17  11:04:03  jsb]
 * 
 */
/* CMU_ENDHIST */
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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	norma/xmm_svm.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Xmm layer providing consistent shared virtual memory.
 */

#ifdef	KERNEL
#include <norma/xmm_obj.h>
#include <mach/vm_param.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#else	KERNEL
#include <xmm_obj.h>
#endif	KERNEL

#include <norma/xmm_dipc.h>

#define	dprintf	xmm_svm_dprintf

#define	USE_XMM_BUFFER	0

typedef struct request *	request_t;
typedef struct mchange *	mchange_t;
typedef struct pending *	pending_t;

#define	REQUEST_NULL		((request_t) 0)
#define	CHANGE_NULL		((mchange_t) 0)
#define	PENDING_NULL		((pending_t) 0)

#define	MOBJ_STATE_UNCALLED		0
#define	MOBJ_STATE_CALLED		1
#define	MOBJ_STATE_READY		2
#define MOBJ_STATE_SHOULD_TERMINATE	3
#define	MOBJ_STATE_TERMINATED		4

#define	DATA_NONE		((vm_offset_t) 0)
#define	DATA_UNAVAILABLE	((vm_offset_t) 1)
#define	DATA_ERROR		((vm_offset_t) 2)

#define	K			((struct kobj *) k)
#define	KK(xobj)		((struct kobj *) xobj)

#define	REQUEST_TRACKING	0
#if	REQUEST_TRACKING
typedef struct req_track {
	request_t	req;
	char		*op;
	unsigned int	req_count;
	vm_offset_t	offset;
} *req_track_t;
#define	MAX_OBJ_TRACKS	40
#define	MAX_SYS_TRACKS	400
struct req_track	global_tracks[MAX_SYS_TRACKS];
unsigned int		global_tracks_mobj[MAX_SYS_TRACKS];
unsigned int		global_track_index;
boolean_t		svm_verify_requests_closely = FALSE;
#else
#define	verify_request_list(mobj)
#define	k_verify_request_list(kobj)
#define	track(mobj,req,op)
#endif

/*
 * lock is set when pager gives us a message.
 * prot is set when we send message to kernels;
 * it should simply reflect max of all corresponding kobj prot.
 */

typedef unsigned char svm_state_data_t, *svm_state_t;

struct mobj {
	struct xmm_obj	obj;
	xmm_obj_t	kobj_list;
	int		kobj_count;
	int		state;
	unsigned int	num_pages;
	queue_head_t	requests;		/* requests at diff offsets */
	unsigned int	request_count;
	request_t	last_found;		/* lookup optimization */
	svm_state_t	svm_state;
	boolean_t	may_cache;
	int		use_routine;
	ipc_port_t	memory_object;		/* for xmm_object_release */
	ipc_port_t	memory_object_name;	/* at most one send right */
	memory_object_copy_strategy_t
			copy_strategy;
	mchange_t	change;
	int		k_count;		/* outstanding kernel reqs. */
	boolean_t	write_completions;
	vm_size_t	cluster_size;
	pending_t	pending;		/* data_writes needing completeds */
#if	PENDING_TRACKING
	int		p_seen;
	int		p_matched;
	int		p_terminated;
#endif
#if	REQUEST_TRACKING
	struct req_track tracks[MAX_OBJ_TRACKS];
	unsigned int	track_index;
#endif
};

struct mchange {
	mchange_t	next;
	int		k_count;		/* -> m_yield_count */
	boolean_t	object_ready;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
	int		use_routine;
	ipc_port_t	memory_object_name;
	xmm_reply_t	reply;
};


union who {
	xmm_obj_t	kobj;
	xmm_reply_t	reply;
};

/*
 * XXX some of these fields could be aliased to save space
 * XXX eg: needs_data,should_clean; lock_value,desired_access
 *
 * XXX should probably add ref counts to kobjs....
 */
struct request {
	queue_chain_t	chain_neq;		/* must be first */
	union who	who;
	int		m_count;		/* -> m_yield_count */
	int		k_count;		/* -> m_yield_count */
	boolean_t	is_kernel;
	boolean_t	needs_data;		/* ours alone */
	boolean_t	should_clean;		/* same as needs_data? */
	boolean_t	should_flush;
	vm_prot_t	desired_access;
	vm_prot_t	lock_value;
	vm_offset_t	offset;			/* -> page */
	request_t	next_eq;
	boolean_t	sending;		/* send or hold request */
};

struct kobj {
	struct xmm_obj	obj;
	unsigned int	num_pages;		/* needed for deallocation */
	svm_state_t	svm_state;
	xmm_obj_t	svm_mobj;			/* back pointer */
	xmm_obj_t	next;
	boolean_t	inited;			/* Ok to launch ops? */
	boolean_t	terminated;
	int		k_count;
	struct gather	*gather;
};

struct gather {
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_offset_t	begin_offset;
	boolean_t	should_clean;
	boolean_t	should_flush;
	vm_prot_t	lock_value;
};

#define	GATHER_NULL	(struct gather *)0

	
/*
 * The pending structure is used to track data_{write,return,initialize}
 * calls that require a data_write_completed back from the manager.
 */
struct pending {
	xmm_obj_t	kobj;
	vm_offset_t	offset;			/* -> page */
	pending_t	next;
};



/*
 *	The SVM layer splits the XMM interface into two halves.
 *	The mobj is the manager side xmm_obj; the only invocations
 *	valid on it are those generated by a manager, i.e., the k_
 *	invocations directed at the kernel.  The kobj is the kernel
 *	(there is one kobj per kernel, hence a single mobj may have
 *	multiple kobj's).  The only invocations valid on it are those
 *	generated by the kernel (i.e., the m_ invocations directed at
 *	the manager).
 *
 *	NOTE: The get_attributes invocation is absorbed by the
 *	user layer, and so should never reach here.  Both
 *	supply_completed and copy are to be implemented.  XXX XXX
 */

#define	m_msvm_init		m_invalid_init
#define	m_msvm_terminate	m_invalid_terminate
#define	m_msvm_copy		m_invalid_copy
#define	m_msvm_data_request	m_invalid_data_request
#define	m_msvm_data_unlock	m_invalid_data_unlock
#define	m_msvm_data_write_return m_invalid_data_write_return
#define	m_msvm_lock_completed	m_invalid_lock_completed
#define	m_msvm_supply_completed	m_invalid_supply_completed
#define	m_msvm_change_completed	m_invalid_change_completed
#define	k_msvm_data_unavailable	k_svm_data_unavailable
#define	k_msvm_get_attributes	k_invalid_get_attributes
#define	k_msvm_lock_request	k_svm_lock_request
#define	k_msvm_data_error	k_svm_data_error
#define	k_msvm_set_ready	k_svm_set_ready
#define	k_msvm_destroy		k_svm_destroy
#define	k_msvm_data_supply	k_svm_data_supply
#define	k_msvm_release		k_invalid_release
#define	m_msvm_deallocate	m_svm_deallocate
#define	k_msvm_data_write_completed	k_svm_data_write_completed

extern m_msvm_db_print();
xmm_decl(msvm, "msvm", sizeof(struct mobj));

#define	m_ksvm_init		m_svm_init
#define	m_ksvm_terminate	m_svm_terminate
#define	m_ksvm_copy		m_invalid_copy
#define	m_ksvm_data_request	m_svm_data_request
#define	m_ksvm_data_unlock	m_svm_data_unlock
#define	m_ksvm_data_write_return m_svm_data_write_return
#define	m_ksvm_lock_completed	m_svm_lock_completed
#define	m_ksvm_supply_completed	m_invalid_supply_completed
#define	m_ksvm_change_completed	m_svm_change_completed
#define	k_ksvm_data_unavailable	k_invalid_data_unavailable
#define	k_ksvm_get_attributes	k_invalid_get_attributes
#define	k_ksvm_lock_request	k_invalid_lock_request
#define	k_ksvm_data_error	k_invalid_data_error
#define	k_ksvm_set_ready	k_invalid_set_ready
#define	k_ksvm_destroy		k_invalid_destroy
#define	k_ksvm_data_supply	k_invalid_data_supply
#define	k_ksvm_release		k_invalid_release
#define	m_ksvm_deallocate	k_svm_deallocate
#define	k_ksvm_data_write_completed	k_interpose_data_write_completed

extern m_ksvm_db_print();
xmm_decl(ksvm, "ksvm", sizeof(struct kobj));

extern void	xmm_object_release();

boolean_t	m_svm_add_request();
request_t	m_svm_lookup_request();
request_t	m_svm_remove_request();
xmm_obj_t	m_svm_lookup_pending();
void		m_svm_satisfy_request();
void		m_svm_satisfy_kernel_request();
void		m_svm_satisfy_pager_request();
void		m_svm_process_request();
void		m_svm_process_kernel_request();
void		m_svm_process_pager_request();
void		m_svm_alloc_pending();
#if	MACH_KDB
int		m_svm_print( /* mobj */ );
#endif

zone_t		xmm_svm_request_zone;
zone_t		xmm_svm_change_zone;
zone_t		xmm_svm_pending_zone;
zone_t		xmm_svm_gather_zone;

int C_mobj_state = 0;
int C_user_state = 0;

/*
 * Macros for access to page state tables
 *
 * Update of C_mobj_state and C_user_state is hardwired in the macros.
 * SVM_SET_STATE is only called on kobjects and updates C_user_state
 * SVM_SET_LOCK/PROT are only called on mobjects and updates C_mobj_state
 */

#define XMM_SVM_EXTEND_CHUNK	8
#define XMM_DIRECT_PAGES	128

#define SVM_BUILD(prot, lock) ((lock<<4) | prot)
#define SVM_IND_BUILD(p, l) (svm_state_t) (((SVM_BUILD(p, l)) << 1) | 1)
#define SVM_IND_VALUE(abused_pointer) (((int) (abused_pointer)) >> 1)
#define SVM_IS_POINTER(abused_pointer) ((((int) (abused_pointer)) & 1 ) == 0)

#define SVM_GET_PROT(obj, page) (vm_prot_t) (( \
		(obj->num_pages <= XMM_DIRECT_PAGES) ? \
		obj->svm_state[page] : \
		xmm_state_table_get(obj->svm_state, page)) & 0xf)

#define SVM_GET_LOCK(obj, page) (vm_prot_t) (( \
		(obj->num_pages <= XMM_DIRECT_PAGES) ? \
		obj->svm_state[page] : \
		xmm_state_table_get(obj->svm_state, page)) >> 4)

#define SVM_SET_STATE(obj, page, prot, lock) \
                MACRO_BEGIN \
		if (obj->num_pages <= XMM_DIRECT_PAGES) \
                	obj->svm_state[page] = SVM_BUILD(prot, lock); \
		else \
			xmm_state_table_set(obj->svm_state, page, \
				prot, lock, &C_user_state); \
                MACRO_END

#define SVM_SET_LOCK(obj, page, lock) \
                MACRO_BEGIN \
		if (obj->num_pages <= XMM_DIRECT_PAGES) { \
                	svm_state_t p = &obj->svm_state[page]; \
                	*p &= 0xf; \
                	*p |= (lock<<4); \
		} else \
			xmm_state_table_set(obj->svm_state, page, \
				VM_PROT_NO_CHANGE, lock, &C_mobj_state); \
                MACRO_END

#define SVM_SET_PROT(obj, page, prot) \
                MACRO_BEGIN \
		if (obj->num_pages <= XMM_DIRECT_PAGES) { \
                	svm_state_t p = &obj->svm_state[page]; \
                	*p &= 0xf0; \
                	*p |= prot; \
		} else \
			xmm_state_table_set(obj->svm_state, page, \
				prot, VM_PROT_NO_CHANGE, &C_mobj_state); \
                MACRO_END


/* XXX should be implemented by kalloc.c */
/* XXX should kalloc have asm help for round-to-power-of-two? */
krealloc(old_buf_p, old_size, new_size, counter)
	char **old_buf_p;
	vm_size_t old_size;
	vm_size_t new_size;
	int *counter;
{
	char *new_buf;

	xmm_entry4(krealloc, old_buf_p, old_size, new_size, counter);

	new_buf = (char *) kalloc(new_size);
	if (new_buf == (char *) 0) {
		panic("krealloc");
	}
	if (old_size > 0) {
		bcopy(*old_buf_p, new_buf, old_size);
		kfree(*old_buf_p, old_size);
	}
	*counter += (new_size - old_size);
	*old_buf_p = new_buf;
}


void
m_svm_alloc_pending(mobj, kobj, offset)
	xmm_obj_t mobj;
	xmm_obj_t kobj;
	vm_offset_t offset;
{
	pending_t p;

	xmm_entry3(m_svm_alloc_pending, mobj, kobj, offset);

#if	REQUEST_TRACKING
	if (svm_verify_requests_closely == TRUE)
		assert(m_svm_lookup_pending(mobj, offset) == XMM_OBJ_NULL);
#endif
	p = (pending_t)zalloc(xmm_svm_pending_zone);
	p->kobj = kobj;
	p->offset = offset;
	p->next = MOBJ->pending;
	MOBJ->pending = p;
#if	PENDING_TRACKING
	MOBJ->p_seen++;
#endif
}


void xmm_state_table_extend();
void xmm_state_table_set();

void m_svm_extend(mobj, new_num_pages)
	xmm_obj_t mobj;
	unsigned int new_num_pages;
{
	xmm_obj_t kobj;
	unsigned int extend_mask;
	unsigned int old_num_pages = MOBJ->num_pages;

	xmm_entry2(m_svm_extend, mobj, new_num_pages);

	/*
	 *	Extend svm state in chunks of XMM_SVM_EXTEND_CHUNK up to
	 *	XMM_DIRECT_PAGES, and in chunks of XMM_DIRECT_PAGES thereafter.
	 * 	For this code to work, both XMM_SVM_EXTEND_CHUNK and
	 *	XMM_DIRECT_PAGES must be a power of two.
	 */

	if (new_num_pages > XMM_DIRECT_PAGES)
		extend_mask = XMM_DIRECT_PAGES - 1;
	else
		extend_mask = XMM_SVM_EXTEND_CHUNK -1;
	new_num_pages = (new_num_pages + extend_mask) & ~extend_mask;

	assert(new_num_pages > old_num_pages);

	xmm_state_table_extend( &MOBJ->svm_state, &MOBJ->num_pages,
				new_num_pages, &C_mobj_state,
				VM_PROT_NONE,VM_PROT_ALL);
	for (kobj = MOBJ->kobj_list; kobj; kobj = KOBJ->next) {
		assert(KOBJ->num_pages == old_num_pages);
		xmm_state_table_extend( &KOBJ->svm_state, &KOBJ->num_pages,
				new_num_pages, &C_user_state, VM_PROT_NONE,
                        	0 /* lock field unused in KOBJ's */ );
	}
}

/*
 * Expand page state table. The table may be one or two level.
 * If required, conversion is done as required.
 *
 * XXX 
 * This implemetation does not allow indirect arrays of 
 * <= XMM_DIRECT_PAGES pages. Eliminating this limitation would be a
 * valuable optimization.
 */

void
xmm_state_table_extend(state_table, num_pages, new_num_pages, counter, prot, lock)
	svm_state_t	*state_table;   /* IN/OUT */
        unsigned int	*num_pages;	/* IN/OUT */
        unsigned int	new_num_pages;
	int		*counter;	/* IN/OUT */
	vm_prot_t	prot, lock;
{
	svm_state_t	*indirect_table;
	int i;

	xmm_entry6(xmm_state_table_extend,
		state_table, num_pages, new_num_pages, counter, prot, lock);
	/*
	 * The expansion is done in three incremental steps:
	 *    1) grow within first page range   (direct --> direct)
	 *    2) switch from direct to indirect presentation
	 *    3) grow by additional page ranges (indirect --> indirect)
	 */

	/*
	 * First grow within first page range. Can be skipped if
	 * - we are already outside the first page range, or
	 * - we grow from zero immediately to full page range(s)
	 */

	if (!((*num_pages >= XMM_DIRECT_PAGES) ||
	    ((*num_pages == 0) && (new_num_pages > XMM_DIRECT_PAGES)))) {

		svm_state_t	table;
		int upper_limit = (new_num_pages > XMM_DIRECT_PAGES) ?
					XMM_DIRECT_PAGES : new_num_pages;

		krealloc((char**) state_table, 
			*num_pages * sizeof(svm_state_data_t),
			upper_limit * sizeof(svm_state_data_t),
			counter);
		
		table = *state_table;
		for (i = *num_pages; i < upper_limit; i++)
			table[i] = SVM_BUILD(prot, lock);
		*num_pages = upper_limit;

		if (*num_pages == new_num_pages)
			return;
	}

	/*
	 * Now either switch to indirect presentation and allocate a
	 * indirect page state pointer array, or resize the already
	 * existant array.
	 */

	assert(*num_pages < new_num_pages);
	assert((*num_pages & (XMM_DIRECT_PAGES - 1)) == 0);
	assert((new_num_pages & (XMM_DIRECT_PAGES - 1)) == 0);

	if (*num_pages <= XMM_DIRECT_PAGES) {

		/*
		 * (*num_pages) is either XMM_DIRECT_PAGES or ZERO.
		 * Switch to indirect presentation.
		 */

		int size=(new_num_pages/XMM_DIRECT_PAGES)*sizeof(svm_state_t);
		indirect_table = (svm_state_t*) kalloc(size);
		if (*num_pages)
			indirect_table[0] = *state_table; 
		*state_table=(svm_state_t) indirect_table;
		*counter += size;

	} else {

		/*
		 * (*num_pages) is a multiple of XMM_DIRECT_PAGES. We are
		 * already using two levels. Allocate a larger indirect block.
		 */
		
		krealloc((char**) state_table, 
			(*num_pages / XMM_DIRECT_PAGES) * sizeof(svm_state_t),
			(new_num_pages / XMM_DIRECT_PAGES) *sizeof(svm_state_t),
			counter);
		indirect_table = (svm_state_t *) *state_table;

	}

	/*
	 * Initialize new part of the indirect page state table (not expanded)
	 */

	for (i = *num_pages; i < new_num_pages; i += XMM_DIRECT_PAGES)
		indirect_table[i/XMM_DIRECT_PAGES] =
			SVM_IND_BUILD(prot, lock);
	*num_pages = new_num_pages;
}


/*
 * Free two level page state table. The table may be one or two level.
 */

void xmm_state_table_free(state_table, num_pages, counter)
        svm_state_t     state_table;
        unsigned int    num_pages;
        int             *counter;       /* IN/OUT */
{
	svm_state_t *indirect_table;
	int i;

	xmm_entry3(xmm_state_table_free, state_table, num_pages, counter);

	if (num_pages == 0)
		return;

	if (num_pages <= XMM_DIRECT_PAGES) {
		kfree(state_table, num_pages * sizeof(svm_state_data_t));
		*counter -= num_pages * sizeof(svm_state_data_t);
		return;
	}

	indirect_table = (svm_state_t*) state_table;
	for (i=0; i < (num_pages / XMM_DIRECT_PAGES); i++) {
		if (SVM_IS_POINTER(indirect_table[i])) {
			kfree(indirect_table[i],
				 XMM_DIRECT_PAGES * sizeof(svm_state_data_t));
			*counter -= XMM_DIRECT_PAGES * sizeof(svm_state_data_t);
		}
	}
	assert((num_pages & (XMM_DIRECT_PAGES-1)) == 0);
	kfree(indirect_table, num_pages/XMM_DIRECT_PAGES*sizeof(svm_state_t));
	*counter -= num_pages/XMM_DIRECT_PAGES*sizeof(svm_state_t);
}


/*
 * Get an entry from page state table. Only called if table is really
 * two level.
 */

xmm_state_table_get(indirect_table, page)
	svm_state_t  *indirect_table;
	int          page;
{
	xmm_entry2(xmm_state_table_get, indirect_table, page);

	indirect_table += (page / XMM_DIRECT_PAGES);
	if (SVM_IS_POINTER(*indirect_table)) {
		svm_state_t state_table = *indirect_table;
		return(state_table[page & (XMM_DIRECT_PAGES-1)]);
	} else
		return(SVM_IND_VALUE(*indirect_table));
}


/*
 * Store entries into from page state table. Only called if table is really
 * two level. VM_PROT_NO_CHANGE is a legal value for prot and lock.
 */


void xmm_state_table_set(indirect_table, page, prot, lock, counter)
	svm_state_t  *indirect_table;
	int          page;
	vm_prot_t    prot, lock;
	int          *counter;
{
	svm_state_t state_table;
	int i, size;

	xmm_entry5(xmm_state_table_set,
		indirect_table, page, prot, lock, counter);
	indirect_table += (page / XMM_DIRECT_PAGES);

	if (!SVM_IS_POINTER(*indirect_table)) {

		/*
		 * determine, if we must expand, or if the range value
		 * is ok anyway. In the latter case, simply return.
		 */

		int range_value = SVM_IND_VALUE(*indirect_table);


		if (prot == VM_PROT_NO_CHANGE) {
			if ((range_value>>4) == lock)
				return;
		} else if (lock == VM_PROT_NO_CHANGE) {
			if ((range_value & 0xf) == prot)
				return;
		} else if (range_value == SVM_BUILD(prot, lock))
			return;

		/*
		 * Ok, we are still here. Expand the page range.
		 */


		size = XMM_DIRECT_PAGES * sizeof(svm_state_data_t);
		state_table = (svm_state_t) kalloc(size);
		for (i=0; i<XMM_DIRECT_PAGES; i++)
			state_table[i] = range_value;
		*counter += size;
		*indirect_table = state_table;
	} 

	/*
	 * The page range in question is expanded. Modify the
	 * appropriate values
	 */

	assert(SVM_IS_POINTER(*indirect_table));

	page &= (XMM_DIRECT_PAGES - 1);
	state_table = &(*indirect_table)[page];

	if (prot == VM_PROT_NO_CHANGE) {
		*state_table &= 0xf;
		*state_table |= (lock<<4);
	} else if (lock == VM_PROT_NO_CHANGE) {
		*state_table &= 0xf0;
		*state_table |= prot;
	} else
		*state_table = SVM_BUILD(prot, lock);
}



kern_return_t
xmm_svm_create(old_mobj, memory_object, new_mobj)
	xmm_obj_t old_mobj;
	ipc_port_t memory_object;
	xmm_obj_t *new_mobj;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry3(xmm_svm_create, old_mobj, memory_object, new_mobj);

#if	USE_XMM_BUFFER
	kr = xmm_buffer_create(old_mobj, &old_mobj);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
#endif	USE_XMM_BUFFER
	kr = xmm_obj_allocate(&msvm_class, old_mobj, &mobj);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	MOBJ->kobj_list = XMM_OBJ_NULL;
	MOBJ->kobj_count = 0;
	MOBJ->state = MOBJ_STATE_UNCALLED;
	MOBJ->num_pages = 0;
	MOBJ->request_count = 0;
	queue_init(&MOBJ->requests);
#if	REQUEST_TRACKING
	track_init(&MOBJ->tracks, &MOBJ->track_index);
#endif
	MOBJ->last_found = REQUEST_NULL;
	MOBJ->svm_state = (svm_state_t) 0;
	MOBJ->may_cache = FALSE;
	MOBJ->use_routine = XMM_USE_DATA_WRITE;  /* XXX change to RETURN */
	MOBJ->memory_object = memory_object;
	MOBJ->memory_object_name = IP_NULL;
	MOBJ->copy_strategy = MEMORY_OBJECT_COPY_NONE;
	MOBJ->change = CHANGE_NULL;
	MOBJ->k_count = 0;
	MOBJ->pending = PENDING_NULL;
	MOBJ->write_completions = FALSE;
#if	PENDING_TRACKING
	MOBJ->p_seen = 0;
	MOBJ->p_matched = 0;
	MOBJ->p_terminated = 0;
#endif
	*new_mobj = mobj;
	return KERN_SUCCESS;
}

kern_return_t
xmm_ksvm_create(mobj, new_kobj)
	xmm_obj_t mobj;
	xmm_obj_t *new_kobj;
{
	xmm_obj_t kobj;

	xmm_entry2(xmm_ksvm_create, mobj, new_kobj);

	assert(mobj->class == &msvm_class);

	if (xmm_obj_allocate(&ksvm_class, XMM_OBJ_NULL, &kobj)) {
		panic("xmm_ksvm_create");
	}

	KOBJ->num_pages = 0;
	if (MOBJ->num_pages)
		xmm_state_table_extend(&KOBJ->svm_state, &KOBJ->num_pages,
			 MOBJ->num_pages, &C_user_state, 0, 0);
	else
		KOBJ->svm_state = (svm_state_t) 0;

	KOBJ->next = MOBJ->kobj_list;
	MOBJ->kobj_list = kobj;
	MOBJ->kobj_count++;
	KOBJ->k_count = 0;
	KOBJ->inited = FALSE;
	KOBJ->svm_mobj = mobj;
	KOBJ->terminated = FALSE;

	/*
	 * If there are multiple kernels, then we had better be
	 * using MEMORY_OBJECT_COPY_NONE, at least until we get
	 * trickier about changing copy strategies.
	 */
	if (MOBJ->kobj_count > 1 &&
	    MOBJ->copy_strategy != MEMORY_OBJECT_COPY_NONE) {
		panic("losing big on multiple copies of temporary object");
	}

	*new_kobj = kobj;
	return KERN_SUCCESS;
}

m_svm_init(kobj, pagesize, internal, size)
	xmm_obj_t kobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	xmm_obj_t mobj;

	xmm_entry4(m_svm_init, kobj, pagesize, internal, size);

	assert(kobj->class == &ksvm_class);
#ifdef	lint
	M_INIT(mobj, pagesize, internal, size);
#endif	lint
	assert(pagesize == PAGE_SIZE);
	assert(KOBJ->inited == FALSE);
	KOBJ->inited = TRUE;
	mobj = KOBJ->svm_mobj;
	
	switch (MOBJ->state) {

	case MOBJ_STATE_UNCALLED:

		/*
		 *	Haven't sent the init operation to
		 *	the pager yet.  Mark the object and do it.
		 */
		MOBJ->state = MOBJ_STATE_CALLED;
		M_INIT(mobj, PAGE_SIZE, internal, size);
		break;

	case MOBJ_STATE_CALLED:

		/*
		 *	Pager has already been called; set_ready
		 *	will come back and init this kernel in response.
		 *	Nothing else to do here.
		 */
		break;

	case MOBJ_STATE_READY:

		/*
		 *	This object is already ready; tell the new kernel.
		 *	This is the most common case.
		 */
		assert(MOBJ->memory_object_name != IP_NULL);
		ipc_port_copy_send(MOBJ->memory_object_name);
		K_SET_READY(kobj, OBJECT_READY_TRUE, MOBJ->may_cache,
			    MOBJ->write_completions, MOBJ->copy_strategy, 
			    MOBJ->cluster_size, XMM_USE_DATA_WRITE,
			    MOBJ->memory_object_name, XMM_REPLY_NULL);
		break;

	case MOBJ_STATE_SHOULD_TERMINATE:
	case MOBJ_STATE_TERMINATED:

		/*
		 *	Can't happen.  Kernel has been fixed.
		 */
		panic("m_svm_init: terminate/lookup race");

	default:

		/*
		 *	Can't happen.  Something is really wrong.
		 */
		printf("MOBJ state = %d\n", MOBJ->state);
		panic("m_svm_init: bad MOBJ state");
	}

	return KERN_SUCCESS;
}

m_svm_terminate(kobj, release)
	xmm_obj_t kobj;
	boolean_t release;
{
	xmm_entry2(m_svm_terminate, kobj, release);

#ifdef	lint
	M_TERMINATE(kobj, release);
#endif	lint

	KOBJ->terminated = TRUE;
	/*
	 * If there are outstanding requests, do not actually tear
	 * things down yet.  When the completions get back, they
	 * will check for KOBJ->terminated and k_count==0 and do
	 * the teardown then.
	 */
	if (KOBJ->k_count == 0) {
      		xmm_ksvm_terminate(kobj);
	}

	return KERN_SUCCESS;
}


#define	TERMINATE_PENDING	1
#if	TERMINATE_PENDING
/*
 *	At the time we terminate the kobj, we probably
 *	should deallocate any pending requests that are
 *	pointing at it.
 */
unsigned int	c_xmm_ksvm_terminate_pending = 0; /* alan */
#endif

xmm_ksvm_terminate(kobj)
	xmm_obj_t kobj;
{
	xmm_obj_t mobj;
	xmm_obj_t kobj_terminated, *kp;
#if	TERMINATE_PENDING
	pending_t p, *pp;
#endif

	xmm_entry1(xmm_ksvm_terminate, kobj);
	K_RELEASE(kobj);
	/*
	 * Remove kobj from list and free its resources.
	 */
	mobj = KOBJ->svm_mobj;

	kobj_terminated = kobj;
	for (kp = &MOBJ->kobj_list; kobj = *kp; kp = &KOBJ->next) {
		if (kobj == kobj_terminated) {
			*kp = KOBJ->next;
			MOBJ->kobj_count--;
			break;
		}
	}
	kobj = kobj_terminated;
	KOBJ->svm_mobj = XMM_OBJ_NULL;

#if	TERMINATE_PENDING
	/*
	 *	Blow away any pending operations.
	 */
	assert(mobj->class == &msvm_class);
	pp = &MOBJ->pending;
	while ((p = *pp) != PENDING_NULL) {
		if (p->kobj == kobj) {
			*pp = p->next;
			zfree(xmm_svm_pending_zone, (vm_offset_t) p);
			++c_xmm_ksvm_terminate_pending;
#if	PENDING_TRACKING
			MOBJ->p_terminated++;
#endif
		} else
			pp = &p->next;
	}
#endif

	/*
	 * Release one reference to xmm object. If there are no
	 * more references, then xmm_svm_destroy will be called.
	 */
	xmm_object_release(MOBJ->memory_object);

	return KERN_SUCCESS;
}

int xmm_terminate_pending = 0;

void
xmm_svm_destroy(mobj)
	xmm_obj_t mobj;
{
	xmm_entry1(xmm_svm_destroy, mobj);

	assert(mobj->class == &msvm_class);

	/*
	 * If waiting for a kernel reply, don't do anything.
	 * The k_count logic should get us back in here when
	 * MOBJ->k_count goes to zero.
	 */
	if (MOBJ->k_count > 0) {
		MOBJ->state = MOBJ_STATE_SHOULD_TERMINATE;
		xmm_terminate_pending++;
	} else {
		MOBJ->state = MOBJ_STATE_TERMINATED;
		(void) M_TERMINATE(mobj, TRUE);
	}
}

void
k_svm_deallocate(kobj)
	xmm_obj_t kobj;
{
	xmm_entry1(k_svm_deallocate, kobj);

	/*
	 * Free kobj's resources.
	 */

	xmm_state_table_free(KOBJ->svm_state, KOBJ->num_pages, &C_user_state);
}

void
m_svm_deallocate(mobj)
	xmm_obj_t mobj;
{
	xmm_entry1(m_svm_deallocate, mobj);

	/*
	 * Free mobj's resources.
	 */

	xmm_state_table_free(MOBJ->svm_state, MOBJ->num_pages, &C_mobj_state);

	if (MOBJ->memory_object_name != IP_NULL)
		ipc_port_release_send(MOBJ->memory_object_name);

	xmm_svm_cleanup(mobj);
}


void
m_svm_request(mobj, r)
	xmm_obj_t mobj;
	request_t r;
{
	xmm_entry2(m_svm_request, mobj, r);

	assert(mobj->class == &msvm_class);

	if((unsigned long)atop(r->offset) >= MOBJ->num_pages) {
		m_svm_extend(mobj, atop(r->offset) + 1);
	}

	if (m_svm_add_request(mobj, r)) {
		m_svm_process_kernel_request(mobj, r);
	}
}

m_svm_add_change (mobj, object_ready,may_cache, copy_strategy,
		     use_routine, memory_object_name, reply)
	xmm_obj_t mobj;
	boolean_t object_ready;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	int use_routine;
	ipc_port_t memory_object_name;
	xmm_reply_t reply;
{
	mchange_t c, new;

	xmm_entry7(m_svm_add_change,
		mobj,
		object_ready,
		may_cache,
		copy_strategy,
		use_routine,
		memory_object_name,
		reply);

	new = (mchange_t) zalloc(xmm_svm_change_zone);

	new->k_count	= 0;
	new->next	= 0;
	new->object_ready = object_ready;
	new->may_cache	= may_cache;
	new->copy_strategy = copy_strategy;
	new->use_routine = use_routine;
	new->memory_object_name = memory_object_name;
	new->reply	= reply;

	if (MOBJ->change == CHANGE_NULL)
		MOBJ->change = new;
	else {
		for (c = MOBJ->change; c->next; c = c->next);
		c->next = new;
	}

	return KERN_SUCCESS;
}


unsigned int	m_svm_data_request_calls = 0;
m_svm_data_request(kobj, offset, length, desired_access)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	request_t r;

	xmm_entry4(m_svm_data_request, kobj, offset, length, desired_access);

#ifdef	lint
	M_DATA_REQUEST(kobj, offset, length, desired_access);
#endif	lint
	assert(kobj->class == &ksvm_class);
	++m_svm_data_request_calls;
	if (length != PAGE_SIZE) {
		K_DATA_ERROR(kobj, offset, length, KERN_FAILURE);
		return KERN_FAILURE;
	}
	r = (request_t) zalloc(xmm_svm_request_zone);
	r->who.kobj = kobj;
	r->is_kernel = TRUE;
	r->m_count = 0;
	r->k_count = 0;
	r->needs_data = TRUE;
	r->should_clean = FALSE;
	r->should_flush = FALSE;
	r->desired_access = desired_access;
	r->offset = offset;
	queue_init(&r->chain_neq);
	r->next_eq = REQUEST_NULL;
	r->sending = TRUE;

	assert(KOBJ->svm_mobj != XMM_OBJ_NULL);
	m_svm_request(KOBJ->svm_mobj, r);
	return (KERN_SUCCESS);
}

m_svm_data_unlock(kobj, offset, length, desired_access)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	request_t r;

	xmm_entry4(m_svm_data_unlock, kobj, offset, length, desired_access);

#ifdef	lint
	M_DATA_UNLOCK(kobj, offset, length, desired_access);
#endif	lint
	assert(kobj->class == &ksvm_class);
	if (length != PAGE_SIZE) {
		K_DATA_ERROR(kobj, offset, length, KERN_FAILURE);
		return KERN_FAILURE;
	}
	r = (request_t) zalloc(xmm_svm_request_zone);
	r->who.kobj = kobj;
	r->is_kernel = TRUE;
	r->m_count = 0;
	r->k_count = 0;
	r->needs_data = FALSE;
	r->should_clean = FALSE;
	r->should_flush = FALSE;
	r->desired_access = desired_access;
	r->offset = offset;
	queue_init(&r->chain_neq);
	r->next_eq = REQUEST_NULL;
	r->sending = TRUE;
	assert(KOBJ->svm_mobj != XMM_OBJ_NULL);
	m_svm_request(KOBJ->svm_mobj, r);
	return (KERN_SUCCESS);
}

int svm_initialize_discard = 0;

unsigned int c_m_svm_data_write_return_ml = 0; /* alan */
m_svm_data_write_return(kobj, offset, data, length,
			dirty, kernel_copy, use_routine)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
	xmm_obj_t mobj;
	request_t r;
	int c_loops = 0;		/* alan */

	xmm_entry7(m_svm_data_write_return,
		kobj, offset, data, length, dirty, kernel_copy, use_routine);

#ifdef	lint
	M_DATA_WRT_RTN(kobj, offset, data, length, dirty, kernel_copy,
			use_routine);
#endif	lint

	mobj = KOBJ->svm_mobj;

	if (use_routine == XMM_USE_DATA_INITIALIZE) {
		unsigned long  page;

		/*
		 *	If this page is anywhere in the XMM
		 *	system, discard this request.  This means
		 *
		 *	Pass through if beyond MOBJ->num_pages.  This
		 *		means that this page has never been
		 *		through here, and hence has not been
		 *		initialized.
		 *
		 *	Discard if MOBJ prot is not NONE; this means
		 *		that the page must already have been
		 *		intialized because someone might have it.
		 *
		 *	Pass request through if MOBJ prot is NONE and there
		 *		is no request out against this page with
		 *		the exception of a kernel request for data.
		 *
		 *	Else discard -- the outstanding request indicates
		 *		that we have the page somewhere, and hence
		 *		the page has been initialized.
		 */
		if ((page = atop(offset)) >= MOBJ->num_pages) {
			if (MOBJ->write_completions)
				m_svm_alloc_pending(mobj, kobj, offset);
			return M_DATA_WRT_RTN(mobj, offset, data, length,
				      dirty, kernel_copy, use_routine);
		}
		if (SVM_GET_PROT(MOBJ,page) == VM_PROT_NONE) {

		    r = m_svm_lookup_request(mobj, offset);
		    while (r != REQUEST_NULL &&
			r->is_kernel && r->needs_data) {

			    /*
			     * XXX With precious pages we probably have to
			     * XXX wait for kernel data requests to complete
			     * XXX because the pager could supply the page
			     * XXX precious and forget that it has the bits.
			     * XXX
			     * XXX TO BE IMPLEMENTED when this module is
			     * XXX upgraded to understand precious pages.
			     */
			    r = r->next_eq;
			    ++c_loops;	/* alan */
		    }
		    if (c_loops > c_m_svm_data_write_return_ml)
			    c_m_svm_data_write_return_ml = c_loops;

		    /*
		     *	At this point r either is a request that is not
		     *  the kernel asking for data (drop through and discard),
		     *  or there is no such request (r is NULL - pass through).
		     *  This is an unsolicited pageout, and should not be
		     *	buffered.
		     */
		    if (r == REQUEST_NULL) {
			if (MOBJ->write_completions)
				m_svm_alloc_pending(mobj, kobj, offset);
			return M_DATA_WRT_RTN(mobj, offset, data, length,
				      dirty, kernel_copy, use_routine);
		    }
		}

		svm_initialize_discard++;  /* XXX XXX XXX */
		/*
		 *	Discard request.  Page has already been initialized.
		 */
		vm_map_copy_discard((vm_map_copy_t) data);
		return KERN_SUCCESS;
	}		 


#if	USE_XMM_BUFFER
	assert(kobj->class == &ksvm_class);
	/* make sanity checks */
	r = m_svm_lookup_request(mobj, offset);
	if (r == REQUEST_NULL || ! r->is_kernel) {
		/*
		 * If there is no request, then this is an unsolicited
		 * pageout. We don't want to buffer this, since no one
		 * wants it.
		 *
		 * If this is not a kernel request, then it is a pager
		 * request, and thus the pager wants this page. We
		 * don't want to buffer the page in this case either.
		 */
		if (MOBJ->write_completions)
			m_svm_alloc_pending(mobj, kobj, offset);
		return M_DATA_WRT_RTN(mobj, offset, data, length,
				      dirty, kernel_copy, use_routine);
	} else {
		/*
		 * To avoid deadlock, pager requests have priority.
		 * Thus, if first request is a kernel, then all are.
		 * Therefore this pageout is wanted by kernels and
		 * not by the memory manager. This is case in which
		 * we want to buffer the page.
		 */
		 /*
		  * XXX If for some strange reason the buffer code is
		  * XXX ever enabled the write_completed logic will have
		  * XXX to be thought through.  I don't anticipate this
		  * XXX happening - sjs.
		  */
		return M_BUFFERED_DATA_WRITE(mobj, offset, data,
					     length, dirty, kernel_copy,
					     use_routine);
	}
#else	USE_XMM_BUFFER
	if (MOBJ->write_completions)
		m_svm_alloc_pending(mobj, kobj, offset);
	return M_DATA_WRT_RTN(mobj, offset, data, length,
			      dirty, kernel_copy, use_routine);
#endif	USE_XMM_BUFFER
}

/*
 * contact all of the kernels with the change request.
 */
k_svm_do_change_request(mobj)
	xmm_obj_t mobj;
{
	xmm_obj_t k;
	kern_return_t kr;
	xmm_reply_t reply;
	mchange_t c = MOBJ->change;

	xmm_entry1(k_svm_do_change_request, mobj);

	/*
	 * Increment k_count and decrement when the loop is
	 * exited to make sure we control when that happens; a
	 * nasty race condition exists with change_complete() if the
	 * kernels all complete and the count == 0 before we are done.
	 */
cr_top:
	c->k_count++;
	MOBJ->k_count++;
	for (k = MOBJ->kobj_list; k; k = K->next) {
		if (!(K->inited))
			continue;
		c->k_count++;
		MOBJ->k_count++;
		K->k_count++;
		/*
		 * XXX is the xmm_obj_reference() necessary?
		 */
		xmm_obj_reference(mobj);
		kr = xmm_reply_allocate(k, (ipc_port_t) mobj,
					XMM_SVM_REPLY, &reply);
		if (kr != KERN_SUCCESS) {
			panic("k_svm_do_change_request: xmm_reply_allocate: %d\n", kr);
		}
		ipc_port_copy_send(c->memory_object_name);
		K_SET_READY(k, c->object_ready, c->may_cache,
			    MOBJ->write_completions,MOBJ->copy_strategy, 
			    MOBJ->cluster_size, c->use_routine,
			    c->memory_object_name, reply);
	}
	/*
	 * See if everybody finished and we need to do clean up
	 */
	MOBJ->k_count--;
	if (--c->k_count == 0) {

		/*
		 *	Free this change and advance list.
		 */
		MOBJ->change = c->next;
		zfree(xmm_svm_change_zone, (vm_offset_t) c);

		/*
		 *	If we are supposed to kill the object, then
		 *	do so, making sure that the terminate precedes
		 *	the release.
		 */
		if ((MOBJ->k_count == 0) &&
		    (MOBJ->state == MOBJ_STATE_SHOULD_TERMINATE) &&
		    (MOBJ->change == (mchange_t) 0)) {

			xmm_terminate_pending--;
			M_TERMINATE(mobj, FALSE);
			M_CHANGE_COMPLETED(mobj, c->may_cache, 
					   c->copy_strategy, c->reply, TRUE);
		}
		else {
			M_CHANGE_COMPLETED(mobj, c->may_cache, 
					   c->copy_strategy, c->reply, FALSE);
		}

		/*
		 *	On to the next change request if there is one.
		 */
		if ((c = MOBJ->change)) {
			goto cr_top;
		}
	}
}	

#define	BIGHACK

m_svm_do_lock_request(k, should_clean, should_flush, lock_value, r, mobj)
	xmm_obj_t k;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	request_t r;
	xmm_obj_t mobj;

{
	kern_return_t kr;
	xmm_reply_t reply;

	xmm_entry6(m_svm_do_lock_request,
		k, should_clean, should_flush, lock_value, r, mobj);

#ifdef	BIGHACK
	if (!r->sending) {
		gather_lock_request(k, r->offset, should_clean,should_flush,
				    lock_value, mobj);
		return;
	}
#endif
	xmm_obj_reference(mobj);
	kr = xmm_reply_allocate(k, (ipc_port_t) mobj, XMM_SVM_REPLY, &reply);
	if (kr != KERN_SUCCESS) {
		panic("m_svm_do_lock_request: xmm_reply_allocate: %d\n", kr);
	}
	K_LOCK_REQUEST(k, r->offset, PAGE_SIZE, should_clean, should_flush,
		       lock_value, reply);
}	

gather_lock_request(kobj, offset, should_clean, should_flush, lock_value, mobj)
	xmm_obj_t kobj;
	vm_offset_t offset;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_obj_t mobj;
{
	struct gather *g;

	xmm_entry6(gather_lock_request,
		kobj, offset, should_clean, should_flush, lock_value, mobj);

	g = KOBJ->gather;
	if (g == GATHER_NULL) {
		xmm_obj_reference(mobj);
		g = (struct gather *) zalloc(xmm_svm_gather_zone);
		g->kobj = kobj;
		g->begin_offset = offset;
		g->offset = offset;
		g->should_clean = should_clean;
		g->should_flush = should_flush;
		g->lock_value = lock_value;
		KOBJ->gather = g;
	} else if (offset == g->offset + PAGE_SIZE) {
			if (should_clean != g->should_clean ||
			    should_flush != g->should_flush ||
			    lock_value != g->lock_value) {
			  	/*
				 * This may happen, but we will see if
				 * it really does.  If so, we should
				 * send out the current gather list
				 * and start a new one.  This would
				 * happen if an object was being shared
				 * and pages were spread across nodes -
				 * perhaps causing certain pages to be
				 * written back and others to be
				 * invalidated.
				 */
				printf("Changing clean parameters!\n");
				assert(0);
			}
			g->offset = offset;
	} else {
		/*
		 *	We have a skip in the range; this means
		 *	we don't want to do a lock_request on all
		 *	pages in the range to this kernel.  Send
		 *	what we have and start a new gather list.
		 */
		xmm_obj_reference(mobj);
		gather_send(kobj);
		g = (struct gather *) zalloc(xmm_svm_gather_zone);
		g->kobj = kobj;
		g->begin_offset = offset;
		g->offset = offset;
		g->should_clean = should_clean;
		g->should_flush = should_flush;
		g->lock_value = lock_value;
		KOBJ->gather = g;
	}
}

gather_send(kobj)
	xmm_obj_t kobj;
{
	struct gather *g;
	xmm_reply_t reply;
	kern_return_t kr;

	xmm_entry1(gather_send, kobj);

	g = KOBJ->gather;
	assert(g);
	kr = xmm_reply_allocate(kobj, (ipc_port_t) KOBJ->svm_mobj,
			        XMM_SVM_REPLY, &reply);
	if (kr != KERN_SUCCESS) {
		panic("gather_send: xmm_reply_allocate: %d\n", kr);
	}
	(void) K_LOCK_REQUEST(kobj,
			      g->begin_offset,
			      g->offset - g->begin_offset + PAGE_SIZE,
			      g->should_clean,
			      g->should_flush,
			      g->lock_value,
			      reply);
	zfree(xmm_svm_gather_zone, (vm_offset_t) g);
	KOBJ->gather = GATHER_NULL;
}


k_svm_push_gather(mobj)
	xmm_obj_t mobj;
{
	xmm_obj_t kobj;

	xmm_entry1(k_svm_push_gather, mobj);

	for (kobj = MOBJ->kobj_list; kobj; kobj = KOBJ->next) {
		if (KOBJ->gather)
			gather_send(kobj);
	}
}

m_svm_change_completed(kobj, may_cache, copy_strategy, reply, release)
	xmm_obj_t kobj;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	xmm_reply_t reply;
	boolean_t release;
{
	mchange_t c;
	xmm_obj_t mobj;

	xmm_entry5(m_svm_change_completed,
		kobj, may_cache, copy_strategy, reply, release);

#ifdef	lint
	M_CHANGE_COMPLETED(kobj, may_cache, copy_strategy, reply, release);
#endif	lint
	/* XXX should make sanity checks */
	assert(reply->reply_to_type == XMM_SVM_REPLY);
	mobj = KOBJ->svm_mobj;
	xmm_reply_deallocate(reply);
	assert(mobj->class == &msvm_class);
	c = MOBJ->change;
	if (c == CHANGE_NULL) {
		panic("m_svm_change_completed: missing request");
	}

	KOBJ->k_count--; /* for this reply */
	if (KOBJ->terminated && (KOBJ->k_count == 0)) {
      		xmm_ksvm_terminate(kobj);
	}

	MOBJ->k_count--;
	if (--c->k_count == 0) {

		/*
		 *	Free this change and advance list.
		 */
		MOBJ->change = c->next;
		zfree(xmm_svm_change_zone, (vm_offset_t) c);

		/*
		 *	If we are supposed to kill the object, then
		 *	do so, making sure that the terminate precedes
		 *	the release.  This relies on the xmm_object_release
		 *	above tripping the no senders logic synchronously
		 * XXX	which is probably not the case.
		 */
		if ((MOBJ->k_count == 0) &&
		    (MOBJ->state == MOBJ_STATE_SHOULD_TERMINATE) &&
		    (MOBJ->change == (mchange_t) 0)) {

			xmm_terminate_pending--;
			M_TERMINATE(mobj, FALSE);
			M_CHANGE_COMPLETED(mobj, may_cache, c->copy_strategy,
				   c->reply, TRUE);
		}
		else {
			M_CHANGE_COMPLETED(mobj, may_cache, c->copy_strategy,
					   c->reply, FALSE);
		}
 
		/*
		 *	On to the next change request if there is one.
		 */
		if (MOBJ->change) {
			k_svm_do_change_request (mobj);
		}
	}

	xmm_obj_release(mobj);	/* reference obtained by do_change_request */
	return KERN_SUCCESS;
}

m_svm_lock_completed(kobj, offset, length, reply, release)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_reply_t reply;
	boolean_t release;
{
	xmm_obj_t mobj;

	xmm_entry5(m_svm_lock_completed,
		kobj, offset, length, reply, release);

	mobj = KOBJ->svm_mobj;
	while(length > PAGE_SIZE) {
		m_svm_do_lock_completed(kobj, offset, PAGE_SIZE, reply, FALSE);
		length -= PAGE_SIZE;
		offset += PAGE_SIZE;
	}
	m_svm_do_lock_completed(kobj,offset,PAGE_SIZE,reply,release);
	xmm_reply_deallocate(reply);
	xmm_obj_release(mobj);	/* reference obtained by do_lock_request */
}


m_svm_do_lock_completed(kobj, offset, length, reply, release)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_reply_t reply;
	boolean_t release;
{
	request_t r;
	xmm_obj_t mobj;

	xmm_entry5(m_svm_do_lock_completed,
		kobj, offset, length, reply, release);

#ifdef	lint
	M_LOCK_COMPLETED(kobj, offset, length, reply, release);
#endif	lint
	/* XXX should make sanity checks */
	/* XXX should store r in reply */
	assert(reply->reply_to_type == XMM_SVM_REPLY);
	mobj = KOBJ->svm_mobj;
#if 0
	xmm_reply_deallocate(reply);
#endif
	assert(mobj->class == &msvm_class);
	r = m_svm_lookup_request(mobj, offset);
	if (r == REQUEST_NULL) {
		panic("m_svm_lock_completed: missing request");
	}

	KOBJ->k_count--; /* for this reply */
	if (KOBJ->terminated && (KOBJ->k_count == 0)) {
      		xmm_ksvm_terminate(kobj);
	}

	/*
	 *	This isn't as bad as the change completed logic above
	 *	because we don't have to order the lock_completed and
	 *	terminate operations.
	 */
	if (--r->k_count == 0 && r->m_count == 0) {
		m_svm_satisfy_request(mobj, r, DATA_NONE, 0);
		if (--MOBJ->k_count == 0 &&
		    MOBJ->state == MOBJ_STATE_SHOULD_TERMINATE) {
		        xmm_terminate_pending--;
			xmm_svm_destroy(mobj);
		}
	}
	else {
		MOBJ->k_count--;
	}
#if 0
	xmm_obj_release(mobj);	/* reference obtained by do_lock_request */
#endif
	return KERN_SUCCESS;
}

k_svm_data_supply(mobj, offset, data, length, lock_value, precious, reply)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	vm_prot_t lock_value;
	boolean_t precious;
	xmm_reply_t reply;
{
	request_t r;

	xmm_entry7(k_svm_data_supply,
		mobj, offset, data, length, lock_value, precious, reply);

	assert (!(lock_value & VM_PROT_NO_CHANGE));

#ifdef	lint
K_DATA_SUPPLY(mobj, offset, data, length, lock_value, precious, reply);
#endif	lint
	assert(mobj->class == &msvm_class);
	/* make sanity checks */

	if (precious) {
		panic("k_svm_data_supply: precious");
	}
	if (reply != XMM_REPLY_NULL) {
		panic("k_svm_data_supply: reply");
	}

	r = m_svm_lookup_request(mobj, offset);
	if (r == REQUEST_NULL) {
		printf("how strange, data_supply for nothing!\n");
		return KERN_FAILURE;
	}
	r->m_count--;
	assert(r->m_count == 0 && r->k_count == 0);

	/*
	 *	Do something about the supplied lock value.
	 *
	 *	Locks against read result in an immediate retry
	 *	the data is being supplied no access.
	 */
	if (lock_value & VM_PROT_READ) {
		m_svm_remove_request(mobj, offset);
		m_svm_request(mobj, r);
	}
	else {
		/*
		 *	Locks against write are OK; we should have no
		 *	writers at this point.  If we have a writer,
		 * 	then this is a manager error.  Also, allow
		 *	execute, since read is allowed.
		 */
		if (lock_value &
		    SVM_GET_PROT(MOBJ,atop(offset)) & VM_PROT_WRITE) {
			dprintf("data_supply FAILS, lock 0x%x, value 0x%x\n",
				SVM_GET_LOCK(MOBJ,atop(offset)),
				lock_value);
			return(KERN_FAILURE);
		}
		lock_value &= ~VM_PROT_EXECUTE;
		SVM_SET_LOCK(MOBJ,atop(offset),lock_value);
		assert(r->is_kernel);
		m_svm_satisfy_kernel_request(mobj, r, data, 0);
	}		

	return KERN_SUCCESS;
}

k_svm_data_write_completed(mobj, offset, length)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
{
	xmm_obj_t kobj;

	xmm_entry3(k_svm_data_write_completed, mobj, offset, length);

	assert(mobj->class == &msvm_class);

	kobj = m_svm_lookup_pending(mobj, offset);
#if	!TERMINATE_PENDING
	assert(kobj != XMM_OBJ_NULL);
#endif
	if (kobj == XMM_OBJ_NULL) {
#if	TERMINATE_PENDING
		/*
		 *	In a terminate race, data_write_completed
		 *	can lose.  Give the caller credit for a
		 *	nice try, though.
		 */
		return KERN_SUCCESS;
#else
		printf("svm_data_write_completed error:  No data_write!\n");
		return KERN_FAILURE;
#endif
	}

	return K_DATA_WRITE_COMPLETED(kobj, offset, length);
}

k_svm_data_unavailable(mobj, offset, length)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
{
	request_t r;

	xmm_entry3(k_svm_data_unavailable, mobj, offset, length);

#ifdef	lint
	K_DATA_UNAVAILABLE(mobj, offset, length);
#endif	lint
	assert(mobj->class == &msvm_class);
	/* make sanity checks */

	/* XXX is this absolutely correct? */
	SVM_SET_LOCK(MOBJ,atop(offset),VM_PROT_NONE);

	r = m_svm_lookup_request(mobj, offset);
	if (r == REQUEST_NULL) {
		printf("how strange, data_unavailable for nothing!\n");
                assert(0);
		return KERN_FAILURE;
	}
	if (--r->m_count == 0 && r->k_count == 0) {
		m_svm_satisfy_request(mobj, r, DATA_UNAVAILABLE, 0);
	}
	return KERN_SUCCESS;
}


unsigned int	c_k_svm_lock_request_is_kernel = 0; /* debug alan XXX */
k_svm_lock_request(mobj, offset, length, should_clean, should_flush,
		   lock_value, reply)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
	request_t	r, r0;
	boolean_t	result;

	xmm_entry7(k_svm_lock_request,
		mobj,
		offset,
		length,
		should_clean,
		should_flush,
		lock_value,
		reply);

#ifdef	lint
	K_LOCK_REQUEST(mobj, offset, length, should_clean, should_flush,
		       lock_value, reply);
#endif	lint
	assert(mobj->class == &msvm_class);
	dprintf("k_svm_lock_request!\n");

	/*
	 *	Treat read and execute the same for lock value.
	 */
	if (lock_value & VM_PROT_READ) {
		lock_value |= VM_PROT_EXECUTE;
	}
	else {
		lock_value &= ~VM_PROT_EXECUTE;
	}		

	if (length != PAGE_SIZE) {
		if (length > PAGE_SIZE) {
			panic("k_svm_lock_request: %d > PAGE_SIZE\n", length);
		}
		length = PAGE_SIZE;
	}
	if((unsigned long)atop(offset) >= MOBJ->num_pages) {
		m_svm_extend(mobj, atop(offset) + 1);
	}

	r0 = m_svm_lookup_request(mobj, offset);

	/*
	 * If we are not increasing lock value, flushing, or cleaning,
	 * then we set simply set lock value, without creating a request.
	 * However, we do need to see whether we can satisfy a kernel request.
	 */
	if ( !should_clean && !should_flush) {
		if (lock_value == VM_PROT_NO_CHANGE) {

			/*
			 *	No change from manager, hence no
			 *	possibility of satisfying a kernel request.
			 */
		  	return KERN_SUCCESS;
		}

		if (! (lock_value & ~SVM_GET_LOCK(MOBJ,atop(offset)))) {
 
			/*
			 *	Lock value is not increasing.  Set
			 *	new value and check to see if this
			 *	makes the kernel happy.
			 */
			SVM_SET_LOCK(MOBJ,atop(offset),lock_value);
			if (r0 && r0->is_kernel
			    && !(lock_value & r0->desired_access)
			    && r0->m_count > 0 && --r0->m_count == 0
			    && r0->k_count == 0) {
				m_svm_satisfy_kernel_request(mobj, r0,
							     DATA_NONE, 0);
			}
			return KERN_SUCCESS;
		}
	}

	/*
	 * We need to submit a request. Create the request.
	 */
	dprintf("** lock_request: submitting request\n");
	r = (request_t) zalloc(xmm_svm_request_zone);
	r->who.reply = reply;
	r->is_kernel = FALSE;
	r->m_count = 0;
	r->k_count = 0;
	r->needs_data = FALSE;
	r->should_clean = should_clean;
	r->should_flush = should_flush;
	r->lock_value = lock_value;
	r->desired_access = VM_PROT_NO_CHANGE;	/* filler */
	r->offset = offset;
	queue_init(&r->chain_neq);
	r->next_eq = REQUEST_NULL;
	r->sending = FALSE;

	/*
	 * If there are no requests, then add new request and process it.
	 */
	if (! r0) {
		dprintf("- no reqs\n");
		result = m_svm_add_request(mobj, r);
		assert(result == TRUE);
		(void) m_svm_process_pager_request(mobj, r);
		return KERN_SUCCESS;
	}

	/*
	 * If first request is pager request, then place new request
	 * after all pager requests, but before any kernel requests.
	 */
	if (! r0->is_kernel) {
		dprintf("- only pager reqs\n");
		while (r0->next_eq && ! r0->next_eq->is_kernel) {
			r0 = r0->next_eq;
		}
		r->next_eq = r0->next_eq;
		r0->next_eq = r;
		/*
		 *	We do not need to update the lookup hint.
		 *	Even if the hint points to r0, the new
		 *	record is being inserted AFTER r0.
		 */
		return KERN_SUCCESS;
	}

	/*
	 * First request is a kernel request.
	 * To avoid deadlock, pager requests have priority.
	 * Thus, if first request is a kernel, then all are.
	 * In this case, we place new request at the top
	 * (before all kernel requests) and process it immediately.
	 *
	 * XXXO
	 * This is slightly pessimal because we just ignore any
	 * request that the kernel request made to the other kernels.
	 */
	if (r0->is_kernel) {
		++c_k_svm_lock_request_is_kernel; /* debug alan XXX */

		/*
		 *	Make r take r0's place in the request chain.
		 */
		assert(r->offset == r0->offset);
		r->chain_neq.next = r0->chain_neq.next;
		r->chain_neq.prev = r0->chain_neq.prev;
		r0->chain_neq.next->prev = (struct queue_entry *) r;
		r0->chain_neq.prev->next = (struct queue_entry *) r;
		/*
		 *	r0 is no longer on the request chain.
		 */
		queue_init(&r0->chain_neq);


		/*
		 *	Hang r0 (and the rest of the next_eq list)
		 *	onto r.
		 */
		r->next_eq = r0;

		/*
		 *	Update lookup hint.  If the lookup hint
		 *	matches the current record, we need to make
		 *	it point to the new record.
		 */
		if (MOBJ->last_found == r0)
			MOBJ->last_found = r;

		/*
		 * XXX	There has to be a better way to do this.
		 * XXX	Move the pending counts to the first request
		 * XXX	in the queue.  If our new request can't be
		 * XXX	satisfied immediately, we wait for everything.
		 */
		r->m_count = r0->m_count;
		r->k_count = r0->k_count;
		r0->m_count = 0;
		r0->k_count = 0;
		(void) m_svm_process_pager_request(mobj, r);
		return KERN_SUCCESS;
	}
	panic("k_svm_lock_request");
}


k_svm_data_error(mobj, offset, length, error_value)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t error_value;
{
	request_t r;

	xmm_entry4(k_svm_data_error, mobj, offset, length, error_value);

#ifdef	lint
	K_DATA_ERROR(mobj, offset, length, error_value);
#endif	lint
	assert(mobj->class == &msvm_class);
	/* make sanity checks */

	/* XXX certainly questionable! */
	SVM_SET_LOCK(MOBJ,atop(offset),VM_PROT_NONE);

	r = m_svm_lookup_request(mobj, offset);
	if (r == REQUEST_NULL) {
		printf("how strange, data_error for nothing!\n");
		return KERN_FAILURE;
	}
	if (--r->m_count == 0 && r->k_count == 0) {
		m_svm_satisfy_request(mobj, r, DATA_ERROR, error_value);
	}
	/* XXX should keep and return error_value */
	return KERN_SUCCESS;
}

k_svm_set_ready(mobj, object_ready, may_cache, write_completions, 
		copy_strategy, cluster_size, use_routine,
		memory_object_name, reply)
	xmm_obj_t mobj;
	boolean_t object_ready;
	boolean_t may_cache;
        boolean_t write_completions;
	memory_object_copy_strategy_t copy_strategy;
        vm_size_t cluster_size;
	int use_routine;
	ipc_port_t memory_object_name;
	xmm_reply_t reply;
{
	xmm_obj_t kobj;

	/*xmm_entry9(...);*/
	xmm_entry7(k_svm_set_ready,
		mobj, object_ready, may_cache, write_completions,
		copy_strategy, cluster_size, use_routine);

#ifdef	lint
	K_SET_READY(mobj, object_ready, may_cache, write_completions,
		    copy_strategy, cluster_size,
		    use_routine, memory_object_name, reply);
#endif	lint
	assert(mobj->class == &msvm_class);
	MOBJ->may_cache = may_cache;
	MOBJ->write_completions = write_completions;
	MOBJ->cluster_size = cluster_size;

	/*
	 * Compute our copy strategy based on that of underlying pager.
	 *
	 * XXX
	 * Right now, we always use COPY_NONE, except if underlying pager
	 * specifies COPY_TEMPORARY, in which case we use COPY_DELAY.
	 * What this means is that we don't have any intelligent way
	 * of dealing with sharing, but that if it's a temporary object
	 * (either a vm internal object, created via memory_object_create,
	 * or an xmm internal object, created via norma_copy_create),
	 * then we don't expect any sharing, so we can use a lazy copy.
	 *
	 * THIS WILL BREAK IN ITS CURRENT FORM WHEN WE ENABLE VM_INHERIT_SHARE
	 */
	if (MOBJ->kobj_count <= 1 &&
	    copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
		MOBJ->copy_strategy = MEMORY_OBJECT_COPY_TEMPORARY;
	} else {
		MOBJ->copy_strategy = MEMORY_OBJECT_COPY_NONE;
		copy_strategy = MEMORY_OBJECT_COPY_NONE;  /* XXX loser */
	}

	if (MOBJ->memory_object_name == IP_NULL) {
		MOBJ->memory_object_name = memory_object_name;
	} else {
		assert(MOBJ->memory_object_name == memory_object_name);
		ipc_port_release_send(memory_object_name);
	}
	/*
	 * Given a reply message we need to contact all kernels
	 * with the change.  Only one outstanding change can be in
	 * progress at a time, so queue up subsequent requests.
	 */
	if (reply != XMM_REPLY_NULL) {
		boolean_t new_change;

		new_change = MOBJ->change ? FALSE : TRUE;
		m_svm_add_change (mobj, object_ready, may_cache, copy_strategy,
				  use_routine, memory_object_name, reply);
		if (new_change) {
			k_svm_do_change_request (mobj);
		}
		return (KERN_SUCCESS);
	}

	/*
	 * XXX	Setting an object not ready causes undefined behavior
	 * XXX	We undefine it to cause the entire operation to be ignored.
	 */
	if (!object_ready) {
		return (KERN_SUCCESS);
	}

	MOBJ->state = MOBJ_STATE_READY;
	MOBJ->use_routine = use_routine;
	
	/*
	 * If there are multiple kernels, then we had better be
	 * using MEMORY_OBJECT_COPY_NONE, at least until we get
	 * trickier about changing copy strategies.
	 */
	if (MOBJ->kobj_count > 1 &&
	    MOBJ->copy_strategy != MEMORY_OBJECT_COPY_NONE) {
		panic("losing big on multiple copies of temporary object");
	}

	/*
	 * Let all kernels know what's going on.
	 */
	for (kobj = MOBJ->kobj_list; kobj; kobj = KOBJ->next) {
		if (!(KOBJ->inited))
			continue;
		assert(MOBJ->memory_object_name != IP_NULL);
		ipc_port_copy_send(MOBJ->memory_object_name);
		K_SET_READY(kobj, object_ready, may_cache,
			    write_completions, MOBJ->copy_strategy, 
			    cluster_size, use_routine,
			    memory_object_name, XMM_REPLY_NULL);
	}
	return KERN_SUCCESS;
}

k_svm_destroy(mobj, reason)
	xmm_obj_t mobj;
	kern_return_t reason;
{
	xmm_entry2(k_svm_destroy, mobj, reason);

#ifdef	lint
	K_DESTROY(mobj, reason);
#endif	lint
	assert(mobj->class == &msvm_class);
	printf("k_svm_destroy: Gack!\n");
}


/*
 * Place request at end of appropriate queue.
 * Return TRUE if first request in queue for this page.
 */
unsigned int	c_max_request_chain = 0; /* alan */
unsigned int	c_max_ar_outer_loop = 0; /* alan */
unsigned int	c_max_ar_inner_loop = 0; /* alan */
unsigned int	c_m_svm_add_request_calls = 0;
#define	update_ar_counters					\
	if (MOBJ->request_count > c_max_request_chain)		\
		c_max_request_chain = MOBJ->request_count;	\
	if (c_inner > c_max_ar_inner_loop)			\
		c_max_ar_inner_loop = c_inner;			\
	if (c_outer > c_max_ar_outer_loop)			\
		c_max_ar_outer_loop = c_outer;
boolean_t
m_svm_add_request(mobj, r0)
	xmm_obj_t mobj;
	request_t r0;
{
	request_t	r;
	unsigned int	c_outer = 0;	/* alan */
	unsigned int	c_inner = 0;	/* alan */

	xmm_entry2(m_svm_add_request, mobj, r0);

	assert(mobj->class == &msvm_class);
	assert(r0->next_eq == REQUEST_NULL);
	dprintf("m_svm_add_request(0x%x, 0x%x)", mobj, r0);
	++c_m_svm_add_request_calls;
	for (r = (request_t) queue_first(&MOBJ->requests);
	     !queue_end(&MOBJ->requests, (queue_head_t *) r);
	     r = (request_t) queue_next(&r->chain_neq)) {
		++c_outer;		/* alan */
		if (r->offset < r0->offset)
			break;
		if (r->offset == r0->offset) {
			for (; r->next_eq; r = r->next_eq) {
				++c_inner; /* alan */
				continue;
			}
			r->next_eq = r0;
			track(MOBJ, r0, "add equal");
			assert(queue_empty(&r0->chain_neq));
			assert(r0->next_eq == REQUEST_NULL);
			update_ar_counters; /* alan */
			return FALSE;
		}
	}
	track(MOBJ, r0, "add");
	enqueue_tail(r, r0);
	MOBJ->request_count++;
	update_ar_counters;		/* alan */
	return TRUE;
}


/*
 * Look for first request for given offset.
 * If we find such a request, move it to front of list
 * since we expect to remove it soon.
 */
unsigned int	c_m_svm_lookup_request = 0;
unsigned int	c_m_svm_lookup_request_backward = 0;
unsigned int	c_m_svm_lookup_request_hit = 0;
unsigned int	c_m_svm_lookup_fast_small = 0;
unsigned int	c_max_lr_outer_loop = 0; /* alan */
#define	update_lr_counters					\
	if (c_outer > c_max_lr_outer_loop)			\
		c_max_lr_outer_loop = c_outer;
request_t
m_svm_lookup_request(mobj, offset)
	xmm_obj_t mobj;
	vm_offset_t offset;
{
	request_t	r, last, q_end;
	vm_offset_t	smallest_offset;
	boolean_t	forward;
	int		delta_smallest;
	unsigned int	c_outer;	/* alan */

	xmm_entry2(m_svm_lookup_request, mobj, offset);

	assert(mobj->class == &msvm_class);
	++c_m_svm_lookup_request;

	/*
	 *	Fast hit on previous lookup.  Optimizes	lookup/remove case.
	 */
	if ((r = MOBJ->last_found) != REQUEST_NULL && r->offset == offset) {
		++c_m_svm_lookup_request_hit;
		MOBJ->last_found = REQUEST_NULL;
		track(MOBJ, r, "lookup hit");
		return r;
	}

	/*
	 *	Decide in which direction to search along
	 *	the queue.  The queue is ordered with the
	 *	largest offset first, i.e., queue_first on
	 *	the header returns the request with the
	 *	largest offset.  Default search is largest
	 *	towards smallest.  Switch direction if
	 *	the desired offset is within a few pages of
	 *	the smallest offset.
	 */
	forward = TRUE;
	if (MOBJ->request_count > 4) {
		last = (request_t) queue_last(&MOBJ->requests);
		if ((smallest_offset = last->offset) >= offset) {
			++c_m_svm_lookup_fast_small;
			if (smallest_offset == offset) {
				MOBJ->last_found = last;
				track(MOBJ, r, "lookup smallest hit");
				return last;
			}
			return REQUEST_NULL;
		}
		delta_smallest = (int) (offset - smallest_offset);
		assert(delta_smallest > 0);
		if (delta_smallest < 3 * PAGE_SIZE) {
			forward = FALSE;
			++c_m_svm_lookup_request_backward;
		}
	}

	/*
	 *	Traverse the queue.  When travelling forwards,
	 *	finding a request with an offset less than the
	 *	one sought implies the request queue doesn't
	 *	contain the offset.  The opposite condition
	 *	applies when travelling backwards.
	 */
	r = (forward == TRUE ?
	     (request_t) queue_first(&MOBJ->requests) :
	     (request_t) queue_last(&MOBJ->requests));
	c_outer = 0;			/* alan */
	while (!queue_end(&MOBJ->requests, (queue_head_t *) r)) {
		++c_outer;	/* alan */
		if ((forward == TRUE && r->offset < offset) ||
		    (forward == FALSE && r->offset > offset))
			break;
		if (r->offset == offset) {
			MOBJ->last_found = r;
			update_lr_counters; /* alan */
			track(MOBJ, r, "lookup found");
			return r;
		}
		r = (forward == TRUE ?
		     (request_t) queue_next(&r->chain_neq) :
		     (request_t) queue_prev(&r->chain_neq));
	}
	update_lr_counters;	/* alan */
	track(MOBJ, REQUEST_NULL, "lookup miss");
	return REQUEST_NULL;
}


/*
 * Look for the first pending record for a given offset.
 * Once the pending record is found, remove it from the list
 * and deallocate the record; return the kobj.
 */
unsigned int	c_max_lp_outer_loop = 0;	/* alan */
#define update_lp_counters				\
	if (c_outer > c_max_lp_outer_loop)		\
		c_max_lp_outer_loop = c_outer;
xmm_obj_t
m_svm_lookup_pending(mobj, offset)
	xmm_obj_t mobj;
	vm_offset_t offset;
{
	pending_t p, *pp;
	xmm_obj_t kobj;
	unsigned int c_outer = 0;	/* alan */

	xmm_entry2(m_svm_lookup_pending, mobj, offset);

	assert(mobj->class == &msvm_class);
	for (pp = &MOBJ->pending; p = *pp; pp = &p->next) {
		++c_outer;		/* alan */
		if (p->offset == offset) {
			*pp = p->next;
			kobj = p->kobj;
			zfree(xmm_svm_pending_zone, (vm_offset_t) p);
#if	PENDING_TRACKING
			MOBJ->p_matched++;
#endif
			update_lp_counters; /* alan */
			return kobj;
		}
	}
	update_lp_counters;		/* alan */
	return XMM_OBJ_NULL;
}


/*
 * Remove first request for given offset.
 * Return next request for same offset, if any.
 * Caller is obliged to free the storage
 * associated with the request being deleted.
 */
unsigned int	c_m_svm_remove_request_calls = 0;
request_t
m_svm_remove_request(mobj, offset)
	xmm_obj_t mobj;
	vm_offset_t offset;
{
	request_t	r, r0;

	xmm_entry2(m_svm_remove_request, mobj, offset);

	assert(mobj->class == &msvm_class);
	assert(MOBJ->request_count > 0);
	++c_m_svm_remove_request_calls;

	/*
	 *	This lookup should be very fast as we expect
	 *	that a lookup was done before calling this
	 *	routine, and that the lookup operation caches
	 *	a hint.
	 */
	r = m_svm_lookup_request(mobj, offset);
	if (r == REQUEST_NULL) {
		printf("m_svm_remove_request: request not found!\n");
		return REQUEST_NULL;
	}

	if (MOBJ->last_found == r)
		MOBJ->last_found = REQUEST_NULL;

	/*
	 *	If more than one request exists for the specified
	 *	offset, replace the first request for the offset
	 *	with the next one in line.
	 */
	if ((r0 = r->next_eq) != REQUEST_NULL) {
		track(MOBJ, r, "remove equal 1");
		track(MOBJ, r0, "remove equal 2");
		assert(queue_empty(&r0->chain_neq));
		r0->chain_neq.next = r->chain_neq.next;
		r0->chain_neq.prev = r->chain_neq.prev;
		r->chain_neq.next->prev = (struct queue_entry *) r0;
		r->chain_neq.prev->next = (struct queue_entry *) r0;
		r->next_eq = REQUEST_NULL;
		assert(MOBJ->request_count > 0);
		return r0;
	}

	/*
	 *	Only one request at this offset.  Delete it.
	 */
	track(MOBJ, r, "remove");
	MOBJ->request_count--;
	(void) remque(r);
	assert(MOBJ->request_count >= 0);
	return REQUEST_NULL;
}


/*
 * All the real work takes place in m_svm_process_request and
 * m_svm_satisfy_request.
 *
 * m_svm_process_request takes a request for a page that does not already have
 * outstanding requests and generates the appropriate K/M_ requests.
 * If, after generating all apropriate K/M_ requests, there are no outstanding
 * K/M_ requests (either because no K/M_ requests were required, or because
 * they were all satisfied by the time we check), we call
 * m_svm_satisfy_request.
 *
 * m_svm_satisfy_request takes a request for a page that has had its last
 * outstanding K/M_ request satisfied, and sends the appropriate K/M_ reply
 * to the entity (kernel or memory manager) that generated the request. If more
 * requests follow the request being satisfied, m_svm_satisfy_request calls
 * m_svm_process_request on the first such request.
 */

/*
 * This routine does not worry about lock[page]; m_svm_satisfy_request does.
 */
void
m_svm_process_kernel_request(mobj, r)
	xmm_obj_t mobj;
	request_t r;
{
	int page;
	xmm_obj_t kobj, k;

	xmm_entry2(m_svm_process_kernel_request, mobj, r);

	assert(mobj->class == &msvm_class);
	page = atop(r->offset);
	kobj = r->who.kobj;

	/*
	 * If requesting kernel wants to write, we must flush and lock
	 * all kernels (either readers or a single writer).
	 */
	if (r->desired_access & VM_PROT_WRITE) {
		boolean_t writing = !! (SVM_GET_PROT(MOBJ,page) & VM_PROT_WRITE);
		SVM_SET_PROT(MOBJ,page,VM_PROT_NONE);
		/*
		 * Increment k_count and decrement when the loop is
		 * exited to make sure we control when that happens;
		 * a nasty race condition exists if the kernels all
		 * complete and the count == 0 before we are done.
		 */
		r->k_count++;
		MOBJ->k_count++;
		for (k = MOBJ->kobj_list; k; k = K->next) {
			if (k == kobj ||
			    SVM_GET_PROT(K,page) == VM_PROT_NONE) {
				continue;
			}
			assert(K->inited);
			r->k_count++;
			MOBJ->k_count++;
			K->k_count++;
			SVM_SET_STATE(K,page,VM_PROT_NONE,0);
			m_svm_do_lock_request(k, writing, TRUE, VM_PROT_ALL,
					      r, mobj);
			if (writing) {
				break;
			}
		}
		if (--r->k_count == 0 && r->m_count == 0) {
			m_svm_satisfy_kernel_request(mobj, r, DATA_NONE, 0);
			if (--MOBJ->k_count == 0 &&
			    MOBJ->state == MOBJ_STATE_SHOULD_TERMINATE) {
			        xmm_terminate_pending--;
				xmm_svm_destroy(mobj);
			}
		}
		else {
			MOBJ->k_count--;
		}
		return;
	}

	/*
	 * If requesting kernel wants to read, but the page is being written,
	 * then we must clean and lock the writer.
	 */
	if (r->desired_access &&
	    (SVM_GET_PROT(MOBJ,page) & VM_PROT_WRITE)) {
		if (SVM_GET_PROT(KOBJ,page) & VM_PROT_WRITE) {
			/*
			 * What could the writer be doing asking us for read?
			 *
			 * This can happen if page was cleaned and flushed,
			 * or (more commonly?) cleaned and then paged out.
			 *
			 * Should we give this kernel read (more concurrency)
			 * or write (on the assumption that he will want
			 * to write again)?
			 *
			 * For now, we just give him read.
			 * We have to correct our notion of how this page is
			 * used. Note that there is no problem giving
			 * him either read or write, since there is nobody
			 * else to evict.
			 */
			SVM_SET_PROT(MOBJ,page,r->desired_access);
			SVM_SET_STATE(KOBJ,page,r->desired_access,0);
			m_svm_satisfy_kernel_request(mobj, r, DATA_NONE, 0);
			return;
		}
		for (k = MOBJ->kobj_list; k; k = K->next) {
			if (SVM_GET_PROT(K,page) & VM_PROT_WRITE) {
				break;
			}
		}
		if (k == XMM_OBJ_NULL) {
			/*
			 * This case occurs if a kobj holding the write
			 * token has been terminated.  Pretend the
			 * MOBJ did not have a write token.
			 */
			SVM_SET_PROT(MOBJ,page,r->desired_access);
			SVM_SET_STATE(KOBJ,page,r->desired_access,0);
			m_svm_satisfy_kernel_request(mobj, r, DATA_NONE, 0);
			return;
		}
		assert(K->inited);
		SVM_SET_PROT(MOBJ,page,VM_PROT_READ);
		SVM_SET_STATE(K,page,VM_PROT_READ,0);
		r->k_count++;
		MOBJ->k_count++;
		K->k_count++;
		m_svm_do_lock_request(k, TRUE, FALSE, VM_PROT_WRITE, r, mobj);
		return;
	}

	/*
	 * No current kernel use conflicts with requesting kernel's
	 * desired use. Call m_svm_satisfy_kernel_request, which
	 * will handle any requests that need to be made of the pager.
	 */
	m_svm_satisfy_kernel_request(mobj, r, DATA_NONE, 0);
}

void
m_svm_process_pager_request(mobj, r)
	xmm_obj_t mobj;
	request_t r;
{
	int page;
	xmm_obj_t k;

	xmm_entry2(m_svm_process_pager_request, mobj, r);

	assert(mobj->class == &msvm_class);
	page = atop(r->offset);

	/*
	 *	Analyze protection value to determine whether clean
	 *	or flush is required.
	 */
	if (r->lock_value != VM_PROT_NO_CHANGE) {
		/*
		 * Locking against non-write implies locking all access.
		 * Is this a bug, or universal truth?
		 * Beware: code below and elsewhere depends on this mapping.
		 */
		if (r->lock_value & ~VM_PROT_WRITE) {
			r->lock_value = VM_PROT_ALL;
		}

		/*
		 * XXX we can't yet represent
		 *	(lock=write but dirty)
		 * or
		 *	(lock=all but resident)
		 *
		 * Thus we force lock=write into clean,
		 * and lock=all into flush.
		 */
		if (r->lock_value == VM_PROT_WRITE) {
			r->should_clean = TRUE;
		} else if (r->lock_value) {
			r->should_clean = TRUE;
			r->should_flush = TRUE;
		}
        }

	/*
	 * If we need to flush, or lock all access, then we must talk
	 * to all kernels.
	 */
	if (r->should_flush || r->lock_value == VM_PROT_ALL) {
		/*
		 *	Page is not resident when done, hence no access.
		 */
		r->k_count++;
		MOBJ->k_count++;
		r->lock_value = VM_PROT_ALL;
		SVM_SET_PROT(MOBJ,page,VM_PROT_NONE);

		for (k = MOBJ->kobj_list; k; k = K->next) {
			if (SVM_GET_PROT(K,page) == VM_PROT_NONE) {
				continue;
			}
			assert(K->inited);
			r->k_count++;
			MOBJ->k_count++;
			K->k_count++;
			SVM_SET_STATE(K,page,VM_PROT_NONE,0);
			m_svm_do_lock_request(k, r->should_clean,
					      r->should_flush,
					      VM_PROT_ALL, r, mobj);
		}
		if (--r->k_count == 0 && r->m_count == 0) {
			m_svm_satisfy_request(mobj, r, DATA_NONE, 0);
			if (--MOBJ->k_count == 0 &&
			    MOBJ->state == MOBJ_STATE_SHOULD_TERMINATE) {
				xmm_terminate_pending--;
				xmm_svm_destroy(mobj);
			}
		}
		else {
			MOBJ->k_count--;
		}
		return;
	}

	/*
	 * If we need to clean, or lock write access, and there is in fact
	 * a writer, then we must talk to that writer.  If there is no
	 * kobj writer, then assume it has been terminated and proceed
	 * as if there is no writer.
	 */
	if ((r->should_clean || r->lock_value == VM_PROT_WRITE)
	    && (SVM_GET_PROT(MOBJ,page) & VM_PROT_WRITE)) {

		for (k = MOBJ->kobj_list; k; k = K->next) {
			if (SVM_GET_PROT(K,page) & VM_PROT_WRITE) {
				break;
			}
		}

		if (k == XMM_OBJ_NULL) {
			vm_prot_t tmp = SVM_GET_PROT(MOBJ, page);
			if (r->lock_value != VM_PROT_NO_CHANGE)
				tmp &= ~r->lock_value;
			else
				tmp &= ~VM_PROT_WRITE;
			SVM_SET_PROT(MOBJ, page, tmp);
			m_svm_satisfy_pager_request(mobj, r);
			return;
		}

		if (r->lock_value != VM_PROT_NO_CHANGE) {
			vm_prot_t tmp;
			tmp=SVM_GET_PROT(MOBJ, page) & ~r->lock_value;
			SVM_SET_PROT(MOBJ, page, tmp);
			tmp=SVM_GET_PROT(K, page) & ~r->lock_value;
			SVM_SET_STATE(K, page, tmp, 0);
		}

		assert(K->inited);
		r->k_count++;
		MOBJ->k_count++;
		K->k_count++;
		m_svm_do_lock_request(k, r->should_clean, FALSE, r->lock_value,
				      r, mobj);
		return;
	}

	/*
	 * We didn't need to flush, clean, or lock.
	 */
	m_svm_satisfy_pager_request(mobj, r);
}

void
m_svm_process_request(mobj, r)
	xmm_obj_t mobj;
	request_t r;
{
	xmm_entry2(m_svm_process_request, mobj, r);

	assert(mobj->class == &msvm_class);
	if (r->is_kernel) {
		m_svm_process_kernel_request(mobj, r);
	} else {
		m_svm_process_pager_request(mobj, r);
	}
}

void
m_svm_satisfy_kernel_request(mobj, r, data, err_value)
	xmm_obj_t mobj;
	request_t r;
	vm_offset_t data;
	kern_return_t err_value;
{
	xmm_obj_t kobj;
	request_t r_next;
	vm_prot_t page_lock;

	xmm_entry4(m_svm_satisfy_kernel_request, mobj, r, data, err_value);

	kobj = r->who.kobj;
	assert(mobj->class == &msvm_class);
	assert(r->is_kernel);
	assert(r->k_count == 0);
	assert(r->m_count == 0);

	/*
	 * If we need an unlock or data from the pager, make the request now.
	 */
	page_lock = SVM_GET_LOCK(MOBJ,atop(r->offset));
	if ((page_lock & r->desired_access) ||
	    (r->needs_data && data == DATA_NONE)) {
		if (data) {
			/* coerce into using memory_object_data_write path */
			M_DATA_WRT_RTN(mobj, r->offset, data,
				     PAGE_SIZE, TRUE, FALSE,
				     MOBJ->use_routine);
		}
		r->m_count++;
		if (r->needs_data) {
			M_DATA_REQUEST(mobj, r->offset, PAGE_SIZE,
				       r->desired_access);
		} else {
			M_DATA_UNLOCK(mobj, r->offset, PAGE_SIZE,
				      r->desired_access);
		}
		return;
	}

	/*
	 * Remove the request from the queue now
	 */
	r_next = m_svm_remove_request(mobj, r->offset);
	assert(MOBJ->request_count != 0 || queue_empty(&MOBJ->requests));
	track(MOBJ, r, "satisfy_kernel");

	/*
	 * We have everything we need. Satisfy the kernel request.
	 */
	if (! r->needs_data) {
		K_LOCK_REQUEST(r->who.kobj, r->offset, PAGE_SIZE, FALSE,
			       FALSE, r->desired_access ^ VM_PROT_ALL,
			       XMM_REPLY_NULL);
	} else if (data == DATA_UNAVAILABLE) {
		K_DATA_UNAVAILABLE(r->who.kobj, r->offset, PAGE_SIZE);
		r->desired_access = VM_PROT_ALL;
	} else if (data == DATA_ERROR) {
		K_DATA_ERROR(r->who.kobj, r->offset, PAGE_SIZE,
			     err_value);
		/* XXX start killing object? */
	} else {
		/*
		 *	If there's only 1 kernel, there's no advantage
		 *	to restricting the kernel's access -- upgrade
		 *	its request to everything allowed by the manager.
		 */
	  	if (MOBJ->kobj_count == 1) {
			r->desired_access = page_lock;
		}
		K_DATA_SUPPLY(r->who.kobj, r->offset, data, PAGE_SIZE,
			      r->desired_access ^ VM_PROT_ALL, FALSE,
			      XMM_REPLY_NULL);
	}

	/*
	 * Update KOBJ prot and MOBJ prot values.
	 */
	SVM_SET_PROT(MOBJ,atop(r->offset),r->desired_access);
	SVM_SET_STATE(KOBJ,atop(r->offset),r->desired_access, 0);

	/*
	 * Free the request.
	 */

	zfree(xmm_svm_request_zone, (vm_offset_t) r);

	/*
	 * If there is another request, process it now.
	 */
	if (r_next) {
		track(MOBJ, r_next, "satisfy_kernel r_next");
		assert(MOBJ->request_count != 0||queue_empty(&MOBJ->requests));
		m_svm_process_request(mobj, r_next);
		assert(MOBJ->request_count != 0||queue_empty(&MOBJ->requests));
	}
}

void
m_svm_satisfy_pager_request(mobj, r)
	xmm_obj_t mobj;
	request_t r;
{
	request_t r_next;

	xmm_entry2(m_svm_satisfy_pager_request, mobj, r);

	assert(mobj->class == &msvm_class);
	assert(! r->is_kernel);

#if	USE_XMM_BUFFER
	/*
	 * Flush or clean any buffered data if necessary.
	 */
	if (r->should_flush || r->should_clean) {
		M_UNBUFFER_DATA(mobj, r->offset, PAGE_SIZE,
				r->should_clean, r->should_flush);
	}
#endif	USE_XMM_BUFFER

	/*
	 * We have everything we need. Satisfy the pager request.
	 */
	if (r->who.reply != XMM_REPLY_NULL) {
		M_LOCK_COMPLETED(mobj, r->offset, PAGE_SIZE, r->who.reply,
				 FALSE);
	}

	/*
	 * Update MOBJ lock value.
	 */
	if (r->lock_value != VM_PROT_NO_CHANGE) {
	    SVM_SET_LOCK(MOBJ,atop(r->offset),r->lock_value);
	}

	/*
	 * Remove and free request.
	 * 
	 * We may have satisfied this request even though there
	 * are outstanding requests on the kernel or pager.  Move
	 * the bookkeeping counts.  In this case we cannot process
	 * another request.
	 */
	r_next = m_svm_remove_request(mobj, r->offset);
	assert(MOBJ->request_count != 0 || queue_empty(&MOBJ->requests));
	if (r->k_count != 0 || r->m_count != 0) {
		assert(r_next && (r->offset == r_next->offset));

		r_next->k_count = r->k_count;
		r_next->m_count = r->m_count;
		zfree(xmm_svm_request_zone, (vm_offset_t) r);
	}
	else { 
		zfree(xmm_svm_request_zone, (vm_offset_t) r);

		/*
		 * If there is another request, process it now.
		 */
		if (r_next) {
			m_svm_process_request(mobj, r_next);
		}
	}
	assert(MOBJ->request_count != 0 || queue_empty(&MOBJ->requests));
}

void
m_svm_satisfy_request(mobj, r, data, err_value)
	xmm_obj_t mobj;
	request_t r;
	vm_offset_t data;
	kern_return_t err_value;
{
	xmm_entry4(m_svm_satisfy_request, mobj, r, data, err_value);

	assert(mobj->class == &msvm_class);
	assert(MOBJ->request_count != 0 || queue_empty(&MOBJ->requests));
	if (r->is_kernel) {
		m_svm_satisfy_kernel_request(mobj, r, data, err_value);
	} else {
		m_svm_satisfy_pager_request(mobj, r);
	}
	assert(MOBJ->request_count != 0 || queue_empty(&MOBJ->requests));
}


/*
 *	This mobj is dead.  Free all the changes and requests attached to it.
 *
 */
xmm_svm_cleanup(mobj)
xmm_obj_t mobj;
{
	mchange_t	c, c_next;
	request_t	r, r_next_eq;

	xmm_entry1(xmm_svm_cleanup, mobj);

	assert(MOBJ->kobj_list == XMM_OBJ_NULL && MOBJ->kobj_count == 0);

	c = MOBJ->change;
	while (c) {
		c_next = c->next;
		zfree(xmm_svm_change_zone, (vm_offset_t) c);
		c = c_next;
	}

	while ((r = (request_t) dequeue_head(&MOBJ->requests))!=REQUEST_NULL) {
		MOBJ->request_count--;
		do {
			r_next_eq = r->next_eq;
			zfree(xmm_svm_request_zone, (vm_offset_t) r);
			r = r_next_eq;
		} while (r != REQUEST_NULL);
	}
	MOBJ->last_found = REQUEST_NULL;
}


xmm_svm_init()
{
	xmm_entry0(xmm_svm_init);

	xmm_svm_request_zone = zinit(sizeof(struct request), 512*1024,
				     sizeof(struct request), FALSE,
				     "xmm.svm.request");
	xmm_svm_change_zone = zinit(sizeof(struct mchange), 512*1024,
				     sizeof(struct mchange), FALSE,
				     "xmm.svm.change");
	xmm_svm_pending_zone = zinit(sizeof(struct pending), 512*1024,
				     sizeof(struct pending), FALSE,
				     "xmm.svm.pending");
	xmm_svm_gather_zone = zinit(sizeof(struct gather), 512*1024,
				     sizeof(struct gather), FALSE,
				     "xmm.svm.gather");
#if	REQUEST_TRACKING
	global_track_init();
#endif
}

#include <sys/varargs.h>

int xmm_svm_debug = 0;

/* VARARGS */
xmm_svm_dprintf(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (xmm_svm_debug) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		printf(fmt, &listp);
		va_end(listp);
	}
}

#include <mach_kdb.h>
#if	MACH_KDB
#define	printf	kdbprintf

#define	BOOL(b)		((b) ? "" : "!")


unsigned int
xmm_svm_count_pending(p)
pending_t	p;
{
	unsigned int	count;

	for (count = 0; p != PENDING_NULL; ++count, p = p->next)
		;
	return count;
}


/*
 *	Routine:	m_svm_print
 *	Purpose:
 *		Pretty-print an svm mobj.
 */

m_svm_print(mobj)
	xmm_obj_t mobj;
{
	extern int indent;

	iprintf("svm mobj 0x%x\n", mobj);

	indent += 2;

	iprintf("kobj_list=0x%x(%d)", MOBJ->kobj_list, MOBJ->kobj_count);

	switch (MOBJ->state) {

	case MOBJ_STATE_UNCALLED:
		printf(", uncalled");
		break;

	case MOBJ_STATE_CALLED:
		printf(", called");
		break;

	case MOBJ_STATE_READY:
		printf(", ready");
		break;

	case MOBJ_STATE_SHOULD_TERMINATE:
		printf(", should_terminate");
		break;

	case MOBJ_STATE_TERMINATED:
		printf(", terminated");
		break;

	default:
		printf(", state=%d?", MOBJ->state);
	}
	printf(", num_pages=%d\n", MOBJ->num_pages);

	iprintf("request{next=0x%x,prev=0x%x} count=0x%x last_found=0x%x\n",
		MOBJ->requests.next,
		MOBJ->requests.prev,
		MOBJ->request_count,
		MOBJ->last_found);

	iprintf("svm_state=0x%x, %smay_cache, use_routine=0x%x",
		MOBJ->svm_state,
		BOOL(MOBJ->may_cache),
		MOBJ->use_routine);

	switch (MOBJ->copy_strategy) {

	case MEMORY_OBJECT_COPY_NONE:
		printf(", copy_none\n");
		break;

	case MEMORY_OBJECT_COPY_CALL:
		printf(", copy_call\n");
		break;

	case MEMORY_OBJECT_COPY_DELAY:
		printf(", copy_delay\n");
		break;

	case MEMORY_OBJECT_COPY_TEMPORARY:
		printf(", copy_temporary\n");
		break;

	default:
		printf(", copy_strategy=%d?\n", MOBJ->copy_strategy);
	}

	iprintf("memory_object=0x%x, memory_object_name=0x%x\n",
		MOBJ->memory_object,
		MOBJ->memory_object_name);

	iprintf("change=0x%x, k_count=%d\n",
		MOBJ->change,
		MOBJ->k_count);
	iprintf("%swrite_completions, pending=0x%x pending count is 0x%x\n",
		MOBJ->write_completions ? "" : "!",
		MOBJ->pending,
		xmm_svm_count_pending(MOBJ->pending));
#if	PENDING_TRACKING
	iprintf("p_seen=0x%x p_matched=0x%x p_terminated=0x%x\n",
		MOBJ->p_seen, MOBJ->p_matched, MOBJ->p_terminated);
#endif

	indent -= 2;

	return 0;
}

/*
 *	Routine:	k_svm_print
 *	Purpose:
 *		Pretty-print an svm kobj.
 */

void
svm_prot_print(prot)
	vm_prot_t prot;
{
	if (prot == VM_PROT_NO_CHANGE) {
		printf("<no change>");
	} else {
		printf("0x%x:%c%c%c",
		       prot,
		       ((prot & VM_PROT_READ)    ? 'r' : '-'),
		       ((prot & VM_PROT_WRITE)   ? 'w' : '-'),
		       ((prot & VM_PROT_EXECUTE) ? 'x' : '-'));
	}
}


k_svm_print(kobj)
	xmm_obj_t kobj;
{
	extern int indent;

	iprintf("svm kobj 0x%x\n", kobj);

	indent += 2;

	iprintf("num_pages=%d, svm_state=0x%x mobj=0x%x\n", KOBJ->num_pages,
		KOBJ->svm_state, KOBJ->svm_mobj);
	iprintf("next=0x%x k_count=0x%x gather=0x%x inited=%s terminated=%s\n",
		KOBJ->next, KOBJ->k_count, KOBJ->gather,
		db_bool_str(KOBJ->inited), db_bool_str(KOBJ->terminated));
	
	indent -=2;
}

/*
 *	Routine:	r_svm_print
 *	Purpose:
 *		Pretty-print a request.
 */

void
r_svm_print(req)
	request_t req;
{
	extern int indent;

	indent += 2;

	iprintf("kobj=0x%x, offset=0x%x, m_count=0x%x, k_count=0x%x\n",
		req->who.kobj,
		req->offset,
		req->m_count,
		req->k_count);

	iprintf("%sis_kernel, %sneeds_data, %sshould_clean, %sshould_flush\n",
		BOOL(req->is_kernel),
		BOOL(req->needs_data),
		BOOL(req->should_clean),
		BOOL(req->should_flush));

	iprintf("desired_access=");
	svm_prot_print(req->desired_access);
	printf(", lock_value=");
	svm_prot_print(req->lock_value);
	printf("\n");
	iprintf("next_eq=0x%x, chain_neq{next=0x%x,prev=0x%x}\n",
		req->next_eq,
		req->chain_neq.next,
		req->chain_neq.prev);

	indent -=2;
}


m_msvm_db_print(mobj)
xmm_obj_t	mobj;
{
#ifdef	lint
	M_DB_PRINT(mobj);
#endif
	iprintf("***m_msvm_db_print***\n");
	return 0;
}


m_ksvm_db_print(mobj)
xmm_obj_t	mobj;
{
#ifdef	lint
	M_DB_PRINT(mobj);
#endif
	iprintf("***m_ksvm_db_print***\n");
	return 0;
}


k_msvm_db_print(mobj)
xmm_obj_t	mobj;
{
	return m_svm_print(mobj);
}


k_ksvm_db_print(kobj)
xmm_obj_t	kobj;
{
	return k_svm_print(kobj);
}


xmm_obj_t
ksvm_to_msvm(kobj)
xmm_obj_t	kobj;
{
	return KOBJ->svm_mobj;
}
#else	/* MACH_KDB */
m_msvm_db_print(mobj) xmm_obj_t	mobj; { }
m_ksvm_db_print(mobj) xmm_obj_t	mobj; { }
#endif	/* MACH_KDB */


#if	REQUEST_TRACKING
global_track_init()
{
	req_track_t	tp;

	global_track_index = 0;
	for (tp = global_tracks; tp < global_tracks + MAX_SYS_TRACKS; ++tp) {
		tp->req = REQUEST_NULL;
		tp->op = "global no op";
		tp->req_count = 0;
	}
}


track_init(trackp, indexp)
req_track_t	trackp;
unsigned int	*indexp;
{
	req_track_t	tp;

	*indexp = 0;
	for (tp = trackp; tp < trackp + MAX_OBJ_TRACKS; ++tp) {
		tp->req = REQUEST_NULL;
		tp->op = "no op";
		tp->req_count = 0;
	}
}


track(mobj, req, op)
struct mobj	*mobj;
request_t	req;
char		*op;
{
	req_track_t	tp;

	if (mobj->track_index >= MAX_OBJ_TRACKS)
		mobj->track_index = 0;
	tp = mobj->tracks + mobj->track_index;
	tp->req = req;
	tp->op = op;
	tp->req_count = mobj->request_count;
	if (req != REQUEST_NULL)
		tp->offset = req->offset;
	else
		tp->offset = -1;
	mobj->track_index++;

	if (global_track_index >= MAX_SYS_TRACKS)
		global_track_index = 0;
	tp = global_tracks + global_track_index;
	tp->req = req;
	tp->op = op;
	tp->req_count = mobj->request_count;
	if (req != REQUEST_NULL)
		tp->offset = req->offset;
	else
		tp->offset = -1;
	global_tracks_mobj[global_track_index] = (unsigned int) mobj;
	++global_track_index;
}


track_print(mobj, start, end)
int		start;
int		end;
{
	req_track_t	tp;

	for (tp = MOBJ->tracks + start; tp < MOBJ->tracks + end; ++tp)
		kdbprintf("[%3d] req=0x%x count=%d op=%s\n",
			  tp - MOBJ->tracks, tp->req, tp->req_count, tp->op);
}


m_track_print(mobj)
xmm_obj_t	mobj;
{
	kdbprintf("mobj 0x%x most recent event at index %d\n",
		  mobj, MOBJ->track_index);
	track_print(mobj, 0, MAX_OBJ_TRACKS);
}


global_track_print(start, end)
{
	req_track_t	tp;

	for (tp = global_tracks + start; tp < global_tracks + end; ++tp)
		kdbprintf("[%3d] mobj=0x%x req=0x%x count=%d op=%s\n",
			  tp - global_tracks,
			  global_tracks_mobj[tp - global_tracks],
			  tp->req, tp->req_count, tp->op);
}


k_verify_request_list(kobj)
xmm_obj_t	kobj;
{
	xmm_obj_t mobj = KOBJ->svm_mobj;

	verify_request_list(MOBJ);
}


verify_request_list(mobj)
xmm_obj_t	mobj;
{
	struct queue_entry	*q_first, *q_last, *q_end;
	struct request		*r;

	/*
	 *	Verify state of head of queue and
	 *	adjacent entries.
	 */
	assert(MOBJ->request_count >= 0);
	q_last = queue_last(&MOBJ->requests);
	q_end = q_last->next;
	assert(q_end == &MOBJ->requests);
	assert(MOBJ->requests.prev == q_last);
	q_first = queue_first(&MOBJ->requests);
	assert(q_first->prev == &MOBJ->requests);
	assert(MOBJ->requests.next == q_first);
	if (MOBJ->request_count < 2) {
		assert(MOBJ->requests.next == MOBJ->requests.prev);
		if (MOBJ->request_count < 1) {
			assert(MOBJ->requests.next == &MOBJ->requests);
			assert(queue_empty(&MOBJ->requests));
		}
	}

	if (svm_verify_requests_closely == TRUE)
		for (r = (request_t) queue_first(&MOBJ->requests);
		     !queue_end(&MOBJ->requests, (queue_head_t *) r);
		     r = (request_t) queue_next(&r->chain_neq)) {
			assert(r == (request_t) r->chain_neq.prev->next);
			assert(r == (request_t) r->chain_neq.next->prev);
		}

	return 1;
}
#endif	/* REQUEST_TRACKING */
