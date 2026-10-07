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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1992 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_appl.c,v 1.20 1994/11/18 20:43:46 mtm Exp $
 */

/*
 * mcmsg_appl.c
 *
 * Routines to put/get application information to/from the Mach
 * task structure.
 */

#include <mach/port.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/mcmsg/mcmsg_appl.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>

extern int	ipsc_physnode;

/*
 *
 *	Routine: nx_task_put_info [Server Entry]
 *	
 *	Place MP application information into task.
 *
 */
nx_task_put_info(host, nodelist, nodelistCnt, applinfo, mp_task, pid)
   ipc_port_t	host;
   LP_MAP_T	nodelist;
   unsigned int	nodelistCnt;
   APPLINFO_T	applinfo;
   task_t 	mp_task;	/* Message Passing Task */
   int		pid;
{
	int		s, i, my_node,	/* Scratch */
			physnode, 	/* Physical node number */
			virnode; 	/* Virtual  node number */
	kern_return_t	kr;
	vm_offset_t	addr;
	vm_map_copy_t	copy;

	/* Check the parameters.  */

	if (mp_task == TASK_NULL) {
		return KERN_INVALID_TASK;
	}

	/* Convert nodelist from vm_map_copy type to data.  */

	copy = (vm_map_copy_t) nodelist;
	kr = vm_map_copyout(ipc_kernel_map,
			    &addr,
			    copy);

	if (kr != KERN_SUCCESS) {
		return kr;
	}
	nodelist = (long *)addr;

	/*
	 * find my_node. 
	 * if we are not in the physical list, error
	 */

	my_node = -1;
	for (virnode=0; virnode<nodelistCnt; virnode++) {
		physnode = nodelist[virnode];
		if (physnode == ipsc_physnode) {
			my_node = virnode;
		}
	}
	if (my_node == -1 && applinfo.app != -1) {
		return KERN_INVALID_VALUE;
	}

	/* Init the task */

	if (mcmsg_call(POST_TASKINIT,
		       mp_task,
	               pid, 
	               my_node, 
		       nodelist,
	               nodelistCnt, 
	               &applinfo) == -1) {
		/* failed */
		kr = KERN_FAILURE;

		/*
		 * There's a basic assumption that task_init will work.
		 * If it doesn't, bad things happen so we just stop here.
		 */
		printf("mcmsg_call(POST_TASKINIT...) failed! kr=%X\n",kr);
		assert(0);
	} else {
		/* success - stuff nodelist into task */

		kr = KERN_SUCCESS;
	}

	/* Discard the memory allocated by vm_map_copyout */

	(void) kmem_free(ipc_kernel_map,
			 nodelist,
			 nodelistCnt * sizeof(LP_MAP_ENTRY_T));
			
	/* Done */
	return kr;
}

/*
 *	
 *	Routine: nx_task_get_info [Server Entry]
 *	
 *	Get MP application information from task.
 *
 */

nx_task_get_info(host,mp_task,applinfo,nodelist,nodelistCnt)
   ipc_port_t	host;
   task_t	mp_task;
   APPLINFO_T	*applinfo;
   LP_MAP_T	*nodelist;
   unsigned int	*nodelistCnt;
{
	int		s, i, physnode;
	vm_map_copy_t	copy;
	kern_return_t	kr;
	vm_size_t	alloc_size, list_size;
	vm_offset_t	addr;
	LP_MAP_ENTRY_T	*lp;

	list_size = 0;
	alloc_size = 0;
	addr = 0;
	
	if (mp_task == TASK_NULL) {
		return KERN_INVALID_TASK;
	}

	if (mp_task->mcmsg_task == 0) {
		return KERN_INVALID_TASK;
	}

	/* Get numnodes (plus host task) */

  	*nodelistCnt = mp_task->mcmsg_task->numnodes+1;

	/* Allocate nodelist */

	list_size = *nodelistCnt * sizeof(LP_MAP_ENTRY_T);
	alloc_size = round_page(list_size);

	if (kmem_alloc( ipc_kernel_map, &addr, alloc_size) != KERN_SUCCESS) {
		return KERN_RESOURCE_SHORTAGE;
	}

	/*
	 * kmem_alloc() could have blocked, and the task may have been
	 * destroyed. Bail out if the task not valid.
	 */
	if (mp_task->mcmsg_task == 0) {
		kmem_free(ipc_kernel_map, addr, alloc_size);
		return KERN_INVALID_TASK;
	}

	bzero((char *)addr, alloc_size);	/* start fresh */

	/* Fill in nodelist */

	lp = (LP_MAP_ENTRY_T *)addr;
	for (i=0; i<*nodelistCnt; i++) {
		lp[i] = mp_task->mcmsg_task->task_ptype_list->phys_node_list[i];
	}
	
	/* Make nodelist a vm_map_copy_t object */

	kr = vm_map_copyin(ipc_kernel_map,
			   addr,
			   list_size,
			   TRUE,
			   &copy);

	if (kr != KERN_SUCCESS) {
		return kr;
	}
	*nodelist = (LP_MAP_T) copy;

        /* Get applinfo  */

	bcopy(&(mp_task->mcmsg_task->applinfo),
	      applinfo,
	      sizeof(APPLINFO_T));

	/* done */

  	return KERN_SUCCESS;
}
