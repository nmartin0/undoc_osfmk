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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/allocutils.c,v 1.9 1994/11/19 03:04:07 mtm Exp $
 *
 */

/* History:
 *	$Log: allocutils.c,v $
 * Revision 1.9  1994/11/19  03:04:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/09/16  17:20:46  sdh
 *  In assign_rect_lp() take into account any already assigned bits
 *  in assigning the nodes to a rectangle.
 *
 *  Reviewer: sdh (fix done by mag)
 *  Risk: meduim
 *  Benefit or PTS #: 10434
 *  Testing:
 * 	EATS: rmcall, rmcmd, controlc
 *  Module(s):
 * 	allocutils.c
 *
 * Revision 1.7  1994/06/13  16:41:31  sdh
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
 * Revision 1.6  1994/04/06  00:28:57  sdh
 * Merge PTS# 8448 into main branch from R1.2 branch.
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #: 8448
 *  Testing:
 *  Module(s):
 *
 * Revision 1.5.2.1  1994/04/06  00:18:09  sdh
 * Changed too_many_layers() to accept a LAYER_T instead of a PART_T
 * and just search the layer passed. Also changed deny_overlap to pass
 * a LAYER_T instead of a PART_T.
 *  Reviewer: jkearns
 *  Risk: medium
 *  Benefit or PTS #: 8448
 *  Testing: Ran the sched, controlc, rmcall and rmcmd portions of EATS
 *  Module(s): allocutils.c
 *
 * Revision 1.5  1993/11/18  20:23:48  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.4  1993/11/17  02:53:38  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.3  1993/11/10  02:38:40  carbajal
 * Changed parameter order in assign_rect_lp()
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: -sz HxW and -nd HxW:N for R1.2 User Model
 *  Testing: developer
 *  Module(s): allocutils.c, allocutils.h
 *
 * Revision 1.2  1993/11/04  17:46:29  carbajal
 * Pass number of nodes wanted into assign_rect_lp() to prevent overrunning
 * the node list with numnodes is a prime number.
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: Fixes rmcmd EAT failure
 *  Testing: Developer and rmcmd EATS
 *  Module(s): allocutils.c,allocutils.h,mkpart_rpc.c,tiles.c,init_appl.c
 *
 * Revision 1.1  1993/10/27  01:56:35  carbajal
 * New module
 *
*/

static char rcsid[] = "$Id: allocutils.c,v 1.9 1994/11/19 03:04:07 mtm Exp $";

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
#include <nx/writepart.h>
#include "debug.h"
#include "macros.h"

/* External data and functions */
extern BITMAP_T        *unusable_node_map;
/*
 * Partition and application hash tables
 */
extern HASH_TBL_T              part_tbl[HASH_TBL_LENGTH];
extern int     conflict_debug,sched_debug,allocation_debug,
        	signal_debug,compare_debug,tile,pspart_debug;
extern alloc_config_t		alloc_config;


/* Forward declarations */
void
assign_rect_lp(BITMAP_T *bm, int size, int col, int row, 
		int cols, int rows,LP_MAP_T lp);
int
node_in_map(BITMAP_T *src_bitmap, LP_MAP_ENTRY_T node);

PART_T *
validate_allocator_access(ino_t parent_inode,ino_t inode,uid_t uid, 
			 gid_t gid,int req_access,int * error)
{
	PART_T	*part;
	PART_T	*parent;
	PART_T	*p_ptr;

#ifdef DEBUG
printf("validate allocator access\n");
#endif DEBUG

	 /*
         * Lookup partition.
         */
        part = HASH_LOOKUP_PART(inode);
	parent = (PART_T *)0;
         
        if (part == (PART_T *) 0) {
                *error = EPINVALPART;   /* Partition not found */
                return((PART_T *)0);
        }

#ifdef DEBUG
printf("found partition \n");
#endif DEBUG

	/* check the req_access flag to see if we really need to check
	 * permission against the parent 
	*/
	if ( (req_access & CHECK_AGAINST_PARENT) > 0){
#ifdef DEBUG
printf("confirm access against parent \n");
#endif DEBUG
	 	/*
         	* Lookup parent partition.
         	*/
        	parent = HASH_LOOKUP_PART(parent_inode);
        	if (parent == (PART_T *) 0) {
                	*error = EPINVALPART;   /* Partition not found */
                	return((PART_T *)0);
        	}
		p_ptr = parent;
#ifdef DEBUG
printf("found parent partition \n");
#endif DEBUG
	}
	else{
		p_ptr = part;
	}
 
        /*
         * Check permissions
         */
        if (!check_access(uid, gid, p_ptr->owner, p_ptr->group,
            (~CHECK_AGAINST_PARENT & req_access), p_ptr->protection))
        {
                *error = EPACCES;        /* Permission denied */
                return((PART_T *)0);
        }

	return(part);

}

/*
 * Select size nodes from src_bitmap, allocate them in dest_bitmap, and
 * set the logical to physical node map.
 *
 *	Parameters:
 *	int		strictly_rect	TRUE means only look for rectangles
 *					FALSE will cause select_nodes() to revert to finding any
 *					size nodes if no rectangle can be found
 *	int		default_szflg;	TRUE if we are looking for the default size 
 *	int		size;		Number of nodes to allocate 
 *	BITMAP_T	*src_bitmap;	Bitmap indicating which nodes are free
 *	BITMAP_T	*dest_bitmap;	Bitmap to set bits indicating nodes used 
 * 	LP_MAP_T	src_lp;		Logical to physical node map associated with
 *				   	bitmap from which we are allocating 
 *	int		src_lp_size;
 *	LP_MAP_T	dest_lp;	Logical to physical node map of bitmap
 *				   	we are allocating to
 *
 *	Returns:
 *		-1 if number of nodes found != size 
 *		otherwise
 *			number of nodes found
*/ 
int
select_nodes(int strictly_rect,int default_szflg,int size, BITMAP_T *src_bitmap, BITMAP_T *dest_bitmap, 
		LP_MAP_T src_lp, int src_lp_size,LP_MAP_T dest_lp,int *nrows,int *ncols)
{
	int		i;
	int		node;
	int		row,col,width,height;

	debug_allocation(5,"select_nodes:src_lp_size %d size %d\n",src_lp_size,size);
	if (unusable_node_map != (BITMAP_T *)0)
		bitmap_allocate_space(unusable_node_map,src_bitmap);
	if (bitmap_find_size(size,src_bitmap,&row,&col,&width,&height)){
		/* we found a rectangle */
		debug_allocation(5,"row %d col %d width %d height %d\n",
				row,col,width,height);
		assign_rect_lp(src_bitmap,size,row,col,width,height,dest_lp);
		for (i = 0; i < size; i++)
		    setbit_bitmap(dest_bitmap, 1, dest_lp[i]);

		if (allocation_debug) {
		    debug_allocation(5, "select_nodes found rect %d\n",size);
		    debug_print_bitmap(DEBUG_ALLOC, dest_bitmap);
		}
		*nrows = height;
		*ncols = width;
		return(size);
	}
	if (unusable_node_map != (BITMAP_T *)0)
		bitmap_free_space(unusable_node_map,src_bitmap);
	/* If we are here we did not find a rectangle. If we are only 
	 * interested in finding rectangles then return with error
	*/
	if (strictly_rect == TRUE)
		return(-1);

	for (i = 0, node = 0; (i < src_lp_size) && (node < size); i++) {
		/* a node is usable if it is part of our bitmap, part of 
		 * root's bitmap and not part of the bad nodes bitmap
		*/
		debug_allocation(5,"src_lp[%d] %d\n",i,src_lp[i]);
		if (node_in_map(src_bitmap,src_lp[i]) == 0){
			setbit_bitmap(dest_bitmap, 1, src_lp[i]);
			dest_lp[node++] = src_lp[i];
		}
	}

	debug_allocation(5,"select_nodes node %d\n",node);

	if ( (node < size ) && (!default_szflg))
		node = -1;

	debug_allocation(5,"select_nodes new size %d\n",node);

	*nrows = 1;
	*ncols = node;
	return(node);
}

int
node_in_map(BITMAP_T *src_bitmap, LP_MAP_ENTRY_T node)
{
	if (getbit_bitmap(src_bitmap, node)){
		/* if we have bad nodes then make sure the node we are
		* selecting is not in the unusable node list
		*/
		debug_allocation(5,"%d is in our bitmap\n",node);
		if ((unusable_node_map == (BITMAP_T *)0) ||
			((unusable_node_map != (BITMAP_T *)0) &&
			!getbit_bitmap(unusable_node_map,node) )){
			debug_allocation(5,"%d is not in unusable map\n",node);
			return(0);
		}
		else{
			debug_allocation(5,"%d is in unusable map\n",node);
			return(-2);
		}
				
	}
	else
		return(-1);
}

/*
 * assign_rect_lp() 
 *
 * Fill in the logical to physical map associated with a rectangle.
 *
 * Take note of missing bits in "bm".  These will be due to nodes
 * skipped due to "relaxed" allocation.
 */
void
assign_rect_lp(BITMAP_T *bm, int size,int col, int row, 
		int cols, int rows,LP_MAP_T lp)
{
	int i, j, k;

	debug_allocation(5,"assign_rect_lp size %d bm->cols %d col %d \
		row %d cols %d rows %d\n", size,bm->cols,col,row,cols,rows);
	k = 0;
	for (i = row; i < rows + row; i++)
		for (j = col; j < cols + col; j++){
			/*
			* Check if the bit is set in the bitmap. If it is
			* then set the next logical node to that phyisical
			* node.
			*/
			if (getbit_bitmap(bm, i*bm->cols + j)) {
			    lp[k++] = (i * bm->cols) + j;
			    if (k >= size)
				    goto assign_done;
			}
		}

assign_done:
	if (allocation_debug)
		for(i = 0; i < size; i++)
			debug_allocation(5,"lp[%d] %d\n",i,lp[i]);
}

BITMAP_T *
overlay_layers(LAYER_T *layer_list)
{
	LAYER_T		*layer;
	BITMAP_T	*ptr,*bitmap;

	if (layer_list == (LAYER_T *)0)
		return((BITMAP_T *)0);

	layer = layer_list;

	ptr = bitmap_clone(layer->bitmap);
	if (layer->next == (LAYER_T *)0)
		return(ptr);

	layer = layer->next;

	debug_allocation(5, "overlay_layers layer_list 0x%x\n");

	
	while (layer != (LAYER_T *)0){
		bitmap = AND_bitmaps(ptr,layer->bitmap);
		if (allocation_debug) {
		    debug_allocation(5,"layer 0x%x ptr\n",layer);
		    debug_print_bitmap(DEBUG_ALLOC, ptr);
		    debug_allocation(5,"layer->bitmap\n");
		    debug_print_bitmap(DEBUG_ALLOC,layer->bitmap);
		    debug_allocation(5,"bitmap\n");
		    debug_print_bitmap(DEBUG_ALLOC,bitmap);
		}
		free_bitmap(ptr);
		ptr = bitmap;
		layer = layer->next;
	}

	return(ptr);

}

int
too_many_layers(LAYER_T *layer_list)
{
	LAYER_T	*layer;
	int	count;

	debug_allocation(5,"too_many_layers: layer_list = %lx\n", layer_list);

	if (alloc_config.degree_of_overlap > 0){
		count = 0;
		debug_allocation(5,"too_many_layers: alloc_config.degree_of_overlap=%d\n", 
			alloc_config.degree_of_overlap);
		debug_allocation(5,"too_many_layers: looping thru layers\n");
		for (layer = layer_list; 
			layer != (LAYER_T *)0; layer = layer->next){
			debug_allocation(5,"too_many_layers: layer=%lx\n", layer);
			count++;
			if (count == alloc_config.degree_of_overlap){
				debug_allocation(5,"too many overlapping partitions\n");
				return(TRUE);
			}
		}
	}
	return(FALSE);
}
	
int
deny_overlap(PART_T *part, LAYER_T *layer_list)
{
	/* A request to overlap will be denied if
	 * 1. alloc_config.space_share == TRUE
	 * 2. The partition is a space shared partition
 	 * 3. We would exceed the max number of layers
	*/
	int	deny;

	deny = FALSE;
	
	if (layer_list != (LAYER_T *)0){
		if (alloc_config.space_share == TRUE){
			debug_allocation(5,"SPACE SHARE IS ON\n");
			deny = TRUE;
		}
		else
		if (part->sched == SPACE_SHARE){
			debug_allocation(5,"Partition is space shared\n");
			deny = TRUE;
		}
		else
		if (too_many_layers(layer_list)) {
			debug_allocation(5,"Too many layers\n");
			deny = TRUE;
		}
	}

	return(deny);
}

