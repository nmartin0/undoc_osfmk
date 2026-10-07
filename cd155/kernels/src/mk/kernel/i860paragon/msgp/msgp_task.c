/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_task.c,v 1.17 1995/03/06 23:58:35 terry Exp $
 */

/*
 * mcmsg_task.c
 *
 * Multicomputer task management.
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_appl.h>

unsigned long mcmsg_task_active = 0;

mcmsg_init_task(mt, task, pid, node, nodelist, nodecount, applinfo)
	mcmsg_task_t	*mt;
	task_t		task;
	long		pid;
	unsigned long	node;
	LP_MAP_T	nodelist;
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
	int		task_ptype_list_installed;
	int		par_task;	/* boolean - parallel task? */
	unsigned long	prev_nodecount;	/* nodes in previous partition */
	select_item_t	*si;


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
		if (pkt_size % sizeof(xmsg_t)) {
			pkt_size =
			 (pkt_size + sizeof(xmsg_t) - 1) & ~(sizeof(xmsg_t));
			mcmsg_trace_debug("mcmsg init pkt rounded", 1,
					pkt_size, 0, 0, 0);
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
				  mt->node_ptype_list,
				  0, 0);
	
		    mcmsg_l2free(mt->task_ptype_list->phys_node_list,
		    l2size((mt->numnodes+1)*sizeof(unsigned long)));

		    mcmsg_ptype_free_all( mt->node_ptype_list,
			l2size((mt->numnodes+1)*sizeof(unsigned long)));
		    mcmsg_l2free(mt->node_ptype_list,
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
		mt->xmsg_rdy = 0;
		mt->xmsg_rdy_end = 0;
		mt->xmsg_head = 0;
		mt->xmsg_tail = 0;
		mt->wired_start = 0;
		mt->wired_end = 0;
		mt->provided = 0;
		mt->assigned = applinfo->noc * applinfo->memory_each;
		mt->reserved = mt->assigned;
		mt->avail_need = 0;
		mt->attached = 0;
		mt->attach_wait = 0;
		mt->attach_pend = 0;
	
		mt->rk_recv_need = 0;
		mt->send_wait_unk = 0;

		/* Set up per task AST post buffer */
		assert(mt->nxport_req_list == 0);
		mt->nxport_req_in = 0;
		mt->nxport_req_out = 0;

		mt->nxport_req_list = (mcmsg_nxport_req_t *)
			mcmsg_l2malloc(l2size(MAX_NXPORT_REQS * sizeof(mcmsg_nxport_req_t)));

		if (mt->nxport_req_list == 0) {
			mcmsg_l2free(mt, l2size(sizeof(mcmsg_task_t)));
			mcmsg_trace_debug("mcmsg init err no mem nxport_req_list", 1,
				mt, 0, 0, 0);
			return -1;
		}
		bzero(mt->nxport_req_list, MAX_NXPORT_REQS * sizeof(mcmsg_nxport_req_t));

		mt->pid_sel = (select_t *)
				     mcmsg_l2malloc(l2size(sizeof(select_t)));
		mcmsg_selector_init(mt->pid_sel, SELMETH_PID_SEL);
	
		/*
		 * Install task in application and ptype selection path
		 */
	
		si = mcmsg_selector_lookup_si(&mcmsg_app_sel, app);
		task_ptype_list_installed = si != 0;
		if (task_ptype_list_installed) {
			task_ptype_list = (ptype_list_t *)(si->item);
			mt->task_ptype_list = task_ptype_list;
		}
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
		task_ptype_list->phys_node_list = 0;
		task_ptype_list->refcount=0;
	}

	node_ptype_list = (ptype_list_t *)
			     mcmsg_l2malloc(l2size(sizeof(ptype_list_t)));
	if (node_ptype_list == 0) {
		mcmsg_l2free(task_ptype_list, l2size(sizeof(ptype_list_t)));
		mcmsg_trace_debug("mcmsg init err no mem node_ptype_list", 0,
				   0, 0, 0, 0);
			return -1;
	}

	mt->node_ptype_list = node_ptype_list;
	mcmsg_ptype_init(node_ptype_list);
	task_ptype_list->method = SELMETH_NODE_PTYPE;
	node_ptype_list->app = app;
	node_ptype_list->refcount = 0;

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
		mcmsg_selector_install(mt,
				       &mcmsg_app_sel,
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
		mcmsg_selector_install(mt,
				       &mcmsg_local_sel,
				       pid,
				       mt,
				       SELMETH_TASK);
	}

	for (i=0; i<nodecount; i++) {
		mt->task_ptype_list->phys_node_list[i] = nodelist[i];
	}

	task->mcmsg_task = mt;
	mcmsg_task_active++;
	mcmsg_trace_debug("mcmsg init task done", 
	                   3, task, mt, mcmsg_task_active, 0);
	return 0;
}

mcmsg_destroy_task(mt)
	mcmsg_task_t	*mt;
{
	int	par_task;	/* boolean - parallel task? */

	mcmsg_trace_debug("destroy task", 1, mt, 0, 0, 0);

	par_task = (mt->applinfo.app != -1);

	mcmsg_remove_send_store_task(mt);

	mcmsg_ptype_clear_item(mt->task_ptype_list, (void *)mt);
	mt->task_ptype_list->refcount--;
	if (mt->task_ptype_list->refcount == 0) {
		mcmsg_trace_debug("destroy ptype lists", 2,
				  mt->task_ptype_list,
				  mt->node_ptype_list,
				  0, 0);

		if (par_task) {
			mcmsg_selector_remove(&mcmsg_app_sel,
				      mt->applinfo.app);
		}

		mcmsg_l2free(mt->task_ptype_list->phys_node_list,
		    l2size((mt->numnodes+1)*sizeof(unsigned long)));

		mcmsg_l2free(mt->task_ptype_list,
			     l2size(sizeof(ptype_list_t)));
	}


	mcmsg_ptype_free_all(
		mt->node_ptype_list,
		l2size((mt->numnodes+1)*sizeof(unsigned long))
	);
	mcmsg_l2free(mt->node_ptype_list, l2size(sizeof(ptype_list_t)));



	/* XXX Walk data structures, free select_item's and selectors */
	if (par_task) {
		mcmsg_nx_clear_task(mt);
		mcmsg_selector_clear_task_seq(mt);
		mcmsg_selector_remove(&mcmsg_local_sel, mt->pid);
		mcmsg_task_pid_sel_clear(mt);
		mcmsg_l2free(mt->pid_sel, l2size(sizeof(select_t)));
		mcmsg_l2free(mt->nxport_req_list,
			l2size(MAX_NXPORT_REQS * sizeof(mcmsg_nxport_req_t)));
	}

#if	MCMSG_MODULE_PUMA
	mcmsg_puma_destroy_task(mt);
#endif	MCMSG_MODULE_PUMA

#if	MCMSG_MODULE_RDMA
	mcmsg_rdma_destroy_task(mt);
#endif	MCMSG_MODULE_RDMA

#if	MCMSG_MODULE_URDMA
	mcmsg_urdma_destroy_task(mt);
#endif	MCMSG_MODULE_URDMA

	/*
	 * Free mode, the task structure and adjust counts.
	 */
	mcmsg_l2free(mt, l2size(sizeof(mcmsg_task_t)));

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

	mcmsg_trace_debug("destroy task done", 1, mcmsg_task_active, 0, 0, 0);
}

mcmsg_nx_clear_task(mt)
	register mcmsg_task_t	*mt;
{
	register select_item_t	*st;
	register select_item_t	*si;
	register select_item_t	*sn;
	register unsigned long	t;

	/*
	 * Clear selection path
	 */

	st = mt->selection_path;
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
		mt->selection_path = 0;
	}

	/*
	 * Clear send_unk list
	 */

	mcmsg_trace_debug("clear unk", 1, mt->send_wait_unk, 0, 0, 0);
	mcmsg_clear_item_list(mt->send_wait_unk);
	mt->send_wait_unk = 0;
}

/*
 *	Routine:
 *		mcmsg_task_pid_sel_clear
 *
 *	Purpose:
 *		Clean out task's pid selector and associated structures.
 *		Also send termination messages.
 */
mcmsg_task_pid_sel_clear(mt)
	register mcmsg_task_t	*mt;
{
	register select_t	*sel;
	register int		hi;
	register select_item_t	*si;
	register select_item_t	*st;
	register select_item_t	*sn;
	register void		**sh;
	unsigned long		t;

	sel = mt->pid_sel;
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
     				 * Send disconnect message 
     				 *
					 * Since NDA is used for exiting and the related data
					 * structures (mt, pid_si) may get deallocated, don't
					 * rely on them being there if the send is queued. gjr
     				 */
    				if (si->ppid.send_ready) {

       					mcmsg_send(mt, MCTRL_NDA,
						           si->ppid.route,
						           si->ppid.send_avail,
						           mt->pid,
						           si->value);

       					si->ppid.send_ready = 0;
       					si->ppid.send_avail = 0;
    				}

				/*
				 * Send termination message to this process.
				 * Like NDA, do not reference the data structures
				 * of the exiting processes.
				 */
				mcmsg_send(mt, MCTRL_PRM, 
				           si->ppid.route,
				           mt->pid,
				           si->value);

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
