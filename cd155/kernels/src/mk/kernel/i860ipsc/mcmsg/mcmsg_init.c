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
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_init.c,v 1.55 1994/11/18 20:41:28 mtm Exp $
 */

/*
 * mcmsg_init.c
 *
 * Initialization for multicomputer message passing
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>

/* XXX */
#define	LTU_ALIGN		32
#define LTU_ALIGN_MASK	(~(LTU_ALIGN-1))

unsigned long mcmsg_task_active = 0;

extern char	*getbootenv();

netipc_network_init()
{

	mcmsg_memory_init();
	mcmsg_ipc_init();
	mcmsg_select_init();
	mcmsg_selector_init(&mcmsg_app_sel, SELMETH_APP);
	mcmsg_selector_init(&mcmsg_pid_sel, SELMETH_PID_TASK);
}

mcmsg_basic_init()
{
	extern int ipsc_physnode;
	extern int _node_self;
	char *s;

	_node_self = ipsc_physnode;

	mcmsg_send_store_in = 0;
	mcmsg_send_store_out = 0;

#if	PARAGON860

	if ((s = getbootenv("BOOT_FIRST_NODE"))) {
		paragon_first_node = atoi(s);
	}
	if ((s = getbootenv("BOOT_MESH_X"))) {
		paragon_mesh_x = atoi(s);
	}
	assert(paragon_mesh_x != 0);
	if ((s = getbootenv("BOOT_MESH_Y"))) {
		paragon_mesh_y = atoi(s);
	}
	assert(paragon_mesh_y != 0);

#endif	PARAGON860

	mcmsg_hw_init();
	mcmsg_console_init();
}

dcm_init_recv()
{

	return;
}

dcm_init_send()
{

	return;
}


syscall_mcmsg_nodeinfo(nodeinfo)
	nodeinfo_t	*nodeinfo;
{
	task_t		task;
	unsigned long	*np;
	int		x;

	/*
	 * Attach to Mach task
	 */

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	mcmsg_trace_debug("nodeinfo", 1, nodeinfo, 0, 0, 0);

	task = current_task();
	if (task->mcmsg_task == 0) {
		mcmsg_trace_drop("nodeinfo no mcmsg_task", task);
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}
	mcmsg_task = task->mcmsg_task;

	if (mcmsg_task->applinfo.app == -1) {
		mcmsg_trace_drop("nodeinfo not parallel", task);
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}

	if (mcmsg_validate_long(nodeinfo) == 0 ||
	    mcmsg_validate_long( ((long)&nodeinfo[1]) - sizeof(long) ) == 0) {
		mcmsg_trace_drop("nodeinfo bad pointer", nodeinfo);
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}

	nodeinfo->mynode = task->mcmsg_task->node;
	nodeinfo->numnodes = task->mcmsg_task->numnodes;
	nodeinfo->applinfo = task->mcmsg_task->applinfo;

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

syscall_mcmsg_myphysnode()
{

	return ipsc_physnode;
}

syscall_mcmsg_reserve_ptype(ptype, count)
	long		ptype;
	long		count;
{
	task_t		task;
	mcmsg_task_t	*mcmsg_task;
	ptype_list_t	*task_ptype_list;
	int		x;

	/*
	 * Attach to Mach task
	 */

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	mcmsg_trace_debug("reserve_ptype", 2, ptype, count, 0, 0);

	task = current_task();
	if (task->mcmsg_task == 0) {
		mcmsg_trace_drop("reserve_ptype no mcmsg_task", task);
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}

	mcmsg_task = task->mcmsg_task;
	task_ptype_list = mcmsg_task->task_ptype_list;

	/*
	 * Insert ptypes
	 */

	/* XXX Replace with range based add and eliminate limit of 1000 */

	if (count <= 0 || count > 1000) {
		mcmsg_trace_drop("reserve_ptype bad count", count);
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}
	if (ptype <= INVALID_PTYPE && ptype + count >= INVALID_PTYPE) {
		mcmsg_trace_drop("reserve_ptype bad ptype", ptype);
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return -1;
	}

	for (; count; count--, ptype++) {
		if (mcmsg_ptype_add(task_ptype_list,
				    ptype,
				    (void *)mcmsg_task,
				    0) == -1) {
			mcmsg_trace_drop("reserve_ptype add failed", ptype);
			assert(mcmsg_reentry--);
			RED_OFF(RED_MSG);
			splx(x);
			return -1;
		}
	}
	mcmsg_flush_pna();
	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}

mcmsg_init_task(task, pid, node, nodecount, applinfo)
	task_t		task;
	long		pid;
	unsigned long	node;
	unsigned	nodecount;
	APPLINFO_T	*applinfo;
{
	long		app;
	int		i;
	unsigned	pkt_size;
	unsigned long	*p;
	unsigned long	nlsize;
	unsigned long	*pnode_list;
	node_list_t	*node_list;
	ptype_list_t	*task_ptype_list;
	ptype_list_t	*node_ptype_list;
	mcmsg_task_t	*mt;
	int		task_ptype_list_installed;
	int		par_task;	/* boolean - parallel task? */
	unsigned long	prev_nodecount;	/* nodes in previous partition */


	/*
	 * Are we going parallel?
	 */
	app = applinfo->app;
	par_task = (app != -1);

	/*
	 * Check message passing parameters
	 */
	if (par_task) {

		mcmsg_trace_debug("mcmsg init parallel", 4,
				  task, pid, node, nodecount);

		pkt_size = applinfo->pkt_size;
		if (pkt_size % LTU_ALIGN) {
			pkt_size += LTU_ALIGN;
			pkt_size &= LTU_ALIGN_MASK;
			mcmsg_trace_debug("mcmsg init pkt rounded", 1, pkt_size, 0, 0, 0);
			applinfo->pkt_size = pkt_size;
		}

		if (pkt_size < MIN_PKT_SIZE || pkt_size > MAX_PKT_SIZE) {
			mcmsg_trace_debug("mcmsg init bad pkt_size", 1,
					  pkt_size, 0, 0, 0);
			return -1;
		}

		/*
		 * Ensure send_count is multiple of pkt_size.
		 * (Silently round up)
		 */
		if (applinfo->send_count % pkt_size != 0) {
			applinfo->send_count += 
				(pkt_size - (applinfo->send_count % pkt_size));
			mcmsg_trace_debug("mcmsg init send_count rounded", 1,
					  applinfo->send_count, 0, 0, 0);
		}
	} else {
		mcmsg_trace_debug("mcmsg init service", 4,
				  task, pid, node, nodecount);
	}

	/*
	 * Attach to Mach task
	 */

	mt = task->mcmsg_task;

	if (mt == 0) {
		mt = (mcmsg_task_t *)
		     mcmsg_l2malloc(l2size(sizeof(mcmsg_task_t)));
		if (mt == 0) {
			mcmsg_trace_debug("mcmsg init err no mem mcmsg_task", 0,
					  0, 0, 0, 0);
			return -1;
		}
		bzero(mt, sizeof(mcmsg_task_t));
		mt->task = task;
		mt->dirbase = task->map->pmap->dirbase;
/* XXX THIS SHOULD BE FIXED ELSEWHERE */
#define MAP_DP_FIX 1
#if	MAP_DP_FIX
/* map DP ASIC to user */
if (getbootenv("MAP_DP_FIX")) mapdp(mt->dirbase);
		mcmsg_phys = 0;
#endif	MAP_DP_FIX
	} else if (mt->task_ptype_list) {
		/* 
		 * Fix PTS 3477, when the service partition proxy_pid goes 
		 * parallel, it already has a task_ptype_list, we need to it
		 * deallocate here and reallocate it below.
		 */
		mcmsg_ptype_clear_item(mt->task_ptype_list, (void *)mt);
		mt->task_ptype_list->refcount--;
		if (mt->task_ptype_list->refcount == 0) {
		    mcmsg_trace_debug("destroy ptype lists", 2,
				  mt->task_ptype_list,
				  mt->task_ptype_list->node_ptype_list,
				  0, 0);
	
		    mcmsg_l2free(mt->task_ptype_list->phys_node_list,
		    l2size((mt->numnodes+1)*sizeof(unsigned long)));

		    mcmsg_ptype_free_all(
			mt->task_ptype_list->node_ptype_list,
			l2size((mt->numnodes+1)*sizeof(unsigned long)));
		    mcmsg_l2free(mt->task_ptype_list->node_ptype_list,
			     l2size(sizeof(ptype_list_t)));

		    mcmsg_l2free(mt->task_ptype_list,
			     l2size(sizeof(ptype_list_t)));
		}
		mt->task_ptype_list = 0;
	}

	/*
	 * Interact with applinfo structure
	 */

	/*
	 * Fill out mcmsg_task
	 */

	mt->node = node;
	mt->numnodes = nodecount-1;
	mt->applinfo = *applinfo;
	mt->pid = pid;

	task_ptype_list_installed = 0;
	if (par_task) {
		mt->method = SELMETH_TASK;
		mt->link = 0;
		mt->selection_path = 0;
		mt->xmsg_head = 0;
		mt->xmsg_tail = 0;
		mt->wired_start = 0;
		mt->wired_end = 0;
		mt->provided = 0;
		mt->assigned = 0;
		mt->assign_target = 0;
		mt->avail_need = 0;
	
		mt->rk_recv_need = 0;
		mt->send_wait_unk = 0;
	
		mt->pid_sel = (select_t *)
				     mcmsg_l2malloc(l2size(sizeof(select_t)));
		mcmsg_selector_init(mt->pid_sel, SELMETH_PID_SEL);
	
		/*
		 * Install task in application and ptype selection path
		 */
	
		task_ptype_list = (ptype_list_t *)mcmsg_selector_lookup(
							&mcmsg_app_sel, 
							app);
		task_ptype_list_installed = task_ptype_list != 0;
		mt->task_ptype_list = task_ptype_list;
		mcmsg_task = mt;
	}
	
	/*
	 * Create ptype lists
	 */

	task_ptype_list = mt->task_ptype_list;

	if (task_ptype_list == 0) {

		task_ptype_list = (ptype_list_t *)
			     mcmsg_l2malloc(l2size(sizeof(ptype_list_t)));
		if (task_ptype_list == 0) {
		 mcmsg_trace_debug("mcmsg init err no mem task_ptype_list", 0,
				   0, 0, 0, 0);
			return -1;
		}
		mcmsg_ptype_init(task_ptype_list);
		task_ptype_list->method = SELMETH_TASK_PTYPE;
		task_ptype_list->app = app;
		task_ptype_list->node_ptype_list = 0;
		task_ptype_list->phys_node_list = 0;
	}

	node_ptype_list = task_ptype_list->node_ptype_list;
	if (node_ptype_list == 0) {
		node_ptype_list = (ptype_list_t *)
			     mcmsg_l2malloc(l2size(sizeof(ptype_list_t)));
		if (node_ptype_list == 0) {
			mcmsg_l2free(task_ptype_list,
			     	     l2size(sizeof(ptype_list_t)));
		 mcmsg_trace_debug("mcmsg init err no mem node_ptype_list", 0,
				   0, 0, 0, 0);
			return -1;
		}
		mcmsg_ptype_init(node_ptype_list);
		task_ptype_list->method = SELMETH_NODE_PTYPE;
		node_ptype_list->app = app;
		node_ptype_list->refcount = 0;
	}

	task_ptype_list->node_ptype_list = node_ptype_list;
	node_ptype_list->node_ptype_list = node_ptype_list;


	/*
	 * Free old node list (if any)
	 */

	if (task_ptype_list->phys_node_list != 0) {
		prev_nodecount = task_ptype_list->numnodes+1;
		nlsize = l2size(prev_nodecount*sizeof(unsigned long));

		mcmsg_trace_debug("mcmsg init free old pnode_list", 
		   2, task_ptype_list->phys_node_list, nlsize, 0, 0);

		mcmsg_l2free(task_ptype_list->phys_node_list, nlsize);
		task_ptype_list->phys_node_list = 0;
	}

	/*
	 * Create new phys node list
	 */

	nlsize = l2size(nodecount*sizeof(unsigned long));
	pnode_list = (unsigned long *)mcmsg_l2malloc(nlsize);
	if (pnode_list == 0) {
		mcmsg_l2free(task_ptype_list,
		     	     l2size(sizeof(ptype_list_t)));
		mcmsg_l2free(node_ptype_list,
		     	     l2size(sizeof(ptype_list_t)));
		mcmsg_trace_debug("mcmsg init err no mem pnode_list", 0,
				  0, 0, 0, 0);
		return -1;
	}
	task_ptype_list->phys_node_list = pnode_list;
	task_ptype_list->numnodes = nodecount - 1;

	node_ptype_list->phys_node_list = pnode_list;
	node_ptype_list->numnodes = nodecount - 1;


	if (par_task && !task_ptype_list_installed) {
		mcmsg_selector_install(&mcmsg_app_sel,
				       app,
				       task_ptype_list,
				       SELMETH_PTYPE);
	}

	mt->task_ptype_list = task_ptype_list;
	task_ptype_list->refcount++;

	/*
	 * Install pid
	 */

	if (par_task) {
		mcmsg_selector_install(&mcmsg_pid_sel,
					pid,
					mt,
					SELMETH_TASK);
	}

	task->mcmsg_task = mt;
	mcmsg_task_active++;
	mcmsg_trace_debug("mcmsg init task done", 
	                   3, task, mt, mcmsg_task_active, 0);
	return 0;
}

mcmsg_task_destroy(task)
	task_t		task;
{
	int	par_task;	/* boolean - parallel task? */
	int x;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);

	mcmsg_task = task->mcmsg_task;

	mcmsg_trace_debug("destroy task", 2, task, mcmsg_task, 0, 0);

	par_task = (mcmsg_task->applinfo.app != -1);

	mcmsg_ptype_clear_item(mcmsg_task->task_ptype_list, (void *)mcmsg_task);
	mcmsg_task->task_ptype_list->refcount--;
	if (mcmsg_task->task_ptype_list->refcount == 0) {
		mcmsg_trace_debug("destroy ptype lists", 2,
				  mcmsg_task->task_ptype_list,
				  mcmsg_task->task_ptype_list->node_ptype_list,
				  0, 0);

		if (par_task) {
			mcmsg_selector_remove(&mcmsg_app_sel,
				      mcmsg_task->applinfo.app);
		}

		mcmsg_l2free(mcmsg_task->task_ptype_list->phys_node_list,
		    l2size((mcmsg_task->numnodes+1)*sizeof(unsigned long)));

		mcmsg_ptype_free_all(
			mcmsg_task->task_ptype_list->node_ptype_list,
			l2size((mcmsg_task->numnodes+1)*sizeof(unsigned long)));
		mcmsg_l2free(mcmsg_task->task_ptype_list->node_ptype_list,
			     l2size(sizeof(ptype_list_t)));

		mcmsg_l2free(mcmsg_task->task_ptype_list,
			     l2size(sizeof(ptype_list_t)));
	}
	/* XXX Walk data structures, free select_item's and selectors */
	if (par_task) {
		mcmsg_nx_clear_task(mcmsg_task);
		mcmsg_selector_clear_task_seq(&mcmsg_pid_sel, mcmsg_task);
		mcmsg_selector_remove(&mcmsg_pid_sel, mcmsg_task->pid);
		mcmsg_task_pid_sel_clear(mcmsg_task);
		mcmsg_l2free(mcmsg_task->pid_sel, l2size(sizeof(select_t)));
	}
	mcmsg_l2free(mcmsg_task, l2size(sizeof(mcmsg_task_t)));
	task->mcmsg_task = 0;

	mcmsg_task_active--;
	if (mcmsg_task_active == 0) {
		extern unsigned long mcmsg_memalloc[];
		register int i;

#if DANGEROUS
		for (i = 0; i <= LOG2MMSIZE; i++) {
			assert(mcmsg_memalloc[i] == 0);
		}
#endif DANGEROUS
	}

	mcmsg_trace_debug("destroy task done", 2, task, mcmsg_task_active, 0, 0);

	mcmsg_task = 0;

	RED_OFF(RED_MSG);
	assert(mcmsg_reentry--);
	splx(x);
}

mcmsg_install_sequence(source_pid, si)
	register long		source_pid;
	register select_item_t	*si;
{
	register select_item_t	*sh;

	sh = mcmsg_selector_lookup_si(&mcmsg_pid_sel, source_pid);
	if (sh != 0) {
		si->nxrq.seq_link = sh->nxrq.seq_link;
		sh->nxrq.seq_link = si;
	} else {
		si->nxrq.seq_link = 0;
		assert(source_pid != 0);
		mcmsg_selector_install_si(&mcmsg_pid_sel,
					  si,
					  source_pid,
					  si->method);
	}
}

select_item_t *
mcmsg_lookup_sequence(source_pid, sequence)
	register long		source_pid;
	register unsigned long	sequence;
{
	register select_item_t	*si;
	int			t;

	si = mcmsg_selector_lookup_si(&mcmsg_pid_sel, source_pid);
	if (si != 0) {
		/*
		 * This item can be a process or a message. 
		 */
		if (si->method == SELMETH_TASK) { 	/* FIX PTS 3087 */
			si = si->nxrq.seq_link;
		}
		assert((t = MAXLOOP) != 0);
		while (si != 0) {
			if (si->nxrq.sequence == sequence) {
				break;
			}
			si = si->nxrq.seq_link;
			assert(t-- != 0);
		}
		if ((si) && (si->mcmsg_task)) {
			mcmsg_task = si->mcmsg_task; /* FIX PTS 3087 */
		}
	}
	return si;
}

mcmsg_remove_sequence(source_pid, si)
	register long		source_pid;
	register select_item_t	*si;
{
	register select_item_t	*sh;
	int			t;

	sh = mcmsg_selector_lookup_si(&mcmsg_pid_sel, source_pid);
	assert(sh != 0);
	if (sh == si) {
		si = si->nxrq.seq_link;
		mcmsg_selector_remove(&mcmsg_pid_sel, source_pid);
		if (si != 0) {
			mcmsg_selector_install_si(&mcmsg_pid_sel,
						  si,
						  source_pid,
						  si->method);
		}
	} else {
		assert((t = MAXLOOP) != 0);
		while (sh->nxrq.seq_link != si) {
			sh = sh->nxrq.seq_link;
			assert(t-- != 0);
		}
		sh->nxrq.seq_link = si->nxrq.seq_link;
		mcmsg_free_select_item(si);
	}
}

calculate_route(physnode)
	register unsigned long	physnode;
{

#if	iPSC860

	return (physnode) ^ (ipsc_physnode & 0x7F);

#else	iPSC860

	unsigned long	my_x, my_y;
	unsigned long	target_x, target_y;
	unsigned long	x, y;

	my_x = ipsc_physnode % paragon_mesh_x;
	my_y = ipsc_physnode / paragon_mesh_x;
	target_x = physnode % paragon_mesh_x;
	target_y = physnode / paragon_mesh_x;
	if (my_x > target_x) {
		x = (my_x - target_x) | NIC_WEST;
	} else {
		x = (target_x - my_x) | NIC_EAST;
	}
	if (my_y >= target_y) {
		y = (my_y - target_y) | NIC_NORTH;
	} else {
		y = (target_y - my_y) | NIC_SOUTH;
	}
	return (y << 16) | x;

#endif	iPSC860
}

syscall_mcmsg_init(address, size)
	unsigned long	address;
	unsigned long	size;
{
	task_t		task;
	mcmsg_task_t	*mt;
	int		i;
	int		npages,pavail;

	/*
	 *	Make sure there is enough memory to wire without paging
	 *	out the server.
	 */
	npages = size/PAGE_SIZE;	/* convert to signed */
	pavail = vm_page_free_count + 
		 vm_page_inactive_count + 
		 vm_page_active_count;
	if ((pavail - npages) < 275) {
		return -2;
	}

	task = current_task();
	mt = task->mcmsg_task;
	if (mt == 0) {
		return -1;
	}

	mcmsg_trace_debug("mcmsg wire", 3,
			  address, size, address + size - 1, 0);
	i = mcmsg_wire_buffer(mt, address, address + size - 1);

	if (mt->wired_start == mt->wired_end) {
		mt->wired_start = address;
		mt->wired_end = address + size - 1;
	} else if (mt->wired_start > address) {
		mt->wired_start = address;
	} else if (mt->wired_end < address + size - 1) {
		mt->wired_end = address + size - 1;
	}

	return i;
}

mcmsg_wire_buffer(mt, lo, hi)
	mcmsg_task_t	*mt;
	unsigned long	lo;
	unsigned long	hi;
{
	task_t		task;
	int		i;

	task = mt->task;
	i = vm_map_pageable_user(task->map,
			trunc_page(lo),
			round_page(hi),
			VM_PROT_READ | VM_PROT_WRITE);
	mcmsg_trace_debug("vm_map_pageable_user", 4, 
			task->map, trunc_page(lo), round_page(hi), i);
	if (i != 0) {
#if 0
	 printf("vm_map_pageable_user(%08X, %08X, %08X, %08X) returns %d\n",
			task->map,
			trunc_page(lo),
			round_page(hi),
			VM_PROT_READ | VM_PROT_WRITE,
			 i);
#endif
		return -1;
	}
	return 0;
}

mcmsg_clear_item_list(list)
	select_item_t	*list;
{
	register select_item_t	*si;
	register select_item_t	*sn;
	register unsigned long	t;

	if (list == 0) {
		return;
	}
	assert((t = MAXLOOP) != 0);
	si = list->link;
	for (;;) {
		sn = si->link;
mcmsg_trace_debug("clear list", 2, si, sn, 0, 0);
		mcmsg_free_select_item(si);
		if (si == list) {
			break;
		}
		assert(sn != si);
		si = sn;
		assert(t-- != 0);
	}
}

mcmsg_nx_clear_task(mcmsg_task)
	register mcmsg_task_t	*mcmsg_task;
{
	register select_item_t	*st;
	register select_item_t	*si;
	register select_item_t	*sn;
	register unsigned long	t;

	/*
	 * Clear selection path
	 */

	st = mcmsg_task->selection_path;
	mcmsg_trace_debug("clear path", 1, st, 0, 0, 0);
	if (st != 0) {
		si = st->link;
		assert((t = MAXLOOP) != 0);
		for (;;) {
			sn = si->link;
			if (si->method == SELMETH_RECV_TYPESEL) {
				mcmsg_selector_clear(si->item);
				mcmsg_l2free(si->item,
					     l2size(sizeof(select_t)));
			}
			mcmsg_free_select_item(si);
			if (si == st) {
				break;
			}
			si = sn;
			assert(t-- != 0);
		}
		mcmsg_task->selection_path = 0;
	}

	/*
	 * Clear send_unk list
	 */

	mcmsg_trace_debug("clear unk", 1, mcmsg_task->send_wait_unk, 0, 0, 0);
	mcmsg_clear_item_list(mcmsg_task->send_wait_unk);
	mcmsg_task->send_wait_unk = 0;
}

/*
 *	Routine:
 *		mcmsg_task_pid_sel_clear
 *
 *	Purpose:
 *		Clean out task's pid selector and associated structures.
 *		Also send termination messages.
 */
mcmsg_task_pid_sel_clear(mcmsg_task)
	register mcmsg_task_t	*mcmsg_task;
{
	register select_t	*sel;
	register int		hi;
	register select_item_t	*si;
	register select_item_t	*st;
	register select_item_t	*sn;
	register void		**sh;
	unsigned long		t;

	sel = mcmsg_task->pid_sel;
	mcmsg_trace_debug("pid_sel clear", 1, sel, 0, 0, 0);
	for (hi = 0; hi <= SELECT_HASH_LEN; hi++) {
		if (hi == SELECT_HASH_LEN) {
			sh = &sel->zero;
		} else {
			sh = &sel->hash[hi];
		}
		
		st = *sh;
		if (st != 0) {
			si = st->link;
			assert((t = MAXLOOP) != 0);
			for (;;) {
				sn = si->link;
				assert(si->method == SELMETH_PID);

				/*
				 * Send termination message to this process.
				 */
				mcmsg_send_now(SENDMETH_PRM, si, 0);

				if (si->ppid.send_wait != 0) {
					mcmsg_trace_debug("clear wait", 1, 
					                   si->ppid.send_wait, 0, 0, 0);
					mcmsg_clear_item_list(si->ppid.send_wait);
				}
				mcmsg_free_select_item(si);
				if (si == st)
					break;
				si = sn;
				assert(t-- != 0);
			}
		}
		*sh = 0;
	}
}

#if	MAP_DP_FIX
mapdp(dirbase)
	unsigned long	*dirbase;
{
	unsigned long	*pt;

	dirbase[0x3ff] |= 4;
	pt = (unsigned long *)(dirbase[0x3ff] & 0xfffff000);
	pt[0x3f4] |= 4;
}
#endif	MAP_DP_FIX

/*
 * These stubs help the transition to the message processor code
 */

call_pmap_update_interrupt()
{

	return;
}

mcmsg_interrupt_from_msgp()
{

	assert(0);
	return;
}

int msgp_stack;
msgp_main()
{

	for (;;)
		assert(0);
}

syscall_mcmsg_mp_access()
{

	return -1;
}
