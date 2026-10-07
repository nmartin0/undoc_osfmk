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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/conflict.c,v 1.12 1995/03/13 15:46:26 sdh Exp $
 *
*/
 
/* History:
 *      $Log: conflict.c,v $
 * Revision 1.12  1995/03/13  15:46:26  sdh
 * Changed the sizeof clauses of bzero and bcopy in insert_conflict().
 *
 *  Reviewer: carolr
 *  Risk: low
 *  Benefit or PTS #: 12532
 *  Testing:
 * 	EATS: sched, controlc, rmcall, rmcmd
 * 	manual tests
 *  Module(s):
 * 	cmds_libs/src/usr/sbin/allocator/conflict.c
 *
 * Revision 1.11  1994/11/19  03:04:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/06/13  16:43:47  sdh
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
 * Revision 1.9  1993/12/10  20:39:50  carbajal
 * Religiously use macros FREE and MALLOC to help debug vm leaks
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: Works towards solving 7257
 *  Testing: Bug test
 *  Module(s):
 *
 * Revision 1.8  1993/11/18  20:23:49  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get nx and mcmsg headers and libs out of the export tree
 *  Testing: built on Suns and 486
 *  Module(s): scripts.mk standard.mk
 *
 * Revision 1.7  1993/11/10  00:23:39  carbajal
 * Corrected insert_conflict_list() to be more robust in dealing with a
 * full list. This was causing problems in trying to support the 2 layers
 * proposed in the R1.2 user model.
 *
 *  Reviewer: NONE
 *  Risk: LOW
 *  Benefit or PTS #: Allows multiple tiles to coexist
 *  Testing: Developer scheduling tests, ran for 3 hours
 *  Module(s): conflict.c
 *
 * Revision 1.6  1993/10/27  01:58:01  carbajal
 * Free conflict map in detect_conflict()
 *
 * Revision 1.5  1993/07/15  22:20:40  carbajal
 * Cleanup conflict list when a partition has no more scheduling
 * layers.
 *
 * Revision 1.4  1993/07/13  22:15:07  carbajal
 * Clean up
 *
 * Revision 1.3  1993/07/07  22:02:52  carbajal
 * Added some more asserts to validate that the consumer type is APPL | PART
 *
 * Revision 1.2  1993/05/26  20:31:00  carbajal
 * Added more comments
 *
 * Revision 1.1  1993/05/25  23:53:56  carbajal
 * New module dealing with conflict list maintainence
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
#include <nx/writepart.h>
#include "debug.h"

extern int conflict_debug;

void
init_conflict_list(conflict_list_t *conflicts)
{
	int	i;
	for (i = 0; i < conflicts->list_size ; i++){
		conflicts->list[i] = (CONSUMER_T *)0;
	}
	conflicts->n_list = 0;
	conflicts->free_index = 0;
}

/* Return a pointer to the consumer's conflict list
 *	
 *	Parameters:
 *		object	pointer to consumer whose conflict list 
 *			we want a pointer to
 *
 *	Returns:
 *		pointer to conflict list
*/
conflict_list_t *
get_conflict_list(CONSUMER_T *object)
{				
	PART_T		*p;
	APPL_T		*appl;
	conflict_list_t	*list;

	if (object != (CONSUMER_T *)0){
		assert(object->type == PART || object->type == APPL);
		if (object->type == PART){
			p = (PART_T *)object;
			list = &(p->conflict);
		}
		else{
			appl = (APPL_T *)object;
			list = &(appl->conflict);
		}
	}
	else
		list = (conflict_list_t *)0;

	return(list);
}

/* Dump the contents of the conflict list
 *
 *	This is debug only
 *
*/
void
dump_conflict(CONSUMER_T *object)
{
	CONSUMER_T	*c;
	conflict_list_t	*conflicts;
	int		i;

	conflicts = get_conflict_list(object);

	if (conflict_debug && (object != (CONSUMER_T *)0)){
		printf("dump conflict list for ");
		if (object->type == PART)
			printf("inode %d\n",((PART_T *)object)->inode);
		else
			printf("pgroup %d\n",((APPL_T *)object)->pgroup);
		printf("conflict list stats: list_size %d n_list %d free_index %d\n",
		conflicts->list_size,conflicts->n_list,conflicts->free_index);
		for (i = 0; i < conflicts->list_size ; i++){
			c = conflicts->list[i];
			if (c != (CONSUMER_T *)0){
				if (c->type == PART)
					printf("conflict object inode %d\n",((PART_T *)c)->inode);
				else
					printf("conflict object pgroup %d\n",((APPL_T *)c)->pgroup); 
			}
		}
	}
}

/* Detect conflict between two objects by AND'ing the bitmaps.
 *
 *	Parameters:
 *		src1	pointer to 1st bitmap
 *		src2	pointer to 2nd bitmap
 *
 *	Returns:
 *		0	== no conflict detected
 *		1	== conflict detected
 *
*/
int
detect_conflict(BITMAP_T *src1,BITMAP_T *src2)
{
	BITMAP_T	*conflict_map;
	int		i,conflict;

	conflict = 0;

	conflict_map = AND_bitmaps(src1,src2);

	if (conflict_map != (BITMAP_T *)0){
		/* Check each column of the conflict bitmap, if it is not
		 * zero then we have a conflict
		*/
		for(i = 0; i < conflict_map->cols;i++)
			if (conflict_map->colmap[i] != 0){
				conflict = 1;
				break;
			}
		FREE(conflict_map);
	}

	return(conflict);
}

/*	Add a scheduling object to a conflict list 
 *
 * To insert an element in the conflict list, we need to insert the 
 * consumer pointed to by c in the consumer's conflict list pointed 
 * to by object. 
 *
 *	Parameters:
 *		object	our consumer object
 *		c	the consumer that object conflicts with
*/
void
insert_conflict(CONSUMER_T *object,CONSUMER_T *c)
{
	int		insert_done,new_index,i;
	conflict_list_t	*my_conflicts;
	CONSUMER_T	**list;


	debug_conflict(5,"insert_conflict 0x%x for object 0x%x\n",c,object);

	my_conflicts = get_conflict_list(object);
	assert(my_conflicts != (conflict_list_t *)0);

	if (my_conflicts->list == (CONSUMER_T **)0){
		/* There is no conflict list so build one */
		debug_conflict(5,"allocate conflict list \n");
		/* allocate space for list */
		my_conflicts->list = (CONSUMER_T **) MALLOC( sizeof(CONSUMER_T *) 
					* CONFLICT_ALLOC_QUANTUM);
		if (my_conflicts->list == (CONSUMER_T **) 0){
			/* XXX */
			assert(TRUE);
			return;
		}
		my_conflicts->list_size = CONFLICT_ALLOC_QUANTUM;
		init_conflict_list(my_conflicts);
	}
	else
	if (my_conflicts->list_size <= my_conflicts->n_list){
		debug_conflict(5,"need a bigger conflict list\n");
		/* Need a bigger list */
		list = (CONSUMER_T **) MALLOC( sizeof(CONSUMER_T *) *
			 (my_conflicts->list_size + CONFLICT_ALLOC_QUANTUM));
		if (list == (CONSUMER_T **) 0){
			/* XXX */
			assert(TRUE);
			return;
		}
		bzero(list, (my_conflicts->list_size + CONFLICT_ALLOC_QUANTUM) * sizeof(CONSUMER_T *)); 
		bcopy(my_conflicts->list, list, my_conflicts->n_list * sizeof(CONSUMER_T *));
		FREE(my_conflicts->list);
		my_conflicts->list = list;
		my_conflicts->list_size += CONFLICT_ALLOC_QUANTUM;
	}

	insert_done = FALSE;

	do{
		/* Make sure our free index is valid */
		if (my_conflicts->free_index < my_conflicts->list_size){
			/* we have a valid free index, go ahead and assign the value */
			my_conflicts->list[my_conflicts->free_index] = c;
			my_conflicts->n_list++;
			insert_done = TRUE;
		}

		/* Look for the next free index */
		new_index = -1;
		/* Now increment the free index */
		if (my_conflicts->free_index + 1 >= my_conflicts->list_size)
			i = 0;
		else
			i = my_conflicts->free_index + 1;
		do {
			if (my_conflicts->list[i] == (CONSUMER_T *)0){
				new_index = i;
				break;
			}
			if (i + 1 >= my_conflicts->list_size)
				i = 0;
			else
				i++;
		} while (i != my_conflicts->free_index);

		/* make sure we have some place to store this */
		if (insert_done == FALSE)
			/* we have not inserted yet */
			if (new_index == -1)
				/* we have no place to insert! */
				assert(TRUE);

	} while (insert_done != TRUE);
	

	if (new_index == -1)
		/* We did not find any free space! */
		my_conflicts->free_index = my_conflicts->list_size;
	else
		my_conflicts->free_index = new_index;

	dump_conflict(object);
}

/* Build a conflict list for a schedulable object 
 *
 *	Parameters:
 *		object		pointer to consumer whose conflict list we will build
 *		part		partition that the consumer resides in
 *
*/
void
build_conflict_list(CONSUMER_T *object,PART_T *part)
{
	LAYER_T		*l,*my_layer;
	CONSUMER_T	*c;
	BITMAP_T	*bitmap,*consumer_bitmap;
	PART_T		*p;
	APPL_T		*appl;

	assert( part != (PART_T *)0);
	assert( object != (CONSUMER_T *)0);
	assert(object->type == PART || object->type == APPL);
	
	/* Grab the object's bitmap */
	if (object->type == PART){
		p   = (PART_T *)object;
		bitmap = p->bitmap;
		my_layer = p->parent_sched_lyr;
		debug_conflict(5,"build_conflict_list: inode %d my_layer 0x%x\n",p->inode,my_layer); 
	}
	else{
		appl = (APPL_T *)object;
		bitmap = appl->bitmap;
		my_layer = appl->parent_lyr;
		debug_conflict(5, "build_conflict_list: pgroup %d 0x%x\n",appl->pgroup,my_layer); 
	}
		
	/* To build a conflict list we need to search through all scheduling tiles
	 * of the partition and call detect_conflict() for each object in the 
	 * tile. When a conflict is found the object is added to our conflict
	 * list, and we add the object to the conflicting object's conflict list
	*/

	for(l = part->child_sched_lyr; l != (LAYER_T *)0; l = l->next){
		/* skip over my layer */
		if (l == my_layer)
			continue;
		/* look through all the objects in the tile */
		for (c = l->consumer; c != (CONSUMER_T *)0; c = c->next){
			assert(c->type == PART || c->type == APPL);
			/* Grab the object's bitmap */
			if (c->type == PART){
				p = (PART_T *)c;
				consumer_bitmap = p->bitmap;
			}
			else{
				appl = (APPL_T *)c;
				/* Grab the object's bitmap */
				consumer_bitmap = appl->bitmap;
			}
			if (detect_conflict(bitmap,consumer_bitmap)){
				/* add the consumer to our conflict list */
				insert_conflict(object,c);
				insert_conflict(c,object);
			}
		}
	}
}

/* Remove a consumer from a conflict list
 *
 *	Parameters:
 *		object		pointer to consumer to remove
 *		c		pointer to consumer that contains the conflict list we 
 *				wish to remove object from
 *
*/
void
remove_from_conflict_list(CONSUMER_T *object,CONSUMER_T *c)
{
	int		i;
	conflict_list_t	*their;

	debug_conflict(5,"remove_from_conflict_list\n");

	dump_conflict(c);
	if ( (their = get_conflict_list(c)) != (conflict_list_t *)0){
		for (i = 0; i < their->list_size; i++){
			/* Search list for our object */
			if (their->list[i] == object){
				/* Found us, now make the entry null and set the free_index to
				 * point to us
				*/
				their->list[i] = (CONSUMER_T *)0;
				their->free_index = i;
				their->n_list--;
				break;
			}
		}
	}
	dump_conflict(c);

}
	
/* Remove the consumer pointed to by object from all conflict lists that it
 * appears in.
 *
 *	Parameters:
 *		object		pointer to consumer
 *
*/
void
remove_from_all_conflict_lists(CONSUMER_T *object)
{
	CONSUMER_T      *c;
	int             i;
	conflict_list_t *ours;
	
	debug_conflict(5, "remove_from_all_conflict_lists\n");
	if ( (ours = get_conflict_list(object)) != (conflict_list_t *)0){
		for (i = 0; i < ours->list_size; i++)
			if (ours->list[i] != (CONSUMER_T *)0){
				c = ours->list[i];
				remove_from_conflict_list(object,c);
			}
	}
}

