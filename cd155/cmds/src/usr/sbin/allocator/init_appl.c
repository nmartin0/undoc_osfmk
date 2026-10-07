/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
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
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/init_appl.c,v 1.30 1995/03/01 21:58:20 sdh Exp $
 *
 */

/* History:
 *	$Log: init_appl.c,v $
 * Revision 1.30  1995/03/01  21:58:20  sdh
 * Added a line to initialize selector_mask to 0.
 *
 *  Reviewer: carolr
 *  Risk: low
 *  Benefit or PTS #: 12225
 *  Testing:
 * 	EATS: sched, rmcmd, rmcall, controlc
 * 	manual testing
 *  Module(s):
 * 	init_appl.c
 *
 * Revision 1.29  1995/02/28  17:39:57  carolr
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 11085
 *  Testing: EATS: controlc, rmcall, rmcmd, sched, manual testing
 *  Module(s): src/usr/sbin/allocator/init_appl.c
 *
 *  Description:
 *
 *    Added check to ensure the anchor node did not
 *    exceed the largest node in the partition and was
 *    not less than -1.
 *
 *    Added explicit casts to 'long' for comparison lines involving
 *    nx_info.start_node, as this variable is declared to be an unsigned long
 *    (the compiler was doing this automagically for us)
 *
 * Revision 1.28  1994/12/19  15:57:27  johannes
 * init_appl(): Initialize the new field appl_core in the application structure
 *              to APPL_NOT_DUMPING_CORE
 *
 *  Reviewer: Scott Hahn
 *  Risk: High (several components involved)
 *  Benefit or PTS #: 11577
 *  Testing: developer testing, special testing by Simon Tsang,
 *           corefile EAT, sched EAT, rmcmd EAT, controlc EAT
 *  Module(s): svr/server/paracore/dvp_pvpcore.c
 *             svr/server/nx/nx.c, nx.defs
 *             usr/sbin/allocator/alloc.defs, misc_rpcs.c, init_appl.c,
 *                                allocator.c, pspart.c
 *             usr/include/nx/schedule.h
 *             usr/include/allocsys.h
 *             usr/bin/pspart
 *
 * Revision 1.27  1994/11/19  03:04:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.26  1994/10/14  23:18:54  sdh
 * Merged in some R1.2 changes to handle the ACCOUNT
 * environment variable, that got lost in the R1.3
 * branch.
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 11097
 *  Testing:
 * 	EATS: controlc, rmcall, rmcmd
 * 	manual tests
 *  Module(s):
 * 	init_appl.c (allocator)
 * 	nx_initve.c (libnx)
 * 	macs.c (allocator)
 * 	alloc_types.h
 *
 * Revision 1.25  1994/09/16  16:39:40  sdh
 *  Changed to code that finds the number of available nodes
 *  in a partition to use the MIN of the part->nodes field or
 *  the number returned from num_nodes_in_map(). In effect, this
 *  cause the number to never be larger than the number of node
 *  that should have booted.
 *
 *  Reviewer: davidl
 *  Risk: low
 *  Benefit or PTS #: 10872
 *  Testing: manual testing; EATS: rmcall, rmcmd, controlc
 *  Module(s):
 * 	mkpart_rpc.c
 * 	init_appl.c
 *
 * Revision 1.24  1994/07/19  20:28:28  mag
 * Correct crash/bad handling of -rlx option on anchored rectangles
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10206, 10210
 *  Testing: developer, EATS: rmcmd, rmcall
 *  Module(s): init_appl.c
 *
 * Revision 1.23  1994/06/17  17:32:45  sdh
 * Func BuildMatchingBitmap() was called with 3 args instead of 4.
 *
 *  Reviewer: mag
 *  Risk: Medium
 *  Benefit or PTS #: 9871
 *  Testing: manual
 *  Module(s):
 * 	init_appl.c
 *
 * Revision 1.22  1994/06/13  17:13:44  sdh
 * Changed debug messages to go through debug print routine.
 *
 *  Reviewer: mag
 *  Risk: Low
 *  Benefit or PTS #:
 *  Testing: EATS
 *  Module(s):
 *         cmds_libs/src/usr/sbin/allocator/tiles.c
 *         cmds_libs/src/usr/sbin/allocator/allocator.c
 *         cmds_libs/src/usr/sbin/allocator/allocutils.c
 *         cmds_libs/src/usr/sbin/allocator/conflict.c
 *         cmds_libs/src/usr/sbin/allocator/init_appl.c
 *         cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 *         cmds_libs/src/usr/sbin/allocator/mkpart_rpc.c
 *         cmds_libs/src/usr/sbin/allocator/rmpart_rpc.c
 *         cmds_libs/src/usr/sbin/allocator/schedule.c
 *         cmds_libs/src/usr/sbin/allocator/server_loop.c
 *         cmds_libs/src/usr/sbin/allocator/smd.c
 *         cmds_libs/src/usr/sbin/allocator/tiles.c
 *
 * Revision 1.20  1994/06/01  20:49:13  mag
 * Mesh utilities changes adding Node Attributes
 *  Reviewer: cfj, sdh, shala
 *  Risk: High
 *  Benefit or PTS #: Needed for MP support
 *  Testing: EATS: rmcall, rmcmd, sched
 *  Module(s): Makefile, alloc.defs, alloc_types.defs, allocator.c init_appl.c
 * 	    misc_rpcs.c, mkpart_rpc.c, schedule.c, tiles.c, tiles.h,
 * 	    attributes.c (new), attributes.h (new)
 *  Related: libnx, server, emulator, bootmesh, mkpart, showpart, lspart
 *
 * Revision 1.19  1994/04/06  19:40:15  sdh
 * Merge of PTS# 8893 from R1.2 branch.
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #: 8893
 *  Testing:
 *  Module(s):
 *
 * Revision 1.15.2.4  1994/04/06  19:21:12  sdh
 * Change the call to ok_with_MACS() and add some more cleanup calls.
 *  Reviewer: jkearns
 *  Risk: Medium
 *  Benefit or PTS #: 8893
 *  Testing: EATS: sched, controlc, rmcmd, rmcall
 *  Module(s): init_appl.c macs.c macs.h tiles.c tiles.h
 *
 * Revision 1.15.2.3  1994/01/25  18:25:54  carbajal
 *  7526 PARAGON      OPEN      M        carbajal  simont              R1.2 WW47
 *       MESH UTILS   14-DEC-93 **       14-DEC-93 14-DEC-93
 *       -nd HxW:N (HxW:N invalid) gives non-rectangular node sets in a full
 *       config sys
 * Issue: In loading an application there were no checks made to ensure that
 * the rectangle you asked for did not exceed the dimensions of the partition
 * you were allocating out of. The fix is to make sure that the rectangle you
 * asked for (HxW:N) actually fits in the parent partition.
 *
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7526
 *  Testing: rmcall, rmcmd, sched EATs, bug report
 *  Module(s): init_appl.c (allocator)
 *
 * Revision 1.15.2.2  1994/01/06  22:56:19  carbajal
 * 7514 PARAGON      OPEN      H        carbajal  gregt               R1.2 WW49
 *       MESH UTILS   16-DEC-93 2        14-DEC-93 14-DEC-93
 *       pspart shows incorrect PRI values
 * (SPS partitions were always setting application priority to 5)
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7514
 *  Testing: bug report
 *  Module(s): init_appl.c
 *
 * Revision 1.15.2.1  1993/12/29  17:42:30  carbajal
 * Store group id with application structure.
 *  Reviewer: John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 7425
 *  Testing: EATs, bug report
 *  Module(s): init_appl.c
 *
 * Revision 1.15  1993/12/01  01:32:19  carbajal
 * check for proper use of -plk, use time() instead of gettimeofday()
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.14  1993/11/18  20:23:52  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.13  1993/11/17  02:54:05  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.12  1993/11/10  02:41:09  carbajal
 * Capture rectangular dimensions for application loading
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: Implement R1.2 User Model
 *  Testing: developer and EATs
 *  Module(s): init_appl.c
 *
 * Revision 1.11  1993/11/04  17:47:00  carbajal
 * Pass number of nodes wanted into assign_rect_lp() to prevent overrunning
 * the node list with numnodes is a prime number.
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: Fixes rmcmd EAT failure
 *  Testing: Developer and rmcmd EATS
 *  Module(s): allocutils.c,allocutils.h,mkpart_rpc.c,tiles.c,init_appl.c
 *
 * Revision 1.10  1993/10/27  01:59:12  carbajal
 * Use effective uid and gid to check permission
 * Use new nx_info structure
 * record the aspect ratio of the assigned node set
 *
 * Revision 1.9  1993/09/21  21:33:00  carbajal
 * Use EAOVLP if -tile is on. PTS #6493
 *
 * Revision 1.8  1993/09/03  22:03:37  shala
 * Clean up datastructures if perk_active_partition fails #PTS 6490.
 * Fixed by carbajal.
 *
 * Revision 1.7  1993/08/28  01:01:15  carbajal
 * Call MACS if do_MACS is set. PTS #6141
 *
 * Revision 1.6  1993/07/22  16:57:25  carbajal
 * Initialize all pointers to null
 *
 * Revision 1.5  1993/07/20  18:10:24  carbajal
 * Always set default_szflg to FALSE. This fixes the pts bug where the
 * allocator was trying to load apps onto bad nodes
 *
 * Revision 1.4  1993/07/19  17:38:39  carbajal
 * Set nx_acctid for MACS
 *
 * Revision 1.3  1993/07/18  19:47:37  carbajal
 * Added nx_acctid to stub for MACS
 *
 * Revision 1.1  1993/05/25  23:54:13  carbajal
 * New module dealing with init_appl() RPC.
 *
*/
#include <sys/dir.h>
#include <signal.h>
#include <stdio.h>
#include <mach/mach.h>
#include <mach_error.h>
#include <mach/mig_errors.h>
#include <mach/message.h>
#include <mach/norma_special_ports.h>
#include <mach/mach_host.h>
#include <sys/errno.h>
#include <assert.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/defines.h>
#include <nx/bitmap.h>
#include <nx/hash.h>
#include <nx/schedule.h>
#include <nx/allocator.h>
#include <nx/smd.h>
#include <nx/utils.h>
#include <nx/writepart.h>
#include "debug.h"
#include "macros.h"
#include "conflict.h"
#include "tiles.h"
#include "macs.h"
#include "allocutils.h"
#include <nx/alloc_types.h>

extern HASH_TBL_T              part_tbl[];
extern HASH_TBL_T              appl_tbl[];
extern int signal_debug,sched_debug,allocation_debug,attrib_debug;
extern int init_app_cnt;
extern int do_MACS;
extern alloc_config_t	alloc_config;
extern  APPL_T  *new_appl_ptr;
extern PART_T* root;

#ifdef iPSC860_COMPAT
/*
 * Hypercube mesh mapping
 */
extern LP_MAP_T mesh_map;
#endif iPSC860_COMPAT


/*
 * init_appl() - Exported MIG routine to allocate a set of nodes for an
 *               application to run in.
 */
int
init_appl(serv_port, uid, e_uid, gid, e_gid, nx_acctid, nx_info, rows, cols, 
		pgroup, selector_cnt, selector_buf, selector_buflen,
		nd_list, nd_listCnt, error)
mach_port_t	serv_port;	/* Server port */
uid_t		uid;		/* Uid of calling process */
uid_t		e_uid;		/* effective Uid of calling process */
gid_t		gid;		/* Gid of calling process */
gid_t		e_gid;		/* Effective Gid of calling process */
gid_t		nx_acctid;	/* NX accounting id (MACS) of calling process */
NX_INIT_T	nx_info;	/* NX init values */
int		*rows;		/* Number of rows allocated to application */
int		*cols;		/* Number of cols allocated to application */
pid_t		pgroup;		/* Process group of calling process */
int		selector_cnt;   /* # of selector strings */
char*		selector_buf;   /* buffer containing selector strings */
int		selector_buflen;/* size of selector_buf */
LP_MAP_T	*nd_list;	/* Array of nodes allocated */
unsigned int	*nd_listCnt;	/* Size of the array */
int		*error;		/* Errno */
{
	APPL_T	 *appl;		/* New application */
	PART_T	 *part;		/* Partition from which to allocate nodes */
	BITMAP_T *appl_bitmap;	/* Application's bitmap */
	BITMAP_T  *selector_mask; /* Mask of nodes matching selector criteria */
	BITMAP_T  *available_nodes;
	BITMAP_T  *free_nodes;
	BITMAP_T  *available_and_free_nodes;
	LAYER_T	 *layer;
	LAYER_T	 *p;
	int	 ret,size,new_size,default_szflg,i;
	unsigned long	myrow,mycol;


	*rows = nx_info.rows;
	*cols = nx_info.cols;

	/* initialize all pointers to NULL */
	appl = (APPL_T *)0;
	part = (PART_T *)0;
	appl_bitmap = (BITMAP_T *)0;
	available_nodes = (BITMAP_T *)0;
	free_nodes = (BITMAP_T *)0;
	available_and_free_nodes = (BITMAP_T *)0;
	layer = (LAYER_T *)0;
	p = (LAYER_T *)0;
	*nd_list = (LP_MAP_T )0;
	selector_mask = (BITMAP_T *)0;

	default_szflg = FALSE;
	layer = (LAYER_T *)0;

/*
 * See if there is already an application for this pgroup in application table.
 */
	if (HASH_LOOKUP_APPL(pgroup) != (APPL_T *) 0) {
		*error = EAEXIST; /* Application exists for process group */
		goto internal_error;
	}

/*
 * Lookup partition.
 */
	part = HASH_LOOKUP_PART(nx_info.inode);

	if (part == (PART_T *) 0) {
		*error = EPINVALPART;	/* Partition not found */
		goto internal_error;
	}

	if (BuildMatchingBitmap(selector_cnt, selector_buf, 0, &selector_mask) < 0) {
	    *error = EINVAL;
	    goto internal_error;
        }


	if (selector_mask != (BITMAP_T*)0)
	  available_nodes = AND_bitmaps(part->bitmap, selector_mask);
	else
	  available_nodes = bitmap_clone(part->bitmap);

	free_nodes = bitmap_clone(part->bitmap);
	build_free_bitmap(part, free_nodes);

	available_and_free_nodes = AND_bitmaps(available_nodes, free_nodes);

	if (signal_debug || sched_debug)
		printf(
		  "\ninit application uid %d gid %d pgroup %d size %d rows %d cols %d \
		  anchor node %d  plk %d priority %d in part->inode %d parent \
		  layer 0x%x\n",
		  uid,gid,pgroup,nx_info.size,nx_info.rows,nx_info.cols,
		  nx_info.start_node,nx_info.plk,nx_info.priority,
		  nx_info.inode,part->parent_sched_lyr);

	/* See if they have specified -plk */

	if (nx_info.plk == 1)

		/* They have specified it, so check to see if it is legal */

		if (part->gang_ancestor && alloc_config.reject_plk){

			/* we are running in a gang scheduled partition and
			 * the configuration file says to reject use of -plk
			*/

			*error = EAREJPLK;
			goto internal_error;
		}

	if (nx_info.cols > 0 && nx_info.rows > 0){

		/* We want a rectangle */

		size = nx_info.cols * nx_info.rows;

		if ((long)nx_info.start_node == -1 && nx_info.relaxed) {
		    *error = EINVAL;
		    goto internal_error;
		}

	} else {

		/* If relaxed, reduce size to those available and free in partition */

		if (nx_info.relaxed) {
			int nAvailable = num_nodes_in_map(available_and_free_nodes);
			size = MIN(nx_info.size, nAvailable);
		} else
			size = nx_info.size;
	}

	if (part->nodes < size) {
		*error = EPXRS;		/* Exceeds partition resources */
		goto internal_error;
	}

	if (size == 0) {

		/* default is all nodes of partition that match selector */

		if (nx_info.relaxed)
		    size = num_nodes_in_map(available_and_free_nodes);
		else
		    size = MIN(part->nodes, num_nodes_in_map(available_nodes));

		debug_allocation(5,"init_appl: size <- %d\n", size);

		if (size == 0) {
			*error = EPXRS;
			goto internal_error;
		}
	}

	new_size = size;

	if (!check_access(e_uid,e_gid,part->owner,part->group,EXEC,part->protection))
	{
		*error = EACCES;	/* Permission denied */
		goto internal_error;
	}

	/* Verify with MACS that the job can run */

	if (do_MACS)
                if ((nx_info.macs_account == -1) ||
                    (!ok_with_MACS(nx_info.macs_account,uid,size))){
			*error =  EANXACCT;	/* MACS Permission denied */
			goto internal_error;
		}

	/* See if we are specifying a rectangle anchored at a given node */

	if ((long)nx_info.start_node != -1){

		/* make sure the anchor node is within our node space */

		if ((long)nx_info.start_node >= 0 && nx_info.start_node < part->slots){

			/* yes we are, so create a bitmap to represent the node set */

			debug_sched(5,"anchor phys node %d\n",part->lp[nx_info.start_node]);

			if (bitmap_provide_coord(part->lp[nx_info.start_node],
				part->bitmap->rows,part->bitmap->cols, &myrow,&mycol)){

					debug_sched(5,"myrow %d mycol %d\n",myrow,mycol);

					/* Make sure that the dimensions don't exceed 
					 * the partition
					*/

					if (nx_info.rows * nx_info.cols > part->slots){
						*error = EPXRS;
						goto internal_error;
					}

					appl_bitmap = bitmap_clone(part->bitmap);
					init_bitmap(0,appl_bitmap);

					/* Construct the rectangular request */

					if (bitmap_fill_rect(appl_bitmap,1,
						mycol,myrow,*cols,*rows) == -1){

						/* The rectangle is not possible */

						*error = EPXRS;
						goto internal_error;
					}  
					
					/* If relaxed, don't ask for non-matching nodes */

					if (nx_info.relaxed) {
						if (bitmap_mask(available_and_free_nodes,
						  appl_bitmap) == -1)
							goto internal_error;
							size = num_nodes_in_map(appl_bitmap);
							new_size = size;
						}

					/* See if there is such a rectangle in the partition */

					if (!bitmap_space_there(appl_bitmap,part->bitmap)){

						/* No such rectangle */

						*error = EPXRS;
						goto internal_error;
					}

					/* We have a rectangle ! */

					if (sched_debug)
						debug_print_bitmap(DEBUG_SCHED, appl_bitmap);
			}
		}
		else {
		  /* 
		   * nx_info.start is declared as an unsigned long - need to make
		   * it signed to do the comparison 
		   */
		  if((long)nx_info.start_node < 0) {
		    *error = EINVAL;   /* specified a negative anchor node (not -1) */
			debug_sched(5,"start_node is negative: %d\n", nx_info.start_node);
		  }
		  else {
		    *error = EPXRS;    /* specified a node beyond our partition */
			debug_sched(5,"start_node is too big: %d\n", nx_info.start_node);
		  }

		  goto internal_error;
		}
	}

	/*
 	* Allocate space for node list to be returned, leave room for "host" node.
 	*/

        *nd_list = (LP_MAP_T) MALLOC(sizeof(int) * (size + 1));
	if (*nd_list == (LP_MAP_T) 0) {
		*error = EPALLOCERR;	/* Allocator internal error */
		goto internal_error;
	}

	*error = find_best_tile(default_szflg,size,part,&part->child_sched_lyr,
			&layer,&appl_bitmap,nd_list,rows,cols, nx_info.relaxed, selector_mask);

	if (*error != 0)
		goto internal_error;

	debug_allocation(5,"init_appl layer 0x%x\n",layer);

	/*
 	* Allocate application.
 	*/

	if ((appl = ALLOC_APPL()) == (APPL_T *) 0) {
		*error = EPALLOCERR;	/* Allocator internal error */
		goto internal_error;
	}

	if ((long)nx_info.start_node != -1){

		/* we need to assign a node list, this step was skipped
		 * by find_best_tile() and needs to be performed here
		*/

		assign_rect_lp(appl_bitmap,size,mycol,myrow,*cols,*rows,*nd_list);

		for (i = 0; i < size; i++){
			ret = node_in_map(part->bitmap,(*nd_list)[i]);
			if (ret != 0){
				if (ret == -1)
					*error = EPXRS;
				else
					*error = EPBXRS;
				goto internal_error;
			}
		}
	}

	/*
 	* Initialize application.
 	*/

	appl->type = APPL;
	appl->bitmap = appl_bitmap;
	appl->lp = *nd_list;
	appl->size = size;

	bitmap_encl_rect(appl->bitmap,cols,rows,&(appl->ncols),&(appl->nrows));

	/* check to see if this is a contiguous rectangle */

	if (appl->ncols * appl->nrows != appl->size){

		/* no, so set values accordingly */

		appl->nrows = -1;
		appl->ncols = -1;
		*rows = 1;
		*cols = appl->size;
	} else{

		/* this is a contiguous rectangle */

		*rows = appl->nrows;
		*cols = appl->ncols;
	}

	debug_allocation(5,"appl->nrows %d appl->ncols %d\n",
		appl->nrows,appl->ncols);

	/* Check to see what kind of layer we are loading this into */

	if (layer->part->sched == GANG){
		debug_sched(5,"loading application into a gang scheduled partition\n");
		appl->priority = nx_info.priority;

	} else if (layer->part->sched == SPACE_SHARE) {
		debug_sched(5,"loading application into a space shared partition\n");
		appl->priority = nx_info.priority;

	} else{

	/* On a standard scheduled partition, set it to the
	* infinite, 2*MAX is close enough
	*/

		debug_sched(5,"loading application into a std scheduled partition\n");
		appl->priority = STD_PRI; 
	}

	init_app_cnt++;
	appl->rolled_in = 0;
	appl->elapsed = 0;
	appl->pgroup = pgroup;
	appl->uid = uid;
	appl->gid = gid;
	appl->nx_acctid = nx_info.macs_account;;
	appl->appl_stat = APPL_NOT_STARTED;
	appl->conflict.list_size = 0;
	appl->conflict.n_list = 0;
	appl->conflict.free_index = 0;
	appl->conflict.list = NULL;
	appl->start_time = 0;
	time(&(appl->start_time));
#ifdef PARACORE
	appl->appl_core = APPL_NOT_DUMPING_CORE;
#endif /* PARACORE */

	add_consumer_to_tile(TRUE,(CONSUMER_T *)appl,&layer,
	  &appl->parent_lyr,&part->child_sched_lyr);

/*
 * Insert new application in application table.
 */

	if (HASH_INSERT_APPL(pgroup, appl) == -1) {

		/* XXX need to remove consumer from tile here */
		remove_consumer_from_tile(FALSE,TRUE,(CONSUMER_T *)appl,&layer,
			appl->bitmap,appl->size,&part->child_sched_lyr);
		*error = EPALLOCERR;	/* Allocator internal error */
		goto internal_error;
	}

	/* Build the application's conflict list */

	build_conflict_list((CONSUMER_T *)appl,part);

	/* If the partition has a current priority of NO_PRI then this is
	 * the first application to be loaded into the partition. We need to
	 * add the partition to its parent's scheduling layers to make it active
	*/

	if (part->cur_priority == NO_PRI){
		set_layer_pri(layer);
		set_cur_part_pri(part);

		if (perk_active_partition(part) != 0){

			/* we failed to add our partition to our parent's scheduling
			 * layers. This can happen if we are not allowing any overlap
			 * we need to remove this application from our partition's
			 * scheduling layer
			*/

			remove_consumer_from_tile(FALSE,TRUE,(CONSUMER_T *)appl,&layer,
				appl->bitmap,appl->size,&part->child_sched_lyr);

			/* reset the layer and partition priorities */

			if (layer != (LAYER_T *)0)
				set_layer_pri(layer);

			set_cur_part_pri(part);
			remove_from_all_conflict_lists((CONSUMER_T *)appl);

			*error = EAOVLP;

			/*
			* Remove application from hash table
			*/

			HASH_REMOVE_APPL(appl->pgroup);
			goto internal_error;
		}
	} else

		/* Record the effect of adding a new application to this layer. */ 

		perk_player_pri(layer);

	/*
	 * Need to halt the application until it is time to roll it in,
	 * but we can't signal it yet because the default action for
	 * SIGGSTOP on a process that is not part of a parallel application
	 * is to kill the process and since we have not yet returned to the
	 * server the process is not yet marked as parallel. 
	 *
	 * What we do instead is to hold onto the new application's pgroup
	 * so that we can signal it in schedule after we have returned.
	 */

	new_appl = pgroup;
	new_appl_ptr = appl;
	
	/* Tell smd that we have started the application */

	notify_smd(new_appl_ptr, APP_START);
	
	/*
	 * Normal exit, everything went OK.
	 */

	*nd_listCnt = new_size + 1;         /* one extra for host node */
	*error = ESUCCESS;

#ifdef iPSC860_COMPAT
	/*
	 * For hypercube we must do gray code mapping of the
	 * node list.
	 */

	if (is_ipsc860()) {
		for (i = 0; i < new_size; i++) {
			(*nd_list)[i] = mesh_map[(*nd_list)[i]]; 
			debug_allocation(5,
			  "(*nd_list)[%d] = mesh_map[(*nd_list)[%d]];: (*nd_list)[%d] = %d\n",
			  i, i, i, (*nd_list)[i]);
		}
	}
#endif iPSC860_COMPAT

	if (available_nodes!= (BITMAP_T *)0)
		FREE((void*)available_nodes);

	if (selector_mask!= (BITMAP_T *)0)
		FREE((void*)selector_mask);

	return 0;

internal_error:

	if (*nd_list != (LP_MAP_T )0)
		FREE((void *) nd_list);

	if ( appl != (APPL_T *)0)
		FREE((void *) appl);

	if ( appl_bitmap != (BITMAP_T *)0)
		FREE((void *) appl_bitmap);

	if (layer != (LAYER_T *)0)
		/* see if we need to cleanup any work done on the
		 * layer, or list of layers
		*/
		cleanup_layer_list(FALSE,TRUE,&(part->child_sched_lyr),&layer);

	*nd_listCnt = 0;

	if (available_nodes!= (BITMAP_T *)0)
		FREE((void*)available_nodes);

	if (selector_mask!= (BITMAP_T *)0)
		FREE((void*)selector_mask);

	if (free_nodes!= (BITMAP_T *)0)
		FREE((void*)free_nodes);

	if (available_and_free_nodes!= (BITMAP_T *)0)
		FREE((void*)available_and_free_nodes);

	return 0;
}


