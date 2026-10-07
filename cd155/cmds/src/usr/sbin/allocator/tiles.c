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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/tiles.c,v 1.24 1994/11/19 03:04:33 mtm Exp $
 *
*/

/* History:
 *      $Log: tiles.c,v $
 * Revision 1.24  1994/11/19  03:04:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.23  1994/11/18  18:00:42  sdh
 * Fixed 2 SIGSEGV problems.
 * 1) In migrate_objects_to_tile(), changed the check at the beginning
 * of the routine, to check *my_layer and *layer_list instead of **my_layer
 * and **layer_list.
 * 2) In migrate_objects_to_tile(), changed the layer loop, so the next
 * pointer is saved before calling remove_consumer_from_tile() in case
 * the layer struct is freed during the call.
 *
 *  Reviewer: gregt
 *  Risk: low
 *  Benefit or PTS #: 11381
 *  Testing:
 * 	EATS: rmcall, rmcmd, sched
 * 	manual tests
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/allocator/tiles.c
 *
 * Revision 1.22  1994/07/19  20:27:07  mag
 * Correct handling of bad/missing nodes.
 *  Reviewer: none
 *  Risk: Low
 *  Benefit or PTS #: 10177, 10178, 10179
 *  Testing: developer, EATS: rmcmd, rmcall
 *  Module(s): tiles.c
 *
 * Revision 1.21  1994/07/19  17:31:24  sdh
 * Added code to find_best_tile() to take bad nodes into consideration
 * when trying to allocate rectangle that had a sz specification.
 *
 *
 *  Reviewer:
 *  Risk: Low
 *  Benefit or PTS #: 9561
 *  Testing: developer, EATS: sched, rmcall, rmcmd, controlc
 *  Module(s):
 * 	tiles.c
 *
 * Revision 1.20  1994/06/13  16:40:48  sdh
 * Changed debug messages to go through debug print routine.
 *
 *  Reviewer: mag
 *  Risk: Low:
 *  Benefit or PTS #:
 *  Testing: EATS
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/allocator/tiles.c
 * 	cmds_libs/src/usr/sbin/allocator/allocator.c
 * 	cmds_libs/src/usr/sbin/allocator/allocutils.c
 * 	cmds_libs/src/usr/sbin/allocator/conflict.c
 * 	cmds_libs/src/usr/sbin/allocator/init_appl.c
 * 	cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 * 	cmds_libs/src/usr/sbin/allocator/mkpart_rpc.c
 * 	cmds_libs/src/usr/sbin/allocator/rmpart_rpc.c
 * 	cmds_libs/src/usr/sbin/allocator/schedule.c
 * 	cmds_libs/src/usr/sbin/allocator/server_loop.c
 * 	cmds_libs/src/usr/sbin/allocator/smd.c
 * 	cmds_libs/src/usr/sbin/allocator/tiles.c
 *
 * Revision 1.19  1994/06/02  18:14:26  mag
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
 * Revision 1.18  1994/04/06  19:41:09  sdh
 * Merge of PTS# 8893 from R1.2 branch.
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #: 8893
 *  Testing:
 *  Module(s):
 *
 * Revision 1.15.2.3  1994/04/06  19:26:03  sdh
 * Broke up remove_consumer_from_tile() into 2 routines, remove_consumer_from_tile()
 * and cleanup_layer_list().
 *  Reviewer: jkearns
 *  Risk: Medium
 *  Benefit or PTS #: 8893
 *  Testing: EATS: sched, controlc, rmcmd, rmcall
 *  Module(s): init_appl.c macs.c macs.h tiles.c tiles.h
 *
 * Revision 1.15.2.2  1994/02/09  00:18:25  carbajal
 *   7548 PARAGON      OPEN      C        carbajal  simont              R1.2 WW49
 *        MESH UTILS   02-FEB-94 0        02-FEB-94 15-DEC-93
 *        exception raise when run non-ovlp jobs in subpart. of compute (1 uses
 *        -sz HxW)
 *
 *  Reviewer: cameron
 *  Risk: low
 *  Benefit or PTS #: 7548
 *  Testing: bug report, resource mgmt EATs, scheduling EATs
 *  Module(s): tiles.c (allocator)
 *  Issue: If you requested a rectangle and the allocator could fit the
 *  request in an existing layer it never assigned the nodes which caused
 *  the server to panic.
 *
 * Revision 1.15.2.1  1994/01/06  23:04:55  carbajal
 *  7479 PARAGON      OPEN      C        carbajal  simont              R1.2 WW47
 *       MESH UTILS   03-JAN-94 **       11-DEC-93 11-DEC-93
 *       Allocator dies/system crash when run non-ovlp apps in sps part. using
 *       -sz HxW
 * (Problem was no bitmap was ever being assigned, and when the application was
 * released via exit, the allocator failed on an assert indicating the bitmap
 * was null.)
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7479
 *  Testing: bug report
 *  Module(s): tiles.c
 *
 * Revision 1.15  1993/12/01  01:41:00  carbajal
 * Set start time and elapsed time of an active partition
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.14  1993/11/18  20:24:08  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.13  1993/11/17  02:55:39  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.12  1993/11/10  02:43:02  carbajal
 * Better debug statements
 *  Reviewer: NONE
 *  Risk: LOW (only effects debug mode)
 *  Benefit or PTS #: R1.2 Usage Model
 *  Testing: Developer and EATs
 *  Module(s): tiles.c
 *
 * Revision 1.11  1993/11/04  17:47:26  carbajal
 * Pass number of nodes wanted into assign_rect_lp() to prevent overrunning
 * the node list with numnodes is a prime number.
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: Fixes rmcmd EAT failure
 *  Testing: Developer and rmcmd EATS
 *  Module(s): allocutils.c,allocutils.h,mkpart_rpc.c,tiles.c,init_appl.c
 *
 * Revision 1.10  1993/10/27  02:04:30  carbajal
 * handle rectangular node set request.
 *
 * Revision 1.9  1993/09/24  17:20:34  carbajal
 * Don't call insert_in_schedlist() in find_best_tile() until we
 * are sure that we have found a good layer. PTS #6739
 *
 * Revision 1.8  1993/09/21  21:35:05  carbajal
 * Added use of EAOVLP when -tile is on and overlap happens. PTS #6493
 *
 * Revision 1.7  1993/09/03  22:15:15  shala
 * Clean up data structures if perk_active_partition fails #PTS 6490
 * Make perk_active_partition return a status PTS #6490
 * Fixed by carbajal.
 *
 * Revision 1.6  1993/08/31  01:53:30  carbajal
 * Fixed PTS #6384. remove_from_partlist was incorrectly updating
 * the linked list
 *
 * Revision 1.5  1993/08/23  17:22:01  carbajal
 * Added support for part->sched_list maintenance. PTS #6095
 *
 * Revision 1.4  1993/07/29  00:31:05  carbajal
 * Use pointers alloc_prev and alloc_next in PART_T to remove partitions
 * from allocation layers.
 * remove_consumer_from_partition is now savy about these new pointers
 *
 * Revision 1.3  1993/07/20  18:14:03  carbajal
 * If we fail in find_best_tile() make sure we free any allocated
 * data structures.
 *
 * Revision 1.2  1993/07/01  17:27:05  carbajal
 * In perk_active_partition() call build_conflict_list()
 *
 * Revision 1.1  1993/05/25  23:55:15  carbajal
 * New module dealing with the handling of allocation and scheduling layers
 * Code was pulled out of allocator.c
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
#include <errno.h>
#include <assert.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/defines.h>
#include <nx/bitmap.h>
#include <nx/hash.h>
#include <nx/schedule.h>
#include <nx/allocator.h>
#include <nx/utils.h>
#include "debug.h"
#include "macros.h"
#include "conflict.h"
#include "allocutils.h"
 

extern int
select_nodes(int strictly_rect,int default_szflg,int size, BITMAP_T *src_bitmap, BITMAP_T *dest_bitmap,
                LP_MAP_T src_lp, int src_lp_size,LP_MAP_T dest_lp,int *nrows,int *ncols);

/* Forward references */
void
remove_consumer_from_tile(int doing_migration,int sched_tile,CONSUMER_T *c,LAYER_T **my_layer,
		BITMAP_T *bitmap,int nodes,LAYER_T **layer_list);
int search_for_conflict_in_tile(CONSUMER_T *object,LAYER_T *my_layer);
void remove_from_partlist(LAYER_T **layer,PART_T *a);
void remove_from_schedlist(LAYER_T *my_layer);
void dump_schedlist(PART_T *part);
void insert_in_schedlist(LAYER_T *my_layer);
void cleanup_layer_list(int doing_migration,int sched_tile,
                        LAYER_T **layer_list,LAYER_T **my_layer);



extern int	conflict_debug,sched_debug,allocation_debug,
		signal_debug,compare_debug,tile,pspart_debug;
extern BITMAP_T *unusable_node_map;

/* 	Dump layer consumers 
 *
 *	Debug code to dump the consumers for a given scheduling layer
 *
 *	Parameters:
 *		my_layer	pointer to layer to dump
 *
 *
*/
void
dump_consumers(LAYER_T *my_layer)
{
	CONSUMER_T	*c;

	if (!sched_debug)
		return;

	debug_sched(5,"dump consumers in layer 0x%x\n",my_layer);
	for(c = my_layer->consumer; c != (CONSUMER_T *)0; c = c->next)
		if (c->type == PART)
			debug_sched(5,"consumer inode %d\n", ( (PART_T *)c)->inode);
		else
			debug_sched(5,"consumer pgroup %d\n", ( (APPL_T *)c)->pgroup);
}


/* 	Dump allocation layer consumers 
 *
 *	Debug code to dump the consumers for a given layer
 *
 *	Parameters:
 *		my_layer	pointer to layer to dump
 *
 *
*/
void
dump_alloc_consumers(LAYER_T *my_layer)
{
	PART_T	*p;

	if (!sched_debug)
		return;

	debug_sched(5,"dump allocation consumers in layer 0x%x\n",my_layer);
	if (my_layer) {
	    for(p = (PART_T *)my_layer->consumer; p != (PART_T *)0;p = p->alloc_next){
		assert(p->type == PART); 
		debug_sched(5,"consumer inode %d\n", p->inode);
	    }
	}
}		


/* Find the best tile to put our consumer in. Currently best is defined by a size policy and
 * does not take into account priorities. Layers are organized in least free to most free and
 * the best layer is the first layer that can accomodate the consumer.
 *
 *	Parameters:
 *		default_szflg		TRUE if we did not specify any size
 *		want_numnodes		size we are looking for
 *		part			pointer to the partition that contains the consumer
 *		layer_list      	pointer to the layer list pointer
 *		my_layer		pointer to a layer pointer, this will be the best layer
 *		my_bitmap		pointer to the consumer's bitmap
 *		my_nodes		pointer to the node list to be filled in
 *		nrows			pointer to integer indicating x dimension of request
 *		ncols			pointer to integer indicating y dimension of request
 *		selector_mask		NULL or ptr to bitmap indicating nodes matching selection
 *
 *	Returns:
 *		0			all is ok
 *		errno			otherwise
 *		
*/

int
find_best_tile(int default_szflg,int want_numnodes, PART_T *part, LAYER_T **layer_list,
		LAYER_T **my_layer,BITMAP_T **my_bitmap,LP_MAP_T *my_nodes,int *nrows,int *ncols,
		int relaxed, BITMAP_T* selector_mask)
{
	int		spec_bitmap,want_rect,foundit,strictly_rect,error,new_size,row,col,num_free;
	BITMAP_T	*rect;
	BITMAP_T	*matching_nodes;
	
	debug_sched(5,
	  "find_best_tile inode %d size %d part->slots %d rows %d cols %d bitmap 0x%x mask 0x%x\n",
	  part->inode,want_numnodes,part->slots,*nrows,*ncols,*my_bitmap, selector_mask);
	foundit = FALSE;
	rect = (BITMAP_T *)0;
	matching_nodes = (BITMAP_T *)0;

	/* We are looking for a specific bitmap */
	spec_bitmap = ( *my_bitmap != (BITMAP_T *)0);

	if (!spec_bitmap){
		if ( (*nrows != 0) && (*ncols != 0)){
			want_rect = TRUE;
			/* We are asking for a rectangle, allocate the rect bitmap now */
			if ((rect = allocate_bitmap(*ncols,*nrows)) == (BITMAP_T *) 0) {
				debug_allocation(5,"can't allocate bitmap %d\n",errno);
				goto internal_error;
			}
			init_bitmap(1, rect);
			if (allocation_debug){
				debug_allocation(5,"find_best_tile:alloc and init rect bitmap\n");
				debug_print_bitmap(DEBUG_ALLOC, rect);
			}
		}
		else
			want_rect = FALSE;
	}

	/* First try to find a rectangle in the existing layers */
	strictly_rect = TRUE;
	do{
	*my_layer = (LAYER_T *)0;
	/*
	* Find first layer with room in it. Layers are in order of least free
 	* space to most free space.
 	*/
	for (*my_layer=*layer_list; *my_layer != (LAYER_T *) 0; *my_layer=(*my_layer)->next) {
		debug_sched(5,"layer 0x%x my_layer->num_free %d\n",*my_layer,(*my_layer)->num_free);
		if (selector_mask && matching_nodes)
		    FREE((void*)matching_nodes);
		if (selector_mask) {
		    matching_nodes = AND_bitmaps((*my_layer)->bitmap, selector_mask);
		    num_free = num_nodes_in_map(matching_nodes);
		} else {
		    matching_nodes = (*my_layer)->bitmap;
		    num_free = (*my_layer)->num_free;
		}
		/* Adjust matching_nodes to take any bad nodes into consideration */
		if (unusable_node_map != (BITMAP_T *)0) {
			debug_allocation(5,"Adjust matching_nodes to remove any bad nodes\n");
			matching_nodes = AND_bitmaps(or_bitmaps(matching_nodes, unusable_node_map), invert_bitmap(unusable_node_map));
		}

		if (num_free >= want_numnodes){
			/* we MIGHT have a layer with room in it */
			if ( !spec_bitmap && !want_rect){
				/* if we are starting with a null bitmap then just grab any "size"
				 * number of nodes. We need to make a clone of the partition which
				 * contains the child layer that was passed in. This may or may not
				 * be the same partition as pointed to by the parameter part.
				 */
				*my_bitmap = bitmap_clone(part->bitmap);
				init_bitmap(0, *my_bitmap);
				if ( (new_size = select_nodes(strictly_rect, default_szflg,
				  want_numnodes, matching_nodes, *my_bitmap, part->lp,
				  part->slots,*my_nodes,nrows,ncols)) == -1){
					/* Nope keep trying */
					FREE((void *) *my_bitmap);
				}
				else{
					foundit = TRUE;
					/* we HAVE found a layer */
					break;
				}
			}
			else{
			/* We will get to here if we have a specific set of nodes in mind,
			 * ie my_bitmap != NULL OR we are looking for a rectangle
			*/
				if (spec_bitmap){
					/* we came in with a set of nodes in mind. 
					 * find space for them.
					 *
					 * force us out of the loop at one iteration
					 */
					foundit = TRUE;
					if (bitmap_space_there(*my_bitmap, matching_nodes)) {
						/* we found space */
						/* set new size to be the size of our partition */
						new_size = want_numnodes;
						break;
					}
				}
				else{
					/* we are looking for a rectangle */
					if (bitmap_find_space(rect, matching_nodes, &col, &row)) {
						*my_bitmap = bitmap_clone(part->bitmap);
						init_bitmap(0, *my_bitmap);
						bitmap_fill_rect(*my_bitmap, 1, col, row,
						  *ncols, *nrows);
						if (my_nodes != (LP_MAP_T *)0)
							assign_rect_lp(*my_bitmap, want_numnodes,
							  col,row, *ncols, *nrows, *my_nodes);

						if (allocation_debug){
						    debug_print_bitmap(DEBUG_ALLOC, *my_bitmap);
						}
						new_size = want_numnodes;
						foundit = TRUE;
						break;
					}
				}
			}
		}
	}
	if (foundit == TRUE)
		break;
	/* toggle value */
	if (strictly_rect == TRUE)
		strictly_rect = FALSE;
	else
		strictly_rect = TRUE;
	}while(strictly_rect == FALSE);

	if (selector_mask && matching_nodes)
	    FREE((void*)matching_nodes);

	if (*my_layer != (LAYER_T *) 0) {
		/*
	 	* We found a layer with room. Take layer out of list so
	 	* we can insert it back in its proper place.
 	 	*/
		REMOVE(*layer_list, *my_layer);
	}
	else {
		/*
	 	* We didn't find a layer with room, so we will allocate
	 	* a new one and clone bitmap of the parent partition.
	 	*/
		if ( deny_overlap(part, *layer_list) ) {
			/*
                         * We would overlap, return error.
                         */
			debug_allocation(5,"Error: overlap detected\n");
			/* Request overlaps with nodes in use */
                        error = EAOVLP; 
                        goto internal_error;
                }

		debug_allocation(5,"Need to allocate a new layer\n");
		if ((*my_layer = ALLOC_LAYER()) == (LAYER_T *) 0) {
			error = EPALLOCERR;	/* Allocator internal error */
			goto internal_error;
		}

		/* we make a copy of the bitmap for the partition in which our layer will 
		 * reside in.
		*/
		(*my_layer)->bitmap = bitmap_clone(part->bitmap);
		if ((*my_layer)->bitmap == (BITMAP_T *) 0) {
			debug_sched(5, "find_best_tile(1): freeing my_layer 0x%x\n", *my_layer);
			FREE((void *)*my_layer);
			error = EPALLOCERR;	/* Allocator internal error */
			goto internal_error;
		}

		(*my_layer)->part = part;
		(*my_layer)->priority = NO_PRI;
		(*my_layer)->num_free = part->nodes;
		(*my_layer)->consumer = (CONSUMER_T *) 0;
		(*my_layer)->prev = (LAYER_T *)0;
		(*my_layer)->next = (LAYER_T *) 0;
		(*my_layer)->sched_prev = (LAYER_T *)0;
		(*my_layer)->sched_next = (LAYER_T *) 0;

		debug_allocation(5,"layer 0x%x inode %d num_free %d\n",
			(*my_layer),(*my_layer)->part->inode,
			(*my_layer)->num_free);

		/* if we do not have a bitmap already then build one */
		if ( (*my_bitmap == (BITMAP_T *)0)){
			*my_bitmap = bitmap_clone((*my_layer)->bitmap);
			init_bitmap(0, *my_bitmap);
		}

		if (selector_mask) {
		    matching_nodes = AND_bitmaps((*my_layer)->bitmap, selector_mask);
		} else {
		    matching_nodes = (*my_layer)->bitmap;
		}

		if ( !spec_bitmap && !want_rect){
			/*
 			* At this point we have a layer that has room in it for size nodes.
 			* Allocate a bitmap structure for the application and select a set
 			* of size nodes from it.
 			*/
			if ((new_size = select_nodes(FALSE,default_szflg,want_numnodes,
			  matching_nodes, *my_bitmap, part->lp,part->slots,
			  *my_nodes,nrows,ncols)) == -1)
			{
				FREE((void *)*my_bitmap);
				FREE((void *)(*my_layer)->bitmap);
				debug_sched(5, "find_best_tile(2): freeing my_layer 0x%x\n", *my_layer);
				FREE((void *)*my_layer);
				if (num_nodes_in_map(matching_nodes) !=
				  num_nodes_in_map((*my_layer)->bitmap))
				    error = EPXRS;	
				else
				    error = EPBXRS;	
				if (selector_mask)
				    FREE((void *)matching_nodes);
				goto internal_error;
			}
		}	
		else{
			if (spec_bitmap){
				/* Adjust my_bitmap to take any bad nodes into consideration */
                		if (unusable_node_map != (BITMAP_T *)0) {
					debug_allocation(5,"Adjust my_bitmap to remove any bad nodes\n");
					*my_bitmap = AND_bitmaps(
						or_bitmaps(*my_bitmap, unusable_node_map), 
							invert_bitmap(unusable_node_map));
				}
				
			        if (allocation_debug){
				    debug_allocation(5,"looking for specific map\n");
				    debug_print_bitmap(DEBUG_ALLOC, *my_bitmap);
				    debug_print_bitmap(DEBUG_ALLOC, matching_nodes);
				}
				/* we have a bitmap, so we need to allocate space using it */	
				if (!bitmap_space_there(*my_bitmap,matching_nodes)){
					FREE((void *)(*my_layer)->bitmap);
					debug_sched(5, "find_best_tile(3): freeing my_layer 0x%x\n", *my_layer);
					FREE((void *)*my_layer);
					if (selector_mask)
					    FREE((void *)matching_nodes);
					error = EPNOMATCH;;
					goto internal_error;
				}
			}
			else{
				/* we are looking for a rectangle */
			        if (allocation_debug){
				    debug_allocation(5,"looking for a rectangle\n");
				    debug_print_bitmap(DEBUG_ALLOC, rect);
				    debug_print_bitmap(DEBUG_ALLOC, matching_nodes);
				}
				if (!bitmap_find_space(rect,matching_nodes, &col, &row)){
					/* we don't have space in the cloned bitmap.
					 * Either we've reject nodes 'coz of a selector (EPXRS)
					 * or something strange is happening (EPALLOCERR).
					 */
					FREE((void *)(*my_layer)->bitmap);
					debug_sched(5, "find_best_tile(4): freeing my_layer 0x%x\n", *my_layer);
					FREE((void *)*my_layer);
					if (selector_mask)
					    FREE((void *)matching_nodes);
					if (num_nodes_in_map(matching_nodes) !=
					  num_nodes_in_map((*my_layer)->bitmap))
					    error = EPXRS;
					else
					    error = EPALLOCERR;
					goto internal_error;
				}
				bitmap_fill_rect(*my_bitmap, 1, col, row,*ncols, *nrows);
				if (my_nodes != (LP_MAP_T *)0)
					assign_rect_lp(*my_bitmap,want_numnodes,col,row,
								*ncols,*nrows,*my_nodes);
				if (allocation_debug){
				    debug_print_bitmap(DEBUG_ALLOC, *my_bitmap);
				}
			}
			new_size = want_numnodes;
		}
		/* insert this into the scheduling list for round robin selection */
		insert_in_schedlist(*my_layer);

		if (selector_mask)
		    FREE((void *)matching_nodes);
	}
	
	return(0);

internal_error:
	if (rect != (BITMAP_T *)0)
		free_bitmap(rect);
	return(error);
}

/*
 *	Insert the consumer into the list of consumers for the given layer
 *
 *	Parameters:
 *		insert_layer	== 1 if we need to call REMOVE and INSERT_LAYER for tile
 *				   maintainence. This switch is primarily used when migrating
 *				   objects from one tile to another. The tile is removed from
 *				   the list of scheduling tiles prior to migrating and will be
 *				   added back once all the migration has taken place
 *		my_consumer	pointer to the consumer to insert
 *		my_layer	pointer to pointer to the tile to insert the consumer in
 *		layer_list	pointer to pointer to the list of layers in which to insert my_layer
 *				into
*/
void
add_consumer_to_tile(int insert_layer,CONSUMER_T *my_consumer,LAYER_T **my_layer,
			LAYER_T **parent_layer,LAYER_T **layer_list)
{
	int	nodes;
	BITMAP_T *bitmap;

	assert(my_consumer != (CONSUMER_T *)0);

	debug_sched(5,"add_consumer_to_tile: my_consumer 0x%x my_layer 0x%x consumer list 0x%x layer list 0x%x\n",
			my_consumer,*my_layer,(*my_layer)->consumer,*layer_list);

	if (my_consumer->type == PART){
		nodes = ( (PART_T *)my_consumer)->slots;
		bitmap = ( (PART_T *)my_consumer)->bitmap;
	}
	else{
		nodes = ( (APPL_T *)my_consumer)->size;
		bitmap = ( (APPL_T *)my_consumer)->bitmap;
	}

	/*
 	* Mark nodes in layer as being unavailable and update the count of free nodes.
 	*/
	(*my_layer)->num_free -= nodes;
	debug_allocation(5,"(*my_layer)->num_free %d nodes %d\n",(*my_layer)->num_free,nodes);
	bitmap_allocate_space(bitmap, (*my_layer)->bitmap);

	*parent_layer = *my_layer;

	/*
	 * Insert application in list of consumer objects
 	*/
	INSERT_FIRST((*my_layer)->consumer, my_consumer);
	
	/* See if we are supposed to go ahead and insert the layer back into the list
	 * of layers. The only time we really do not want to do this is when we are 
	 * migrating objects into this layer. In that case we move everything first
	 * and then reinsert the layer
	*/
	if (insert_layer){
		if ( *layer_list != (LAYER_T *)0)
			/* Remove tile from list of tiles */
			REMOVE(*layer_list,*my_layer);
		/*
 		*  Insert layer in the proper place in the list of layers.
 		*/
		INSERT_LAYER(*layer_list, *my_layer);
	}
	debug_sched(5,"added consumer to tile: my_consumer 0x%x my_layer 0x%x consumer list 0x%x layer list 0x%x\n",
			my_consumer,*my_layer,(*my_layer)->consumer,*layer_list);
}

/* Recursively add a partition to its parent partition's scheduling layers 
 *
 *	Parameters:
 *		part	pointer to the partition to add
 *
 *	Returns:
 *		0	is partition could be added to parent's scheduling layers
 *		-1	otherwise
*/
int
perk_active_partition(PART_T *part)
{
	LAYER_T	*layer;
	PART_T  *parent_part;
	int	nrows,ncols,parent_active,error;

	assert(part != (PART_T *) 0);

	time((time_t *)&(part->start_time));
	part->elapsed = 0;
	nrows = ncols = 0;
	parent_active = 0;

	debug_sched(5,"perk_active_partition inode %d\n",part->inode);

	if (part->parent_alloc_lyr)
		parent_part = part->parent_alloc_lyr->part;
	else
		return(0);

	/* See if we have a parent partition */
	if (parent_part != (PART_T *) 0){
		/* we have a parent */
		parent_active = (parent_part->cur_priority != NO_PRI);

		/* Now find a scheduling layer for the partition to reside in.
		 * Note that we look in our parent's list of scheduling layers
		*/

		error = find_best_tile(FALSE,part->slots,parent_part,
				&parent_part->child_sched_lyr,&layer,&(part->bitmap),(LP_MAP_T *)0,
				&nrows,&ncols, 0, (BITMAP_T*)0);
		if (error == 0){

			/* add partition to layer's list of consumers */
			add_consumer_to_tile(TRUE,(CONSUMER_T *)part,&layer,&part->parent_sched_lyr,
								&parent_part->child_sched_lyr);
			set_layer_pri(layer);
			set_cur_part_pri(parent_part);

			/* build the conflict list for this partition */
			build_conflict_list((CONSUMER_T *)part,parent_part);

			/* if our parent was not active then keep percolating the active partition
		 	* up through the scheduling layers
			*/
			if (!parent_active)
				if (perk_active_partition(parent_part) != 0){
					/* our request to percolate was refused so we need to
					 * clean up this tile
					*/
					remove_consumer_from_tile(FALSE,TRUE,(CONSUMER_T *)part,
						&layer,part->bitmap,part->nodes,
						&parent_part->child_sched_lyr);
					/* Adjust priorities */
					if (layer != (LAYER_T *)0)
						set_layer_pri(layer);
					set_cur_part_pri(parent_part);
					remove_from_all_conflict_lists((CONSUMER_T *)part);
					return(-1);
				}
			else
				perk_player_pri(layer);
			
		}
		else
			/* can't find a suitable tile */
			return(-1);
	}
	
	return(0);
}

/* Migrate objects into the specified tile. Objects are taken from
 * sibling tiles.
 *
 *	Parameters:
 *		do_start	== 1 go ahead and issue a gang_start()
 *				== 0 do not issue gang_start()
 *				
 *		my_layer	pointer to pointer to the tile to migrate objects to
 *		layer_list	pointer to pointer to list of layers
*/
void
migrate_objects_to_tile(int do_start,LAYER_T **my_layer,LAYER_T **layer_list)
{
	CONSUMER_T	*next,*c;
	LAYER_T		*alloc_layer,**parent_layer,*layer;
	BITMAP_T	*bitmap;
	int		is_part,size;
	LAYER_T		*next_lyr;

	alloc_layer = (LAYER_T *)0;

	if (*my_layer == (LAYER_T *)0 || *layer_list == (LAYER_T *)0)
		return;

	/* If this layer is full then just return */
	if ((*my_layer)->num_free == 0){
		debug_sched(5,"layer is full\n");
		return;
	}

	debug_sched(5,"migrate_objects_to_tile 0x%x %d\n",(*my_layer),(*my_layer)->num_free);

	/* First remove my_layer from the list of layers */
	REMOVE(*layer_list, *my_layer);

	/* Search through the list of layers for objects that will fit into my_layer */
	layer = *layer_list;
	while ((layer != (LAYER_T *)0) && ((*my_layer)->num_free > 0)) {
	        /* save the next pointer because the layer struct can 
		 * be freed in remove_objects_from_tile
		 */
	        next_lyr = layer->next; 
		/* Search through the consumer list for this layer */
		c = layer->consumer;
		while ( (c != (CONSUMER_T *)0) && ((*my_layer)->num_free > 0)){
			/* See if the consumer is a candidate */
			assert( (c->type == PART) || (c->type == APPL));
			if (c->type == PART){
				/* consumer is a partition */
				size = ((PART_T *)c)->nodes;
				bitmap = ((PART_T *)c)->bitmap;
				parent_layer = &((PART_T *)c)->parent_sched_lyr;
				alloc_layer = ((PART_T *)c)->parent_alloc_lyr;
				is_part = TRUE;
			}
			else{
				/* consumer is an application */
				size = ((APPL_T *)c)->size;
				bitmap = ((APPL_T *)c)->bitmap;
				parent_layer = &((APPL_T *)c)->parent_lyr;
				is_part = FALSE;
			}

			/* see if consumer will fit into this layer */
			if (size <= (*my_layer)->num_free){

				if (is_part)
					/* This consumer is a partition */
					debug_sched(5,"consumer is a partition, that will fit\n");
				else
					/* This consumer is an application */
					debug_sched(5,"consumer is an application that will fit\n");

				/* Consumer will fit, so take a look at the conflict list for this
 				 * consumer. If there aren't any objects on the conflict list that
				 * reside in this tile, then we can migrate the object
				*/
				if (!search_for_conflict_in_tile(c,*my_layer)){
					/* this does not conflict, so remove it from its 
					* current tile and place it in this one
					*/
					/* Grab the next consumer, before we change the
					* current one
					*/
					next = c->next;
					debug_sched(5,"migrating consumer\n");
					/* release the consumer from its scheduling tile */

					remove_consumer_from_tile(TRUE,SCHED_TILE,c,&layer,bitmap,
									size,layer_list);

					/* Add the consumer to this tile */
					add_consumer_to_tile(FALSE,c,my_layer,parent_layer,layer_list);

					/* Do we need to start the application. */
					if (do_start){
						if (is_part)
							/* This is a partition, call rollin() */
							rollin(TRUE,(PART_T *)c);
						else
							gang_start((APPL_T *)c);
					}
					c = next;
				}
				else
					c = c->next;
			}
			else
				c = c->next;
		}	
		layer = next_lyr;
	}

	/* Now place our layer in the list of layers */
	INSERT_LAYER(*layer_list, *my_layer);
}

/* Search through the conflict list for object, looking for any other object that is a 
 * resident of my_layer.
 *
 *	Parameters:
 *		object		pointer to the consumer whose conflict list we wish to search
 *		my_layer	pointer to layer we are looking for a conflict in
 *
 *	Returns:
 *		FALSE		object does not conflict with my_layer
 *		TRUE		object does conflict with my_layer
 *
*/
int
search_for_conflict_in_tile(CONSUMER_T *object,LAYER_T *my_layer)
{
	CONSUMER_T	*c;
	LAYER_T		*layer;
	conflict_list_t	*conflict;
	int		i,conflicts;

	assert(object->type == PART || object->type == APPL);

	if ( object->type == PART)
		debug_sched(5,"search_for_conflict_in_tile inode %d\n",( (PART_T *)object)->inode);
	else
		debug_sched(5,"search_for_conflict_in_tile pgroup %d\n",( (APPL_T *)object)->pgroup);
 
	conflict = get_conflict_list(object);

	conflicts = FALSE;

	/* For each object in the conflict list, check to see if the tile that it
	 * resides in is the same as my_layer
	*/
	for (i = 0; i < conflict->list_size; i++){
		c = conflict->list[i];
		if (c == (CONSUMER_T *)0)
			continue;
		assert(c != (CONSUMER_T *)0 && (c->type == PART || c->type == APPL));
		if (c->type == PART)
			layer = ( (PART_T *)c)->parent_sched_lyr;
		else
			layer = ( (APPL_T *)c)->parent_lyr;
		if (layer == my_layer){
			/* objects has conflicts with residents of this layer, bail out */
			conflicts = TRUE;
			break;
		}
	}

	return(conflicts);
}

/* Free the specified consumer from the tile in which it resides.
 *
 *	Parameters:
 *		sched_tile	== 1 if we are freeing a consumer from a scheduling tile
 *				== 0 if we are freeing a consumer from an allocation tile
 *		c		pointer to consumer to remove
 *		layer		pointer to pointer to layer in which consumer resides
 *		layer_list	pointer to pointer of list of layers in which layer resides
*/
void
free_object_from_layer(int sched_tile,CONSUMER_T *c,LAYER_T **my_layer,LAYER_T **layer_list)
{
	PART_T		*part;
	BITMAP_T	*bitmap;
	APPL_T		*appl;
	int		nodes;

	debug_allocation(5,"free_object_from_layer my_layer 0x%x layer_list 0x%x\n",*my_layer,*layer_list);
	/* see what type of object this is */
	if (c->type == PART){
		/* this is a partition */
		part = (PART_T *)c;
		/* get pointers to the bitmaps and the number of nodes to free */
		bitmap = part->bitmap;
		nodes = part->nodes;
	}
	else{
		appl  = (APPL_T *)c;
		/* get pointers to the bitmaps and the number of nodes to free */
		nodes = appl->size;
		bitmap = appl->bitmap;
	}

	/* remove the consumer from its parent layer */
	remove_consumer_from_tile(FALSE,sched_tile,c,my_layer,bitmap,nodes,layer_list);

}

/* Remove a consumer from a specified tile. 
 *
 *	Parameters:
 *		doing_migration	== 1 if we are removing the object to migrate
 *				   it to another layer
				== 0 otherwise
 *		sched_tile	== 1 if my_layer is a scheduling tile
 *				== 0 otherwise
 *		c		pointer to consumer object
 *		my_layer	pointer to layer which holds object
 *		bitmap		pointer to the consumer's bitmap
 *		nodes		number of nodes held by consumer
 *		layer_list	list of layers to which layer belongs
 *
 *		
*/
void
remove_consumer_from_tile(int doing_migration,int sched_tile,CONSUMER_T *c,LAYER_T **my_layer,
		BITMAP_T *bitmap,int nodes,LAYER_T **layer_list)
{
	PART_T	*parent_part;
	int	is_part;
	LAYER_T	*layer;

	assert(c != (CONSUMER_T *) 0);
	assert(c->type == PART || c->type == APPL);
	assert(*my_layer != (LAYER_T *) 0);
	assert(bitmap != (BITMAP_T *) 0);
	assert(*layer_list != (LAYER_T *)0);

	layer = (LAYER_T *)0;
	if (c->type == PART){
		is_part = TRUE;
		layer = ( (PART_T *)c)->parent_alloc_lyr;
	}
	else
		is_part = FALSE;

	if (allocation_debug || sched_debug){
		printf("remove_consumer_from_tile: my_layer 0x%x num free %d layer_list 0x%x nodes %d\n",
		       *my_layer,(*my_layer)->num_free,(*layer_list),nodes);
		if (is_part)
			printf("consumer part->inode: %d\n",((PART_T *)c)->inode);
		else
			printf("consumer appl->pgroup: %d\n",((APPL_T *)c)->pgroup);
	}

	/*
	 * Deallocate the nodes from our parent layer (which contains this
	 * consumer) and update the count of free nodes.
	 */
	bitmap_free_space(bitmap, (*my_layer)->bitmap);
	(*my_layer)->num_free += nodes;

	/*
	 * Remove consumer from list of consumer objects.
	 */
	if (!sched_tile)
		remove_from_partlist(my_layer,(PART_T *)c);
	else
		REMOVE((*my_layer)->consumer, c);

	cleanup_layer_list(doing_migration,sched_tile,layer_list,my_layer);

}

/* This routine will cleanup the layer list when objects are being removed
 * If the layer is empty after removing the object the layer will be removed 
 * from the list of layers and deallocated. If the layer is not empty the
 * layer will be reinserted into its proper place within the layer list
 *
 * Parameters:
 *	doing_migration		TRUE if being called by migrate_objects()
 *	sched_tile		TRUE if the scheduling layer list is being 
 *				manipulated
 *	layer_list		list of layers to manipulate
 *	my_layer		the layer in question
 *
 *	
*/
void
cleanup_layer_list(int doing_migration,int sched_tile,
			LAYER_T **layer_list,LAYER_T **my_layer)
{
	PART_T	*parent_part;

	/*
	* Remove the parent layer from its list of layers.
	*/
	parent_part = (*my_layer)->part;
	REMOVE(*layer_list,*my_layer);

	debug_sched(5,"parent_part %d nodes %d layer_list 0x%x\n",parent_part->inode,parent_part->nodes,*layer_list);

	if ((*my_layer)->num_free == parent_part->nodes) {
		/* the layer is empty, we can deallocate it */
		
	        debug_allocation(5,"my layer is empty\n");
	        debug_sched(5,"my layer is empty\n");

		/* if this is a scheduling tile, then cleanup the active layer */
		if (!doing_migration && sched_tile == SCHED_TILE)
			if (parent_part->active_lyr == *my_layer){ 
			    debug_allocation(5,"deactivate active layer\n");
			    debug_sched(5,"deactivate active layer\n");
			    debug_sched(5,"cleanup_layer_list: clearing active layer, part = 0x%x, active_lyr = 0x%x\n",
				parent_part, parent_part->active_lyr);
			    parent_part->active_lyr = (LAYER_T *)0;
			}

		if (sched_tile == SCHED_TILE)
			/* remove this scheduling tile from the scheduling list */
			remove_from_schedlist(*my_layer);

		FREE((void *) (*my_layer)->bitmap);
		debug_sched(5, "cleanup_layer_list: freeing layer 0x%x\n", *my_layer);
		FREE((void *) (*my_layer));
		*my_layer = (LAYER_T *)0;
	}
	else {
 	/*
	* Reinsert layer at the right spot in the list.
        */
		INSERT_LAYER(*layer_list, *my_layer);
		/* If this is a scheduling tile, then readjust the layer priorities */
		if (!doing_migration && sched_tile == SCHED_TILE)
			perk_player_pri(*my_layer);
	}

}

/* Insert partition into the list of allocation layer consumsers 
 *
 *	Parameters:
 *		my_layer	pointer to a layer
 *		a		pointer to the partition to add to my_layer's
 *				list of consumers
 *
*/
void
insert_in_partlist(LAYER_T *my_layer, PART_T *a)
{
	a->alloc_prev = (PART_T *)0;
	a->alloc_next = (PART_T *)my_layer->consumer;
	if (my_layer->consumer)
		((PART_T *)(my_layer->consumer))->alloc_prev = a;
	my_layer->consumer = (CONSUMER_T *)a;
}

/* Remove partition from the list of consumers in the allocation layer
 *
 *	Parameters:
 *		hd	head of linked list of allocation layer
 *		a	pointer to the partition to remove
 *
*/
void
remove_from_partlist(LAYER_T **layer,PART_T *a)
{
	if (a->alloc_next)
		a->alloc_next->alloc_prev = a->alloc_prev;
	if ( (*layer)->consumer == (CONSUMER_T *)a)
		(*layer)->consumer = (CONSUMER_T *)a->alloc_next;
	if (a->alloc_prev)
		a->alloc_prev->alloc_next = a->alloc_next;
	a->alloc_next = (PART_T *)0;
	a->alloc_prev = (PART_T *)0;
}

/* Insert the layer in the partition schedule list 
 *
 *	Parameters:
 *		my_layer	pointer to the layer to insert
 *
*/
void
insert_in_schedlist(LAYER_T *my_layer)
{
	LAYER_T	*last,*l;
	PART_T	*part;

	part = my_layer->part;
	dump_schedlist(part);
	/* find end of list */
	for (l = part->sched_list,last = part->sched_list; l != (LAYER_T *)0; l = l->sched_next)
		last = l;

	/* Did we find a last ? */
	if (last == (LAYER_T *)0){
		/* no, so assign this to the head of the list */
		part->sched_list = my_layer;
	}
	else{
		/* yes there was a valid last element in the list */
		last->sched_next = my_layer;
		my_layer->sched_prev = last;	
	}
	dump_schedlist(part);
}

/* Remove a layer from the partition's schedule list 
 *
 *	Parameters:
 *		my_layer	pointer to the layer to remove
 *
*/
void
remove_from_schedlist(LAYER_T *my_layer)
{
	PART_T	*part;

	part = my_layer->part;
	dump_schedlist(part);
	if (my_layer->sched_next)
		my_layer->sched_next->sched_prev = my_layer->sched_prev;
	if (part->sched_list == my_layer)
		part->sched_list = my_layer->sched_next;
	if (my_layer->sched_prev)
		my_layer->sched_prev->sched_next = my_layer->sched_next;
	my_layer->sched_next = (LAYER_T *)0;
	my_layer->sched_prev = (LAYER_T *)0;
	dump_schedlist(part);
}

/*	Debug only
 *
 *	Dump the layers on the partition's schedule list 
*/
void
dump_schedlist(PART_T *part)
{
	LAYER_T	*l;

	if (!sched_debug)
		return;
	debug_sched(5, "dumping scheduling list for part->inode %d\n",part->inode);
	for(l = part->sched_list; l != (LAYER_T *)0; l = l->sched_next)
		debug_sched(5, "layer 0x%x\n",l);
}

