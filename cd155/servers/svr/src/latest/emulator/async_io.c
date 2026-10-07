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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 *
 */

/*
 *      This module contains the emulator functions that are
 *      used to implement user level asynchronous I/O support.
 *
 * HISTORY
 * $Log: async_io.c,v $
 * Revision 1.19  1995/04/04  16:41:00  tnt
 * PTS #: 12348
 * Mandatory?:  Tensor Mandatory (H0).
 * Description: When running ipd the SIGTRAP signal used to stop the process
 *              under debug was interferring with the operation of the
 *              asynchronous pfs system calls.  The pfs system calls detect
 *              the interruption due to the receipt of the signal, and return
 *              the EINTR errno.  The solution is to override the error returned
 *              if, after querying the server, we find that no signals are
 *              pending for the process.  A status indicating that the operation
 *              completed normally is then returned.
 * Reviewer(s): Bob Godley, John Litvin
 * Risk:        Medium
 * Testing:     gprof, prof, VSX, and ipd EATs.  Parallel SATs.
 * Module(s):   Server/svr/src/svr/emulator/async_io.c
 *              Server/svr/src/svr/emulator/i860/emul_machdep.c
 *
 * Changed async_done() and async_wait() to always place the byte count
 * written into the first location in the rval array.  This should be
 * done independent of the error value returned.
 *
 * Revision 1.18  1994/11/18  20:22:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/11/16  22:05:27  rlg
 * A timing window in async_scheduler() found during MP testing was fixed.
 * The setting of the IODONE flag in the async request structure was moved
 * to follow the call to fdt_unref_entry().
 *
 *  Reviewer:  Len Brown
 *  Risk:  low
 *  Benefit or PTS #:  11529
 *  Testing:  fileio and pfs EATs
 *  Module(s):  emulator/async_io.c
 *
 * Revision 1.16  1994/03/29  17:12:40  rlg
 * Merged the changes from 1.12.4.4 on the R1.2 branch into the R1.3 branch
 *
 * Revision 1.15  1994/03/11  22:43:10  rlg
 * Merged the changes from 1.12.4.3 on the R1.2 branch to the R1.3 branch.
 *
 * Revision 1.14  1994/02/24  23:17:23  rlg
 * Merged the changes from 1.12.4.2 on the R1.2 branch to the R1.3 branch.
 *
 * Revision 1.13  1993/12/21  18:57:04  rlg
 *  Merged the R1.12.4.1 update into the main trunk of the data base
 *
 * Revision 1.12.4.4  1994/03/29  16:08:48  rlg
 * The warnings found by lint were evaluated and corrected as required.
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  low
 *  Benefit or PTS #:  7720
 *  Testing:  fileio and pfs EATs; six eval tests
 *  Module(s):  async_io.c
 *
 * Revision 1.12.4.3  1994/03/11  22:18:45  rlg
 * The get_async() function was changed so that the list lock is released on
 * the error condition of too many outstanding asynchronous I/O requests.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  low
 *  Benefit or PTS #:  8481
 *
 * Revision 1.12.4.2  1994/02/24  22:52:26  rlg
 * The initialization of the two async spin locks was removed from async_init()
 * and put into a new subroutine, async_lock_init().  This change allows the
 * async_list_lock  to be initialized before its first use out of the
 * get_async()  function.  (PTS # 8211)
 *
 * Revision 1.12.4.1  1993/12/15  23:32:15  rlg
 * Added code to  free_async_queue()  to free any asynchronous I/O requests
 * that the user had not waited for when a file is being closed.
 *
 *  Reviewer:  Dave Minturn
 *  Risk:  Low
 *  Benefit or PTS #:  6486
 *
 * Revision 1.12  1993/08/04  17:03:19  wunder
 * Changed use of EQMID to EBADID per BUG 5936.
 *
 * Revision 1.11  1993/08/03  01:42:12  wunder
 * Added thread_switch call within queue_async() to forcibly pass control to
 * async request thread to get I/O started to server.
 *
 * Revision 1.10  1993/07/21  20:51:27  dbm
 * Initialized the state of 'used' async I/O threads to not be in
 * emulator space.  This was causing the callback thread to restart them.
 *
 * Revision 1.9  1993/06/16  23:11:44  wunder
 * Added initializing async_errno field for queue_sync().
 *
 * Revision 1.8  1993/05/27  20:10:05  wunder
 * Fixed incorrect host_info call, added info_count.
 *
 * Revision 1.7  1993/05/27  02:46:54  wunder
 * Modified queue_sync to not queue request is queue is empty, added modified
 * system call tracing, now use thread_switch instead of swtch for performance
 * improvement.
 *
 * Revision 1.6  1993/05/25  18:32:24  dbm
 * Added interrupt support for asyncronous calls.
 *
 * Revision 1.5  1993/05/06  20:14:27  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/27  00:09:48  wunder
 * Fixed bug in async_scheduler were request was marked done before it
 * had been removed from the fdte queue.
 *
 * Revision 1.3  1993/04/06  17:50:16  wunder
 * Added ifdef PFS around module, changed dequeue_sync to be void function.
 *
 * Revision 1.2  1993/04/03  03:17:09  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.4  1993/03/25  23:49:41  wunder
 * Removed use of type field within async_req and redefined these values to
 * within the state field.
 *
 * Revision 1.1.2.3  1993/03/20  01:16:48  wunder
 * Updated async initialization to support fork.
 *
 * Revision 1.1.2.2  1993/03/17  22:28:54  wunder
 * Corrected case statements in async_scheduler().
 *
 * Revision 1.1.2.1  1993/03/16  18:31:36  wunder
 * Modified async_scheduler to use sync_readv and sync_writev.
 *
 * Revision 1.1  1993/03/16  01:46:37  wunder
 * Initial version for user asynchronous I/O support.
 *
 *
 *
 */
#ifdef PFS

/*#include <mach_init.h>*/
#include <mach/mig_errors.h>
#include <mach/thread_switch.h>
#include <mach/mach_traps.h>
#include <uxkern/fsvr.h>
#include <machine/vmparam.h>
#include <cthreads.h>
#include <machine/psl.h>
#include "emul_stack.h"
#include "emul.h"
#include "pfs_fdt.h"
#include "fdt.h"

#ifdef MAP_UAREA
#include <sys/ushared.h>
extern int shared_enabled;
extern spin_lock_t in_emulator_lock;
extern struct ushared_ro *shared_base_ro;
extern struct ushared_rw *shared_base_rw;
#endif
extern int	async_interrupt;
extern boolean_t must_suspend;

/*
 * define global pointers for asynchronous request lists
 */
async_req	*next_free_async;	/* pointer to free async_req list */
int		next_free_thread;	/* index to free thread list */
async_req	**active_list;		/* array of active async_reqs */
async_fdte_queue    *free_thread_list;	/* array of free request threads */
spin_lock_t	thread_list_lock;	/* lock required to access free
					   list of request threads */
spin_lock_t	async_list_lock;	/* lock required to access free and
					   active lists */
ulong		current_asyncs;		/* number of already created async_req
					   structures */
ulong		current_threads;	/* number of already created thread
					   structures */
ulong		num_free_threads;	/* number of unallocated threads */
ulong		active_lst_size;	/* number of entries available in the
					   active list. should always be >= the
					   number of current_asyncs */
boolean_t	async_initialized = 0;	/* flag indicating that initial async
					   initialization has completed */
int		min_wait_time = 0;	/* minimum thread switch wait time */

/* all external references */
extern boolean_t	stack_init_done;
extern vm_size_t	emul_stack_size;
extern vm_offset_t	emul_stack_mask;
#ifdef DEBUG_ASYNC
extern void 		e_printf();
#endif
extern int pfs_debug_flag;

/* forward references */
void			async_scheduler();
emul_stack_t		async_stack_alloc();




/*
 * NAME:        
 *	async_lock_init()
 *
 * DESCRIPTION:
 *	This subroutine initializes the two global locks that control
 *	acces to the list of free async threads and the list of free and
 *	active async elements.
 *	
 *
 * PARAMETERS:
 *	None
 *
 * RETURNS:
 *      None
 */
void
async_lock_init()
{
	/*
	 * initialize list access locks:
	 */
	spin_lock_init(&async_list_lock);
	spin_lock_init(&thread_list_lock);

} /* end async_lock_init() */




/*
 * NAME:        
 *	async_init()
 *
 * DESCRIPTION:
 *	This function will initialize the lists of asynchronous request
 *	and asynchronous fdte structures.  This function is also called
 *	to add more asynchronous request structures when the pool of free
 *	request structures has run out.  Page size areas are allocated
 *	and partitioned into individual request structures.  Request
 *	structures are linked together for quick access.  The initial list
 *	of fdte lock structures is created large enough to cover the
 *	maximum number of open files (NOFILE).  Another list, the active
 *	list, is maintained large enough to hold a pointer to each request
 *	structure that is allocated from the free list.
 *	
 *
 * PARAMETERS:
 *	None
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
async_init()
{
	register long	i, status;
	register long	new_size, new_count;
	async_req	*new_async_req;
	char	*async_data_area, *new_data_area;
	async_req	**tmp_active_list;
	kern_return_t	rc;
	host_sched_info_data_t  sched_info;
	int		info_count;

	PFS_TRACE(("  async_init:\n"));

#ifdef DEBUG_ASYNC
	{
	vm_statistics_data_t	vm_stats;

	/* verify vm statistics */
	rc = vm_statistics(mach_task_self(), &vm_stats);
	e_printf("vm_stat: free_count %d\n", vm_stats.free_count);
	e_printf("vm_stat: active_count %d\n", vm_stats.active_count);
	e_printf("vm_stat: inactive_count %d\n", vm_stats.inactive_count);
	e_printf("vm_stat: wire_count %d\n", vm_stats.wire_count);
	e_printf("vm_stat: zero_count %d\n", vm_stats.zero_fill_count);
	}
#endif

        /* determine host scheduling quantum */
	info_count = HOST_SCHED_INFO_COUNT;
        rc = host_info(mach_host_self(), HOST_SCHED_INFO, &sched_info,
                        &info_count);
        if (rc != KERN_SUCCESS) {
#ifdef DEBUG_ASYNC
                e_printf("host_info returns %x\n", rc);
#endif
                return EIO;
        }
        min_wait_time = sched_info.min_timeout;
#ifdef DEBUG_ASYNC
        e_printf("host_info min_timeout %d\n", sched_info.min_timeout);
        e_printf("host_info min_quantum %d\n", sched_info.min_quantum);
#endif

	/* perform initial asynchronous request setup */
	if (!async_initialized) {
		/* initialize global values */
		current_asyncs = 0;
		current_threads = 0;
		num_free_threads = 0;
		active_lst_size = 0;

		/* allocate initial async data structure area, outside
		   of the emulator range */
		if (status = pfs_malloc(&async_data_area, vm_page_size)) {
			return status;
		}
#ifdef DEBUG_ASYNC
		e_printf("async_init: async data area %X\n", async_data_area);
#endif

		/* set pointers into initial data area. fist part of area is
		   for fixed number of open file async lock structures.  the
		   following area left in the allocated page will be used
		   for async request structures */
		free_thread_list = (async_fdte_queue *)async_data_area;
		new_async_req = (async_req *)
			&async_data_area[sizeof(async_fdte_queue)*NOFILE];

		/* initialize and chain together each async_req entry */
		new_count = (vm_page_size-(sizeof(async_fdte_queue)*NOFILE))/
							sizeof(async_req);
#ifdef DEBUG_ASYNC
		e_printf("async_init: ftl %X, nar %X, nc %d\n", 
			free_thread_list, new_async_req, new_count);
#endif
		next_free_async = NULL;
		for (i=0; i<new_count; i++) {

			/* initialize free list setting each unique id
			   and initializing each lock */
			new_async_req->next = next_free_async;
			new_async_req->async_id = i;
			spin_lock_init(&new_async_req->async_lock);
			next_free_async = new_async_req++;

		}

		/* create the first set of async request threads */
		spin_lock(&thread_list_lock);
		if (status = init_threads())
			return (status);
		spin_unlock(&thread_list_lock);
		async_initialized++;

	} else {

		/* verify we do not exceed the maximum 
		   number of async requests */
		if (current_asyncs >= MAX_ASYNC_REQUESTS) {
			return EQNOMID;
		}

		/* create additional page of async request 
		   structure data area */
		if (status = pfs_malloc(&new_data_area, vm_page_size)) {
			return status;
		}
		new_async_req = (async_req *)new_data_area;
#ifdef DEBUG_ASYNC
		e_printf("async_init: async data area %X\n",
			 async_data_area);
#endif
		/* link new structures together and initialize them */
		new_count = vm_page_size/sizeof(async_req);
		for (i=current_asyncs; i<(current_asyncs + new_count); i++) {

			/* initialize free list setting each unique id */
			new_async_req->next = next_free_async;
			new_async_req->async_id = i;
			spin_lock_init(&new_async_req->async_lock);
			next_free_async = new_async_req++;

		}
	}
	current_asyncs += new_count;
#ifdef DEBUG_ASYNC
	e_printf("async_init: current_asyncs = %d\n", current_asyncs);
#endif

	/* check if active list needs to be expanded.  since a full page
	   is allocated each time make use of it all */
	if (current_asyncs > active_lst_size) {

		/* create a new active list data segment */
		tmp_active_list = active_list;
		new_size = active_lst_size +
				(vm_page_size/sizeof(next_free_async));
		if (status = pfs_malloc(&active_list,
					new_size*sizeof(next_free_async))) {
			/* reset list pointer to indicate create still needed */
			next_free_async = NULL;
			/* free previous buffer allocated */
			return status;
		}

		/* copy old active list to the new list, free the old list */
		for (i=0; i<new_size; i++) {
			if (i < active_lst_size)
				active_list[i] = tmp_active_list[i];
			else
				active_list[i] = NULL;
		}
		if (tmp_active_list != 0)
			free(tmp_active_list);
		active_lst_size = new_size;
#ifdef DEBUG_ASYNC
		e_printf("async_init: active_lst_size = %d\n",
			 active_lst_size);
#endif

	}

#ifdef DEBUG_ASYNC
	e_printf("async_init complete\n");
#endif

	return (ESUCCESS);

} /* end async_init() */




/*
 * NAME: 
 *	init_threads()
 *
 * DESCRIPTION:
 *	This function will initialize a set of asynchronous request threads
 *	(ARTs) that are associated one with each fdte lock structure. The
 *	thread is created along with a stack for it.
 *
 * PARAMETERS:
 *	None
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
init_threads()
{
	int i;
	kern_return_t rc;
	async_fdte_queue *new_art;


	PFS_TRACE(("  init_threads:\n"));

	/* current threads must not be >= NOFILE */
	if (current_threads == NOFILE)
		return EMFILE;

	/* add to list of free async request threads */
	for (i = current_threads; 
	     i < current_threads+PFS_INITIAL_ASYNC_OPS; i++) {

		new_art = &free_thread_list[i];
		new_art->index = (unsigned short)i;
		new_art->state = ASYNC_FREE;

		/* create the asynchronous request thread (ART) */
		rc = thread_create(mach_task_self(), &new_art->thread);
		if (rc != KERN_SUCCESS) {
#ifdef DEBUG_ASYNC
			e_printf("thread_create returns %x\n", rc);
#endif
			return EIO;
		}
#ifdef DEBUG_ASYNC
		e_printf("New thread #%d = %d\n", i, new_art->thread);
#endif
		/* create new stack for thread */
		new_art->stack = async_stack_alloc();
#ifdef DEBUG_ASYNC
		e_printf("New stack %X, emul_end %X\n", 
				new_art->stack, EMULATOR_END);
#endif
	}
	current_threads += PFS_INITIAL_ASYNC_OPS;
	num_free_threads += PFS_INITIAL_ASYNC_OPS;
	next_free_thread = current_threads-1;

	return ESUCCESS;
} /* end init_threads() */




/*
 * NAME:        
 *	async_stack_alloc()
 *
 * DESCRIPTION:
 *	Procedure that will create a request thread stack outside the
 *	emulator range.  Standard stack preparation is performed as
 *	done within emul_stack_alloc.c.
 *
 * PARAMETERS:
 *	None
 *
 * RETURNS:
 *      Returns a pointer for the newly created stack.
 */
emul_stack_t
async_stack_alloc()
{
        vm_offset_t             base;
        register emul_stack_t   new_stack;
        kern_return_t           ret;
        mach_port_t             reply_port;

	PFS_TRACE(("  async_stack_alloc:\n"));

        /*
         * Allocate a new reply port for the stack;
         * or take the global port if this is the first stack.
         */
        if (stack_init_done)
                reply_port = mach_reply_port();
        else
                reply_port = mig_get_reply_port();
        if (reply_port == MACH_PORT_NULL)
                emul_panic("async_stack_alloc: no reply port");

        /*
         * Look for the next free region at the correct alignment.
         * We must pass in the reply port explicitly, because
         * we aren't running on the new stack yet, so mig_get_reply_port()
         * can't find an appropriate reply port.
         */
	base = ((vm_offset_t)EMULATOR_END + emul_stack_size -1) 
		& emul_stack_mask;

        ret = emul_vm_map(mach_task_self(),
                     &base, emul_stack_size, (emul_stack_size - 1),
                     TRUE, MEMORY_OBJECT_NULL, (vm_offset_t)0, FALSE,
                     VM_PROT_DEFAULT, VM_PROT_ALL, VM_INHERIT_NONE);

        if (ret != KERN_SUCCESS) {
                emul_panic("async_stack_alloc: emul_vm_map failed");
	}

        /*
         * Set up top-of-stack structure with new reply port.
         */
#ifdef  STACK_GROWTH_UP
        new_stack = (emul_stack_t) (base);
        new_stack->link = 0;
        new_stack->reply_port = reply_port;
        new_stack++;
#else   STACK_GROWTH_UP
        new_stack = (emul_stack_t) (base + emul_stack_size);
        new_stack--;
        new_stack->link = 0;
        new_stack->reply_port = reply_port;
#endif  STACK_GROWTH_UP
#ifdef  DEBUG_ASYNC
	e_printf("async_stack_alloc: stack base %X, new_stack %X\n",
		base, new_stack);
#endif

        return (new_stack);

} /* end async_stack_alloc() */




/*
 * NAME:        
 *	free_async_queue()
 *
 * DESCRIPTION:
 *	This function is called only from free_fdte() to return an fdte
 *	queue structure to the free list when a file is closed.  The request
 *	thread is suspended and aborted in order to free in from all
 *	kernel queues.
 *
 * PARAMETERS:
 *	fdte	        - Pointer to the fdte entry being closed.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
free_async_queue(fdte)
fdt_entry_t	*fdte;
{
	thread_t	  emul_thread_id;
	async_fdte_queue *fdte_queue_ptr;
	int               i;
	async_req        *next_req;
	kern_return_t     rc;
	int               status;

	struct i860_thread_state 	state;

	PFS_TRACE(("  free_async_queue: fdte %X\n", fdte));

#ifdef DEBUG_ASYNC
	e_printf("free_async_queue: fdte #%d refcnt %d\n",
		 fdte, fdte->refcnt);
        e_printf("free_async_queue: async_queue %X\n",
		 fdte->async_queue);
        e_printf("free_async_queue: stack %X\n",
		 fdte->async_queue->stack);
        e_printf("free_async_queue: thread %X\n",
		 fdte->async_queue->thread);
        e_printf("free_async_queue: state %b\n",
		 fdte->async_queue->state);
        e_printf("free_async_queue: head %X\n",
		 fdte->async_queue->head);
#endif
	EASSERT(((async_fdte_queue *)
			&free_thread_list[fdte->async_queue->index])->thread 
			== fdte->async_queue->thread);

        /* get control of thread lock */
        spin_lock(&fdte->async_queue->lock);

	/* first suspend the thread from being scheduled */
        rc = thread_suspend(fdte->async_queue->thread);
        if (rc != KERN_SUCCESS) {
                emul_panic("free_async_queue: thread_suspend failed");
	}

	emul_thread_id = emul_thread_id_of(fdte->async_queue->thread, NULL);

	/* now remove it from any internal queues making it ready to be
	   restarted when needed for the next file */
        rc = thread_abort(fdte->async_queue->thread);
        if (rc != KERN_SUCCESS) {
                emul_panic("free_async_queue: thread_abort failed");
	}

	/* Reset program counter */
	state.pc = (unsigned long) EMULATOR_END + 1;

	/*
	 * Reset the asynchronous I/O request thread (ART)
	 */
	if ((rc = thread_set_state(
			fdte->async_queue->thread,
			i860_THREAD_STATE,
			(thread_state_t)&state,
			i860_THREAD_STATE_COUNT)) != KERN_SUCCESS) {
                return EIO;
	}
	
	/*
         * If there are any remaining asynchronous I/O requests that
	 * the user has not waited for, then free these orphan async_req
	 * entries and return them to the free chain.  (This code was
	 * added to fix Bug #6486)
	 */
	if ((fdte_queue_ptr = fdte->async_queue) != NULL) {

	        while (fdte_queue_ptr->head != NULL) {

			next_req = (fdte_queue_ptr->head)->next;
		        status = free_async(fdte_queue_ptr->head);
			fdte_queue_ptr->head = next_req;
		}
        }

        /* give up control of thread async fdte queue lock */
        spin_unlock(&fdte->async_queue->lock);

	/* get control of thread list lock */
	spin_lock(&thread_list_lock);

	/* mark thread stucture as free and stop the thread */
	fdte->async_queue->state = ASYNC_FREE;
	num_free_threads++;
	if ((i = fdte->async_queue->index) > next_free_thread)
		next_free_thread = i;

	/* give up control of thread list lock */
	spin_unlock(&thread_list_lock);

	/*
	 * Deregister this asynchronous thread in the isc table
	 */
	isc_async_deregister(emul_thread_id);

#ifdef  MAP_UAREA
	/*
	 * Inform the server that this thread is in the emulator
	 * data space.  This will allow the server to go through the
	 * appropriate signal so that the task is made aware of signals.
	 */
        if (shared_enabled) {
                spin_lock(&in_emulator_lock);
                shared_base_rw->us_in_emulator--;
                spin_unlock(&in_emulator_lock);
        }
#endif
	
	/* reset fdte to indicate async requesting not 
	   initialized on this file */
	fdte->async_queue = NULL;

	return ESUCCESS;
} /* end free_async_queue() */




/*
 * NAME:
 *	get_async()
 *
 * DESCRIPTION:
 *	This procedure pulls an async request structure from the free
 *	list and initializes its critical fields.  A pointer to this
 *	structure is placed in the active request list.  Lock is used
 *	around access to free list.
 *	
 * PARAMETERS:
 *	None
 *
 * RETURNS:
 *      ESUCCESS	- if successful.
 *      error number	- if an error occurred.
 */
int
get_async(new_req_id)
int	*new_req_id;
{
	async_req	*new_req;
	register int	status;

	PFS_TRACE(("  get_async: new_req_id ptr %X\n", new_req_id));

	/* get control of the access lock for the async_req lists */
	spin_lock(&async_list_lock);

	/* verify a free async_req entry is available */
	if (!async_initialized || next_free_async == NULL) {
		if (status = async_init()) {
		        spin_unlock(&async_list_lock);
			return status;
		}
	}

	/* get free entry from free list and insert pointer into active list */
	new_req = next_free_async;
	next_free_async = new_req->next;
	EASSERT(active_list[new_req->async_id] == NULL);
	active_list[new_req->async_id] = new_req;

	/* release control of access lock */
	spin_unlock(&async_list_lock);

	/* clear entries in the structure before returning */
	new_req->state = 0;
	new_req->actual = 0;
	new_req->async_errno = 0;
	new_req->next = NULL;

#ifdef DEBUG_ASYNC
	e_printf("get_async: give async_req #%d\n", new_req->async_id);
#endif
	*new_req_id = new_req->async_id;
	return ESUCCESS;

} /* end get_async() */




/*
 * NAME:
 *	free_async()
 *
 * DESCRIPTION:
 *	This function will take a request structure and free it back
 *	onto the free list.  The active list entry for this request
 *	structure is cleared.  Lock is used around access to free list.
 *
 * PARAMETERS:
 *	free_req        - pointer to request structure to free.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
free_async(free_req)
async_req *free_req;
{

	PFS_TRACE(("  free_async: free_req %X\n", free_req));

	/* get control of the access lock for the async_req lists */
	spin_lock(&async_list_lock);

	/* remove the request from the active list */
	EASSERT(active_list[free_req->async_id] == free_req);
	active_list[free_req->async_id] = NULL;

	/* add async_req structure back into free list */
	free_req->next = next_free_async;
	next_free_async = free_req;

	/* release control of access lock */
	spin_unlock(&async_list_lock);

	return ESUCCESS;

} /* end free_async() */




#ifdef DEBUG_ASYNC
/*
 * NAME: 
 *	dump_list_info()
 *
 * DESCRIPTION:
 *	Debug procedure to dump the state of all async lists and queues.
 *
 * PARAMETERS:
 *	None
 *
 * RETURNS:
 *	None
 */
void
dump_list_info()
{
	int i;
	async_req *tmp_req;

	/* print out the statistics of the lists */
	if (next_free_async != 0) {
		e_printf("Next free async_req id %d\n", 
			 next_free_async->async_id);
		/* scan free list, verify not also active */
		for (tmp_req = next_free_async; tmp_req != 0;
						tmp_req = tmp_req->next) {
			if (active_list[tmp_req->async_id] != 0)
				e_printf("Free req %d also active\n", 
					 tmp_req->async_id);
		}
	} else {
		e_printf("Free list now empty\n");
	}
	e_printf("Total # of async_reqs %d\n", current_asyncs);
	e_printf("Active list size %d\n", active_lst_size);

	/* scan active list and print active entries */
	for (i=0; i<active_lst_size; i++) {
		if (active_list[i] != 0) {
			e_printf("[%d] = %X\n", i, active_list[i]);
		}
	}

} /* dump_list_info() */
#endif




/*
 * NAME: 
 *	queue_async()
 *
 * DESCRIPTION:
 *	This function will take a request structure and queue it onto
 *	the fdte async request queue.  Lock is used around access to the
 *	request queue.  If this is the first async request for this open
 *	file then an async fdte lock structure is allocated along and
 *	async request thread started.
 *
 * PARAMETERS:
 *	req_ptr         - pointer to the request structure to be queued.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
queue_async(req_ptr)
async_req *req_ptr;
{
	async_fdte_queue *fdte_queue_ptr;
	register int	status;
	kern_return_t	rc;

#ifdef DEBUG_ASYNC
	async_req	*tmp_ptr;

#endif


	PFS_TRACE(("  queue_async: req_ptr %X\n", req_ptr));

	/* verify fdte lock has been allocated */
	fdte_queue_ptr = ((fdt_entry_t *)(req_ptr->fdte))->async_queue;
#ifdef DEBUG_ASYNC
	e_printf("queue_async: req %X fdte %X lock %X\n", 
		 req_ptr, req_ptr->fdte, fdte_queue_ptr);
#endif
	if (fdte_queue_ptr == NULL) {
		/* get control of thread lock */
		spin_lock(&thread_list_lock);

		/* if no free threads available, allocate more */
		if (!num_free_threads) {
			if (status = init_threads())
				return status;
		}

		/* pull off free thread */
		((fdt_entry_t *)(req_ptr->fdte))->async_queue = 
					&free_thread_list[next_free_thread];
		fdte_queue_ptr = ((fdt_entry_t *)(req_ptr->fdte))->async_queue;
		((async_fdte_queue *)&free_thread_list[next_free_thread])->state
								= ASYNC_BUSY;
		num_free_threads--;
		next_free_thread--;

		/* locate the next available thread structure */
		if (num_free_threads)
			for (;;) {
				if (((async_fdte_queue *)
				    &free_thread_list[next_free_thread])->state
				    == ASYNC_BUSY)
					next_free_thread--;
				else
					break;
			}

		/* get control of wait queue lock */
		spin_lock(&fdte_queue_ptr->lock);

		/* make call to start async thread */
		if (status = start_thread(req_ptr->fdte))
			return status;

		/* free control of thread lock */
		spin_unlock(&thread_list_lock);
	} else {
		/* get control of wait queue lock */
		spin_lock(&fdte_queue_ptr->lock);
	}

	/* get onto the queue */
	if (fdte_queue_ptr->head == NULL) {
		/* no other ART on queue, become start of queue 
		   and owner of fdte */
		fdte_queue_ptr->head = req_ptr;
	} else {
		/* go to end of the queue and wait to become head */
		fdte_queue_ptr->tail->next = req_ptr;
	}
	fdte_queue_ptr->tail = req_ptr;
	req_ptr->async_thread = fdte_queue_ptr->thread;

#ifdef DEBUG_ASYNC
	tmp_ptr = fdte_queue_ptr->head;
	while (tmp_ptr != NULL) {
		e_printf("H %X->", tmp_ptr);
		if (tmp_ptr->next == NULL && tmp_ptr != fdte_queue_ptr->tail)
			e_printf("Tail bad, %x", fdte_queue_ptr->tail);
		tmp_ptr = tmp_ptr->next;
	}
	e_printf("\n");
#endif
		
	/* release control of wait queue lock */
	spin_unlock(&fdte_queue_ptr->lock);

        /* swap processor control over to ART for it to get started */
	rc = thread_switch(fdte_queue_ptr->thread,
			   SWITCH_OPTION_DEPRESS, min_wait_time);
	if (rc != KERN_SUCCESS)
		return EIO;

	return ESUCCESS;

} /* end queue_async() */




/*
 * NAME: 
 *	dequeue_sync()
 *
 * DESCRIPTION:
 *	This function will remove the dummy synchronous request structure
 *	from the fdte async request queue.
 *
 * PARAMETERS:
 *	fdte_queue_ptr  - pointer to the async lock structure associated
 *			  with the fdte.
 *	dummy_req_ptr   - pointer for the dummy structure to be removed.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
void
dequeue_sync(fdte_queue_ptr, dummy_req_ptr)
async_fdte_queue	*fdte_queue_ptr;
async_req	*dummy_req_ptr;
{

	PFS_TRACE(("  dequeue_sync: fdte_queue_ptr %X dummy_req_ptr %X\n",
		fdte_queue_ptr, dummy_req_ptr));

	/* get control of wait queue lock */
	spin_lock(&fdte_queue_ptr->lock);

	/* verify we are still the head to be able to free the fdte */
	EASSERT(fdte_queue_ptr->head == dummy_req_ptr);
	fdte_queue_ptr->head = dummy_req_ptr->next;
	if (fdte_queue_ptr->head == NULL)
		/* no one else in the queue, so nobody to signal */
		fdte_queue_ptr->tail = NULL;

	/* release control of wait queue lock */
	spin_unlock(&fdte_queue_ptr->lock);

	return; 

} /* end dequeue_sync() */




/*
 * NAME:
 *	queue_sync()
 *
 * DESCRIPTION:
 *	This procedure will place a synchronous request into the
 *	asynchronous fdte request queue.  This is only required
 *	if asynchronous requests are being used on this file.  It
 *	is assumed that this is checked before calling this procedure.
 *	A dummy request block is used as a placeholder in the queue
 *	for the synchronous request.  The calling thread is blocked
 * 	until the dummy request becomes the head of the queue.  The
 *	calling thread will then execute the sync request, not the ART.
 *
 * PARAMETERS:
 *	fdte_queue_ptr - pointer to the async lock structure associated
 *			 with this fdte.
 *	dummy_req_ptr  - pointer to a dummy stack based request structure.
 *
 *	queued         - pointer to variable used to indicate whether
 *			 queued or not. 
 *
 * RETURNS:
 *	0 	       - if successful.
 * 	!0	       - if failed, returns errno.
 */
int
queue_sync(fdte_queue_ptr, dummy_req_ptr, queued)
async_fdte_queue		*fdte_queue_ptr;
async_req		*dummy_req_ptr;
int			*queued;
{
	kern_return_t	rc;
	int		error=ESUCCESS;

#ifdef DEBUG_ASYNC
	async_req	*tmp_ptr;

#endif


	PFS_TRACE(("  queue_sync: fdte_queue_ptr %X dummy_req_ptr %X queued %X\n",
		fdte_queue_ptr, dummy_req_ptr, queued));

	/* only attempt queuing request if async I/O has been started */
	EASSERT(fdte_queue_ptr != NULL);

	/* get control of wait queue lock */
	spin_lock(&fdte_queue_ptr->lock);


	/* If any requests queued must put sync request onto queue.
	   Install request onto queue but wait for request to become 
	   head of wait queue */
	if (fdte_queue_ptr->head == NULL) {
		*queued = FALSE;
		spin_unlock(&fdte_queue_ptr->lock);
		return error;
	}
	dummy_req_ptr->state = ISYNC;
	dummy_req_ptr->next = NULL;
	dummy_req_ptr->async_errno = ESUCCESS;
	fdte_queue_ptr->tail->next = dummy_req_ptr;
	fdte_queue_ptr->tail = dummy_req_ptr;

#ifdef DEBUG_ASYNC
	tmp_ptr = fdte_queue_ptr->head;
	while (tmp_ptr != NULL) {
		e_printf("H %X->", tmp_ptr);
		if (tmp_ptr->next == NULL && tmp_ptr != fdte_queue_ptr->tail)
			e_printf("Tail bad, %x", fdte_queue_ptr->tail);
		tmp_ptr = tmp_ptr->next;
	}
	e_printf("\n");
#endif

	while (fdte_queue_ptr->head != dummy_req_ptr) {
		spin_unlock(&fdte_queue_ptr->lock);
		rc = thread_switch(fdte_queue_ptr->thread,
				   SWITCH_OPTION_DEPRESS, min_wait_time);
		if (rc != KERN_SUCCESS)
			return EIO;
		if (must_suspend)
			syscall_suspend_barrier();

		spin_lock(&fdte_queue_ptr->lock);
	}

	/*
	 * Check to see if interrupted:
	 */
	if (dummy_req_ptr->async_errno) {
		error = dummy_req_ptr->async_errno;
	}

	/* release control of wait queue lock */
	spin_unlock(&fdte_queue_ptr->lock);

	*queued = 1;
	return error;

} /* end queue_sync() */




/*
 * NAME:
 *	async_scheduler()
 *
 * DESCRIPTION:
 *	This module is executed by the asynchronous request threads.
 *	Here request entries are pulled off the fdte wait queue, maintained
 *	in the async_fdte_queue, and processed.  If the head of the queue
 *	is a synchronous request this code will wait for that request to
 *	be processed by some other sync thread and removed from the queue.
 *	
 * PARAMETERS:
 *	fdte - pointer to this threads target file descriptor table entry.
 *
 * RETURNS:
 *	None
 */
void
async_scheduler(fdte)
fdt_entry_t	*fdte;
{
	int			save_errno;
	async_req		*req_ptr;
	async_fdte_queue	*fdte_queue_ptr;
	transaction_id_t	transid;
	kern_return_t		rc;

	req_ptr = NULL;
	fdte_queue_ptr = fdte->async_queue;

	/*
	 * Register this asynchronous thread in the isc table
	 * so that the callback thread can find it if an 
	 * interrupt occurs.  The thread will be deregistered
	 * when the thread goes away via an fdte_unref().
	 */

	isc_async_register(&transid, &fdte_queue_ptr->interrupt);
#ifdef DEBUG_ISC
	e_printf("start: fdte = %x\n", fdte); 
#endif

#ifdef  MAP_UAREA
	/*
	 * Inform the server that this thread is in the emulator
	 * data space.  This will allow the server to go through the 
	 * appropriate signal so that the task is made aware of signals.
	 */
	if (shared_enabled) {
		spin_lock(&in_emulator_lock);
		shared_base_rw->us_in_emulator++;
		spin_unlock(&in_emulator_lock);
	}
#endif
	for (;;) {

		/* get control of wait queue lock */
		spin_lock(&fdte_queue_ptr->lock);

		/* Spin waiting for next asynchronous request to
		 * get processed. 
		 */
		while ((fdte_queue_ptr->head == NULL) || 
		       (fdte_queue_ptr->head->state == ISYNC)) {
			spin_unlock(&fdte_queue_ptr->lock);
			rc = thread_switch(0, SWITCH_OPTION_DEPRESS,
					   min_wait_time);
			EASSERT(rc == KERN_SUCCESS);

			/*
			 * See if we have been interrupted and must suspend.
			 */
			if (must_suspend) {
				syscall_suspend_barrier();
			} /* if (must_suspend) */
			spin_lock(&fdte_queue_ptr->lock);
		} /* end while */

		req_ptr = fdte_queue_ptr->head;

		/* give up control of wait queue lock */
		spin_unlock(&fdte_queue_ptr->lock);

		switch (req_ptr->state) {
			case IREAD:
				save_errno = 
					sync_read(req_ptr->proc_port,
						req_ptr->interrupt,
						req_ptr->fdte, 
						req_ptr->buffer,
						req_ptr->nbytes,
						&req_ptr->actual);
				break;
			case IREADV:
				save_errno = 
					sync_readv(req_ptr->proc_port,
						req_ptr->interrupt,
						req_ptr->fdte, 
						(struct iovec *)req_ptr->buffer,
						req_ptr->nbytes,
						&req_ptr->actual);
				break;
			case IWRITE:
				save_errno = 
					sync_write(req_ptr->proc_port,
						req_ptr->interrupt,
						req_ptr->fdte, 
						req_ptr->buffer,
						req_ptr->nbytes,
						&req_ptr->actual);
				break;
			case IWRITEV:
				save_errno = 
					sync_writev(req_ptr->proc_port,
						req_ptr->interrupt,
						req_ptr->fdte, 
						(struct iovec *)req_ptr->buffer,
						req_ptr->nbytes,
						&req_ptr->actual);
				break;
			default:
                		emul_panic(
				       "async_scheduler: invalid request type");
				break;
		} /* end case req_ptr->state */

		/* lock request before pulling it off the fdte queue and 
		   updating request results */
		spin_lock(&req_ptr->async_lock);

		/* remove processed entry from queue */
		spin_lock(&fdte_queue_ptr->lock);
		EASSERT(req_ptr == fdte_queue_ptr->head);
		fdte_queue_ptr->head = fdte_queue_ptr->head->next;
		/*
		 * See if we were interrupted in the middle 
		 * of the operation:
		 */
		if (fdte_queue_ptr->interrupt) {
			async_req  *next_req_ptr; 
			async_req  *sync_req_ptr = NULL;
			async_req  *sync_req_head = NULL;
			req_ptr->async_errno = EINTR;
			fdte_queue_ptr->interrupt = 0;
			/*
			 * Clear the queue of any outstanding asynchronous
			 * requests:
			 */
			while((next_req_ptr = fdte_queue_ptr->head) != NULL) {
#ifdef DEBUG_ISC
	e_printf("fdte[%x] Dequeueing next_req_ptr = %x, state = %x\n",
		 next_req_ptr->fdte, next_req_ptr, next_req_ptr->state);
#endif
				if (next_req_ptr->state ==  ISYNC) {
#ifdef DEBUG_ISC
	e_printf("fdte[%x] State indicates a synchronous request\n",
		 next_req_ptr->fdte);
#endif
					if (sync_req_ptr) {
					   sync_req_ptr->next = next_req_ptr;
					} else  {
					   sync_req_head = next_req_ptr;
					}
					sync_req_ptr = next_req_ptr;
				} else {
					next_req_ptr->state |= IDONE;
				}

				next_req_ptr->async_errno = EINTR;
				next_req_ptr->actual = 0;
				fdte_queue_ptr->head = next_req_ptr->next; 

			} /* end while (fdte_queue_ptr->head != NULL) */
			/*
			 * If there were any synchronous requests in the 
			 * queue, put them back on the head of the 
			 * queue so that the synchronous thread finds
			 * them.
			 */
			if (sync_req_head != NULL) { 
#ifdef DEBUG_ISC
	e_printf("fdte[%x]Had a sync_req_ptr\n",
		  sync_req_head->fdte);
#endif
				fdte_queue_ptr->head = sync_req_head; 
				sync_req_ptr->next = NULL;
			} /* end if sync_req_head */
				
		} /* end if(async_interrupt) */


		spin_unlock(&fdte_queue_ptr->lock);

		/* save the async errno and mark request complete
		   for async_wait or async_done */
		if (req_ptr->actual < 0)
			req_ptr->async_errno = save_errno;

#ifdef DEBUG_ISC
	if(req_ptr->async_errno) {
		e_printf("fdte[%x]Result of the i/o (errno) = %d\n", 
			req_ptr->fdte,
			req_ptr->async_errno);
	}

#endif
		spin_unlock(&req_ptr->async_lock);

		/*
		 * unreference the fdte entry for the last request.
		 * It is done here because we can't count on user
		 * calling iowait or iodone to get results.
		 */
		(void) fdt_unref_entry(fdte);

		/*
		 * Finally, set the IODONE flag in the async request
		 * structure.  This must be done after the call to
		 * fdt_unref_entry()  because of a timing window
		 * (PTS #11529).
		 */
		spin_lock(&req_ptr->async_lock);
		req_ptr->state |= IDONE;
		spin_unlock(&req_ptr->async_lock);

		/*
		 * suspension barrier:
		 */
		if (must_suspend)
			syscall_suspend_barrier();
	} /* end of async scheduler process loop */

} /* async_scheduler */




/*
 * NAME:        
 *	start_thread()
 *
 * DESCRIPTION:
 *	This procedure will set the state for an asynchronous thread and
 *	start it up.
 *
 * PARAMETERS:
 *	fdte            - pointer to new threads target file descriptor
 *                        table entry.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
start_thread(fdte)
fdt_entry_t	*fdte;
{
	register long	i, *p;
	struct i860_thread_state 	state;
	kern_return_t	rc;

	PFS_TRACE(("  start_thread: fdte %X\n", fdte));

	/* setup the state for the asynchronus request thread (ART) */
	for (p = (long *) &state, i=0; i<i860_THREAD_STATE_COUNT; i++)
		p[i] = 0;
	state.r16 = (int)fdte;
	state.psr = (unsigned long) PSR_U | PSR_PU | PSR_IM | PSR_PIM;
	state.pc = (unsigned long) &async_scheduler;
	state.sp = (unsigned long) fdte->async_queue->stack;
	state.fp = 0;	/* clear frame ptr */

	/*
	* Start the asynchronous I/O request thread (ART)
	*/
#ifdef DEBUG_ASYNC
	e_printf("thread_set_state, sp = %X\n", fdte->async_queue->stack);
#endif
	if ((rc = thread_set_state(
			fdte->async_queue->thread,
			i860_THREAD_STATE,
			(thread_state_t)&state,
			i860_THREAD_STATE_COUNT)) != KERN_SUCCESS) {
#ifdef DEBUG_ASYNC
		e_printf("thread_set_state returns %x\n", rc);
#endif
		return EIO;
	}

	/* start the ART */
	if ((rc = thread_resume(fdte->async_queue->thread)) != KERN_SUCCESS) {
#ifdef DEBUG_ASYNC
		e_printf("thread_resume returns %x\n", rc);
#endif
		return EIO;
	}

	return ESUCCESS;

} /* start_thread() */




/*
 * NAME:        
 *	async_read()
 *
 * DESCRIPTION:
 *	This procedure will get an async request structure to represent
 *	this request, initilize request specifics into the structure,
 *	call a procedure that will queue the request structure onto the
 *	fdte wait queue for the target file, and finally return the
 *	unique id for the posted async request.
 *
 * PARAMETERS:
 *	proc_port       - pass through interface parameter not used.
 *	interrupt       - pass through interface parameter not used.
 *	fdt_index       - index into the fdt table for the file to be read.
 *	buffer          - pointer to a user defined buffer to receive the
 *                        data read.
 *	nbytes          - number of bytes to be read.
 *	rval	        - pointer to location to set return unique request
 *                        id value.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
async_read(proc_port, interrupt, fdt_index, buffer, nbytes, rval)
mach_port_t	proc_port;
int		*interrupt;	/* out */
int             fdt_index;	/* File descriptor table index */
char            *buffer;        /* Pointer to the destination buffer. */
unsigned int    nbytes;         /* Number of bytes to read. */
int		*rval;		/* out */
{
	int			new_req_id;
	async_req		*new_req;
	fdt_entry_t		*fdte;
	int			status;

	PFS_TRACE(("  async_read: proc_port %X interrupt %X fdt_index %d buffer %X nbytes %d rval %X\n",
		proc_port, interrupt, fdt_index, buffer, nbytes, rval));

	/* get an async_req structure to represent request */
	if (status = get_async(&new_req_id))
		return status;
	new_req = active_list[new_req_id];
	rval[0] = new_req_id;

	/* reference the file descriptor table entry */
	if (status = fdt_ref_entry(fdt_index, &fdte))
		return status;

	/* initialize request specific fields within async_req */
	new_req->proc_port = proc_port;
	new_req->interrupt = interrupt;
	new_req->fdte = (int)fdte;
	new_req->buffer = buffer;
	new_req->nbytes = nbytes;
	new_req->state = IREAD; 

	/* queue request onto scheduling queue */
	if (status = queue_async(new_req))
		return status;

	return ESUCCESS;

} /* end async_read() */




/*
 * NAME:        
 *	async_readv()
 *
 * DESCRIPTION:
 *	This procedure will get an async request structure to represent
 *	this request, initilize request specifics into the structure,
 *	call a procedure that will queue the request structure onto the
 *	fdte wait queue for the target file, and finally return the
 *	unique id for the posted async request.
 *
 * PARAMETERS:
 *	proc_port       - pass through interface parameter not used.
 *	interrupt       - pass through interface parameter not used.
 *	fdt_index       - index into the fdt table for the file to be read.
 *	iov             - pointer to list of user defined buffer areas to
 *                        receive data read.
 *	iovcount        - number of buffer areas to read into.
 *	rval            - pointer to location to set return unique request
 *                        id value.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
async_readv(proc_port, interrupt, fdt_index, iov, iovcount, rval)
mach_port_t	proc_port;
int		*interrupt;	/* out */
int             fdt_index;	/* File descriptor table index */
struct iovec    *iov;		/* Pointer to the segment list. */
unsigned int    iovcount;	/* Number of segments to read. */
int		*rval;		/* out */
{
	int			new_req_id;
	async_req		*new_req;
	fdt_entry_t		*fdte;
	int			status;

	PFS_TRACE(("  async_readv: proc_port %X interrupt %X fdt_index %d iov %X iovcount %d rval %X\n",
		proc_port, interrupt, fdt_index, iov, iovcount, rval));

	/* get an async_req structure to represent request */
	if (status = get_async(&new_req_id))
		return status;
	new_req = active_list[new_req_id];
	rval[0] = new_req_id;

	/* reference the file descriptor table entry */
	if (status = fdt_ref_entry(fdt_index, &fdte))
		return status;

	/* initialize request specific fields within async_req */
	new_req->proc_port = proc_port;
	new_req->interrupt = interrupt;
	new_req->fdte = (int)fdte;
	new_req->buffer = (char *)iov;
	new_req->nbytes = iovcount;
	new_req->state = IREADV; 

	/* queue request onto scheduling queue */
	if (status = queue_async(new_req))
		return status;

	return ESUCCESS;

} /* end async_readv() */




/*
 * NAME:        
 *	async_done()
 *
 * DESCRIPTION:
 *	This procedure is used to sample for an asynchronous request
 *	results.  The request is complete when the state flag of the
 *	async request structure indicates IDONE.  The caller tests the
 *	flag, returning the results if the request is complete, or returning
 *	-1 if the request has not yet completed. The caller provides a 
 *	unique id used to locate the specific request from within the 
 *	active list of asynchronous requests.
 *
 * PARAMETERS:
 *	bsd_serv_port   - interface parameter, not used.
 *	interrupt       - interface parameter, not used.
 *	async_req_id    - unique asynchronous request identifier.
 *	rval            - pointer to area to return actual number of
 *                        bytes transferred or -1 indicating
 *                        the request is not complete.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
async_done(bsd_serv_port, interrupt, async_req_id, rval)
mach_port_t     bsd_serv_port;
boolean_t       *interrupt;     /* OUT */
unsigned int	async_req_id;
int             *rval;          /* OUT */
{
	int status, save_errno;
	async_req	*req_ptr;
	long		complete;

	PFS_TRACE(("  async_done: bsd_serv_port %X interrupt %X async_req_id %d rval %X\n",
		bsd_serv_port, interrupt, async_req_id, rval));

	/* verify index is within valid range */
	if (async_req_id > active_lst_size)
		return EBADID;

	/* locate async_req structure */
	req_ptr = active_list[async_req_id];
	if (req_ptr == NULL)
		return EBADID;

	/* get control of the structure lock in order to read state */
	spin_lock(&req_ptr->async_lock);

	/* sample state field for complete bit turned on */
	complete = req_ptr->state & IDONE;

	/* free control of the structure lock */
	spin_unlock(&req_ptr->async_lock);

	/* if request was complete free the request structure */
	if (complete) {
		save_errno = req_ptr->async_errno;
		rval[0] = req_ptr->actual;
		if (status = free_async(req_ptr))
			return status;
		if (save_errno)
			return save_errno; 
		return ESUCCESS;
	}

	/* request is not complete */
	rval[0] = -1;
	return ESUCCESS;

} /* end async_done() */




/*
 * NAME:        
 *	async_wait()
 *
 * DESCRIPTION:
 *	This procedure is used to wait for an asynchronous request to
 *	complete.  The request is complete when the state flag of the
 *	async request structure indicates IDONE.  The caller is blocked
 *	during this time.  When the request is complete the results are
 *	pulled from the request structure and the structure is then
 *	returned to the free pool.  The caller provides a unique id used
 *	to locate the specific request from within the active list of
 *	asynchronous requests.
 *
 * PARAMETERS:
 *	bsd_serv_port   - interface parameter, not used.
 *	interrupt       - interface parameter, not used.
 *	async_req_id    - unique asynchronous request identifier.
 *	rval            - pointer to area to return actual number of
 *                        bytes transferred.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
async_wait(bsd_serv_port, interrupt, async_req_id, rval)
mach_port_t     bsd_serv_port;
boolean_t       *interrupt;     /* OUT */
unsigned int	async_req_id;
int             *rval;          /* OUT */
{
	kern_return_t	rc;
	int status, save_errno;
	async_req	*req_ptr;

	PFS_TRACE(("  async_wait: bsd_serv_port %X interrupt %X async_req_id %d rval %X\n",
		bsd_serv_port, interrupt, async_req_id, rval));

	/* verify index is within valid range */
	if (async_req_id > active_lst_size)
		return EBADID;

	/* locate async_req structure */
	req_ptr = active_list[async_req_id];
	if (req_ptr == NULL)
		return EBADID;

	/* get control of the structure lock in order to read state */
	spin_lock(&req_ptr->async_lock);

	/* if request is not done we will wait for it to complete */
	while ((req_ptr->state & IDONE) != IDONE) {
#ifdef DEBUG_ASYNC
		e_printf("async_wait: wait on req #%d\n", req_ptr->async_id);
#endif
		spin_unlock(&req_ptr->async_lock);
		rc = thread_switch(req_ptr->async_thread, 
				   SWITCH_OPTION_DEPRESS, min_wait_time);
		if (must_suspend) {
			syscall_suspend_barrier();
		}
		spin_lock(&req_ptr->async_lock);
	}
	rval[0] = req_ptr->actual;
	save_errno = req_ptr->async_errno;

	/* free control of the structure lock */
	spin_unlock(&req_ptr->async_lock);

	/* free the request structure */
	if (status = free_async(req_ptr))
		return status;
	if (save_errno)
		return save_errno; 
	return ESUCCESS;

} /* end async_wait() */




/*
 * NAME:        
 *	async_write()
 *
 * DESCRIPTION:
 *	This procedure will get an async request structure to represent
 *	this request, initilize request specifics into the structure,
 *	call a procedure that will queue the request structure onto the
 *	fdte wait queue for the target file, and finally return the
 *	unique id for the posted async request.
 *
 * PARAMETERS:
 *	proc_port       - pass through interface parameter not used.
 *	interrupt       - pass through interface parameter not used.
 *	fdt_index       - index into the fdt table for the file to be written.
 *	buffer          - pointer to a user defined buffer to write data from.
 *	nbytes          - number of bytes to be read.
 *	rval            - pointer to location to set return unique
 *                        request id value.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
async_write(proc_port, interrupt, fdt_index, buffer, nbytes, rval)
mach_port_t     proc_port;
int             *interrupt;     /* out */
int             fdt_index;	/* File descriptor. */
char            *buffer;	/* Pointer to the destination buffer. */
unsigned int    nbytes;		/* Number of bytes to write. */
int             *rval;          /* out */
{
	async_req		*new_req;
	int			new_req_id;
        fdt_entry_t             *fdte;
        int                     status;

	PFS_TRACE(("  async_write: proc_port %X interrupt %X fdt_index %d buffer %X nbytes %d rval %X\n",
		proc_port, interrupt, fdt_index, buffer, nbytes, rval));

	/* get an async_req structure to represent request */
	if (status = get_async(&new_req_id))
		return status;
	new_req = active_list[new_req_id];
	rval[0] = new_req_id;

	/* reference the file descriptor table entry */
	if (status = fdt_ref_entry(fdt_index, &fdte))
		return status;

	/* initialize request specific fields within async_req */
	new_req->proc_port = proc_port;
	new_req->interrupt = interrupt;
	new_req->fdte = (int)fdte;
	new_req->buffer = buffer;
	new_req->nbytes = nbytes;
	new_req->state = IWRITE; 

	/* queue request onto scheduling queue */
	if (status = queue_async(new_req))
		return status;

	return ESUCCESS;

} /* end async_write() */




/*
 * NAME:        
 *	async_writev()
 *
 * DESCRIPTION:
 *	This procedure will get an async request structure to represent
 *	this request, initilize request specifics into the structure,
 *	call a procedure that will queue the request structure onto the
 *	fdte wait queue for the target file, and finally return the
 *	unique id for the posted async request.
 *
 * PARAMETERS:
 *	proc_port       - pass through interface parameter not used.
 *	interrupt       - pass through interface parameter not used.
 *	fdt_index       - index into the fdt table for the file to be written.
 *	iov             - pointer to list of user defined buffer areas to pull
 *                        data from.
 *	iovcount        - number of buffer areas to write from.
 *	rval            - pointer to location to set return unique
 *                        request id value.
 *
 * RETURNS:
 *      ESUCCESS        - if successful
 *      error number	- if an error occurred.
 */
int
async_writev(proc_port, interrupt, fdt_index, iov, iovcount, rval)
mach_port_t     proc_port;
int             *interrupt;     /* out */
int             fdt_index;		/* File descriptor. */
struct iovec    *iov;    /* Pointer to the destination buffer. */
unsigned int    iovcount;     /* Number of bytes to write. */
int             *rval;          /* out */
{
	async_req		*new_req;
	int			new_req_id;
        fdt_entry_t             *fdte;
        int                     status;

	PFS_TRACE(("  async_writev: proc_port %X interrupt %X fdt_index %d iov % X iovcount %d rval %X\n",
		proc_port, interrupt, fdt_index, iov, iovcount, rval));

	/* get an async_req structure to represent request */
	if (status = get_async(&new_req_id))
		return status;
	new_req = active_list[new_req_id];
	rval[0] = new_req_id;

	/* reference the file descriptor table entry */
	if (status = fdt_ref_entry(fdt_index, &fdte))
		return status;

	/* initialize request specific fields within async_req */
	new_req->proc_port = proc_port;
	new_req->interrupt = interrupt;
	new_req->fdte = (int)fdte;
	new_req->buffer = (char *)iov;
	new_req->nbytes = iovcount;
	new_req->state = IWRITEV; 

	/* queue request onto scheduling queue */
	if (status = queue_async(new_req))
		return status;

	return ESUCCESS;

} /* end async_writev() */

#endif PFS
