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
 * $Log: kern_task.c,v $
 * Revision 1.11  1994/11/18  20:56:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/07/12  19:24:48  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9  1994/05/25  19:19:41  stefan
 * Modified function task_copy_vm() so that VM_INHERIT_SHARE works for external
 * memory objects. This allows processes with attached shared memory segments
 * to be rfork()ed to remote nodes.  [Developed by Stefan Zeisset (sz)].
 *
 *
 *  Reviewer: stefan, sz
 *  Risk: low - functionality only used for shared memory segments
 *  Benefit or PTS #: VM_INHERIT_SHARE now works for external memory objects
 *  Testing: developer testing
 *  Module(s): kern_task.c
 *
 * Revision 1.8  1994/03/19  01:30:46  lenb
 * Benefit: MP locking here needs work.
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 1.7.4.2  1994/06/22  18:21:38  stans
 *    In norma_task_create() skip the call to task_get_inherited_ports() as
 *    the call placed extra send-right references on the inherited ports.
 *    Since the inherited bootstrap port is the only task specific port (will
 *    be destroyed) it was never released due to the extra reference.
 *
 * Revision 1.7.4.1  1994/03/11  23:01:57  andyp
 * Corrected the use of varargs to be more portable.
 *
 * Revision 1.7  1993/11/06  05:21:44  stans
 *  In 'norma_task_create()', 'new_task' is a placeholder task for the benefit of
 *  convert_task_to_port(). Make sure the IPC task lock is initialized so
 *  'convert_task_to_port()' to functions correctly; doesn't timeout the lock.
 *  Added a call to 'itk_lock_init(new_task);' for task IPC initialization.
 *
 *  Reviewer: none
 *  Risk: none for R1.2, medium for 1.3
 *  Benefit or PTS #: Allow parallel pgms to run on an MP3 kernel (uniprocessor).
 *  Testing: booted and run parallel pgms never run before.
 *
 * Revision 1.6  1993/09/30  17:36:10  stans
 *    More items under MACH_ASSERT; feel the need for speed!!
 *    MP3: bootstrap port is still locked during set special port for child.
 * 	Force an unlock!
 *
 * Revision 1.5  1993/07/22  02:21:03  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:51:05  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:15  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:19:50  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/16  05:15:04  SSD
 * third code drop for page flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: kern_task.c,v
 * Revision 1.2.4.2  1993/04/16  16:09:43  mmp
 * 	release reference acquired by vm_region.
 * 	[93/04/15            sjs]
 *
 * Revision 1.2  1992/11/25  01:15:11  robert
 * 	fix history
 * 	[1992/11/09  21:58:59  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:47:47  robert]
 * 
 * Revision 0.0  92/10/23            dwm
 * 	Terminate remote_task in norma_task_create() error path,
 * 	to avoid leaving remote orphans hanging around, and avoid
 * 	returning random MiG errors.  (#471, 475).
 * 	Rearrange task_create() et al. to prevent user code from
 * 	being able to pass in a null parent.	(#478)
 * 
 * 	Revision 1.1  1992/11/05  21:00:06  robert
 * 	Initial revision
 * 	[92/10/23            dwm]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.7.2.7  92/09/15  17:34:52  jeffreyh
 * 	When inheriting memory in task_copy_vm, set protection in
 * 	locally forked map to max.  This deals with regions that are
 * 		currently unreadable but could become readable in the future.
 * 	Don't set up cross node copy logic for permanently unreadable
 * 	regions.  Don't copy read-only regions.
 * 	[92/08/14            dlb]
 * 
 * 	Add bounds checking on the node to task_set_child_node
 * 	and norma_task_create.
 * 	[92/07/23            jeffreyh]
 * 
 * Revision 2.7.2.6  92/05/28  18:20:32  jeffreyh
 * 	Add new type argument to remote_host call
 * 
 * Revision 2.7.2.5  92/05/27  00:53:43  jeffreyh
 * 	norma_task_create inits mcmsg_task in MCMSG ifdef
 * 	[regnier@ssd.intel.com]
 * 
 * Revision 2.7.2.4  92/03/04  16:07:16  jeffreyh
 * 	Converted to use out-of-line forms of task_{get,set}_emulation_vector.
 * 	[92/03/04  14:55:51  jsb]
 * 
 * Revision 2.7.2.3  92/02/21  11:25:15  jsb
 * 	Release send right to memory_object after mapping it in remote task
 * 	in task_copy_vm.
 * 	[92/02/20  10:29:45  jsb]
 * 
 * 	Set copy flag TRUE in r_vm_map call in task_copy_vm.
 * 	This is now practical due to smarter copy strategy management
 * 	in xmm_svm (compared to when it always used MEMORY_OBJECT_COPY_NONE).
 * 	It also keeps xmm_copy from having to deal with data writes without
 * 	having to use an xmm_shadow layer.
 * 	[92/02/11  11:39:23  jsb]
 * 
 * 	Release map reference in task_copy_vm.
 * 	[92/01/22  10:29:57  jsb]
 * 
 * Revision 2.7.2.2  92/01/09  18:46:08  jsb
 * 	Added logic in task_create to alternate task creation between local
 * 	node and a patchable remote node. For testing purposes only.
 * 	[92/01/08  16:41:27  jsb]
 * 
 * 	Use varargs for debugging printfs.
 * 	[92/01/08  10:22:12  jsb]
 * 
 * 	Use remote_host() instead of norma_get_special_port().
 * 	[92/01/04  18:17:46  jsb]
 * 
 * Revision 2.7.2.1  92/01/03  16:38:29  jsb
 * 	Removed unused routine ipc_task_reinit.
 * 	[91/12/28  17:59:18  jsb]
 * 
 * Revision 2.7  91/12/13  13:53:22  jsb
 * 	Changed name of task_create_remote to norma_task_create.
 * 	Added check for local case in norma_task_create.
 * 
 * Revision 2.6  91/12/10  13:26:19  jsb
 * 	Changed printfs to frets.
 * 	[91/12/10  11:34:35  jsb]
 * 
 * Revision 2.5  91/11/14  16:51:51  rpd
 * 	Use new child_node task field in place of task_server_node to decide
 * 	upon what node to create child task. Also add task_set_child_node().
 * 	[91/09/23  09:22:18  jsb]
 * 
 * Revision 2.4  91/08/28  11:16:18  jsb
 * 	Turned off remaining printf.
 * 	[91/08/26  11:16:11  jsb]
 * 
 * 	Added support for remote task creation with inherited memory.
 * 	Remove task creation is accessible either via task_create_remote,
 * 	or task_create with task_server_node patched to some reasonable value.
 * 	[91/08/15  13:55:54  jsb]
 * 
 * Revision 2.3  91/06/17  15:48:07  jsb
 * 	Moved routines here from kern/ipc_tt.c and kern/task.c.
 * 	[91/06/17  11:01:51  jsb]
 * 
 * Revision 2.2  91/06/06  17:08:15  jsb
 * 	First checkin.
 * 	[91/05/25  11:47:39  jsb]
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
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	norma/kern_task.c
 *	Author:	Joseph S. Barrera III
 *
 *	NORMA task support.
 */

#include <norma_task.h>
#include <mach_assert.h>

#include <mach/machine/vm_types.h>
#include <mach/vm_param.h>
#include <mach/task_info.h>
#include <mach/task_special_ports.h>
#include <ipc/ipc_space.h>
#include <kern/mach_param.h>
#include <kern/task.h>
#include <kern/host.h>
#include <kern/thread.h>
#include <kern/zalloc.h>
#include <kern/kalloc.h>
#include <kern/processor.h>
#include <kern/ipc_tt.h>
#include <sys/varargs.h>
#include <norma/ipc_node.h>

ipc_port_t norma_task_server;
decl_simple_lock_data(,norma_task_server_lock)

extern zone_t task_zone;

/*
 * XXX This definition should be elsewhere
 */
norma_node_self(host, node)
	host_t host;
	int *node;
{
	*node = node_self();
	return KERN_SUCCESS;
}

#if	NORMA_TASK

int fff = 1;
int mmm = 0;
int bbb = 0;
extern cnputc();

fret(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (fff) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

#if	MACH_ASSERT

mumble(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (mmm) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}

babble(va_alist)
	va_dcl
{
	va_list	listp;
	char	*fmt;

	if (bbb) {
		va_start(listp);
		fmt = va_arg(listp, char *);
		_doprnt(fmt, &listp, cnputc, 0);
		va_end(listp);
	}
}
#endif	/* MACH_ASSERT */

int task_create_remote_node = -1;
boolean_t task_create_use_remote = FALSE;

kern_return_t task_create(parent_task, inherit_memory, child_task)
	task_t		parent_task;
	boolean_t	inherit_memory;
	task_t		*child_task;		/* OUT */
{
	if (parent_task == TASK_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (task_create_remote_node != -1) {
		task_create_use_remote = ! task_create_use_remote;
		if (task_create_use_remote) {
			return norma_task_create(parent_task, inherit_memory,
						 task_create_remote_node,
						 child_task);
		} else {
			return task_create_local(parent_task, inherit_memory,
						 child_task);
		}
	}
	if (parent_task->child_node == -1 ) {
		return task_create_local(parent_task, inherit_memory,
					 child_task);
	} else {
		return norma_task_create(parent_task, inherit_memory,
					 parent_task->child_node,
					 child_task);
	}
}

kern_return_t task_set_child_node(task, child_node)
	task_t	task;
	int	child_node;
{
	if (task == TASK_NULL || !node_is_valid (child_node)) {
		return KERN_INVALID_ARGUMENT;
	} else {
		task->child_node = child_node;
		return KERN_SUCCESS;
	}
}

/*
 * This allows us to create a task without providing a parent.
 */
kern_return_t norma_task_allocate(host, task)
	host_t	host;
	task_t	*task;		/* OUT */
{
#if     MACH_ASSERT
	babble("norma_task_allocate: called...\n");
#endif
	return task_create_local(TASK_NULL, FALSE, task);
}

kern_return_t
task_get_inherited_ports(task, r0, r1, r2, r3, exception, bootstrap)
	task_t task;
	ipc_port_t *r0;
	ipc_port_t *r1;
	ipc_port_t *r2;
	ipc_port_t *r3;
	ipc_port_t *exception;
	ipc_port_t *bootstrap;
{
	if (task == TASK_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	itk_lock(task);
	*r0 = ipc_port_copy_send(task->itk_registered[0]);
	*r1 = ipc_port_copy_send(task->itk_registered[1]);
	*r2 = ipc_port_copy_send(task->itk_registered[2]);
	*r3 = ipc_port_copy_send(task->itk_registered[3]);
	*exception = ipc_port_copy_send(task->itk_exception);
	*bootstrap = ipc_port_copy_send(task->itk_bootstrap);
	itk_unlock(task);
	return KERN_SUCCESS;
}

kern_return_t
task_set_inherited_ports(task, r0, r1, r2, r3, exception, bootstrap)
	task_t task;
	ipc_port_t r0;
	ipc_port_t r1;
	ipc_port_t r2;
	ipc_port_t r3;
	ipc_port_t exception;
	ipc_port_t bootstrap;
{
	int i;

	if (task == TASK_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	itk_lock(task);
	for (i = 0; i < 4; i++) {
		if (IP_VALID(task->itk_registered[i])) {
			ipc_port_release_send(task->itk_registered[i]);
		}
	}
	if (IP_VALID(task->itk_exception)) {
		ipc_port_release_send(task->itk_exception);
	}
	if (IP_VALID(task->itk_bootstrap)) {
		ipc_port_release_send(task->itk_bootstrap);
	}

	task->itk_registered[0] = r0;
	task->itk_registered[1] = r1;
	task->itk_registered[2] = r2;
	task->itk_registered[3] = r3;
	task->itk_exception = exception;
	task->itk_bootstrap = bootstrap;
	itk_unlock(task);
	return KERN_SUCCESS;
}

kern_return_t norma_task_create(parent_task, inherit_memory, child_node,
				child_task)
	task_t		parent_task;
	boolean_t	inherit_memory;
	int		child_node;
	task_t		*child_task;		/* OUT */
{
	ipc_port_t remote_task;
	task_t new_task;
	kern_return_t kr;
	int vector_start;
	unsigned int entry_vector_count;
	mach_port_t r0, r1, r2, r3, exception, bootstrap;
	emulation_vector_t *entry_vector;

	if (parent_task == TASK_NULL || !node_is_valid (child_node)) {
		return KERN_INVALID_ARGUMENT;
	}

	if (child_node == node_self()) {
		return task_create_local(parent_task, inherit_memory,
					 child_task);
	}

#if     MACH_ASSERT
	babble("task_create: doing %d -> %d\n", node_self(), child_node);
#endif

	kr = r_norma_task_allocate(remote_host(child_node,
					       MACH_MSG_TYPE_PORT_SEND),
				   &remote_task);
	if (kr != KERN_SUCCESS)
		return kr;

	kr = task_get_emulation_vector(parent_task, &vector_start,
				       &entry_vector, &entry_vector_count);
	if (kr != KERN_SUCCESS) {
		fret("task_get_emulation_vector failed: kr %d %x\n", kr, kr);
		goto L_ntc_failure;
	}

	kr = r_task_set_emulation_vector(remote_task, vector_start,
					 entry_vector, entry_vector_count);
	if (kr != KERN_SUCCESS) {
		fret("task_set_emulation_vector failed: kr %d %x\n", kr, kr);
		goto L_ntc_failure;
	}

#if     NCPUS > 1 && defined(PARAGON860) && !ASMP
	/* XXX not the way to go..... */
	if ( is_port_locked(parent_task->itk_bootstrap) ) {
        	ip_unlock(parent_task->itk_bootstrap);
	}
#endif

	/*
	 * get inherited task ports without send references as the send refs
	 * will be generated in r_task_set_inherited_ports() ala processing
	 * from mach_msg_rpc_from_kernel(). 3 must == TASK_PORT_REGISTER_MAX.
	 */
	itk_lock(parent_task);
	r0 = (mach_port_t)parent_task->itk_registered[0];
	r1 = (mach_port_t)parent_task->itk_registered[1];
	r2 = (mach_port_t)parent_task->itk_registered[2];
	r3 = (mach_port_t)parent_task->itk_registered[3];
	exception = (mach_port_t)parent_task->itk_exception;
	bootstrap = (mach_port_t)parent_task->itk_bootstrap;
	itk_unlock(parent_task);

#if     MACH_ASSERT
	mumble("%x %x %x %x %x %x\n", r0, r1, r2, r3, exception, bootstrap);
#endif

	kr = r_task_set_inherited_ports(remote_task, r0, r1, r2, r3,
					exception, bootstrap);
	if (kr != KERN_SUCCESS) {
		fret("task_set_inherited_ports failed: kr %d %x\n", kr, kr);
		goto L_ntc_failure;
	}

	if (inherit_memory) {
		kr = task_copy_vm(parent_task->map, remote_task);
#if     MACH_ASSERT
		mumble("task_create: task_copy_vm: (%x)\n", kr);
#endif
		if (kr != KERN_SUCCESS)
			goto L_ntc_failure;
	}

	/*
	 * Create a placeholder task for the benefit of convert_task_to_port.
	 * Set new_task->map to VM_MAP_NULL so that task_deallocate will
	 * know that this is only a placeholder task.
	 * XXX decr send-right count?
	 */
	new_task = (task_t) zalloc(task_zone);
	if (new_task == TASK_NULL)
		goto L_ntc_failure;

#if     MCMSG
	new_task->mcmsg_task = 0;
#endif  MCMSG

	/* only one ref, for our caller */
	new_task->ref_count = 1;

	new_task->map = VM_MAP_NULL;
	new_task->itk_self = remote_task;
	simple_lock_init(&new_task->lock);
	/*
	 * since 'new_task' is a placeholder task for the benefit of
	 * convert_task_to_port(). Make sure the IPC lock is also initialized.
	 * Allows convert_task_to_port() to function correctly.
	 */
	itk_lock_init(new_task);

#if     MACH_ASSERT
	babble("task_create: did   %d -> %d\n", node_self(), child_node);
#endif
	*child_task = new_task;
	return(KERN_SUCCESS);

L_ntc_failure:
	(void)r_task_terminate(remote_task);
	return(KERN_FAILURE);	/* Generic to avoid using MiG errors */
}

task_copy_vm(from0, to)
	vm_map_t from0;
	ipc_port_t to;
{
	vm_offset_t address;
	vm_size_t size;
	vm_prot_t protection, max_protection;
	vm_inherit_t inheritance;
	boolean_t shared, make_copy;
	ipc_port_t object_name;
	vm_offset_t offset;
	kern_return_t kr;
	vm_map_t from;
	ipc_port_t memory_object;
	
	from = vm_map_fork(from0);
	if (from == VM_MAP_NULL) {
		panic("task_copy_vm: vm_map_fork\n");
	}
	for (address = 0;; address += size) {
		kr = vm_region(from, &address, &size, &protection,
			       &max_protection, &inheritance, &shared,
			       &object_name, &offset);
		
		if (kr == KERN_NO_SPACE) {
			break;
		}
		/*
		 * dump the reference acquired by vm_region
		 */
		if (object_name != IP_NULL)
			ipc_port_release_send(object_name);
		switch (inheritance) {
			case VM_INHERIT_NONE:
			break;

			case VM_INHERIT_SHARE:

			/*
			 *  Only the simple case of sharing external
			 *  memory objects is handled now.
			 *  For sharing internal memory objects,
			 *  they would first have to be exported
			 *  by the VM system.
			 */
			{   vm_map_entry_t  entry; 
				boolean_t vm_object_is_internal; 
				vm_offset_t paging_offset;

			/*
			 * Look up the vm object and extract needed
			 * information.
			 */
			vm_map_lock_read(from);
			if (!vm_map_lookup_entry(from, address, &entry)) {
				vm_map_unlock_read(from);
				return(KERN_NO_SPACE);
			}
			vm_object_is_internal = entry->object.vm_object->internal;
			paging_offset = entry->object.vm_object->paging_offset;
			memory_object = entry->object.vm_object->pager;
			vm_map_unlock_read(from);

			/*
			 * Bail out if this is an internal object.
			 */
			if(vm_object_is_internal) {
			    fret("task_copy_vm: VM_INHERIT_SHARE!\n");
			    return KERN_FAILURE;
			}
			
			/*
			 * vm_map() the memory object on the target node.
			 */
			kr = r_vm_map(to, &address, size, 0, FALSE,
				      memory_object, paging_offset, FALSE,
				      protection, max_protection,
				      inheritance);
			if (kr != KERN_SUCCESS) {
				fret("task_cv: vm_map: %d 0x%x\n", kr, kr);
				return kr;
			}
			} break;

			case VM_INHERIT_COPY:

			/*
			 * 	If protection and max protection don't
			 * 	agree on read, fix local map to allow
			 * 	read.
			 */
			if ((protection & VM_PROT_READ) == 0 &&
			    (max_protection & VM_PROT_READ) != 0) {
				kr = vm_protect(from, address, size,
					FALSE, protection|VM_PROT_READ);
				assert(kr == KERN_SUCCESS);
			}
			
			/*
			 *	If this is an unreadable segment, map in
			 *	an unreadable blank on the remote node
			 *	instead of creating an xmm copy.
			 */
			if ((max_protection & VM_PROT_READ) == 0) {
				memory_object = IP_NULL;
				make_copy = FALSE;
			}
			else {
				kr = norma_copy_create(from, address, size,
					       &memory_object);
				if (kr != KERN_SUCCESS) {
					fret("task_cv: copy_create: %d 0x%x\n",
					     kr, kr);
					return kr;
				}
				make_copy =
				    ((max_protection & VM_PROT_WRITE) != 0);
			}

			kr = r_vm_map(to, &address, size, 0, FALSE,
				      memory_object, 0, make_copy,
				      protection, max_protection,
				      inheritance);
			if (kr != KERN_SUCCESS) {
				fret("task_cv: vm_map: %d 0x%x\n",
				     kr, kr);
				return kr;
			}

			if (memory_object != IP_NULL) {
				ipc_port_release_send(memory_object);
			}

			break;

			default:
			panic("task_copy_vm: inheritance=%d!\n",
			      inheritance);
		}
	}
	vm_map_deallocate(from);
	return KERN_SUCCESS;
}
#else	NORMA_TASK

norma_task_allocate()
{
}

task_get_inherited_ports()
{
}

task_set_inherited_ports()
{
}

norma_task_create()
{
}

norma_copy_create()
{
}

#endif	NORMA_TASK
